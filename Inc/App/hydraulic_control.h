/*!
 * @file hydraulic_control.h
 * @brief 液压系统控制逻辑模块 - 接口定义
 */

#ifndef HYDRAULIC_CONTROL_H
#define HYDRAULIC_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ============================================  Define  ============================================ */

/* 时间常量定义 */
#define COOLER_FORCE_OFF_DURATION_MS    (5U * 60U * 1000U)      // 风冷器强制关闭持续时间：5分钟
#define COOLER_MAX_RUN_TIME_MS          (2U * 60U * 60U * 1000U) // 风冷器最大运行时间：2小时
#define COOLER_MIN_ON_TIME_MS           (20U * 60U * 1000U)     // 风冷器最小运行时间：20min
#define COOLER_MIN_OFF_TIME_MS          (20U * 1000U)           // 风冷器最小停机时间：20秒
#define REV_FREQ_CALC_WINDOW_MS         (60U * 1000U)           // 换向频率计算窗口：1分钟
#define REV_FREQ_WINDOW_SIZE_SEC        (60U)                   // 统计窗口大小：60秒	

/* 风冷器温度滤波参数 */
#define COOLER_TEMP_MEDIAN_SIZE         (5U)                    // 中值滤波窗口
#define COOLER_TEMP_AVG_SIZE            (8U)                    // 平均滤波窗口

/* ===========================================  Typedef  ============================================ */

/**
 * @brief 系统运行状态
 */
typedef enum {
    SYSTEM_STATE_DISABLED = 0,      // 系统禁用
    SYSTEM_STATE_INITIALIZING,      // 初始化中（旁通阀线性下降阶段）
    SYSTEM_STATE_WAITING_REV,       // 等待换向阀启动条件满足
    SYSTEM_STATE_REV_PHASE1,        // 换向阀第一阶段（固定频率）
    SYSTEM_STATE_REV_PHASE2         // 换向阀第二阶段（压力闭环）
} system_state_t;

/**
 * @brief 换向阀控制状态内部定义
 */
typedef enum {
    REV_STATE_IDLE = 0,
    REV_STATE_ON,
    REV_STATE_OFF,
    REV_STATE_FORCE_ON,
    REV_STATE_FORCE_OFF
} reversal_valve_control_state_t;

typedef enum {
    PHASE2_CYCLE_STATE_WAIT_HIGH = 0,
    PHASE2_CYCLE_STATE_RETRACT_TIMED,
    PHASE2_CYCLE_STATE_WAIT_LOW,
    PHASE2_CYCLE_STATE_EXTEND_TIMED
} phase2_cycle_state_t;

/**
 * @brief 控制参数结构体（由PC/CAN端下发）
 */
typedef struct {
    bool system_enable;                         // 系统总使能
    bool auto_mode;                             // 自动模式（0=手动，1=自动）
    
    /* 旁通阀参数 */
    float set_bypass_ratio;                     // 旁通阀目标开度（%）
    float set_bypass_initial_decline_time;      // 初始下降时间（秒）
    bool  bypass_off;                           // 强制关闭旁通阀（触发Ramp上升逻辑）
    
    /* 换向阀启动条件（阶段0->1） */
    float set_rev_start_oilP_max;               // 启动压力上限
    float set_rev_start_oilP_min;               // 启动压力下限
    
    /* 第一阶段参数（固定频率） */
    float set_first_fix_freq_time_on;           // 开启持续时间
    float set_first_fix_freq_time_off;          // 关闭持续时间

    /* 第二阶段参数（伸出/回缩控制） */
    float set_extend_time;                      // 伸出时间（秒）
    float set_extend_pressure;                  // 伸出压力（MPa）
    float set_retract_time;                     // 回缩时间（秒）
    float set_retract_pressure;                 // 回缩压力（MPa）
    
    /* 风冷器参数 */
    float set_cooler_temperature_on;            // 开启温度 (W1)
    float set_cooler_temperature_off;           // 关闭温度 (W2)
} control_params_t;

/**
 * @brief 控制模块内部状态监控结构体
 */
typedef struct {
    system_state_t system_state;                // 当前系统主状态
    reversal_valve_control_state_t rev_state;   // 换向阀逻辑状态
    
    /* 旁通阀实时状态 */
    float current_bypass_duty;                  // 当前PWM占空比（0-100%）
    uint32_t bypass_decline_start_time;         // 下降流程起始时间
    bool bypass_decline_active;                 // 是否处于初始下降流程
    bool bypass_off_prev;                       // bypass_off 信号边沿检测
    uint32_t bypass_off_ramp_last_time;         // ramp 逻辑上一次计算时间
    
    // --- 换向及频率统计 (滑动窗口法) ---
    bool     reversal_valve_enabled;            // 换向阀全局使能标志
    bool     rev_last_state;                    // 上一次换向阀物理状态（ON/OFF）
    uint32_t rev_state_change_time;             // 换向阀状态改变时间戳（阶段1/2计时用）
    uint32_t rev_phase1_start_time;             // 阶段1开始时间
    uint32_t rev_phase2_start_time;             // 阶段2开始时间

    uint16_t rev_freq_per_min;                  // 最终显示值：过去60秒的总换向次数
    uint8_t  rev_toggle_count;                  // 翻转计数器（两次翻转=一次换向）
    uint8_t  counts_per_sec[REV_FREQ_WINDOW_SIZE_SEC]; // 环形队列：每秒换向次数
    uint8_t  current_sec_index;                 // 队列索引
    uint32_t rev_last_update_ms;                // 滑动窗口更新时间戳（ms）
    uint8_t  current_sec_count;                 // 当前这一秒内的临时换向计数

    phase2_cycle_state_t phase2_cycle_state;

    /* 风冷器状态 */
    bool     cooler_enabled;                    // 风冷器当前是否开启
    uint32_t cooler_start_time;                 // 本次启动时间
    uint32_t cooler_stop_time;                  // 上次停止时间
    uint32_t cooler_force_off_start_time;       // 强制保护起始时间
    bool     cooler_force_off_active;           // 是否处于强制关闭保护期

    /* 内部缓存参数 */
    control_params_t params;                    
} hydraulic_control_state_t;

/* ==========================================  Functions  =========================================== */

/**
 * @brief 初始化液压控制模块，重置所有内部状态
 */
void HydraulicControl_Init(void);

/**
 * @brief 更新控制算法参数
 * @param params 外部传入的参数结构体指针
 */
void HydraulicControl_UpdateParams(const control_params_t *params);

/**
 * @brief 设置系统总使能切换
 * @param enable true: 开启控制, false: 关闭并安全复位执行器
 */
void HydraulicControl_SetSystemEnable(bool enable);

/**
 * @brief 液压控制主逻辑处理（建议10ms周期调用）
 * @param oil_pressure 经过初步滤波的实时压力值
 * @param oil_temperature 原始油温传感器值
 */
void HydraulicControl_Process(float oil_pressure, float oil_temperature, float display_lng_pressure);

/**
 * @brief 获取系统当前运行阶段
 */
system_state_t HydraulicControl_GetSystemState(void);

/**
 * @brief 获取当前统计到的换向频率
 */
uint16_t HydraulicControl_GetReversalFrequency(void);

/**
 * @brief 获取完整的内部状态数据（用于监控/调试）
 */
const hydraulic_control_state_t* HydraulicControl_GetState(void);

#ifdef __cplusplus
}
#endif

#endif /* HYDRAULIC_CONTROL_H */

