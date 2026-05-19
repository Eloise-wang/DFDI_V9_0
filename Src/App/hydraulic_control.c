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

/*!
 * @brief 处理换向阀第二阶段（伸出/回缩循环控制）
 *
 * 对应 控制策略.md（二阶段控制逻辑）：
 * - 进入二阶段条件（不可逆）：在一阶段内检测到油压 oil_pressure >= set_extend_pressure 后进入二阶段；
 * - 二阶段手动模式流程（当前实现）：通过“压力阈值 + 定时动作”的组合实现伸出/回缩循环：
 *   1) 达到伸出压力（oil_pressure >= set_extend_pressure）→ 执行回缩动作，回缩持续 set_retract_time；
 *   2) 回缩时间结束后，等待油压降到回缩压力（oil_pressure <= set_retract_pressure）→ 执行伸出动作，伸出持续 set_extend_time；
 *   3) 伸出时间结束后，等待油压再次达到伸出压力 → 回到步骤1。
 *
 * 设计要点：
 * - 采用 set_extend_pressure / set_retract_pressure 形成滞回区间，避免单阈值导致的频繁抖动；
 * - 定时动作（extend/retract time）确保阀位切换后有“最小维持时间”，避免压力瞬时抖动造成频繁反向；
 * - auto_mode 当前仅作为预留接口（策略文档说明），未在此函数内改变流程；后续可在这里切换到真正自动算法。
 *
 * 与硬件动作的对应关系（基于现有 ValveControl_SetDirectionalValve 约定）：
 * - DirectionalValve = ON  ：视为“伸出/做功”方向；
 * - DirectionalValve = OFF ：视为“回缩/卸载”方向；
 *
 * 状态机说明（phase2_cycle_state）：
 * - WAIT_HIGH      ：等待油压到达伸出压力（并保持阀为ON）
 * - RETRACT_TIMED  ：开始回缩并定时保持 set_retract_time（阀强制OFF）
 * - WAIT_LOW       ：等待油压降到回缩压力（阀保持OFF）
 * - EXTEND_TIMED   ：开始伸出并定时保持 set_extend_time（阀强制ON）
 */
static void ProcessReversalValvePhase2(float oil_pressure)
{
    if (g_control_state.system_state != SYSTEM_STATE_REV_PHASE2) {
        return;
    }

    uint32_t now_ms = GetCurrentTimeMs();
    float extend_pressure = g_control_state.params.set_extend_pressure;
    float retract_pressure = g_control_state.params.set_retract_pressure;

    // 保护：若回缩压力设置高于伸出压力，会导致滞回区间反向、逻辑难以收敛
    // 这里将 retract_pressure 裁剪到不高于 extend_pressure，避免“永远等不到低阈值/高阈值”的卡死情形。
    if (retract_pressure > extend_pressure) {
        retract_pressure = extend_pressure;
    }

    float retract_time_ms_f = g_control_state.params.set_retract_time * 1000.0f;
    float extend_time_ms_f = g_control_state.params.set_extend_time * 1000.0f;

    // 时间保护：避免出现负时间导致比较逻辑异常
    if (retract_time_ms_f < 0.0f) retract_time_ms_f = 0.0f;
    if (extend_time_ms_f < 0.0f) extend_time_ms_f = 0.0f;

    // elapsed_ms：当前阶段的“计时参考点”为 rev_state_change_time（在进入某些状态时会重置）
    uint32_t elapsed_ms = now_ms - g_control_state.rev_state_change_time;
    bool valve_on = (ValveControl_GetDirectionalValveState() == VALVE_STATE_ON);

    switch (g_control_state.phase2_cycle_state)
    {
        case PHASE2_CYCLE_STATE_RETRACT_TIMED:
            // 回缩定时阶段：确保阀处于 OFF（回缩方向），并保持 set_retract_time
            if (valve_on) {
                ValveControl_SetDirectionalValve(false);
                g_control_state.rev_state_change_time = now_ms;
                RecordReversalEvent();
                elapsed_ms = 0U;
            }

            if (elapsed_ms >= (uint32_t)retract_time_ms_f) {
                // 回缩时间到 → 进入“等待低压阈值”阶段
                g_control_state.phase2_cycle_state = PHASE2_CYCLE_STATE_WAIT_LOW;
            }
            break;

        case PHASE2_CYCLE_STATE_WAIT_LOW:
            // 等待低阈值阶段：阀保持 OFF，直到油压下降到 retract_pressure
            if (valve_on) {
                ValveControl_SetDirectionalValve(false);
                g_control_state.rev_state_change_time = now_ms;
                RecordReversalEvent();
            }

            if (oil_pressure <= retract_pressure) {
                // 达到回缩压力阈值（低阈值）→ 转入“伸出定时阶段”
                g_control_state.phase2_cycle_state = PHASE2_CYCLE_STATE_EXTEND_TIMED;
                g_control_state.rev_state_change_time = now_ms;
            }
            break;

        case PHASE2_CYCLE_STATE_EXTEND_TIMED:
            // 伸出定时阶段：确保阀处于 ON（伸出方向），并保持 set_extend_time
            if (!valve_on) {
                ValveControl_SetDirectionalValve(true);
                g_control_state.rev_state_change_time = now_ms;
                RecordReversalEvent();
                elapsed_ms = 0U;
            }

            if (elapsed_ms >= (uint32_t)extend_time_ms_f) {
                // 伸出时间到 → 进入“等待高压阈值”阶段
                g_control_state.phase2_cycle_state = PHASE2_CYCLE_STATE_WAIT_HIGH;
            }
            break;

        case PHASE2_CYCLE_STATE_WAIT_HIGH:
        default:
            // 等待高阈值阶段：阀保持 ON，直到油压上升到 extend_pressure
            if (!valve_on) {
                ValveControl_SetDirectionalValve(true);
                g_control_state.rev_state_change_time = now_ms;
                RecordReversalEvent();
            }

            if (oil_pressure >= extend_pressure) {
                // 达到伸出压力阈值（高阈值）→ 转入“回缩定时阶段”
                g_control_state.phase2_cycle_state = PHASE2_CYCLE_STATE_RETRACT_TIMED;
                g_control_state.rev_state_change_time = now_ms;
            }
            break;
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
    // 二阶段参数默认值：
    // - auto_mode 默认 false（先按手动流程跑，自动模式仅预留接口）
    // - extend/retract 默认 0（若未配置，二阶段进入条件不会满足；或进入后定时阶段会立即结束）
    g_control_state.params.auto_mode = false;
    g_control_state.params.set_extend_time = 11.5f;
    g_control_state.params.set_extend_pressure = 4.5f;
    g_control_state.params.set_retract_time = 0.8f;
    g_control_state.params.set_retract_pressure = 0.3f;
    g_control_state.params.set_cooler_temperature_on = 55.0f;
    g_control_state.params.set_cooler_temperature_off = 50.0f;

    // 二阶段状态机默认从“等待高压阈值”开始：
    // 当系统进入二阶段时，会在 HydraulicControl_Process() 中强制切换为 RETRACT_TIMED 作为首动作，
    // 以满足策略“进入二阶段后，检测到达到伸出压力则回缩”的动作顺序。
    g_control_state.phase2_cycle_state = PHASE2_CYCLE_STATE_WAIT_HIGH;
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
            
            float target_duty = g_control_state.params.set_bypass_ratio;
            if (target_duty < 0.0f) target_duty = 0.0f;
            if (target_duty > 50.0f) target_duty = 50.0f;
            
            if (g_control_state.current_bypass_duty != target_duty) {
                g_control_state.current_bypass_duty = target_duty;
                ValveControl_SetBypassValve(g_control_state.current_bypass_duty);
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
        // 二阶段进入条件（不可逆）：
        // 一阶段运行中，当油压达到/超过 set_extend_pressure 时进入二阶段，并执行回缩动作作为二阶段的首个动作。
        if (oil_pressure >= g_control_state.params.set_extend_pressure) {
            g_control_state.system_state = SYSTEM_STATE_REV_PHASE2;
            g_control_state.rev_state_change_time = GetCurrentTimeMs();
            g_control_state.rev_phase2_start_time = GetCurrentTimeMs();
            g_control_state.phase2_cycle_state = PHASE2_CYCLE_STATE_RETRACT_TIMED;
        }
    }

    /* 5. 换向阀阶段调度 */
    switch (g_control_state.system_state)
    {
        case SYSTEM_STATE_REV_PHASE1:
            ProcessReversalValvePhase1();
            break;

        case SYSTEM_STATE_REV_PHASE2:
            ProcessReversalValvePhase2(oil_pressure);
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
