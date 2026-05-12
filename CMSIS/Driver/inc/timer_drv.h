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
 * @file timer_drv.h
 *
 * @brief This file provides timer integration functions interface.
 *
 */

#ifndef _TIMER_DRV_H
#define _TIMER_DRV_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
/*!< Max count period of all timer mode except for dual 16 bit mode */
#define MAX_PERIOD_COUNT                       (0xFFFFFFFFUL)
/*!< Max count period of dual 16 bit mode                           */
#define MAX_PERIOD_COUNT_IN_DUAL_16BIT_MODE    (0x1FFFEUL)
/*!< Max count of 16 bit                                            */
#define MAX_PERIOD_COUNT_16_BIT                (0xFFFFUL)

/* ===========================================  Typedef  ============================================ */
/*!< Callback for timer module */
typedef void (*timer_callback_t)(void *device, uint32_t wpara, uint32_t lpara);

/*!
 * @brief timer index enum
 */
typedef enum
{
    TIMER_CHANNEL_0 = 0x00U, /*!< timer channel 0 */
    TIMER_CHANNEL_1 = 0x01U, /*!< timer channel 1 */
    TIMER_CHANNEL_2 = 0x02U, /*!< timer channel 2 */
    TIMER_CHANNEL_3 = 0x03U  /*!< timer channel 3 */
} timer_channel_index_t;

/*!
 * @brief timer counter Mode
 */
typedef enum
{
    TIMER_PERIODIC_COUNTER = 0x00U,      /*!< 32-bit Periodic Counter        */
    TIMER_DUAL_PERIODIC_COUNTER = 0x01U, /*!< Dual 16-bit Periodic Counter   */
    TIMER_TRIGGER_ACCUMULATOR = 0x02U,   /*!< 32-bit Trigger Accumulator     */
    TIMER_INPUT_CAPTURE = 0x03U          /*!< 32-bit Trigger Input Capture   */
} timer_modes_t;

/*!
 * @brief timer cval read presicion sel
 */
typedef enum
{
    TIMER_BCLK_FCLK_4 = 0x00U,      /*!< BUSCLK/FUNCTION CLK >= 4        */
    TIMER_BCLK_FCLK_3 = 0x01U,      /*!< BUSCLK/FUNCTION CLK >= 3        */
    TIMER_BCLK_FCLK_1 = 0x02U,      /*!< BUSCLK/FUNCTION CLK >= 1        */
} timer_sel_t;

/*!
 * @brief Trigger source options
 */
typedef enum
{
    TIMER_TRIGGER_SOURCE_EXTERNAL = 0x00U, /*!< external trigger  */
    TIMER_TRIGGER_SOURCE_INTERNAL = 0x01U  /*!< internal trigger  */
} timer_trigger_source_t;

/*!
 * @brief Unit of TIMER period
 */
typedef enum
{
    TIMER_PERIOD_UNITS_COUNTS = 0x00U,      /*!< use count as Period value unit        */
    TIMER_PERIOD_UNITS_MICROSECONDS = 0x01U /*!< use microsecond as Period value unit  */
} timer_period_units_t;

/*!
 * @brief the configuration structure of the channel timer
 */
typedef struct
{
    timer_modes_t timerMode;              /*!< count mode of timer channel                                   */
    timer_period_units_t periodUnits;     /*!< units of timer period value                                   */
    uint32_t period;                      /*!< Period of timer channel                                       */
    timer_trigger_source_t triggerSource; /*!< Selects between internal and external trigger sources         */
    timer_sel_t cvalreadsel;              /*!< Selects Read Cval Precision selection                         */
    uint32_t triggerSelect;               /*!< Selects one trigger from the internal trigger sources         */
    bool enableReloadOnTrigger;           /*!< enable or disable control for reload on trigger               */
    bool enableStopOnInterrupt;           /*!< enable or disable control for stop on interrupt               */
    bool enableStartOnTrigger;            /*!< enable or disable control for start on trigger                */
    bool chainChannel;                    /*!< enable or disable control for channel chain mode              */
    bool isInterruptEnabled;              /*!< enable or disable control for timer channel interrupt         */
    timer_callback_t callback;            /*!< Pointer to the user callback method                           */
} timer_user_channel_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Gets the default timer channel configuration.
 *
 * @param[out] config: The channel configuration structure
 * @return None
 */
void TIMER_DRV_GetDefaultChanConfig(timer_user_channel_config_t *const config);

/*!
 * @brief Initializes the TIMER module.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] enableRunInDebug: Pointer to TIMER configuration structure
 * @return None
 */
void TIMER_DRV_Init(uint8_t instance, bool enableRunInDebug);

/*!
 * @brief De-Initializes the TIMER module.
 * @param[in] instance: instance number of TIMER module
 * @return None
 */
void TIMER_DRV_Deinit(uint8_t instance);

/*!
 * @brief Initializes and configures the parameters of TIMER channel.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] userChannelConfig: Pointer to TIMER channel configuration structure
 * @return Operation status
 *         - STATUS_SUCCESS: Operation was successful.
 *         - STATUS_ERROR: The channel 0 is chained.
 *         - STATUS_ERROR: The input period is invalid.
 */
status_t TIMER_DRV_InitChannel(uint8_t instance, uint32_t channel,
                               const timer_user_channel_config_t *userChannelConfig);

/*!
 * @brief Enables the timer channel to start count according to counte mode and control bits.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: enable mask of timer channels(such as mask = 0x03U will start timer 0&1)
 * @return None
 */
void TIMER_DRV_StartChannels(uint8_t instance, uint32_t mask);

/*!
 * @brief Disables the timer channel to stop counting.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: disable mask of timer channels(such as mask = 0x03U will stop timer 0&1)
 * @return None
 */
void TIMER_DRV_StopChannels(uint8_t instance, uint32_t mask);

/*!
 * @brief Sets the timer channel period in microseconds, and the period range depends on the clock and counting mode.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] periodUs: Timer channel period in microseconds
 * @return Operation status
 *         - STATUS_SUCCESS: Input period of timer channel is valid
 *         - STATUS_ERROR: Input period of timer channel is invalid
 */
status_t TIMER_DRV_SetPeriodByUs(uint8_t instance, uint32_t channel, uint32_t periodUs);

/*!
 * @brief Sets the timer channel period in microseconds for dual 16 bit count mode.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] periodHigh: Period of higher 16 bit in microseconds
 * @param[in] periodLow: Period of lower 16 bit in microseconds
 * @return Operation status
 *         - STATUS_SUCCESS: Input period of timer channel is valid
 *         - STATUS_ERROR: Input period of timer channel is invalid
 */
status_t TIMER_DRV_SetDual16PeriodByUs(uint8_t instance, uint32_t channel, uint16_t periodHigh, uint16_t periodLow);

/*!
 * @brief Gets the timer channel period in microseconds for 32 bit periodic count or dual 16 bit periodic count mode.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @return Timer channel period in microseconds
 */
uint64_t TIMER_DRV_GetPeriodByUs(uint8_t instance, uint32_t channel);

/*!
 * @brief Gets the current timer channel counting value in microseconds, and callimg this function twice can get
 * the runing time.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @return Current timer channel counting value in microseconds
 */
uint64_t TIMER_DRV_GetCurrentUs(uint8_t instance, uint32_t channel);

/*!
 * @brief Sets the timer channel period in count unit, when period is modified, the new value will take affect after
 * the old period expires or immediately depends on the TVAL_UP register field setting.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] count: Timer channel period in count unit
 * @return None
 */
void TIMER_DRV_SetPeriodByCount(uint8_t instance, uint32_t channel, uint32_t count);

/*!
 * @brief Sets the timer channel period in count unit for dual 16 count mode.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] periodHigh: Period of higher 16 bit in count unit
 * @param[in] periodLow: Period of lower 16 bit in count unit
 * @return None
 */
void TIMER_DRV_SetDual16PeriodByCount(uint8_t instance, uint32_t channel, uint16_t periodHigh, uint16_t periodLow);

/*!
 * @brief Gets the current timer channel period in count unit.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @return Timer channel period in count unit
 */
uint32_t TIMER_DRV_GetPeriodByCount(uint8_t instance, uint32_t channel);

/*!
 * @brief Clears the current counter by writing any value to it.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] count: counter in count unit
 * @return None
 */
void TIMER_DRV_ClearCurrentCount(uint8_t instance, uint32_t channel, uint32_t count);

/*!
 * @brief Gets the current counting value in count.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @return Current timer channel counting value in count
 */
uint32_t TIMER_DRV_GetCurrentCount(uint8_t instance, uint32_t channel);

/*!
 * @brief Enables the interrupt of timer channel.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: The interrupt enable mask (such as mask = 0x03u will enable the interrupt of channel 0&1)
 * @return None
 */
void TIMER_DRV_EnableInterrupt(uint8_t instance, uint32_t mask);

/*!
 * @brief Disables the interrupt of timer channel.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: The interrupt disable mask (such as mask = 0x03u will disable the interrupt of channel 0&1)
 * @return None
 */
void TIMER_DRV_DisableInterrupt(uint8_t instance, uint32_t mask);

/*!
 * @brief Gets the interrupt flag of timer channels.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: The interrupt flag getting mask (such as mask = 0x03u will get the interrupt flag of channel 0&1)
 * @return the interrupt flag of timer channels
 */
uint32_t TIMER_DRV_GetInterruptFlag(uint8_t instance, uint32_t mask);

/*!
 * @brief Clears the interrupt flag of timer channels.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: The interrupt flag clearing mask (such as mask = 0x03u will clear the interrupt flag of channel 0&1)
 * @return None
 */
void TIMER_DRV_ClearInterruptFlag(uint8_t instance, uint32_t mask);

/*!
 * @brief Installs timer callback function.
 *
 * @param[in] channel: timer channel
 * @param[in] func: callback task
 * @return None
 */
void TIMER_DRV_InstallCallback(uint32_t channel, const timer_callback_t func);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMER_DRV_H*/

/* =============================================  EOF  ============================================== */
