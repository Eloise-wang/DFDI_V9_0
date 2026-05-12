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
 * @file cmu_drv.c
 *
 * @brief This file provides cmu integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "cmu_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of CMU base address */
static CMU_Type *const s_cmuBase[CMU_INSTANCE_MAX] = CMU_BASE_PTRS;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initialize CMU module.
 *
 * @param[in] instance: CMU module instance
 * @param[in] window: window length
 * @param[in] upper: upper threshold
 * @param[in] low: low threshold
 * @return initialize status success or error
 */
status_t CMU_DRV_Init(uint8_t instance, uint16_t window, uint32_t upper, uint32_t low)
{
    DEVICE_ASSERT(instance < CMU_INSTANCE_MAX);

    CMU_Type *base = s_cmuBase[instance];

    if ((window < 7U) || (upper < low))
    {
        return STATUS_ERROR;
    }

    /* disable monitor and clear flags */
    CMU_Disable(base);
    CMU_ClearState(base);

    CMU_SetWindowLen(base, window);
    CMU_SetHighThreshold(base, upper);
    CMU_SetLowThreshold(base, low);
    CMU_Enable(base);

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable the CMU module.
 *
 * @param[in] instance: CMU module instance
 * @return None
 */
void CMU_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < CMU_INSTANCE_MAX);

    CMU_Type *base = s_cmuBase[instance];

    /* disable monitor and clear flags */
    CMU_Disable(base);
    /* write 1 to clear */
    CMU_ClearState(base);
}

/*!
 * @brief Get the state of CMU
 *
 * @param[in] instance: CMU module instance
 * @return CMU State
 */
uint8_t CMU_DRV_GetState(uint8_t instance)
{
    DEVICE_ASSERT(instance < CMU_INSTANCE_MAX);

    uint8_t state = 0U;

    state = CMU_GetState(s_cmuBase[instance]);
    state &= ((uint8_t)CMU_SR_FLL_Msk | CMU_SR_FHH_Msk | CMU_SR_CLKLOSS_Msk);

    return state;
}

/*!
 * @brief Clear the state of CMU
 *
 * @param[in] base: base address of CMU
 * @return None
 */
void CMU_DRV_ClearState(uint8_t instance)
{
    DEVICE_ASSERT(instance < CMU_INSTANCE_MAX);

    CMU_ClearState(s_cmuBase[instance]);
}

/* =============================================  EOF  ============================================== */
