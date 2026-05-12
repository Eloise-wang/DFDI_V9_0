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
 * @file pwm_irq.c
 *
 * @brief This file provides pwm irq integration functions.
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
 * @brief PWM0 overflow interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM0_Overflow_IRQHandler(void)
{
    PWM_DRV_Overflow_IRQHandler(0U);
}

/*!
 * @brief PWM1 overflow interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM1_Overflow_IRQHandler(void)
{
    PWM_DRV_Overflow_IRQHandler(1U);
}

/*!
 * @brief PWM2 overflow interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM2_Overflow_IRQHandler(void)
{
    PWM_DRV_Overflow_IRQHandler(2U);
}

/*!
 * @brief PWM3 overflow interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM3_Overflow_IRQHandler(void)
{
    PWM_DRV_Overflow_IRQHandler(3U);
}

/*!
 * @brief PWM4 overflow interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM4_Overflow_IRQHandler(void)
{
    PWM_DRV_Overflow_IRQHandler(4U);
}

/*!
 * @brief PWM5 overflow interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM5_Overflow_IRQHandler(void)
{
    PWM_DRV_Overflow_IRQHandler(5U);
}

/*!
 * @brief PWM0 channel interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM0_Channel_IRQHandler(void)
{
    PWM_DRV_Channel_IRQHandler(0U);
}

/*!
 * @brief PWM1 channel interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM1_Channel_IRQHandler(void)
{
    PWM_DRV_Channel_IRQHandler(1U);
}

/*!
 * @brief PWM2 channel interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM2_Channel_IRQHandler(void)
{
    PWM_DRV_Channel_IRQHandler(2U);
}

/*!
 * @brief PWM3 channel interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM3_Channel_IRQHandler(void)
{
    PWM_DRV_Channel_IRQHandler(3U);
}

/*!
 * @brief PWM4 channel interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM4_Channel_IRQHandler(void)
{
    PWM_DRV_Channel_IRQHandler(4U);
}

/*!
 * @brief PWM5 channel interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM5_Channel_IRQHandler(void)
{
    PWM_DRV_Channel_IRQHandler(5U);
}

/*!
 * @brief PWM0 fault interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM0_Fault_IRQHandler(void)
{
    PWM_DRV_Fault_IRQHandler(0U);
}

/*!
 * @brief PWM1 fault interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM1_Fault_IRQHandler(void)
{
    PWM_DRV_Fault_IRQHandler(1U);
}

/*!
 * @brief PWM2 fault interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM2_Fault_IRQHandler(void)
{
    PWM_DRV_Fault_IRQHandler(2U);
}

/*!
 * @brief PWM3 fault interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM3_Fault_IRQHandler(void)
{
    PWM_DRV_Fault_IRQHandler(3U);
}

/*!
 * @brief PWM4 fault interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM4_Fault_IRQHandler(void)
{
    PWM_DRV_Fault_IRQHandler(4U);
}

/*!
 * @brief PWM5 fault interrupt request handler.
 *
 * @param[in] none
 * @return none
 */
void PWM5_Fault_IRQHandler(void)
{
    PWM_DRV_Fault_IRQHandler(5U);
}

/* =============================================  EOF  ============================================== */
