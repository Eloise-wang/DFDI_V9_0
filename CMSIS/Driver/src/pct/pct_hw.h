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
 * @file pct_hw.h
 *
 * @brief This file provides pct integration functions interface.
 *
 */

#ifndef _PCT_HW_H
#define _PCT_HW_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "pct_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes PCT to default value.
 *
 * @param[in] base: base address of PCT
 * @return none
 */
void PCT_Init(PCT_Type* const base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Gets the compare flag.
 *
 * @param[in] base: base address of PCT
 * @return The compare flag
 */
static inline bool PCT_GetCompareFlag(const PCT_Type* const base)
{
    return ((READ_BIT32(base->CSR, PCT_CSR_CF_Msk) >> PCT_CSR_CF_Pos) != 0U);
}

/*!
 * @brief Clears the compare flag.
 *
 * @param[in] base: base address of PCT
 * @return none
 */
static inline void PCT_ClearCompareFlag(PCT_Type* const base)
{
    SET_BIT32(base->CSR, PCT_CSR_CF_Msk);
}

/*!
 * @brief Gets if the interrupt enabled.
 *
 * @param[in] base: base address of PCT
 * @return Interrupt Enable state
 * @return none
 */
static inline bool PCT_GetInterruptEnable(const PCT_Type* const base)
{
    return ((READ_BIT32(base->CSR, PCT_CSR_CIE_Msk) >> PCT_CSR_CIE_Pos) != 0U);
}

/*!
 * @brief Configures if the iInterrupt enabled.
 *
 * @param[in] base: base address of PCT
 * @param[in] enable: enable & disable interrupt control
 * @return none
 */
static inline void PCT_SetInterrupt(PCT_Type* const base, bool enable)
{
    MODIFY_REG32(base->CSR, PCT_CSR_CIE_Msk, PCT_CSR_CIE_Pos, enable);
}

/*!
 * @brief Gets the pin selection for pulse counter mode.
 *
 * @param[in] base: base address of PCT
 * @return pulse pin selection
 */
static inline pct_pinselect_t PCT_GetPinSelect(const PCT_Type* const base)
{
    return ((pct_pinselect_t)(READ_BIT32(base->CSR, PCT_CSR_IPS_Msk) >> PCT_CSR_IPS_Pos));
}

/*!
 * @brief Selects pin selection for pulse counter mode.
 *
 * @param[in] base: base address of PCT
 * @param[in] pinsel: pin selection to be configured
 * @return none
 */
static inline void PCT_SetPinSelect(PCT_Type* const base, const pct_pinselect_t pinsel)
{
    MODIFY_REG32(base->CSR, PCT_CSR_IPS_Msk, PCT_CSR_IPS_Pos, (uint32_t)pinsel);
}

/*!
 * @brief Gets pin polarity for pulse counter mode.
 *
 * @param[in] base: base address of PCT
 * @return pulse pin polarity got
 */
static inline pct_pinpolarity_t PCT_GetPinPolarity(const PCT_Type* const base)
{
    return ((pct_pinpolarity_t)(READ_BIT32(base->CSR, PCT_CSR_PPS_Msk) >> PCT_CSR_PPS_Pos));
}

/*!
 * @brief Sets pin polarity for pulse counter mode.
 *
 * @param[in] base: base address of PCT
 * @param[in] pol : the pin polarity to be configured
 * @return none
 */
static inline void PCT_SetPinPolarity(PCT_Type* const base, const pct_pinpolarity_t pol)
{
    MODIFY_REG32(base->CSR, PCT_CSR_PPS_Msk, PCT_CSR_PPS_Pos, pol);
}

/*!
 * @brief Gets if PCT is in free running state.
 *
 * @param[in] base: base address of PCT
 * @return Free running mode state
 */
static inline bool PCT_GetFreeRunning(const PCT_Type* const base)
{
    return ((READ_BIT32(base->CSR, PCT_CSR_FRE_Msk) >> PCT_CSR_FRE_Pos) != 0U);
}

/*!
 * @brief Sets the free running state of PCT.
 *
 * @param[in] base : base address of PCT
 * @param[in] enable: enable & disable free running state
 * @return none
 */
static inline void PCT_SetFreeRunning(PCT_Type* const base, const bool enable)
{
    MODIFY_REG32(base->CSR, PCT_CSR_FRE_Msk, PCT_CSR_FRE_Pos, (uint32_t)enable);
}

/*!
 * @brief Gets the work mode of PCT.
 *
 *
 * @param[in] base: base address of PCT
 * @return work mode
 */
static inline pct_workmode_t PCT_GetWorkMode(const PCT_Type* const base)
{
    return ((pct_workmode_t)(READ_BIT32(base->CSR, PCT_CSR_MS_Msk) >> PCT_CSR_MS_Pos));
}

/*!
 * @brief Sets the work mode.
 *
 * @param[in] base: base address of PCT
 * @param[in] mode: work mode to be configured
 * @return none
 */
static inline void PCT_SetWorkMode(PCT_Type* const base, const pct_workmode_t mode)
{
    MODIFY_REG32(base->CSR, PCT_CSR_MS_Msk, PCT_CSR_MS_Pos, (uint32_t)mode);
}

/*!
 * @brief Gets if PCT is enabled.
 *
 * @param[in] base: base address of PCT
 * @return The enable state of the PCT
 */
static inline bool PCT_GetEnable(const PCT_Type* const base)
{
    return ((READ_BIT32(base->CSR, PCT_CSR_PCT_EN_Msk) >> PCT_CSR_PCT_EN_Pos) != 0U);
}

/*!
 * @brief Enable PCT and starts counting.
 *
 * @param[in] base: base address of PCT
 * @return none
 */
static inline void PCT_Enable(PCT_Type* const base)
{
    MODIFY_REG32(base->CSR, PCT_CSR_PCT_EN_Msk, PCT_CSR_PCT_EN_Pos, 1U);
}

/*!
 * @brief Disable PCT and stop counting.
 *
 * @param[in] base: base address of PCT
 * @return none
 */
static inline void PCT_Disable(PCT_Type* const base)
{
    MODIFY_REG32(base->CSR, PCT_CSR_PCT_EN_Msk, PCT_CSR_PCT_EN_Pos, 0U);
}

/*!
 * @brief Gets the prescaler/flter value.
 *
 * @param[in] base: base address of PCT
 * @return The prescaler/filter value
 */
static inline pct_prescaler_t PCT_GetPrescaler(const PCT_Type* const base)
{
    return ((pct_prescaler_t)(READ_BIT32(base->PSR, PCT_PSR_PS_VAL_Msk) >> PCT_PSR_PS_VAL_Pos));
}

/*!
 * @brief Sets the prescaler/filter value.
 *
 * @param[in] base : base address of PCT
 * @param[in] presc: the Prescaler value to be configured
 * @return none
 */
static inline void PCT_SetPrescaler(PCT_Type* const base, const pct_prescaler_t presc)
{
    MODIFY_REG32(base->PSR, PCT_PSR_PS_VAL_Msk, PCT_PSR_PS_VAL_Pos, (uint32_t)presc);
}

/*!
 * @brief Gets if the prescaler/filter is bypassed.
 *
 * @param[in] base: base address of PCT
 * @return The prescaler/filter bypass state

 */
static inline bool PCT_GetBypass(const PCT_Type* const base)
{
    return ((READ_BIT32(base->PSR, PCT_PSR_BYP_Msk) >> PCT_PSR_BYP_Pos) != 0U);
}

/*!
 * @brief Sets Read cval register presivison.
 *
 * @param[in] base: base address of PCT
 * @param[in] sel: Read cval register presivison value
 * @return none
 */
static inline void PCT_SetCvalReadSel(PCT_Type* const base, const pct_sel_t sel)
{
    MODIFY_REG32(base->PSR, PCT_PSR_CVAL_SEL_Msk, PCT_PSR_CVAL_SEL_Pos, sel);
}

/*!
 * @brief Gets Read cval register presivison.
 *
 * @param[in] base: base address of PCT
 * @return pct sel value
 */
static inline pct_sel_t PCT_GetCvalReadSel(const PCT_Type* const base)
{
    return ((pct_sel_t)(READ_BIT32(base->PSR, PCT_PSR_CVAL_SEL_Msk) >> PCT_PSR_CVAL_SEL_Pos));
}

/*!
 * @brief Sets if the prescaler/filter is bypassed.
 *
 * @param[in] base : base address of PCT
 * @param[in] enable: the prescaler/filter bypass state to be configured
 * @return none
 */
static inline void PCT_SetBypass(PCT_Type* const base, const bool enable)
{
    MODIFY_REG32(base->PSR, PCT_PSR_BYP_Msk, PCT_PSR_BYP_Pos, (uint32_t)enable);
}

/*!
 * @brief Gets the PCT input Clock selection.
 *
 * @param[in] base: base address of PCT
 * @return The Clock source of PCT
 */
static inline pct_clocksource_t PCT_GetClockSelect(const PCT_Type* const base)
{
    return ((pct_clocksource_t)(READ_BIT32(base->PSR, PCT_PSR_PCS_Msk) >> PCT_PSR_PCS_Pos));
}

/*!
 * @brief Sets the PCT input Clock selection.
 *
 * @param[in] base: base address of PCT
 * @param[in] clocksel: the clock cource to be configured
 * @return none
 */
static inline void PCT_SetClockSelect(PCT_Type* const base, const pct_clocksource_t clocksel)
{
    MODIFY_REG32(base->PSR, PCT_PSR_PCS_Msk, PCT_PSR_PCS_Pos, (uint32_t)clocksel);
}

/*!
 * @brief Gets the compare value.
 *
 * @param[in] base: base address of PCT
 * @return The compare value got
 */
static inline uint16_t PCT_GetCompareValue(const PCT_Type* const base)
{
    return ((uint16_t)(READ_BIT32(base->CMR, PCT_CMR_CMR_Msk) >> PCT_CMR_CMR_Pos));
}

/*!
 * @brief Sets the compare value
 *
 * @param[in] base: base address of PCT
 * @param[in] compval: The Compare Value to be configured
 * @return none
 */
static inline void PCT_SetCompareValue(PCT_Type* const base, const uint16_t compval)
{
    MODIFY_REG32(base->CMR, PCT_CMR_CMR_Msk, PCT_CMR_CMR_Pos, (uint32_t)compval);
}

/*!
 * @brief Gets the current counter Value.
 *
 * @param[in] base: base address of PCT
 * @return The current counter Value got
 */
static inline uint16_t PCT_GetCounterValue(PCT_Type* const base)
{
    return (uint16_t)(base->CNR);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PCT_HW_H */

/* =============================================  EOF  ============================================== */
