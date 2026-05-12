/*!
 * @file fault_diagnosis.h
 *
 * @brief 故障诊断模块头文件（v9.0 嵌入式控制架构）
 *
 * 功能：
 * - 基于传感器模块提供的物理量（油压/气压/温度）进行范围校验与健康检查
 * - 结合阀门响应、系统状态，给出统一的故障代码和健康状态
 * - 为上层控制算法和日志模块提供统一的故障查询接口
 */

#ifndef FAULT_DIAGNOSIS_H
#define FAULT_DIAGNOSIS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include <stdint.h>
#include <stdbool.h>
#include "common_types.h"

/* ============================================  Define  ============================================ */

/* ==================== 故障诊断参数 ==================== */
// 使用common_types.h中的统一定义
// 使用config.h中的统一错误恢复时间
#define FAULT_CONSECUTIVE_LIMIT         3       // 连续故障次数限制

/* ==========================================  Functions  =========================================== */

/* ==================== 故障诊断初始化 ==================== */

/*!
 * @brief 初始化故障诊断模块
 */
void FaultDiagnosis_Init(void);

/* ==================== 传感器故障诊断 ==================== */

/*!
 * @brief 检查传感器健康状态
 * @return true: 正常, false: 异常
 */
bool FaultDiagnosis_CheckSensorHealth(void);

/*!
 * @brief 获取传感器健康状态
 * @return 传感器健康状态
 */
sensor_health_t FaultDiagnosis_GetSensorHealth(void);

/*!
 * @brief 传感器故障诊断
 */
void FaultDiagnosis_SensorFaultDiagnosis(void);

/* ==================== 阀门故障诊断 ==================== */

/*!
 * @brief 检查阀门响应状态
 * @return true: 正常, false: 异常
 */
bool FaultDiagnosis_CheckValveResponse(void);

/*!
 * @brief 获取阀门响应状态
 * @return 阀门响应状态
 */
valve_response_t FaultDiagnosis_GetValveResponse(void);

/*!
 * @brief 阀门故障诊断
 */
void FaultDiagnosis_ValveFaultDiagnosis(void);

/* ==================== 系统故障诊断 ==================== */

/*!
 * @brief 诊断系统故障
 * @return 故障诊断结果
 */
fault_diagnosis_t FaultDiagnosis_DiagnoseSystem(void);

/*!
 * @brief 更新故障诊断数据
 * @param oil_pressure 油压
 * @param lng_pressure LNG压力
 * @param oil_temp 油温
 * @param lng_temp LNG温度
 */
void FaultDiagnosis_UpdateData(float oil_pressure, float lng_pressure, 
                              float oil_temp, float lng_temp);

/* ==================== 故障状态管理 ==================== */

/*!
 * @brief 重置故障诊断
 */
void FaultDiagnosis_Reset(void);

/*!
 * @brief 设置故障代码
 * @param fault_code 故障代码
 */
void FaultDiagnosis_SetFaultCode(fault_code_t fault_code);

/*!
 * @brief 获取故障代码
 * @return 故障代码
 */
fault_code_t FaultDiagnosis_GetFaultCode(void);

/*!
 * @brief 检查是否有故障
 * @return true: 有故障, false: 无故障
 */
bool FaultDiagnosis_HasFault(void);

/*!
 * @brief 清除故障
 */
void FaultDiagnosis_ClearFault(void);

/* ==================== 故障类型检查 ==================== */

/*!
 * @brief 检查是否有传感器故障
 * @return true: 有传感器故障, false: 无传感器故障
 */
bool FaultDiagnosis_HasSensorFault(void);

/*!
 * @brief 获取最后诊断时间
 * @return 最后诊断时间戳(毫秒)
 */
uint32_t FaultDiagnosis_GetLastDiagnosisTime(void);

/*!
 * @brief 检查是否有阀门故障
 * @return true: 有阀门故障, false: 无阀门故障
 */
bool FaultDiagnosis_HasValveFault(void);

/*!
 * @brief 检查是否有压力异常
 * @return true: 有压力异常, false: 无压力异常
 */
bool FaultDiagnosis_HasPressureAbnormal(void);

/*!
 * @brief 检查是否有频率异常
 * @return true: 有频率异常, false: 无频率异常
 */
bool FaultDiagnosis_HasFrequencyAnomaly(void);

/* ==================== 故障诊断工具函数 ==================== */

/*!
 * @brief 验证数值是否在有效范围内
 * @param value 待验证的数值
 * @param min_valid 最小值
 * @param max_valid 最大值
 * @return true: 有效, false: 无效
 */
bool FaultDiagnosis_ValidateValue(float value, float min_valid, float max_valid);

/*!
 * @brief 打印故障诊断状态
 */
void FaultDiagnosis_PrintStatus(void);

#ifdef __cplusplus
}
#endif

#endif
