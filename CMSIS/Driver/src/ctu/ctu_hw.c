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
 * @file ctu_hw.c
 *
 * @brief This file provides ctu hardware access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "ctu_hw.h"

/* ============================================  Define  ============================================ */
/* Number of possible outputs(target module) for TRGMUX IP */
#define TRGMUX_NUM_TARGET_MODULES ((uint8_t)(sizeof(s_trgmuxTargetModule) / sizeof(trgmux_target_module_t)))

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initial all trgmux register with default values.
 *
 * @param[in] base: the ctu base address
 * @return lock status
 *         - STATUS_SUCCESS: not lock, all trgmux register be set with default value
 *         - STATUS_ERROR: at least one register is lock, trgmux register has not be set with default value
 */
status_t TRGMUX_Init(CTU_Type * const base)
{
    DEVICE_ASSERT(base != NULL);

    static const trgmux_target_module_t s_trgmuxTargetModule[] = FEATURE_TRGMUX_TARGET_MODULE;
    uint8_t i = 0U;
    bool lockState = false;
    status_t status = STATUS_ERROR;

    while ((i < TRGMUX_NUM_TARGET_MODULES) && (lockState != true))
    {
        lockState = TRGMUX_GetLockForTargetModule(base, s_trgmuxTargetModule[i++]);
    }

    if (false == lockState)
    {
        for (i = 0U; i < TRGMUX_NUM_TARGET_MODULES; i++)
        {
            TRGMUX_SetTrigSourceForTargetModule(base, (trgmux_trigger_source_t)0U, s_trgmuxTargetModule[i]);
        }
        status = STATUS_SUCCESS;
    }

    return (status);
}

/* =============================================  EOF  ============================================== */
