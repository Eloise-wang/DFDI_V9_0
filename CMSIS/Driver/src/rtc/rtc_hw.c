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
 * @file rtc_hw.c
 *
 * @brief This file provides rtc integration functions.
 *
 */

/* ===========================================  Include  ============================================ */
#include "rtc_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enable RTC counter.
 *
 * @param[in] base: The RTC base pointer
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: the counter is already enabled
 */
status_t RTC_Enable(RTC_Type * const base)
{
    status_t status = STATUS_SUCCESS;
    bool rtcCounterEn;

    /* check counter is enable or disable */
    rtcCounterEn = RTC_GetTimeCounterEnable(base);
    if (rtcCounterEn)
    {
        status = STATUS_ERROR;
    }
    else
    {
        /* Enable time counter */
        RTC_SetTimeCounterEnable(base, true);
    }

    return status;
}

/*!
 * @brief Disable RTC counter.
 *
 * @param[in] base: The RTC base pointer
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if the counter is not disabled
 */
status_t RTC_Disable(RTC_Type * const base)
{
    /* check counter is enable or disable */
    if (RTC_GetTimeCounterEnable(base))
    {
        /* Disable time counter */
        RTC_SetTimeCounterEnable(base, false);
    }

    return (status_t)(RTC_GetTimeCounterEnable(base) ? STATUS_ERROR : STATUS_SUCCESS);
}

/*!
 * @brief Set RTC time counter value.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] timeCnt: 32-bit RTC time counter value
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if setting value error because timer is enabled
 */
status_t RTC_SetTimeCntRegister(RTC_Type * const base, uint32_t timeCnt)
{
    status_t status = STATUS_SUCCESS;

    if (RTC_GetTimeCounterEnable(base))
    {
        status = STATUS_ERROR;
    }
    else
    {
        RTC->TC = timeCnt;
        status = STATUS_SUCCESS;
    }

    return status;
}

/*!
 * @brief Set RTC prescaler value.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] psrValue: 32-bit RTC prescaler value
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if setting value error because timer is enabled
 */
status_t RTC_SetPrescaler(RTC_Type * const base, uint32_t psrValue)
{
    status_t status = STATUS_SUCCESS;

    if (RTC_GetTimeCounterEnable(base))
    {
        status = STATUS_ERROR;
    }
    else
    {
        RTC->PSR = psrValue;
        status = STATUS_SUCCESS;
    }

    return status;
}

/*!
 * @brief Configure RTC lock register.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] registerType: RTC lock register type
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if lock register is locked, and lock setting cannot be modified.
 */
status_t RTC_ConfigureLockRegister(RTC_Type * const base, rtc_lock_register_select_t registerType)
{
     status_t status = STATUS_SUCCESS;

    /* check if the lock register is already locked */
    /* lock register is locked, and register lock status cannot be modified. */
    if (RTC_GetLRL(base))
    {
        status = STATUS_ERROR;
    }
    else  /* configure the register lock */
    {
        switch (registerType)
        {
        case RTC_LOCK_REG_LOCK:
            RTC_LockLR(base);
            break;

        case RTC_STATUS_REG_LOCK:
            RTC_LockSR(base);
            break;

        case RTC_CTRL_REG_LOCK:
            RTC_LockCTRL(base);
            break;

        default:
            status = STATUS_ERROR;
            break;
        }
    }

    return status;
}

/*!
 * @brief Get RTC lock register status.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] reg: RTC lock register type
 * @return True if the register is locked, false if not
 */
bool RTC_IsRegisterLocked(const RTC_Type * const base, rtc_lock_register_select_t reg)
{
    bool state = false;

    switch (reg)
    {
    case RTC_LOCK_REG_LOCK:
        state = RTC_GetLRL(base);
        break;

    case RTC_STATUS_REG_LOCK:
        state = RTC_GetSRL(base);
        break;

    case RTC_CTRL_REG_LOCK:
        state = RTC_GetCRL(base);
        break;

    default:
        break;
    }

    return state;
}

/*!
 * @brief Configure the RTC clkout pin.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] config: RTC clkout pin configuration
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if control register is locked, and cannot enable RTC clkout function
 */
status_t RTC_ConfigureClockOut(RTC_Type * const base, rtc_clk_out_config_t config)
{
    status_t status = STATUS_SUCCESS;

    /*Check if control register is already locked*/
    if (RTC_GetCRL(base))
    {
        status = STATUS_ERROR;
    }
    else
    {
        switch (config)
        {
        case RTC_CLKOUT_DISABLED:
            /* Disable the clock out pin */
            RTC_SetRTCOutput(base, false);
            break;

        case RTC_CLKOUT_SRC_PSO:
            /* Select clock out source as perscaler overflow output and enable clock out */
            RTC->CTRL &= ~(RTC_CTRL_COS_Msk);
            RTC_SetRTCOutput(base, true);
            break;

        case RTC_CLKOUT_SRC_CLK:
            /* Select clock out source as RTC_CLK and enable clock out */
            RTC->CTRL |= (RTC_CTRL_COS_Msk);
            RTC_SetRTCOutput(base, true);
            break;

        default:
            break;
        }
    }

    return status;
}

/* =============================================  EOF  ============================================== */
