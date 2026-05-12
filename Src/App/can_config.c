/*!
 * @file can_config.c
 *
 * @brief CAN配置实现文件
 *
 */

/* ===========================================  Includes  =========================================== */
#include "can_config.h"
#include "common_types.h"  /* 包含公共宏定义 */
#include "ac7840x.h"
#include "device_register.h"
#include "device_status.h"
#include "can_drv.h"
#include "can_hw.h"
#include "gpio_drv.h"
#include "ckgen_drv.h"
#include "gcu_control_dbc.h"  /* 包含新DBC头文件 */
#include "sensor.h"  /* 包含传感器模块以获取数据 */
#include "valve_control.h"  /* 包含阀门控制模块 */
#include "debug_log.h"
#include <string.h>

/* ============================================  Define  ============================================ */
/* 使用common_types.h中的宏定义，无需重复定义 */

/* Multiple Bitrate Support */
#if (CAN_SOURCE_CLOCK == 60UL) /*时钟源为60M*/
static const can_time_segment_t s_canBitrate[CAN_BITRATE_NUM_HPC] =
{
    {0x0D, 0x04, 0x04, 0x02},  /*   1M, 75% */
    {0x12, 0x04, 0x04, 0x02},  /* 800K, 80% */
    {0x1C, 0x09, 0x09, 0x02},  /* 500K, 75% */
    {0x1C, 0x09, 0x09, 0x05},  /* 250K, 75% */
    {0x1C, 0x09, 0x09, 0x0B},  /* 125K, 75% */
    {0x1C, 0x09, 0x09, 0x0E},  /* 100K, 75% */
    {0x1C, 0x09, 0x09, 0x1D},  /*  50K, 75% */
};
#endif

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
static can_app_config_t s_canAppConfig = {0}; 
static uint8_t s_canRxBuffer[CAN_MSG_DATA_MAX_SIZE];
static uint8_t s_canTxBuffer[CAN_MSG_DATA_MAX_SIZE];
static can_user_config_t s_canUserConfig;
static can_cur_node_t s_canCurNode[CAN_INSTANCE_MAX_HPC] = {0}; /* 节点信息 */

/* CAN接收回调 - 用于接收PC端参数配置 */
static can_rx_callback_t g_rx_callback = NULL;

/* ====================================  Functions declaration  ===================================== */
static void CAN_ConfigGpio(void);
static void CAN_EventCallback(uint8_t instance, uint32_t event, uint32_t koer);
static void CAN_ConfigFilters(void);

/* ======================================  Functions define  ======================================== */

/*!
 * @brief 配置CAN GPIO引脚
 * @retval None
 */
static void CAN_ConfigGpio(void)
{
    /* Configure PE4 as CAN RX */
    GPIO_DRV_SetMuxModeSel(CAN_RX_PORT, CAN_RX_PIN, PORT_MUX_ALT5);  /* CAN0_RX function */
    
    /* Configure PE5 as CAN TX */
    GPIO_DRV_SetMuxModeSel(CAN_TX_PORT, CAN_TX_PIN, PORT_MUX_ALT5);  /* CAN0_TX function */
    
    /* Configure PE10 as CAN alternate function (备用通道) */
    GPIO_DRV_SetMuxModeSel(CAN_STB_PORT, CAN_STB_PIN, PORT_MUX_ALT5);  /* 备用通道 */
}

/*!
 * @brief 配置CAN过滤器用于消息接收
 * @retval None
 */
static void CAN_ConfigFilters(void)
{
    /* No filters configured - accept all messages */
    /* This matches BLDC project configuration */
}

/*!
 * @brief CAN事件回调函数
 * @param[in] instance: CAN实例
 * @param[in] event: CAN事件类型
 * @param[in] koer: 错误类型
 * @retval None
 */
static void CAN_EventCallback(uint8_t instance, uint32_t event, uint32_t koer)
{
    (void)instance;
    
    /* Handle receive event */
    if (event & CAN_EVENT_RECEIVE_DONE)
    {
        can_msg_info_t msg;
        msg.DATA = s_canRxBuffer;
        
        if (STATUS_SUCCESS == CAN_DRV_Receive(CAN_INSTANCE, &msg))
        {
            s_canAppConfig.rxCount++;
            /* Call receive handler if installed */
            if (s_canAppConfig.rxHandler != NULL)
            {
                s_canAppConfig.rxHandler(&msg);
            }
            
            /* 调用注册的回调函数 - 接收PC端参数配置 */
            if (g_rx_callback != NULL && msg.DATA != NULL)
            {
                g_rx_callback(msg.ID, msg.DATA, msg.DLC);
            }
        }
    }
    
    /* Handle transmit complete event */
    if (event & (CAN_EVENT_TRANS_PRI_DONE | CAN_EVENT_TRANS_SEC_DONE))
    {
        s_canAppConfig.txCount++;

        /* Call transmit handler if installed */
        if (s_canAppConfig.txHandler != NULL)
        {
            can_msg_info_t msg = {0};  /* Empty message for transmit complete indication */
            s_canAppConfig.txHandler(&msg);
        }
    }
    
    /* Handle error events */
    bool has_error = (event & (CAN_EVENT_ERROR | CAN_EVENT_BUS_ERROR | CAN_EVENT_ARBIT_LOST | CAN_EVENT_ERROR_PASSIVE)) != 0;
    
    if (has_error)
    {
        s_canAppConfig.errorCount++;
    }
    
    /* 打印错误信息（带状态管理） */
    Debug_Log_CANEventError(s_canAppConfig.errorCount, event, has_error);
}

/*!
 * @brief 使用500Kbps配置初始化CAN模块
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_Init(void)
{
    if (s_canAppConfig.initialized)
    {
        return true;  /* Already initialized */
    }
    
    /* Configure GPIO pins */
    CAN_ConfigGpio();
    
    /* Configure CAN filters */
    CAN_ConfigFilters();
    
    /* Get default CAN configuration */
    CAN_DRV_GetDefaultConfig(&s_canUserConfig);
    
    /* Configure CAN for 500Kbps (index 2 in s_canBitrate array) */
    s_canUserConfig.bitrate.SEG_1 = s_canBitrate[2].SEG_1;
    s_canUserConfig.bitrate.SEG_2 = s_canBitrate[2].SEG_2;
    s_canUserConfig.bitrate.SJW = s_canBitrate[2].SJW;
    s_canUserConfig.bitrate.PRESC = s_canBitrate[2].PRESC;
    
    /* Configure CAN mode and features */
    s_canUserConfig.canMode = CAN_NORMAL_MODE;
    s_canUserConfig.interruptEn = true;
    s_canUserConfig.filterNum = 0U;  /* 不过滤，接收所有消息 */
    s_canUserConfig.filterList = NULL;
    s_canUserConfig.callback = CAN_EventCallback;
    s_canUserConfig.fdModeEn = false;  /* CAN 2.0A/B mode */
    s_canUserConfig.tsMode = CAN_TSMODE_FIFO;
    s_canUserConfig.tsAmount = CAN_TRANSMIT_SEC_ALL;
    s_canUserConfig.tpss = false;      /* 使能PTB */
    s_canUserConfig.tsss = false;      /* 使能STB */
    s_canUserConfig.selfAckEn = false;
    s_canUserConfig.rom = CAN_ROM_OVER_WRITE;  /* 覆盖写入 */
    s_canUserConfig.errorWarningLimit = 0x0BU; /* 错误警告限制 */
    s_canUserConfig.busOffRecDisable = false;  /* 不使用自动重发 */
    /* No explicit interrupt mask - using driver default */
    
    /* Initialize CAN driver */
    status_t status = CAN_DRV_Init(CAN_INSTANCE, &s_canUserConfig);
    
    if (STATUS_SUCCESS == status)
    {
        s_canAppConfig.initialized = true;
        s_canAppConfig.rxCount = 0;
        s_canAppConfig.txCount = 0;
        s_canAppConfig.errorCount = 0;
        s_canAppConfig.currentBitrateIndex = 2U; /* 默认使用500Kbps (index 2) */
        
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * @brief 反初始化CAN模块
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_Deinit(void)
{
    if (!s_canAppConfig.initialized)
    {
        return true;  /* Already deinitialized */
    }
    
    /* No STB control needed - using ALT5 mode */
    
    /* Deinitialize CAN driver */
    status_t status = CAN_DRV_Deinit(CAN_INSTANCE);
    
    if (STATUS_SUCCESS == status)
    {
        s_canAppConfig.initialized = false;
        s_canAppConfig.rxHandler = NULL;
        s_canAppConfig.txHandler = NULL;
        
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * @brief 发送CAN消息
 * @param[in] id: CAN标识符
 * @param[in] data: 消息数据
 * @param[in] length: 数据长度 (0-8字节)
 * @param[in] isExtended: true为扩展ID, false为标准ID
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_SendMessage(uint32_t id, const uint8_t *data, uint8_t length, bool isExtended)
{
    if (!s_canAppConfig.initialized)
    {
        return false;
    }
    
    if (length > CAN_MSG_DATA_MAX_SIZE)
    {
        return false;
    }
    
    /* 检查CAN控制器状态并尝试恢复 */
    CAN_Type *base = CAN_DRV_GetBase(CAN_INSTANCE);
    uint32_t ctrl0 = base->CTRL0;
    
    /* 如果CAN控制器处于错误状态，尝试重置 */
    if (ctrl0 & (1U << 7)) {  /* BOFF bit */
        Debug_Log_CANBusOffRecovery();
        CAN_DRV_Deinit(CAN_INSTANCE);
        for (volatile int i = 0; i < 10000; i++);  /* 简单延时 */
        CAN_DRV_Init(CAN_INSTANCE, &s_canUserConfig);
        for (volatile int i = 0; i < 10000; i++);  /* 简单延时 */
    }
    
    /* 检查发送缓冲区状态 - 简化检查 */
    if (base->CTRL0 & (1U << 8)) {  /* TX buffer full bit */
        Debug_Log_CANTransmitBufferFull();
        return false;
    }
    
    /* Prepare message structure */
    can_msg_info_t msg = {0};
    msg.ID = id;
    msg.DLC = length;
    msg.RTR = CAN_MSG_DATA_FRAME;
    msg.IDE = isExtended ? 1 : 0;
    msg.FDF = 0;  /* CAN 2.0 format */
    msg.BRS = 0;  /* No bit rate switch */
    msg.ESI = 0;  /* No error state indicator */
    
    /* Copy data to buffer */
    if (length > 0 && data != NULL)
    {
        memcpy(s_canTxBuffer, data, length);
        msg.DATA = s_canTxBuffer;
    }
    else
    {
        msg.DATA = s_canTxBuffer;
    }
    
    /* Send message using primary buffer */
    status_t status = CAN_DRV_Send(CAN_INSTANCE, &msg, CAN_TRANSMIT_PRIMARY);
    
    /* 发送错误统计和打印（带状态管理） */
    static uint32_t send_count = 0;
    static uint32_t send_error_count = 0;
    
    bool is_success = (status == STATUS_SUCCESS);
    if (!is_success) {
        send_error_count++;
    }
    
    /* 打印发送错误状态（带状态管理） */
    Debug_Log_CANSendErrorStatus(status, send_error_count, is_success);
    
    /* 打印发送统计信息（带状态管理，每100次打印一次） */
    Debug_Log_CANSendStatistics(&send_count, send_error_count);
    
    return is_success;
}

/*!
 * @brief 接收CAN消息（非阻塞）
 * @param[out] msg: 接收到的消息信息
 * @retval true: 接收到消息, false: 无消息
 */
bool CAN_Config_ReceiveMessage(const can_msg_info_t *msg)
{
    if (!s_canAppConfig.initialized || msg == NULL)
    {
        return false;
    }
    
    /* Check if receive buffer has data */
    can_rbuf_status_t rbufStatus = CAN_DRV_GetRbufStatus(CAN_INSTANCE);
    if (rbufStatus == CAN_RSTAT_EMPTY)
    {
        return false;  /* No message available */
    }
    
    /* Create a local message structure for modification */
    can_msg_info_t localMsg = *msg;
    
    /* Set buffer for received data */
    localMsg.DATA = s_canRxBuffer;
    
    /* Receive message */
    status_t status = CAN_DRV_Receive(CAN_INSTANCE, &localMsg);
    
    if (STATUS_SUCCESS == status)
    {
        s_canAppConfig.rxCount++;
        return true;
    }
    
    return false;
}

/*!
 * @brief 注册CAN接收回调函数 - 用于接收PC端参数配置
 * @param[in] callback: 回调函数指针
 */
void CAN_Config_RegisterRxCallback(can_rx_callback_t callback)
{
    g_rx_callback = callback;
}

/*!
 * @brief 设置CAN待机模式
 * @param[in] enable: true启用待机, false禁用待机
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_SetStandby(bool enable)
{
    if (!s_canAppConfig.initialized)
    {
        return false;
    }
    
    /* Control STB pin */
    if (enable)
    {
        GPIO_DRV_SetPins(CAN_STB_GPIO, (1U << CAN_STB_PIN));  /* Set STB high - standby mode */
    }
    else
    {
        GPIO_DRV_ClearPins(CAN_STB_GPIO, (1U << CAN_STB_PIN));  /* Set STB low - normal mode */
    }
    
    /* Use driver standby function */
    status_t status = CAN_DRV_SetStandby(CAN_INSTANCE, enable);
    
    return (STATUS_SUCCESS == status);
}

/*!
 * @brief 安装接收消息处理器
 * @param[in] handler: 消息处理器回调
 */
void CAN_Config_InstallRxHandler(can_msg_handler_t handler)
{
    s_canAppConfig.rxHandler = handler;
}

/*!
 * @brief 安装发送完成处理器
 * @param[in] handler: 发送完成回调
 */
void CAN_Config_InstallTxHandler(can_msg_handler_t handler)
{
    s_canAppConfig.txHandler = handler;
}

/*!
 * @brief 获取CAN统计信息
 * @param[out] rxCount: 接收消息计数
 * @param[out] txCount: 发送消息计数
 * @param[out] errorCount: 错误计数
 */
void CAN_Config_GetStats(uint32_t *rxCount, uint32_t *txCount, uint32_t *errorCount)
{
    if (rxCount != NULL)
    {
        *rxCount = s_canAppConfig.rxCount;
    }
    
    if (txCount != NULL)
    {
        *txCount = s_canAppConfig.txCount;
    }
    
    if (errorCount != NULL)
    {
        *errorCount = s_canAppConfig.errorCount;
    }
}

/*!
 * @brief CAN周期性任务（应在主循环中调用）
 */
void CAN_Config_Task(void)
{
    if (!s_canAppConfig.initialized)
    {
        return;
    }
    
    /* Process received messages */
    can_msg_info_t rxMsg;
    while (CAN_Config_ReceiveMessage(&rxMsg))
    {
        if (s_canAppConfig.rxHandler != NULL)
        {
            s_canAppConfig.rxHandler(&rxMsg);
        }
        
        /* 调用注册的回调函数 - 接收PC端参数配置 */
        if (g_rx_callback != NULL && rxMsg.DATA != NULL)
        {
            g_rx_callback(rxMsg.ID, rxMsg.DATA, rxMsg.DLC);
        }
    }
    
    /* CAN状态发送由应用层负责（can_message_handler.c模块实现），此处不再主动发送 */
}

/*!
 * @brief 检查CAN收发器状态和STB引脚状态
 * @param[out] stbPinState: STB引脚状态 (true=高电平/待机, false=低电平/正常)
 * @param[out] canControllerStandby: CAN控制器待机状态
 * @param[out] canBusIdle: CAN总线空闲状态
 * @retval true: 检查成功, false: CAN未初始化
 */
bool CAN_Config_CheckTransceiverStatus(bool *stbPinState, bool *canControllerStandby, bool *canBusIdle)
{
    if (!s_canAppConfig.initialized)
    {
        return false;
    }
    
    if (stbPinState == NULL || canControllerStandby == NULL || canBusIdle == NULL)
    {
        return false;
    }
    
    /* Read STB pin state - PE10 */
    uint32_t pinState = GPIO_DRV_ReadPins(CAN_STB_GPIO);
    *stbPinState = (pinState & (1U << CAN_STB_PIN)) != 0;
    
    /* Read CAN controller standby state from CTRL0 register */
    CAN_Type *base = CAN_DRV_GetBase(CAN_INSTANCE);
    *canControllerStandby = CAN_GetStandbyState(base);
    
    /* Check CAN bus idle state */
    *canBusIdle = ((base->CTRL0 & CAN_CTRL0_IDLE_Msk) >> CAN_CTRL0_IDLE_Pos) != 0;
    
    return true;
}

/* ====================================  High Pressure Controller Specific Functions  ===================================== */

/*!
 * @brief 更新高压控制器状态数据
 * @param[in] statusData: 新的状态数据
 */
void CAN_Config_UpdateHPCStatus(const hpc_status_data_t *statusData)
{
    if (statusData != NULL)
    {
        s_canAppConfig.statusData = *statusData;
    }
}

/*!
 * @brief 获取当前高压控制器状态数据
 * @param[out] statusData: 当前状态数据
 */
void CAN_Config_GetHPCStatus(hpc_status_data_t *statusData)
{
    if (statusData != NULL)
    {
        *statusData = s_canAppConfig.statusData;
    }
}

/* ====================================  Advanced CAN Functions  ===================================== */

/*!
 * @brief 获取指定波特率的配置参数
 * @param[in] index: 波特率索引 (0-6)
 * @return 波特率配置结构体
 */
can_time_segment_t CAN_Config_GetBitrate(uint8_t index)
{
    can_time_segment_t bitrate = {0};
    
    if (index < CAN_BITRATE_NUM_HPC)
    {
        bitrate = s_canBitrate[index];
    }
    
    return bitrate;
}

/*!
 * @brief 动态切换CAN波特率
 * @param[in] bitrateIndex: 新的波特率索引
 * @retval true: success, false: failure
 */
bool CAN_Config_ChangeBitrate(uint8_t bitrateIndex)
{
    if (!s_canAppConfig.initialized || bitrateIndex >= CAN_BITRATE_NUM_HPC)
    {
        return false;
    }
    
    /* 获取新的波特率配置 */
    can_time_segment_t newBitrate = CAN_Config_GetBitrate(bitrateIndex);
    
    /* 更新CAN配置 */
    s_canUserConfig.bitrate = newBitrate;
    
    /* 重新初始化CAN驱动 */
    status_t status = CAN_DRV_Init(CAN_INSTANCE, &s_canUserConfig);
    
    if (STATUS_SUCCESS == status)
    {
        s_canAppConfig.currentBitrateIndex = bitrateIndex;
        return true;
    }
    
    return false;
}

/*!
 * @brief 获取当前波特率索引
 * @return 当前波特率索引
 */
uint8_t CAN_Config_GetCurrentBitrateIndex(void)
{
    return s_canAppConfig.currentBitrateIndex;
}

/*!
 * @brief 配置CAN过滤器
 * @param[in] filterIndex: 过滤器索引
 * @param[in] code: 过滤器代码
 * @param[in] mask: 过滤器掩码
 * @param[in] idType: ID类型
 * @retval true: success, false: failure
 */
bool CAN_Config_SetFilter(uint8_t filterIndex, uint32_t code, uint32_t mask, uint8_t idType)
{
    (void)filterIndex;
    (void)code;
    (void)mask;
    (void)idType;
    /* 当前项目无需动态过滤器，保留占位以兼容接口 */
    return false;
}

/*!
 * @brief 获取CAN节点统计信息
 * @param[in] instance: CAN实例
 * @param[out] nodeInfo: 节点统计信息
 * @retval true: success, false: failure
 */
bool CAN_Config_GetNodeStats(uint8_t instance, can_cur_node_t *nodeInfo)
{
    if (instance >= CAN_INSTANCE_MAX_HPC || nodeInfo == NULL)
    {
        return false;
    }
    
    *nodeInfo = s_canCurNode[instance];
    return true;
}

/*!
 * @brief 重置CAN统计信息
 * @param[in] instance: CAN实例
 */
void CAN_Config_ResetStats(uint8_t instance)
{
    if (instance < CAN_INSTANCE_MAX_HPC)
    {
        s_canCurNode[instance].recvCount = 0;
        s_canCurNode[instance].sendCount = 0;
    }
    
    s_canAppConfig.rxCount = 0;
    s_canAppConfig.txCount = 0;
    s_canAppConfig.errorCount = 0;
}

/*!
 * @brief 设置CAN时间戳功能
 * @param[in] enable: 是否启用时间戳
 * @param[in] clockSrc: 时间戳时钟源
 * @param[in] clockDiv: 时间戳时钟分频
 * @retval true: success, false: failure
 */
bool CAN_Config_SetTimeStamp(bool enable, uint8_t clockSrc, uint8_t clockDiv)
{
    if (!s_canAppConfig.initialized)
    {
        return false;
    }
    
    /* 更新时间戳配置 */
    s_canUserConfig.timeStampEn = enable;
    if (enable)
    {
        s_canUserConfig.timeStampClkSrc = (can_time_stamp_clk_src_t)clockSrc;
        s_canUserConfig.timeStampClkDiv = (can_time_stamp_clk_div_t)clockDiv;
        s_canUserConfig.timeStampPos = CAN_TIME_STAMP_SOF;
    }
    
    /* 重新初始化CAN驱动 */
    status_t status = CAN_DRV_Init(CAN_INSTANCE, &s_canUserConfig);
    
    return (STATUS_SUCCESS == status);
}

/*!
 * @brief 重置CAN控制器（用于错误恢复）
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_ResetController(void)
{
    if (!s_canAppConfig.initialized)
    {
        return false;
    }
    
    Debug_Log_CANResetStart();
    
    /* 反初始化CAN驱动 */
    CAN_DRV_Deinit(CAN_INSTANCE);
    
    /* 等待一段时间 */
    for (volatile int i = 0; i < 100000; i++);
    
    /* 重新初始化CAN驱动 */
    status_t status = CAN_DRV_Init(CAN_INSTANCE, &s_canUserConfig);
    
    if (STATUS_SUCCESS == status)
    {
        Debug_Log_CANResetSuccess();
        return true;
    }
    else
    {
        Debug_Log_CANResetFailed(status);
        return false;
    }
}

/*!
 * @brief 获取CAN错误状态
 * @param[out] errorWarningLimit: 错误警告限制
 * @param[out] rxErrorCount: 接收错误计数
 * @param[out] txErrorCount: 发送错误计数
 * @retval true: success, false: failure
 */
bool CAN_Config_GetErrorStatus(uint8_t *errorWarningLimit, uint8_t *rxErrorCount, uint8_t *txErrorCount)
{
    if (!s_canAppConfig.initialized)
    {
        return false;
    }
    
    if (errorWarningLimit != NULL)
    {
        *errorWarningLimit = s_canUserConfig.errorWarningLimit;
    }
    
    if (rxErrorCount != NULL)
    {
        /* 使用应用层统计信息 */
        *rxErrorCount = (uint8_t)(s_canAppConfig.errorCount & 0xFF);
    }
    
    if (txErrorCount != NULL)
    {
        /* 使用应用层统计信息 */
        *txErrorCount = (uint8_t)(s_canAppConfig.errorCount >> 8);
    }
    
    return true;
}

/* =============================================  EOF  ============================================== */
 
