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
 * @file pdt_hw.c
 *
 * @brief This file provides PDT hardware access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "pdt_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Config PDT pulse out width and output enable or disable.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] config: pointer to pulse out configuration structure
 * @return none
 */
void PDT_ConfigPulseOut(PDT_Type * const base, const pdt_pulse_out_config_type_t *config)
{
    DEVICE_ASSERT(base != NULL);
    DEVICE_ASSERT(config != NULL);

    PDT_SetCmpPulseOutDelayForLow(base, config->poly2);
    PDT_SetCmpPulseOutDelayForHigh(base, config->poly1);
    PDT_SetCmpPulseOutEnable(base, config->pulseOutEnable);
}

/*!
 * @brief Config PDT timer.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] config: pointer to timer configuration structure
 * @return none
 */
void PDT_ConfigTimer(PDT_Type * const base, const pdt_timer_config_t *config)
{
    DEVICE_ASSERT(base != NULL);
    DEVICE_ASSERT(config != NULL);

    PDT_SetLoadMode(base, config->loadValueMode);
    PDT_SetTriggerSource(base, config->triggerInput);
    PDT_SetMultForPrescaler(base, config->clkPreMultFactor);
    PDT_SetClockPrescaler(base, config->clkPreDiv);

    PDT_SetCountMode(base, config->continuousModeEnable);
    PDT_SetInterruptEnable(base, config->intEnable);
}

/* =============================================  EOF  ============================================== */
