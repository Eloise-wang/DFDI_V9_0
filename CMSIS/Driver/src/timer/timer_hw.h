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
 * @file timer_hw.h
 *
 * @brief This file provides timer integration functions interface.
 *
 */

#ifndef _TIMER_HW_H
#define _TIMER_HW_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "timer_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enables the functional clock of the TIMER module.
 *
 * @param[in] base: TIMER base address
 * @return None
 */
static inline void TIMER_Enable(TIMER_CTRL_Type * const base)
{
    MODIFY_REG32(base->CR, TIMER_CTRL_CR_MC_EN_Msk, TIMER_CTRL_CR_MC_EN_Pos, 1U);
}

/*!
 * @brief Disables the functional clock of TIMER module.
 *
 * @param[in] base: TIMER base address
 * @return None
 */
static inline void TIMER_Disable(TIMER_CTRL_Type * const base)
{
    MODIFY_REG32(base->CR, TIMER_CTRL_CR_MC_EN_Msk, TIMER_CTRL_CR_MC_EN_Pos, 0U);
}

/*!
 * @brief Starts the timer channel counting.
 *
 * @param[in] base: TIMER base address
 * @param[in] mask: Timer channels starting mask
 * @return None
 */
static inline void TIMER_StartChannels(TIMER_CTRL_Type * const base, uint32_t mask)
{
    SET_BIT32(base->ENR, mask);
}

/*!
 * @brief Stops the timer channel from counting.
 *
 * @param[in] base: TIMER base address
 * @param[in] mask: Timer channels stopping mask
 * @return None
 */
static inline void TIMER_StopChannels(TIMER_CTRL_Type * const base, uint32_t mask)
{
    CLEAR_BIT32(base->ENR, mask);
}

/*!
 * @brief Sets the timer channel period in count unit.
 *
 * @param[in] base: TIMER base address
 * @param[in] count: Timer channel period in count unit
 * @return None
 */
static inline void TIMER_SetPeriodByCount(TIMER_CHANNEL_Type * const base, uint32_t count)
{
    WRITE_REG32(base->TVAL, count);
}

/*!
 * @brief Gets the timer channel period in count unit.
 *
 * @param[in] base: TIMER base address
 * @return Timer channel period in count unit
 */
static inline uint32_t TIMER_GetPeriodByCount(const TIMER_CHANNEL_Type * base)
{
    return (base->TVAL);
}

/*!
 * @brief Sets the current timer channel counting value.
 *
 * @param[in] base: TIMER base address
 * @param[in] count: counting value to be set
 * @return None
 */
static inline void TIMER_SetCurrentCount(TIMER_CHANNEL_Type * const base, uint32_t count)
{
    WRITE_REG32(base->CVAL, count);
}

/*!
 * @brief Gets the current timer channel counting value.
 *
 * @param[in] base: TIMER base address
 * @return Current timer channel counting value
 */
static inline uint32_t TIMER_GetCurrentCount(const TIMER_CHANNEL_Type * base)
{
    return (base->CVAL);
}

/*!
 * @brief Enables the interrupt generation for timer channel.
 *
 * @param[in] base: TIMER base address
 * @param[in] mask: The interrupt enabling mask
 * @return None
 */
static inline void TIMER_EnableInterrupt(TIMER_CTRL_Type * const base, uint32_t mask)
{
    SET_BIT32(base->IER, mask);
}

/*!
 * @brief Disables the interrupt for timer channel.
 *
 * @param[in] base: TIMER base address
 * @param[in] mask: The interrupt disabling mask
 * @return None
 */
static inline void TIMER_DisableInterrupt(TIMER_CTRL_Type * const base, uint32_t mask)
{
    CLEAR_BIT32(base->IER, mask);
}

/*!
 * @brief Gets the interrupt flag of timer channel.
 *
 * @param[in] base: TIMER base address
 * @param[in] mask: The interrupt flag getting mask
 * @return The interrupt flag of timer channels
 */
static inline uint32_t TIMER_GetInterruptFlag(const TIMER_CTRL_Type * base, uint32_t mask)
{
    return READ_BIT32(base->SR, mask);
}

/*!
 * @brief Clears the interrupt flag of timer channel.
 *
 * @param[in] base: TIMER base address
 * @param[in] mask: The interrupt flag clearing mask
 * @return None
 */
static inline void TIMER_ClearInterruptFlag(TIMER_CTRL_Type * const base, uint32_t mask)
{
    WRITE_REG32(base->SR, mask);
}

/*!
 * @brief Sets operation mode of timer channel.
 *
 * @param[in] base: TIMER base address
 * @param[in] mode: Operation mode of timer channel
 * @return None
 */
static inline void TIMER_SetChannelModeCmd(TIMER_CHANNEL_Type * const base, timer_modes_t mode)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_MODE_Msk, TIMER_CHANNEL_CTRL_MODE_Pos, mode);
}

/*!
 * @brief Gets current operation mode of timer channel.
 *
 * @param[in] base: TIMER base address
 * @return Operation mode of timer channel
 */
static inline timer_modes_t TIMER_GetChannelModeCmd(const TIMER_CHANNEL_Type * base)
{
    uint32_t tmp;
    timer_modes_t mode;

    tmp = ((READ_BIT32(base->CTRL, TIMER_CHANNEL_CTRL_MODE_Msk)) >> TIMER_CHANNEL_CTRL_MODE_Pos);

    switch (tmp)
    {
    case 0x00U:
        mode = TIMER_PERIODIC_COUNTER;
        break;

    case 0x01U:
        mode = TIMER_DUAL_PERIODIC_COUNTER;
        break;

    case 0x02U:
        mode = TIMER_TRIGGER_ACCUMULATOR;
        break;

    case 0x03U:
        mode = TIMER_INPUT_CAPTURE;
        break;

    default:
        mode = TIMER_PERIODIC_COUNTER;
        break;
    }

    return mode;
}

/*!
 * @brief Sets Read cval register presivison.
 *
 * @param[in] base: TIMER base address
 * @param[in] sel: Read cval register presivison value
 * @return None
 */
static inline void TIMER_SetChannelCavlReadSel(TIMER_CHANNEL_Type * const base, timer_sel_t sel)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_CVAL_SEL_Msk, TIMER_CHANNEL_CTRL_CVAL_SEL_Pos, sel);
}

/*!
 * @brief Gets Read cval register presivison.
 *
 * @param[in] base: TIMER base address
 * @return timer sel value
 */
static inline timer_sel_t TIMER_GetChannelCavlReadSel(TIMER_CHANNEL_Type * const base)
{
    DEVICE_ASSERT(base != NULL);

    return ((timer_sel_t)(READ_BIT32(base->CTRL, TIMER_CHANNEL_CTRL_CVAL_SEL_Msk) >> TIMER_CHANNEL_CTRL_CVAL_SEL_Pos));
}

/*!
 * @brief Sets internal trigger source for timer channel.
 *
 * @param[in] base: TIMER base address
 * @param[in] triggerChannelSelect: Number of the channel selected as trigger source
 * @return None
 */
static inline void TIMER_SetTriggerSelectCmd(TIMER_CHANNEL_Type * const base, uint32_t triggerChannelSelect)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_TTRG_SEL_Msk, TIMER_CHANNEL_CTRL_TTRG_SEL_Pos, triggerChannelSelect);
}

/*!
 * @brief Sets trigger source of timer channel.
 *
 * @param[in] base: TIMER base address
 * @param[in] triggerSource: internal or external source seted as Trigger source of timer channel
 * @return None
 */
static inline void TIMER_SetTriggerSourceCmd(TIMER_CHANNEL_Type * const base, timer_trigger_source_t triggerSource)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_TRG_SRC_Msk, TIMER_CHANNEL_CTRL_TRG_SRC_Pos, triggerSource);
}

/*!
 * @brief Sets timer channel reload on trigger.
 *
 * @param[in] base: TIMER base address
 * @param[in] isReloadOnTrigger: Timer channel reload on trigger
 * @return None
 */
static inline void TIMER_SetReloadOnTriggerCmd(TIMER_CHANNEL_Type * const base, bool isReloadOnTrigger)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_TROT_Msk, TIMER_CHANNEL_CTRL_TROT_Pos, isReloadOnTrigger);
}

/*!
 * @brief Sets timer channel stop on interrupt.
 *
 * @param[in] base: TIMER base address
 * @param[in] isStopOnInterrupt: Timer channel stop on interrupt
 * @return None
 */
static inline void TIMER_SetStopOnInterruptCmd(TIMER_CHANNEL_Type * const base, bool isStopOnInterrupt)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_TSOI_Msk, TIMER_CHANNEL_CTRL_TSOI_Pos, isStopOnInterrupt);
}

/*!
 * @brief Sets timer channel start on trigger.
 *
 * @param[in] base: TIMER base address
 * @param[in] isStartOnTrigger: Timer channel start on trigger
 * @return None
 */
static inline void TIMER_SetStartOnTriggerCmd(TIMER_CHANNEL_Type * const base, bool isStartOnTrigger)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_TSOT_Msk, TIMER_CHANNEL_CTRL_TSOT_Pos, isStartOnTrigger);
}

/*!
 * @brief Sets timer channel to be chained or not chained. (The timer channel 0 cannot be chained)
 *
 * @param[in] base: TIMER base address
 * @param[in] isChannelChained Timer channel chaining control
 * @return None
 */
static inline void TIMER_SetChannelChainCmd(TIMER_CHANNEL_Type * const base, bool isChannelChained)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_CHN_EN_Msk, TIMER_CHANNEL_CTRL_CHN_EN_Pos, isChannelChained);
}

/*!
 * @brief Sets operation of TIMER in debug mode.
 *
 * @param[in] base: TIMER base address
 * @param[in] isRunInDebug: enable or disable TIMER run in debug mode
 * @return None
 */
static inline void TIMER_SetRunInDebugCmd(TIMER_CTRL_Type * const base, bool isRunInDebug)
{
    MODIFY_REG32(base->CR, TIMER_CTRL_CR_DBG_EN_Msk, TIMER_CTRL_CR_DBG_EN_Pos, isRunInDebug);
}

/*!
 * @brief Set timer channel timeout value update time.
 *
 * @param[in] base: TIMER base address
 * @param[in] state: timer period take effect mode selection
                - period value take effect when timout occured
                - period value take effect immediately
 * @return None
 */
static inline void TIMER_SetValueUpdate(TIMER_CHANNEL_Type * const base, bool state)
{
    MODIFY_REG32(base->CTRL, TIMER_CHANNEL_CTRL_TVAL_UP_Msk, TIMER_CHANNEL_CTRL_TVAL_UP_Pos, state);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMER_HW_H */

/* =============================================  EOF  ============================================== */
