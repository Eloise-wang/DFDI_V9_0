/**
 * @file pt1000.h
 * @brief PT1000温度传感器模块 - 基于IEC 60751标准
 * 
 * 温度范围：-200°C 到 +100°C
 * 温度步长：1°C
 * 基于IEC 60751标准生成
 * 包含电阻查找表和温度计算功能
 */

#ifndef PT1000_H
#define PT1000_H

#include <stdint.h>
#include "common_types.h"


/**
 * @brief PT1000电阻查找表（浮点版本）从-200°C到+100°C
 * 基于IEC 60751标准，每1°C一个数据点
 */
const float* pt1000_get_lut_float(void);

/**
 * @brief PT1000电阻查找表（定点整数版本）从-200°C到+100°C
 * 注意：电阻值已放大100倍
 */
const uint32_t* pt1000_get_lut_int(void);

/* ==========================================  Functions  =========================================== */

/**
 * @brief 根据测量的电阻值计算温度
 * @param resistance 测得的PT1000电阻值 (单位: Ohm)
 * @return float     计算出的温度值 (单位: °C)
 *                   如果电阻值超出查找表范围，返回 PT1000_RESISTANCE_OUT_OF_RANGE_F
 */
float pt1000_get_temp_float(float resistance);

/**
 * @brief 电压转PT1000电阻值（基于实测标定数据）
 * @param voltage 输入电压 (单位: V)
 * @return float  计算出的PT1000电阻值 (单位: Ohm)
 */
float pt1000_voltage_to_resistance(float voltage);

/**
 * @brief 根据ADC原始值计算温度（适配放大倍数=10的电路）
 * @param adc_raw    ADC原始采样值
 * @param adc_ref_mv ADC参考电压 (单位: mV)
 * @param adc_bits   ADC分辨率位数 (如12位ADC则为12)
 * @return float     计算出的温度值 (单位: °C)
 *                   如果计算出的电阻值超出查找表范围，返回 PT1000_RESISTANCE_OUT_OF_RANGE_F
 */
float pt1000_adc_to_temp_float(uint32_t adc_raw, float adc_ref_mv, uint8_t adc_bits);

/**
 * @brief 根据电压直接计算温度
 * @param voltage_v  输入电压 (单位: V)
 * @return float     计算出的温度值 (单位: °C)
 */
float pt1000_voltage_to_temp_float(float voltage_v);

/**
 * @brief 获取PT1000电阻查找表（浮点版本）
 * @return const float* 电阻查找表指针
 */
const float* pt1000_get_resistance_lut(void);

#endif
