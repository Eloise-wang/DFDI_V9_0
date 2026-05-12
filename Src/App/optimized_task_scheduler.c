/*!
 * @file optimized_task_scheduler.c
 * @brief 优化的任务调度器实现
 *
 * 版本历史：
 * - v2.0 (2025-10-14): 性能优化，智能休眠、缓存时间戳
 * - v1.0 (2025-10-10): 初始版本，优先级调度
 *
 * 当前状态（v7.2+）：
 * - 9个任务运行（含2个CAN通信任务）
 * - CPU占用率：20-25%
 * - 系统调用：200-220次/秒
 */

#include "optimized_task_scheduler.h"
#include "unified_filter.h"
#include "valve_control.h"
#include "can_config.h"
#include "can_message_handler.h"
#include "fault_diagnosis.h"
#include "debug_log.h"
#include "hydraulic_control.h"
#include "sensor.h"
#include "common_types.h"
#include "osif.h"
#include <string.h>


/* ==========================================  Variables  =========================================== */

// 使用头文件配置的最大任务数，默认为10
#ifndef MAX_TASKS
#define MAX_TASKS MAX_TASKS_DEFAULT
#endif

static optimized_task_t g_tasks[MAX_TASKS];
static task_scheduler_status_t g_scheduler_status;
static bool g_scheduler_running = false;
static uint32_t g_scheduler_start_time = 0;

/* ==========================================  Functions  =========================================== */

/* 任务函数声明 */
static void Task_10ms_HydraulicControl(void);
static void Task_2000ms_SensorDataMonitor(void);

void OptimizedTaskScheduler_Init(void) {
    // 初始化任务数组
    memset(g_tasks, 0, sizeof(g_tasks));
    memset(&g_scheduler_status, 0, sizeof(task_scheduler_status_t));
    
    g_scheduler_running = false;
    g_scheduler_start_time = OSIF_GetMilliseconds();
    
}

void OptimizedTaskScheduler_Start(void) {
    g_scheduler_running = true;
    g_scheduler_start_time = OSIF_GetMilliseconds();
}

/*!
 * @brief 配置所有系统任务
 * 
 * 功能：
 * - 添加所有系统任务到调度器
 * - 设置任务周期和优先级
 */
void OptimizedTaskScheduler_ConfigureAllTasks(void)
{
    // CAN消息处理任务
    OptimizedTaskScheduler_AddTask(CAN_MessageHandler_Task_1ms_CANMessageProcess, 1, TASK_PRIORITY_CRITICAL);
    OptimizedTaskScheduler_AddTask(CAN_MessageHandler_Task_10ms_SendSensorData, 10, TASK_PRIORITY_HIGH);
    OptimizedTaskScheduler_AddTask(CAN_MessageHandler_Task_100ms_RealTimeCANMonitor, 100, TASK_PRIORITY_NORMAL);
    OptimizedTaskScheduler_AddTask(CAN_MessageHandler_Task_1000ms_CANStatusMonitor, 1000, TASK_PRIORITY_LOW);
    
    // 液压控制任务（10ms周期，与控制逻辑同步）
    OptimizedTaskScheduler_AddTask(Task_10ms_HydraulicControl, 10, TASK_PRIORITY_HIGH);
    
    // 传感器数据监控任务
    OptimizedTaskScheduler_AddTask(Task_2000ms_SensorDataMonitor, 2000, TASK_PRIORITY_LOW);
    
    // CAN错误检测任务（5000ms周期）
    OptimizedTaskScheduler_AddTask(CAN_MessageHandler_ErrorCheck, 5000, TASK_PRIORITY_LOW);
}

void OptimizedTaskScheduler_Stop(void) {
    g_scheduler_running = false;
}

void OptimizedTaskScheduler_Update(void) {
    if (!g_scheduler_running) return;
    
    uint32_t current_time = OSIF_GetMilliseconds();
    g_scheduler_status.scheduler_uptime = current_time - g_scheduler_start_time;
    g_scheduler_status.last_update_time = current_time;
    
    // 更新任务状态统计
    g_scheduler_status.total_tasks = 0;
    g_scheduler_status.active_tasks = 0;
    g_scheduler_status.suspended_tasks = 0;
    g_scheduler_status.error_tasks = 0;
    
    for (int i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].task_function != NULL) {
            g_scheduler_status.total_tasks++;
            
            switch (g_tasks[i].state) {
                case TASK_STATE_READY:
                case TASK_STATE_RUNNING:
                    g_scheduler_status.active_tasks++;
                    break;
                case TASK_STATE_SUSPENDED:
                    g_scheduler_status.suspended_tasks++;
                    break;
                case TASK_STATE_ERROR:
                    g_scheduler_status.error_tasks++;
                    break;
            }
        }
    }
}

void OptimizedTaskScheduler_MainLoop(void) {
    
    // 主调度循环 - 标准版优化 v2.0
    while (g_scheduler_running) {
        // ✅ 优化1：每轮循环只获取一次时间戳（缓存优化）
        uint32_t loop_time = OSIF_GetMilliseconds();
        bool any_task_executed = false;
        uint32_t next_task_time = UINT32_MAX;
        
        // 按优先级顺序执行任务
        for (int priority = TASK_PRIORITY_CRITICAL; priority <= TASK_PRIORITY_LOW; priority++) {
            for (int i = 0; i < MAX_TASKS; i++) {
                optimized_task_t *task = &g_tasks[i];
                
                // 快速跳过无效任务
                if (task->task_function == NULL) continue;
                if (!task->enabled) continue;
                if (task->state == TASK_STATE_SUSPENDED) continue;
                if (task->priority != priority) continue;
                
                // ✅ 优化2：使用缓存的时间戳计算
                uint32_t time_since_last_run = loop_time - task->last_run_time;
                
                // 检查是否到了执行时间
                if (time_since_last_run >= task->period_ms) {
                    // 执行任务
                    task->state = TASK_STATE_RUNNING;
                    
                    #if ENABLE_TASK_PROFILING
                    // 性能分析模式：记录执行时间
                    uint32_t task_start = OSIF_GetMilliseconds();
                    #endif
                    
                    task->task_function();
                    
                    #if ENABLE_TASK_PROFILING
                    uint32_t task_end = OSIF_GetMilliseconds();
                    task->actual_execution_time = task_end - task_start;
                    
                    // 检查执行时间是否超限
                    if (task->actual_execution_time > task->max_execution_time) {
                        // 任务执行时间超限（无打印）
                    }
                    #endif
                    
                    // ✅ 使用缓存的时间戳更新
                    task->last_run_time = loop_time;
                    task->run_count++;
                    task->state = TASK_STATE_READY;
                    
                    any_task_executed = true;
                } else {
                    // ✅ 优化3：计算下一次执行时间（智能休眠）
                    uint32_t time_until_next_run = task->period_ms - time_since_last_run;
                    if (time_until_next_run < next_task_time) {
                        next_task_time = time_until_next_run;
                    }
                }
            }
        }
        
        // ✅ 优化4：智能休眠机制
        if (!any_task_executed) {
            uint32_t sleep_time = 1;  // 默认休眠1ms
            
            if (next_task_time != UINT32_MAX && next_task_time > 0) {
                // 根据下一个任务时间调整休眠时长
                // 提前1ms唤醒，确保不错过任务
                sleep_time = (next_task_time > 1) ? (next_task_time - 1) : 1;
                
                // 限制最大休眠时间为100ms（保持系统响应性）
                if (sleep_time > 100) {
                    sleep_time = 1;
                }
            }
            
            OSIF_TimeDelay(sleep_time);
        }
    }
    
}

int32_t OptimizedTaskScheduler_AddTask(void (*task_function)(void), 
                                       uint32_t period_ms, 
                                       uint8_t priority) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].task_function == NULL) {
            g_tasks[i].task_function = task_function;
            g_tasks[i].period_ms = period_ms;
            g_tasks[i].priority = priority;
            g_tasks[i].state = TASK_STATE_READY;
            g_tasks[i].enabled = true;
            g_tasks[i].last_run_time = OSIF_GetMilliseconds();
            g_tasks[i].run_count = 0;
            g_tasks[i].max_execution_time = period_ms / 2; // 最大执行时间为周期的一半
            g_tasks[i].actual_execution_time = 0;
            
            return i;
        }
    }
    
    return -1;
}

bool OptimizedTaskScheduler_RemoveTask(int32_t task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return false;
    
    if (g_tasks[task_id].task_function != NULL) {
        memset(&g_tasks[task_id], 0, sizeof(optimized_task_t));
        return true;
    }
    
    return false;
}

bool OptimizedTaskScheduler_SetTaskEnabled(int32_t task_id, bool enabled) {
    if (task_id < 0 || task_id >= MAX_TASKS) return false;
    
    if (g_tasks[task_id].task_function != NULL) {
        g_tasks[task_id].enabled = enabled;
        return true;
    }
    
    return false;
}

bool OptimizedTaskScheduler_SetTaskSuspended(int32_t task_id, bool suspended) {
    if (task_id < 0 || task_id >= MAX_TASKS) return false;
    
    if (g_tasks[task_id].task_function != NULL) {
        g_tasks[task_id].state = suspended ? TASK_STATE_SUSPENDED : TASK_STATE_READY;
        return true;
    }
    
    return false;
}

const optimized_task_t* OptimizedTaskScheduler_GetTaskStatus(int32_t task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return NULL;
    
    if (g_tasks[task_id].task_function != NULL) {
        return &g_tasks[task_id];
    }
    
    return NULL;
}

const task_scheduler_status_t* OptimizedTaskScheduler_GetSchedulerStatus(void) {
    return &g_scheduler_status;
}

uint32_t OptimizedTaskScheduler_GetTaskRunCount(int32_t task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return 0;
    
    if (g_tasks[task_id].task_function != NULL) {
        return g_tasks[task_id].run_count;
    }
    
    return 0;
}

uint32_t OptimizedTaskScheduler_GetTaskAverageExecutionTime(int32_t task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return 0;
    
    if (g_tasks[task_id].task_function != NULL) {
        return g_tasks[task_id].actual_execution_time;
    }
    
    return 0;
}

void OptimizedTaskScheduler_ResetTaskStats(int32_t task_id) {
    if (task_id == -1) {
        // 重置所有任务统计
        for (int i = 0; i < MAX_TASKS; i++) {
            if (g_tasks[i].task_function != NULL) {
                g_tasks[i].run_count = 0;
                g_tasks[i].actual_execution_time = 0;
            }
        }
    } else if (task_id >= 0 && task_id < MAX_TASKS) {
        // 重置指定任务统计
        if (g_tasks[task_id].task_function != NULL) {
            g_tasks[task_id].run_count = 0;
            g_tasks[task_id].actual_execution_time = 0;
        }
    }
}

void OptimizedTaskScheduler_PrintStatus(void) {
    // 任务调度器状态检查（无打印）
}

void OptimizedTaskScheduler_PrintStats(void) {
    // 任务调度器统计信息（无打印）
}

bool OptimizedTaskScheduler_IsRunning(void) {
    return g_scheduler_running;
}

/* ==================== 预定义任务函数实现 ==================== */

/*!
 * @brief 任务：液压控制逻辑（10ms周期）
 * 执行条件：传感器值在正常测量范围内，系统使能检查在 HydraulicControl_Process() 内部进行
 */
void Task_10ms_HydraulicControl(void)
{
    float oil_pressure = Sensor_GetOilPressure();
    float oil_temperature = Sensor_GetOilTemperature();
    // 【关键修改】调用 CAN 模块中已经计算好的滤波罐压
    float filtered_lng_p = CAN_MessageHandler_GetFilteredLNGPressure();
    float lng_temperature = Sensor_GetLNGTemperature();
    
    // 传感器范围检查（执行控制逻辑前的安全检查）
    bool sensors_ok =
        Sensor_ValidateValue(oil_pressure, OIL_PRESSURE_MEAS_MIN_MPA, OIL_PRESSURE_MEAS_MAX_MPA) &&
        Sensor_ValidateValue(filtered_lng_p, LNG_PRESSURE_MEAS_MIN_MPA, LNG_PRESSURE_MEAS_MAX_MPA) &&
        Sensor_ValidateValue(oil_temperature, OIL_TEMP_MEAS_MIN_C, OIL_TEMP_MEAS_MAX_C) &&
        Sensor_ValidateValue(lng_temperature, LNG_TEMP_MEAS_MIN_C, LNG_TEMP_MEAS_MAX_C);
    
    if (!sensors_ok) {
        return;  // 传感器超量程，不执行控制逻辑
    }
    
    HydraulicControl_Process(oil_pressure, oil_temperature, filtered_lng_p);
}

/*!
 * @brief 任务：传感器数据监控（2000ms周期）
 */
void Task_2000ms_SensorDataMonitor(void)
{
    Debug_Log_SensorDataMonitor();
}
