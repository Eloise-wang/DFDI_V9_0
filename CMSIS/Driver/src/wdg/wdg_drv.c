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
 * @file wdg_drv.c
 *
 * @brief This file provides wdg integration functions.
 *
 */

/* ===========================================  Include  ============================================ */
#include "wdg_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of base addresses for WDG instances. */
static WDG_Type * const s_wdgBase[WDG_INSTANCE_MAX] = WDG_BASE_PTRS;

/* Table to save WDG IRQ numbers. */
static const IRQn_Type s_wdgIrqId[WDG_INSTANCE_MAX] = WDG_IRQS;

/* Table to save WDG clock names as defined in clock manager. */
static const ckgen_clock_t s_wdgClkNames[WDG_INSTANCE_MAX] = WDG_CKGEN_CLOCKS;

/* WDG callback function */
static wdg_callback_t s_wdgCallback[WDG_INSTANCE_MAX] = {NULL};

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Gets the frequency of the specified WDG clock source.
 *
 * @param[in] wdgClk: The WDG clock source
 * @return The frequency of WDG clock source
 */
static uint32_t WDG_DRV_GetClockSourceFreq(wdg_clk_source_t wdgClk)
{
    uint32_t freq = 0;

    switch (wdgClk)
    {
    case WDG_LSI_CLOCK:
        (void)CKGEN_DRV_GetFreq(LSI_CLK, &freq);
        break;

    case WDG_BUS_CLOCK:
        (void)CKGEN_DRV_GetFreq(BUS_CLK, &freq);
        break;

    case WDG_HSI_CLOCK:
        (void)CKGEN_DRV_GetFreq(HSI_CLK, &freq);
        break;

    case WDG_HSE_CLOCK:
        (void)CKGEN_DRV_GetFreq(HSE_CLK, &freq);
        break;

    default:
        break;
    }

    return freq;
}

/*!
 * @brief Initialize WDG driver based on configuration input.
 *
 * @param[in] instance: The WDG instance number
 * @param[in] userConfigPtr: WDG configuration pointer
 * @return The result of execution
 *         - STATUS_SUCCESS: Initialize successful
 *         - STATUS_ERROR: Initialize failed
 */
status_t WDG_DRV_Init(uint8_t instance, const wdg_user_config_t *userConfigPtr)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);
    DEVICE_ASSERT(userConfigPtr != NULL);

    WDG_Type * base = s_wdgBase[instance];
    status_t status = STATUS_SUCCESS;
    status_t statusClockSource = STATUS_SUCCESS;
    uint32_t prevClock, ctrlClock;

    /* Check if the previous clock source and the configuration clock source are enabled */
    prevClock = WDG_DRV_GetClockSourceFreq(WDG_GetClkSource(s_wdgBase[instance]));
    ctrlClock = WDG_DRV_GetClockSourceFreq(userConfigPtr->clkSource);

    if ((0U == prevClock) || (0U == ctrlClock))
    {
        statusClockSource = STATUS_ERROR;
    }

    /* If clock source enabled */
    if (STATUS_SUCCESS == statusClockSource)
    {
        /* If window mode enabled and window value greater than or equal to the timeout value. Or timeout value is 0. */
        if (((userConfigPtr->winEn) && (userConfigPtr->windowValue >= userConfigPtr->timeoutValue)) \
             || (userConfigPtr->timeoutValue <= 0U))
        {
            status = STATUS_ERROR;
        }
        else
        {
            /* enable wdg interrupt */
            if (userConfigPtr->intEn)
            {
                NVIC_EnableIRQ(WDG_IRQn);
            }
            else
            {
                NVIC_DisableIRQ(WDG_IRQn);
                NVIC_ClearPendingIRQ(WDG_IRQn);
            }
            /* enable wdg system module */
            CKGEN_DRV_Enable(s_wdgClkNames[instance], true);
            status = WDG_Config(base, userConfigPtr);
        }
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief De-initialize WDG driver
 *
 * @param[in] instance: The WDG instance number
 * @return The result of execution
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation was failed
 */
status_t WDG_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type *base = s_wdgBase[instance];
    status_t status = STATUS_SUCCESS;


    if (WDG_IsUpdateEnabled(base))
    {
        DisableInterrupts;
        /* Disable timeout interrupt of WDG */
        NVIC_DisableIRQ(s_wdgIrqId[instance]);
        NVIC_ClearPendingIRQ(s_wdgIrqId[instance]);

        /* Deinitialize WDG */
        WDG_Deinit(base);
        EnableInterrupts;
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Gets the current configuration of the WDG
 *
 * @param[in] instance: The WDG instance number
 * @param[out] config: Pointer to the current configuration
 * @return none
 */
void WDG_DRV_GetConfig(uint8_t instance, wdg_user_config_t * const config)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    const WDG_Type * base = s_wdgBase[instance];

    *config = WDG_GetConfig(base);
}

/*!
 * @brief Gets default configuration of the WDG
 *
 * @param[out] config: Pointer to the default configuration
 * @return none
 */
void WDG_DRV_GetDefaultConfig(wdg_user_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    /* Get WDG register value */
    config->windowValue = WDG_WIN_RESET_DEFALUT_VALUE;
    config->timeoutValue = WDG_TIMEOUT_RESET_DEFAULT_VALUE;
    config->winEn = false;
    config->prescalerEn = false;
    config->intEn = false;
    config->updateEn = true;
    config->opMode.debugEn = false;
    config->clkSource = WDG_LSI_CLOCK;
    config->opMode.stopEn = false;
}

/*!
 * @brief Set the WDG timeout interrupt
 *
 * @param[in] instance: The WDG instance number
 * @param[in] enable: Enable/disable WDG timeout interrupt
 * @return the result of operation
 *         - STATUS_SUCCESS: set successful
 *         - STATUS_ERROR: set failed
 */
status_t WDG_DRV_SetInt(uint8_t instance, bool enable)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type * base = s_wdgBase[instance];
    status_t status = STATUS_SUCCESS;

    /* If WDG can be reconfigured */
    if (WDG_IsUpdateEnabled(base))
    {
        DisableInterrupts;
        WDG_SetInt(base, enable);
        EnableInterrupts;
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Clear interrupt flag of the WDG
 *
 * @param[in] instance: The WDG instance number
 * @return none
 */
void WDG_DRV_ClearIntFlag(uint8_t instance)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type * base = s_wdgBase[instance];

    WDG_Unlock(base);
    /* Clear the interrupt flag of WDG */
    WDG_ClearInterruptFlag(base);
}

/*!
 * @brief Refreshes the WDG counter
 *
 * @param[in] instance: The WDG instance number
 * @return none
 */
void WDG_DRV_Trigger(uint8_t instance)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type * base = s_wdgBase[instance];

    /* Refreshes the WDG counter */
    WDG_Feed(base);
}

/*!
 * @brief Gets the value of the WDG counter
 *
 * @param[in] instance: The WDG instance number
 * @return the value of the WDG counter
 */
uint16_t WDG_DRV_GetCounter(uint8_t instance)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    const WDG_Type * base = s_wdgBase[instance];

    /* Get the value of counter */
    return  WDG_GetTimeoutCnt(base);
}

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
status_t WDG_DRV_SetWindow(uint8_t instance, bool enable, uint16_t windowValue)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type * base = s_wdgBase[instance];
    status_t status = STATUS_SUCCESS;

    /* If WDG can be reconfigured */
    if (WDG_IsUpdateEnabled(base))
    {
        WDG_Unlock(base);
        if (enable)
        {
            /* Set window value for the WDG */
            WDG_SetWindowVal(base, windowValue);
            /* Enable the window mode */
            WDG_EnableWindow(base);
        }
        else
        {
            WDG_DisableWindow(base);
        }
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Sets the mode operation of the WDG
 *
 * @param[in] instance: The WDG instance number
 * @param[in] enable: Enable/disable debug/stop mode
 * @param[in] setMode: Select mode of the WDG
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t WDG_DRV_SetMode(uint8_t instance, bool enable, wdg_set_mode_t setMode)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type * base = s_wdgBase[instance];
    status_t status = STATUS_SUCCESS;

    /* If WDG can be reconfigured */
    if (WDG_IsUpdateEnabled(base))
    {
        switch (setMode)
        {
        case WDG_DEBUG_MODE:
            /* Set WDG debug mode */
            WDG_SetDebug(base, enable);
            break;

        case WDG_STOP_MODE:
            /* Set WDG stop mode */
            WDG_SetStop(base, enable);
            break;

        default:
            break;
        }
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Sets the value of the WDG timeout
 *
 * @param[in] instance: The WDG instance number
 * @param[in] timeout: The value of the WDG timeout
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t WDG_DRV_SetTimeout(uint8_t instance, uint16_t timeout)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type * base = s_wdgBase[instance];
    status_t status = STATUS_SUCCESS;

    /* If WDG can be reconfigured */
    if (WDG_IsUpdateEnabled(base))
    {
        WDG_Unlock(base);
        /* Set timeout value */
        WDG_SetTimeOutVal(base, timeout);
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Sets the WDG test mode
 *
 * @param[in] instance: The WDG instance number
 * @param[in] testMode: Test modes for the WDG
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: operation failed
 */
status_t WDG_DRV_SetTestMode(uint8_t instance, wdg_test_mode_t testMode)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type * base = s_wdgBase[instance];
    status_t status = STATUS_SUCCESS;

    /* If WDG can be reconfigured */
    if (WDG_IsUpdateEnabled(base))
    {
        WDG_Unlock(base);
        /* Set test mode */
        WDG_SetTestMode(base, testMode);
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Gets the WDG test mode
 *
 * @param[in] instance: The WDG instance number
 * @return test modes for the WDG
 */
wdg_test_mode_t WDG_DRV_GetTestMode(uint8_t instance)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    const WDG_Type * base = s_wdgBase[instance];
    wdg_test_mode_t testMode = WDG_TST_DISABLED;

    /* Get test mode */
    switch (WDG_GetTestMode(base))
    {
    case WDG_TST_DISABLED:
        testMode = WDG_TST_DISABLED;
        break;

    case WDG_TST_ENABLED:
        testMode = WDG_TST_ENABLED;
        break;

    default:
        testMode = WDG_TST_DISABLED;
        break;
    }

    return testMode;
}

/*!
 * @brief Set WDG callback function
 *
 * @param[in] instance: WDG instance
 * @param[in] function: The callback function of the WDG
 * @return none
 */
void WDG_DRV_InstallCallback(uint8_t instance, wdg_callback_t function)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    s_wdgCallback[instance] = function;
}


/*!
 * @brief WDG tnterrupt handler
 *
 * @param[in] instance: WDG instance
 * @return none
 */
void WDG_DRV_IRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < WDG_INSTANCE_MAX);

    WDG_Type * base = s_wdgBase[instance];

    WDG_Unlock(base);
    /* Clear the interrupt flag of WDG */
    WDG_ClearInterruptFlag(base);
    if (s_wdgCallback[instance] != NULL)
    {
       s_wdgCallback[instance]();
    }
}

/* =============================================  EOF  ============================================== */
