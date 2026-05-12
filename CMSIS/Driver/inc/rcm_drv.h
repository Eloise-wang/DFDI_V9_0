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
 * @file rcm_drv.h
 *
 * @brief This file provides rcm module integration functions interfaces.
 *
 */

#ifndef _RCM_DRV_H
#define _RCM_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
#define RESET_FILTER_MAX_VALUE    (127UL)

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Reset source or interrupt enumeration.
 */
typedef enum
{
    RCM_RESET_SOURCE_SW = 0U,      /*!< Reset or interrupt source select: software reset     */
    RCM_RESET_SOURCE_LOCKUP,       /*!< Reset or interrupt source select: lockup reset       */
    RCM_RESET_SOURCE_ACK_ERR,      /*!< Reset or interrupt source select: sleep ack error    */
    RCM_RESET_SOURCE_WDG,          /*!< Reset or interrupt source select: wdg reset          */
    RCM_RESET_SOURCE_XOSC_LOSS,    /*!< Reset or interrupt source select: xosc loss          */
    RCM_RESET_SOURCE_PLL_UNLOCK,   /*!< Reset or interrupt source select: PLL unlock         */
    RCM_RESET_SOURCE_VHSI_LOSS,    /*!< Reset or interrupt source select: VHSI loss          */
    RCM_RESET_SOURCE_EXT_RST,      /*!< Reset or interrupt source select: external reset pin */
    RCM_RESET_SOURCE_SMU_ERR,      /*!< Reset or interrupt source select: SMU error          */
    RCM_RESET_SOURCE_ECC2_ERR      /*!< Reset or interrupt source select: ECC 2bits error    */
} rcm_reset_source_t;

/*!
 * @brief Reset delay time enumeration.
 */
typedef enum
{
    RCM_RESET_DELAY_8_128KHz = 0U,     /*!< Reset delay 8   cycles * 128K     */
    RCM_RESET_DELAY_32_128KHz,         /*!< Reset delay 32  cycles * 128K     */
    RCM_RESET_DELAY_128_128KHz,        /*!< Reset delay 128 cycles * 128K     */
    RCM_RESET_DELAY_512_128KHz         /*!< Reset delay 512 cycles * 128K     */
} rcm_reset_delay_time_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Set reset source trigger interrupt.
 *
 * @param[in] rstSource
 *            - RCM_RESET_SOURCE_SW
 *            - RCM_RESET_SOURCE_LOCKUP
 *            - RCM_RESET_SOURCE_ACK_ERR
 *            - RCM_RESET_SOURCE_WDG
 *            - RCM_RESET_SOURCE_XOSC_LOSS
 *            - RCM_RESET_SOURCE_PLL_UNLOCK
 *            - RCM_RESET_SOURCE_VHSI_LOSS
 *            - RCM_RESET_SOURCE_SMU_ERR
 *            - RCM_RESET_SOURCE_ECC2_ERR
 * @param[in] enable
 *            - true: enable trigger interrupt
 *            - false: disable trigger interrupt
 * @param[in] time
 *            - RCM_RESET_DELAY_8_128KHz
 *            - RCM_RESET_DELAY_32_128KHz
 *            - RCM_RESET_DELAY_128_128KHz
 *            - RCM_RESET_DELAY_512_128KHz
 * @return none
 */
void RCM_DRV_SetResetInterruptSource(rcm_reset_source_t rstSource, bool enable, rcm_reset_delay_time_t time);

/*!
 * @brief Set ext reset filter value.
 *
 * @param[in] filterVal: filter value
 *            - 0: disable ext reset filter function
 *            - 1~127: filter width value is N * 128KHz.
 * @return none
 */
void RCM_DRV_SetExtResetFilter(uint8_t filterVal);

/*!
 * @brief Set reset source trigger reset.
 *
 * @param[in] rstSource
 *            - RCM_RESET_SOURCE_SW
 *            - RCM_RESET_SOURCE_LOCKUP
 *            - RCM_RESET_SOURCE_ACK_ERR
 *            - RCM_RESET_SOURCE_WDG
 *            - RCM_RESET_SOURCE_XOSC_LOSS
 *            - RCM_RESET_SOURCE_PLL_UNLOCK
 *            - RCM_RESET_SOURCE_VHSI_LOSS
 *            - RCM_RESET_SOURCE_EXT_RST
 *            - RCM_RESET_SOURCE_SMU_ERR
 *            - RCM_RESET_SOURCE_ECC2_ERR
 * @param[in] enable
 *            - true: enable trigger reset
 *            - false: disable trigger reset
 * @return none
 */
void RCM_DRV_SetResetSource(rcm_reset_source_t rstSource, bool enable);

/*!
 * @brief Get reset and interrupt status.
 *
 * @param[in] none
 * @return value of reset and interrupt status
 *         - BIT0:RCM_RESET_INT_STATUS_SW_RST
 *         - BIT1:RCM_RESET_INT_STATUS_LOCKUP_RST
 *         - BIT2:RCM_RESET_INT_STATUS_ACK_ERR_RST
 *         - BIT3:RCM_RESET_INT_STATUS_WDG_RST
 *         - BIT5:RCM_RESET_INT_STATUS_XOSC_LOSS_RST
 *         - BIT6:RCM_RESET_INT_STATUS_PLL_UNLOCK_RST
 *         - BIT7:RCM_RESET_INT_STATUS_VHSI_LOSS_RST
 *         - BIT9:RCM_RESET_INT_STATUS_SMU_ERR_RST
 *         - BIT10:RCM_RESET_INT_STATUS_ECC2_ERR_RST
 *         - BIT16:RCM_RESET_STATUS_POR_RST
 *         - BIT17:RCM_RESET_STATUS_LVR_RST
 *         - BIT18:RCM_RESET_STATUS_SW_RST
 *         - BIT19:RCM_RESET_STATUS_LOCKUP_RST
 *         - BIT20:RCM_RESET_STATUS_ACK_ERR_RST
 *         - BIT21:RCM_RESET_STATUS_WDG_RST
 *         - BIT23:RCM_RESET_STATUS_XOSC_LOSS_RST
 *         - BIT24:RCM_RESET_STATUS_PLL_UNLOCK_RST
 *         - BIT25:RCM_RESET_STATUS_VHSI_LOSS_RST
 *         - BIT26:RCM_RESET_STATUS_EXT_RST
 *         - BIT27:RCM_RESET_STATUS_SMU_ERR_RST
 *         - BIT28:RCM_RESET_STATUS_ECC2_ERR_RST
 */
uint32_t RCM_DRV_GetResetStatus(void);

/*!
 * @brief Clear all reset interrupt status.
 *
 * @param[in] none
 * @return none
 */
void RCM_DRV_ClearResetInterruptStatus(void);

/*!
 * @brief Clear all reset status.
 *
 * @param[in] none
 * @return none
 */
void RCM_DRV_ClearResetStatus(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RCM_DRV_H */

/* =============================================  EOF  ============================================== */
