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
 * @file smu_drv.h
 *
 * @brief This file provides smu integration functions interface.
 *
 */

#ifndef _SMU_DRV_H
#define _SMU_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief single point fault enumeration
 */
typedef enum
{
    SP_FAULT_CHANNEL0 = 0U,
    SP_FAULT_CHANNEL1,
    SP_FAULT_CHANNEL2,
    SP_FAULT_CHANNEL3,
    SP_FAULT_CHANNEL4,
    SP_FAULT_CHANNEL5,
    SP_FAULT_CHANNEL6,
    SP_FAULT_CHANNEL7,
    SP_FAULT_CHANNEL8,
    SP_FAULT_CHANNEL9,
    SP_FAULT_CHANNEL10,
    SP_FAULT_CHANNEL11,
    SP_FAULT_CHANNEL12,
    SP_FAULT_CHANNEL13,
    SP_FAULT_CHANNEL14,
    SP_FAULT_CHANNEL15,
    SP_FAULT_CHANNEL16,
    SP_FAULT_CHANNEL17,
    SP_FAULT_CHANNEL18
} single_fault_channel_t;

/*!
 * @brief latent fault enumeration
 */
typedef enum
{
    LATENT_FAULT_CHANNEL0 = 0U,
    LATENT_FAULT_CHANNEL1,
    LATENT_FAULT_CHANNEL2,
    LATENT_FAULT_CHANNEL3,
    LATENT_FAULT_CHANNEL4,
    LATENT_FAULT_CHANNEL5
} latent_fault_channel_t;

/*!
 * @brief software single point fault enumeration
 */
typedef enum
{
    SW_SP_FAULT_CHANNEL0 = 0U,
    SW_SP_FAULT_CHANNEL1,
    SW_SP_FAULT_CHANNEL2,
    SW_SP_FAULT_CHANNEL3,
    SW_SP_FAULT_CHANNEL4,
    SW_SP_FAULT_CHANNEL5,
    SW_SP_FAULT_CHANNEL6,
    SW_SP_FAULT_CHANNEL7
} software_single_fault_channel_t;

/*!
 * @brief software latent fault enumeration
 */
typedef enum
{
    SW_LATENT_FAULT_CHANNEL0 = 0U,
    SW_LATENT_FAULT_CHANNEL1,
    SW_LATENT_FAULT_CHANNEL2,
    SW_LATENT_FAULT_CHANNEL3,
    SW_LATENT_FAULT_CHANNEL4,
    SW_LATENT_FAULT_CHANNEL5,
    SW_LATENT_FAULT_CHANNEL6,
    SW_LATENT_FAULT_CHANNEL7
} software_latent_fault_channel_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Init SMU module.
 *
 * @param[in] none
 * @return none
 */
void SMU_DRV_Init(void);

/*!
 * @brief Single point fault register access permission unlock.
 *
 * @param[in] none
 * @return none
 */
void SMU_DRV_SPFaultRegUnlock(void);

/*!
 * @brief Latent fault register access permission unlock.
 *
 * @param[in] none
 * @return none
 */
void SMU_DRV_LatentFaultRegUnlock(void);

/*!
 * @brief Get smu single point fault status.
 *
 * @param[in] smu single point fault status
 * @return none
 */
uint32_t SMU_DRV_GetSPFaultStatus(void);

/*!
 * @brief Clear smu single point fault status.
 *
 * @param[in] none
 *
 * @return none
 */
void SMU_DRV_ClearSPFaultStatus(void);

/*!
 * @brief Get smu latent fault status.
 *
 * @param[in] none
 * @return smu latent fault status
 */
uint32_t SMU_DRV_GetLatentFaultStatus(void);

/*!
 * @brief Clear smu latent fault status.
 *
 * @param[in] none
 * @return none
 */
void SMU_DRV_ClearLatentFaultStatus(void);

/*!
 * @brief Enable single point fault channel reset function.
 *
 * @param[in] channel: single fault channel
 * @param[in] enable: enable or disble single fault channel reset function
 * @return none
 */
void SMU_DRV_SPFaultResetEnable(single_fault_channel_t channel, bool enable);

/*!
 * @brief Enable latent fault channel reset function.
 *
 * @param[in] channel: latent fault channel
 * @param[in] enable: enable or disble latent fault channel reset function
 * @return none
 */
void SMU_DRV_LatentFaultResetEnable(latent_fault_channel_t channel, bool enable);

/*!
 * @brief  set smu reset counter value.
 *
 * @param[in] val: reset counter value which can be 0~15
 * @return none
 */
void SMU_DRV_SetResetCounterVal(uint8_t val);

/*!
 * @brief get smu reset counter shadow.
 *
 * @param[in] none
 * @return smu reset counter shadow data
 */
uint32_t SMU_DRV_GetResetCounterShadow(void);

/*!
 * @brief clear smu reset counter shadow.
 *
 * @param[in] none
 * @return none
 */
void SMU_DRV_ClearResetCounterVal(void);

/*!
 * @brief Enable software single point fault channel function.
 *
 * @param[in] channel: software single point fault channel
 * @param[in] enable: enable or disble software single point fault channel function
 * @return none
 */
void SMU_DRV_SetSoftwareSPFaultEn(software_single_fault_channel_t channel, bool enable);

/*!
 * @brief Enable software latent fault channel function.
 *
 * @param[in] channel: software latent fault channel
 * @param[in] enable: enable or disble software latent fault channel function
 * @return none
 */
void SMU_DRV_SetSoftwareLatentFaultEn(software_latent_fault_channel_t channel, bool enable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SMU_DRV_H */

/* =============================================  EOF  ============================================== */
