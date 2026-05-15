/*!
 * @file hydraulic_control.c
 *
 * @brief 液压系统控制逻辑模块实现 - 嵌入式控制架构
 *
 * 功能说明：
 * - 在嵌入式端执行控制算法逻辑（温度控制、压力控制、换向阀逻辑等）
 * - 根据传感器数据和接收的PC端参数配置执行控制决策
 * - 自动控制阀门硬件（不再被动接收PC端命令）
 */

#include <string.h>
#include <math.h>
#include "hydraulic_control.h"
#include "valve_control.h"
#include "sensor.h"
#include "debug_log.h"
#include "osif.h"

/* ===============================================  Constants  =========================================== */
#define TANK_PRESSURE_THRESHOLD_MPA   PHASE2_TARGET_PRESSURE_MPA

/* ===========================================  Private Variables  =========================================== */
static hydraulic_control_state_t g_control_state;

typedef struct
{
    float median_buf[COOLER_TEMP_MEDIAN_SIZE];
    float avg_buf[COOLER_TEMP_AVG_SIZE];
    uint8_t median_index;
    uint8_t avg_index;
    bool initialized;
} cooler_temp_filter_t;


static cooler_temp_filter_t g_cooler_temp_filter;
static const float k_bypass_off_ramp_rate_percent_per_sec = 5.0f;  //以5%匀速上升到50%

/* ===========================================  Private Functions  =========================================== */

/*!
 * @brief 获取当前时间（毫秒）
 */
static uint32_t GetCurrentTimeMs(void)
{
    return OSIF_GetMilliseconds();
}

static void SortFloatArray(float *arr, uint8_t len)
{
    uint8_t i;
    uint8_t j;

    if (arr == NULL || len < 2U) {
        return;
    }

    for (i = 0U; i < (uint8_t)(len - 1U); i++) {
        for (j = 0U; j < (uint8_t)(len - 1U - i); j++) {
            if (arr[j] > arr[j + 1U]) {
                float temp = arr[j];
                arr[j] = arr[j + 1U];
                arr[j + 1U] = temp;
            }
        }
    }
}

/* 冷却器温度滤波：
* - 中值滤波（去噪）
* - 简单移动平均（平滑）
*/
static float FilterOilTemperature(float input_temp)
{
    float temp_array[COOLER_TEMP_MEDIAN_SIZE];
    float median_value;
    float sum = 0.0f;
    uint8_t i;

    if (!g_cooler_temp_filter.initialized) {
        for (i = 0U; i < COOLER_TEMP_MEDIAN_SIZE; i++) {
            g_cooler_temp_filter.median_buf[i] = input_temp;
        }

        for (i = 0U; i < COOLER_TEMP_AVG_SIZE; i++) {
            g_cooler_temp_filter.avg_buf[i] = input_temp;
        }

        g_cooler_temp_filter.median_index = 0U;
        g_cooler_temp_filter.avg_index = 0U;
        g_cooler_temp_filter.initialized = true;

        return input_temp;
    }

    /* 1. 更新中值滤波窗口 */
    g_cooler_temp_filter.median_buf[g_cooler_temp_filter.median_index] = input_temp;
    g_cooler_temp_filter.median_index++;
    if (g_cooler_temp_filter.median_index >= COOLER_TEMP_MEDIAN_SIZE) {
        g_cooler_temp_filter.median_index = 0U;
    }

    /* 2. 拷贝窗口并排序，取中值 */
    for (i = 0U; i < COOLER_TEMP_MEDIAN_SIZE; i++) {
        temp_array[i] = g_cooler_temp_filter.median_buf[i];
    }

    SortFloatArray(temp_array, COOLER_TEMP_MEDIAN_SIZE);
    median_value = temp_array[COOLER_TEMP_MEDIAN_SIZE / 2U];

    /* 3. 中值结果再进入平均滤波窗口 */
    g_cooler_temp_filter.avg_buf[g_cooler_temp_filter.avg_index] = median_value;
    g_cooler_temp_filter.avg_index++;
    if (g_cooler_temp_filter.avg_index >= COOLER_TEMP_AVG_SIZE) {
        g_cooler_temp_filter.avg_index = 0U;
    }

    /* 4. 求平均 */
    for (i = 0U; i < COOLER_TEMP_AVG_SIZE; i++) {
        sum += g_cooler_temp_filter.avg_buf[i];
    }

    return (sum / (float)COOLER_TEMP_AVG_SIZE);
}

/*!
 * @brief 检查是否应该启动换向阀（对应需求3）
 *
 * 对应需求描述：
 *  3. 在旁通阀从50%下降到0%的过程中，当检测到旁通比例小于25%的时候检测油压值，
 *     当油压值大于set_rev_start_oilP_max或者小于set_rev_start_oilP_min时，启动换向阀。
 *
 * 实现逻辑：
 *  - 旁通阀必须正在下降（bypass_decline_active == true）
 *  - 旁通阀开度必须小于25%
 *  - 油压必须满足：oil_pressure > set_rev_start_oilP_max 或 oil_pressure < set_rev_start_oilP_min
 */
static bool ShouldStartReversalValve(float oil_pressure)
{
    if (!g_control_state.bypass_decline_active) {
        return false;
    }
    
    // 检查旁通阀是否小于25%（在下降过程中）
    if (g_control_state.current_bypass_duty >= 25.0f) {
        return false;
    }
    
    // 检查油压是否超出范围（大于上限或小于下限）
    float max_p = g_control_state.params.set_rev_start_oilP_max;  // 上限
    float min_p = g_control_state.params.set_rev_start_oilP_min;  // 下限
    
    // 当油压大于上限或小于下限时，启动换向阀
    return (oil_pressure > max_p || oil_pressure < min_p);
}

/*!
 * @brief 更新换向频率统计（滑动窗口法）
 *
 * 定义：一次“换向”= 两次物理翻转（ON->OFF->ON 或 OFF->ON->OFF）。
 * 频率：过去 60 秒内的总换向次数（次/分钟）。
 *
 * 工业现场更关注“最近一分钟”真实发生次数，而非按瞬时周期推算。
 * 因此采用 1 秒分辨率的 60 秒滑动窗口：
 * - 每秒将上一秒的换向次数写入环形队列 counts_per_sec[60]；
 * - 输出 rev_freq_per_min = 队列内 60 个桶的总和；
 * - 若调度抖动导致累计超过 1 秒，则推进多个桶，缺失的秒填 0，避免窗口滞后。
 */
static void UpdateReversalFrequency(void)
{
    uint32_t current_time = GetCurrentTimeMs();
    uint32_t elapsed_ms = current_time - g_control_state.rev_last_update_ms; 

    if (elapsed_ms < 1000U) {
        return;
    }

    uint32_t elapsed_s = elapsed_ms / 1000U;

    if (elapsed_s >= (uint32_t)REV_FREQ_WINDOW_SIZE_SEC) {
        memset(g_control_state.counts_per_sec, 0, sizeof(g_control_state.counts_per_sec));
        g_control_state.current_sec_index = 0U;
        g_control_state.current_sec_count = 0U;
        g_control_state.rev_freq_per_min = 0U;
        g_control_state.rev_last_update_ms = current_time;
        return;
    }

    for (uint32_t i = 0U; i < elapsed_s; i++) {
        uint8_t value_to_store = (i == 0U) ? g_control_state.current_sec_count : 0U;

        g_control_state.counts_per_sec[g_control_state.current_sec_index] = value_to_store;
        g_control_state.current_sec_index =
            (uint8_t)((g_control_state.current_sec_index + 1U) % REV_FREQ_WINDOW_SIZE_SEC);

        if (i == 0U) {
            g_control_state.current_sec_count = 0U;
        }
    }

    uint16_t sum = 0U;
    for (int i = 0; i < REV_FREQ_WINDOW_SIZE_SEC; i++) {
        sum = (uint16_t)(sum + (uint16_t)g_control_state.counts_per_sec[i]);
    }

    g_control_state.rev_freq_per_min = sum;
    g_control_state.rev_last_update_ms += elapsed_s * 1000U;
}


/*!
 * @brief 记录换向事件（两次状态翻转算一次换向）
 */
static void RecordReversalEvent(void)
{
    bool current_state = (ValveControl_GetDirectionalValveState() == VALVE_STATE_ON);
    if (current_state != g_control_state.rev_last_state)
    {
        g_control_state.rev_toggle_count++;   // 记录一次翻转
        g_control_state.rev_last_state = current_state;

        if (g_control_state.rev_toggle_count >= 2)
        {
            // 重点：这里不再去加 window 计数，而是只加当前秒的临时计数
            if (g_control_state.current_sec_count < 255U) {
                g_control_state.current_sec_count++;
            }
            g_control_state.rev_toggle_count = 0;   // 清零重新累计
        }
    }
}

/*!
 * @brief 处理旁通阀线性下降（对应需求2）
 *
 * 需求2：设备运作之后，会接收到 CAN 传过来的旁通阀 50%（set_bypass_ratio）的信号，
 *        然后旁通阀从 50% 在 T 秒（set_bypass_initial_decline_time）内线性下降到 0。
 *
 * 实现方式：
 *  - 当系统进入 SYSTEM_STATE_INITIALIZING 状态时，
 *    current_bypass_duty 初始化为 set_bypass_ratio（默认 50%）；
 *  - 在 0~T 秒之间按线性比例从 start_duty 下降到 0；
 *  - 超过 T 秒或 T=0 时，直接置为 0 并结束下降流程。
 */
static void ProcessBypassValveDecline(void)
{
    if (!g_control_state.bypass_decline_active) {
        return;
    }
    
    uint32_t current_time = GetCurrentTimeMs();
    uint32_t elapsed_time = current_time - g_control_state.bypass_decline_start_time;
    float decline_time_ms = g_control_state.params.set_bypass_initial_decline_time * 1000.0f;
    
    if (decline_time_ms <= 0.0f) {
        // 如果下降时间为0，直接设置为0
        g_control_state.current_bypass_duty = 0.0f;
        g_control_state.bypass_decline_active = false;
    } else if (elapsed_time >= (uint32_t)decline_time_ms) {
        // 下降完成
        g_control_state.current_bypass_duty = 0.0f;
        g_control_state.bypass_decline_active = false;
    } else {
        // 线性下降计算：从50%线性下降到0%
        // 注意：初始值固定为50%，不受set_bypass_ratio参数影响
        float start_duty = 50.0f;  // 固定从50%开始下降
        float progress = (float)elapsed_time / decline_time_ms;
        g_control_state.current_bypass_duty = start_duty * (1.0f - progress);
        
        if (g_control_state.current_bypass_duty < 0.0f) {
            g_control_state.current_bypass_duty = 0.0f;
        }
    }
    
    // 设置旁通阀开度
    ValveControl_SetBypassValve(g_control_state.current_bypass_duty);
}

/*!
 * @brief 处理换向阀第一阶段（对应需求4）
 *
 * 需求4：换向阀一旦开始启动，就进入第一阶段（不可逆），之后换向阀以固定频率运行：
 *        固定开 T1（set_first_fix_freq_time_on）秒，固定关 T2（set_first_fix_freq_time_off）秒。
 *
 * 实现方式：
 *  - system_state == SYSTEM_STATE_REV_PHASE1 时启用该逻辑；
 *  - 当前阀门为“开”状态时，到达 T1 秒后切换为“关”；
 *  - 当前阀门为“关”状态时，到达 T2 秒后切换为“开”；
 *  - 每次从开→关或关→开时调用 RecordReversalEvent() 记录一次换向。
 */
static void ProcessReversalValvePhase1(void)
{
    if (g_control_state.system_state != SYSTEM_STATE_REV_PHASE1) {
        return;
    }
    
    uint32_t current_time = GetCurrentTimeMs();
    uint32_t state_duration = current_time - g_control_state.rev_state_change_time;
    
    bool current_state = ValveControl_GetDirectionalValveState() == VALVE_STATE_ON;
    
    if (current_state) {
        // 当前是开启状态
        float time_on_ms = g_control_state.params.set_first_fix_freq_time_on * 1000.0f;
        if (state_duration >= (uint32_t)time_on_ms) {
            // 切换到关闭状态
            ValveControl_SetDirectionalValve(false);
            g_control_state.rev_state_change_time = current_time;
            RecordReversalEvent();
        }
    } else {
        // 当前是关闭状态
        float time_off_ms = g_control_state.params.set_first_fix_freq_time_off * 1000.0f;
        if (state_duration >= (uint32_t)time_off_ms) {
            // 切换到开启状态
            ValveControl_SetDirectionalValve(true);
            g_control_state.rev_state_change_time = current_time;
            RecordReversalEvent();
        }
    }
}


static void ProcessReversalValvePhase2(float display_lng_pressure)
{
    // 仅在主状态处于第二阶段时执行
    if (g_control_state.system_state != SYSTEM_STATE_REV_PHASE2) {
        return;
    }

    uint32_t current_time = GetCurrentTimeMs();
    bool is_manual = g_control_state.params.set_second_manual;

    // 1. 模式切换检测与参数同步
    if (is_manual != g_control_state.phase2_prev_manual_mode) {
        if (!is_manual) {
            // 手动 -> 自动
            // 将当前的开启/关闭时间作为自动模式的起始参考值
            // 如果刚从一阶段进入二阶段，则已经初始化过，这里是运行过程中的模式切换
            g_control_state.phase2_auto_current_time_on = g_control_state.params.set_second_workDone_time;
            g_control_state.phase2_auto_current_time_off = g_control_state.params.set_second_oilSuction_time;
            g_control_state.phase2_auto_last_adjust_time = current_time;
            Debug_Log("Phase 2: Switched to AUTO mode. Initial T_on=%.1fs, T_off=%.1fs", 
                      g_control_state.phase2_auto_current_time_on, g_control_state.phase2_auto_current_time_off);
        } else {
            // 自动 -> 手动
            Debug_Log("Phase 2: Switched to MANUAL mode. Using user params: T_on=%.1fs, T_off=%.1fs", 
                      g_control_state.params.set_second_workDone_time, g_control_state.params.set_second_oilSuction_time);
        }
        g_control_state.phase2_prev_manual_mode = is_manual;
    }

    float target_time_on;
    float target_time_off;

    // 2. 模式逻辑处理
    if (is_manual) {
        // 手动模式：直接使用用户输入的参数
        target_time_on = g_control_state.params.set_second_workDone_time;
        target_time_off = g_control_state.params.set_second_oilSuction_time;
        // 旁通阀开度在 HydraulicControl_Process 中根据 set_bypass_ratio 同步
        // 说明：手动模式完全由上位机给定参数驱动，不启用“超压保持态”这类自动保护冻结逻辑。
        // 若上一轮处于自动模式的超压保持态，切到手动后必须清除该状态，否则会误冻结手动换向。
        g_control_state.phase2_overpressure_hold_prev = false;
    } else {
        // 自动模式：根据罐压实时调整
        // 当罐压在 30±1.5MPa 内，停止调整（维持现状）
        // 当罐压 < 28.5MPa，增加动力，提高频率
        if (current_time - g_control_state.phase2_auto_last_adjust_time >= PHASE2_ADJUST_PERIOD_MS) {
            g_control_state.phase2_auto_last_adjust_time = current_time;

            if (display_lng_pressure < PHASE2_PRESSURE_LOW_MPA) {
                // 罐压呈下降趋势，需要增加做功
                // 调节目标：提高排气量/泵送能力，尽快把罐压拉回到 30±1.5MPa 范围内
                
                // A. 旁通阀开度调整（增加动力）：步长 2%
                g_control_state.current_bypass_duty -= PHASE2_BYPASS_STEP_PERCENT;
                if (g_control_state.current_bypass_duty < 0.0f) {
                    g_control_state.current_bypass_duty = 0.0f;
                }
                ValveControl_SetBypassValve(g_control_state.current_bypass_duty);

                // B. 换向阀时间调整（提高频率）：缩短开启和关闭时间，步长 100ms
                // 注意：同时缩短 on/off，可以在“占空比”不变的情况下整体提升频率，达到更快泵送
                g_control_state.phase2_auto_current_time_on -= PHASE2_TIME_STEP_S;
                g_control_state.phase2_auto_current_time_off -= PHASE2_TIME_STEP_S;
                
                // 安全限制：最小换向时间设置为 0.1s
                if (g_control_state.phase2_auto_current_time_on < PHASE2_MIN_VALVE_TIME_S) g_control_state.phase2_auto_current_time_on = PHASE2_MIN_VALVE_TIME_S;
                if (g_control_state.phase2_auto_current_time_off < PHASE2_MIN_VALVE_TIME_S) g_control_state.phase2_auto_current_time_off = PHASE2_MIN_VALVE_TIME_S;
                
                Debug_Log("Auto Adjust: Pressure %.1f < %.1f. Bypass=%.1f%%, T_on=%.1fs, T_off=%.1fs",
                          display_lng_pressure, (float)PHASE2_PRESSURE_LOW_MPA, g_control_state.current_bypass_duty,
                          g_control_state.phase2_auto_current_time_on, g_control_state.phase2_auto_current_time_off);
            } else if (display_lng_pressure > PHASE2_PRESSURE_HIGH_MPA) {
                // 超压：罐压高于目标上限（31.5MPa），需要“减小做功/降低频率”
                // 这里区分一般超压与危险超压：
                // - 一般超压：按步长逐步开大旁通阀，同时降低频率（延长 on/off 时间）
                // - 危险超压：直接强制全旁通（50%），快速切断做功，避免继续推高压力
                bool is_danger = (display_lng_pressure > PHASE2_PRESSURE_DANGER_MPA);
                float prev_bypass = g_control_state.current_bypass_duty;

                if (is_danger) {
                    // 危险超压：强制全旁通（停止做功）
                    g_control_state.current_bypass_duty = 50.0f;
                } else {
                    // 一般超压：按步长朝 50% 方向调整（逐步减少做功）
                    g_control_state.current_bypass_duty += PHASE2_BYPASS_OVER_STEP_PERCENT;
                    if (g_control_state.current_bypass_duty > 50.0f) {
                        g_control_state.current_bypass_duty = 50.0f;
                    }
                }

                if (g_control_state.current_bypass_duty != prev_bypass) {
                    ValveControl_SetBypassValve(g_control_state.current_bypass_duty);
                }

                // 降低换向频率：延长 on/off 时间（频率下降 → 单位时间做功次数下降）
                g_control_state.phase2_auto_current_time_on += PHASE2_TIME_STEP_S;
                g_control_state.phase2_auto_current_time_off += PHASE2_TIME_STEP_S;

                // 安全限制：最大换向时间（防止频率过低到“几乎不泵送”）
                if (g_control_state.phase2_auto_current_time_on > PHASE2_MAX_VALVE_TIME_S) g_control_state.phase2_auto_current_time_on = PHASE2_MAX_VALVE_TIME_S;
                if (g_control_state.phase2_auto_current_time_off > PHASE2_MAX_VALVE_TIME_S) g_control_state.phase2_auto_current_time_off = PHASE2_MAX_VALVE_TIME_S;

                Debug_Log("Auto Adjust: Pressure %.1f > %.1f%s. Bypass=%.1f%%, T_on=%.1fs, T_off=%.1fs",
                          display_lng_pressure, (float)PHASE2_PRESSURE_HIGH_MPA, is_danger ? "(DANGER)" : "",
                          g_control_state.current_bypass_duty,
                          g_control_state.phase2_auto_current_time_on, g_control_state.phase2_auto_current_time_off);
            }
        }
        
        target_time_on = g_control_state.phase2_auto_current_time_on;
        target_time_off = g_control_state.phase2_auto_current_time_off;
    }

    bool overpressure_hold_active =
        (!is_manual) &&
        (display_lng_pressure > PHASE2_PRESSURE_HIGH_MPA) &&
        (g_control_state.current_bypass_duty >= 50.0f);

    if (overpressure_hold_active) {
        // 超压保持态（极限保护）：
        // - 触发条件：自动模式 + 超压 + 旁通阀已经到 50%（已经“全旁通”）
        // - 保护动作：停止换向阀动作并保持在 OFF 位（不再做功），直到压力回落到 ≤31.5MPa
        // 说明：不在此处“持续下发 OFF”是为了避免无意义的重复GPIO/PWM操作与日志刷屏，
        //      因此仅在首次进入保持态时下发一次强制 OFF。
        if (!g_control_state.phase2_overpressure_hold_prev) {
            ValveControl_SetDirectionalValve(false);
            g_control_state.rev_state_change_time = current_time;
            Debug_Log("Phase 2 Auto: Overpressure hold active. Pressure=%.1f MPa, Bypass=%.1f%%",
                      display_lng_pressure, g_control_state.current_bypass_duty);
        }
        g_control_state.phase2_overpressure_hold_prev = true;
        return;
    }

    if (g_control_state.phase2_overpressure_hold_prev) {
        // 从“超压保持态”退出（压力已回落或旁通阀不再是 50%）：
        // - 清除保持态标志
        // - 重置换向计时基准：避免保持态期间累计的时间导致恢复瞬间直接发生一次换向
        g_control_state.phase2_overpressure_hold_prev = false;
        g_control_state.rev_state_change_time = current_time;
    }

    // 3. 执行换向阀动作控制
    uint32_t state_duration = current_time - g_control_state.rev_state_change_time;
    bool current_state = (ValveControl_GetDirectionalValveState() == VALVE_STATE_ON);
    
    if (current_state) {
        // 当前是开启状态 (做功)
        float time_on_ms = target_time_on * 1000.0f;
        if (state_duration >= (uint32_t)time_on_ms) {
            // 切换到关闭状态 (吸油)
            ValveControl_SetDirectionalValve(false);
            g_control_state.rev_state_change_time = current_time;
            RecordReversalEvent();
        }
    } else {
        // 当前是关闭状态 (吸油)
        float time_off_ms = target_time_off * 1000.0f;
        if (state_duration >= (uint32_t)time_off_ms) {
            // 切换到开启状态 (做功)
            ValveControl_SetDirectionalValve(true);
            g_control_state.rev_state_change_time = current_time;
            RecordReversalEvent();
        }
    }
}


/*!
 * @brief 处理风冷器控制（对应需求7，以及扩展的运行保护逻辑）
 *
 * 需求7：当检测到油温 > W1（set_cooler_temperature_on）时开启风冷器；
 *        当油温 < W2（set_cooler_temperature_off）时关闭风冷器。
 *
 * 扩展逻辑：
 *  - 油温先经过中值 + 平均滤波，减少抖动引起的频繁开关；
 *  - 增加最小运行时间与最小停机时间保护：
 *    - 开启后至少运行 COOLER_MIN_ON_TIME_MS；
 *    - 关闭后至少等待 COOLER_MIN_OFF_TIME_MS 才允许再次开启；
 *  - 保留风冷器最大运行时间和强制关闭时间：
 *    - 连续运行超过 COOLER_MAX_RUN_TIME_MS → 强制关闭；
 *    - 强制关闭持续 COOLER_FORCE_OFF_DURATION_MS 后恢复正常控制。
 */
static void ProcessCoolerControl(float oil_temperature)
{
    uint32_t current_time = GetCurrentTimeMs();
    float temp_on;
    float temp_off;

    /* 先做中值 + 平均滤波 */
    oil_temperature = FilterOilTemperature(oil_temperature);

    /* 检查强制关闭状态 */
    if (g_control_state.cooler_force_off_active) {
        uint32_t force_off_duration = current_time - g_control_state.cooler_force_off_start_time;

        if (force_off_duration >= COOLER_FORCE_OFF_DURATION_MS) {
            /* 强制关闭时间结束，恢复正常检测 */
            g_control_state.cooler_force_off_active = false;
            g_control_state.cooler_stop_time = current_time;
        } else {
            /* 强制关闭期间，保持关闭状态 */
            ValveControl_SetCooler(false);
            g_control_state.cooler_enabled = false;
            return;
        }
    }

    /* 检查是否达到最大运行时间 */
    if (g_control_state.cooler_enabled && (g_control_state.cooler_start_time > 0U)) 
    {
        uint32_t run_duration = current_time - g_control_state.cooler_start_time;

        if (run_duration >= COOLER_MAX_RUN_TIME_MS) 
        {
            /* 达到最大连续运行时间，强制关闭 */
            ValveControl_SetCooler(false);
            g_control_state.cooler_enabled = false;
            g_control_state.cooler_force_off_active = true;
            g_control_state.cooler_force_off_start_time = current_time;
            g_control_state.cooler_start_time = 0U;
            g_control_state.cooler_stop_time = current_time;
            return;
        }
    }

    /* 正常温度控制 */
    temp_on = g_control_state.params.set_cooler_temperature_on;
    temp_off = g_control_state.params.set_cooler_temperature_off;

    if (oil_temperature > temp_on) 
    {
        /* 温度高于开启阈值，开启风冷器，但需满足最小停机时间 */
        if (!g_control_state.cooler_enabled) 
        {
            uint32_t off_duration = current_time - g_control_state.cooler_stop_time;

            if ((g_control_state.cooler_stop_time == 0U) ||(off_duration >= COOLER_MIN_OFF_TIME_MS)) 
            {
                ValveControl_SetCooler(true);
                g_control_state.cooler_enabled = true;
                g_control_state.cooler_start_time = current_time;
            }
        }
    } else if (oil_temperature < temp_off) {
        /* 温度低于关闭阈值，关闭风冷器，但需满足最小运行时间 */
        if (g_control_state.cooler_enabled) 
        {
            uint32_t run_duration = current_time - g_control_state.cooler_start_time;

            if ((g_control_state.cooler_start_time == 0U) ||(run_duration >= COOLER_MIN_ON_TIME_MS)) 
            {
                ValveControl_SetCooler(false);
                g_control_state.cooler_enabled = false;
                g_control_state.cooler_start_time = 0U;
                g_control_state.cooler_stop_time = current_time;
            }
        }
    } else {
        /* 位于滞回区间内，不动作，保持当前状态 */
    }
}

/* ===========================================  Public Functions  =========================================== */

void HydraulicControl_Init(void)
{
    memset(&g_control_state, 0, sizeof(g_control_state));
	memset(&g_cooler_temp_filter, 0, sizeof(g_cooler_temp_filter));
    
    g_control_state.rev_last_update_ms = GetCurrentTimeMs();
    g_control_state.system_state = SYSTEM_STATE_DISABLED;
    g_control_state.rev_state = REV_STATE_IDLE;
    g_control_state.current_bypass_duty = 0.0f;
    g_control_state.current_sec_count = 0U;
    g_control_state.current_sec_index = 0U;
    memset(g_control_state.counts_per_sec, 0, sizeof(g_control_state.counts_per_sec));

    // 设置默认参数（防止未初始化）
    g_control_state.params.set_bypass_ratio = 50.0f;
    g_control_state.params.set_bypass_initial_decline_time = 20.0f; // 20秒内降至0%
    g_control_state.params.set_rev_start_oilP_max = 1.2f;
    g_control_state.params.set_rev_start_oilP_min = 0.9f;
    g_control_state.params.set_first_fix_freq_time_on = 1.0f;
    g_control_state.params.set_first_fix_freq_time_off = 1.0f;
    g_control_state.params.set_second_manual = false; // 默认为自动模式
    g_control_state.params.set_second_oilSuction_time = 0.0f;
    g_control_state.params.set_second_workDone_time = 0.0f;
    g_control_state.params.set_cooler_temperature_on = 55.0f;
    g_control_state.params.set_cooler_temperature_off = 50.0f;
}

void HydraulicControl_UpdateParams(const control_params_t *params)
{
    if (params != NULL) {
        memcpy(&g_control_state.params, params, sizeof(control_params_t));
        
        // 如果系统已使能且下降已完成，根据set_bypass_ratio参数控制旁通阀
        // 注意：下降流程只在系统启动时执行一次，不会因为参数改变而重新启动
        // 只有在下降完成后（bypass_decline_active == false）且不在初始化阶段时，才根据set_bypass_ratio控制
        if (g_control_state.params.system_enable && 
            !g_control_state.params.bypass_off &&
            !g_control_state.bypass_decline_active &&
            g_control_state.system_state != SYSTEM_STATE_DISABLED &&
            g_control_state.system_state != SYSTEM_STATE_INITIALIZING) {
            // 下降已完成，根据CAN传来的set_bypass_ratio控制旁通阀
            g_control_state.current_bypass_duty = (float)g_control_state.params.set_bypass_ratio;
            if (g_control_state.current_bypass_duty < 0.0f) g_control_state.current_bypass_duty = 0.0f;
            if (g_control_state.current_bypass_duty > 50.0f) g_control_state.current_bypass_duty = 50.0f;
            ValveControl_SetBypassValve(g_control_state.current_bypass_duty);
        }
    }
}

void HydraulicControl_SetSystemEnable(bool enable)
{
    g_control_state.params.system_enable = enable;
    
    if (!enable) {
        // 系统禁用，关闭所有执行器
        g_control_state.system_state = SYSTEM_STATE_DISABLED;
        ValveControl_SetBypassValve(0.0f);
        ValveControl_SetDirectionalValve(false);
        ValveControl_SetCooler(false);
        g_control_state.bypass_decline_active = false;
        g_control_state.reversal_valve_enabled = false;
        g_control_state.cooler_enabled = false;
    } else {
        // 系统使能，开始初始化阶段
        // 注意：只有在从禁用状态变为使能状态时，才重新初始化
        // 如果系统已经在运行（REV_PHASE1/REV_PHASE2），不应该重新初始化，避免打断正在进行的流程
        if (g_control_state.system_state == SYSTEM_STATE_DISABLED) {
            // 从禁用状态变为使能状态，开始初始化
            g_control_state.system_state = SYSTEM_STATE_INITIALIZING;
            // 旁通阀初始值固定为50%，不受TSMaster参数影响（按需求：旁通阀初始值为50%）
            g_control_state.current_bypass_duty = 50.0f;
            g_control_state.bypass_decline_start_time = GetCurrentTimeMs();
            g_control_state.bypass_decline_active = true;
            // 重置换向阀状态
            g_control_state.reversal_valve_enabled = false;
            ValveControl_SetBypassValve(g_control_state.current_bypass_duty);
        }
        // 如果系统已经在运行（不是DISABLED状态），不做任何操作，保持当前状态
    }
}

void HydraulicControl_Process(float oil_pressure, float oil_temperature, float display_lng_pressure)
{
    // 更新换向频率统计（1分钟窗口）
    UpdateReversalFrequency();
    
    /* 2. 系统使能检查 */
    if (!g_control_state.params.system_enable) {
        return;
    }

    /* 3. 旁通阀逻辑处理 */
    // 3.1 处理强制关闭(Bypass_Off)触发的 Ramp 上升逻辑
    bool bypass_off_active = g_control_state.params.bypass_off && 
                            (g_control_state.system_state == SYSTEM_STATE_REV_PHASE1 || 
                             g_control_state.system_state == SYSTEM_STATE_REV_PHASE2);

    if (bypass_off_active) {
        g_control_state.bypass_decline_active = false;
        uint32_t now_ms = GetCurrentTimeMs();
        
        if (!g_control_state.bypass_off_prev) {
            g_control_state.bypass_off_ramp_last_time = now_ms;
        }

        uint32_t dt_ms = now_ms - g_control_state.bypass_off_ramp_last_time;
        g_control_state.bypass_off_ramp_last_time = now_ms;

        // 以固定斜率向上 Ramp
        float dt_s = (float)dt_ms / 1000.0f;
        float target_duty = g_control_state.current_bypass_duty + k_bypass_off_ramp_rate_percent_per_sec * dt_s;
        
        if (target_duty > 50.0f) target_duty = 50.0f;
        if (target_duty != g_control_state.current_bypass_duty) {
            g_control_state.current_bypass_duty = target_duty;
            ValveControl_SetBypassValve(g_control_state.current_bypass_duty);
        }
        g_control_state.bypass_off_prev = true;
    } else {
        g_control_state.bypass_off_prev = false;
        
        // 3.2 处理初始线性下降逻辑
        if (g_control_state.bypass_decline_active) {
            ProcessBypassValveDecline();
        } 
        // 3.3 下降完成后，根据外部下发的 set_bypass_ratio 进行控制
        else if (g_control_state.system_state != SYSTEM_STATE_DISABLED && 
                 g_control_state.system_state != SYSTEM_STATE_INITIALIZING) {
            
            // 只有在非二阶段自动模式下，才同步外部 set_bypass_ratio
            bool is_phase2_auto = (g_control_state.system_state == SYSTEM_STATE_REV_PHASE2 && 
                                  !g_control_state.params.set_second_manual);
            
            if (!is_phase2_auto) {
                float target_duty = g_control_state.params.set_bypass_ratio;
                if (target_duty < 0.0f) target_duty = 0.0f;
                if (target_duty > 50.0f) target_duty = 50.0f;
                
                if (g_control_state.current_bypass_duty != target_duty) {
                    g_control_state.current_bypass_duty = target_duty;
                    ValveControl_SetBypassValve(g_control_state.current_bypass_duty);
                }
            }
        }
    }
    /* 4. 换向阀阶段转换判断 */
    if (g_control_state.system_state == SYSTEM_STATE_INITIALIZING)
    {
        // 在旁通阀下降过程中，检测是否满足启动换向阀条件（油压异常）
        if (ShouldStartReversalValve(oil_pressure))
        {
            g_control_state.system_state = SYSTEM_STATE_REV_PHASE1;
            g_control_state.rev_state_change_time = GetCurrentTimeMs();
            g_control_state.rev_phase1_start_time = GetCurrentTimeMs();
            Debug_Log("Entering REV Phase 1: Oil pressure (%.2f MPa) triggered startup", oil_pressure);
        }
    }
    else if (g_control_state.system_state == SYSTEM_STATE_REV_PHASE1) 
    {
        // 检测到罐压达到30MPa，进入二阶段
        if (display_lng_pressure >= TANK_PRESSURE_THRESHOLD_MPA) 
        {
            g_control_state.system_state = SYSTEM_STATE_REV_PHASE2;
            g_control_state.rev_state_change_time = GetCurrentTimeMs();
            g_control_state.rev_phase2_start_time = GetCurrentTimeMs();
            
            // 进入二阶段时，如果是自动模式，初始化自动调整的时间参数
            if (!g_control_state.params.set_second_manual) {
                g_control_state.phase2_auto_current_time_on = g_control_state.params.set_first_fix_freq_time_on;
                g_control_state.phase2_auto_current_time_off = g_control_state.params.set_first_fix_freq_time_off;
                g_control_state.phase2_auto_last_adjust_time = GetCurrentTimeMs();
                g_control_state.phase2_prev_manual_mode = false;
            } else {
                g_control_state.phase2_prev_manual_mode = true;
            }
            
            Debug_Log("Entering REV Phase 2: Tank pressure reached %.1f MPa", display_lng_pressure);
        }
    }

    /* 5. 换向阀阶段调度 */
    switch (g_control_state.system_state)
    {
        case SYSTEM_STATE_REV_PHASE1:
            ProcessReversalValvePhase1();
            break;

        case SYSTEM_STATE_REV_PHASE2:
            ProcessReversalValvePhase2(display_lng_pressure);
            break;

        default:
            break;
    }

    /* 6. 风冷器控制 */
    // 基于滤波后的油温及保护逻辑（最小启停时间、强制关闭等）执行控制
    ProcessCoolerControl(oil_temperature);
}

system_state_t HydraulicControl_GetSystemState(void)
{
    return g_control_state.system_state;
}

uint16_t HydraulicControl_GetReversalFrequency(void)
{
    return g_control_state.rev_freq_per_min;
}

const hydraulic_control_state_t* HydraulicControl_GetState(void)
{
    return &g_control_state;
}
