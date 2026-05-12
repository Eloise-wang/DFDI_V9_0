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
 * @file sram_hw.h
 *
 * @brief This file provides sram functions interface.
 *
 */

#ifndef _SRAM_HW_H
#define _SRAM_HW_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "sram_drv.h"

/* ============================================  Define  ============================================ */
/* The bit offset between channels */
#define SRAM_CHANNELS_OFFSET_SIZE     (8UL)
#define SRAM_L_BASE_ADDR              (0x1FFF0000UL)
#define SRAM_U_BASE_ADDR              (0x20000000UL)
#define SRAM_U_UPPER_ADDR             (0x2000F000UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes SRAM to default configuration.
 *
 * @param[in] base: base address of SRAM
 * @return None
 */
void SRAM_Init(ECC_SRAM_Type * const base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enables control of SRAM interrupt.
 *
 * @param[in] base: base address of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[in] eccEvent: The ECC event
 * @param[in] enable: Enable & disable cintrol of SRAM interrupt
 * @return None
 */
static inline void SRAM_EnableEventInterrupt(ECC_SRAM_Type * const base, uint8_t channel,
                                             sram_ecc_event_t eccEvent, bool enable)
{
    uint32_t tempCtrl = base->ECC_ERR_CTRL;

    /* Single-bit correction */
    if (SRAM_EVENT_SINGLE_BIT == eccEvent)
    {
        if (enable)
        {
            tempCtrl |= ECC_SRAM_ECC_ERR_CTRL_ECC0_1BIT_IRQ_EN_Msk << (channel * SRAM_CHANNELS_OFFSET_SIZE);
        }
        else
        {
            tempCtrl &= ~(ECC_SRAM_ECC_ERR_CTRL_ECC0_1BIT_IRQ_EN_Msk << (channel * SRAM_CHANNELS_OFFSET_SIZE));
        }
    }
    /* Non-correctable */
    else if (SRAM_EVENT_NON_CORRECTABLE == eccEvent)
    {
        if (enable)
        {
            tempCtrl |= ECC_SRAM_ECC_ERR_CTRL_ECC0_2BIT_IRQ_EN_Msk << (channel * SRAM_CHANNELS_OFFSET_SIZE);
        }
        else
        {
            tempCtrl &= ~(ECC_SRAM_ECC_ERR_CTRL_ECC0_2BIT_IRQ_EN_Msk << (channel * SRAM_CHANNELS_OFFSET_SIZE));
        }
    }
    else
    {
        /* Do nothing */
    }

    /* Write to register */
    base->ECC_ERR_CTRL = tempCtrl;
}

/*!
 * @brief Gets if the SRAM interrupt is enabled.
 *
 * @param[in] base: The base address of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[in] eccEvent: The ECC event
 * @return Interrupt enable status
 */
static inline bool SRAM_IsEventInterruptEnabled(const ECC_SRAM_Type * const base, uint8_t channel,
                                                sram_ecc_event_t eccEvent)
{
    uint32_t retVal = 0U;

    /* Single-bit correction */
    if (eccEvent == SRAM_EVENT_SINGLE_BIT)
    {
        retVal = base->ECC_ERR_CTRL \
                 & (ECC_SRAM_ECC_ERR_CTRL_ECC0_1BIT_IRQ_EN_Msk << (channel * SRAM_CHANNELS_OFFSET_SIZE));
    }
    /* Non-correctable */
    else if (eccEvent == SRAM_EVENT_NON_CORRECTABLE)
    {
        retVal = base->ECC_ERR_CTRL \
                 & (ECC_SRAM_ECC_ERR_CTRL_ECC0_2BIT_IRQ_EN_Msk << (channel * SRAM_CHANNELS_OFFSET_SIZE));
    }
    else
    {
        /* Do nothing */
    }

    return (retVal != 0U);
}

/*!
 * @brief Gets if the SRAM ECC error event occurred.
 *
 * @param[in] base: The base address of SRAM
 * @param[in] channel: The channel index of SRAM
 * @param[in] eccEvent: The ECC event
 * @return The SRAM error event status
 */
static inline bool SRAM_IsEventDetected(const ECC_SRAM_Type * const base, uint8_t channel, sram_ecc_event_t eccEvent)
{
    uint32_t retVal = 0U;
    uint32_t regAddr = 0U;

    regAddr = ((uint8_t)ECC_CHANNEL_0 == channel) ? (base->CH0_STATUS1) : (base->CH1_STATUS1);
    /* Single-bit correction */
    if (SRAM_EVENT_SINGLE_BIT == eccEvent)
    {
        retVal = (regAddr & ECC_SRAM_CH1_STATUS1_ERR_STS0_Msk) >> ECC_SRAM_CH1_STATUS1_ERR_STS0_Pos;
    }
    /* Non-correctable */
    else if (SRAM_EVENT_NON_CORRECTABLE == eccEvent)
    {
        retVal = (regAddr & ECC_SRAM_CH1_STATUS1_ERR_STS1_Msk) >> ECC_SRAM_CH1_STATUS1_ERR_STS1_Pos;
    }
    else
    {
        /* Do nothing */
    }

    return (retVal != 0U);
}

/*!
 * @brief Gets SRAM ECC error status.
 *
 * @param[in] base: The base address of SRAM
 * @param[in] channel: The channel index of SRAM
 * @return SRAM ECC error status
 */
static inline uint32_t SRAM_GetErrStatus(ECC_SRAM_Type * const base, uint8_t channel)
{
    uint32_t value = 0U;
    sram_ecc_status_t status = ECC_STATUS_NO_ERROR;

    if ((uint8_t)ECC_CHANNEL_0 == channel)
    {
        value = (base->CH0_STATUS0 & ECC_SRAM_CH0_STATUS0_ERR_ST_Msk) >> ECC_SRAM_CH0_STATUS0_ERR_ST_Pos;
    }
    else
    {
        value = (base->CH1_STATUS0 & ECC_SRAM_CH1_STATUS0_ERR_ST_Msk) >> ECC_SRAM_CH1_STATUS0_ERR_ST_Pos;
    }

    switch (value)
    {
    case 0U:
        status = ECC_STATUS_NO_ERROR;
        break;

    case 1U:
        status = ECC_STATUS_2BIT_ERROR;
        break;

    case 2U:
        status = ECC_STATUS_1BIT_AT_BIT38;
        break;

    case 3U:
        status = ECC_STATUS_1BIT_AT_BIT37_0;
        break;

    case 4U:
    case 5U:
    case 6U:
    case 7U:
        status = ECC_STATUS_ADDR_ERROR;
        break;

    default:
        break;
    }

    return (uint32_t)status;
}

/*!
 * @brief Clears error status and error address of SRAM ECC.
 *
 * @param[in] base: The base address of SRAM
 * @param[in] channel: The channel index of SRAM
 * @return None
 */
static inline void SRAM_ClearErrStatusAddr(ECC_SRAM_Type * const base, uint8_t channel)
{
    MODIFY_REG32(base->ECC_ERR_CTRL, ECC_SRAM_ECC_ERR_CTRL_ECC0_ST_CLR2_Msk << (channel * SRAM_CHANNELS_OFFSET_SIZE), \
                 ECC_SRAM_ECC_ERR_CTRL_ECC0_ST_CLR2_Pos + (channel * SRAM_CHANNELS_OFFSET_SIZE), 1U);
}

/*!
 * @brief Clears error interrupt of SRAM ECC.
 *
 * @param[in] base: The base address of SRAM
 * @param[in] channel: The channel index of SRAM
 * @return None
 */
static inline void SRAM_ClearErrInterrupt(ECC_SRAM_Type * const base, uint8_t channel)
{
    MODIFY_REG32(ECC_SRAM->ECC_ERR_CTRL, ECC_SRAM_ECC_ERR_CTRL_ECC0_ST_CLR1_Msk << (channel * SRAM_CHANNELS_OFFSET_SIZE), \
                 ECC_SRAM_ECC_ERR_CTRL_ECC0_ST_CLR1_Pos + (channel * SRAM_CHANNELS_OFFSET_SIZE), 1U);
}

/*!
 * @brief Gets 1bit error address of SRAM ECC.
 *
 * @param[in] base: The base address of SRAM
 * @param[in] channel: The channel index of SRAM
 * @return  1bit error address of SRAM ECC
 */
static inline uint32_t SRAM_Get1BitErrAddr(const ECC_SRAM_Type * const base, uint8_t channel)
{
    uint32_t singleBitAddr = 0x0U;

    if ((uint8_t)ECC_CHANNEL_0 == channel)
    {
        singleBitAddr = (READ_BIT32(base->CH0_STATUS0, ECC_SRAM_CH0_STATUS0_ERR_ADDR0_Msk) \
                         >> ECC_SRAM_CH0_STATUS0_ERR_ADDR0_Pos);
    }
    else if ((uint8_t)ECC_CHANNEL_1 == channel)
    {
        singleBitAddr = (READ_BIT32(base->CH1_STATUS0, ECC_SRAM_CH1_STATUS0_ERR_ADDR0_Msk) \
                        >> ECC_SRAM_CH1_STATUS0_ERR_ADDR0_Pos);
    }

    return singleBitAddr;
}

/*!
 * @brief Gets 2bit error address of SRAM ECC.
 *
 * @param[in] base: The base address of SRAM
 * @param[in] channel: The channel index of SRAM
 * @return  2bit error address of SRAM ECC
 */
static inline uint32_t SRAM_Get2BitErrAddr(const ECC_SRAM_Type * const base, uint8_t channel)
{
    uint32_t doubleBitAddr = 0x0U;

    if ((uint8_t)ECC_CHANNEL_0 == channel)
    {
        doubleBitAddr = (READ_BIT32(base->CH0_STATUS1, ECC_SRAM_CH0_STATUS1_ERR_ADDR1_Msk) \
                        >> ECC_SRAM_CH0_STATUS1_ERR_ADDR1_Pos);
    }
    else
    {
        doubleBitAddr = (READ_BIT32(base->CH1_STATUS1, ECC_SRAM_CH1_STATUS1_ERR_ADDR1_Msk) \
                        >> ECC_SRAM_CH1_STATUS1_ERR_ADDR1_Pos);
    }

    return doubleBitAddr;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SRAM_HW_H */

/* =============================================  EOF  ============================================== */
