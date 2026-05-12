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
 * @file cmu_hw.h
 *
 * @brief cmu access register inline function definition.
 *
 */

#ifndef _CMU_HW_H
#define _CMU_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "cmu_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief enable the module.
 *
 * @param[in] base: base address of CMU
 * @return None
 */
static inline void CMU_Enable(CMU_Type * const base)
{
    SET_BIT32(base->CR, CMU_CR_MONE_Msk);
}

/*!
 * @brief disable the module.
 *
 * @param[in] base: base address of CMU
 * @return None
 */
static inline void CMU_Disable(CMU_Type * const base)
{
    CLEAR_BIT32(base->CR, CMU_CR_MONE_Msk);
}

/*!
 * @brief Set the Reference clock counter window length.
 *
 * @param[in] base: base address of CMU
 * @param[in] length: window length
 * @return None
 */
static inline void CMU_SetWindowLen(CMU_Type * const base, const uint16_t length)
{
    WRITE_REG32(base->RCCR, length);
}

/*!
 * @brief Set the upper threshold of the monitored clock counter.
 *
 * @param[in] base: base address of CMU
 * @param[in] threshold: upper threshold
 * @return None
 */
static inline void CMU_SetHighThreshold(CMU_Type * const base, const uint32_t threshold)
{
    WRITE_REG32(base->HTCR, threshold);
}

/*!
 * @brief Set the lower threshold of the monitored clock counter.
 *
 * @param[in] base: base address of CMU
 * @param[in] threshold: lower threshold
 * @return None
 */
static inline void CMU_SetLowThreshold(CMU_Type * const base, const uint32_t threshold)
{
    WRITE_REG32(base->LTCR, threshold);
}

/*!
 * @brief Get the state of CMU
 *
 * @param[in] base: base address of CMU
 *
 * @return CMU State
 */
static inline uint8_t CMU_GetState(CMU_Type * const base)
{
    return ((uint8_t)base->SR);
}

/*!
 * @brief Clear the state of CMU
 *
 * @param[in] base: base address of CMU
 *
 * @return None
 */
static inline void CMU_ClearState(CMU_Type * const base)
{
    SET_BIT32(base->SR, (CMU_SR_FLL_Msk | CMU_SR_FHH_Msk));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CMU_HW_H */

/* =============================================  EOF  ============================================== */
