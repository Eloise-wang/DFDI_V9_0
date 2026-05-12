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
 * @file rtc_drv.h
 *
 * @brief This file provides i2c integration functions interface.
 *
 */

#ifndef _RTC_DRV_H
#define _RTC_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================  Include  ============================================= */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief RTC CLKOUT pin configuration
 */
typedef enum
{
    RTC_RUNMODE_CLK = 0x00U,        /*!< RTC run mode clk source select */
    RTC_STANDBY_LSI_CLK  = 0x01U,   /*!< RTC standby mode lsi clk source select */
    RTC_STANDBY_RTCIN_CLK  = 0x02U  /*!< RTC standby mode rtcin clk source select */
} rtc_clksource_select_t;

/*!
 * @brief RTC CLKOUT pin configuration
 */
typedef enum
{
    RTC_CLKOUT_DISABLED = 0x00U,     /*!< Clock out pin is disabled */
    RTC_CLKOUT_SRC_PSO  = 0x01U,     /*!< Output on RTC_CLKOUT of prescaler overflow */
    RTC_CLKOUT_SRC_CLK  = 0x02U      /*!< Output on RTC_CLKOUT of RTC CLKIN */
} rtc_clk_out_config_t;

/*!
 * @brief RTC register lock
 */
typedef enum
{
    RTC_LOCK_REG_LOCK    = 0x00U,   /*!< RTC lock register lock */
    RTC_STATUS_REG_LOCK  = 0x01U,   /*!< RTC status register lock */
    RTC_CTRL_REG_LOCK    = 0x02U    /*!< RTC control register lock */
} rtc_lock_register_select_t;

/*!
 * @brief RTC register lock configuration
 */
typedef struct
{
    bool lockRegisterLock;                /*!< Lock state of the lock register */
    bool statusRegisterLock;              /*!< Lock state of the status register */
    bool controlRegisterLock;             /*!< Lock state of the control register */
} rtc_register_lock_config_t;

/*!
 * @brief RTC alarm configuration
 */
typedef struct
{
    uint32_t alarmValue;                             /*!< Alarm time value */
    uint32_t repetitionInterval;                     /*!< Interval of repetition in time */
    uint32_t numberOfRepeats;                        /*!< Number of alarm repeats */
    bool repeatForever;                              /*!< Repeat forever if set, discard number of repeats */
    bool   timeAlarmIntEnable;                       /*!< Enable time alarm interrupt */
    void  (*alarmCallback)(void *callbackParam);     /*!< Pointer to the user callback method */
    void  *callbackParams;                           /*!< Pointer to the callback parameters */
} rtc_alarm_config_t;

/*!
 * @brief RTC initialization structure
 */
typedef struct
{
    rtc_clksource_select_t clockSrcSelect;  /*!< RTC clock source select */
    rtc_clk_out_config_t  clockOutConfig;   /*!< RTC clock out source */
    bool updateEn;                          /*!< Enable changing the TCE bit even if the status register is locked */
    uint32_t psrValue;                      /*!< RTC prescaler value */
    uint32_t timeCnt;                       /*!< RTC real time counter value */
} rtc_init_config_t;

/*!
 * @brief RTC interrupt configuration
 */
typedef struct
{
    bool   timeOverflowIntEnable;                      /*!< Enable time overflow interrupt */
    bool   timeInvalidIntEnable;                       /*!< Enable time invalid interrupt */
    bool   presOverflowIntEnable;                      /*!< Enable time prescaler overflow interrupt */
    void  (*rtcCallback)(void *callbackParam);         /*!< Pointer to the user callback method */
    void  *callbackParams;                             /*!< Pointer to the callback parameters */
} rtc_int_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes RTC driver based on configuration input.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] config: RTC configuration pointer
 * @return The result of execution
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if control register is locked
 */
status_t RTC_DRV_Init(uint8_t instance, const rtc_init_config_t *config);

/*!
 * @brief De-initialize RTC module.
 *
 * @param[in] instance: The RTC instance number
 * @return The result of execution
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if control register is locked
 */
status_t RTC_DRV_Deinit(uint8_t instance);

/*!
 * @brief Get RTC default configuration structure.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] config: The pointer to RTC configuration
 * @return None
 */
void RTC_DRV_GetDefaultConfigure(uint8_t instance, rtc_init_config_t * const config);

/*!
 * @brief Start RTC counter.Before calling this function the user
 *        should use RTC_DRV_Init to configure the start counter.
 *
 * @param[in] instance: The RTC instance number
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR:  if the counter cannot be enabled or is already enabled.
 */
status_t RTC_DRV_StartCounter(uint8_t instance);

/*!
 * @brief Stop RTC counter.
 *
 * @param[in] instance: The RTC instance number
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if the counter cannot be stopped.
 */
status_t RTC_DRV_StopCounter(uint8_t instance);

/*!
 * @brief Set time prescaler value of RTC.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] prescalerCnt: The value of time prescaler
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t RTC_DRV_SetPrescalerCnt(uint8_t instance, uint32_t prescalerCnt);

/*!
 * @brief Set time counter of RTC.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] timeCnt: The value of time counter
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t RTC_DRV_SetTimeCnt(uint8_t instance, uint32_t timeCnt);

/*!
 * @brief Get current time counter of RTC.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] timeCnt: The value of time counter
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t RTC_DRV_GetCurrentTimeCnt(uint8_t instance, uint32_t * const timeCnt);

/*!
 * @brief Set lock status for RTC registers.Remember that all the registers
 *        are unlocked only by software reset or power on reset.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] lockConfig: The pointer to lock type
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if the Lock Register is locked.
 */
status_t RTC_DRV_ConfigureRegisterLock(uint8_t instance, const rtc_register_lock_config_t * const lockConfig);

/*!
 * @brief Get lock status for RTC registers.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] lockConfig: The pointer to lock type
 * @return None
 */
void RTC_DRV_GetRegisterLock(uint8_t instance, rtc_register_lock_config_t * const lockConfig);

/*!
 * @brief Configure RTC interrupt.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] intConfig: The pointer to interrupt type
 * @return None
 */
void RTC_DRV_ConfigureInt(uint8_t instance, rtc_int_config_t * const intConfig);

/*!
 * @brief Configures the alarm with the configuration structure.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] alarmConfig: The value of alarm configuration structure
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if the alarm time is invalid
 */
status_t RTC_DRV_ConfigureAlarm(uint8_t instance, rtc_alarm_config_t * const alarmConfig);

/*!
 * @brief Get the configuration structure of RTC alarm.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] alarmConfig: The value of alarm configuration structure
 * @return None
 */
void RTC_DRV_GetAlarmConfig(uint8_t instance, rtc_alarm_config_t *alarmConfig);

/*!
 * @brief Check if an alarm has occured.
 *
 * @param[in] instance: The RTC instance number
 * @return if ture alarm has occurred, false alarm has not occurred
 */
bool RTC_DRV_IsAlarmPending(uint8_t instance);

/*!
 * @brief Gets the next alarm value.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] alarmValue: The value of next alarm
 * @return the result of operation
 *         - STATUS_SUCCESS: if the next alarm value is valid
 *         - STATUS_ERROR: if there is no new alarm
 */
status_t RTC_DRV_GetNextAlarmCnt(uint8_t instance, uint32_t * const alarmValue);

/*!
 * @brief RTC interrupt request handler.
 *
 * @param[in] None
 * @return None
 */
void RTC_DRV_IRQHandler(uint8_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTC_DRV_H */

/* =============================================  EOF  ============================================== */
