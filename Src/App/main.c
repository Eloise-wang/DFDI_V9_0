/*!
* @file main.c
*
* @brief 高压液压控制系统主程序 - 嵌入式控制版本
*
* 版本历史：
* - v9.0 (2025-01-XX): 嵌入式控制架构 - 控制算法在嵌入式端执行，PC端负责参数配置和数据显示
* - v8.0 (2025-10-22): 平台无关化 - 只负责数据采集和执行，控制算法移至PC端
* - v7.3 (2025-10-15): 代码深度清理

*/

/* ===========================================  Includes  =========================================== */
#include "optimized_task_scheduler.h"
#include "can_config.h"
#include "can_message_handler.h"
#include "system_init.h"
#include "debug_log.h"

/* ============================================  Define  ============================================ */

/* ==========================================  Variables  =========================================== */
// 系统使能状态（供其他模块使用）
bool g_systemEnabled = false;

/* ======================================  Functions define  ======================================== */

/*!
 * @brief 最小错误处理实现
 */
void ErrorHandler_Process(void)
{
    /* no-op */
}

/*!
* @brief 主函数 - 嵌入式控制版本
* 
* 功能说明：
* - 传感器数据采集和通过CAN发送给PC端显示
* - 接收PC端的参数配置（温度阈值、压力阈值、旁通阀开度等）
* - 在嵌入式端执行控制算法逻辑（温度控制、压力控制、换向阀逻辑等）
* - 根据算法结果自动控制阀门硬件
*/
int main(void)
{
    // 系统初始化
    App_SystemInit();
    
    // CAN消息处理模块初始化
    CAN_MessageHandler_Init();
    
    // 注册CAN接收回调
    CAN_Config_RegisterRxCallback(CAN_MessageHandler_RxCallback);
    
    // 配置所有系统任务
    OptimizedTaskScheduler_ConfigureAllTasks();
    
    // 启动任务调度器
    OptimizedTaskScheduler_Start();
    
    // 主循环
    while (1) {
			  /* ============================= */
        /* CAN 接收处理（最关键）     */
        /* ============================= */
				CAN_MessageHandler_Task_1ms_CANMessageProcess();
        // 执行任务调度（内部包含智能休眠机制）
        OptimizedTaskScheduler_MainLoop();
    }
}

/* =============================================  EOF  ============================================== */
