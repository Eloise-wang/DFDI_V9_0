/*!
 * @file system_init.h
 *
 * @brief 系统初始化模块 - 负责系统级初始化和硬件配置
 *
 * 功能说明：
 * - 硬件初始化（时钟、GPIO、PWM、UART等）
 * - 系统初始化流程管理
 * - 系统使能状态管理（由PC端system_enable CAN信号控制）
 *
 * @note PC17启动开关功能已暂时禁用，相关函数声明已注释保留供后期使用
 * @note 当前系统启动完全由PC端的system_enable CAN信号控制
 */

#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include <stdbool.h>

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Functions  =========================================== */

/*!
 * @brief 应用层系统初始化（统一入口）
 * 
 * 功能：
 * - 硬件初始化
 * - 核心模块初始化
 * - CAN通信模块初始化
 * - 任务调度器初始化
 * - 默认状态设置
 * 
 * @note 命名为 App_SystemInit 以避免与系统库中的 SystemInit 函数冲突
 * @note 系统启动由PC端的system_enable CAN信号控制
 */
void App_SystemInit(void);

/* ========== 启动开关相关函数（已注释，暂不使用） ========== */
/*!
 * @note PC17启动开关功能已暂时禁用
 * @note 当前系统启动由PC端的system_enable CAN信号控制
 * @note 以下函数声明保留供后期项目更新使用
 */
/*
//!
// * @brief 检查启动开关是否激活
// * @return true: 开关激活, false: 开关关闭
// */
//bool IsStartupSwitchActive(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_INIT_H */

