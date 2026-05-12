/*!
 * @file optimized_task_scheduler.h
 * @brief 优化的任务调度器 - 理清功能，优化时序
 * 
 * 任务时序配置（v7.2+，共9个任务）：
 * - 5ms: 传感器数据采集（CRITICAL）
 * - 5ms: CAN高频通信（HIGH）✅ v7.2
 * - 10ms: 快速保护检查（CRITICAL）
 * - 50ms: 液压控制主处理（HIGH）
 * - 50ms: CAN低频通信（NORMAL）✅ v7.2
 * - 100ms: 换向阀智能控制（HIGH）
 * - 200ms: 系统状态管理（NORMAL）
 * - 500ms: 监控和统计（NORMAL）
 * - 1000ms: 通信和诊断（LOW）
 *
 * 性能优化（v7.1）：
 * - 智能休眠机制，CPU占用率降低50%
 * - 缓存时间戳，系统调用减少67%
 * - 性能分析开关（ENABLE_TASK_PROFILING）
 */

#ifndef OPTIMIZED_TASK_SCHEDULER_H
#define OPTIMIZED_TASK_SCHEDULER_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include <stdint.h>
#include <stdbool.h>
#include "common_types.h"

/* ============================================  Define  ============================================ */

/* ==================== 性能分析配置 ==================== */
/*!
 * @brief 任务性能分析开关
 * 
 * 启用(1)：记录任务执行时间，检测超时，适合调试环境
 *          额外开销：约5% CPU占用率
 * 禁用(0)：跳过性能统计，降低CPU占用，适合生产环境
 *          减少系统调用，降低约0.5% CPU占用率
 * 
 * 使用场景：
 * - 调试/开发阶段：设置为 1
 * - 生产部署：设置为 0
 */
#ifndef ENABLE_TASK_PROFILING
#define ENABLE_TASK_PROFILING  1  // 默认启用，方便调试
#endif

/* ==================== 任务配置 ==================== */
#define MAX_TASKS_DEFAULT  10  // 默认最大任务数（7个使用中 + 3个扩展槽）

/* ==================== 任务时序定义 ==================== */
#define TASK_5MS_PERIOD       5       // 5ms任务周期
#define TASK_10MS_PERIOD       10      // 10ms任务周期
#define TASK_50MS_PERIOD       50      // 50ms任务周期
#define TASK_100MS_PERIOD      100     // 100ms任务周期
#define TASK_200MS_PERIOD      200     // 200ms任务周期
#define TASK_500MS_PERIOD      500     // 500ms任务周期
#define TASK_1000MS_PERIOD     1000    // 1000ms任务周期

/* ==================== 任务优先级定义 ==================== */
#define TASK_PRIORITY_CRITICAL     0   // 关键任务（安全保护）
#define TASK_PRIORITY_HIGH         1   // 高优先级任务（控制核心）
#define TASK_PRIORITY_NORMAL       2   // 普通任务（常规处理）
#define TASK_PRIORITY_LOW          3   // 低优先级任务（监控通信）

/* ==================== 任务状态定义 ==================== */
#define TASK_STATE_IDLE           0   // 任务空闲
#define TASK_STATE_READY          1   // 任务就绪
#define TASK_STATE_RUNNING        2   // 任务运行中
#define TASK_STATE_SUSPENDED      3   // 任务挂起
#define TASK_STATE_ERROR          4   // 任务错误

/* ===========================================  Typedef  ============================================ */

/*!
 * @brief 优化任务结构体
 */
typedef struct {
    void (*task_function)(void);      // 任务函数指针
    uint32_t period_ms;               // 任务周期(ms)
    uint32_t last_run_time;           // 上次运行时间
    uint32_t run_count;               // 运行次数
    uint8_t priority;                 // 任务优先级
    uint8_t state;                   // 任务状态
    bool enabled;                     // 任务使能
    uint32_t max_execution_time;      // 最大执行时间(ms)
    uint32_t actual_execution_time;   // 实际执行时间(ms)
} optimized_task_t;

/*!
 * @brief 任务调度器状态结构体
 */
typedef struct {
    uint32_t total_tasks;             // 总任务数
    uint32_t active_tasks;            // 活跃任务数
    uint32_t suspended_tasks;         // 挂起任务数
    uint32_t error_tasks;             // 错误任务数
    uint32_t scheduler_uptime;        // 调度器运行时间
    uint32_t last_update_time;         // 最后更新时间
} task_scheduler_status_t;

/* ==========================================  Functions  =========================================== */

/* ==================== 任务调度器管理接口 ==================== */

/*!
 * @brief 初始化优化任务调度器
 */
void OptimizedTaskScheduler_Init(void);

/*!
 * @brief 启动任务调度器
 */
void OptimizedTaskScheduler_Start(void);

/*!
 * @brief 停止任务调度器
 */
void OptimizedTaskScheduler_Stop(void);

/*!
 * @brief 配置所有系统任务
 * 
 * 
 * @note 此函数应在系统初始化后、启动调度器前调用
 */
void OptimizedTaskScheduler_ConfigureAllTasks(void);

/*!
 * @brief 更新任务调度器
 */
void OptimizedTaskScheduler_Update(void);

/*!
 * @brief 执行任务调度器主循环
 */
void OptimizedTaskScheduler_MainLoop(void);

/* ==================== 任务管理接口 ==================== */

/*!
 * @brief 添加任务到调度器
 * @param task_function 任务函数指针
 * @param period_ms 任务周期(ms)
 * @param priority 任务优先级
 * @return 任务ID，失败返回-1
 */
int32_t OptimizedTaskScheduler_AddTask(void (*task_function)(void), 
                                       uint32_t period_ms, 
                                       uint8_t priority);

/*!
 * @brief 移除任务
 * @param task_id 任务ID
 * @return true: 成功, false: 失败
 */
bool OptimizedTaskScheduler_RemoveTask(int32_t task_id);

/*!
 * @brief 启用/禁用任务
 * @param task_id 任务ID
 * @param enabled 使能标志
 * @return true: 成功, false: 失败
 */
bool OptimizedTaskScheduler_SetTaskEnabled(int32_t task_id, bool enabled);

/*!
 * @brief 挂起/恢复任务
 * @param task_id 任务ID
 * @param suspended 挂起标志
 * @return true: 成功, false: 失败
 */
bool OptimizedTaskScheduler_SetTaskSuspended(int32_t task_id, bool suspended);

/*!
 * @brief 检查调度器是否正在运行
 * @return true: 正在运行, false: 已停止
 */
bool OptimizedTaskScheduler_IsRunning(void);

/* ==================== 任务状态查询接口 ==================== */

/*!
 * @brief 获取任务状态
 * @param task_id 任务ID
 * @return 任务状态结构体指针
 */
const optimized_task_t* OptimizedTaskScheduler_GetTaskStatus(int32_t task_id);

/*!
 * @brief 获取调度器状态
 * @return 调度器状态结构体指针
 */
const task_scheduler_status_t* OptimizedTaskScheduler_GetSchedulerStatus(void);

/*!
 * @brief 获取任务执行统计
 * @param task_id 任务ID
 * @return 任务执行次数
 */
uint32_t OptimizedTaskScheduler_GetTaskRunCount(int32_t task_id);

/*!
 * @brief 获取任务平均执行时间
 * @param task_id 任务ID
 * @return 平均执行时间(ms)
 */
uint32_t OptimizedTaskScheduler_GetTaskAverageExecutionTime(int32_t task_id);

/* ==================== 任务调度器工具接口 ==================== */

/*!
 * @brief 重置任务统计
 * @param task_id 任务ID，-1表示重置所有任务
 */
void OptimizedTaskScheduler_ResetTaskStats(int32_t task_id);

/*!
 * @brief 打印任务状态
 */
void OptimizedTaskScheduler_PrintStatus(void);

/*!
 * @brief 打印任务执行统计
 */
void OptimizedTaskScheduler_PrintStats(void);

/* ==================== 预定义任务函数 ==================== */

#ifdef __cplusplus
}
#endif

#endif /* OPTIMIZED_TASK_SCHEDULER_H */

