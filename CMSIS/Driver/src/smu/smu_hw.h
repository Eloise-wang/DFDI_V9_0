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
 * @file smu_hw.h
 *
 * @brief smu access register inline function definition.
 *
 */

#ifndef _SMU_HW_H
#define _SMU_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "smu_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Init SMU module.
 *
 * @param[in] none
 *
 * @return none
 */
void SMU_Init(void);

/*!
 * @brief Single point fault register access permission unlock.
 *
 * @param[in] none
 * @return none
 */
void SMU_SPFaultRegUnlock(void);

/*!
 * @brief Latent fault register access permission unlock.
 *
 * @param[in] none
 * @return none
 */
void SMU_LatentFaultRegUnlock(void);

/*!
 * @brief Enable single point fault channel reset function.
 *
 * @param[in] channel: single fault channel
 * @param[in] enable: enable or disble single fault channel reset function
 * @return none
 */
void SMU_SPFaultResetEnable(single_fault_channel_t channel, bool enable);

/*!
 * @brief  enable latent fault channel reset function.
 *
 * @param[in] channel: latent fault channel
 * @param[in] enable: enable or disble latent fault channel reset function
 * @return none
 */
void SMU_LatentFaultResetEnable(latent_fault_channel_t channel, bool enable);
/*!
 * @brief Enable software single point fault channel function.
 *
 * @param[in] channel: software single point fault channel
 * @param[in] enable: enable or disble software single point fault channel function
 * @return none
 */
void SMU_SetSoftwareSPFaultEn(single_fault_channel_t channel, bool enable);
/*!
 * @brief Enable software latent fault channel function.
 *
 * @param[in] channel: software latent fault channel
 * @param[in] enable: enable or disble software latent fault channel function
 * @return none
 */
void SMU_SetSoftwareLatentFaultEn(latent_fault_channel_t channel, bool enable);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set smu reset counter value.
 *
 * @param[in] val: reset counter value which can be 0~15
 * @return none
 */
static inline void SMU_SetResetCounterVal(uint8_t val)
{
    SMU->SRSTCNTVAL = val;
}

/*!
 * @brief Get smu single point fault status.
 *
 * @param[in] none
 * @return smu single point fault status
 */
static inline uint32_t SMU_GetSPFaultStatus(void)
{
    return SMU->SFESS;
}

/*!
 * @brief Clear smu single point fault status.
 *
 * @param[in] none
 * @return none
 */
static inline void SMU_ClearSPFaultStatus(void)
{
    /* single point fault status clear */
    SMU->SFES = 0xFFFFFFFF;
}

/*!
 * @brief Get smu latent fault status.
 *
 * @param[in] none
 * @return smu latent fault status
 */
static inline uint32_t SMU_GetLatentFaultStatus(void)
{
    return SMU->LFESS;
}

/*!
 * @brief Clear smu latent fault status.
 *
 * @param[in] none
 * @return none
 */
static inline void SMU_ClearLatentFaultStatus(void)
{
    /* latent point fault status clear */
    SMU->LFES = 0xFFFFFFFF;
}

/*!
 * @brief Get smu reset counter shadow status.
 *
 * @param[in] none
 * @return smu reset counter shadow
 */
static inline uint32_t SMU_GetResetCounterShadow(void)
{
    return SMU->SRSTCNTS;
}

/*!
 * @brief Clear smu reset counter shadow
 *
 * @param[in] none
 * @return none
 */
static inline void SMU_ClearResetCounterVal(void)
{
    /* Clear smu reset counter shadow */
    SMU->SRSTCNT = 0xFFFFFFFF;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SMU_HW_H */

/* =============================================  EOF  ============================================== */
