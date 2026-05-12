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
 * @file pwm_hw.c
 *
 * @brief This file provides pwm hardware access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "pwm_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set pwm pair channel dead time.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] psc: prescaler divider
 *            - PWM_DEADTIME_DIVID_1
 *            - PWM_DEADTIME_DIVID_4
 *            - PWM_DEADTIME_DIVID_16
 * @param[in] value: inserted value
 *            - 0 ~ 1023
 * @return none
 */
void PWM_SetDeadtime(PWM_Type *base, uint8_t pairChannel,
                     pwm_dead_time_psc_type_t psc, uint16_t value)
{
    if (pairChannel < 2U)
    {
        MODIFY_REG32(base->DTSET0, PWM_DTSET0_DTPSC0_Msk << (pairChannel * PWM_DTSET_CONFIG_WIDTH),
                     PWM_DTSET0_DTPSC0_Pos + (pairChannel * PWM_DTSET_CONFIG_WIDTH), psc);
        MODIFY_REG32(base->DTSET0, PWM_DTSET0_DTVAL0_Msk << (pairChannel * PWM_DTSET_CONFIG_WIDTH),
                     PWM_DTSET0_DTVAL0_Pos + (pairChannel * PWM_DTSET_CONFIG_WIDTH), value);
    }
    else
    {
        pairChannel = pairChannel - 2U;
        MODIFY_REG32(base->DTSET1, PWM_DTSET1_DTPS2_Msk << (pairChannel * PWM_DTSET_CONFIG_WIDTH),
                     PWM_DTSET1_DTPS2_Pos + (pairChannel * PWM_DTSET_CONFIG_WIDTH), psc);
        MODIFY_REG32(base->DTSET1, PWM_DTSET1_DTVAL2_Msk << (pairChannel * PWM_DTSET_CONFIG_WIDTH),
                     PWM_DTSET1_DTVAL2_Pos + (pairChannel * PWM_DTSET_CONFIG_WIDTH), value);
    }
}

/* =============================================  EOF  ============================================== */
