/**
 * @file sensor.c
 * @brief 统一传感器模块实现 - 整合所有传感器相关功能
 * 
 */

#include "sensor.h"
#include "pt1000.h"
#include "adc_drv.h"
#include "gpio_drv.h"
#include "ckgen_drv.h"
#include "osif.h"
#include "unified_filter.h"
#include <string.h>

/* ==========================================  Variables  =========================================== */

// ADC配置结构体
static adc_converter_config_t adcConfig;
static adc_chan_config_t adcChanConfig;



// 传感器监控数据
static sensor_monitor_t g_sensor_monitor;

// 温度校准偏移量
static float g_oil_temp_calibration_offset = OIL_TEMP_CALIBRATION_OFFSET;
static float g_lng_temp_calibration_offset = LNG_TEMP_CALIBRATION_OFFSET;

// 滤波缓冲区
static float g_filter_buffer[ADC_CHANNEL_COUNT][MONITOR_FILTER_SIZE];
static uint8_t g_filter_index[ADC_CHANNEL_COUNT] = {0};

/* ==========================================  Functions  =========================================== */

// 基础ADC功能
void Sensor_Init(void) {
    Sensor_InitADC();
    Sensor_InitMonitor();
    UnifiedFilter_Init();  // 初始化统一滤波管理器
}

void Sensor_InitADC(void) {
    // 启用ADC1时钟
    CKGEN_DRV_Enable(CLK_ADC1, true);
    
    // 配置GPIO引脚为ADC功能
    gpio_settings_config_t gpioConfigs[ADC_CHANNEL_COUNT] = {
        // LNG温度传感器 PA2_ADC1_IN0_T (物理引脚48) - 通道0
        {
            .base = PORTA,
            .pinPortIdx = 2U,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .mux = PORT_PIN_DISABLED,  // ADC功能
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = true,
            .digitalFilter = false
        },
        // 油温传感器 PA3_ADC1_IN1_LNG_T (物理引脚47) - 通道1
        {
            .base = PORTA,
            .pinPortIdx = 3U,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .mux = PORT_PIN_DISABLED,  // ADC功能
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = true,
            .digitalFilter = false
        },
        // 油压传感器 PD2_ADC1_IN2_P (物理引脚46) - 通道2
        {
            .base = PORTD,
            .pinPortIdx = 2U,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .mux = PORT_PIN_DISABLED,  // ADC功能
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = true,
            .digitalFilter = false
        },
        // LNG压力传感器 PD3_ADC1_IN3_LNG_P (物理引脚45) - 通道3
        {
            .base = PORTD,
            .pinPortIdx = 3U,
            .pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
            .driveSelect = PORT_LOW_DRIVE_STRENGTH,
            .mux = PORT_PIN_DISABLED,  // ADC功能
            .pinLock = false,
            .intConfig = PORT_DMA_INT_DISABLED,
            .clearIntFlag = true,
            .digitalFilter = false
        }
    };
    
    for(uint8_t i = 0; i < ADC_CHANNEL_COUNT; i++)
    {
        GPIO_DRV_Init(1U, &gpioConfigs[i]);
    }
    
    // 初始化ADC配置结构体
    ADC_DRV_InitConverterStruct(&adcConfig);
    
    // 配置ADC转换器参数
    adcConfig.clockDivide = ADC_CLK_DIVIDE_1;
    adcConfig.resolution = ADC_RESOLUTION_12BIT;
    adcConfig.alignment = ADC_DATA_ALIGN_RIGHT;
    adcConfig.regularTrigger = ADC_TRIGGER_INTERNAL;
    adcConfig.injectTrigger = ADC_TRIGGER_INTERNAL;
    adcConfig.dmaEnable = false;
    adcConfig.voltageRef = ADC_VOLTAGEREF_VREF;
    adcConfig.scanModeEn = false;  // 单通道模式
    adcConfig.continuousModeEn = false;
    adcConfig.regularDiscontinuousModeEn = false;
    adcConfig.injectDiscontinuousModeEn = false;
    adcConfig.injectAutoModeEn = false;
    adcConfig.intervalModeEn = false;
    adcConfig.regularSequenceLength = 1;
    adcConfig.injectSequenceLength = 0;
    adcConfig.callback = NULL;
    adcConfig.parameter = NULL;
    adcConfig.powerEn = true;
    
    // 初始化ADC
    ADC_DRV_Init(1U);
    
    // 配置ADC转换器
    ADC_DRV_ConfigConverter(1U, &adcConfig);
    
    // 初始化通道配置结构体
    ADC_DRV_InitChanStruct(&adcChanConfig);
    
    // 配置通道参数
    adcChanConfig.spt = ADC_SPT_CLK_23;
    adcChanConfig.interruptEn = false;
}

uint16_t Sensor_GetADCValue(uint8_t channel) {
    if (channel >= ADC_CHANNEL_COUNT) {
        return 0;
    }
    
    uint16_t adcValue = 0;
    
    // 配置当前通道
    adcChanConfig.channel = (adc_inputchannel_t)channel;
    
    // 配置ADC通道序列
    ADC_DRV_ConfigChan(1U, ADC_RSEQ_0, &adcChanConfig);
    
    // 添加稳定延时
    for(volatile uint32_t delay = 0; delay < 5000; delay++);
    
    // 启动ADC转换
    ADC_DRV_SoftwareStartRegularConvert(1U);
    
    // 等待转换完成
    uint32_t timeout = 0;
    while (!ADC_DRV_GetConvCompleteFlag(1U, ADC_RSEQ_0))
    {
        timeout++;
        if (timeout > 50000) {
            return 0; // 超时返回0
        }
    }
    
    // 获取转换结果
    ADC_DRV_GetSeqResult(1U, ADC_RSEQ_0, &adcValue);
    
    // 清除转换完成标志
    ADC_DRV_ClearConvCompleteFlag(1U, ADC_RSEQ_0);
    
    return adcValue;
}

void Sensor_GetAllADCValues(uint16_t raw_values[ADC_CHANNEL_COUNT]) {
    for (uint8_t i = 0; i < ADC_CHANNEL_COUNT; i++) {
        raw_values[i] = Sensor_GetADCValue(i);
    }
}

// 数据转换功能
float Sensor_ConvertAdcToVoltage(uint16_t adc_raw)
{
    // 将ADC原始值转换为电压值
    return (float)adc_raw * ADC_REFERENCE_VOLTAGE / ADC_MAX_VALUE;
}

float Sensor_ConvertVoltageToPressure(float voltage)
{
    // 电压转压力转换函数
    if (voltage < VOLTAGE_MIN_VALID) {
        return 0.0f; // 电压过低，返回0MPa
    }
    
    // 简单的线性转换（实际应用中可能需要更复杂的转换公式）
    float pressure = voltage * 10.0f; // 假设10V对应100MPa
    
    if (pressure < 0.0f) pressure = 0.0f;
    
    if (voltage > VOLTAGE_MAX_VALID) {
        return PRESSURE_MAX_OUTPUT_MPA; // 返回最大压力值
    }
    
    return pressure;
}

/**
 * @brief 电压转换为油压（4-20mA对应0.8-4V，对应0-40MPa）
 * @param voltage 电压值 (V)
 * @return 油压 (MPa)
 */
static float Voltage_To_Oil_Pressure(float voltage)
{
    // 4-20mA电流信号转换为压力
    // 4mA对应0.8V，20mA对应4V
    // 4mA对应0MPa，20mA对应40MPa
    // 线性关系：P = (V - 0.8) * 40.0 / (4.0 - 0.8) = (V - 0.8) * 12.5
    
    // 检查电压是否在合理范围内
    if (voltage < 0.0f) {
        return 0.0f; // 电压过低，返回0MPa
    }
    
    if (voltage < 0.8f) {
        // 电压低于0.8V，可能是传感器未连接或故障
        return 0.0f;
    }
    
    if (voltage > 4.0f) {
        // 电压超过4V，可能是传感器故障或短路
        return 40.0f; // 返回最大压力值
    }
    
    float pressure = (voltage - 0.8f) * 12.5f;
    
    if (pressure < 0.0f) pressure = 0.0f;
    if (pressure > 40.0f) pressure = 40.0f;
    
    return pressure;
}

/**
 * @brief 电压转换为LNG压力（4-20mA对应0.8-4V，对应0-35MPa）
 * @param voltage 电压值 (V)
 * @return LNG压力 (MPa)
 */
static float Voltage_To_LNG_Pressure(float voltage)
{
    // 4-20mA电流信号转换为压力
    // 4mA对应0.8V，20mA对应4V
    // 4mA对应0MPa，20mA对应35MPa
    // 线性关系：P = (V - 0.8) * 35.0 / (4.0 - 0.8) = (V - 0.8) * 10.9375
    
    // 检查电压是否在合理范围内
    if (voltage < 0.0f) {
        return 0.0f; // 电压过低，返回0MPa
    }
    
    if (voltage < 0.8f) {
        // 电压低于0.8V，可能是传感器未连接或故障
        return 0.0f;
    }
    
    if (voltage > 4.0f) {
        // 电压超过4V，可能是传感器故障或短路
        return 35.0f; // 返回最大压力值
    }
    
    float pressure = (voltage - 0.8f) * 10.9375f;
    
    if (pressure < 0.0f) pressure = 0.0f;
    if (pressure > 35.0f) pressure = 35.0f;
    
    return pressure;
}

float Sensor_ADCToOilPressure(uint16_t adc_raw) {
    float voltage = Sensor_ConvertAdcToVoltage(adc_raw);
    return Voltage_To_Oil_Pressure(voltage);
}

float Sensor_ADCToLNGPressure(uint16_t adc_raw) {
    float voltage = Sensor_ConvertAdcToVoltage(adc_raw);
    return Voltage_To_LNG_Pressure(voltage);
}

/**
 * @brief 通用温度转换函数（使用PT1000标定）
 * @param voltage 电压值 (V)
 * @return 温度 (°C)
 */
static float Voltage_To_Temperature_Common(float voltage)
{
    // 使用PT1000标定算法计算温度
    float temperature = pt1000_voltage_to_temp_float(voltage);
    
    // 如果标定失败，返回默认值
    if (temperature == PT1000_RESISTANCE_OUT_OF_RANGE_F) {
        return 25.0f; // 默认25°C
    }
    
    // 限制最低温度为-40°C
    if (temperature < -40.0f) {
        temperature = -40.0f;
    }
    
    return temperature;
}

/**
 * @brief ADC原始值直接转换为油温（使用PT1000标定）
 * @param adc_raw ADC原始值 (0-4095)
 * @return 油温 (°C)
 */
float Sensor_ADCToOilTemperature(uint16_t adc_raw) {
    float voltage = Sensor_ConvertAdcToVoltage(adc_raw);
    float temperature = Voltage_To_Temperature_Common(voltage);
    return temperature + g_oil_temp_calibration_offset;
}

/**
 * @brief ADC原始值直接转换为LNG温度（使用PT1000标定）
 * @param adc_raw ADC原始值 (0-4095)
 * @return LNG温度 (°C)
 */
float Sensor_ADCToLNGTemperature(uint16_t adc_raw) {
    float voltage = Sensor_ConvertAdcToVoltage(adc_raw);
    float temperature = Voltage_To_Temperature_Common(voltage);
    return temperature + g_lng_temp_calibration_offset;
}

// 温度校准功能
float Sensor_GetOilTempCalibrationOffset(void) {
    return g_oil_temp_calibration_offset;
}

float Sensor_GetLNGTempCalibrationOffset(void) {
    return g_lng_temp_calibration_offset;
}

void Sensor_SetOilTempCalibrationOffset(float offset) {
    g_oil_temp_calibration_offset = offset;
}

void Sensor_SetLNGTempCalibrationOffset(float offset) {
    g_lng_temp_calibration_offset = offset;
}

bool Sensor_ValidateTemperatureCalibration(uint8_t sensor_type) {
    // 验证温度校准精度
    if (sensor_type == 0) { // 油温传感器
        float offset = g_oil_temp_calibration_offset;
        return (offset >= -10.0f && offset <= 10.0f);
    } else if (sensor_type == 1) { // LNG温度传感器
        float offset = g_lng_temp_calibration_offset;
        return (offset >= -10.0f && offset <= 10.0f);
    }
    return false;
}

// 传感器监控功能
void Sensor_InitMonitor(void) {
    memset(&g_sensor_monitor, 0, sizeof(g_sensor_monitor));
    
    // 初始化滤波缓冲区
    memset(g_filter_buffer, 0, sizeof(g_filter_buffer));
    memset(g_filter_index, 0, sizeof(g_filter_index));
    
    // 初始化有效性状态
    g_sensor_monitor.validity.oil_temp_valid = true;
    g_sensor_monitor.validity.lng_temp_valid = true;
    g_sensor_monitor.validity.oil_pressure_valid = true;
    g_sensor_monitor.validity.lng_pressure_valid = true;
    g_sensor_monitor.validity.last_valid_time = 0;
}

void Sensor_UpdateMonitor(void) {
    // 获取ADC原始值
    uint16_t adc_raw_values[ADC_CHANNEL_COUNT];
    Sensor_GetAllADCValues(adc_raw_values);
    
    // 更新原始数据
    g_sensor_monitor.raw_data.adc_raw[ADC_CHANNEL_OIL_TEMP] = adc_raw_values[ADC_CHANNEL_OIL_TEMP];
    g_sensor_monitor.raw_data.adc_raw[ADC_CHANNEL_LNG_TEMP] = adc_raw_values[ADC_CHANNEL_LNG_TEMP];
    g_sensor_monitor.raw_data.adc_raw[ADC_CHANNEL_OIL_PRESSURE] = adc_raw_values[ADC_CHANNEL_OIL_PRESSURE];
    g_sensor_monitor.raw_data.adc_raw[ADC_CHANNEL_LNG_PRESSURE] = adc_raw_values[ADC_CHANNEL_LNG_PRESSURE];
    
    // 转换为电压值
    g_sensor_monitor.raw_data.voltage[ADC_CHANNEL_OIL_TEMP] = Sensor_ConvertAdcToVoltage(adc_raw_values[ADC_CHANNEL_OIL_TEMP]);
    g_sensor_monitor.raw_data.voltage[ADC_CHANNEL_LNG_TEMP] = Sensor_ConvertAdcToVoltage(adc_raw_values[ADC_CHANNEL_LNG_TEMP]);
    g_sensor_monitor.raw_data.voltage[ADC_CHANNEL_OIL_PRESSURE] = Sensor_ConvertAdcToVoltage(adc_raw_values[ADC_CHANNEL_OIL_PRESSURE]);
    g_sensor_monitor.raw_data.voltage[ADC_CHANNEL_LNG_PRESSURE] = Sensor_ConvertAdcToVoltage(adc_raw_values[ADC_CHANNEL_LNG_PRESSURE]);
    
    // 转换为物理量
    g_sensor_monitor.raw_data.oil_temp_celsius = Sensor_ADCToOilTemperature(adc_raw_values[ADC_CHANNEL_OIL_TEMP]);
    g_sensor_monitor.raw_data.lng_temp_celsius = Sensor_ADCToLNGTemperature(adc_raw_values[ADC_CHANNEL_LNG_TEMP]);
    g_sensor_monitor.raw_data.oil_pressure_mpa = Sensor_ADCToOilPressure(adc_raw_values[ADC_CHANNEL_OIL_PRESSURE]);
    g_sensor_monitor.raw_data.lng_pressure_mpa = Sensor_ADCToLNGPressure(adc_raw_values[ADC_CHANNEL_LNG_PRESSURE]);
    
    // 使用统一滤波管理器更新数据
    UnifiedFilter_UpdateData(
        g_sensor_monitor.raw_data.oil_temp_celsius,
        g_sensor_monitor.raw_data.lng_temp_celsius,
        g_sensor_monitor.raw_data.oil_pressure_mpa,
        g_sensor_monitor.raw_data.lng_pressure_mpa
    );
    
    // 获取滤波后的数据
    g_sensor_monitor.filtered_data.oil_temp_celsius = UnifiedFilter_GetFilteredOilTemperature();
    g_sensor_monitor.filtered_data.lng_temp_celsius = UnifiedFilter_GetFilteredLNGTemperature();
    g_sensor_monitor.filtered_data.oil_pressure_mpa = UnifiedFilter_GetFilteredOilPressure();
    g_sensor_monitor.filtered_data.lng_pressure_mpa = UnifiedFilter_GetFilteredLNGPressure();
    
    // 更新工程量程有效性（用于上电自检和 device_status.debug 标志）
    g_sensor_monitor.validity.oil_pressure_valid =
        Sensor_ValidateValue(g_sensor_monitor.filtered_data.oil_pressure_mpa,
                             OIL_PRESSURE_MEAS_MIN_MPA, OIL_PRESSURE_MEAS_MAX_MPA);
    g_sensor_monitor.validity.lng_pressure_valid =
        Sensor_ValidateValue(g_sensor_monitor.filtered_data.lng_pressure_mpa,
                             LNG_PRESSURE_MEAS_MIN_MPA, LNG_PRESSURE_MEAS_MAX_MPA);
    g_sensor_monitor.validity.oil_temp_valid =
        Sensor_ValidateValue(g_sensor_monitor.filtered_data.oil_temp_celsius,
                             OIL_TEMP_MEAS_MIN_C, OIL_TEMP_MEAS_MAX_C);
    g_sensor_monitor.validity.lng_temp_valid =
        Sensor_ValidateValue(g_sensor_monitor.filtered_data.lng_temp_celsius,
                             LNG_TEMP_MEAS_MIN_C, LNG_TEMP_MEAS_MAX_C);
    
    // 更新冷却需求（仍使用警告高值作为依据）
    g_sensor_monitor.cooling_needed = (g_sensor_monitor.filtered_data.oil_temp_celsius > OIL_TEMP_WARNING_HIGH);
    
    // 更新故障状态
    Sensor_FaultDiagnosis();
    
    // 更新时间戳
    g_sensor_monitor.last_update_time = OSIF_GetMilliseconds();
}

const sensor_data_t* Sensor_GetMonitorData(void) {
    return &g_sensor_monitor.filtered_data;
}

float Sensor_GetOilTemperature(void) {
    return g_sensor_monitor.filtered_data.oil_temp_celsius;
}

float Sensor_GetLNGTemperature(void) {
    return g_sensor_monitor.filtered_data.lng_temp_celsius;
}

float Sensor_GetOilPressure(void) {
    return g_sensor_monitor.filtered_data.oil_pressure_mpa;
}

float Sensor_GetLNGPressure(void) {
    return g_sensor_monitor.filtered_data.lng_pressure_mpa;
}


bool Sensor_CheckDataValidity(void) {
    return g_sensor_monitor.validity.oil_temp_valid && 
           g_sensor_monitor.validity.lng_temp_valid && 
           g_sensor_monitor.validity.oil_pressure_valid && 
           g_sensor_monitor.validity.lng_pressure_valid;
}

sensor_fault_status_t Sensor_GetFaultStatus(void) {
    return g_sensor_monitor.fault_status;
}

void Sensor_FaultDiagnosis(void) {
    g_sensor_monitor.fault_status = SENSOR_FAULT_NONE;
    
    // 检查油温传感器故障
    if (!Sensor_ValidateValue(g_sensor_monitor.filtered_data.oil_temp_celsius, 
                             SENSOR_VALUE_MIN_VALID, SENSOR_VALUE_MAX_VALID)) {
        g_sensor_monitor.fault_status |= SENSOR_FAULT_OIL_TEMP;
    }
    
    // 检查LNG温度传感器故障
    if (!Sensor_ValidateValue(g_sensor_monitor.filtered_data.lng_temp_celsius, 
                             SENSOR_VALUE_MIN_VALID, SENSOR_VALUE_MAX_VALID)) {
        g_sensor_monitor.fault_status |= SENSOR_FAULT_LNG_TEMP;
    }
    
    // 检查油压传感器故障
    if (!Sensor_ValidateValue(g_sensor_monitor.filtered_data.oil_pressure_mpa, 
                             SENSOR_VALUE_MIN_VALID, SENSOR_VALUE_MAX_VALID)) {
        g_sensor_monitor.fault_status |= SENSOR_FAULT_OIL_PRESSURE;
    }
    
    // 检查LNG压力传感器故障
    if (!Sensor_ValidateValue(g_sensor_monitor.filtered_data.lng_pressure_mpa, 
                             SENSOR_VALUE_MIN_VALID, SENSOR_VALUE_MAX_VALID)) {
        g_sensor_monitor.fault_status |= SENSOR_FAULT_LNG_PRESSURE;
    }
}

bool Sensor_ValidateValue(float value, float min_valid, float max_valid) {
    return (value >= min_valid) && (value <= max_valid);
}
