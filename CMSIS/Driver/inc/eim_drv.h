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
 * @file eim_drv.h
 *
 * @brief This file provides eim functions interface.
 */

#ifndef _EIM_DRV_H
#define _EIM_DRV_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
/*!< The default value of EIM check-bit mask  */
#define EIM_CHECKBITMASK_DEFAULT    (0x00UL)
/*!< The default value of EIM data mask       */
#define EIM_DATAMASK_DEFAULT        (0x00UL)
/*!< The default value of EIM data mask       */
#define EIM_ADDRMASK_DEFAULT        (0x00UL)

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief EIM index enum
 */
typedef enum
{
    EIM_CHANNEL_0   = 0x00U,  /*!< eim channel 0 */
    EIM_CHANNEL_1   = 0x01U   /*!< eim channel 1 */
}eim_channel_index_t;

/*!
 * @brief EIM channel configuration structure
 */
typedef struct
{
    uint8_t channel;      /*!< EIM channel index                        */
    uint8_t checkBitMask; /*!< the invert mask of the check-bit         */
    uint32_t dataMask;    /*!< the invert mask of the data bus          */
    uint32_t addrMask;    /*!< the invert mask of the address           */
    bool enable;          /*!< enable & disable control of EIM channel  */
} eim_user_channel_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Configures EIM channels and initializes the EIM module.
 *
 * @param[in] instance: instance number of EIM module
 * @param[in] channelIndex: EIM channel index
 * @param[in] channelConfigArr: configuration structure of EIM channel
 * @return None
 */
void EIM_DRV_Init(uint8_t instance, uint8_t channelIndex, const eim_user_channel_config_t *channelConfigArr);

/*!
 * @brief De-initializes EIM module.
 *
 * @param[in] instance: instance number of EIM module
 * @return None
 */
void EIM_DRV_Deinit(uint8_t instance);

/*!
 * @brief Configures the EIM channel.
 *
 * @param[in] instance: instance number of EIM module
 * @param[in] userChannelConfig: configuration structure of EIM channel
 * @return None
 */
void EIM_DRV_ConfigChannel(uint8_t instance, const eim_user_channel_config_t *userChannelConfig);

/*!
 * @brief Gets the check bit mask,data mask,addr mask and operation status of EIM channel.
 *
 * @param[in] instance: instance number of EIM module
 * @param[in] channel: EIM channel index
 * @param[out] channelConfig: configuration structure of EIM channel
 * @return None
 */
void EIM_DRV_GetChannelConfig(uint8_t instance, uint8_t channel, eim_user_channel_config_t *channelConfig);

/*!
 * @brief Gets the default value of the check bit mask,data mask,addr mask and operation status.
 *
 * @param[in] channel: EIM channel index
 * @param[out] channelConfig: configuration structure default of EIM channel
 * @return None
 */
void EIM_DRV_GetDefaultConfig(uint8_t channel, eim_user_channel_config_t *channelConfig);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIM_DRV_H */

/* =============================================  EOF  ============================================== */
