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
 * @file wdg_hw.c
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

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Sets the WDG by user configuration.
 *
 * @param[in] base: The WDG base pointer
 * @param[in] wdgUserConfig: Pointer to user configuration structure
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: The register of WDG is write once
 */
status_t WDG_Config(WDG_Type * const base, const wdg_user_config_t * wdgUserConfig)
{
    status_t status = STATUS_SUCCESS;
    uint32_t CS0 = base->CS0, CS1 = base->CS1;
    bool tmp = WDG_IsUpdateEnabled(base);

    if (tmp)
    {
        CS1 &= ~(WDG_CS1_FLG_Msk | WDG_CS1_CLK_Msk | WDG_CS1_PRES_Msk | WDG_CS1_WIN_Msk);
        /* config wdg clocksource */
        CS1 |= (((uint32_t)wdgUserConfig->clkSource) << WDG_CS1_CLK_Pos) & WDG_CS1_CLK_Msk;
        /* config wdg psr enable */
        if (wdgUserConfig->prescalerEn)
        {
            CS1 |= WDG_CS1_PRES_Msk;
        }
        /* config wdg win enable */
        if (wdgUserConfig->winEn)
        {
            CS1 |= WDG_CS1_WIN_Msk;
        }
        CS0 &= ~(WDG_CS0_EN_Msk | WDG_CS0_INT_Msk | WDG_CS0_STOP_Msk | WDG_CS0_DBG_Msk | WDG_CS0_UPDATE_Msk);
        /* config wdg update */
        if (wdgUserConfig->updateEn)
        {
            CS0 |= WDG_CS0_UPDATE_Msk;
        }
        /* config wdg enable */
        CS0 |= WDG_CS0_EN_Msk;
        /* config wdg int enable */
        if (wdgUserConfig->intEn)
        {
            CS0 |= WDG_CS0_INT_Msk;
        }
        /* config wdg debug mode */
        if (wdgUserConfig->opMode.debugEn)
        {
            CS0 |= WDG_CS0_DBG_Msk;
        }
        /* config wdg stop mode */
        if (wdgUserConfig->opMode.stopEn)
        {
            CS0 |= WDG_CS0_STOP_Msk;
        }
        DisableInterrupts;
        WDG_Unlock(base);
        while (!WDG_IsUnlocked(base))
        {
            /* Wait until register are unlocked */
        }
        WDG_SetTimeOutVal(base, wdgUserConfig->timeoutValue);
        if (wdgUserConfig->winEn)
        {
            WDG_SetWindowVal(base, wdgUserConfig->windowValue);
        }
        WDG_SetCS1(base, CS1);
        WDG_SetCS0(base, CS0);
        while (WDG_IsUnlocked(base))
        {
            /* Wait until register are locked */
        }
        EnableInterrupts;
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief De-initial WDG.
 *
 * @param[in] base: The WDG base pointer
 * @return none
 */
void WDG_Deinit(WDG_Type * const base)
{
    /* Unlock WDG */
    WDG_Unlock(base);

    /* Set default value to WDG register, and disable WDG */
    WDG_SetCS0(base, WDG_CS0_RESET_VALUE);
    WDG_SetCS1(base, WDG_CS1_RESET_VALUE);
    WDG_SetTimeOutVal(base, WDG_TOVAL_RESET_VALUE);
    WDG_SetWindowVal(base, WDG_WIN_RESET_VALUE);

    /* Refresh counter value */
    WDG_Feed(base);
}

/*!
 * @brief Gets the current WDG configuration.
 *
 * @param[in] base: The WDG base pointer
 * @return the current WDG configuration
 */
wdg_user_config_t WDG_GetConfig(const WDG_Type * base)
{
    wdg_user_config_t config;
    uint32_t CS0 = base->CS0, CS1 = base->CS1;

    /* get wdg cs0/1 register value */
    config.timeoutValue = (uint16_t)base->TOVAL;
    config.windowValue = (uint16_t)base->WIN;
    config.updateEn = ((CS0 & WDG_CS0_UPDATE_Msk) != 0U);
    config.winEn = ((CS1 & WDG_CS1_WIN_Msk) != 0U);
    config.prescalerEn = ((CS1 & WDG_CS1_PRES_Msk) != 0U);
    config.opMode.debugEn = ((CS0 & WDG_CS0_DBG_Msk) != 0U);
    config.opMode.stopEn = ((CS0 & WDG_CS0_STOP_Msk) != 0U);
    config.intEn = ((CS0 & WDG_CS0_INT_Msk) != 0U);

    switch ((CS1 & WDG_CS1_CLK_Msk) >> WDG_CS1_CLK_Pos)
    {
    case WDG_BUS_CLOCK:
        config.clkSource = WDG_BUS_CLOCK;
        break;

    case WDG_HSI_CLOCK:
        config.clkSource = WDG_HSI_CLOCK;
        break;

    case WDG_HSE_CLOCK:
        config.clkSource = WDG_HSE_CLOCK;
        break;

    default:
        config.clkSource = WDG_LSI_CLOCK;
        break;
    }

    return config;
}

/*!
 * @brief Enable/disable the WDG timeout interrupt.
 *
 * @param[in] base: The WDG base pointer
 * @param[in] enable: Enable/disable WDG timeout interrupt
 * @return none
 */
void WDG_SetInt(WDG_Type * const base, bool enable)
{
    /* Unlock WDG register */
    WDG_Unlock(base);
    /* Enable/disable WDG interrupt */
    if (enable)
    {
        WDG_EnableInterrupt(base);
    }
    else
    {
        WDG_DisableInterrupt(base);
    }
}

/*!
 * @brief Enable/disable WDG debug mode.
 *
 * @param[in] base: The WDG base pointer
 * @param[in] enable: Enable/disable WDG debug mode
 * @return none
 */
void WDG_SetDebug(WDG_Type * const base, bool enable)
{
    WDG_Unlock(base);
    if (enable)
    {
        WDG_EnableDebug(base);
    }
    else
    {
        WDG_DisableDebug(base);
    }
}

/*!
 * @brief Enable/disable WDG stop mode.
 *
 * @param[in] base: The WDG base pointer
 * @param[in] enable: Enable/disable WDG stop mode
 * @return none
 */
void WDG_SetStop(WDG_Type * const base, bool enable)
{
    WDG_Unlock(base);
    if (enable)
    {
        WDG_EnableStop(base);
    }
    else
    {
        WDG_DisableStop(base);
    }
}

/* =============================================  EOF  ============================================== */
