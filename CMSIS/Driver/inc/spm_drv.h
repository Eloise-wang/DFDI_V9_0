/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of AutoChips Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2021. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT AUTOCHIPS'S OPTION, TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */

/*!
 * @file spm_drv.h
 *
 * @brief This file provides spm module integration functions interfaces.
 *
 */

#ifndef _SPM_DRV_H
#define _SPM_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< SPM callback function define. */
typedef void (*spm_callback_t)(void *device, uint32_t wpara, uint32_t lpara);

/*!
 * @brief Config the ack timeout action.
 */
typedef enum
{
    ACK_TIMEOUT_ACTION_INTERRUPT  = 0U,   /*!< interrupt after sleep timeout occurs */
    ACK_TIMEOUT_ACTION_RESET      = 1U    /*!< reset after sleep timeout occurs     */
} spm_ack_timeout_action_t;

/*!
 * @brief Config the power mode enumeration.
 */
typedef enum
{
    POWER_MODE_RUN    = 0U,    /*!< run mode    */
    POWER_MODE_VLPR   = 1U,    /*!< vlpr mode   */
    POWER_MODE_STOP1  = 2U,    /*!< stop1 mode  */
    POWER_MODE_STOP2  = 3U,    /*!< stop2 mode  */
    POWER_MODE_VLPS   = 4U,    /*!< vlps mode   */
    POWER_MODE_STANDBY = 5U    /*!< standby mode */
} spm_power_mode_t;

/*!
 * @brief Config the lvd threshold enumeration.
 */
typedef enum
{
    LVD_LOW_THRESHOLD   = 0U,  /*!< lvd low threshold  */
    LVD_HIGH_THRESHOLD  = 1U   /*!< lvd high threshold */
} spm_lvd_threshold_t;

/*!
 * @brief Config the lvr threshold enumeration.
 */
typedef enum
{
    LVR_HIGH_THRESHOLD  = 0U,  /*!< lvr high threshold */
    LVR_LOW_THRESHOLD   = 1U   /*!< lvr low threshold */
} spm_lvr_threshold_t;

/* ==========================================  Variables  =========================================== */
/*!
 * @brief Standby wakeup source enumeration.
 */
typedef enum
{
    STANDBY_WAKEUP_PA12 = 0U,   /*!< PA12 wakeup */
    STANDBY_WAKEUP_PB0  = 1U,   /*!< PB0  wakeup */
    STANDBY_WAKEUP_PB1  = 2U,   /*!< PB1  wakeup */
    STANDBY_WAKEUP_PB12 = 3U,   /*!< PB12 wakeup */
    STANDBY_WAKEUP_PD3  = 4U,   /*!< PD3  wakeup */
    STANDBY_WAKEUP_PC2  = 5U,   /*!< PC2  wakeup */
    STANDBY_WAKEUP_PC3  = 6U,   /*!< PC3  wakeup */
    STANDBY_WAKEUP_PC6  = 7U,   /*!< PC6  wakeup */
    STANDBY_WAKEUP_PC7  = 8U,   /*!< PC7  wakeup */
    STANDBY_WAKEUP_PC16 = 9U,   /*!< PC16 wakeup */
    STANDBY_WAKEUP_PC17 = 10U,  /*!< PC17 wakeup */
    STANDBY_WAKEUP_PD6  = 11U,  /*!< PD6  wakeup */
    STANDBY_WAKEUP_PD7  = 12U,  /*!< PD7  wakeup */
    STANDBY_WAKEUP_PE4  = 13U,  /*!< PE4  wakeup */
    STANDBY_WAKEUP_PE5  = 14U,  /*!< PE5  wakeup */
    STANDBY_WAKEUP_RTC  = 15U   /*!< RTC  wakeup */
} spm_standby_wakeup_source_t;

/*!
 * @brief Standby wakeup type enumeration.
 */
typedef enum
{
    STANDBY_WAKEUP_DISABLE      = 0U,  /*!< wakeup disable */
    STANDBY_WAKEUP_RISING_EDGE  = 1U,  /*!< rising edge wakeup enable */
    STANDBY_WAKEUP_FALLING_EDGE = 2U,  /*!< falling edge wakeup enable */
    STANDBY_WAKEUP_EITHER_EDGE  = 3U   /*!< either edge wakeup enable */
} spm_standby_wakeup_t;
/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Set the power mode.
 *
 * @param[in] mode: power mode, value can be
 *            - POWER_MODE_RUN
 *            - POWER_MODE_VLPR
 *            - POWER_MODE_STOP1
 *            - POWER_MODE_STOP2
 *            - POWER_MODE_VLPS
 * @return operation status
 *         - STATUS_SUCCESS: mode switch success
 *         - STATUS_ERROR: mode switch error
 */
status_t SPM_DRV_SetPowerMode(spm_power_mode_t mode);

/*!
 * @brief Get the current power mode.
 *
 * @param[in] none
 * @return the current power mode,value can be
 *         - POWER_MODE_RUN
 *         - POWER_MODE_VLPR
 *         - POWER_MODE_STOP1
 *         - POWER_MODE_STOP2
 *         - POWER_MODE_VLPS
 */
spm_power_mode_t SPM_DRV_GetPowerMode(void);

/*!
 * @brief Get the power control mode.
 *
 * @param[in] none
 * @return the power control mode,value can be
 *         - POWER_MODE_RUN
 *         - POWER_MODE_VLPR
 *         - POWER_MODE_STOP1
 *         - POWER_MODE_STOP2
 *         - POWER_MODE_VLPS
 */
spm_power_mode_t SPM_DRV_GetPowerControlMode(void);

/*!
 * @brief Select lvd threshold.
 *
 * @param[in] threshold value
 *            - LVD_LOW_THRESHOLD
 *            - LVD_HIGH_THRESHOLD
 * @return none
 */
void SPM_DRV_SelectLVDThreshold(spm_lvd_threshold_t threshold);

/*!
 * @brief Select lvr threshold, it is recommended to use low gear only in VLPS mode.
 *
 * @param[in] threshold value
 *            - LVR_HIGH_THRESHOLD
 *            - LVR_LOW_THRESHOLD
 * @return none
 */
void SPM_DRV_SelectLVRThreshold(spm_lvr_threshold_t threshold);

/*!
 * @brief Set SPM IRQHandler callback function.
 *
 * @param[in] eventFunc: the pointer of the call back function, which will be called in SPM_IRQHandler
 * @return none
 */
void SPM_DRV_InstallCallback(const spm_callback_t eventFunc);

/*!
 * @brief Set LVD Handler callback function.
 *
 * @param[in] eventFunc: the pointer of the call back function, which will be called in LVD_IRQHandler
 * @return none
 */
void LVD_DRV_InstallCallback(const spm_callback_t eventFunc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SPM_DRV_H */

/* =============================================  EOF  ============================================== */
