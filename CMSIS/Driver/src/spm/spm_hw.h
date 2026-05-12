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
 * @file spm_hw.h
 *
 * @brief This file provides spm hardware integration functions interface.
 *
 */

#ifndef _SPM_HW_H
#define _SPM_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "spm_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set the power mode.
 *
 * @param[in] power mode, value can be
 *            - POWER_MODE_RUN
 *            - POWER_MODE_VLPR
 *            - POWER_MODE_STOP1
 *            - POWER_MODE_STOP2
 *            - POWER_MODE_VLPS
 * @return none
 */
static inline void SPM_SetPowerMode(spm_power_mode_t mode)
{
    MODIFY_REG32(SPM->PWR_MGR_CFG0, SPM_PWR_MGR_CFG0_POWER_MODE_Msk, SPM_PWR_MGR_CFG0_POWER_MODE_Pos, (uint32_t)mode);
    /* enable AWIC */
    SET_BIT32(SPM->PWR_MGR_CFG0, SPM_PWR_MGR_CFG0_AWIC_EN_Msk);
}

/*!
 * @brief Get the current power mode.
 *
 * @param[in] none
 * @return the current power mode,value can be
 *         - POWER_MODE_RUN
 *         - POWER_MODE_VLPR
 *         - POWER_MODE_STOP1
 *         - POWER_MODE_STOP2
 *         - POWER_MODE_VLPS
 */
static inline spm_power_mode_t SPM_GetPowerMode(void)
{
    return (spm_power_mode_t)(SPM->STATUS & SPM_STATUS_CURR_POWER_MODE_Msk);
}

/*!
 * @brief Get the power control mode.
 *
 * @param[in] none
 * @return the power control mode,value can be
 *         - POWER_MODE_RUN
 *         - POWER_MODE_VLPR
 *         - POWER_MODE_STOP1
 *         - POWER_MODE_STOP2
 *         - POWER_MODE_VLPS
 */
static inline spm_power_mode_t SPM_GetPowerControlMode(void)
{
    return (spm_power_mode_t)((SPM->PWR_MGR_CFG0 & SPM_PWR_MGR_CFG0_POWER_MODE_Msk) >> SPM_PWR_MGR_CFG0_POWER_MODE_Pos);
}

/*!
 * @brief Enable XOSC bypass mode.
 *
 * @param[in] enable State
 *            - true: enable XOSC bypass
 *            - false: disable XOSC bypass
 * @return none
 */
static inline void SPM_EnableXOSCBypassMode(bool enable)
{
    MODIFY_REG32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_HSEBYP_Msk, SPM_PWR_MGR_CFG1_XOSC_HSEBYP_Pos, (uint32_t)enable);
}

/*!
 * @brief Enable HSI.
 *
 * @param[in] enable State
 *            - true: enable HSI
 *            - false: disable HSI
 * @return none
 */
static inline void SPM_EnableHSI(bool enable)
{
    MODIFY_REG32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_HSI_EN_NORMAL_Msk, SPM_PWR_MGR_CFG1_HSI_EN_NORMAL_Pos, (uint32_t)enable);
}

/*!
 * @brief Enable HSI in VLPS.
 *
 * @param[in] enable State
 *            - true: enable HSI in VLPS
 *            - false: disable HSI in VLPS
 * @return none
 */
static inline void SPM_EnableHSIInVLPS(bool enable)
{
    MODIFY_REG32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_HSI_EN_VLPS_Msk, SPM_PWR_MGR_CFG1_HSI_EN_VLPS_Pos, (uint32_t)enable);
}

/*!
 * @brief Gets HSI status.
 *
 * @param[in] none
 * @return The HSI status
 *         - true : HSI valid
 *         - false: HSI invalid
 */
static inline bool SPM_GetHSIStatus(void)
{
    return (((SPM->PWR_MGR_CFG1 & SPM_PWR_MGR_CFG1_HSI_RDY_Msk) >> SPM_PWR_MGR_CFG1_HSI_RDY_Pos) != 0U);
}

/*!
 * @brief Enable VHSI.
 *
 * @param[in] enable State
 *            - true: enable VHSI
 *            - false: disable VHSI
 * @return none
 */
static inline void SPM_EnableVHSI(bool enable)
{
    MODIFY_REG32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_VHSI_EN_Msk, SPM_PWR_MGR_CFG1_VHSI_EN_Pos, (uint32_t)enable);
}

/*!
 * @brief Gets VHSI status.
 *
 * @param[in] none
 * @return The VHSI status
 *         - true : VHSI valid
 *         - false: VHSI invalid
 */
static inline bool SPM_GetVHSIStatus(void)
{
    return (((SPM->PWR_MGR_CFG1 & SPM_PWR_MGR_CFG1_VHSI_RDY_Msk) >> SPM_PWR_MGR_CFG1_VHSI_RDY_Pos) != 0U);
}

/*!
 * @brief Enable XOSC.
 *
 * @param[in] enable State
 *            - true: enable XOSC
 *            - false: disable XOSC
 * @return none
 */
static inline void SPM_EnableXOSC(bool enable)
{
    MODIFY_REG32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_HSEEN_Msk, SPM_PWR_MGR_CFG1_XOSC_HSEEN_Pos, (uint32_t)enable);
}

/*!
 * @brief Gets XOSC status.
 *
 * @param[in] none
 * @return The XOSC status
 *         - true : XOSC valid
 *         - false: XOSC invalid
 */
static inline bool SPM_GetXOSCStatus(void)
{
    return (((SPM->PWR_MGR_CFG1 & SPM_PWR_MGR_CFG1_XOSC_RDY_Msk) >> SPM_PWR_MGR_CFG1_XOSC_RDY_Pos) != 0U);
}

/*!
 * @brief Enable SPLL.
 *
 * @param[in] enable State
 *            - true: enable SPLL
 *            - false: disable SPLL
 * @return none
 */
static inline void SPM_EnableSPLL(bool enable)
{
    MODIFY_REG32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_SPLL_EN_Msk, SPM_PWR_MGR_CFG1_SPLL_EN_Pos, (uint32_t)enable);
}

/*!
 * @brief Gets SPLL status.
 *
 * @param[in] none
 * @return The SPLL status
 *         - true : SPLL valid
 *         - false: SPLL invalid
 */
static inline bool SPM_GetSPLLStatus(void)
{
    return (((SPM->PWR_MGR_CFG1 & SPM_PWR_MGR_CFG1_SPLL_RDY_Msk) >> SPM_PWR_MGR_CFG1_SPLL_RDY_Pos) != 0U);
}

/*!
 * @brief Get sleep ack status.
 *
 * @param[in] none
 * @return sleep ack status value
 *         - BIT0:SLEEP_ACK_I2C0
 *         - BIT2:SLEEP_ACK_SPI0
 *         - BIT3:SLEEP_ACK_SPI1
 *         - BIT4:SLEEP_ACK_SPI2
 *         - BIT5:SLEEP_ACK_CAN0
 *         - BIT6:SLEEP_ACK_CAN1
 *         - BIT7:SLEEP_ACK_CAN2
 *         - BIT8:SLEEP_ACK_CAN3
 *         - BIT11:SLEEP_ACK_UART0
 *         - BIT12:SLEEP_ACK_UART1
 *         - BIT13:SLEEP_ACK_UART2
 *         - BIT14:SLEEP_ACK_UART3
 *         - BIT17:SLEEP_ACK_DMA0
 *         - BIT18:SLEEP_ACK_EIO
 *         - BIT19:SLEEP_ACK_FLASH
 */
static inline uint32_t SPM_GetModuleSleepACKStatus(void)
{
    return (SPM->PERIPH_SLEEP_ACK_STATUS);
}

/*!
 * @brief Select reset or interrupt after ack time out.
 *
 * @param[in] action after ack time out
 *            - ACK_TIMEOUT_ACTION_INTERRUPT
 *            - ACK_TIMEOUT_ACTION_RESET
 * @return none
 */
static inline void SPM_SelectACKTimeOutAction(spm_ack_timeout_action_t action)
{
    if (ACK_TIMEOUT_ACTION_INTERRUPT == action)
    {
        CLEAR_BIT32(SPM->PWR_MGR_CFG0, SPM_PWR_MGR_CFG0_ACK_TIMEOUT_ACTION_Msk);
    }
    else
    {
        SET_BIT32(SPM->PWR_MGR_CFG0, SPM_PWR_MGR_CFG0_ACK_TIMEOUT_ACTION_Msk);
    }
}

/*!
 * @brief Clear ack timeout flag.
 *
 * @param[in] none
 * @return none
 */
static inline void SPM_ClearACKTimeOutFlag(void)
{
    SET_BIT32(SPM->STATUS, SPM_STATUS_ACK_TIMEOUT_FLAG_Msk);
}

/*!
 * @brief Select lvd threshold.
 *
 * @param[in] threshold value
 *            - LVD_LOW_THRESHOLD
 *            - LVD_HIGH_THRESHOLD
 * @return none
 */
static inline void SPM_SelectLVDThreshold(spm_lvd_threshold_t threshold)
{
    if (LVD_LOW_THRESHOLD == threshold)
    {
        CLEAR_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_LVD_THRESHOLD_Msk);
    }
    else
    {
        SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_LVD_THRESHOLD_Msk);
    }
}

/*!
 * @brief Select lvr threshold.
 *
 * @param[in] threshold value
 *            - LVR_LOW_THRESHOLD
 *            - LVR_HIGH_THRESHOLD
 * @return none
 */
static inline void SPM_SelectLVRThreshold(spm_lvr_threshold_t threshold)
{
    if (LVR_HIGH_THRESHOLD == threshold)
    {
        CLEAR_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_LVR_THRESHOLD_Msk);
    }
    else
    {
        SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_LVR_THRESHOLD_Msk);
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SPM_HW_H */

/* =============================================  EOF  ============================================== */
