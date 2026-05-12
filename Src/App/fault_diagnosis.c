/**
 * @file fault_diagnosis.c
 * @brief 统一故障诊断模块实现（v9.0 嵌入式控制架构）
 *
 * 说明：
 * - 使用 Sensor 模块提供的物理量（单位：MPa、°C）和 common_types.h 中的统一阈值进行诊断
 * - 不直接依赖 CAN 生的原始值，避免与 DBC 变化强耦合
 * - 当前仅实现基本的范围检查和简单统计逻辑，后续可在此基础上扩展更复杂的故障规则
 */

#include "fault_diagnosis.h"
#include "sensor.h"
#include "osif.h"
#include <string.h>

/* ==========================================  Variables  =========================================== */

// 故障诊断数据
static fault_diagnosis_t g_fault_diagnosis;
static sensor_health_t g_sensor_health;
static valve_response_t g_valve_response;

// 故障诊断状态
// 这些变量将在后续实现中使用
static uint32_t g_last_diagnosis_time = 0;

/* ==========================================  Functions  =========================================== */

/* ==================== 故障诊断初始化 ==================== */

void FaultDiagnosis_Init(void) {
    memset(&g_fault_diagnosis, 0, sizeof(fault_diagnosis_t));
    memset(&g_sensor_health, 0, sizeof(sensor_health_t));
    memset(&g_valve_response, 0, sizeof(valve_response_t));
    
    // 初始化传感器健康状态
    g_sensor_health.oil_pressure_sensor_ok = true;
    g_sensor_health.lng_pressure_sensor_ok = true;
    g_sensor_health.oil_temp_sensor_ok = true;
    g_sensor_health.lng_temp_sensor_ok = true;
    g_sensor_health.sensor_check_time = OSIF_GetMilliseconds();
    
    // 初始化阀门响应状态
    g_valve_response.directional_valve_ok = true;
    g_valve_response.bypass_valve_ok = true;
    g_valve_response.cooler_ok = true;
    g_valve_response.valve_response_time = 0;
    g_valve_response.cycle_count = 0;
    g_valve_response.valve_stuck = false;
    
    g_last_diagnosis_time = OSIF_GetMilliseconds();
    
}

/* ==================== 传感器故障诊断 ==================== */

bool FaultDiagnosis_CheckSensorHealth(void) {
    uint32_t current_time = OSIF_GetMilliseconds();
    
    // 验证传感器数据有效性
    bool oil_temp_valid = Sensor_ValidateValue(Sensor_GetOilTemperature(), OIL_TEMP_MIN_C, OIL_TEMP_MAX_C);
    bool lng_temp_valid = Sensor_ValidateValue(Sensor_GetLNGTemperature(), LNG_TEMP_MIN_C, LNG_TEMP_MAX_C);
    bool oil_pressure_valid = Sensor_ValidateValue(Sensor_GetOilPressure(), 0.0f, OIL_PRESSURE_MAX_MPA);
    bool lng_pressure_valid = Sensor_ValidateValue(Sensor_GetLNGPressure(), 0.0f, LNG_PRESSURE_MAX_MPA);
    
    // 更新传感器健康状态
    g_sensor_health.oil_temp_sensor_ok = oil_temp_valid;
    g_sensor_health.lng_temp_sensor_ok = lng_temp_valid;
    g_sensor_health.oil_pressure_sensor_ok = oil_pressure_valid;
    g_sensor_health.lng_pressure_sensor_ok = lng_pressure_valid;
    g_sensor_health.sensor_check_time = current_time;
    
    // 检查连续故障次数
    if (!oil_temp_valid || !lng_temp_valid || !oil_pressure_valid || !lng_pressure_valid) {
        g_sensor_health.consecutive_failures++;
        if (g_sensor_health.consecutive_failures > FAULT_CONSECUTIVE_LIMIT) {
            return false; // 传感器故障
        }
    } else {
        g_sensor_health.consecutive_failures = 0;
    }
    
    return true; // 传感器正常
}

sensor_health_t FaultDiagnosis_GetSensorHealth(void) {
    return g_sensor_health;
}

void FaultDiagnosis_SensorFaultDiagnosis(void) {
    // 调用传感器模块的故障诊断
    Sensor_FaultDiagnosis();
    
    // 更新故障诊断状态
    sensor_fault_status_t sensor_fault = Sensor_GetFaultStatus();
    if (sensor_fault != SENSOR_FAULT_NONE) {
        g_fault_diagnosis.sensor_fault = true;
        g_fault_diagnosis.fault_code = FAULT_CODE_SENSOR;
        g_fault_diagnosis.fault_timestamp = OSIF_GetMilliseconds();
        g_fault_diagnosis.consecutive_failures++;
    } else {
        g_fault_diagnosis.sensor_fault = false;
    }
}

/* ==================== 阀门故障诊断 ==================== */

bool FaultDiagnosis_CheckValveResponse(void) {
    uint32_t current_time = OSIF_GetMilliseconds();
    
    // 检查阀门响应时间
    if (current_time - g_valve_response.valve_response_time > FAULT_DETECTION_TIMEOUT_MS) {
        g_valve_response.valve_stuck = true;
        return false;
    }
    
    // 检查换向次数
    if (g_valve_response.cycle_count > 0) {
        g_valve_response.directional_valve_ok = true;
    }
    
    return true; // 阀门响应正常
}

valve_response_t FaultDiagnosis_GetValveResponse(void) {
    return g_valve_response;
}

void FaultDiagnosis_ValveFaultDiagnosis(void) {
    // 检查阀门响应状态
    if (!FaultDiagnosis_CheckValveResponse()) {
        g_fault_diagnosis.valve_fault = true;
        g_fault_diagnosis.fault_code = FAULT_CODE_VALVE_STUCK;
        g_fault_diagnosis.fault_timestamp = OSIF_GetMilliseconds();
        g_fault_diagnosis.consecutive_failures++;
    } else {
        g_fault_diagnosis.valve_fault = false;
    }
}

/* ==================== 系统故障诊断 ==================== */

fault_diagnosis_t FaultDiagnosis_DiagnoseSystem(void) {
    uint32_t current_time = OSIF_GetMilliseconds();
    
    // 执行传感器故障诊断（基于物理量和统一阈值）
    FaultDiagnosis_SensorFaultDiagnosis();
    
    // 执行阀门故障诊断（当前主要检查响应超时/卡死）
    FaultDiagnosis_ValveFaultDiagnosis();
    
    // 检查压力异常（与 common_types.h 中的上限保持一致）
    float oil_pressure = Sensor_GetOilPressure();
    float lng_pressure = Sensor_GetLNGPressure();
    
    // 检查压力是否超出安全范围
    if (oil_pressure > OIL_PRESSURE_LIMIT_HIGH_MPA || lng_pressure > LNG_PRESSURE_UPPER_LIMIT_MPA) {
        // 压力超出安全范围，记录故障
        g_fault_diagnosis.fault_code = FAULT_CODE_PRESSURE_ABNORMAL;
        g_fault_diagnosis.fault_pressure = (oil_pressure > lng_pressure) ? oil_pressure : lng_pressure;
        g_fault_diagnosis.fault_timestamp = OSIF_GetMilliseconds();
        g_fault_diagnosis.pressure_abnormal = true;
        g_last_diagnosis_time = current_time;
        return g_fault_diagnosis;
    }
    
    // 平台无关化后，换向阀频率由PC端控制和监控
    
    g_last_diagnosis_time = current_time;
    
    return g_fault_diagnosis;
}

void FaultDiagnosis_UpdateData(float oil_pressure, float lng_pressure, 
                              float oil_temp, float lng_temp) {
    // 更新故障诊断数据
    g_fault_diagnosis.fault_pressure = oil_pressure;
    
    // 验证传感器数据有效性
    if (!Sensor_ValidateValue(oil_pressure, 0.0f, OIL_PRESSURE_MAX_MPA) ||
        !Sensor_ValidateValue(lng_pressure, 0.0f, LNG_PRESSURE_MAX_MPA) ||
        !Sensor_ValidateValue(oil_temp, OIL_TEMP_MIN_C, OIL_TEMP_MAX_C) ||
        !Sensor_ValidateValue(lng_temp, LNG_TEMP_MIN_C, LNG_TEMP_MAX_C)) {
        // 传感器数据无效，记录故障
        g_fault_diagnosis.fault_code = FAULT_CODE_SENSOR;
        g_fault_diagnosis.fault_timestamp = OSIF_GetMilliseconds();
        g_fault_diagnosis.sensor_fault = true;
    }
}

/* ==================== 故障状态管理 ==================== */

void FaultDiagnosis_Reset(void) {
    memset(&g_fault_diagnosis, 0, sizeof(fault_diagnosis_t));
    g_fault_diagnosis.error_handled = true;
}

void FaultDiagnosis_SetFaultCode(fault_code_t fault_code) {
    g_fault_diagnosis.fault_code = fault_code;
    g_fault_diagnosis.fault_timestamp = OSIF_GetMilliseconds();
    g_fault_diagnosis.error_handled = false;
}

fault_code_t FaultDiagnosis_GetFaultCode(void) {
    return g_fault_diagnosis.fault_code;
}

bool FaultDiagnosis_HasFault(void) {
    return (g_fault_diagnosis.fault_code != FAULT_CODE_NONE);
}

void FaultDiagnosis_ClearFault(void) {
    g_fault_diagnosis.fault_code = FAULT_CODE_NONE;
    g_fault_diagnosis.error_handled = true;
}

/* ==================== 故障类型检查 ==================== */

bool FaultDiagnosis_HasSensorFault(void) {
    return g_fault_diagnosis.sensor_fault;
}

uint32_t FaultDiagnosis_GetLastDiagnosisTime(void) {
    return g_last_diagnosis_time;
}

bool FaultDiagnosis_HasValveFault(void) {
    return g_fault_diagnosis.valve_fault;
}

bool FaultDiagnosis_HasPressureAbnormal(void) {
    return g_fault_diagnosis.pressure_abnormal;
}

bool FaultDiagnosis_HasFrequencyAnomaly(void) {
    return g_fault_diagnosis.frequency_anomaly;
}

/* ==================== 故障诊断工具函数 ==================== */



void FaultDiagnosis_PrintStatus(void) {
    // 故障诊断状态检查（无打印）
}

/* =============================================  EOF  ============================================== */ 
