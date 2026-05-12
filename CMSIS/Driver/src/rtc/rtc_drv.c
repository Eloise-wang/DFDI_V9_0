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
 * @file rtc_drv.c
 *
 * @brief This file provides rtc integration functions.
 *
 */

/* ===========================================  Include  ============================================ */
#include "rtc_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of base addresses for RTC instances. */
static RTC_Type * const s_rtcBase[RTC_INSTANCE_MAX] = RTC_BASE_PTRS;

/* Table to save RTC IRQ numbers. */
static const IRQn_Type s_rtcIrqId[RTC_INSTANCE_MAX] = RTC_IRQS;

/* Table to save RTC clock names as defined in clock manager. */
static const ckgen_clock_t s_rtcClkNames[UART_INSTANCE_MAX] = RTC_CKGEN_CLOCKS;

/*!
 * @brief static RTC runtime structure, it is designed only for internal purposes
 */
static struct
{
    rtc_alarm_config_t     *alarmConfig;    /* Alarm configuration */
    bool                    isAlarmTimeNew; /* Check if there is a new alarm  */
    rtc_int_config_t       *intConfig;      /* Interrupt configuration */
} g_rtcRuntimeConfig[RTC_INSTANCE_MAX];

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initializes RTC driver based on configuration input.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] config: RTC configuration pointer
 * @return The result of execution
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if control register is locked
 */
status_t RTC_DRV_Init(uint8_t instance, const rtc_init_config_t *config)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);
    DEVICE_ASSERT(NULL != config);

    status_t status = STATUS_SUCCESS;
    RTC_Type *base = s_rtcBase[instance];

    /* Initialize runtime structure */
    g_rtcRuntimeConfig[instance].alarmConfig    = NULL;
    g_rtcRuntimeConfig[instance].intConfig      = NULL;
    g_rtcRuntimeConfig[instance].isAlarmTimeNew = false;
    /* enable rtc system module */
    CKGEN_DRV_Enable(s_rtcClkNames[instance], true);

    /* Check if the control register is locked. */
    if (RTC_IsRegisterLocked(base, RTC_CTRL_REG_LOCK))
    {
        status = STATUS_ERROR;
    }
    else
    {
        /* Software reset */
        RTC_SoftwareReset(base);
        /* clear int flag */
        RTC_ClearPOF(base);
        /* config prescaler counter value */
        (void)RTC_SetPrescaler(base, config->psrValue);
        /* Config RTC clock out pin */
        (void)RTC_ConfigureClockOut(base, config->clockOutConfig);
        /* config time counter value */
        (void)RTC_SetTimeCntRegister(base, config->timeCnt);
        /* Set update enable bit value */
        RTC_SetUpdateMode(base, config->updateEn);
    }

    /* Return the exit code */
    return status;
}

/*!
 * @brief De-initialize RTC module.
 *
 * @param[in] instance: The RTC instance number
 * @return The result of execution
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if control register is locked
 */
status_t RTC_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    status_t status = STATUS_SUCCESS;
    RTC_Type *base = s_rtcBase[instance];

    /* Check if the control register is locked.*/
    CKGEN_DRV_Enable(s_rtcClkNames[instance], true);
    if (RTC_IsRegisterLocked(base, RTC_CTRL_REG_LOCK))
    {
        status = STATUS_ERROR;
    }
    else
    {
        NVIC_DisableIRQ(s_rtcIrqId[instance]);
        /* Perform a software reset */
        RTC_SoftwareReset(base);
        NVIC_ClearPendingIRQ(s_rtcIrqId[instance]);
        CKGEN_DRV_Enable(s_rtcClkNames[instance], false);
    }

    return status;
}

/*!
 * @brief Get RTC default configuration structure.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] config: The pointer to RTC configuration
 * @return None
 */
void RTC_DRV_GetDefaultConfigure(uint8_t instance, rtc_init_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    config->clockOutConfig = RTC_CLKOUT_DISABLED;
    config->updateEn = true;
    config->psrValue = 0UL;
    config->timeCnt = 0UL;
}

/*!
 * @brief Checks the following conditions to find if the time counter enable bit is writable.
 *         - if update mode bitfield if 1 and:
 *         - time is invalid or Real Time Counter Register has overflowed
 *         - Real Time Counter Register has overflowed or
 *         - time counter is disabled, then the TCE bit can be set even if Status Register is locked.
 *
 * @param[in] instance: The RTC instance number
 * @return True if the TCE can be set, otherwise false
 */
static bool RTC_CanWriteTimerCntEn(uint8_t instance)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    RTC_Type *base = s_rtcBase[instance];
    bool result = false;
    bool tifFlagSet;
    bool tofFlagSet;
    bool tceBitSet;

    /* Check if the status register is locked */
    if (false == RTC_IsRegisterLocked(base, RTC_STATUS_REG_LOCK))
    {
        result = true;
    }
    /* Get the update mode bit */
    else if (RTC_GetUpdateMode(base))
    {
        tifFlagSet = RTC_GetTIF(base);
        tofFlagSet = RTC_GetTOF(base);
        tceBitSet = RTC_GetTimeCounterEnable(base);

        /* Check for the specified conditions */
        if ((tifFlagSet) || (tofFlagSet) || (!tceBitSet))
        {
            result = true;
        }
    }
    else
    {
        result = false;
    }

    return result;
}

/*!
 * @brief Start RTC counter.Before calling this function the user
 *        should use RTC_DRV_Init to configure the start counter.
 *
 * @param[in] instance: The RTC instance number
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR:  if the counter cannot be enabled or is already enabled.
 */
status_t RTC_DRV_StartCounter(uint8_t instance)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    status_t status = STATUS_SUCCESS;
    RTC_Type *base = s_rtcBase[instance];

    /* Check if the TCE is writeable, and return status */
    if (false == RTC_CanWriteTimerCntEn(instance))
    {
        status = STATUS_ERROR;
    }
    else
    {
        /* Enable RTC Counter */
        status = RTC_Enable(base);
    }

    return status;
}

/*!
 * @brief Stop RTC counter.
 *
 * @param[in] instance: The RTC instance number
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if the counter cannot be stopped.
 */
status_t RTC_DRV_StopCounter(uint8_t instance)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    status_t status = STATUS_SUCCESS;
    RTC_Type *base = s_rtcBase[instance];

    /* Check if the TCE is writeable */
    if (false == RTC_CanWriteTimerCntEn(instance))
    {
        status = STATUS_ERROR;
    }
    else
    {
        /* Disable rtc counter */
        status = RTC_Disable(base);
    }

    return status;
}

/*!
 * @brief Set time prescaler value of RTC.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] prescalerCnt: The value of time prescaler
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t RTC_DRV_SetPrescalerCnt(uint8_t instance, uint32_t prescalerCnt)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    RTC_Type *base = s_rtcBase[instance];
    status_t status = STATUS_SUCCESS;

    status = RTC_SetPrescaler(base, prescalerCnt);

    return status;
}

/*!
 * @brief Set time counter of RTC.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] timeCnt: The value of time counter
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t RTC_DRV_SetTimeCnt(uint8_t instance, uint32_t timeCnt)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    RTC_Type *base = s_rtcBase[instance];
    status_t status = STATUS_SUCCESS;

    status = RTC_SetTimeCntRegister(base, timeCnt);

    return status;
}

/*!
 * @brief Get current time counter of RTC.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] timeCnt: The value of time counter
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t RTC_DRV_GetCurrentTimeCnt(uint8_t instance, uint32_t * const timeCnt)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    RTC_Type *base = s_rtcBase[instance];
    status_t status = STATUS_SUCCESS;

    (*timeCnt) = RTC_GetTimeCntReg(base);

    return status;
}

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
status_t RTC_DRV_ConfigureRegisterLock(uint8_t instance, const rtc_register_lock_config_t * const lockConfig)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);
    DEVICE_ASSERT(lockConfig != NULL);

    RTC_Type *base = s_rtcBase[instance];
    status_t status = STATUS_SUCCESS;

    /* Configure Control register */
    if (lockConfig->controlRegisterLock)
    {
        status = RTC_ConfigureLockRegister(base, RTC_CTRL_REG_LOCK);
    }
    /* Configure Lock register */
    if (lockConfig->lockRegisterLock)
    {
        status = RTC_ConfigureLockRegister(base, RTC_LOCK_REG_LOCK);
    }
    /* Configure Status register */
    if (lockConfig->statusRegisterLock)
    {
        status = RTC_ConfigureLockRegister(base, RTC_STATUS_REG_LOCK);
    }

    return status;
}

/*!
 * @brief Get lock status for RTC registers.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] lockConfig: The pointer to lock type
 * @return None
 */
void RTC_DRV_GetRegisterLock(uint8_t instance, rtc_register_lock_config_t * const lockConfig)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);
    DEVICE_ASSERT(lockConfig != NULL);

    RTC_Type *base = s_rtcBase[instance];

    lockConfig->controlRegisterLock = RTC_IsRegisterLocked(base, RTC_CTRL_REG_LOCK);
    lockConfig->lockRegisterLock = RTC_IsRegisterLocked(base, RTC_LOCK_REG_LOCK);
    lockConfig->statusRegisterLock = RTC_IsRegisterLocked(base, RTC_STATUS_REG_LOCK);
}

/*!
 * @brief Configure RTC interrupt.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] intConfig: The pointer to interrupt type
 * @return None
 */
void RTC_DRV_ConfigureInt(uint8_t instance, rtc_int_config_t * const intConfig)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);
    DEVICE_ASSERT(intConfig != NULL);

    RTC_Type *base = s_rtcBase[instance];

    RTC_SetPOIE(base, intConfig->presOverflowIntEnable);
    RTC_SetTOIE(base, intConfig->timeOverflowIntEnable);
    RTC_SetTIIE(base, intConfig->timeInvalidIntEnable);
    /* register user callback function */
    g_rtcRuntimeConfig[instance].intConfig = intConfig;

    /* config rtc nvic interrupt */
    if ((intConfig->presOverflowIntEnable) \
       || (intConfig->timeOverflowIntEnable) \
       || (intConfig->timeInvalidIntEnable))
    {
        NVIC_EnableIRQ(s_rtcIrqId[instance]);
    }
    else
    {
        NVIC_DisableIRQ(s_rtcIrqId[instance]);
        NVIC_ClearPendingIRQ(s_rtcIrqId[instance]);
    }
}

/*!
 * @brief Configures the alarm with the configuration structure.
 *
 * @param[in] instance: The RTC instance number
 * @param[in] alarmConfig: The value of alarm configuration structure
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if the alarm time is invalid
 */
status_t RTC_DRV_ConfigureAlarm(uint8_t instance, rtc_alarm_config_t * const alarmConfig)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);
    DEVICE_ASSERT(alarmConfig != NULL);

    RTC_Type *base = s_rtcBase[instance];
    status_t status = STATUS_SUCCESS;
    uint32_t alarmTime = 0U;
    uint32_t currentTime = 0U;

    alarmTime = alarmConfig->alarmValue;
    /* Get current time counter value */
    currentTime = RTC_GetTimeCntReg(base);

    /* Check if the alarm time is greater than current time */
    if (alarmTime > currentTime)
    {
        NVIC_DisableIRQ(s_rtcIrqId[instance]);
        g_rtcRuntimeConfig[instance].alarmConfig = alarmConfig;

        /* Set alarm time into Time Alarm Register */
        RTC_SetTimeAlarmReg(base, alarmTime);
        /* Enable/disable RTC alarm interrupt based on the configuration */
        RTC_SetTAIE(base, alarmConfig->timeAlarmIntEnable);
        NVIC_EnableIRQ(s_rtcIrqId[instance]);
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Get the configuration structure of RTC alarm.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] alarmConfig: The value of alarm configuration structure
 * @return None
 */
void RTC_DRV_GetAlarmConfig(uint8_t instance, rtc_alarm_config_t *alarmConfig)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);
    DEVICE_ASSERT(alarmConfig != NULL);

    *alarmConfig = *(g_rtcRuntimeConfig[instance].alarmConfig);
}

/*!
 * @brief Check if an alarm has occured.
 *
 * @param[in] instance: The RTC instance number
 * @return if ture alarm has occurred, false alarm has not occurred
 */
bool RTC_DRV_IsAlarmPending(uint8_t instance)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    RTC_Type *base = s_rtcBase[instance];

    /* Return the alarm interrupt flag */
    return RTC_GetTAF(base);
}

/*!
 * @brief Gets the next alarm value.
 *
 * @param[in] instance: The RTC instance number
 * @param[out] alarmValue: The value of next alarm
 * @return the result of operation
 *         - STATUS_SUCCESS: if the next alarm value is valid
 *         - STATUS_ERROR: if there is no new alarm
 */
status_t RTC_DRV_GetNextAlarmCnt(uint8_t instance, uint32_t * const alarmValue)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    RTC_Type *base = s_rtcBase[instance];
    status_t status = STATUS_SUCCESS;

    /* Check if is a new alarm and if true get the value */
    if (g_rtcRuntimeConfig[instance].isAlarmTimeNew)
    {
        *alarmValue = RTC_GetTimeAlarmReg(base);
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief RTC interrupt request handler.
 *
 * @param[in] None
 * @return None
 */
void RTC_DRV_IRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < RTC_INSTANCE_MAX);

    RTC_Type *base = s_rtcBase[instance];

    uint32_t temp;
    /* Get alarm configuration */
    rtc_alarm_config_t *alarmConfig = g_rtcRuntimeConfig[instance].alarmConfig;
    /* Get interrupt configuration */
    const rtc_int_config_t * const intConfig = g_rtcRuntimeConfig[instance].intConfig;

    /* Clear POF flag */
    RTC_ClearPOF(base);
    /* Check if an alarm interrupt has occurred */
    if (RTC_GetTAF(base))
    {
        /* If the alarm interrupt configuration has been defined process the alarm IRQ */
        if (alarmConfig != NULL)
        {
            /* If recurrence is enabled modify the alarm register to the next alarm value */
            if ((alarmConfig->numberOfRepeats > 0U) || (alarmConfig->repeatForever))
            {
                /* Get current time counter */
                temp = RTC_GetTimeCntReg(base);
                /* current time is incremented with the repetition value */
                temp += alarmConfig->repetitionInterval - 1U;
                /* Set new alarm value */
                RTC_SetTimeAlarmReg(base, temp);
                g_rtcRuntimeConfig[instance].isAlarmTimeNew = true;
                /* If the alarm repeats forever, set number of repeats to 0
                 * to avoid an accidental trigger of the core overflow flag
                 */
                alarmConfig->numberOfRepeats = (false == alarmConfig->repeatForever) \
                                                ? (alarmConfig->numberOfRepeats - 1U) : 0U;
            }
            else
            {
                /* If the alarm does not repeat, write Time Alarm Register to clear alarm interrupt flag */
                 RTC_SetTimeAlarmReg(base, 0U);
                 g_rtcRuntimeConfig[instance].isAlarmTimeNew = false;
            }
            /* If the user has defined a callback, call it */
            if (alarmConfig->alarmCallback != NULL)
            {
                alarmConfig->alarmCallback(alarmConfig->callbackParams);
            }
        }
    }
    /* If the interrupt is not caused by the alarm then call the user callback if defined */
    else if (intConfig != NULL)
    {
        if (intConfig->rtcCallback != NULL)
        {
            /* Call the RTC interrupt callback function with callback parameter */
            intConfig->rtcCallback(intConfig->callbackParams);
        }
    }
    else
    {
        /* Do nothing */
    }
}

/* =============================================  EOF  ============================================== */
