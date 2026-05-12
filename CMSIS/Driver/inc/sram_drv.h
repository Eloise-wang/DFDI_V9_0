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
 * @file sram_drv.h
 *
 * @brief This file provides sram functions interface.
 *
 */

#ifndef _SRAM_DRV_H
#define _SRAM_DRV_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< Callback for SRAM ECC */
typedef void (*sram_callback_t)(void *device, uint32_t wpara, uint32_t lpara);

/*!
 * @brief ECC index enum
 */
typedef enum
{
    ECC_CHANNEL_0   = 0x00U,  /*!< ecc channel 0 */
    ECC_CHANNEL_1   = 0x01U   /*!< ecc channel 1 */
} sram_channel_index_t;

/*!
 * @brief SRAM ECC events
 */
typedef enum
{
    SRAM_EVENT_NONE              = 0U,   /*!< None event                      */
    SRAM_EVENT_SINGLE_BIT        = 1U,   /*!< Single-bit correction ECC event */
    SRAM_EVENT_NON_CORRECTABLE   = 2U    /*!< Non-correctable ECC event       */
} sram_ecc_event_t;

/*!
 * @brief SRAM ECC status
 */
typedef enum
{
    ECC_STATUS_NO_ERROR          = 0U,    /*!< No ECC error                               */
    ECC_STATUS_2BIT_ERROR        = 1U,    /*!< 2bit Non-correctable ECC error             */
    ECC_STATUS_1BIT_AT_BIT38     = 2U,    /*!< Single-bit correction ECC error at bit38   */
    ECC_STATUS_1BIT_AT_BIT37_0   = 3U,    /*!< Single-bit correction ECC error at bit37:0 */
    ECC_STATUS_ADDR_ERROR        = 4U     /*!< address error                              */
} sram_ecc_status_t;

/*!
 * @brief SRAM interrupt configuration structure
 */
typedef struct
{
    bool enableSingleCorrection;    /*!< Enable Single Correction Interrupt  */
    bool enableNonCorrectable;      /*!< Enable Non-Correctable Interrupt    */
} sram_interrupt_config_t;

/*!
 * @brief SRAM configuration structure
 */
typedef struct
{
    uint8_t channel;                                 /*!< SRAM channel                 */
    const sram_interrupt_config_t * interruptCfg;    /*!< SRAM Interrupt configuration */
} sram_user_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes and configures SRAM module.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channelIndex: The channel index of SRAM
 * @param[in] userConfigArr:  configure structure of SRAM
 * @return None
 */
void SRAM_DRV_Init(uint8_t instance, uint8_t channelIndex, const sram_user_config_t *userConfigArr);

/*!
 * @brief Sets SRAM to default configuration.
 *
 * @param[in] instance: The instance number of SRAM
 * @return None
 */
void SRAM_DRV_Deinit(uint8_t instance);

/*!
 * @brief Sets interrupt notification of SRAM.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[in] interruptCfg: The configuration structure of SRAM interrupt
 * @return None
 */
void SRAM_DRV_SetInterruptConfig(uint8_t instance, uint8_t channel, sram_interrupt_config_t interruptCfg);

/*!
 * @brief Gets interrupt notification of SRAM.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[out] interruptPtr: SRAM interrupt configuration structure
 * @return None
 */
void SRAM_DRV_GetInterruptConfig(uint8_t instance, uint8_t channel, sram_interrupt_config_t * const interruptPtr);

/*!
 * @brief Clears error event and interrupt flag.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channel: The channel index of SRAM
 * @return None
 */
void SRAM_DRV_ClearEvent(uint8_t instance, uint8_t channel);

/*!
 * @brief Gets the error address and event of SRAM ECC.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[out] addressPtr: The error address
 * @return The ECC event
 */
sram_ecc_event_t SRAM_DRV_GetErrorDetail(uint8_t instance, uint8_t channel, uint32_t *addressPtr);

/*!
 * @brief Installs SRAM callback function.
 *
 * @param[in] event: ECC error event
 * @param[in] func: Timer callback task
 * @return None
 */
void SRAM_DRV_InstallCallback(sram_ecc_event_t event, const sram_callback_t func);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SRAM_DRV_H */

/* =============================================  EOF  ============================================== */
