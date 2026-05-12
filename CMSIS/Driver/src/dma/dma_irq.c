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
 * @file dma_irq.c
 *
 * @brief This file provides dma interrupts handler functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
extern void DMA_DRV_IRQHandler(uint8_t virtualChannel);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief DMA0_Channel0 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel0_IRQHandler(void)
{
    DMA_DRV_IRQHandler(0U);
}

/*!
 * @brief DMA0_Channel0 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel1_IRQHandler(void)
{
    DMA_DRV_IRQHandler(1U);
}

/*!
 * @brief DMA0_Channel2 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel2_IRQHandler(void)
{
    DMA_DRV_IRQHandler(2U);
}

/*!
 * @brief DMA0_Channel3 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel3_IRQHandler(void)
{
    DMA_DRV_IRQHandler(3U);
}

/*!
 * @brief DMA0_Channel4 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel4_IRQHandler(void)
{
    DMA_DRV_IRQHandler(4U);
}

/*!
 * @brief DMA0_Channel5 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel5_IRQHandler(void)
{
    DMA_DRV_IRQHandler(5U);
}

/*!
 * @brief DMA0_Channel6 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel6_IRQHandler(void)
{
    DMA_DRV_IRQHandler(6U);
}

/*!
 * @brief DMA0_Channel7 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel7_IRQHandler(void)
{
    DMA_DRV_IRQHandler(7U);
}

/*!
 * @brief DMA0_Channel8 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel8_IRQHandler(void)
{
    DMA_DRV_IRQHandler(8U);
}

/*!
 * @brief DMA0_Channel9 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel9_IRQHandler(void)
{
    DMA_DRV_IRQHandler(9U);
}

/*!
 * @brief DMA0_Channel10 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel10_IRQHandler(void)
{
    DMA_DRV_IRQHandler(10U);
}

/*!
 * @brief DMA0_Channel11 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel11_IRQHandler(void)
{
    DMA_DRV_IRQHandler(11U);
}

/*!
 * @brief DMA0_Channel12 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel12_IRQHandler(void)
{
    DMA_DRV_IRQHandler(12U);
}

/*!
 * @brief DMA0_Channel13 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel13_IRQHandler(void)
{
    DMA_DRV_IRQHandler(13U);
}

/*!
 * @brief DMA0_Channel14 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel14_IRQHandler(void)
{
    DMA_DRV_IRQHandler(14U);
}

/*!
 * @brief DMA0_Channel15 Interrupt Handler Function
 *
 * @param[in] none
 * @return none
 */
void DMA0_Channel15_IRQHandler(void)
{
    DMA_DRV_IRQHandler(15U);
}

/* =============================================  EOF  ============================================== */
