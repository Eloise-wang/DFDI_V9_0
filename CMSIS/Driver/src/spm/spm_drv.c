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
 * @file spm_drv.c
 *
 * @brief This file provides spm module integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "spm_hw.h"

/* ============================================  Define  ============================================ */
#define SPM_MODE_SWITCH_TIMEOUT_VALUE   (1000UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* for SPM Event call back function */
spm_callback_t g_spmCallback = NULL;
/* for LVD Event call back function */
spm_callback_t g_lvdCallback = NULL;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set the power mode.
 *
 * @param[in] power mode
 *            - POWER_MODE_RUN
 *            - POWER_MODE_VLPR
 *            - POWER_MODE_STOP1
 *            - POWER_MODE_STOP2
 *            - POWER_MODE_VLPS
 * @return operation status
 *         - STATUS_SUCCESS: mode switch success
 *         - STATUS_ERROR: mode switch error
 */
status_t SPM_DRV_SetPowerMode(spm_power_mode_t mode)
{
    DEVICE_ASSERT((POWER_MODE_RUN == mode)   ||
                  (POWER_MODE_VLPR == mode)  ||
                  (POWER_MODE_STOP1 == mode) ||
                  (POWER_MODE_STOP2 == mode) ||
                  (POWER_MODE_VLPS == mode));

    uint32_t timeout, i;
    spm_power_mode_t curMode;
    status_t ret = STATUS_SUCCESS;

    SPM_SetPowerMode(mode);

    if ((POWER_MODE_RUN == mode) || (POWER_MODE_VLPR == mode)) /* run mode */
    {
        timeout = SPM_MODE_SWITCH_TIMEOUT_VALUE;
        do
        {
            curMode = SPM_GetPowerMode();
        } while ((curMode != mode) && (--timeout != 0U));

        if (0U == timeout)
        {
            ret = STATUS_ERROR;
        }
    }
    else /* stop mode */
    {
        /* Set the SLEEPDEEP bit to enable deep sleep mode (STOP) */
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#if defined (__GNUC__)
        __asm volatile("wfi");
#else
        __asm("wfi");
#endif
    }
    if ((POWER_MODE_VLPR != mode) && SPM_GetSPLLStatus())
    {
        MODIFY_REG32(CKGEN->CLK_DIV2, CKGEN_CLK_DIV2_SPLL_DIV2_Msk, CKGEN_CLK_DIV2_SPLL_DIV2_Pos, 0U);
        for (i = 0U; i < 100U; i++)
        {
            __asm("nop");
        }
        MODIFY_REG32(CKGEN->CLK_DIV2, CKGEN_CLK_DIV2_SPLL_DIV2_Msk, CKGEN_CLK_DIV2_SPLL_DIV2_Pos, 1U);
    }

    return ret;
}

/*!
 * @brief Get the current power mode.
 *
 * @param[in] none
 * @return the current power mode, value can be
 *         - POWER_MODE_RUN
 *         - POWER_MODE_VLPR
 *         - POWER_MODE_STOP1
 *         - POWER_MODE_STOP2
 *         - POWER_MODE_VLPS
 */
spm_power_mode_t SPM_DRV_GetPowerMode(void)
{
    return SPM_GetPowerMode();
}

/*!
 * @brief Get the power control mode.
 *
 * @param[in] none
 * @return the power control mode, value can be
 *         - POWER_MODE_RUN
 *         - POWER_MODE_VLPR
 *         - POWER_MODE_STOP1
 *         - POWER_MODE_STOP2
 *         - POWER_MODE_VLPS
 */
spm_power_mode_t SPM_DRV_GetPowerControlMode(void)
{
    return SPM_GetPowerControlMode();
}

/*!
 * @brief Select lvd threshold.
 *
 * @param[in] threshold value
 *            - LVD_LOW_THRESHOLD
 *            - LVD_HIGH_THRESHOLD
 * @return none
 */
void SPM_DRV_SelectLVDThreshold(spm_lvd_threshold_t threshold)
{
    DEVICE_ASSERT((LVD_LOW_THRESHOLD == threshold) || (LVD_HIGH_THRESHOLD == threshold));

    SPM_SelectLVDThreshold(threshold);
}

/*!
 * @brief Select lvr threshold, it is recommended to use low gear only in VLPS mode.
 *
 * @param[in] threshold value
 *            - LVR_HIGH_THRESHOLD
 *            - LVR_LOW_THRESHOLD
 * @return none
 */
void SPM_DRV_SelectLVRThreshold(spm_lvr_threshold_t threshold)
{
    DEVICE_ASSERT((LVR_LOW_THRESHOLD == threshold) || (LVR_HIGH_THRESHOLD == threshold));

    SPM_SelectLVRThreshold(threshold);
}

/*!
 * @brief Set SPM IRQHandler callback function.
 *
 * @param[in] eventFunc: the pointer of the call back function, which will be called in SPM_IRQHandler
 * @return none
 */
void SPM_DRV_InstallCallback(const spm_callback_t eventFunc)
{
    g_spmCallback = eventFunc;
}

/*!
 * @brief Set LVD Handler callback function.
 *
 * @param[in] eventFunc: the pointer of the call back function, which will be called in LVD_IRQHandler
 * @return none
 */
void LVD_DRV_InstallCallback(const spm_callback_t eventFunc)
{
    g_lvdCallback = eventFunc;
}

/* =============================================  EOF  ============================================== */
