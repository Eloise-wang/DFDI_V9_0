/*!
 * @file debug_log.c
 *
 * @brief 统一调试输出接口实现
 * 
 * 说明：
 * - 集中管理所有调试打印函数
 * - 通过各模块提供的接口获取状态信息
 */

#include "debug_log.h"
#include "hydraulic_control.h"
#include "sensor.h"
#include "valve_control.h"
#include "can_config.h"
#include "can_message_handler.h"
#include "common_types.h"
#include <stdio.h>
#include <stdarg.h>

/* ==================== 基础调试输出函数 ==================== */

void Debug_Log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

/* ==================== 集中管理的调试打印函数 ==================== */

void Debug_Log_HydraulicControlStatus(void)
{
    const hydraulic_control_state_t* state = HydraulicControl_GetState();
    if (state == NULL) {
        return;
    }
    
    const char* state_names[] = {
        "DISABLED",
        "INITIALIZING",
        "WAITING_REV",
        "REV_PHASE1",
        "REV_PHASE2"
    };
    
    Debug_Log("\r\n=== Hydraulic Control Status ===\r\n");
    Debug_Log("System State: %s\r\n", state_names[state->system_state]);
    Debug_Log("Bypass Duty: %.1f%%\r\n", state->current_bypass_duty);
    Debug_Log("Reversal Enabled: %s\r\n", state->reversal_valve_enabled ? "YES" : "NO");
    Debug_Log("Reversal Frequency: %u times/min\r\n", state->rev_freq_per_min);
    Debug_Log("Cooler Enabled: %s\r\n", state->cooler_enabled ? "YES" : "NO");
    Debug_Log("===================================\r\n");
}

void Debug_Log_SensorDataMonitor(void)
{
    // 声明外部变量（在main.c中定义）
    extern bool g_systemEnabled;
    
    // 更新传感器数据
    Sensor_UpdateMonitor();
    
    // 获取当前传感器读数
    float oil_temp = Sensor_GetOilTemperature();
    float lng_temp = Sensor_GetLNGTemperature();
    float oil_pressure = Sensor_GetOilPressure();
    float lng_pressure = Sensor_GetLNGPressure();
    
    // 获取阀门状态
    valve_state_t dir_valve_state = ValveControl_GetDirectionalValveState();
    valve_state_t cooler_state = ValveControl_GetCoolerState();
    float bypass_duty = ValveControl_GetBypassValveDuty();
    
    // 打印详细数据
    Debug_Log("\r\n=== Sensor Data Monitor ===\r\n");
    Debug_Log("Oil Temperature: %.2f°C\r\n", oil_temp);
    Debug_Log("LNG Temperature: %.2f°C\r\n", lng_temp);
    Debug_Log("Oil Pressure: %.2f MPa\r\n", oil_pressure);
    Debug_Log("LNG Pressure: %.2f MPa\r\n", lng_pressure);
    Debug_Log("Directional Valve: %s\r\n", (dir_valve_state == VALVE_STATE_ON) ? "ON" : "OFF");
    Debug_Log("Cooler: %s\r\n", (cooler_state == VALVE_STATE_ON) ? "ON" : "OFF");
    Debug_Log("Bypass Valve: %.1f%%\r\n", bypass_duty);
    Debug_Log("System Enabled: %s\r\n", g_systemEnabled ? "YES" : "NO");
    Debug_Log("Reversal Freq: %u times/min\r\n", CAN_MessageHandler_GetReversalValveFreq());
    Debug_Log("===========================\r\n");
    
    // 传感器数据有效性检查
    if (!Sensor_CheckDataValidity()) {
        Debug_Log("[SENSOR] Warning: Sensor data validity check failed\r\n");
    }
    
    // 超限报警
    if (oil_pressure > 40.0f) {
        Debug_Log("[SENSOR] Warning: Oil pressure high (%.2f MPa)\r\n", oil_pressure);
    }
    if (oil_temp > 100.0f) {
        Debug_Log("[SENSOR] Warning: Oil temperature high (%.2f°C)\r\n", oil_temp);
    }
    if (lng_temp > 80.0f) {
        Debug_Log("[SENSOR] Warning: LNG temperature high (%.2f°C)\r\n", lng_temp);
    }
}

/* ==================== 阀门控制模块打印函数 ==================== */

void Debug_Log_ValvePWMInit(uint32_t actual_count)
{
    Debug_Log("[VALVE INIT] PWM0 initialized for bypass valve (PWM_CH_2), "
              "output DISABLED, CountValue: %lu\r\n", actual_count);
}


void Debug_Log_DirectionalValveControl(bool enable, bool prev_enable)
{
    // 只在状态改变时打印
    if (enable == prev_enable) {
        return;
    }

    const char* state_str = enable ? "ON" : "OFF";
    const char* gpio_str  = enable ? "HIGH" : "LOW";

    Debug_Log("[VALVE] Directional Valve: %s (PB4)\r\n", state_str);
    Debug_Log("[VALVE] GPIO Set: PB4 = %s\r\n", gpio_str);
}

void Debug_Log_BypassValveDuty(float prev_duty, float new_duty,
                               uint16_t max_count, uint16_t count_value,
                               uint32_t verify_count)
{
    float duty_change = (new_duty > prev_duty) ?
                        (new_duty - prev_duty) :
                        (prev_duty - new_duty);

    // 只在占空比变化超过1%时打印（避免微小变化导致的频繁打印）
    if (duty_change > 1.0f) {
        Debug_Log("[VALVE] Bypass Valve: %.2f%% (PWM0_CH2)\r\n", new_duty);
        Debug_Log("[VALVE] PWM Set: MaxCount=%u, CountValue=%u\r\n",
                  max_count, count_value);

        // 校验计数值是否一致
        if (verify_count != count_value) {
            Debug_Log("[VALVE WARNING] CountValue mismatch! Set=%u, Read=%lu\r\n",
                      count_value, verify_count);
        }
    }
}

void Debug_Log_CoolerControl(bool enable, bool prev_enable)
{
    // 只在状态改变时打印
    if (enable == prev_enable) {
        return;
    }

    const char* state_str = enable ? "ON" : "OFF";
    const char* gpio_str  = enable ? "HIGH" : "LOW";

    Debug_Log("[VALVE] Cooler: %s (PE8)\r\n", state_str);
    Debug_Log("[VALVE] GPIO Set: PE8 = %s\r\n", gpio_str);
}

void Debug_Log_CANStatusMonitor(uint32_t rx_delta, uint32_t tx_delta, uint32_t error_delta,
                                uint32_t current_rx_count, uint32_t current_tx_count, 
                                uint32_t current_error_count, uint32_t monitor_count)
{
    // 每10次监控（10秒）显示状态
    if (monitor_count % 10 == 0) {
        Debug_Log("[CAN] RX:%lu TX:%lu Err:%lu (10s)\r\n", rx_delta, tx_delta, error_delta);
    }
    
    // 每50次监控（50秒）打印一次详细状态
    if (monitor_count % 50 == 0) {
        Debug_Log("\r\n=== CAN Communication Status ===\r\n");
        Debug_Log("Total RX: %lu, TX: %lu, Errors: %lu\r\n", 
                  current_rx_count, current_tx_count, current_error_count);
        Debug_Log("Rate (10s): RX=%lu, TX=%lu, Errors=%lu\r\n", 
                  rx_delta, tx_delta, error_delta);
        Debug_Log("===============================\r\n");
    }
    
    // 错误检测和报警（只在错误首次出现或数量显著变化时打印一次）
    static uint32_t last_warning_error_delta = 0;
    static bool warning_printed = false;
    
    if (error_delta > 0) {
        // 只在首次出现错误或错误数量变化超过1000时才打印一次（避免频繁打印相似数值）
        uint32_t error_change = (error_delta > last_warning_error_delta) ? 
                                (error_delta - last_warning_error_delta) : 
                                (last_warning_error_delta - error_delta);
        if (!warning_printed || (error_change > 1000)) {
            Debug_Log("[CAN] Warning: %lu errors detected in last 1s\r\n", error_delta);
            last_warning_error_delta = error_delta;
            warning_printed = true;
        }
    } else {
        // 无错误时重置打印标志
        warning_printed = false;
        last_warning_error_delta = 0;
    }
    
    // 通信超时检测
    static uint32_t no_rx_count = 0;
    if (rx_delta == 0 && current_rx_count > 0) {
        if (++no_rx_count >= 5) {  // 5秒无接收
            Debug_Log("[CAN] Warning: No RX messages for %lu seconds\r\n", no_rx_count);
        }
    } else {
        no_rx_count = 0;  // 重置计数器
    }
}

void Debug_Log_RealTimeCANMonitor(uint32_t current_rx_count, uint32_t current_tx_count,
                                   uint32_t rx_delta, uint32_t tx_delta)
{
    static uint32_t monitor_count = 0;
    
    // 每100次监控（10秒）显示实时CAN活动
    if (++monitor_count % 100 == 0) {
        Debug_Log("[CAN] RX: %lu, TX: %lu, Rate: %lu/%lu msg/s\r\n", 
                  current_rx_count, current_tx_count, rx_delta, tx_delta);
    }
}

/* ==================== 系统初始化相关打印函数 ==================== */

void Debug_Log_SystemInitError(const char *error_msg)
{
    Debug_Log("[INIT] ERROR: %s\r\n", error_msg);
}

void Debug_Log_SystemInitCompleted(void)
{
    Debug_Log("[INIT] System initialization completed\r\n");
    Debug_Log("[INIT] Default state: Cooler=OFF, DirectionalValve=OFF, BypassValve=0%%\r\n");
}

/* ==================== CAN配置模块打印函数 ==================== */

void Debug_Log_CANEventError(uint32_t error_count, uint32_t event, bool has_error)
{
    static bool errorActive = false;
    static bool error_has_printed = false;
    
    if (has_error)
    {
        /* 只在首次进入错误状态时打印一次，之后不再打印 */
        if (!errorActive && !error_has_printed)
        {
            Debug_Log("[CAN ERROR] Count: %lu, Event: 0x%08X\r\n", error_count, event);
            error_has_printed = true;
        }
        
        /* Mark error as active */
        if (!errorActive)
        {
            errorActive = true;
        }
    }
    else
    {
        /* No errors - check if we just recovered from an error state */
        if (errorActive)
        {
            errorActive = false;
            error_has_printed = false;  /* 恢复后重置打印标志，允许下次错误时再次打印 */
        }
    }
}

void Debug_Log_CANBusOffRecovery(void)
{
    Debug_Log("[CAN RECOVERY] Bus-off detected, attempting recovery...\r\n");
}

void Debug_Log_CANTransmitBufferFull(void)
{
    Debug_Log("[CAN WARNING] Transmit buffer full, message dropped\r\n");
}

void Debug_Log_CANSendErrorStatus(uint32_t status, uint32_t error_count, bool is_success)
{
    static uint32_t last_error_status = 0;
    static bool status_error_printed = false;
    
    if (!is_success)
    {
        // 只在状态码改变或首次错误时打印一次
        if (status != last_error_status || !status_error_printed)
        {
            Debug_Log("[CAN ERROR] Status: 0x%08X, Errors: %lu\r\n", status, error_count);
            last_error_status = status;
            status_error_printed = true;
        }
    }
    else
    {
        // 成功时重置打印标志
        status_error_printed = false;
    }
}

void Debug_Log_CANSendStatistics(uint32_t *send_count, uint32_t error_count)
{
    static bool stats_printed = false;
    static float last_success_rate = -1.0f;
    
    if (send_count == NULL)
    {
        return;
    }
    
    (*send_count)++;
    
    // 统计信息只在首次打印或成功率有显著变化时打印一次（避免频繁打印）
    if (*send_count % 100 == 0)
    {
        float current_success_rate = *send_count > 0 ? (100.0f * (*send_count - error_count) / *send_count) : 0.0f;
        // 只在首次打印或成功率变化超过10%时打印一次（避免因数值微小变化导致的频繁打印）
        if (!stats_printed || 
            (current_success_rate - last_success_rate > 10.0f) || 
            (last_success_rate - current_success_rate > 10.0f))
        {
            Debug_Log("[CAN STATS] Total: %lu, Errors: %lu, Success: %.1f%%\r\n", 
                      *send_count, error_count, current_success_rate);
            last_success_rate = current_success_rate;
            stats_printed = true;
        }
    }
}

void Debug_Log_CANResetStart(void)
{
    Debug_Log("[CAN RESET] Resetting CAN controller...\r\n");
}

void Debug_Log_CANResetSuccess(void)
{
    Debug_Log("[CAN RESET] Controller reset successful\r\n");
}

void Debug_Log_CANResetFailed(uint32_t status)
{
    Debug_Log("[CAN RESET] Controller reset failed: 0x%08X\r\n", status);
}

/* ==================== CAN消息处理模块打印函数 ==================== */

void Debug_Log_CANSystemEnableChanged(bool system_enable, uint8_t raw_value)
{
    static bool last_system_enable = false;
    
    if (system_enable != last_system_enable)
    {
        Debug_Log("[CAN RX] System Enable changed: %s -> %s (raw: %u)\r\n", 
                  last_system_enable ? "YES" : "NO",
                  system_enable ? "YES" : "NO",
                  raw_value);
        last_system_enable = system_enable;
    }
}

void Debug_Log_CANUnpackFailed(uint32_t msg_id)
{
    static uint32_t unpack_fail_count = 0;
    
    if (++unpack_fail_count % 100 == 0)  // 每100次失败打印一次
    {
        Debug_Log("[CAN RX] tsmaster_control2 unpack failed, MSG ID: 0x%08X\r\n", msg_id);
    }
}

void Debug_Log_CANUnknownMessageID(uint32_t msg_id, uint32_t msg_id_masked, 
                                     uint32_t expected_id1, uint32_t expected_id2, uint8_t length)
{
    static uint32_t unknown_msg_count = 0;
    static uint32_t last_unknown_id = 0;
    
    if (msg_id_masked != last_unknown_id || (++unknown_msg_count % 100 == 0))
    {
        Debug_Log("[CAN RX] Unknown MSG ID: 0x%08X (masked: 0x%08X, expected: 0x%08X or 0x%08X), Length: %u\r\n", 
                  msg_id, msg_id_masked, expected_id1, expected_id2, length);
        last_unknown_id = msg_id_masked;
        if (unknown_msg_count % 100 == 0)
        {
            unknown_msg_count = 0;  // 重置计数
        }
    }
}

void Debug_Log_CANTransmitFailed(uint32_t send_count, uint32_t fail_count, uint32_t msg_id, bool is_success)
{
    static bool tx_fail_printed = false;
    
    if (!is_success)
    {
        if (!tx_fail_printed)
        {
            Debug_Log("[CAN TX FAIL] Count: #%lu, Failures: %lu, ID: 0x%08X\r\n", 
                      send_count, fail_count, msg_id);
            tx_fail_printed = true;
        }
    }
    else
    {
        // 成功时重置打印标志
        tx_fail_printed = false;
    }
}

void Debug_Log_CANPackFailed(int pack_result)
{
    static bool pack_error_printed = false;
    
    if (!pack_error_printed)
    {
        Debug_Log("[CAN TX ERROR] Pack failed, result: %d\r\n", pack_result);
        pack_error_printed = true;
    }
}

void Debug_Log_CANReceivedParameters(const pc_parameter_config_t *config)
{
    if (config == NULL) {
        return;
    }
    
    static pc_parameter_config_t last_config = {0};
    static bool first_print = true;
    
    // 检查参数是否变化（仅在变化时打印）
    bool params_changed = first_print ||
        (last_config.system_enable != config->system_enable) ||
        (last_config.set_bypass_ratio != config->set_bypass_ratio) ||
        (last_config.set_bypass_initial_decline_time != config->set_bypass_initial_decline_time) ||
        (last_config.set_rev_start_oilP_max != config->set_rev_start_oilP_max) ||
        (last_config.set_rev_start_oilP_min != config->set_rev_start_oilP_min) ||
        (last_config.set_first_fix_freq_time_on != config->set_first_fix_freq_time_on) ||
        (last_config.set_first_fix_freq_time_off != config->set_first_fix_freq_time_off) ||
        (last_config.set_second_rev_oilP_max != config->set_second_rev_oilP_max) ||
        (last_config.set_second_rev_oilP_min != config->set_second_rev_oilP_min) ||
        (last_config.set_second_on_overtime != config->set_second_on_overtime) ||
        (last_config.set_second_off_overtime != config->set_second_off_overtime) ||
        (last_config.set_rev_compel_time_on != config->set_rev_compel_time_on) ||
        (last_config.set_rev_compel_time_off != config->set_rev_compel_time_off) ||
        (last_config.set_cooler_temp_on != config->set_cooler_temp_on) ||
        (last_config.set_cooler_temp_off != config->set_cooler_temp_off);
    
    if (params_changed) {
        Debug_Log("\r\n=== TSMaster Parameters Received ===\r\n");
        Debug_Log("System Enable: %s\r\n", config->system_enable ? "YES" : "NO");
        Debug_Log("Bypass Ratio: %.1f%%\r\n", config->set_bypass_ratio);
        Debug_Log("Bypass Decline Time: %.1f s\r\n", config->set_bypass_initial_decline_time);
        Debug_Log("Rev Start OilP Max: %.2f MPa\r\n", config->set_rev_start_oilP_max);
        Debug_Log("Rev Start OilP Min: %.2f MPa\r\n", config->set_rev_start_oilP_min);
        Debug_Log("Phase1 Time ON: %.2f s\r\n", config->set_first_fix_freq_time_on);
        Debug_Log("Phase1 Time OFF: %.2f s\r\n", config->set_first_fix_freq_time_off);
        Debug_Log("Phase2 OilP Max: %.2f MPa\r\n", config->set_second_rev_oilP_max);
        Debug_Log("Phase2 OilP Min: %.2f MPa\r\n", config->set_second_rev_oilP_min);
        Debug_Log("Phase2 ON Overtime: %.2f s\r\n", config->set_second_on_overtime);
        Debug_Log("Phase2 OFF Overtime: %.2f s\r\n", config->set_second_off_overtime);
        Debug_Log("Rev Compel Time ON: %.2f s\r\n", config->set_rev_compel_time_on);
        Debug_Log("Rev Compel Time OFF: %.2f s\r\n", config->set_rev_compel_time_off);
        Debug_Log("Cooler Temp ON: %.1f°C\r\n", config->set_cooler_temp_on);
        Debug_Log("Cooler Temp OFF: %.1f°C\r\n", config->set_cooler_temp_off);
        Debug_Log("=====================================\r\n\r\n");
        
        // 保存当前配置用于下次比较
        last_config = *config;
        first_print = false;
    }
}


