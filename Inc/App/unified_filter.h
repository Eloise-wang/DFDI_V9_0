/*!
 * @file unified_filter.h
 * @brief 统一滤波管理模块 - 整合所有滤波功能
 * 
 * 功能模块：
 * - 统一管理所有传感器的滤波缓冲区
 * - 提供统一的滤波算法接口
 * - 支持多种滤波算法（移动平均、低通滤波等）
 */

#ifndef UNIFIED_FILTER_H
#define UNIFIED_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include <stdint.h>
#include <stdbool.h>
#include "common_types.h"

/* 平台无关算法库 - 可在PC和嵌入式平台编译 */
#ifdef PLATFORM_INDEPENDENT_ALGORITHM
    // 在PC端或TSMaster中使用，不需要平台特定头文件
#else
    // 嵌入式平台使用原有包含
#endif

/* ============================================  Define  ============================================ */

/* ==================== 滤波算法类型 ==================== */
#define FILTER_TYPE_MOVING_AVERAGE    0   // 移动平均滤波
#define FILTER_TYPE_LOW_PASS          1   // 低通滤波
#define FILTER_TYPE_EXPONENTIAL       2   // 指数滤波

/* ==================== 滤波参数 ==================== */
#define UNIFIED_FILTER_SIZE           10U         // 统一滤波窗口大小
#define UNIFIED_FILTER_ALPHA          0.3f        // 滤波系数

/* ===========================================  Typedef  ============================================ */

/*!
 * @brief 滤波配置结构体
 */
typedef struct {
    uint8_t filter_type;              // 滤波算法类型
    uint8_t window_size;              // 滤波窗口大小
    float alpha;                      // 滤波系数
    bool enabled;                     // 滤波使能
} filter_config_t;

/*!
 * @brief 统一滤波管理器结构体
 */
typedef struct {
    // 滤波缓冲区 - 统一管理所有传感器数据
    float oil_temp_buffer[UNIFIED_FILTER_SIZE];
    float lng_temp_buffer[UNIFIED_FILTER_SIZE];
    float oil_pressure_buffer[UNIFIED_FILTER_SIZE];
    float lng_pressure_buffer[UNIFIED_FILTER_SIZE];
    
    // 缓冲区索引
    uint8_t oil_temp_index;
    uint8_t lng_temp_index;
    uint8_t oil_pressure_index;
    uint8_t lng_pressure_index;
    
    // 滤波配置
    filter_config_t config;
    
    // 统计信息
    uint32_t filter_count;
    uint32_t last_update_time;
} unified_filter_t;

/* ==========================================  Functions  =========================================== */

/* ==================== 滤波管理接口 ==================== */

/*!
 * @brief 初始化统一滤波管理器
 */
void UnifiedFilter_Init(void);

/*!
 * @brief 配置滤波参数
 * @param config 滤波配置
 */
void UnifiedFilter_Configure(const filter_config_t* config);

/*!
 * @brief 更新传感器数据并滤波
 * @param oil_temp 油温原始值
 * @param lng_temp LNG温度原始值
 * @param oil_pressure 油压原始值
 * @param lng_pressure LNG压力原始值
 */
void UnifiedFilter_UpdateData(float oil_temp, float lng_temp, 
                             float oil_pressure, float lng_pressure);

/* ==================== 滤波数据获取接口 ==================== */

/*!
 * @brief 获取滤波后油温
 * @return 滤波后油温
 */
float UnifiedFilter_GetFilteredOilTemperature(void);

/*!
 * @brief 获取滤波后LNG温度
 * @return 滤波后LNG温度
 */
float UnifiedFilter_GetFilteredLNGTemperature(void);

/*!
 * @brief 获取滤波后油压
 * @return 滤波后油压
 */
float UnifiedFilter_GetFilteredOilPressure(void);

/*!
 * @brief 获取滤波后LNG压力
 * @return 滤波后LNG压力
 */
float UnifiedFilter_GetFilteredLNGPressure(void);

/*!
 * @brief 获取所有滤波后数据
 * @param data 统一传感器数据结构体指针
 */
void UnifiedFilter_GetAllFilteredData(unified_sensor_data_t* data);

/* ==================== 滤波算法接口 ==================== */

/*!
 * @brief 移动平均滤波
 * @param buffer 滤波缓冲区
 * @param index 当前索引
 * @param window_size 窗口大小
 * @param new_value 新值
 * @return 滤波后值
 */
float UnifiedFilter_MovingAverage(float* buffer, uint8_t* index, 
                                 uint8_t window_size, float new_value);

/*!
 * @brief 低通滤波
 * @param last_filtered 上次滤波值
 * @param new_value 新值
 * @param alpha 滤波系数
 * @return 滤波后值
 */
float UnifiedFilter_LowPass(float last_filtered, float new_value, float alpha);

/*!
 * @brief 指数滤波
 * @param last_filtered 上次滤波值
 * @param new_value 新值
 * @param alpha 滤波系数
 * @return 滤波后值
 */
float UnifiedFilter_Exponential(float last_filtered, float new_value, float alpha);

/* ==================== 滤波状态管理 ==================== */

/*!
 * @brief 重置滤波缓冲区
 */
void UnifiedFilter_Reset(void);

/*!
 * @brief 检查滤波是否就绪
 * @return true: 就绪, false: 未就绪
 */
bool UnifiedFilter_IsReady(void);

/*!
 * @brief 获取滤波统计信息
 * @return 滤波次数
 */
uint32_t UnifiedFilter_GetFilterCount(void);

/*!
 * @brief 打印滤波状态
 */
void UnifiedFilter_PrintStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* UNIFIED_FILTER_H */
