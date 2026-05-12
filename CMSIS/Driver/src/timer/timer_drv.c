/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written psramission of AutoChips inc. and/or its licensors, any
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
 * @file timer_drv.c
 *
 * @brief This file provides timer integration functions interface.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "timer_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */
#define   TIMER_DEFAULT_PERIOD            (24000000UL)
#define   TIMER_CLOCK_DEFAULT_UNIT        (1000000UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table to save Timer IRQ numbers. */
static const IRQn_Type s_timerIRQ[TIMER_CHANNEL_MAX] = TIMER_IRQS;
/* Table of base addresses for TIMER channnels */
static TIMER_CHANNEL_Type *s_timerBase[TIMER_CHANNEL_MAX] = TIMER_BASE_PTRS;
/* Table to save TIMER clock in clock manager */
static const clock_names_t s_timerClkNames[TIMER_INSTANCE_COUNT] = TIMER_CLOCK_NAMES;
/* TIMER clock variable */
static uint32_t s_timerSourceClockFrequency[TIMER_INSTANCE_COUNT] = {0};
/* TIMER callback pointer */
static timer_callback_t s_timerCallback[TIMER_CHANNEL_MAX] = {NULL};

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Gets the default timer channel configuration.
 *
 * @param[out] config: The channel configuration structure
 * @return None
 */
void TIMER_DRV_GetDefaultChanConfig(timer_user_channel_config_t *const config)
{
    DEVICE_ASSERT(config != NULL);

    config->timerMode = TIMER_PERIODIC_COUNTER;
    config->periodUnits = TIMER_PERIOD_UNITS_MICROSECONDS;
    config->period = TIMER_DEFAULT_PERIOD;
    config->triggerSource = TIMER_TRIGGER_SOURCE_EXTERNAL;
    config->triggerSelect = 0U;
    config->enableReloadOnTrigger = false;
    config->enableStopOnInterrupt = false;
    config->enableStartOnTrigger = false;
    config->chainChannel = false;
    config->isInterruptEnabled = true;
}

/*!
 * @brief Initializes the TIMER module.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] enableRunInDebug: Pointer to TIMER configuration structure
 * @return None
 */
void TIMER_DRV_Init(uint8_t instance, bool enableRunInDebug)
{
    status_t clkErr;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    clkErr = CKGEN_DRV_GetFreq(s_timerClkNames[instance], &s_timerSourceClockFrequency[instance]);

    (void)clkErr;
    DEVICE_ASSERT(STATUS_SUCCESS == clkErr);
    DEVICE_ASSERT(s_timerSourceClockFrequency[instance] > 0U);

    /* Enable timer clock */
    CKGEN_DRV_Enable(CLK_TIMER, true);
    CKGEN_DRV_SoftReset(SRST_TIMER, true);

    TIMER_Enable(TIMER_CTRL);
    TIMER_SetRunInDebugCmd(TIMER_CTRL, enableRunInDebug);
}

/*!
 * @brief De-Initializes the TIMER module.
 * @param[in] instance: instance number of TIMER module
 * @return None
 */
void TIMER_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);

    TIMER_Disable(TIMER_CTRL);
    /* Disbale timer clock */
    CKGEN_DRV_SoftReset(SRST_TIMER, false);
    CKGEN_DRV_Enable(CLK_TIMER, false);
}

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
                               const timer_user_channel_config_t *userChannelConfig)
{
    TIMER_CHANNEL_Type *base = s_timerBase[channel];
    status_t reVal = STATUS_SUCCESS;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(userChannelConfig != NULL);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    if ((0U == channel) && (userChannelConfig->chainChannel))
    {
        reVal = STATUS_ERROR;
    }
    else
    {
        TIMER_SetChannelChainCmd(base, userChannelConfig->chainChannel);
        TIMER_SetChannelModeCmd(base, userChannelConfig->timerMode);
        if (userChannelConfig->timerMode == TIMER_TRIGGER_ACCUMULATOR)
        {
            TIMER_SetChannelCavlReadSel(base, TIMER_BCLK_FCLK_4);
        }
        else
        {
            TIMER_SetChannelCavlReadSel(base, userChannelConfig->cvalreadsel);
        }
        if (TIMER_PERIOD_UNITS_MICROSECONDS == userChannelConfig->periodUnits)
        {
            reVal = TIMER_DRV_SetPeriodByUs(instance, channel, userChannelConfig->period);
        }
        else
        {
            TIMER_DRV_SetPeriodByCount(instance, channel, userChannelConfig->period);
        }

        if (STATUS_SUCCESS == reVal)
        {
            TIMER_SetTriggerSourceCmd(base, userChannelConfig->triggerSource);
            TIMER_SetTriggerSelectCmd(base, userChannelConfig->triggerSelect);
            TIMER_SetReloadOnTriggerCmd(base, userChannelConfig->enableReloadOnTrigger);
            TIMER_SetStopOnInterruptCmd(base, userChannelConfig->enableStopOnInterrupt);
            TIMER_SetStartOnTriggerCmd(base, userChannelConfig->enableStartOnTrigger);
            TIMER_DRV_InstallCallback(channel, userChannelConfig->callback);

            if (userChannelConfig->isInterruptEnabled)
            {
                TIMER_EnableInterrupt(TIMER_CTRL, (uint32_t)1U << channel);
                NVIC_EnableIRQ(s_timerIRQ[channel]);
            }
            else
            {
                TIMER_DisableInterrupt(TIMER_CTRL, (uint32_t)1U << channel);
                NVIC_DisableIRQ(s_timerIRQ[channel]);
            }
        }
    }

    return reVal;
}

/*!
 * @brief Enables the timer channel to start count according to counte mode and control bits.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: enable mask of timer channels(such as mask = 0x03U will start timer 0&1)
 * @return None
 */
void TIMER_DRV_StartChannels(uint8_t instance, uint32_t mask)
{
    TIMER_CTRL_Type *base = TIMER_CTRL;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(mask < (1U << TIMER_CHANNEL_MAX));

    TIMER_StartChannels(base, mask);
}

/*!
* @brief Disables the timer channel to stop counting.
*
* @param[in] instance: instance number of TIMER module
* @param[in] mask: disable mask of timer channels(such as mask = 0x03U will stop timer 0&1)
* @return None
*/
void TIMER_DRV_StopChannels(uint8_t instance, uint32_t mask)
{
    TIMER_CTRL_Type *base = TIMER_CTRL;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(mask < (1U << TIMER_CHANNEL_MAX));

    TIMER_StopChannels(base, mask);
}

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
status_t TIMER_DRV_SetPeriodByUs(uint8_t instance, uint32_t channel, uint32_t periodUs)
{
    TIMER_CHANNEL_Type *base = s_timerBase[channel];
    timer_modes_t timerMode;
    status_t clkErr;
    status_t reVal = STATUS_SUCCESS;
    uint64_t count;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    clkErr = CKGEN_DRV_GetFreq(s_timerClkNames[instance], &s_timerSourceClockFrequency[instance]);
    (void)clkErr;
    DEVICE_ASSERT(STATUS_SUCCESS == clkErr);
    DEVICE_ASSERT(s_timerSourceClockFrequency[instance] > 0U);

    count = ((uint64_t)periodUs) * s_timerSourceClockFrequency[instance];
    count = (count / TIMER_CLOCK_DEFAULT_UNIT) - 1U;
    timerMode = TIMER_GetChannelModeCmd(base);
    /* Checks the range of count */
    if (count <= MAX_PERIOD_COUNT)
    {
        if (TIMER_DUAL_PERIODIC_COUNTER == timerMode)
        {
            if (count > MAX_PERIOD_COUNT_IN_DUAL_16BIT_MODE)
            {
                reVal = STATUS_ERROR;
            }
            else if (count > MAX_PERIOD_COUNT_16_BIT)
            {
                /* Calculates the count of dual 16 mode */
                count = ((count - (MAX_PERIOD_COUNT_16_BIT + 1U)) << 16U) | (MAX_PERIOD_COUNT_16_BIT);

            }
        }
    }
    else
    {
        reVal = STATUS_ERROR;
    }
    if (STATUS_SUCCESS == reVal)
    {
        /* Sets period by count */
        TIMER_SetPeriodByCount(base, (uint32_t)count);
    }

    return reVal;
}

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
status_t TIMER_DRV_SetDual16PeriodByUs(uint8_t instance, uint32_t channel, uint16_t periodHigh, uint16_t periodLow)
{
    TIMER_CHANNEL_Type *base = s_timerBase[channel];
    status_t reVal = STATUS_SUCCESS;
    status_t clkErr;
    uint64_t periodHighCount;
    uint64_t periodLowCount;
    uint64_t periodCount;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    clkErr = CKGEN_DRV_GetFreq(s_timerClkNames[instance], &s_timerSourceClockFrequency[instance]);
    (void)clkErr;
    DEVICE_ASSERT(STATUS_SUCCESS == clkErr);
    DEVICE_ASSERT(s_timerSourceClockFrequency[instance] > 0U);

    /* converts unit from us to count.*/
    periodHighCount = ((uint64_t)periodHigh) * s_timerSourceClockFrequency[instance];
    periodHighCount = (periodHighCount / TIMER_CLOCK_DEFAULT_UNIT) - 1U;

    periodLowCount = ((uint64_t)periodLow) * s_timerSourceClockFrequency[instance];
    periodLowCount = (periodLowCount / TIMER_CLOCK_DEFAULT_UNIT) - 1U;
     /* Checks the range of count */
    if ((periodHighCount > MAX_PERIOD_COUNT_16_BIT) || (periodLowCount > MAX_PERIOD_COUNT_16_BIT))
    {
        reVal = STATUS_ERROR;
    }
    else
    {
        periodCount = (periodHighCount << 16U) | periodLowCount;
        TIMER_SetPeriodByCount(base, (uint32_t)periodCount);
    }

    return reVal;
}

/*!
 * @brief Gets the timer channel period in microseconds for 32 bit periodic count or dual 16 bit periodic count mode.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @return Timer channel period in microseconds
 */
uint64_t TIMER_DRV_GetPeriodByUs(uint8_t instance, uint32_t channel)
{
    const TIMER_CHANNEL_Type *base = s_timerBase[channel];
    status_t clkErr;
    timer_modes_t timerMode;
    uint64_t currentPeriod;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    clkErr = CKGEN_DRV_GetFreq(s_timerClkNames[instance], &s_timerSourceClockFrequency[instance]);
    (void)clkErr;
    DEVICE_ASSERT(STATUS_SUCCESS == clkErr);
    DEVICE_ASSERT(s_timerSourceClockFrequency[instance] > 0U);

    currentPeriod = TIMER_GetPeriodByCount(base);
    timerMode = TIMER_GetChannelModeCmd(base);

    /* converts unit from count to us.*/
    if (TIMER_DUAL_PERIODIC_COUNTER == timerMode)
    {
        if (currentPeriod > MAX_PERIOD_COUNT_16_BIT)
        {
            currentPeriod = ((currentPeriod >> 16U) + (currentPeriod & MAX_PERIOD_COUNT_16_BIT) + 2U) \
                            * TIMER_CLOCK_DEFAULT_UNIT / s_timerSourceClockFrequency[instance];
        }
        else
        {
            currentPeriod = ((currentPeriod + 1U) * TIMER_CLOCK_DEFAULT_UNIT) / s_timerSourceClockFrequency[instance];
        }
    }
    else
    {
        currentPeriod = ((currentPeriod + 1U) * TIMER_CLOCK_DEFAULT_UNIT) / s_timerSourceClockFrequency[instance];
    }

    return currentPeriod;
}

/*!
 * @brief Gets the current timer channel counting value in microseconds, and callimg this function twice can get
 * the runing time.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @return Current timer channel counting value in microseconds
 */
uint64_t TIMER_DRV_GetCurrentUs(uint8_t instance, uint32_t channel)
{
    const TIMER_CHANNEL_Type *base = s_timerBase[channel];
    status_t clkErr;
    timer_modes_t timerMode;
    uint64_t currentTime = 0U;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    clkErr = CKGEN_DRV_GetFreq(s_timerClkNames[instance], &s_timerSourceClockFrequency[instance]);
    (void)clkErr;
    DEVICE_ASSERT(s_timerSourceClockFrequency[instance] > 0U);

    currentTime = TIMER_GetCurrentCount(base);
    timerMode = TIMER_GetChannelModeCmd(base);

    /* converts unit from count to us */
    if (TIMER_DUAL_PERIODIC_COUNTER == timerMode)
    {
        currentTime = (((currentTime >> 16U) + (currentTime & MAX_PERIOD_COUNT_16_BIT)) * TIMER_CLOCK_DEFAULT_UNIT) \
                      / s_timerSourceClockFrequency[instance];
    }
    else
    {
        currentTime = (currentTime * TIMER_CLOCK_DEFAULT_UNIT) / s_timerSourceClockFrequency[instance];
    }

    return currentTime;
}

/*!
 * @brief Sets the timer channel period in count unit, when period is modified, the new value will take affect after
 *  the old period expires or immediately depends on the TVAL_UP register field setting.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] count: Timer channel period in count unit
 * @return None
 */
void TIMER_DRV_SetPeriodByCount(uint8_t instance, uint32_t channel, uint32_t count)
{
    TIMER_CHANNEL_Type *base = s_timerBase[channel];

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    TIMER_SetPeriodByCount(base, count);
}

/*!
 * @brief Sets the timer channel period in count unit for dual 16 count mode.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] periodHigh: Period of higher 16 bit in count unit
 * @param[in] periodLow: Period of lower 16 bit in count unit
 * @return None
 */
void TIMER_DRV_SetDual16PeriodByCount(uint8_t instance, uint32_t channel, uint16_t periodHigh, uint16_t periodLow)
{
    TIMER_CHANNEL_Type *base = s_timerBase[channel];
    uint32_t period;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    period = ((uint32_t)periodHigh << 16U) | periodLow;
    TIMER_SetPeriodByCount(base, period);
}

/*!
 * @brief Gets the current timer channel period in count unit.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @return Timer channel period in count unit
 */
uint32_t TIMER_DRV_GetPeriodByCount(uint8_t instance, uint32_t channel)
{
    const TIMER_CHANNEL_Type *base = s_timerBase[channel];
    timer_modes_t timerMode;
    uint32_t currentPeriod;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    currentPeriod = TIMER_GetPeriodByCount(base);
    timerMode = TIMER_GetChannelModeCmd(base);

    if (TIMER_DUAL_PERIODIC_COUNTER == timerMode)
    {
        currentPeriod = (currentPeriod >> 16U) + (currentPeriod & MAX_PERIOD_COUNT_16_BIT);
    }

    return currentPeriod;
}

/*!
 * @brief Clears the current counter by writing any value to it.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @param[in] count: counter in count unit
 * @return None
 */
void TIMER_DRV_ClearCurrentCount(uint8_t instance, uint32_t channel, uint32_t count)
{
    TIMER_CHANNEL_Type *base = s_timerBase[channel];

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    TIMER_SetCurrentCount(base, count);
}

/*!
 * @brief Gets the current counting value in count.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] channel: Timer channel index
 * @return Current timer channel counting value in count
 */
uint32_t TIMER_DRV_GetCurrentCount(uint8_t instance, uint32_t channel)
{
    const TIMER_CHANNEL_Type *base = s_timerBase[channel];
    timer_modes_t timerMode;
    uint32_t currentTime;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < TIMER_CHANNEL_MAX);

    currentTime = TIMER_GetCurrentCount(base);
    timerMode = TIMER_GetChannelModeCmd(base);

    if (TIMER_DUAL_PERIODIC_COUNTER == timerMode)
    {
        currentTime = (currentTime >> 16U) + (currentTime & MAX_PERIOD_COUNT_16_BIT);
    }

    return currentTime;
}

/*!
 * @brief Enables the interrupt of timer channel.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: The interrupt enable mask (such as mask = 0x03u will enable the interrupt of channel 0&1)
 * @return None
 */
void TIMER_DRV_EnableInterrupt(uint8_t instance, uint32_t mask)
{
    TIMER_CTRL_Type *base = TIMER_CTRL;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(mask < (1UL << TIMER_CHANNEL_MAX));

    TIMER_EnableInterrupt(base, mask);
}

/*!
 * @brief Disables the interrupt of timer channel.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: The interrupt disable mask (such as mask = 0x03u will disable the interrupt of channel 0&1)
 * @return None
 */
void TIMER_DRV_DisableInterrupt(uint8_t instance, uint32_t mask)
{
    TIMER_CTRL_Type *base = TIMER_CTRL;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(mask < (1UL << TIMER_CHANNEL_MAX));

    TIMER_DisableInterrupt(base, mask);
}

/*!
 * @brief Gets the interrupt flag of timer channel.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask:     The interrupt flag getting mask (such as mask = 0x03u will get the interrupt flag of channel 0&1)
 * @return the interrupt flag of timer channels
 */
uint32_t TIMER_DRV_GetInterruptFlag(uint8_t instance, uint32_t mask)
{
    const TIMER_CTRL_Type *base = TIMER_CTRL;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(mask < (1UL << TIMER_CHANNEL_MAX));

    return TIMER_GetInterruptFlag(base, mask);
}

/*!
 * @brief Clears the interrupt flag of timer channel.
 *
 * @param[in] instance: instance number of TIMER module
 * @param[in] mask: The interrupt flag clearing mask (such as mask = 0x03u will clear the interrupt flag of channel 0&1)
 * @return None
 */
void TIMER_DRV_ClearInterruptFlag(uint8_t instance, uint32_t mask)
{
    TIMER_CTRL_Type *base = TIMER_CTRL;

    DEVICE_ASSERT(instance < TIMER_INSTANCE_COUNT);
    DEVICE_ASSERT(mask < (1UL << TIMER_CHANNEL_MAX));

    TIMER_ClearInterruptFlag(base, mask);
}

/*!
 * @brief Installs timer callback function.
 *
 * @param[in] channel: timer channel
 * @param[in] func: callback task
 * @return None
 */
void TIMER_DRV_InstallCallback(uint32_t channel, const timer_callback_t func)
{
    DEVICE_ASSERT(TIMER_CHANNEL_MAX > channel);

    s_timerCallback[channel] = func;
}

/*!
* @brief TIMER common interrupt service routine.
*
* @param[in] channel: timer channel
* @return None
*/
void TIMER_DRV_IRQHandler(uint32_t channel)
{
    uint32_t wpara = 0U;

    /* store device status */
    wpara = TIMER_GetInterruptFlag(TIMER_CTRL, 1U << channel);
    if (wpara)
    {
        /* clear device status */
        TIMER_ClearInterruptFlag(TIMER_CTRL, 1U << channel);
        if (NULL != s_timerCallback[channel])
        {
            /* callback */
            s_timerCallback[channel](s_timerBase[channel], wpara, 0U);
        }
    }
}

/* =============================================  EOF  ============================================== */
