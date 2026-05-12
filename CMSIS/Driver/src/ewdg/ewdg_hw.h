/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc.	and/or its licensors, any
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
 * @file ewdg_hw.h
 *
 * @brief ewdg access register inline function definition.
 *
 */

#ifndef _EWDG_HW_H
#define _EWDG_HW_H

#if defined(__cplusplus)
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "ewdg_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Refresh EWDG.
 *
 * Call the function wihth in window defined in CMPH and CMPL register
 *
 * @param[in] base: EWDG base pointer
 * @return none
 */
static inline void EWDG_Refresh(EWDG_Type * const base)
{
    /* Write first byte of the service key */
    base->SERV = EWDG_KEY_FIRST_BYTE;
    /* Write second byte of the service key */
    base->SERV = EWDG_KEY_SECOND_BYTE;
}

/*!
 * @brief Get the EWDG enable bit.
 *
 * @param[in] base: EWDG base pointer
 * @return the EWDG enabled status
 *         -FALSE: EWDG is disabled
 *         -TRUE: EWDG is enabled
 */
static inline bool EWDG_IsEnabled(const EWDG_Type * const base)
{
    return (((base->CTRL & EWDG_CTRL_EWDGEN_Msk) >> EWDG_CTRL_EWDGEN_Pos) != 0U);
}

/*!
 * @brief Set the EWDG Control register.
 *
 * @param[in] base: EWDG base pointer
 * @param[in] value: Value to write into Control register
 * @return none
 */
static inline void EWDG_SetControl(EWDG_Type * const base, uint8_t value)
{
    base->CTRL = value;
}

/*!
 * @brief Get the EWDG Control register Value.
 *
 * @param[in] base: EWDG base pointer
 * @return Value stored in EWDG Control register
 */
static inline uint8_t EWDG_GetControl(const EWDG_Type * const base)
{
    return base->CTRL;
}

/*!
 * @brief Set the Compare Low register.
 *
 * @param[in] base: EWDG base pointer
 * @param[in] value: Value to write into Compare Low register
 * @return none
 */
static inline void EWDG_SetCompareLow(EWDG_Type * const base, uint8_t value)
{
    base->CMPL = value;
}

/*!
 * @brief Set the Compare High register.
 *
 * @param[in] base: EWDG base pointer
 * @param[in] value: Value to write into Compare High register
 * @return none
 */
static inline void EWDG_SetCompareHigh(EWDG_Type * const base, uint8_t value)
{
    base->CMPH = value;
}

/*!
 * @brief Set the Prescaler Register.
 *
 * @param[in] base: EWDG base pointer
 * @param[in] value: Prescaler Value
 * @return none
 */
static inline void EWDG_SetPrescaler(EWDG_Type * const base, uint8_t value)
{
    base->CLKPRESCALER = value;
}

#if defined(__cplusplus)
}
#endif

#endif /* _EWDG_HW_H */

/* =============================================  EOF  ============================================== */
