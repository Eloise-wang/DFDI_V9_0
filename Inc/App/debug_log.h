/*!
 * @file debug_log.h
 *
 * @brief 统一调试输出接口
 *
 * 说明：
 * - 封装所有项目中的调试 / 日志输出
 * - 以后如需新增调试信息，统一通过本模块接口输出
 * - 便于后续统一关闭、重定向或格式化日志
 */

#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 包含 can_message_handler.h 以使用 pc_parameter_config_t */
/* 注意：can_message_handler.h 不包含 debug_log.h，所以不会有循环依赖 */
#include "can_message_handler.h"

/*!
 * @brief 调试模块初始化（当前为占位，预留后续扩展）
 */
static inline void Debug_LogInit(void)
{
    /* 当前无需特殊初始化，如需重定向到其他接口可在此实现 */
}

/*!
 * @brief 通用调试输出函数（行为类似 printf）
 * @param fmt  格式化字符串
 * @param ...  可变参数
 */
void Debug_Log(const char *fmt, ...);

/* ==================== 集中管理的调试打印函数 ==================== */

/*!
 * @brief 打印液压控制状态
 */
void Debug_Log_HydraulicControlStatus(void);

/*!
 * @brief 打印传感器数据监控信息
 */
void Debug_Log_SensorDataMonitor(void);

/* ==================== 阀门控制模块打印函数 ==================== */

/*!
 * @brief 阀门初始化时打印PWM配置结果
 * @param actual_count 实际读取到的PWM计数值
 */
void Debug_Log_ValvePWMInit(uint32_t actual_count);

/*!
 * @brief 换向阀状态变更及GPIO控制日志（仅在状态改变时打印）
 * @param enable 本次目标状态（true=ON, false=OFF）
 * @param prev_enable 上一次状态（true=ON, false=OFF）
 */
void Debug_Log_DirectionalValveControl(bool enable, bool prev_enable);

/*!
 * @brief 旁通阀占空比变更及PWM设置日志（仅在变化超过1%时打印）
 * @param prev_duty  上一次占空比(%)
 * @param new_duty   本次占空比(%)
 * @param max_count  PWM计数器最大值
 * @param count_value 本次设置的计数值
 * @param verify_count 读取回来的计数值（用于校验）
 */
void Debug_Log_BypassValveDuty(float prev_duty, float new_duty,
                               uint16_t max_count, uint16_t count_value,
                               uint32_t verify_count);

/*!
 * @brief 风冷器状态变更及GPIO控制日志（仅在状态改变时打印）
 * @param enable 本次目标状态（true=ON, false=OFF）
 * @param prev_enable 上一次状态（true=ON, false=OFF）
 */
void Debug_Log_CoolerControl(bool enable, bool prev_enable);

/*!
 * @brief 打印CAN通信状态（1000ms周期监控）
 * @param rx_delta 接收消息增量
 * @param tx_delta 发送消息增量
 * @param error_delta 错误增量
 * @param current_rx_count 当前接收计数
 * @param current_tx_count 当前发送计数
 * @param current_error_count 当前错误计数
 * @param monitor_count 监控计数
 */
void Debug_Log_CANStatusMonitor(uint32_t rx_delta, uint32_t tx_delta, uint32_t error_delta,
                                 uint32_t current_rx_count, uint32_t current_tx_count, 
                                 uint32_t current_error_count, uint32_t monitor_count);

/*!
 * @brief 打印实时CAN监控信息（100ms周期）
 * @param current_rx_count 当前接收计数
 * @param current_tx_count 当前发送计数
 * @param rx_delta 接收消息增量
 * @param tx_delta 发送消息增量
 */
void Debug_Log_RealTimeCANMonitor(uint32_t current_rx_count, uint32_t current_tx_count,
                                   uint32_t rx_delta, uint32_t tx_delta);

/*!
 * @brief 打印系统初始化错误信息
 * @param error_msg 错误消息
 */
void Debug_Log_SystemInitError(const char *error_msg);

/*!
 * @brief 打印系统初始化完成信息
 */
void Debug_Log_SystemInitCompleted(void);

/* ==================== CAN配置模块打印函数 ==================== */

/*!
 * @brief 处理CAN事件错误打印（带状态管理，只在首次进入错误状态时打印）
 * @param error_count 错误计数
 * @param event 事件类型
 * @param has_error 是否有错误事件
 */
void Debug_Log_CANEventError(uint32_t error_count, uint32_t event, bool has_error);

/*!
 * @brief 打印CAN Bus-off恢复信息
 */
void Debug_Log_CANBusOffRecovery(void);

/*!
 * @brief 打印CAN发送缓冲区满警告
 */
void Debug_Log_CANTransmitBufferFull(void);

/*!
 * @brief 处理CAN发送错误打印（带状态管理）
 * @param status 错误状态码
 * @param error_count 错误计数
 * @param is_success 是否发送成功
 */
void Debug_Log_CANSendErrorStatus(uint32_t status, uint32_t error_count, bool is_success);

/*!
 * @brief 处理CAN发送统计信息打印（带状态管理，每100次打印一次）
 * @param send_count 发送计数（会自增）
 * @param error_count 错误计数
 */
void Debug_Log_CANSendStatistics(uint32_t *send_count, uint32_t error_count);

/*!
 * @brief 打印CAN控制器重置开始信息
 */
void Debug_Log_CANResetStart(void);

/*!
 * @brief 打印CAN控制器重置成功信息
 */
void Debug_Log_CANResetSuccess(void);

/*!
 * @brief 打印CAN控制器重置失败信息
 * @param status 错误状态码
 */
void Debug_Log_CANResetFailed(uint32_t status);

/* ==================== CAN消息处理模块打印函数 ==================== */

/*!
 * @brief 打印系统使能状态变化（带状态管理，只在状态变化时打印）
 * @param system_enable 当前系统使能状态
 * @param raw_value 原始值
 */
void Debug_Log_CANSystemEnableChanged(bool system_enable, uint8_t raw_value);

/*!
 * @brief 打印CAN消息解包失败（带频率控制，每100次打印一次）
 * @param msg_id 消息ID
 */
void Debug_Log_CANUnpackFailed(uint32_t msg_id);

/*!
 * @brief 打印未知CAN消息ID（带频率控制和状态管理）
 * @param msg_id 消息ID
 * @param msg_id_masked 掩码后的消息ID
 * @param expected_id1 期望的消息ID1
 * @param expected_id2 期望的消息ID2
 * @param length 消息长度
 */
void Debug_Log_CANUnknownMessageID(uint32_t msg_id, uint32_t msg_id_masked, 
                                     uint32_t expected_id1, uint32_t expected_id2, uint8_t length);

/*!
 * @brief 打印CAN发送失败（带状态管理，只在首次失败时打印）
 * @param send_count 发送计数
 * @param fail_count 失败计数
 * @param msg_id 消息ID
 * @param is_success 是否发送成功
 */
void Debug_Log_CANTransmitFailed(uint32_t send_count, uint32_t fail_count, uint32_t msg_id, bool is_success);

/*!
 * @brief 打印CAN消息打包失败（带状态管理，只打印一次）
 * @param pack_result 打包结果
 */
void Debug_Log_CANPackFailed(int pack_result);

/*!
 * @brief 打印从TSMaster接收到的参数配置（仅在参数变化时打印）
 * @param config PC端参数配置结构体指针
 */
void Debug_Log_CANReceivedParameters(const pc_parameter_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_LOG_H */


