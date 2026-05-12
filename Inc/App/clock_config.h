/* Copyright Statement:

 */

#ifndef _CLOCK_CONFIG_H
#define _CLOCK_CONFIG_H

/*!
 * @file clock_config.h
 *
 * @brief 时钟配置函数定义
 *
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include <stdbool.h>
#include <stdint.h>
#include "ckgen_drv.h"

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/*! @brief 用户配置结构体 0 */
extern clock_manager_user_config_t clockMan1_InitConfig0;

/*! @brief 用户配置结构体数量 */
#define CLOCK_MANAGER_CONFIG_CNT 1U

/*! @brief 用户配置结构体指针数组 */
extern clock_manager_user_config_t const *g_clockManConfigsArr[];

/*! @brief 用户外设配置结构体 0 */
extern peripheral_clock_config_t peripheralClockConfig0[];

/*! @brief 外设时钟用户配置数量 */
#define NUM_OF_PERIPHERAL_CLOCKS_0 25U


/*! @brief 用户回调数量 */
#define CLOCK_MANAGER_CALLBACK_CNT 0U

/*! @brief 用户回调数组 */
extern clock_manager_callback_user_config_t *g_clockManCallbacksArr[];

/* ====================================  Functions declaration  ===================================== */

/*!
 * @brief 时钟配置初始化
*
* @param[in] none
* @return none
*/
void ClockConfig_Init(void);

/* ======================================  Functions define  ======================================== */



#endif /* _CLOCK_CONFIG_H */

/* =============================================  EOF  ============================================== */
