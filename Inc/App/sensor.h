/**
 * @file sensor.h
 * @brief 统一传感器模块 - 整合所有传感器相关功能
 * 
 * 功能模块：
 * - ADC硬件驱动和配置
 * - 传感器数据采集和转换
 * - 温度传感器标定和校准
 * - 传感器监控和故障诊断
 * - 数据滤波和有效性验证
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include "common_types.h"
#include "ac7840x.h"

/* ============================================  Define  ============================================ */

// ADC通道定义 - 已移动到common_types.h中统一管理
// 温度传感器校准参数 - 已移动到common_types.h中统一管理



/* ===========================================  Typedef  ============================================ */

// 传感器数据结构体已移动到common_types.h中统一管理

// 传感器故障状态枚举已移动到common_types.h中统一管理


/* ==========================================  Functions  =========================================== */

/* ==================== 初始化接口 ==================== */

/**
 * @brief 传感器模块初始化
 */
void Sensor_Init(void);

/* ==================== ADC硬件驱动接口 ==================== */

/**
 * @brief 初始化ADC1 - 支持四个传感器通道
 */
void Sensor_InitADC(void);

/**
 * @brief 获取指定通道的ADC值
 * @param channel ADC通道 (0-3)
 * @return ADC转换值 (0-4095)
 */
uint16_t Sensor_GetADCValue(uint8_t channel);

/**
 * @brief 获取所有传感器的ADC原始值
 * @param raw_values 存储ADC原始值的数组
 */
void Sensor_GetAllADCValues(uint16_t raw_values[ADC_CHANNEL_COUNT]);

/* ==================== 数据转换接口 ==================== */

/**
 * @brief ADC原始值转换为电压
 * @param adc_raw ADC原始值 (0-4095)
 * @return 电压值 (V)
 */
float Sensor_ConvertAdcToVoltage(uint16_t adc_raw);

/**
 * @brief ADC原始值直接转换为油压
 * @param adc_raw ADC原始值 (0-4095)
 * @return 油压 (MPa)
 */
float Sensor_ADCToOilPressure(uint16_t adc_raw);

/**
 * @brief ADC原始值直接转换为LNG压力
 * @param adc_raw ADC原始值 (0-4095)
 * @return LNG压力 (MPa)
 */
float Sensor_ADCToLNGPressure(uint16_t adc_raw);

/**
 * @brief ADC原始值直接转换为油温（使用PT1000标定）
 * @param adc_raw ADC原始值 (0-4095)
 * @return 油温 (°C)
 */
float Sensor_ADCToOilTemperature(uint16_t adc_raw);

/**
 * @brief ADC原始值直接转换为LNG温度（使用PT1000标定）
 * @param adc_raw ADC原始值 (0-4095)
 * @return LNG温度 (°C)
 */
float Sensor_ADCToLNGTemperature(uint16_t adc_raw);

/* ==================== 温度校准接口 ==================== */

/**
 * @brief 获取油温传感器校准偏移量
 * @return 校准偏移量 (°C)
 */
float Sensor_GetOilTempCalibrationOffset(void);

/**
 * @brief 获取LNG温度传感器校准偏移量
 * @return 校准偏移量 (°C)
 */
float Sensor_GetLNGTempCalibrationOffset(void);

/**
 * @brief 设置油温传感器校准偏移量
 * @param offset 校准偏移量 (°C)
 */
void Sensor_SetOilTempCalibrationOffset(float offset);

/**
 * @brief 设置LNG温度传感器校准偏移量
 * @param offset 校准偏移量 (°C)
 */
void Sensor_SetLNGTempCalibrationOffset(float offset);

/**
 * @brief 验证温度标定精度
 * @param sensor_type 传感器类型 (0: 油温, 1: LNG温度)
 * @return 标定精度验证结果
 */
bool Sensor_ValidateTemperatureCalibration(uint8_t sensor_type);

/* ==================== 传感器监控接口 ==================== */

/**
 * @brief 传感器监控初始化
 */
void Sensor_InitMonitor(void);

/**
 * @brief 更新所有传感器数据
 */
void Sensor_UpdateMonitor(void);

/**
 * @brief 获取传感器监控数据
 * @return 传感器监控数据指针
 */
const sensor_data_t* Sensor_GetMonitorData(void);

/**
 * @brief 获取油温
 * @return 油温(°C)
 */
float Sensor_GetOilTemperature(void);

/**
 * @brief 获取LNG温度
 * @return LNG温度(°C)
 */
float Sensor_GetLNGTemperature(void);

/**
 * @brief 获取油压
 * @return 油压(MPa)
 */
float Sensor_GetOilPressure(void);

/**
 * @brief 获取LNG压力
 * @return LNG压力(MPa)
 */
float Sensor_GetLNGPressure(void);

// 已删除：Sensor_NeedCooling() - 控制逻辑，移至PC端
// 已删除：Sensor_IsLNGPressureInRange() - 控制逻辑，移至PC端

/**
 * @brief 检查传感器数据有效性
 * @return true: 有效, false: 无效
 */
bool Sensor_CheckDataValidity(void);

/**
 * @brief 获取传感器故障状态
 * @return 故障状态
 */
sensor_fault_status_t Sensor_GetFaultStatus(void);

/**
 * @brief 传感器故障诊断
 */
void Sensor_FaultDiagnosis(void);

/**
 * @brief 验证传感器数值
 * @param value 传感器数值
 * @param min_valid 最小有效值
 * @param max_valid 最大有效值
 * @return true: 有效, false: 无效
 */
bool Sensor_ValidateValue(float value, float min_valid, float max_valid);

#endif 
