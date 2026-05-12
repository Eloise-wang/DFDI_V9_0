/*!
 * @file valve_control.h
 *
 * @brief 阀门硬件控制模块 - 专注于阀门底层控制和硬件操作
 *
 * 重构说明：
 * - 专注于阀门的硬件控制和底层操作
 * - 系统控制逻辑已迁移到hydraulic_control模块
 * - 提供统一的阀门硬件控制接口
 * - 包含阀门状态管理和统计信息
 */

#ifndef VALVE_CONTROL_H
#define VALVE_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "common_types.h"
#include "osif.h"



/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */


/* 阀门硬件控制数据结构 */
typedef struct {
    valve_state_t directional_valve_state;   // 换向阀状态
    valve_state_t bypass_valve_state;       // 旁通阀状态
    valve_state_t cooler_state;             // 风冷器状态
    float bypass_valve_duty;                // 旁通阀开度
    directional_valve_mode_t valve_mode;    // 换向阀控制模式
    directional_valve_stats_t stats;        // 统计信息
    bool hardware_ready;                    // 硬件就绪状态
    uint32_t last_update_time;              // 最后更新时间
    uint32_t error_count;                   // 硬件错误计数
} valve_control_data_t;

/* ==========================================  Functions  =========================================== */

/* ==================== 初始化函数 ==================== */

/*!
 * @brief 初始化阀门硬件控制模块
 */
void ValveControl_Init(void);

/* ==================== 换向阀控制 ==================== */

/*!
 * @brief 设置换向阀硬件状态
 * @param enable 使能状态
 */
void ValveControl_SetDirectionalValve(bool enable);

/*!
 * @brief 获取换向阀状态
 * @return 换向阀状态
 */
valve_state_t ValveControl_GetDirectionalValveState(void);

/*!
 * @brief 设置换向阀控制模式
 * @param mode 控制模式
 */
void ValveControl_SetDirectionalValveMode(directional_valve_mode_t mode);

/*!
 * @brief 获取换向阀控制模式
 * @return 控制模式
 */
directional_valve_mode_t ValveControl_GetDirectionalValveMode(void);





/*!
 * @brief 重置换向阀控制
 */
void ValveControl_ResetDirectionalValveControl(void);

/*!
 * @brief 获取换向阀统计信息
 * @return 统计信息
 */
directional_valve_stats_t ValveControl_GetDirectionalValveStats(void);

/*!
 * @brief 重置换向阀统计信息
 */
void ValveControl_ResetDirectionalValveStats(void);

/* ==================== 旁通阀控制 ==================== */

/*!
 * @brief 设置旁通阀开度
 * @param duty 开度百分比(0-100)
 */
void ValveControl_SetBypassValve(float duty);

/*!
 * @brief 获取旁通阀开度
 * @return 开度百分比
 */
float ValveControl_GetBypassValveDuty(void);

/*!
 * @brief 获取旁通阀状态
 * @return 旁通阀状态
 */
valve_state_t ValveControl_GetBypassValveState(void);

/*!
 * @brief 旁通阀硬件控制
 */
void ValveControl_BypassValveHardwareControl(void);

/*!
 * @brief 启用旁通阀硬件控制
 */
void ValveControl_EnableBypassHardwareControl(void);

/*!
 * @brief 禁用旁通阀硬件控制
 */
void ValveControl_DisableBypassHardwareControl(void);

/*!
 * @brief 设置旁通阀硬件参数
 * @param min_duty 最小开度
 * @param max_duty 最大开度
 * @param step 调节步长
 */
void ValveControl_SetBypassHardwareParams(float min_duty, float max_duty, float step);

/* ==================== 风冷器控制 ==================== */

/*!
 * @brief 设置风冷器状态
 * @param enable 使能状态
 */
void ValveControl_SetCooler(bool enable);

/*!
 * @brief 获取风冷器状态
 * @return 风冷器状态
 */
valve_state_t ValveControl_GetCoolerState(void);

/* ==================== 状态查询 ==================== */

/*!
 * @brief 获取阀门控制数据
 * @return 阀门控制数据
 */
const valve_control_data_t* ValveControl_GetData(void);

/*!
 * @brief 更新阀门控制模块
 */
void ValveControl_Update(void);

/*!
 * @brief 打印阀门硬件状态
 */
void ValveControl_PrintStatus(void);

/*!
 * @brief 检查阀门硬件状态
 * @return true: 硬件正常, false: 硬件异常
 */
bool ValveControl_CheckHardwareStatus(void);

/*!
 * @brief 获取硬件错误信息
 * @return 错误信息字符串
 */
const char* ValveControl_GetHardwareErrorInfo(void);

/*!
 * @brief 重置硬件错误计数
 */
void ValveControl_ResetHardwareErrors(void);

#ifdef __cplusplus
}
#endif

#endif /* VALVE_CONTROL_H */
