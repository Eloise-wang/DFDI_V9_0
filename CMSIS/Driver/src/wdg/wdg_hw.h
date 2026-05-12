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
 * @file wdg_hw.h
 *
 * @brief WDG access register inline function definition.
 *
 */

#ifndef _WDG_HW_H
#define _WDG_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "wdg_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Sets the WDG by user configuration.
 *
 * @param[in] base: The WDG base pointer
 * @param[in] wdgUserConfig: Pointer to user configuration structure
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: The register of WDG is write once
 */
status_t WDG_Config(WDG_Type * const base, const wdg_user_config_t * wdgUserConfig);

/*!
 * @brief De-initial WDG.
 *
 * @param[in] base: The WDG base pointer
 * @return none
 */
void WDG_Deinit(WDG_Type * const base);

/*!
 * @brief Gets the current WDG configuration.
 *
 * @param[in] base: The WDG base pointer
 * @return the current WDG configuration
 */
wdg_user_config_t WDG_GetConfig(const WDG_Type * base);

/*!
 * @brief Enable/disable the WDG timeout interrupt.
 *
 * @param[in] base: The WDG base pointer
 * @param[in] enable: Enable/disable WDG timeout interrupt
 * @return none
 */
void WDG_SetInt(WDG_Type * const base, bool enable);

/*!
 * @brief Enable/disable WDG debug mode.
 *
 * @param[in] base: The WDG base pointer
 * @param[in] enable: Enable/disable WDG debug mode
 * @return none
 */
void WDG_SetDebug(WDG_Type * const base, bool enable);

/*!
 * @brief Enable/disable WDG stop mode.
 *
 * @param[in] base: The WDG base pointer
 * @param[in] enable: Enable/disable WDG stop mode
 * @return none
 */
void WDG_SetStop(WDG_Type * const base, bool enable);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Verifies if the WDG is enabled.
 *
 * @param[in] base: WDG base pointer
 * @return the state of the WDG
 */
static inline bool WDG_IsEnabled(const WDG_Type * base)
{
    return (((base->CS0 & WDG_CS0_EN_Msk) >> WDG_CS0_EN_Pos) != 0U);
}

/*!
 * @brief Verifies if the WDG updates are allowed.
 *
 * @param[in] base: WDG base pointer
 * @return the state of the WDG updates:
 *         - false: updates are not allowed
 *         - true: updates are allowed
 */
static inline bool WDG_IsUpdateEnabled(const WDG_Type * base)
{
    return (((base->CS0 & WDG_CS0_UPDATE_Msk) >> WDG_CS0_UPDATE_Pos) != 0U);
}

/*!
 * @brief Set WDG clock source
 *
 * @param[in] base: WDG base pointer
 * @param[in] clkSource: Clock source for the WDG
 * @return none
 */
static inline void WDG_SetClkSource(WDG_Type * const base, wdg_clk_source_t clkSource)
{
    uint32_t tmp = base->CS1 & (~(WDG_CS1_FLG_Msk | WDG_CS1_CLK_Msk));

    tmp |= ((((uint32_t)clkSource) << WDG_CS1_CLK_Pos) & WDG_CS1_CLK_Msk);
    base->CS1 = tmp;
}

/*!
 * @brief Get WDG clock source
 *
 * @param[in] base: WDG base pointer
 * @return Mode clock source of the WDG
 */
static inline wdg_clk_source_t WDG_GetClkSource(const WDG_Type * base)
{
    uint32_t tmp = base->CS1;
    wdg_clk_source_t getClkSource;

    switch ((tmp & WDG_CS1_CLK_Msk) >> WDG_CS1_CLK_Pos)
    {
    case WDG_LSI_CLOCK:
        getClkSource = WDG_LSI_CLOCK;
        break;

    case WDG_BUS_CLOCK:
        getClkSource = WDG_BUS_CLOCK;
        break;

    case WDG_HSI_CLOCK:
        getClkSource = WDG_HSI_CLOCK;
        break;

    case WDG_HSE_CLOCK:
        getClkSource = WDG_HSE_CLOCK;
        break;

    default:
        getClkSource = WDG_LSI_CLOCK;
        break;
    }

    return getClkSource;
}

/*!
 * @brief Set WDG counter timeout value
 *
 * @param[in] base: WDG base pointer
 * @param[in] tovalValue: 16bit WDG timeout value
 * @return none
 */
static inline void WDG_SetTimeOutVal(WDG_Type * const base, uint16_t tovalValue)
{
    base->TOVAL = tovalValue;
}

/*!
 * @brief Get WDG counter timeout value
 *
 * @param[in] base: WDG base pointer
 * @return 16bit WDG timeout value
 */
static inline uint16_t WDG_GetTimeOutVal(const WDG_Type * base)
{
    return (base->TOVAL);
}

/*!
 * @brief Set WDG counter window value
 *
 * @param[in] base: WDG base pointer
 * @param[in] winValue: 16 bit WDG window value
 * @return none
 */
static inline void WDG_SetWindowVal(WDG_Type * const base, uint16_t winValue)
{
    base->WIN = winValue;
}

/*!
 * @brief Get WDG counter window value
 *
 * @param[in] base: WDG base pointer
 * @return 16bit WDG window value
 */
static inline uint16_t WDG_GetWindowVal(const WDG_Type * base)
{
    return (base->WIN);
}

/*!
 * @brief Set WDG CS0 reg
 *
 * @param[in] base: WDG base pointer
 * @param[in] value: WDG CS0 reg set value
 * @return none
 */
static inline void WDG_SetCS0(WDG_Type * const base, uint32_t value)
{
    base->CS0 = value;
}

/*!
 * @brief Set WDG CS1 reg
 *
 * @param[in] base: WDG base pointer
 * @param[in] value: WDG CS1 reg set value
 * @return none
 */
static inline void WDG_SetCS1(WDG_Type * const base, uint32_t value)
{
    base->CS1 = value;
}

/*!
 * @brief Enable WDG counting
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_Enable(WDG_Type * const base)
{
    base->CS0 |= WDG_CS0_EN_Msk;
}

/*!
 * @brief Disable WDG counting
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_Disable(WDG_Type * const base)
{
    base->CS0 &= ~WDG_CS0_EN_Msk;
}

/*!
 * @brief Enable WDG update, allow changing WDG register
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_EnableUpdate(WDG_Type * const base)
{
    base->CS0 |= WDG_CS0_UPDATE_Msk;
}

/*!
 * @brief Disable WDG update, not allow changing WDG register
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_DisableUpdate(WDG_Type * const base)
{
    base->CS0 &= ~WDG_CS0_UPDATE_Msk;
}

/*!
 * @brief Enable WDG timeout interrupt,
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_EnableInterrupt(WDG_Type * const base)
{
    base->CS0 |= WDG_CS0_INT_Msk;
}

/*!
 * @brief Disable WDG timeout interrupt
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_DisableInterrupt(WDG_Type * const base)
{
    base->CS0 &= ~WDG_CS0_INT_Msk;
}

/*!
 * @brief Enable WDG debug mode,
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_EnableDebug(WDG_Type * const base)
{
    base->CS0 |= WDG_CS0_DBG_Msk;
}

/*!
 * @brief Disable WDG debug mode
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_DisableDebug(WDG_Type * const base)
{
    base->CS0 &= ~WDG_CS0_DBG_Msk;
}

/*!
 * @brief Enable WDG stop mode,
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_EnableStop(WDG_Type * const base)
{
    base->CS0 |= WDG_CS0_STOP_Msk;
}

/*!
 * @brief Disable WDG stop mode
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_DisableStop(WDG_Type * const base)
{
    base->CS0 &= ~WDG_CS0_STOP_Msk;
}

/*!
 * @brief Enable WDG window mode
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_EnableWindow(WDG_Type * const base)
{
    uint32_t tmp = base->CS1 & (~WDG_CS1_FLG_Msk);

    tmp |= WDG_CS1_WIN_Msk;
    base->CS1 = tmp;
}

/*!
 * @brief Disable WDG window mode
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_DisableWindow(WDG_Type * const base)
{
    uint32_t tmp = base->CS1 & (~WDG_CS1_FLG_Msk);

    tmp &= ~WDG_CS1_WIN_Msk;
    base->CS1 = tmp;
}

/*!
 * @brief Enable WDG fixed 256 prescaler
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_EnablePrescaler(WDG_Type * const base)
{
    uint32_t tmp = base->CS1 & (~WDG_CS1_FLG_Msk);

    tmp |= WDG_CS1_PRES_Msk;
    base->CS1 = tmp;
}

/*!
 * @brief Disable WDG fixed 256 prescaler
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_DisablePrescaler(WDG_Type * const base)
{
    uint32_t tmp = base->CS1 & (~WDG_CS1_FLG_Msk);

    tmp &= ~WDG_CS1_PRES_Msk;
    base->CS1 = tmp;
}

/*!
 * @brief Get WDG timeout interrupt flag
 *
 * @param[in] base: WDG base pointer
 * @return WDG interrupt flag
 */
static inline uint32_t WDG_GetInterruptFlag(const WDG_Type * base)
{
    return (base->CS1 & WDG_CS1_FLG_Msk);
}

/*!
 * @brief Clear WDG timeout interrupt flag
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_ClearInterruptFlag(WDG_Type * const base)
{
    base->CS1 |= WDG_CS1_FLG_Msk;
}

/*!
 * @brief Get WDG timeout counter counting value
 *
 * @param[in] base: WDG base pointer
 * @return WDG cnt
 */
static inline uint16_t WDG_GetTimeoutCnt(const WDG_Type * base)
{
    return (base->CNT);
}

/*!
 * @brief Unlock WDG, prepare to config WDG register
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_Unlock(WDG_Type * const base)
{
    base->CNT = WDG_UNLOCK_FIRST_VALUE;
    (void)base->CNT;
    base->CNT = WDG_UNLOCK_SECOND_VALUE;
    (void)base->CNT;
}

/*!
 * @brief Feed WDG, trig timeout counter to 0
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline void WDG_Feed(WDG_Type * const base)
{
    DisableInterrupts;
    base->CNT = WDG_FEED_FIRST_VALUE;
    (void)base->CNT;
    base->CNT = WDG_FEED_SECOND_VALUE;
    (void)base->CNT;
    EnableInterrupts;
}

/*!
 * @brief Set the test mode of WDG
 *
 * @param[in] base: WDG base pointer
 * @param[in] testMode: The test mode of WDG
 * @return none
 */
static inline void WDG_SetTestMode(WDG_Type * const base, wdg_test_mode_t testMode)
{
    uint32_t tmp = base->CS0 & (~(WDG_CS0_TST_Msk));

    tmp |= ((((uint32_t)testMode) << WDG_CS0_TST_Pos) & WDG_CS0_TST_Msk);
    base->CS0 = tmp;
}

/*!
 * @brief Get the test mode of WDG
 *
 * @param[in] base: WDG base pointer
 * @return none
 */
static inline uint32_t WDG_GetTestMode(const WDG_Type * base)
{
    return ((base->CS0 & WDG_CS0_TST_Msk) >> WDG_CS0_TST_Pos);
}

/*!
 * @brief Check if the WDG is unlocked
 *
 * @param[in] base: WDG base pointer
 * @return true if the module is unlocked, false if the module is locked
 */
static inline bool WDG_IsUnlocked(const WDG_Type * base)
{
    return (((base->CS0 & WDG_CS0_ULK_Msk) >> WDG_CS0_ULK_Pos) != 0U);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WDG_HW_H */

/* =============================================  EOF  ============================================== */
