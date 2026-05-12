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
 * @file sram_drv.c
 *
 * @brief This file provides sram functions interface.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "sram_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table to save Sram IRQ numbers. */
static const IRQn_Type s_sramIRQ[(uint8_t)SRAM_EVENT_NON_CORRECTABLE] = SRAM_IRQS;
/* SRAM ECC callback pointer */
static sram_callback_t s_eccSramCallback[(uint8_t)SRAM_EVENT_NON_CORRECTABLE] = {NULL};

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initializes and configures SRAM module.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channelIndex: The channel index of SRAM
 * @param[in] userConfigArr:  configure structure of SRAM
 * @return None
 */
void SRAM_DRV_Init(uint8_t instance, uint8_t channelIndex, const sram_user_config_t *userConfigArr)
{
    DEVICE_ASSERT(instance < SRAM_INSTANCE_COUNT);
    DEVICE_ASSERT(userConfigArr != NULL);
    DEVICE_ASSERT(channelIndex < SRAM_CHANNEL_MAX);
    ECC_SRAM_Type *base = ECC_SRAM;

    SRAM_Init(base);
    SRAM_DRV_SetInterruptConfig(instance, channelIndex, *userConfigArr->interruptCfg);
}

/*!
 * @brief Sets SRAM to default configuration
 *
 * @param[in] instance: The instance number of SRAM
 * @return None
 */
void SRAM_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < SRAM_INSTANCE_COUNT);
    ECC_SRAM_Type *base = ECC_SRAM;

    /* Set SRAM to default configuration */
    SRAM_Init(base);
}

/*!
 * @brief Sets interrupt notification of SRAM.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[in] interruptCfg: The configuration structure of SRAM interrupt
 * @return None
 */
void SRAM_DRV_SetInterruptConfig(uint8_t instance, uint8_t channel, sram_interrupt_config_t interruptCfg)
{
    DEVICE_ASSERT(instance < SRAM_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < SRAM_CHANNEL_MAX);
    ECC_SRAM_Type *base = ECC_SRAM;

    SRAM_EnableEventInterrupt(base, channel, SRAM_EVENT_SINGLE_BIT, interruptCfg.enableSingleCorrection);
    SRAM_EnableEventInterrupt(base, channel, SRAM_EVENT_NON_CORRECTABLE, interruptCfg.enableNonCorrectable);
    if (interruptCfg.enableSingleCorrection)
    {
        NVIC_EnableIRQ(s_sramIRQ[(uint8_t)SRAM_EVENT_SINGLE_BIT - 1U]);
    }
    else
    {
        NVIC_DisableIRQ(s_sramIRQ[(uint8_t)SRAM_EVENT_SINGLE_BIT - 1U]);
    }
    if (interruptCfg.enableNonCorrectable)
    {
        NVIC_EnableIRQ(s_sramIRQ[(uint8_t)SRAM_EVENT_NON_CORRECTABLE - 1U]);
    }
    else
    {
        NVIC_DisableIRQ(s_sramIRQ[(uint8_t)SRAM_EVENT_NON_CORRECTABLE - 1U]);
    }
}

/*!
 * @brief Gets interrupt notification of SRAM.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[out] interruptPtr: SRAM interrupt configuration structure
 * @return None
 */
void SRAM_DRV_GetInterruptConfig(uint8_t instance, uint8_t channel, sram_interrupt_config_t *const interruptPtr)
{
    DEVICE_ASSERT(instance < SRAM_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < SRAM_CHANNEL_MAX);
    DEVICE_ASSERT(interruptPtr != NULL);
    const ECC_SRAM_Type *base = ECC_SRAM;

    interruptPtr->enableSingleCorrection = SRAM_IsEventInterruptEnabled(base, channel, SRAM_EVENT_SINGLE_BIT);
    interruptPtr->enableNonCorrectable = SRAM_IsEventInterruptEnabled(base, channel, SRAM_EVENT_NON_CORRECTABLE);
}

/*!
 * @brief Clears error event and interrupt flag.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channel: The channel index of SRAM
 * @return None
 */
void SRAM_DRV_ClearEvent(uint8_t instance, uint8_t channel)
{
    DEVICE_ASSERT(instance < SRAM_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < SRAM_CHANNEL_MAX);
    ECC_SRAM_Type *base = ECC_SRAM;

    SRAM_ClearErrStatusAddr(base, channel);
    SRAM_ClearErrInterrupt(base, channel);
}

/*!
 * @brief Gets the error address and event of SRAM ECC.
 *
 * @param[in] instance: The instance number of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[out] addressPtr: The error address
 * @return The ECC event
 */
sram_ecc_event_t SRAM_DRV_GetErrorDetail(uint8_t instance, uint8_t channel, uint32_t *addressPtr)
{
    DEVICE_ASSERT(instance < SRAM_INSTANCE_COUNT);
    DEVICE_ASSERT(channel < SRAM_CHANNEL_MAX);
    DEVICE_ASSERT(addressPtr != NULL);
    const ECC_SRAM_Type *base = ECC_SRAM;
    sram_ecc_event_t eccEvent;
    uint32_t addrBase, errAddr = 0U;

    addrBase = (0U == channel) ? SRAM_L_BASE_ADDR : SRAM_U_BASE_ADDR;

    if (SRAM_IsEventDetected(base, channel, SRAM_EVENT_SINGLE_BIT) != false)
    {
        eccEvent = SRAM_EVENT_SINGLE_BIT;
        errAddr = SRAM_Get1BitErrAddr(base, channel);
    }
    else if (SRAM_IsEventDetected(base, channel, SRAM_EVENT_NON_CORRECTABLE) != false)
    {
        eccEvent = SRAM_EVENT_NON_CORRECTABLE;
        errAddr = SRAM_Get2BitErrAddr(base, channel);
    }
    else
    {
        eccEvent = SRAM_EVENT_NONE;
    }

    /* Calculates the actual error address */
    *addressPtr = ((errAddr << 2U) + addrBase);

    return eccEvent;
}

/*!
 * @brief Installs sram callback function.
 *
 * @param[in] event: ECC error event
 * @param[in] func: Timer callback task
 * @return None
 */
void SRAM_DRV_InstallCallback(sram_ecc_event_t event, const sram_callback_t func)
{
    uint8_t index = 0U;

    index = (uint8_t)event - 1U;
    s_eccSramCallback[index] = func;
}

/*!
 * @brief SRAM ECC error interrupt handler.
 *
 * @param[in] event: ECC error event
 * @return None
 */
void SRAM_DRV_CommonISR(sram_ecc_event_t event)
{
    uint32_t wpara = 0U;

    if (NULL != s_eccSramCallback[(uint8_t)event - 1U])
    {
        /* callback */
        s_eccSramCallback[(uint8_t)event - 1U](ECC_SRAM, wpara, 0U);
    }
}

/* =============================================  EOF  ============================================== */
