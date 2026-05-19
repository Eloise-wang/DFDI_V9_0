/*!
 * @file can_message_handler.c
 *
 * @brief CAN消息处理模块实现
 *
 * 功能说明：
 * - 接收PC端参数配置消息（tsmaster_control, tsmaster_control2）
 * - 发送传感器数据到PC端（device_status）
 * - CAN通信状态监控
 * - 参数配置存储和访问接口
 */

#include "can_message_handler.h"
#include "can_config.h"
#include "gcu_control_dbc.h"
#include "sensor.h"
#include "valve_control.h"
#include "hydraulic_control.h"
#include "debug_log.h"
#include <string.h>

/* ==========================================  Variables  =========================================== */

// PC端参数配置（从CAN接收，用于嵌入式端控制算法）
static pc_parameter_config_t g_pc_parameters = {0};

// CAN接收消息计数
static uint32_t g_can_rx_message_count = 0;

// 系统使能状态（外部访问）
extern bool g_systemEnabled;

static float g_filtered_lng_pressure = 0.0f; 	// 存储滤波后的罐压物理值

/* ==========================================  Functions  =========================================== */

// 新增：供外部获取滤波后的罐压接口
float CAN_MessageHandler_GetFilteredLNGPressure(void)
{
    return g_filtered_lng_pressure;
}

/*!
 * @brief 初始化CAN消息处理模块
 */
void CAN_MessageHandler_Init(void)
{
    memset(&g_pc_parameters, 0, sizeof(pc_parameter_config_t));
    g_can_rx_message_count = 0;
}

/*!
 * @brief CAN接收回调函数
 * @param msg_id CAN消息ID
 * @param data 数据指针
 * @param length 数据长度
 */
void CAN_MessageHandler_RxCallback(uint32_t msg_id, const uint8_t* data, uint8_t length)
{
    // 增加接收消息计数
    g_can_rx_message_count++;
    
    // 使用29位扩展ID掩码进行匹配（扩展ID格式：0x1FFFFFFF）
    uint32_t msg_id_masked = msg_id & 0x1FFFFFFF;
    uint32_t expected_control_id = CAN_MSG_TSMASTER_CONTROL_ID & 0x1FFFFFFF;
    uint32_t expected_control2_id = CAN_MSG_TSMASTER_CONTROL2_ID & 0x1FFFFFFF;
    uint32_t device_status_id = CAN_MSG_DEVICE_STATUS_ID & 0x1FFFFFFF;
    
    // 忽略自己发送的device_status消息
    if (msg_id_masked == device_status_id) {
        return;
    }
    
    // 处理tsmaster_control消息 (ID: 0x98080100)
    if (msg_id_masked == expected_control_id && length == 8) {
        tsmaster_control_t ctrl_msg;
        
        /* 解包CAN消息 */
        if (tsmaster_control_unpack(&ctrl_msg, data, length) == 0) {
            // 存储参数配置（供嵌入式端控制算法使用）
            g_pc_parameters.set_bypass_initial_decline_time = tsmaster_control_set_bypass_initial_decline_time_decode(ctrl_msg.set_bypass_initial_decline_time);
            g_pc_parameters.set_rev_start_oilP_max = tsmaster_control_set_rev_start_oilP_max_decode(ctrl_msg.set_rev_start_oilP_max);
            g_pc_parameters.set_rev_start_oilP_min = tsmaster_control_set_rev_start_oilP_min_decode(ctrl_msg.set_rev_start_oilP_min);
            g_pc_parameters.set_first_fix_freq_time_on = tsmaster_control_set_first_fix_freq_time_on_decode(ctrl_msg.set_first_fix_freq_time_on);
            g_pc_parameters.set_first_fix_freq_time_off = tsmaster_control_set_first_fix_freq_time_off_decode(ctrl_msg.set_first_fix_freq_time_off);
            g_pc_parameters.bypass_off = (ctrl_msg.bypass_off != 0u);
            
            // 打印接收到的参数
            Debug_Log_CANReceivedParameters(&g_pc_parameters);
            
            // 更新参数到 hydraulic_control 模块
            control_params_t control_params;
            control_params.system_enable = g_pc_parameters.system_enable;
            control_params.auto_mode = g_pc_parameters.auto_mode;
            control_params.set_bypass_ratio = (float)g_pc_parameters.set_bypass_ratio;
            control_params.set_bypass_initial_decline_time = (float)g_pc_parameters.set_bypass_initial_decline_time;
            control_params.set_rev_start_oilP_max = (float)g_pc_parameters.set_rev_start_oilP_max;
            control_params.set_rev_start_oilP_min = (float)g_pc_parameters.set_rev_start_oilP_min;
            control_params.set_first_fix_freq_time_on = (float)g_pc_parameters.set_first_fix_freq_time_on;
            control_params.set_first_fix_freq_time_off = (float)g_pc_parameters.set_first_fix_freq_time_off;
            control_params.set_extend_time = (float)g_pc_parameters.set_extend_time;
            control_params.set_extend_pressure = (float)g_pc_parameters.set_extend_pressure;
            control_params.set_retract_time = (float)g_pc_parameters.set_retract_time;
            control_params.set_retract_pressure = (float)g_pc_parameters.set_retract_pressure;
            control_params.set_cooler_temperature_on = (float)g_pc_parameters.set_cooler_temp_on;
            control_params.set_cooler_temperature_off = (float)g_pc_parameters.set_cooler_temp_off;
            control_params.bypass_off = g_pc_parameters.bypass_off;
						Debug_Log_CANReceivedParameters(&g_pc_parameters);
            HydraulicControl_UpdateParams(&control_params);
        } else {
            // 打印解包失败信息（带频率控制，每100次打印一次）
            Debug_Log_CANUnpackFailed(msg_id);
        }
    }
    // 处理tsmaster_control2消息 (ID: 0x98080108)
    else if (msg_id_masked == expected_control2_id && length == 8) {
        tsmaster_control2_t ctrl2_msg;
        
        /* 解包CAN消息 */
        if (tsmaster_control2_unpack(&ctrl2_msg, data, length) == 0) {
            // 系统使能控制（优先处理）
            bool system_enable = (ctrl2_msg.system_enable == 1);
            
            // 打印系统使能状态变化（带状态管理）
            Debug_Log_CANSystemEnableChanged(system_enable, ctrl2_msg.system_enable);
            
            if (!system_enable) {
                // 系统禁用，安全关闭所有执行器
                g_pc_parameters.system_enable = false;
                ValveControl_SetBypassValve(0.0f);
                ValveControl_SetDirectionalValve(false);
                ValveControl_SetCooler(false);
                g_systemEnabled = false;
                HydraulicControl_SetSystemEnable(false);
            } else {
                g_systemEnabled = true;
                HydraulicControl_SetSystemEnable(true);

                // 存储参数配置（供嵌入式端控制算法使用）
                g_pc_parameters.system_enable = true;
                g_pc_parameters.set_bypass_ratio = tsmaster_control2_set_bypass_ratio_decode(ctrl2_msg.set_bypass_ratio);
                if (g_pc_parameters.set_bypass_ratio < 0.0) g_pc_parameters.set_bypass_ratio = 0.0;
                if (g_pc_parameters.set_bypass_ratio > 50.0) g_pc_parameters.set_bypass_ratio = 50.0;
                
                g_pc_parameters.auto_mode = (tsmaster_control2_auto_mode_decode(ctrl2_msg.auto_mode) != 0.0);
                
                g_pc_parameters.set_extend_time = tsmaster_control2_set_extend_time_decode(ctrl2_msg.set_extend_time);
                if (g_pc_parameters.set_extend_time < 0.0) g_pc_parameters.set_extend_time = 0.0;
                if (g_pc_parameters.set_extend_time > 6.3) g_pc_parameters.set_extend_time = 6.3;
                
                g_pc_parameters.set_extend_pressure = tsmaster_control2_set_extend_pressure_decode(ctrl2_msg.set_extend_pressure);
                if (g_pc_parameters.set_extend_pressure < 0.0) g_pc_parameters.set_extend_pressure = 0.0;
                if (g_pc_parameters.set_extend_pressure > 25.5) g_pc_parameters.set_extend_pressure = 25.5;
                
                g_pc_parameters.set_retract_pressure = tsmaster_control2_set_retract_pressure_decode(ctrl2_msg.set_retract_pressure);
                if (g_pc_parameters.set_retract_pressure < 0.0) g_pc_parameters.set_retract_pressure = 0.0;
                if (g_pc_parameters.set_retract_pressure > 12.7) g_pc_parameters.set_retract_pressure = 12.7;
                
                g_pc_parameters.set_retract_time = tsmaster_control2_set_retract_time_decode(ctrl2_msg.set_retract_time);
                if (g_pc_parameters.set_retract_time < 0.0) g_pc_parameters.set_retract_time = 0.0;
                if (g_pc_parameters.set_retract_time > 3.1) g_pc_parameters.set_retract_time = 3.1;
                
                g_pc_parameters.set_cooler_temp_on = tsmaster_control2_set_cooler_temperature_on_decode(ctrl2_msg.set_cooler_temperature_on);
                g_pc_parameters.set_cooler_temp_off = tsmaster_control2_set_cooler_temperature_off_decode(ctrl2_msg.set_cooler_temperature_off);
                
                // 打印接收到的参数
                Debug_Log_CANReceivedParameters(&g_pc_parameters);
                
                // 更新参数到 hydraulic_control 模块
                control_params_t control_params;
                control_params.system_enable = g_pc_parameters.system_enable;
                control_params.auto_mode = g_pc_parameters.auto_mode;
                control_params.set_bypass_ratio = (float)g_pc_parameters.set_bypass_ratio;
                control_params.set_bypass_initial_decline_time = (float)g_pc_parameters.set_bypass_initial_decline_time;
                control_params.set_rev_start_oilP_max = (float)g_pc_parameters.set_rev_start_oilP_max;
                control_params.set_rev_start_oilP_min = (float)g_pc_parameters.set_rev_start_oilP_min;
                control_params.set_first_fix_freq_time_on = (float)g_pc_parameters.set_first_fix_freq_time_on;
                control_params.set_first_fix_freq_time_off = (float)g_pc_parameters.set_first_fix_freq_time_off;
                control_params.set_extend_time = (float)g_pc_parameters.set_extend_time;
                control_params.set_extend_pressure = (float)g_pc_parameters.set_extend_pressure;
                control_params.set_retract_time = (float)g_pc_parameters.set_retract_time;
                control_params.set_retract_pressure = (float)g_pc_parameters.set_retract_pressure;
                control_params.set_cooler_temperature_on = (float)g_pc_parameters.set_cooler_temp_on;
                control_params.set_cooler_temperature_off = (float)g_pc_parameters.set_cooler_temp_off;
                control_params.bypass_off = g_pc_parameters.bypass_off;
                HydraulicControl_UpdateParams(&control_params);
            }
        } else {
            // 打印解包失败信息（带频率控制，每100次打印一次）
            Debug_Log_CANUnpackFailed(msg_id);
        }
    } else {
        // 打印未知消息ID信息（带频率控制和状态管理）
        Debug_Log_CANUnknownMessageID(msg_id, msg_id_masked, expected_control_id, expected_control2_id, length);
    }
}

/*!
 * @brief 任务：发送传感器数据（10ms周期）
 */
void CAN_MessageHandler_Task_10ms_SendSensorData(void)
{
    device_status_t msg;
    uint8_t can_data[8];
    
    /* 1. 更新传感器数据 */
    Sensor_UpdateMonitor();
    
    /* 2. 填充CAN消息（使用encode函数转换物理值→原始值） */
    float oil_temp = Sensor_GetOilTemperature();
    float lng_temp = Sensor_GetLNGTemperature();
    float oil_pressure = Sensor_GetOilPressure();
    float lng_pressure = Sensor_GetLNGPressure();
	
		const float DISPLAY_FILTER_ALPHA = 0.015f;
    g_filtered_lng_pressure = (DISPLAY_FILTER_ALPHA * lng_pressure) + ((1.0f - DISPLAY_FILTER_ALPHA) * g_filtered_lng_pressure);
    
    msg.oil_temperature = device_status_oil_temperature_encode(oil_temp);
    msg.LNG_temperature = device_status_LNG_temperature_encode(lng_temp);
    msg.oil_pressure = device_status_oil_pressure_encode(oil_pressure);
    msg.LNG_pressure = device_status_LNG_pressure_encode(g_filtered_lng_pressure);
    msg.bypass_ratio = device_status_bypass_ratio_encode(ValveControl_GetBypassValveDuty());
    msg.rev_enable = (ValveControl_GetDirectionalValveState() == VALVE_STATE_ON) ? 1 : 0;
    // 换向频率由控制算法计算，从 HydraulicControl_GetReversalFrequency() 获取
    // 注意：DBC v05中rev_freq范围是0..255，需要确保值在范围内
    uint16_t rev_freq_raw = HydraulicControl_GetReversalFrequency();
    if (rev_freq_raw > 255) rev_freq_raw = 255;  // 限制到DBC定义的最大值
    msg.rev_freq = (uint8_t)rev_freq_raw;  // 使用控制算法计算的频率值
    msg.cooler_enable = (ValveControl_GetCoolerState() == VALVE_STATE_ON) ? 1 : 0;
    
    // 传感器工程量程自检：超量程则置 debug 标志为1，正常为0
    bool sensors_ok =
        Sensor_ValidateValue(oil_pressure, OIL_PRESSURE_MEAS_MIN_MPA, OIL_PRESSURE_MEAS_MAX_MPA) &&
        Sensor_ValidateValue(lng_pressure, LNG_PRESSURE_MEAS_MIN_MPA, LNG_PRESSURE_MEAS_MAX_MPA) &&
        Sensor_ValidateValue(oil_temp, OIL_TEMP_MEAS_MIN_C, OIL_TEMP_MEAS_MAX_C) &&
        Sensor_ValidateValue(lng_temp, LNG_TEMP_MEAS_MIN_C, LNG_TEMP_MEAS_MAX_C);
    msg.debug = device_status_debug_encode(sensors_ok ? 0.0 : 1.0);
    
    /* 3. 打包并发送 */
    int pack_result = device_status_pack(can_data, &msg, sizeof(can_data));
    if (pack_result > 0) {
        static uint32_t send_count = 0;
        static uint32_t send_fail_count = 0;
        
        // 发送计数递增
        ++send_count;
        
        bool send_result = CAN_Config_SendMessage(CAN_MSG_DEVICE_STATUS_ID, can_data, 8, true);
        
        // 更新失败计数
        if (!send_result) {
            send_fail_count++;
        }
        
        // 打印发送失败信息（带状态管理，只在首次失败时打印）
        Debug_Log_CANTransmitFailed(send_count, send_fail_count, CAN_MSG_DEVICE_STATUS_ID, send_result);
    } else {
        // 打印打包失败信息（带状态管理，只打印一次）
        Debug_Log_CANPackFailed(pack_result);
    }
}

/*!
 * @brief 任务：CAN通信状态监控（1000ms周期）
 */
void CAN_MessageHandler_Task_1000ms_CANStatusMonitor(void)
{
    static uint32_t last_rx_count = 0;
    static uint32_t last_tx_count = 0;
    static uint32_t last_error_count = 0;
    static uint32_t monitor_count = 0;
    
    uint32_t current_rx_count, current_tx_count, current_error_count;
    
    // 获取CAN统计信息
    CAN_Config_GetStats(&current_rx_count, &current_tx_count, &current_error_count);
    
    // 计算增量
    uint32_t rx_delta = current_rx_count - last_rx_count;
    uint32_t tx_delta = current_tx_count - last_tx_count;
    uint32_t error_delta = current_error_count - last_error_count;
    
    // 更新上次计数
    last_rx_count = current_rx_count;
    last_tx_count = current_tx_count;
    last_error_count = current_error_count;
    
    // 增加监控计数
    monitor_count++;
    
    // 调用集中管理的调试打印函数
    Debug_Log_CANStatusMonitor(rx_delta, tx_delta, error_delta,
                                current_rx_count, current_tx_count, 
                                current_error_count, monitor_count);
}

/*!
 * @brief 任务：实时CAN信号监控（100ms周期）
 */
void CAN_MessageHandler_Task_100ms_RealTimeCANMonitor(void)
{
    static uint32_t last_rx_count = 0;
    static uint32_t last_tx_count = 0;
    
    uint32_t current_rx_count, current_tx_count, current_error_count;
    
    // 获取CAN统计信息
    CAN_Config_GetStats(&current_rx_count, &current_tx_count, &current_error_count);
    
    // 计算增量
    uint32_t rx_delta = current_rx_count - last_rx_count;
    uint32_t tx_delta = current_tx_count - last_tx_count;
    
    // 更新上次计数
    last_rx_count = current_rx_count;
    last_tx_count = current_tx_count;
    
    // 调用集中管理的调试打印函数
    Debug_Log_RealTimeCANMonitor(current_rx_count, current_tx_count, rx_delta, tx_delta);
}

/*!
 * @brief 任务：CAN消息处理（1ms周期 - 关键任务）
 */
void CAN_MessageHandler_Task_1ms_CANMessageProcess(void)
{
    // 调用CAN配置模块的消息处理任务
    // 这个函数负责：
    // 1. 处理接收到的CAN消息
    // 2. 调用接收回调函数
    // 3. 更新CAN统计信息
    CAN_Config_Task();
}

/*!
 * @brief 获取换向阀频率（从控制算法获取）
 * @return 换向阀频率（次/分钟）
 */
uint16_t CAN_MessageHandler_GetReversalValveFreq(void)
{
    return HydraulicControl_GetReversalFrequency();
}

/*!
 * @brief 获取PC端参数配置
 * @param[out] config 参数配置结构体指针
 */
void CAN_MessageHandler_GetParameterConfig(pc_parameter_config_t* config)
{
    if (config != NULL) {
        *config = g_pc_parameters;
    }
}

/*!
 * @brief CAN错误检测和恢复（作为任务调度器任务调用，5000ms周期）
 */
void CAN_MessageHandler_ErrorCheck(void)
{
    static uint32_t can_error_count = 0;
    
    // 检查CAN错误计数
    uint32_t rx_count, tx_count, error_count;
    CAN_Config_GetStats(&rx_count, &tx_count, &error_count);
    
    // 如果错误过多，尝试重置CAN控制器
    if (error_count > 1000) {
        can_error_count++;
        if (CAN_Config_ResetController()) {
            can_error_count = 0;  // 重置计数器
        } else {
            // 保留静默，避免打印风暴
        }
    }
}

/*!
 * @brief 获取CAN接收消息计数
 * @return CAN接收消息总数
 */
uint32_t CAN_MessageHandler_GetRxMessageCount(void)
{
    return g_can_rx_message_count;
}

