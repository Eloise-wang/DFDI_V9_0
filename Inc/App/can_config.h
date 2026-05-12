/*!
 * @file can_config.h
 *
 * @brief CAN配置头文件 - 高压控制器专用
 *
 * @version v9.0 (嵌入式控制架构)
 * @date 2025-01-27
 *
 * @note 本文件已更新为使用 gcu_hp_control_04.dbc 生成的消息定义
 * @note 所有CAN消息ID、结构和编解码函数定义在 gcu_control_dbc.h 中
 * @note 发送和接收逻辑在 can_message_handler.c 中实现
 *
 */

#ifndef __CAN_CONFIG_H
#define __CAN_CONFIG_H

/* ===========================================  Includes  =========================================== */
#include "common_types.h"  /* 包含公共类型和宏定义 */
#include "gcu_control_dbc.h"  /* 包含新的DBC头文件 */

/* 前向声明以避免与can_drv.h的冲突 */
struct can_msg_info_s;
typedef struct can_msg_info_s can_msg_info_t;

/* 包含CAN驱动类型 */
#include "can_drv.h"

/* ============================================  Define  ============================================ */

/* CAN实例配置 */
#define CAN_INSTANCE                0U              /* CAN0实例 */
#define CAN_INSTANCE_MAX_HPC        1U              /* 高压控制器最大CAN实例数 */

/* CAN引脚配置 - 根据硬件接口详细说明.md */
#define CAN_RX_PORT                 PORTE           /* CAN RX端口 */
#define CAN_RX_PIN                  4U              /* PE4 - CAN RX */
#define CAN_TX_PORT                 PORTE           /* CAN TX端口 */
#define CAN_TX_PIN                  5U              /* PE5 - CAN TX */
#define CAN_STB_PORT                PORTE           /* CAN STB端口 */
#define CAN_STB_PIN                 10U             /* PE10 - CAN STB */


/* 500Kbps配置（默认） */
#define CAN_BITRATE_500K_SEG1       34U             /* 位段1 */
#define CAN_BITRATE_500K_SEG2       11U             /* 位段2 */
#define CAN_BITRATE_500K_SJW        11U             /* 同步跳转宽度 */
#define CAN_BITRATE_500K_PRESC      4U              /* 预分频器 */

/* ==================== CAN消息ID定义（来自DBC）==================== */
/* 注意：消息ID已从gcu_control_dbc.h中定义，这里仅作注释说明 */
/* 
 * CAN_MSG_DEVICE_STATUS_ID     (0x980FF16C) - device_status消息，GCU->TSMaster，10ms周期
 * CAN_MSG_TSMASTER_CONTROL_ID  (0x98080100) - tsmaster_control消息，TSMaster->GCU，100ms周期
 * CAN_MSG_TSMASTER_CONTROL2_ID (0x98080108) - tsmaster_control2消息，TSMaster->GCU，100ms周期
 * 
 * 所有消息ID定义请参考 gcu_control_dbc.h
 */
/* ==================== CAN消息使用说明 ==================== */
/* 
 * v9.0架构下的CAN消息使用方式：
 * 
 * 1. 发送消息（在can_message_handler.c中实现）：
 *    - 使用 device_status_t 结构体填充数据
 *    - 使用 device_status_pack() 打包
 *    - 使用 CAN_Config_SendMessage(CAN_MSG_DEVICE_STATUS_ID, ...) 发送
 * 
 * 2. 接收消息（在can_message_handler.c中实现）：
 *    - 使用 CAN_Config_RegisterRxCallback() 注册回调
 *    - 在回调中使用 tsmaster_control_unpack() 或 tsmaster_control2_unpack() 解包
 *    - 使用对应的decode函数解码信号值
 * 
 * 3. 所有消息定义和编解码函数请参考 gcu_control_dbc.h 和 gcu_control_dbc.c
 */
/* ===========================================  Typedef  ============================================ */

/*!
 * @brief CAN消息处理器回调类型
 */
typedef void (*can_msg_handler_t)(const can_msg_info_t *msg);

/*!
 * @brief CAN接收回调类型 - 用于接收PC端参数配置
 * @param msg_id CAN消息ID
 * @param data 数据指针
 * @param length 数据长度
 */
typedef void (*can_rx_callback_t)(uint32_t msg_id, const uint8_t* data, uint8_t length);

/*!
 * @brief CAN节点统计信息
 */
typedef struct {
    uint32_t recvCount;                              /* 接收消息计数 */
    uint32_t sendCount;                              /* 发送消息计数 */
} can_cur_node_t;

/*!
 * @brief 高压控制器状态数据结构（兼容旧代码，建议使用device_status_t）
 * @deprecated 此结构体已废弃，请使用 gcu_control_dbc.h 中的 device_status_t
 * 保留此结构体仅用于兼容 CAN_Config_UpdateHPCStatus/GetHPCStatus 函数
 */
typedef struct {
    float bypass_ratio;             /* 旁通阀开度 (0-50%) */
    bool reversal_valve_st;         /* 换向阀开关状态 */
    float gas_pressure;             /* LNG压力 (0-40 MPa) */
    uint8_t reversal_valve_per_min; /* 换向阀频率 (0-1000 次/分钟) */
    float gas_temperature;          /* LNG温度 (-40~90°C) */
    float oil_pressure;             /* 油压 (0-40 MPa) */
    float oil_temperature;          /* 油温 (-40~90°C) */
    uint8_t reserve_debug1;         /* 保留字段 */
} hpc_status_data_t;

/*!
 * @brief CAN配置结构体
 */
typedef struct {
    bool initialized;                   /*!< CAN初始化状态 */
    can_msg_handler_t rxHandler;        /*!< 接收消息处理器 */
    can_msg_handler_t txHandler;        /*!< 发送完成处理器 */
    uint32_t errorCount;                /*!< 错误计数器 */
    uint32_t rxCount;                   /*!< 接收消息计数器 */
    uint32_t txCount;                   /*!< 发送消息计数器 */
    hpc_status_data_t statusData;       /*!< 高压控制器状态数据（兼容旧代码） */
    uint8_t currentBitrateIndex;        /*!< 当前波特率索引 */
} can_app_config_t;

/* ====================================  Functions declaration  ===================================== */

/*!
 * @brief 使用500Kbps配置初始化CAN模块
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_Init(void);

/*!
 * @brief 反初始化CAN模块
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_Deinit(void);

/*!
 * @brief 发送CAN消息
 * @param[in] id: CAN标识符
 * @param[in] data: 消息数据
 * @param[in] length: 数据长度 (0-8字节)
 * @param[in] isExtended: true为扩展ID, false为标准ID
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_SendMessage(uint32_t id, const uint8_t *data, uint8_t length, bool isExtended);

/*!
 * @brief 接收CAN消息（非阻塞）
 * @param[out] msg: 接收到的消息信息
 * @retval true: 接收到消息, false: 无消息
 */
bool CAN_Config_ReceiveMessage(const can_msg_info_t *msg);

/*!
 * @brief 注册CAN接收回调函数 - 用于接收PC端参数配置
 * @param[in] callback: 回调函数指针
 */
void CAN_Config_RegisterRxCallback(can_rx_callback_t callback);

/*!
 * @brief 设置CAN待机模式
 * @param[in] enable: true启用待机, false禁用待机
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_SetStandby(bool enable);

/*!
 * @brief 安装接收消息处理器
 * @param[in] handler: 消息处理器回调
 */
void CAN_Config_InstallRxHandler(can_msg_handler_t handler);

/*!
 * @brief 安装发送完成处理器
 * @param[in] handler: 发送完成回调
 */
void CAN_Config_InstallTxHandler(can_msg_handler_t handler);

/*!
 * @brief 获取CAN统计信息
 * @param[out] rxCount: 接收消息计数
 * @param[out] txCount: 发送消息计数
 * @param[out] errorCount: 错误计数
 */
void CAN_Config_GetStats(uint32_t *rxCount, uint32_t *txCount, uint32_t *errorCount);

/*!
 * @brief CAN周期性任务（应在主循环中调用）
 */
void CAN_Config_Task(void);

/*!
 * @brief 检查CAN收发器状态和STB引脚状态
 * @param[out] stbPinState: STB引脚状态 (true=高电平/待机, false=低电平/正常)
 * @param[out] canControllerStandby: CAN控制器待机状态
 * @param[out] canBusIdle: CAN总线空闲状态
 * @retval true: 检查成功, false: CAN未初始化
 */
bool CAN_Config_CheckTransceiverStatus(bool *stbPinState, bool *canControllerStandby, bool *canBusIdle);

/*!
 * @brief 重置CAN控制器（用于错误恢复）
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_ResetController(void);

/* ====================================  高压控制器专用函数  ===================================== */

/*!
 * @brief 更新高压控制器状态数据（兼容旧代码）
 * @deprecated 建议直接使用 gcu_control_dbc.h 中的结构体和函数
 * @param[in] statusData: 新的状态数据
 */
void CAN_Config_UpdateHPCStatus(const hpc_status_data_t *statusData);

/*!
 * @brief 获取当前高压控制器状态数据（兼容旧代码）
 * @deprecated 建议直接使用 gcu_control_dbc.h 中的结构体和函数
 * @param[out] statusData: 当前状态数据
 */
void CAN_Config_GetHPCStatus(hpc_status_data_t *statusData);

/*!
 * @brief 发送初始CAN状态消息
 * @deprecated v9.0架构下，传感器数据发送在can_message_handler.c中实现
 * 在系统初始化完成后立即调用，不等待任务调度
 */
void CAN_Config_SendInitialStatus(void);

/* ====================================  高级CAN功能  ===================================== */

/*!
 * @brief 获取指定波特率的配置参数
 * @param[in] index: 波特率索引 (0-6)
 * @return 波特率配置结构体
 */
can_time_segment_t CAN_Config_GetBitrate(uint8_t index);

/*!
 * @brief 动态切换CAN波特率
 * @param[in] bitrateIndex: 新的波特率索引
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_ChangeBitrate(uint8_t bitrateIndex);

/*!
 * @brief 获取当前波特率索引
 * @return 当前波特率索引
 */
uint8_t CAN_Config_GetCurrentBitrateIndex(void);

/*!
 * @brief 配置CAN过滤器
 * @param[in] filterIndex: 过滤器索引
 * @param[in] code: 过滤器代码
 * @param[in] mask: 过滤器掩码
 * @param[in] idType: ID类型
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_SetFilter(uint8_t filterIndex, uint32_t code, uint32_t mask, uint8_t idType);

/*!
 * @brief 测试CAN发送功能
 * @param[in] instance: CAN实例
 * @param[in] type: 发送缓冲区类型
 * @retval 0: 发送成功, 1: 发送失败
 */
int32_t CAN_Config_SendTest(uint8_t instance, uint8_t type);

/*!
 * @brief 获取CAN节点统计信息
 * @param[in] instance: CAN实例
 * @param[out] nodeInfo: 节点统计信息
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_GetNodeStats(uint8_t instance, can_cur_node_t *nodeInfo);

/*!
 * @brief 重置CAN统计信息
 * @param[in] instance: CAN实例
 */
void CAN_Config_ResetStats(uint8_t instance);

/*!
 * @brief 设置CAN时间戳功能
 * @param[in] enable: 是否启用时间戳
 * @param[in] clockSrc: 时间戳时钟源
 * @param[in] clockDiv: 时间戳时钟分频
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_SetTimeStamp(bool enable, uint8_t clockSrc, uint8_t clockDiv);

/*!
 * @brief 获取CAN错误状态
 * @param[out] errorWarningLimit: 错误警告限制
 * @param[out] rxErrorCount: 接收错误计数
 * @param[out] txErrorCount: 发送错误计数
 * @retval true: 成功, false: 失败
 */
bool CAN_Config_GetErrorStatus(uint8_t *errorWarningLimit, uint8_t *rxErrorCount, uint8_t *txErrorCount);


#endif /* __CAN_CONFIG_H */

/* =============================================  EOF  ============================================== */
