/*!
 * @file unified_filter.c
 * @brief 统一滤波管理模块实现
 */

#include "unified_filter.h"
#include "osif.h"
#include <string.h>

/* ==================== 平台抽象接口实现 ==================== */
// 根据算法库平台无关化实施方案添加的平台时间接口实现
platform_uint32_t Platform_GetTimeMs(void) {
    return OSIF_GetMilliseconds();
}

platform_uint32_t Platform_GetTimeSec(void) {
    return OSIF_GetMilliseconds() / 1000;
}

void Platform_DelayMs(platform_uint32_t ms) {
    OSIF_TimeDelay(ms);
}

/* ==========================================  Variables  =========================================== */

static unified_filter_t g_unified_filter;

/* ==========================================  Functions  =========================================== */

void UnifiedFilter_Init(void) {
    // 初始化滤波缓冲区
    memset(&g_unified_filter, 0, sizeof(unified_filter_t));
    
    // 设置默认配置
    g_unified_filter.config.filter_type = FILTER_TYPE_MOVING_AVERAGE;
    g_unified_filter.config.window_size = UNIFIED_FILTER_SIZE;
    g_unified_filter.config.alpha = UNIFIED_FILTER_ALPHA;
    g_unified_filter.config.enabled = true;
    
    g_unified_filter.last_update_time = Platform_GetTimeMs();
    
}

void UnifiedFilter_Configure(const filter_config_t* config) {
    if (config == NULL) return;
    
    g_unified_filter.config = *config;
}

void UnifiedFilter_UpdateData(float oil_temp, float lng_temp, 
                             float oil_pressure, float lng_pressure) {
    if (!g_unified_filter.config.enabled) return;
    
    uint32_t current_time = Platform_GetTimeMs();
    
    // 根据滤波类型选择算法
    switch (g_unified_filter.config.filter_type) {
        case FILTER_TYPE_MOVING_AVERAGE:
            // 移动平均滤波
            g_unified_filter.oil_temp_buffer[g_unified_filter.oil_temp_index] = oil_temp;
            g_unified_filter.oil_temp_index = (g_unified_filter.oil_temp_index + 1) % g_unified_filter.config.window_size;
            
            g_unified_filter.lng_temp_buffer[g_unified_filter.lng_temp_index] = lng_temp;
            g_unified_filter.lng_temp_index = (g_unified_filter.lng_temp_index + 1) % g_unified_filter.config.window_size;
            
            g_unified_filter.oil_pressure_buffer[g_unified_filter.oil_pressure_index] = oil_pressure;
            g_unified_filter.oil_pressure_index = (g_unified_filter.oil_pressure_index + 1) % g_unified_filter.config.window_size;
            
            g_unified_filter.lng_pressure_buffer[g_unified_filter.lng_pressure_index] = lng_pressure;
            g_unified_filter.lng_pressure_index = (g_unified_filter.lng_pressure_index + 1) % g_unified_filter.config.window_size;
            break;
            
        case FILTER_TYPE_LOW_PASS:
        case FILTER_TYPE_EXPONENTIAL:
            // 低通滤波和指数滤波需要上次的滤波值，这里简化处理
            g_unified_filter.oil_temp_buffer[g_unified_filter.oil_temp_index] = oil_temp;
            g_unified_filter.oil_temp_index = (g_unified_filter.oil_temp_index + 1) % g_unified_filter.config.window_size;
            
            g_unified_filter.lng_temp_buffer[g_unified_filter.lng_temp_index] = lng_temp;
            g_unified_filter.lng_temp_index = (g_unified_filter.lng_temp_index + 1) % g_unified_filter.config.window_size;
            
            g_unified_filter.oil_pressure_buffer[g_unified_filter.oil_pressure_index] = oil_pressure;
            g_unified_filter.oil_pressure_index = (g_unified_filter.oil_pressure_index + 1) % g_unified_filter.config.window_size;
            
            g_unified_filter.lng_pressure_buffer[g_unified_filter.lng_pressure_index] = lng_pressure;
            g_unified_filter.lng_pressure_index = (g_unified_filter.lng_pressure_index + 1) % g_unified_filter.config.window_size;
            break;
    }
    
    g_unified_filter.filter_count++;
    g_unified_filter.last_update_time = current_time;
}

float UnifiedFilter_GetFilteredOilTemperature(void) {
    if (!g_unified_filter.config.enabled) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < g_unified_filter.config.window_size; i++) {
        sum += g_unified_filter.oil_temp_buffer[i];
    }
    return sum / g_unified_filter.config.window_size;
}

float UnifiedFilter_GetFilteredLNGTemperature(void) {
    if (!g_unified_filter.config.enabled) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < g_unified_filter.config.window_size; i++) {
        sum += g_unified_filter.lng_temp_buffer[i];
    }
    return sum / g_unified_filter.config.window_size;
}

float UnifiedFilter_GetFilteredOilPressure(void) {
    if (!g_unified_filter.config.enabled) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < g_unified_filter.config.window_size; i++) {
        sum += g_unified_filter.oil_pressure_buffer[i];
    }
    return sum / g_unified_filter.config.window_size;
}

float UnifiedFilter_GetFilteredLNGPressure(void) {
    if (!g_unified_filter.config.enabled) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < g_unified_filter.config.window_size; i++) {
        sum += g_unified_filter.lng_pressure_buffer[i];
    }
    return sum / g_unified_filter.config.window_size;
}

void UnifiedFilter_GetAllFilteredData(unified_sensor_data_t* data) {
    if (data == NULL) return;
    
    data->oil_temp_filtered = UnifiedFilter_GetFilteredOilTemperature();
    data->lng_temp_filtered = UnifiedFilter_GetFilteredLNGTemperature();
    data->oil_pressure_filtered = UnifiedFilter_GetFilteredOilPressure();
    data->lng_pressure_filtered = UnifiedFilter_GetFilteredLNGPressure();
    data->last_update_time = g_unified_filter.last_update_time;
}

float UnifiedFilter_MovingAverage(float* buffer, uint8_t* index, 
                                 uint8_t window_size, float new_value) {
    buffer[*index] = new_value;
    *index = (*index + 1) % window_size;
    
    float sum = 0.0f;
    for (int i = 0; i < window_size; i++) {
        sum += buffer[i];
    }
    return sum / window_size;
}

float UnifiedFilter_LowPass(float last_filtered, float new_value, float alpha) {
    return alpha * new_value + (1.0f - alpha) * last_filtered;
}

float UnifiedFilter_Exponential(float last_filtered, float new_value, float alpha) {
    return alpha * new_value + (1.0f - alpha) * last_filtered;
}

void UnifiedFilter_Reset(void) {
    memset(g_unified_filter.oil_temp_buffer, 0, sizeof(g_unified_filter.oil_temp_buffer));
    memset(g_unified_filter.lng_temp_buffer, 0, sizeof(g_unified_filter.lng_temp_buffer));
    memset(g_unified_filter.oil_pressure_buffer, 0, sizeof(g_unified_filter.oil_pressure_buffer));
    memset(g_unified_filter.lng_pressure_buffer, 0, sizeof(g_unified_filter.lng_pressure_buffer));
    
    g_unified_filter.oil_temp_index = 0;
    g_unified_filter.lng_temp_index = 0;
    g_unified_filter.oil_pressure_index = 0;
    g_unified_filter.lng_pressure_index = 0;
    
    g_unified_filter.filter_count = 0;
    g_unified_filter.last_update_time = Platform_GetTimeMs();
    
}

bool UnifiedFilter_IsReady(void) {
    return g_unified_filter.filter_count >= g_unified_filter.config.window_size;
}

uint32_t UnifiedFilter_GetFilterCount(void) {
    return g_unified_filter.filter_count;
}

void UnifiedFilter_PrintStatus(void) {
    // 统一滤波器状态检查（无打印）
}

/* ==================== 平台无关算法库实现 ==================== */
/* 根据算法库平台无关化实施方案添加的平台无关算法实现 */

/*!
 * @brief 初始化平台无关PID控制器
 */
void PlatformPID_Init(platform_pid_controller_t* pid, platform_float32_t kp, platform_float32_t ki, platform_float32_t kd) {
    if (pid == NULL) {
        return;
    }
    
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->setpoint = 0.0f;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->output_min = -100.0f;
    pid->output_max = 100.0f;
    pid->integral_min = -50.0f;
    pid->integral_max = 50.0f;
    pid->last_time = 0;
}

/*!
 * @brief 计算PID输出
 */
platform_float32_t PlatformPID_Calculate(platform_pid_controller_t* pid, platform_float32_t current_value, platform_uint32_t current_time) {
    if (pid == NULL) {
        return 0.0f;
    }
    
    platform_float32_t error = pid->setpoint - current_value;
    platform_float32_t dt = 0.1f; // 默认100ms控制周期
    
    // 计算时间差
    if (pid->last_time > 0) {
        dt = (platform_float32_t)(current_time - pid->last_time) / 1000.0f; // 转换为秒
        if (dt <= 0.0f || dt > 1.0f) {
            dt = 0.1f; // 限制时间差范围
        }
    }
    
    // 比例项
    platform_float32_t p_term = pid->kp * error;
    
    // 积分项
    pid->integral += error * dt;
    
    // 积分限幅
    if (pid->integral > pid->integral_max) {
        pid->integral = pid->integral_max;
    } else if (pid->integral < pid->integral_min) {
        pid->integral = pid->integral_min;
    }
    platform_float32_t i_term = pid->ki * pid->integral;
    
    // 微分项
    platform_float32_t derivative = 0.0f;
    if (dt > 0.0f) {
        derivative = (error - pid->last_error) / dt;
    }
    platform_float32_t d_term = pid->kd * derivative;
    
    // 计算输出
    platform_float32_t output = p_term + i_term + d_term;
    
    // 输出限幅
    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < pid->output_min) {
        output = pid->output_min;
    }
    
    // 更新状态
    pid->last_error = error;
    pid->last_time = current_time;
    
    return output;
}

/*!
 * @brief 重置PID控制器
 */
void PlatformPID_Reset(platform_pid_controller_t* pid) {
    if (pid == NULL) {
        return;
    }
    
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->last_time = 0;
}

/*!
 * @brief 平台无关移动平均滤波
 */
platform_float32_t PlatformFilter_MovingAverage(platform_float32_t* buffer, platform_uint8_t* index, 
                                               platform_uint8_t window_size, platform_float32_t new_value) {
    if (buffer == NULL || index == NULL || window_size == 0) {
        return new_value;
    }
    
    buffer[*index] = new_value;
    *index = (*index + 1) % window_size;
    
    platform_float32_t sum = 0.0f;
    platform_uint8_t i;
    for (i = 0; i < window_size; i++) {
        sum += buffer[i];
    }
    return sum / window_size;
}

/*!
 * @brief 平台无关低通滤波
 */
platform_float32_t PlatformFilter_LowPass(platform_float32_t last_value, platform_float32_t new_value, platform_float32_t alpha) {
    // 限制alpha范围
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    
    return alpha * new_value + (1.0f - alpha) * last_value;
}

/*!
 * @brief 平台无关主控制算法 (可在PC端运行)
 */
platform_status_t PlatformAlgorithm_Process(platform_sensor_data_t* input, platform_algorithm_result_t* result) {
    if (input == NULL || result == NULL) {
        return PLATFORM_STATUS_INVALID_PARAM;
    }
    
    platform_uint32_t start_time = Platform_GetTimeMs();
    
    // 复制输入数据
    result->sensor_data = *input;
    result->sensor_data.timestamp = start_time;
    
    // 初始化控制输出
    result->control_output.bypass_valve_duty = 0.0f;
    result->control_output.directional_valve_enable = PLATFORM_FALSE;
    result->control_output.cooler_enable = PLATFORM_FALSE;
    result->control_output.system_enable = PLATFORM_FALSE;
    
    // 故障检测
    result->fault_detected = PLATFORM_FALSE;
    
    // 基本故障检测逻辑
    if (input->oil_pressure < 0.0f || input->oil_pressure > 40.0f) {
        result->fault_detected = PLATFORM_TRUE;
    }
    if (input->lng_pressure < 0.0f || input->lng_pressure > 50.0f) {
        result->fault_detected = PLATFORM_TRUE;
    }
    
    // 如果没有故障，进行控制逻辑计算
    if (!result->fault_detected) {
        // LNG压力保护逻辑
        if (input->lng_pressure > 31.5f) {
            // LNG压力过高，停止工作
            result->control_output.system_enable = PLATFORM_FALSE;
        } else if (input->lng_pressure < 28.5f) {
            // LNG压力降低，恢复工作
            result->control_output.system_enable = PLATFORM_TRUE;
        }
        
        // 温度控制
        if (input->oil_temperature > 80.0f) {
            result->control_output.cooler_enable = PLATFORM_TRUE;
        } else if (input->oil_temperature < 75.0f) {
            result->control_output.cooler_enable = PLATFORM_FALSE;
        }
        
        // 换向阀控制（简化逻辑）
        if (input->oil_pressure > 0.5f) {
            result->control_output.directional_valve_enable = PLATFORM_TRUE;
            result->control_output.bypass_valve_duty = 2.0f; // 固定2%开度
        }
    }
    
    // PID控制计算（如果需要的话）
    result->pid_output = 0.0f; // 这里可以添加PID控制逻辑
    
    result->calculation_time = Platform_GetTimeMs() - start_time;
    
    return PLATFORM_STATUS_OK;
}
