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
 * @file rcm_hw.h
 *
 * @brief rcm access register inline function definition.
 *
 */

#ifndef _RCM_HW_H
#define _RCM_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "rcm_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Global reset interrupt enable.
 *
 * @param[in] enable
 *            - true: enable global reset interrupt
 *            - false: disable global reset interrupt
 * @return none
 */
static inline void RCM_EnableGlobalResetInterrupt(bool enable)
{
    MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_RST_GLB_INT_EN_Msk, CKGEN_RCM_CTRL_RST_GLB_INT_EN_Pos, (uint32_t)enable);
}

/*!
 * @brief Set reset source trigger interrupt.
 *
 * @param[in] rstSource, reset source which can trigger RCM interrupt. value can be
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
 * @return none
 */
static inline void RCM_EnableResetInterruptSource(rcm_reset_source_t rstSource, bool enable)
{
    switch (rstSource)
    {
    case RCM_RESET_SOURCE_SW:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_SW_RST_INT_EN_Msk, CKGEN_RCM_CTRL_SW_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_LOCKUP:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_LOCKUP_RST_INT_EN_Msk, CKGEN_RCM_CTRL_LOCKUP_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_ACK_ERR:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_ACK_ERR_RST_INT_EN_Msk, CKGEN_RCM_CTRL_ACK_ERR_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_WDG:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_WDG_RST_INT_EN_Msk, CKGEN_RCM_CTRL_WDG_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_XOSC_LOSS:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_XOSC_LOSS_RST_INT_EN_Msk, CKGEN_RCM_CTRL_XOSC_LOSS_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_PLL_UNLOCK:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_PLL_UNLOCK_RST_INT_EN_Msk, CKGEN_RCM_CTRL_PLL_UNLOCK_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_VHSI_LOSS:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_VHSI_LOSS_RST_INT_EN_Msk, CKGEN_RCM_CTRL_VHSI_LOSS_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_SMU_ERR:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_SMU_ERR_RST_INT_EN_Msk, CKGEN_RCM_CTRL_SMU_ERR_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_ECC2_ERR:
        MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_ECC2_ERR_RST_INT_EN_Msk, CKGEN_RCM_CTRL_ECC2_ERR_RST_INT_EN_Pos, (uint32_t)enable);
        break;

    default:
        break;
    }
}

/*!
 * @brief Set reset delay time.
 *
 * @param[in] time: system reset delay time set, value can be
 *                   - RCM_RESET_DELAY_8_128KHz(8 * 128KHz)
 *                   - RCM_RESET_DELAY_32_128KHz(32 * 128KHz)
 *                   - RCM_RESET_DELAY_128_128KHz(128 * 128KHz)
 *                   - RCM_RESET_DELAY_512_128KHz(512 * 128KHz)
 * @return none
 */
static inline void RCM_SetResetDelayTime(rcm_reset_delay_time_t time)
{
    MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_RST_DLY_TIME_Msk, CKGEN_RCM_CTRL_RST_DLY_TIME_Pos, (uint32_t)time);
}

/*!
 * @brief Set ext reset filter value.
 *
 * @param[in] filterVal: filter value: 0~127, means filter width value is N * 128KHz.
 * @return none
 */
static inline void RCM_SetExtResetFilter(uint8_t filterVal)
{
    MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_EXT_RST_FILTER_VAL_Msk, CKGEN_RCM_CTRL_EXT_RST_FILTER_VAL_Pos, (uint32_t)filterVal);
}

/*!
 * @brief Enable or disable external reset filter.
 *
 * @param[in] enable: enable or disable external reset filter
 *            - true: enable ext reset filter
 *            - false: disable ext reset filter
 * @return none
 */
static inline void RCM_EnableExtResetFilter(bool enable)
{
    MODIFY_REG32(CKGEN->RCM_CTRL, CKGEN_RCM_CTRL_EXT_RST_FILTER_EN_Msk, CKGEN_RCM_CTRL_EXT_RST_FILTER_EN_Pos, (uint32_t)enable);
}

/*!
 * @brief Set reset source trigger reset.
 *
 * @param[in] rstSource, reset source which can trigger reset, value can be
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
 *            - true: enable trigger interrupt
 *            - false: disable trigger interrupt
 * @return none
 */
static inline void RCM_EnableResetSource(rcm_reset_source_t rstSource, bool enable)
{
    switch (rstSource)
    {
    case RCM_RESET_SOURCE_SW:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_SW_RST_EN_Msk, CKGEN_RCM_EN_SW_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_LOCKUP:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_LOCKUP_RST_EN_Msk, CKGEN_RCM_EN_LOCKUP_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_ACK_ERR:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_ACK_ERR_RST_EN_Msk, CKGEN_RCM_EN_ACK_ERR_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_WDG:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_WDG_RST_EN_Msk, CKGEN_RCM_EN_WDG_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_XOSC_LOSS:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_XOSC_LOSS_RST_EN_Msk, CKGEN_RCM_EN_XOSC_LOSS_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_PLL_UNLOCK:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_PLL_UNLOCK_RST_EN_Msk, CKGEN_RCM_EN_PLL_UNLOCK_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_VHSI_LOSS:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_VHSI_LOSS_RST_EN_Msk, CKGEN_RCM_EN_VHSI_LOSS_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_EXT_RST:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_EXT_RST_EN_Msk, CKGEN_RCM_EN_EXT_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_SMU_ERR:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_SMU_ERR_RST_EN_Msk, CKGEN_RCM_EN_SMU_ERR_RST_EN_Pos, (uint32_t)enable);
        break;

    case RCM_RESET_SOURCE_ECC2_ERR:
        MODIFY_REG32(CKGEN->RCM_EN, CKGEN_RCM_EN_ECC2_ERR_RST_EN_Msk, CKGEN_RCM_EN_ECC2_ERR_RST_EN_Pos, (uint32_t)enable);
        break;

    default:
        break;
    }
}

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
static inline uint32_t RCM_GetResetStatus(void)
{
    return CKGEN->RCM_STATUS;
}

/*!
 * @brief Clear all reset interrupt status.
 *
 * @param[in] none
 * @return none
 */
static inline void RCM_ClearResetInterruptStatus(void)
{
    CKGEN->RCM_STATUS |= CKGEN_RCM_STATUS_RST_INT_STATUS_CLR_Msk;
}

/*!
 * @brief Clear all reset status.
 *
 * @param[in] none
 * @return none
 */
static inline void RCM_ClearResetStatus(void)
{
    CKGEN->RCM_STATUS |= CKGEN_RCM_STATUS_RST_STATUS_CLR_Msk;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RCM_HW_H */

/* =============================================  EOF  ============================================== */
