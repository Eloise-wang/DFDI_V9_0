/*!
 * @file system_init.c
 *
 * @brief 系统初始化模块实现
 *
 * 功能说明：
 * - 硬件初始化（时钟、GPIO、PWM、UART等）
 * - 系统初始化流程管理
 * - 系统使能状态管理（由PC端system_enable CAN信号控制）
 *
 * @note PC17启动开关功能已暂时禁用，相关代码已注释保留供后期使用
 * @note 当前系统启动完全由PC端的system_enable CAN信号控制
 */

#include "system_init.h"
#include "clock_config.h"
#include "gpio_drv.h"
#include "sensor.h"
#include "valve_control.h"
#include "fault_diagnosis.h"
#include "can_config.h"
#include "optimized_task_scheduler.h"
#include "debugout_ac7840x.h"
#include "debug_log.h"
#include "osif.h"

/* ============================================  Define  ============================================ */

// 系统使能状态（外部访问）
extern bool g_systemEnabled;

/* ==========================================  Variables  =========================================== */

/* ==========================================  启动开关相关代码（已注释，暂不使用）  =========================================== */
/*!
 * @note PC17启动开关功能已暂时禁用
 * @note 当前系统启动由PC端的system_enable CAN信号控制
 * @note 以下代码保留供后期项目更新使用
 */

/*
// PC17启动开关相关宏定义
#define START_SWITCH_GPIO_PORT   GPIOC
#define START_SWITCH_PIN        17U
// 如需强制将启动开关视为"开启"状态，置1
#define FORCE_START_SWITCH_ON   1

//!
// * @brief 检查启动开关是否激活
// * @return true: 开关激活, false: 开关关闭
// */
//bool IsStartupSwitchActive(void)
//{
//    #if FORCE_START_SWITCH_ON
//    // 强制开启模式：忽略GPIO检测，直接返回true
//    return true;
//    #else
//    // 正常模式：读取GPIO状态
//    gpio_channel_type_t switch_status = GPIO_DRV_ReadPins(START_SWITCH_GPIO_PORT);
//    uint32_t startup_switch = (switch_status & (1U << START_SWITCH_PIN)) ? 0U : 1U;
//    return (startup_switch == 1U);
//    #endif
//}

/* ==========================================  Functions  =========================================== */

/*!
 * @brief 硬件初始化
 */
static void SystemHardwareInit(void)
{
    ClockConfig_Init();                    // 时钟配置
    CKGEN_DRV_Enable(CLK_GPIO, true);     // GPIO时钟使能
    
    // 初始化PWM模块 - 旁通阀控制需要PWM0
    CKGEN_DRV_Enable(CLK_PWM0, true);     // PWM0时钟使能
    CKGEN_DRV_SoftReset(SRST_PWM0, true); // PWM0软复位
    CKGEN_DRV_SoftReset(SRST_PWM0, false); // PWM0软复位完成
    
    // 初始化调试串口（确保上电早期可用）
    CKGEN_DRV_Enable(CLK_UART1, true);
    CKGEN_DRV_SoftReset(SRST_UART1, true);
    CKGEN_DRV_SoftReset(SRST_UART1, false);
    InitDebug();
    
    // 等待串口稳定
    for(volatile int i = 0; i < 100000; i++);
    
    // 初始化PC17启动开关GPIO
    gpio_settings_config_t start_switch_gpio_config = {
        .base = PORTC,
        .pinPortIdx = 17U,
        .pullConfig = PORT_INTERNAL_PULL_UP_ENABLED,
        .driveSelect = PORT_LOW_DRIVE_STRENGTH,
        .mux = PORT_MUX_AS_GPIO,
        .pinLock = false,
        .intConfig = PORT_DMA_INT_DISABLED,
        .clearIntFlag = false,
        .digitalFilter = false,
        .gpioBase = GPIOC,
        .direction = GPIO_INPUT_DIRECTION,
        .initValue = 0U
    };
    GPIO_DRV_Init(1U, &start_switch_gpio_config);

    // 初始化PE8风冷器控制为GPIO输出
    gpio_settings_config_t cooler_gpio_config = {
        .base = PORTE,
        .pinPortIdx = 8U,
        .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect = PORT_LOW_DRIVE_STRENGTH,
        .mux = PORT_MUX_AS_GPIO,
        .pinLock = false,
        .intConfig = PORT_DMA_INT_DISABLED,
        .clearIntFlag = false,
        .digitalFilter = false,
        .gpioBase = GPIOE,
        .direction = GPIO_OUTPUT_DIRECTION,
        .initValue = 0U
    };
    GPIO_DRV_Init(1U, &cooler_gpio_config);

    // 直接拉高PE8以确保硬件上线（若为低有效，请告知以反相）
    GPIO_DRV_SetPins(GPIOE, (1U << 8));

    // 初始化PB4换向阀控制为GPIO输出（确保方向正确）
    gpio_settings_config_t dir_valve_gpio_config = {
        .base = PORTB,
        .pinPortIdx = 4U,
        .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect = PORT_LOW_DRIVE_STRENGTH,
        .mux = PORT_MUX_AS_GPIO,
        .pinLock = false,
        .intConfig = PORT_DMA_INT_DISABLED,
        .clearIntFlag = false,
        .digitalFilter = false,
        .gpioBase = GPIOB,
        .direction = GPIO_OUTPUT_DIRECTION,
        .initValue = 0U
    };
    GPIO_DRV_Init(1U, &dir_valve_gpio_config);
}

/*!
 * @brief 应用层系统初始化（统一入口）
 * 
 * 功能：
 * - 硬件初始化
 * - 核心模块初始化
 * - CAN通信模块初始化
 * - 任务调度器初始化
 * - 默认状态设置
 * 
 * @note 命名为 App_SystemInit 以避免与系统库中的 SystemInit 函数冲突
 */
void App_SystemInit(void)
{
    // 硬件初始化
    SystemHardwareInit();
    
    // 核心模块初始化
    Sensor_Init();            // 传感器模块初始化
    ValveControl_Init();      // 阀门控制初始化
    FaultDiagnosis_Init();    // 故障诊断初始化
    
    // CAN通信模块初始化（错误处理和打印在其他模块中统一管理，如有需要可在后续版本中补充）
    (void)CAN_Config_Init();
    
    // 初始化优化任务调度器
    OptimizedTaskScheduler_Init();
    
    // 系统使能状态初始化（由PC端system_enable CAN信号控制，初始化为禁用状态）
    g_systemEnabled = false;

    // 上电默认关闭风冷器
    ValveControl_SetCooler(false);
    
    // 上电默认关闭换向阀
    ValveControl_SetDirectionalValve(false);
    
    // 上电默认关闭旁通阀（明确设置为0%）
    ValveControl_SetBypassValve(0.0f);
    
    // 系统初始化完成确认
    Debug_Log_SystemInitCompleted();
}


