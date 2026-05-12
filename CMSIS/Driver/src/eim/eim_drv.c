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
 * @file eim_drv.c
 *
 * @brief This file provides eim functions interface.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "eim_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of base addresses for EIM instances */
static EIM_CHANNEL_Type *s_eimBase[EIM_CHANNEL_MAX] = EIM_BASE_PTRS;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Configures EIM channels and initializes the EIM module.
 *
 * @param[in] instance: instance number of EIM module
 * @param[in] channelIndex: EIM channel index
 * @param[in] channelConfigArr: configuration structure array of EIM channel
 * @return None
 */
void EIM_DRV_Init(uint8_t instance, uint8_t channelIndex, const eim_user_channel_config_t *channelConfigArr)
{
    DEVICE_ASSERT(instance < EIM_INSTANCE_COUNT);
    DEVICE_ASSERT(channelIndex < EIM_CHANNEL_MAX);
    DEVICE_ASSERT(channelConfigArr != NULL);
    EIM_CTRL_Type * base = EIM_CTRL;

    /* Configures error injection params */
    EIM_DRV_ConfigChannel(instance, channelConfigArr);
    EIM_Enable(base);
}

/*!
 * @brief De-initializes EIM module.
 *
 * @param[in] instance: instance number of EIM module
 * @return None
 */
void EIM_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < EIM_INSTANCE_COUNT);
    EIM_CTRL_Type * base = EIM_CTRL;

    /* EIM global Disable */
    EIM_Disable(base);
    /* EIM channel Disable */
    EIM_InitChannel(base);
}

/*!
 * @brief Configures the EIM channel.
 *
 * @param[in] instance: instance number of EIM module
 * @param[in] userChannelConfig: configuration structure of EIM channel
 * @return None
 */
void EIM_DRV_ConfigChannel(uint8_t instance, const eim_user_channel_config_t *userChannelConfig)
{
    DEVICE_ASSERT(instance < EIM_INSTANCE_COUNT);
    DEVICE_ASSERT(userChannelConfig != NULL);
    DEVICE_ASSERT(userChannelConfig->channel < EIM_CHANNEL_MAX);
    EIM_CTRL_Type * base = EIM_CTRL;
    uint8_t index = 0U;

    index = userChannelConfig->channel;
    EIM_EnableChannelCmd(base, userChannelConfig->channel, false);
    EIM_SetCheckBitMask(s_eimBase[index], userChannelConfig->checkBitMask);
    EIM_SetDataMask(s_eimBase[index], userChannelConfig->dataMask);
    EIM_SetAddrMask(s_eimBase[index], userChannelConfig->addrMask);
    EIM_EnableChannelCmd(base, userChannelConfig->channel, userChannelConfig->enable);
}

/*!
 * @brief Gets the check bit mask,data mask,addr mask and operation status of EIM channel.
 *
 * @param[in] instance: instance number of EIM module
 * @param[in] channel: EIM channel index
 * @param[out] channelConfig: configuration structure of EIM channel
 * @return None
 */
void EIM_DRV_GetChannelConfig(uint8_t instance, uint8_t channel, eim_user_channel_config_t *channelConfig)
{
    DEVICE_ASSERT(instance < EIM_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < EIM_CHANNEL_MAX);
    DEVICE_ASSERT(channelConfig != NULL);
    const EIM_CTRL_Type * base = EIM_CTRL;

    channelConfig->channel = channel;
    channelConfig->checkBitMask = EIM_GetCheckBitMask(s_eimBase[channel]);
    channelConfig->dataMask = EIM_GetDataMask(s_eimBase[channel]);
    channelConfig->addrMask = EIM_GetAddrMask(s_eimBase[channel]);
    channelConfig->enable = EIM_IsChannelEnabled(base, channel);
}

/*!
 * @brief Gets the default value of the check bit mask,data mask,addr mask and operation status.
 *
 * @param[in] channel: EIM channel index
 * @param[out] channelConfig: configuration structure default of EIM channel
 * @return None
 */
void EIM_DRV_GetDefaultConfig(uint8_t channel, eim_user_channel_config_t *channelConfig)
{
    DEVICE_ASSERT(channelConfig != NULL);
    DEVICE_ASSERT(channel < EIM_CHANNEL_MAX);

    channelConfig->channel = channel;
    channelConfig->checkBitMask = EIM_CHECKBITMASK_DEFAULT;
    channelConfig->dataMask = EIM_DATAMASK_DEFAULT;
    channelConfig->dataMask = EIM_ADDRMASK_DEFAULT;
    channelConfig->enable = true;
}

/* =============================================  EOF  ============================================== */
