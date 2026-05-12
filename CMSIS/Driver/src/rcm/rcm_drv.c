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
 * @file rcm_drv.c
 *
 * @brief This file provides rcm module integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "rcm_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
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
void RCM_DRV_SetResetInterruptSource(rcm_reset_source_t rstSource, bool enable, rcm_reset_delay_time_t time)
{
    DEVICE_ASSERT(rstSource <= RCM_RESET_SOURCE_ECC2_ERR);
    DEVICE_ASSERT(time <= RCM_RESET_DELAY_512_128KHz);

    /* reset interrupt global enable set */
    RCM_EnableGlobalResetInterrupt(true);
    RCM_EnableResetInterruptSource(rstSource, enable);
    RCM_SetResetDelayTime(time);
}

/*!
 * @brief Set ext reset filter value.
 *
 * @param[in] filterVal: filter value
 *            - 0: disable ext reset filter function
 *            - 1~127: filter width value is N * 128KHz.
 * @return none
 */
void RCM_DRV_SetExtResetFilter(uint8_t filterVal)
{
    if (filterVal)
    {
        if (filterVal >= RESET_FILTER_MAX_VALUE) /* 127 cycle * 128K, the register must set be 0 */
        {
            filterVal = 0;
        }
        else
        {
            filterVal++;  /* n cycle * 128K, the register must set be n+1. ( n != 127) */
        }
        /* enable ext reset filter */
        RCM_EnableExtResetFilter(true);
        /* set ext reset filter value */
        RCM_SetExtResetFilter((uint32_t)filterVal);
    }
    else
    {
        /* disable ext reset filter */
        RCM_EnableExtResetFilter(false);
    }
}

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
void RCM_DRV_SetResetSource(rcm_reset_source_t rstSource, bool enable)
{
    DEVICE_ASSERT(rstSource <= RCM_RESET_SOURCE_ECC2_ERR);

    RCM_EnableResetSource(rstSource, enable);
}

/*!
 * @brief Get reset and interrupt status.
 *
 * @param[in] none
 * @return value of reset status
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
uint32_t RCM_DRV_GetResetStatus(void)
{
    return RCM_GetResetStatus();
}

/*!
 * @brief Clear all reset interrupt status.
 *
 * @param[in] none
 * @return none
 */
void RCM_DRV_ClearResetInterruptStatus(void)
{
    RCM_ClearResetInterruptStatus();
}

/*!
 * @brief Clear all reset status.
 *
 * @param[in] none
 * @return none
 */
void RCM_DRV_ClearResetStatus(void)
{
    RCM_ClearResetStatus();
}

/* =============================================  EOF  ============================================== */
