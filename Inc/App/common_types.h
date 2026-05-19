/*!
 * @file common_types.h
 *
 * @brief 公共类型与系统配置定义文件
 *
 * 功能：
 * - 提供平台无关的基础类型（platform_*），供算法库与嵌入式控制共用
 * - 统一管理系统级阈值、传感器/压力/温度范围、CAN参数等全局配置
 * - 为 v9.0 嵌入式控制架构提供单一“配置与类型”入口，避免重复定义和循环依赖
 *
 * 历史：
 * - v7.3 (2025-10-15)：删除15个未使用的宏定义（时间常量、监控参数等），清理已注释的废弃定义
 * - v9.0 (嵌入式控制架构)：确保所有阈值与 DBC v05 以及嵌入式控制逻辑使用的物理量范围保持一致
 */

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include <stdint.h>
#include <stdbool.h>

/* ============================================  Define  ============================================ */

/* ==================== 平台抽象层类型定义 ==================== */
// 根据算法库平台无关化实施方案添加的平台无关类型
typedef unsigned char      platform_uint8_t;
typedef unsigned short     platform_uint16_t;
typedef unsigned int       platform_uint32_t;
typedef signed char        platform_int8_t;
typedef signed short       platform_int16_t;
typedef signed int         platform_int32_t;
typedef float              platform_float32_t;
typedef double             platform_float64_t;

// 布尔类型
typedef enum {
    PLATFORM_FALSE = 0,
    PLATFORM_TRUE = 1
} platform_bool_t;

// 平台状态返回类型
typedef enum {
    PLATFORM_STATUS_OK = 0,
    PLATFORM_STATUS_ERROR = 1,
    PLATFORM_STATUS_TIMEOUT = 2,
    PLATFORM_STATUS_INVALID_PARAM = 3
} platform_status_t;

/* ==================== 监控和更新间隔 ==================== */
#define SENSOR_UPDATE_INTERVAL_MS            5           // 传感器更新间隔（5ms实现5毫秒采集一次油压）

/* ==================== 温度阈值定义 ==================== */
#define OIL_TEMP_WARNING_HIGH               50.0f       // 油温警告高值
#define OIL_TEMP_WARNING_LOW                -40.0f      // 油温警告低值
#define LNG_TEMP_WARNING_HIGH               58.0f       // LNG温度警告高值
#define LNG_TEMP_WARNING_LOW                -40.0f      // LNG温度警告低值
#define OIL_TEMP_MIN_C                      -40.0f      // 油温最小值(°C)
#define OIL_TEMP_MAX_C                      80.0f       // 油温最大值(°C)，与 DBC device_status.oil_temperature 物理上限一致
#define LNG_TEMP_MIN_C                      -40.0f      // LNG温度最小值(°C)
#define LNG_TEMP_MAX_C                      80.0f       // LNG温度最大值(°C)，与 DBC device_status.LNG_temperature 物理上限一致
// 工程量程（用于上电自检与 device_status.debug 标志，按需求：-40~60℃）
#define OIL_TEMP_MEAS_MIN_C                 -40.0f      // 油温测量最小值(°C)
#define OIL_TEMP_MEAS_MAX_C                 60.0f       // 油温测量最大值(°C)
#define LNG_TEMP_MEAS_MIN_C                 -40.0f      // LNG温度测量最小值(°C)
#define LNG_TEMP_MEAS_MAX_C                 60.0f       // LNG温度测量最大值(°C)

/* ==================== 压力阈值定义 ==================== */
#define OIL_PRESSURE_NORMAL_MIN_MPA         2.5f        // 油压正常最小值
#define OIL_PRESSURE_NORMAL_MAX_MPA         9.0f        // 油压正常最大值
#define OIL_PRESSURE_WARNING_MPA            14.5f       // 油压警告值
#define OIL_PRESSURE_LIMIT_HIGH_MPA         15.0f       // 油压高保护阈值
#define OIL_PRESSURE_EMERGENCY_MPA          16.0f       // 油压紧急保护阈值（快速保护使用）
#define OIL_PRESSURE_MAX_MPA                16.0f       // 油压最大值(MPa)
// 工程量程（用于上电自检与 device_status.debug 标志，按需求：0~14MPa）
#define OIL_PRESSURE_MEAS_MIN_MPA           0.0f        // 油压测量最小值 (MPa)
#define OIL_PRESSURE_MEAS_MAX_MPA           14.0f       // 油压测量最大值 (MPa)

// 已删除：LNG_PRESSURE_TARGET_MPA - 控制逻辑已移至PC端
#define LNG_PRESSURE_TOLERANCE_MPA          1.0f        // LNG压力容差
#define LNG_PRESSURE_MAX_MPA                35.0f       // LNG压力最大值(MPa)
#define LNG_PRESSURE_UPPER_LIMIT_MPA        35.0f       // LNG压力上限(MPa)
// 说明：
// - 传感器换算（Voltage_To_LNG_Pressure）按 0~35MPa 线性输出；
// - 二阶段超压策略包含 31.5/33MPa 区间判断，因此工程量程必须覆盖到 ≥33MPa；
// - 若这里仍按 31.5MPa 限幅，10ms 控制任务会因为量程检查失败直接 return，导致超压逻辑无法执行。
// 工程量程（用于上电自检与 device_status.debug 标志，按需求：0~35MPa）
#define LNG_PRESSURE_MEAS_MIN_MPA           0.0f        // LNG压力测量最小值 (MPa)
#define LNG_PRESSURE_MEAS_MAX_MPA           35.0f       // LNG压力测量最大值 (MPa)

/* ==================== 滤波参数 ==================== */
#define MONITOR_FILTER_SIZE                 5           // 监控滤波窗口大小（用于 sensor.c 中 g_filter_buffer）

/* ==================== 阀门控制参数 ==================== */
#define BYPASS_VALVE_MAX_DUTY               50.0f       // 旁通阀最大开度(%)，与 DBC set_bypass_ratio / device_status.bypass_ratio 匹配
#define BYPASS_VALVE_MIN_DUTY               0.0f        // 旁通阀最小开度(%)
#define BYPASS_VALVE_ADJUSTMENT_STEP        0.5f        // 旁通阀调节步长(%)
#define BYPASS_VALVE_FIXED_DUTY             0.0f        // 旁通阀固定开度值（历史参数，当前未在控制逻辑中使用）


/* ==================== 风冷器参数 ==================== */
#define COOLER_COOLDOWN_TIME_MS            30000        // 风冷器冷却时间(ms)
#define COOLER_MIN_RUN_TIME_MS             5000         // 风冷器最小运行时间(ms)


/* ==================== 故障检测参数 ==================== */
#define FAULT_DETECTION_TIMEOUT_MS          10000       // 故障检测超时时间(ms)

/* ==================== 硬件配置参数 ==================== */
#define PWM_PERIOD                          10000U      // PWM周期值
#define ADC_CHANNEL_COUNT                   4           // ADC通道数量
#define ADC_REFERENCE_VOLTAGE               5.0f        // 参考电压5V
#define ADC_MAX_VALUE                       4095.0f     // ADC最大值(12位)

/* ==================== ADC通道定义 ==================== */
#define ADC_CHANNEL_OIL_TEMP               1           // 油温传感器通道 PA3_ADC1_IN1_LNG_T
#define ADC_CHANNEL_LNG_TEMP               0           // LNG温度传感器通道 PA2_ADC1_IN0_T
#define ADC_CHANNEL_OIL_PRESSURE           2           // 油压传感器通道 PD2_ADC1_IN2_P
#define ADC_CHANNEL_LNG_PRESSURE           3           // LNG压力传感器通道 PD3_ADC1_IN3_LNG_P

/* ==================== 电压和压力范围 ==================== */
#define VOLTAGE_MIN_VALID                  0.0f        // 最小有效电压(V)
#define VOLTAGE_MAX_VALID                  4.0f        // 最大有效电压(V)
#define PRESSURE_MAX_OUTPUT_MPA            40.0f       // 压力传感器最大输出(MPa)

/* ==================== 温度校准参数 ==================== */
#define OIL_TEMP_CALIBRATION_OFFSET        0.0f        // 油温传感器校准偏移量 (°C)
#define LNG_TEMP_CALIBRATION_OFFSET        0.0f        // LNG温度传感器校准偏移量 (°C)

/* ==================== 阀门控制引脚定义 ==================== */
#define COOLER_CONTROL_PORT                 GPIOE       // 风冷器控制端口
#define COOLER_CONTROL_PIN                  8U          // 风冷器控制引脚 PE8
#define DIRECTIONAL_VALVE_PORT              GPIOB       // 换向阀控制端口
#define DIRECTIONAL_VALVE_PIN               4U          // 换向阀控制引脚 PB4

/* ==================== PT1000传感器参数 ==================== */
#define PT1000_CIRCUIT_GAIN                10.16f      // INA826电路增益（基于R72=5.49kΩ）
#define PT1000_WORK_CURRENT_A              0.0002f     // 工作电流 0.2mA
#define PT1000_ADC_REFERENCE_MV            5000.0f     // ADC参考电压 5V
#define PT1000_RESISTANCE_MIN_OHM          700.0f      // PT1000最小电阻值(Ω)
#define PT1000_RESISTANCE_MAX_OHM          2000.0f     // PT1000最大电阻值(Ω)
#define PT1000_RESISTANCE_0C_OHM           1000.0f     // PT1000在0°C时的标准电阻值(Ω)
#define PT1000_RESISTANCE_OUT_OF_RANGE_F   (-999.0f)   // 电阻超出范围错误码

/* ==================== PT1000查找表参数 ==================== */
#define LUT_TEMP_START_C                   (-200)      // 查找表起始温度(°C)
#define LUT_TEMP_STEP_C                    (1)         // 查找表温度步长(°C)
#define LUT_TABLE_SIZE                     (301)       // 查找表大小

/* ==================== 传感器有效值范围 ==================== */
#define SENSOR_VALUE_MIN_VALID             -200.0f     // 传感器最小值（放宽范围，允许传感器未连接）
#define SENSOR_VALUE_MAX_VALID             300.0f      // 传感器最大值（放宽范围，允许传感器未连接）

/* ==================== CAN通信参数 ==================== */
#define CAN_MSG_BUFFER_COUNT               10U         // CAN消息缓冲区数量
#define CAN_FILTER_COUNT                   16U         // CAN过滤器数量
#define CAN_MSG_DATA_MAX_SIZE              8U          // CAN 2.0最大数据长度
#define CAN_TIMEOUT_MS                     1000U       // CAN操作超时时间(ms)
#define CAN_SOURCE_CLOCK                   60UL        // CAN时钟源 60MHz
#define CAN_BITRATE_NUM_HPC                7U          // 高压控制器支持的波特率数量
#define CAN_MSG_HANDLE_DIRECT              0U          // 是否直接处理消息 1:是, 0:否
#define CAN_TIME_STAMP_ENABLE              0U          // 是否启用时间戳 1:启用, 0:禁用

/* ==================== CAN引脚配置 ==================== */
#define CAN_RX_PORT                        PORTE       // CAN接收端口
#define CAN_RX_GPIO                        GPIOE       // CAN接收GPIO
#define CAN_RX_PIN                         4U          // CAN接收引脚 PE4
#define CAN_TX_PORT                        PORTE       // CAN发送端口
#define CAN_TX_GPIO                        GPIOE       // CAN发送GPIO
#define CAN_TX_PIN                         5U          // CAN发送引脚 PE5
#define CAN_STB_PORT                       PORTE       // CAN待机端口
#define CAN_STB_GPIO                       GPIOE       // CAN待机GPIO
#define CAN_STB_PIN                        10U         // CAN待机引脚 PE10

/* ===========================================  Typedef  ============================================ */

/* ==================== 平台无关算法库结构体 ==================== */
/*!
 * @brief 平台无关PID控制器结构体
 */
typedef struct {
    platform_float32_t kp;              // 比例系数
    platform_float32_t ki;              // 积分系数
    platform_float32_t kd;              // 微分系数
    platform_float32_t setpoint;        // 目标值
    platform_float32_t integral;        // 积分累积
    platform_float32_t last_error;      // 上次误差
    platform_float32_t output_min;      // 输出下限
    platform_float32_t output_max;      // 输出上限
    platform_float32_t integral_min;    // 积分下限
    platform_float32_t integral_max;    // 积分上限
    platform_float32_t last_time;       // 上次时间戳
} platform_pid_controller_t;

/*!
 * @brief 平台无关传感器数据结构体
 */
typedef struct {
    platform_float32_t oil_pressure;    // 油压 (MPa)
    platform_float32_t lng_pressure;    // LNG压力 (MPa)
    platform_float32_t oil_temperature; // 油温 (°C)
    platform_float32_t lng_temperature; // LNG温度 (°C)
    platform_uint32_t timestamp;        // 时间戳
} platform_sensor_data_t;

/*!
 * @brief 平台无关控制输出结构体
 */
typedef struct {
    platform_float32_t bypass_valve_duty;        // 旁通阀开度 (0-100%)
    platform_bool_t directional_valve_enable;    // 换向阀使能
    platform_bool_t cooler_enable;               // 风冷器使能
    platform_bool_t system_enable;               // 系统使能
} platform_control_output_t;

/*!
 * @brief 平台无关算法结果结构体
 */
typedef struct {
    platform_sensor_data_t sensor_data;          // 传感器数据
    platform_control_output_t control_output;    // 控制输出
    platform_float32_t pid_output;              // PID输出
    platform_bool_t fault_detected;             // 故障检测
    platform_uint32_t calculation_time;         // 计算时间
} platform_algorithm_result_t;

/* ==================== 故障与传感器状态枚举 ==================== */
/*!
 * @brief 故障代码枚举
 */
typedef enum {
    FAULT_CODE_NONE = 0x00,          // 无故障
    FAULT_CODE_SENSOR = 0x01,        // 传感器故障
    FAULT_CODE_PRESSURE_ABNORMAL = 0x02, // 压力异常
    FAULT_CODE_VALVE_STUCK = 0x03,   // 阀门卡死
    FAULT_CODE_FREQUENCY_ANOMALY = 0x04, // 频率异常
    FAULT_CODE_SYSTEM_ERROR = 0x05,  // 系统错误
    FAULT_CODE_COMMUNICATION = 0x06, // 通信故障
    FAULT_CODE_POWER = 0x07,         // 电源故障
    FAULT_CODE_TEMPERATURE = 0x08,   // 温度故障
    FAULT_CODE_PRESSURE_HIGH = 0x09, // 压力过高
    FAULT_CODE_PRESSURE_LOW = 0x0A   // 压力过低
} fault_code_t;

/*!
 * @brief 统一传感器故障状态枚举
 */
typedef enum {
    SENSOR_FAULT_NONE = 0x00,           // 无故障
    SENSOR_FAULT_OIL_TEMP = 0x01,       // 油温传感器故障
    SENSOR_FAULT_LNG_TEMP = 0x02,       // LNG温度传感器故障
    SENSOR_FAULT_OIL_PRESSURE = 0x04,   // 油压传感器故障
    SENSOR_FAULT_LNG_PRESSURE = 0x08,   // LNG压力传感器故障
    SENSOR_FAULT_ADC = 0x10,            // ADC故障
    SENSOR_FAULT_TIMEOUT = 0x20         // 传感器超时故障
} sensor_fault_status_t;

/*!
 * @brief 系统故障状态枚举
 */
typedef enum {
    SYSTEM_FAULT_NONE = 0x00,           // 无故障
    SYSTEM_FAULT_HARDWARE = 0x01,       // 硬件故障
    SYSTEM_FAULT_SOFTWARE = 0x02,       // 软件故障
    SYSTEM_FAULT_COMMUNICATION = 0x04,  // 通信故障
    SYSTEM_FAULT_POWER = 0x08,          // 电源故障
    SYSTEM_FAULT_WATCHDOG = 0x10,       // 看门狗故障
    SYSTEM_FAULT_EMERGENCY = 0x20       // 紧急故障
} system_fault_status_t;

/* ==================== 阀门状态枚举 ==================== */
/*!
 * @brief 通用阀门状态枚举
 */
typedef enum {
    VALVE_STATE_OFF = 0,             // 阀门关闭
    VALVE_STATE_ON,                  // 阀门开启
    VALVE_STATE_ERROR                // 阀门错误
} valve_state_t;

/*!
 * @brief 换向阀控制模式
 */
typedef enum {
    DIRECTIONAL_VALVE_MODE_AUTO = 0,   // 自动模式
    DIRECTIONAL_VALVE_MODE_MANUAL = 1, // 手动模式
    DIRECTIONAL_VALVE_MODE_OFF = 2     // 关闭模式
} directional_valve_mode_t;

/* ==================== 控制相关结构体（保留平台无关 PID） ==================== */

/* ==================== 故障诊断结构体 ==================== */
/*!
 * @brief 故障诊断结构体
 */
typedef struct {
    fault_code_t fault_code;          // 故障代码
    uint32_t fault_timestamp;         // 故障时间戳
    float fault_pressure;             // 故障时压力
    uint32_t consecutive_failures;    // 连续故障次数
    bool sensor_fault;                // 传感器故障标志
    bool valve_fault;                 // 阀门故障标志
    bool pressure_abnormal;           // 压力异常标志
    bool frequency_anomaly;           // 频率异常标志
    bool error_handled;               // 错误已处理标志
} fault_diagnosis_t;

/* ==================== 传感器相关结构体 ==================== */
/*!
 * @brief 传感器健康状态结构体
 */
typedef struct {
    bool oil_pressure_sensor_ok;      // 油压传感器正常
    bool lng_pressure_sensor_ok;      // LNG压力传感器正常
    bool oil_temp_sensor_ok;          // 油温传感器正常
    bool lng_temp_sensor_ok;          // LNG温度传感器正常
    uint32_t sensor_check_time;       // 传感器检查时间
    uint32_t consecutive_failures;    // 连续故障次数
} sensor_health_t;

/*!
 * @brief 传感器数据有效性状态结构体
 */
typedef struct {
    bool oil_temp_valid;        // 油温数据有效
    bool lng_temp_valid;        // LNG温度数据有效
    bool oil_pressure_valid;    // 油压数据有效
    bool lng_pressure_valid;    // LNG压力数据有效
    uint32_t last_valid_time;   // 最后有效时间
} sensor_validity_status_t;

/* ==================== 阀门相关结构体 ==================== */
/*!
 * @brief 阀门响应状态结构体
 */
typedef struct {
    bool directional_valve_ok;        // 换向阀正常
    bool bypass_valve_ok;             // 旁通阀正常
    bool cooler_ok;                   // 风冷器正常
    uint32_t valve_response_time;     // 阀门响应时间
    uint32_t cycle_count;             // 换向次数
    bool valve_stuck;                 // 阀门卡死标志
} valve_response_t;

/*!
 * @brief 换向阀统计数据结构
 */
typedef struct {
    uint32_t total_cycles;           // 总换向次数
    uint32_t successful_cycles;      // 成功换向次数
    float average_frequency;         // 平均换向频率(次/分钟)
    float max_pressure_peak;         // 最大压力峰值(MPa)
    float min_pressure_valley;       // 最小压力谷值(MPa)
    uint32_t protection_activations; // 保护激活次数
    uint32_t last_cycle_time;        // 上次换向时间(ms)
    float pressure_change_rate;      // 压力变化率(MPa/s)
    uint32_t fault_count;            // 故障次数
    uint32_t uptime_hours;           // 运行时间(小时)
} directional_valve_stats_t;

/* ==================== 统一传感器与监控结构体 ==================== */
/*!
 * @brief 统一传感器数据结构体 - 整合原始和滤波数据
 */
typedef struct {
    // 原始数据
    uint16_t adc_raw[ADC_CHANNEL_COUNT];        /*!< ADC原始值 */
    float voltage[ADC_CHANNEL_COUNT];           /*!< 电压值 */
    float oil_temp_celsius;                     /*!< 油温(摄氏度) */
    float lng_temp_celsius;                     /*!< LNG温度(摄氏度) */
    float oil_pressure_mpa;                     /*!< 油压(MPa) */
    float lng_pressure_mpa;                     /*!< LNG压力(MPa) */
    
    // 滤波后数据
    float oil_temp_filtered;                     /*!< 滤波后油温(摄氏度) */
    float lng_temp_filtered;                     /*!< 滤波后LNG温度(摄氏度) */
    float oil_pressure_filtered;                /*!< 滤波后油压(MPa) */
    float lng_pressure_filtered;                /*!< 滤波后LNG压力(MPa) */
    
    // 分析数据
    float pressure_change_rate;                 /*!< 压力变化率(MPa/s) */
    uint32_t last_update_time;                  /*!< 最后更新时间 */
} unified_sensor_data_t;

/*!
 * @brief 传感器数据结构体 - 兼容旧代码
 * @deprecated 使用unified_sensor_data_t替代
 */
typedef unified_sensor_data_t sensor_data_t;

/*!
 * @brief 传感器监控数据结构体
 */
typedef struct {
    sensor_data_t raw_data;             // 原始传感器数据
    sensor_data_t filtered_data;        // 滤波后的传感器数据
    sensor_fault_status_t fault_status; // 传感器故障状态
    sensor_validity_status_t validity;  // 传感器数据有效性
    bool cooling_needed;                // 是否需要风冷
    uint32_t last_update_time;          // 最后更新时间戳
    uint32_t fault_detection_timer;     // 故障检测计时器
    uint32_t timeout_timer;             // 超时计时器
} sensor_monitor_t;

/* ==================== 打印频率常量 ==================== */
#define PRINT_FREQ_STARTUP_SWITCH                 100         // 启动开关状态打印频率(次数) (已使用)
#define PRINT_FREQ_DIRECTIONAL_VALVE              100         // 换向阀状态打印频率(次数) (已使用)
#define PRINT_FREQ_BYPASS_VALVE                   10          // 旁通阀状态打印频率(次数) (已使用)
#define PRINT_FREQ_SYSTEM_STATS                   10          // 系统统计打印频率(次数) (已使用)
#define PRINT_FREQ_SYSTEM_MONITOR                 50          // 系统监控打印频率(次数) (已使用)
#define PRINT_FREQ_TEST_FUNCTIONS                 100         // 测试函数打印频率(次数) (已使用)

/* ==================== 平台无关算法库函数声明 ==================== */
// 根据算法库平台无关化实施方案添加的算法库接口

/*!
 * @brief 初始化平台无关PID控制器
 * @param pid PID控制器指针
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 */
void PlatformPID_Init(platform_pid_controller_t* pid, platform_float32_t kp, platform_float32_t ki, platform_float32_t kd);

/*!
 * @brief 计算PID输出
 * @param pid PID控制器指针
 * @param current_value 当前值
 * @param current_time 当前时间戳
 * @return PID输出值
 */
platform_float32_t PlatformPID_Calculate(platform_pid_controller_t* pid, platform_float32_t current_value, platform_uint32_t current_time);

/*!
 * @brief 重置PID控制器
 * @param pid PID控制器指针
 */
void PlatformPID_Reset(platform_pid_controller_t* pid);

/*!
 * @brief 平台无关滤波算法
 * @param buffer 滤波缓冲区
 * @param index 当前索引指针
 * @param window_size 窗口大小
 * @param new_value 新值
 * @return 滤波后值
 */
platform_float32_t PlatformFilter_MovingAverage(platform_float32_t* buffer, platform_uint8_t* index, 
                                               platform_uint8_t window_size, platform_float32_t new_value);

/*!
 * @brief 平台无关低通滤波
 * @param last_value 上次滤波值
 * @param new_value 新值
 * @param alpha 滤波系数 (0-1)
 * @return 滤波后值
 */
platform_float32_t PlatformFilter_LowPass(platform_float32_t last_value, platform_float32_t new_value, platform_float32_t alpha);

/*!
 * @brief 平台无关主控制算法 (可在PC端运行)
 * @param input 输入传感器数据
 * @param result 输出算法结果
 * @return 算法执行状态
 */
platform_status_t PlatformAlgorithm_Process(platform_sensor_data_t* input, platform_algorithm_result_t* result);

/* ==================== 平台抽象接口声明 ==================== */
// 根据算法库平台无关化实施方案添加的平台时间接口
platform_uint32_t Platform_GetTimeMs(void);
platform_uint32_t Platform_GetTimeSec(void);
void Platform_DelayMs(platform_uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_TYPES_H */
