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
 * @file acmp_hw.c
 *
 * @brief This file provides analog comparator module register access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "acmp_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set ACMP polling mode.
 *
 * @param[in] base: acmp module base
 * @param[in] polling: polling mode
 *                      - ACMP_NONE_POLLING
 *                      - ACMP_POSITIVE_POLLING
 *                      - ACMP_NEGATIVE_POLLING
 * @return none
 */
void ACMP_SetPollingMode(ACMP_Type *base, const acmp_input_polling_t polling)
{
    bool positivePolling = false;

    if (ACMP_NONE_POLLING == polling)
    {
        ACMP_SetPositivePollingEnableFlag(base, false);
        ACMP_SetNegativePollingEnableFlag(base, false);
    }
    else
    {
        if (ACMP_POSITIVE_POLLING == polling)
        {
            positivePolling = true;
        }
        else
        {
            positivePolling = false;
        }
        ACMP_SetPositivePollingEnableFlag(base, positivePolling);
        ACMP_SetNegativePollingEnableFlag(base, !positivePolling);
    }
}

/*!
 * @brief Get ACMP polling mode.
 *
 * @param[in] base: acmp module base
 * @return current polling mode
 *                      - ACMP_NONE_POLLING
 *                      - ACMP_POSITIVE_POLLING
 *                      - ACMP_NEGATIVE_POLLING
 */
acmp_input_polling_t ACMP_GetPollingMode(const ACMP_Type *base)
{
    acmp_input_polling_t polling = ACMP_NONE_POLLING;
    bool positive = ACMP_GetPositivePollingEnableFlag(base);
    bool negative = ACMP_GetNegativePollingEnableFlag(base);

    if (positive && !negative)
    {
        polling = ACMP_POSITIVE_POLLING;
    }
    else if (!positive && negative)
    {
        polling = ACMP_NEGATIVE_POLLING;
    }
    else
    {
        polling = ACMP_NONE_POLLING;
    }

    return polling;
}

/* =============================================  EOF  ============================================== */
