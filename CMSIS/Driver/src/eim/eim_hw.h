/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written psramission of AutoChips inc. and/or its licensors, any
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
 * @file eim_hw.h
 *
 * @brief This file provides eim integration functions interface.
 *
 */

#ifndef _EIM_HW_H
#define _EIM_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "eim_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Disables all channels and clears check-bit,data mask,and address mask of all the channels.
 *
 * @param[in] base: base address of EIM
 * @return None
 */
void EIM_InitChannel(EIM_CTRL_Type *const base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enables EIM module.
 *
 * @param[in] base: base address of EIM
 * @return None
 */
static inline void EIM_Enable(EIM_CTRL_Type *const base)
{
	MODIFY_REG32(base->EIM_GLB_ENABLE, EIM_CTRL_EIM_GLB_ENABLE_GLB_EN_Msk, EIM_CTRL_EIM_GLB_ENABLE_GLB_EN_Pos, 1U);
}

/*!
 * @brief Disables the EIM module.
 *
 * @param[in] base: base address of EIM
 * @return None
 */
static inline void EIM_Disable(EIM_CTRL_Type *const base)
{
	MODIFY_REG32(base->EIM_GLB_ENABLE, EIM_CTRL_EIM_GLB_ENABLE_GLB_EN_Msk, EIM_CTRL_EIM_GLB_ENABLE_GLB_EN_Pos, 0U);
}

/*!
 * @brief Enables or disables EIM channel.
 *
 * @param[in] base: base address of EIM
 * @param[in] channel: EIM channel index
 * @param[in] enable: enable & disable control of EIM channel
 * @return None
 */
static inline void EIM_EnableChannelCmd(EIM_CTRL_Type *const base, uint8_t channel, bool enable)
{
    MODIFY_REG32(base->EIM_CHEN, EIM_CTRL_EIM_CHEN_EIM0_EN_Msk << channel, channel, enable);
}

/*!
 * @brief Gets if EIM channel is enabled.
 *
 * @param[in] base: base address of EIM
 * @param[in] channel: EIM channel index
 * @return enable & disable status of EIM channel
 */
static inline bool EIM_IsChannelEnabled(const EIM_CTRL_Type *const base, uint8_t channel)
{
    return ((base->EIM_CHEN & (1U << channel)) != 0U);
}

/*!
 * @brief Sets check bit mask of EIM channel.
 *
 * @param[in] base: base address of EIM channel
 * @param[in] checkBitMask: Check-bit mask
 * @return None
 */
static inline void EIM_SetCheckBitMask(EIM_CHANNEL_Type *const base, uint8_t checkBitMask)
{
    base->EIM0_INJ_ECC = checkBitMask;
}

/*!
 * @brief Gets check bit mask of EIM channel.
 *
 * @param[in] base: base address of EIM channel
 * @return Check-bit mask
 */
static inline uint8_t EIM_GetCheckBitMask(const EIM_CHANNEL_Type *const base)
{
    return ((uint8_t)(base->EIM0_INJ_ECC));
}

/*!
 * @brief Sets data mask for EIM channel.
 *
 * @param[in] base: base address of EIM channel
 * @param[in] dataMask: Data mask
 * @return None
 */
static inline void EIM_SetDataMask(EIM_CHANNEL_Type *const base, uint32_t dataMask)
{
    base->EIM0_INJ_DATA = dataMask;
}

/*!
 * @brief Gets data mask of EIM channel.
 *
 * @param[in] base: base address of EIM channel
 * @return Data mask
 */
static inline uint32_t EIM_GetDataMask(const EIM_CHANNEL_Type *const base)
{
    return (base->EIM0_INJ_DATA);
}

/*!
 * @brief Sets data mask for EIM channel.
 *
 * @param[in] base: base address of EIM channel
 * @param[in] dataMask: addr mask
 * @return None
 */
static inline void EIM_SetAddrMask(EIM_CHANNEL_Type *const base, uint32_t addrMask)
{
    base->EIM0_INJ_ADDR = addrMask;
}

/*!
 * @brief Gets address mask of EIM channel.
 *
 * @param[in] base: base address of EIM channel
 * @return addr mask
 */
static inline uint32_t EIM_GetAddrMask(const EIM_CHANNEL_Type *const base)
{
    return (base->EIM0_INJ_ADDR);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIM_HW_H */

/* =============================================  EOF  ============================================== */
