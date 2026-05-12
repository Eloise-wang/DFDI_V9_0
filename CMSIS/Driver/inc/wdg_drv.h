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
 * @file wdg_drv.h
 *
 * @brief This file provides wdg integration functions interface.
 *
 */

#ifndef _WDG_DRV_H
#define _WDG_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================  Include  ============================================= */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< Callback for WDG module */
typedef void (*wdg_callback_t)(void);

/*!
 * @brief Clock source for the WDG.
 */
typedef enum
{
    WDG_LSI_CLOCK       = 0x00U,             /*!< LSI clock */
    WDG_BUS_CLOCK       = 0x01U,             /*!< BUS clock */
    WDG_HSI_CLOCK       = 0x02U,             /*!< HSI clock */
    WDG_HSE_CLOCK       = 0x03U              /*!< HSE clock */
} wdg_clk_source_t;

/*!
 * @brief Test modes for the WDG.
 */
typedef enum
{
    WDG_TST_DISABLED    = 0x00U,             /*!< Test mode disabled */
    WDG_TST_ENABLED     = 0x02U              /*!< Test mode enabled */
} wdg_test_mode_t;

/*!
 * @brief Set modes for the WDG.
 */
typedef enum
{
    WDG_DEBUG_MODE    = 0x00U,               /*!< Debug mode */
    WDG_STOP_MODE     = 0x01U                /*!< Stop mode */
} wdg_set_mode_t;

/*!
 * @brief WDG option mode configuration structure.
 */
typedef struct
{
    bool stopEn;                               /*!< Enable/disable stop mode */
    bool debugEn;                              /*!< Enable/disable debug mode */
} wdg_op_mode_t;

/*!
 * @brief WDG user configuration structure.
 */
typedef struct
{
    uint16_t          windowValue;  /*!< The window Value                                        */
    uint16_t          timeoutValue; /*!< The timeout Value                                       */
    wdg_clk_source_t  clkSource;    /*!< The clock source of the WDG                             */
    bool              updateEn;     /*!< If ture, further updates of the WDG are enabled         */
    bool              prescalerEn;  /*!< If ture, a fixed 256 prescaling is enabled              */
    bool              intEn;        /*!< If ture, an interrupt request is generated before reset */
    bool              winEn;        /*!< If ture, window mode is enabled                         */
    wdg_op_mode_t     opMode;       /*!< The modes in which the WDG is functional                */
} wdg_user_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize WDG driver based on configuration input.
 *
 * @param[in] instance: The WDG instance number
 * @param[in] userConfigPtr: WDG configuration pointer
 * @return The result of execution
 *         - STATUS_SUCCESS: Initialize successful
 *         - STATUS_ERROR: Initialize failed
 */
status_t WDG_DRV_Init(uint8_t instance, const wdg_user_config_t *userConfigPtr);

/*!
 * @brief De-initialize WDG driver
 *
 * @param[in] instance: The WDG instance number
 * @return The result of execution
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation was failed
 */
status_t WDG_DRV_Deinit(uint8_t instance);

/*!
 * @brief Gets the current configuration of the WDG
 *
 * @param[in] instance: The WDG instance number
 * @param[out] config: Pointer to the current configuration
 * @return none
 */
void WDG_DRV_GetConfig(uint8_t instance, wdg_user_config_t * const config);

/*!
 * @brief Gets default configuration of the WDG
 *
 * @param[out] config: Pointer to the default configuration
 * @return none
 */
void WDG_DRV_GetDefaultConfig(wdg_user_config_t * const config);

/*!
 * @brief Set the WDG timeout interrupt
 *
 * @param[in] instance: The WDG instance number
 * @param[in] enable: Enable/disable WDG timeout interrupt
 * @return the result of operation
 *         - STATUS_SUCCESS: set successful
 *         - STATUS_ERROR: set failed
 */
status_t WDG_DRV_SetInt(uint8_t instance, bool enable);

/*!
 * @brief Clear interrupt flag of the WDG
 *
 * @param[in] instance: The WDG instance number
 * @return none
 */
void WDG_DRV_ClearIntFlag(uint8_t instance);

/*!
 * @brief Refreshes the WDG counter
 *
 * @param[in] instance: The WDG instance number
 * @return none
 */
void WDG_DRV_Trigger(uint8_t instance);

/*!
 * @brief Gets the value of the WDG counter
 *
 * @param[in] instance: The WDG instance number
 * @return the value of the WDG counter
 */
uint16_t WDG_DRV_GetCounter(uint8_t instance);

/*!
 * @brief Set window mode and window value of the WDG
 *
 * @param[in] instance: The WDG instance number
 * @param[in] enable: Enable/disable window mode
 * @param[in] windowValue: The value of the WDG window
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t WDG_DRV_SetWindow(uint8_t instance, bool enable, uint16_t windowValue);

/*!
 * @brief Sets the mode operation of the WDG
 *
 * @param[in] instance: The WDG instance number
 * @param[in] enable: Enable/disable window mode
 * @param[in] setMode: Select mode of the WDG
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t WDG_DRV_SetMode(uint8_t instance, bool enable, wdg_set_mode_t setMode);

/*!
 * @brief Sets the value of the WDG timeout
 *
 * @param[in] instance: The WDG instance number
 * @param[in] timeout: The value of the WDG timeout
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t WDG_DRV_SetTimeout(uint8_t instance, uint16_t timeout);

/*!
 * @brief Sets the WDG test mode
 *
 * @param[in] instance: The WDG instance number
 * @param[in] testMode: Test modes for the WDG
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t WDG_DRV_SetTestMode(uint8_t instance, wdg_test_mode_t testMode);

/*!
 * @brief Gets the WDG test mode
 *
 * @param[in] instance: The WDG instance number
 * @return test modes for the WDG
 */
wdg_test_mode_t WDG_DRV_GetTestMode(uint8_t instance);

/*!
 * @brief Set WDG callback function
 *
 * @param[in] instance: WDG instance
 * @param[in] function: The callback function of the WDG
 * @return none
 */
void WDG_DRV_InstallCallback(uint8_t instance, wdg_callback_t function);

/*!
 * @brief WDG tnterrupt handler
 *
 * @param[in] instance: WDG instance
 * @return none
 */
void WDG_DRV_IRQHandler(uint8_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WDG_DRV_H */

/* =============================================  EOF  ============================================== */
