/*!
 * @file can_message_handler.h
 *
 * @brief CAN消息处理模块 - 负责CAN消息的接收、解析、发送
 *
 * 功能说明：
 * - 接收PC端参数配置消息（tsmaster_control, tsmaster_control2）
 * - 发送传感器数据到PC端（device_status）
 * - CAN通信状态监控
 * - 参数配置存储和访问接口
 */

#ifndef CAN_MESSAGE_HANDLER_H
#define CAN_MESSAGE_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include <stdint.h>
#include <stdbool.h>

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/*!
 * @brief PC端参数配置结构体
 * 
 * 说明：
 * - 从 tsmaster_control2 消息接收的参数
 * - 从 tsmaster_control 消息接收的参数
 * - 供嵌入式端控制算法使用
 */
typedef struct {
    // ========== 来自 tsmaster_control2 消息 ==========
    // 系统控制
    bool system_enable;                          // 系统使能（从 tsmaster_control2.system_enable）
    bool auto_mode;                             // 自动模式（从 tsmaster_control2.auto_mode）
    
    // 旁通阀控制参数
    double set_bypass_ratio;                     // 旁通阀比例（0..50%，从 tsmaster_control2.set_bypass_ratio）
    
    // 伸出/回缩参数
    double set_extend_time;                      // 伸出时间（0..6.3s，从 tsmaster_control2.set_extend_time）
    double set_extend_pressure;                  // 伸出压力（0..25.5MPa，从 tsmaster_control2.set_extend_pressure）
    double set_retract_time;                     // 回缩时间（0..3.1s，从 tsmaster_control2.set_retract_time）
    double set_retract_pressure;                 // 回缩压力（0..12.7MPa，从 tsmaster_control2.set_retract_pressure）
    
    // 风冷器控制参数
    double set_cooler_temp_on;                   // 风冷器开启温度阈值（-40..80°C，从 tsmaster_control2.set_cooler_temperature_on）
    double set_cooler_temp_off;                  // 风冷器关闭温度阈值（-40..80°C，从 tsmaster_control2.set_cooler_temperature_off）
    
    // ========== 来自 tsmaster_control 消息 ==========
    // 旁通阀控制参数
    double set_bypass_initial_decline_time;      // 旁通阀起始下降时间（0..60s，从 tsmaster_control.set_bypass_initial_decline_time）
    
    // 换向阀启动参数
    double set_rev_start_oilP_max;               // 换向阀启动油压限制最大值（0..10MPa，从 tsmaster_control.set_rev_start_oilP_max）
    double set_rev_start_oilP_min;               // 换向阀启动油压限制最小值（0..10MPa，从 tsmaster_control.set_rev_start_oilP_min）
    
    // 第一阶段换向参数
    double set_first_fix_freq_time_on;           // 第一阶段换向阀固定频率设置开启时常（0..10s，从 tsmaster_control.set_first_fix_freq_time_on）
    double set_first_fix_freq_time_off;          // 第一阶段换向阀固定频率设置关闭时常（0..10s，从 tsmaster_control.set_first_fix_freq_time_off）
    
    bool bypass_off;                             // 旁通阀关闭开关（从 tsmaster_control.bypass_off）
} pc_parameter_config_t;

/* ==========================================  Functions  =========================================== */

/* ==================== 初始化接口 ==================== */

/*!
 * @brief 初始化CAN消息处理模块
 */
void CAN_MessageHandler_Init(void);

/* ==================== CAN接收回调 ==================== */

/*!
 * @brief CAN接收回调函数
 * @param msg_id CAN消息ID
 * @param data 数据指针
 * @param length 数据长度
 */
void CAN_MessageHandler_RxCallback(uint32_t msg_id, const uint8_t* data, uint8_t length);

/* ==================== 任务函数 ==================== */

/*!
 * @brief 任务：发送传感器数据（10ms周期）
 */
void CAN_MessageHandler_Task_10ms_SendSensorData(void);

/*!
 * @brief 任务：CAN通信状态监控（1000ms周期）
 */
void CAN_MessageHandler_Task_1000ms_CANStatusMonitor(void);

/*!
 * @brief 任务：实时CAN信号监控（100ms周期）
 */
void CAN_MessageHandler_Task_100ms_RealTimeCANMonitor(void);

/*!
 * @brief 任务：CAN消息处理（1ms周期 - 关键任务）
 */
void CAN_MessageHandler_Task_1ms_CANMessageProcess(void);

/* ==================== 参数配置访问接口 ==================== */

/*!
 * @brief 获取换向阀频率（从控制算法获取）
 * @return 换向阀频率（次/分钟）
 */
uint16_t CAN_MessageHandler_GetReversalValveFreq(void);

/*!
 * @brief 获取PC端参数配置
 * @param[out] config 参数配置结构体指针
 */
void CAN_MessageHandler_GetParameterConfig(pc_parameter_config_t* config);

/* ==================== CAN错误检测 ==================== */

/*!
 * @brief CAN错误检测和恢复（在主循环中调用）
 */
void CAN_MessageHandler_ErrorCheck(void);

/*!
 * @brief 获取CAN接收消息计数
 * @return CAN接收消息总数
 */
uint32_t CAN_MessageHandler_GetRxMessageCount(void);

float CAN_MessageHandler_GetFilteredLNGPressure(void);

#ifdef __cplusplus
}
#endif

#endif /* CAN_MESSAGE_HANDLER_H */

