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
 * @file can_hw.c
 *
 * @brief This file provides CAN hardware interface functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "can_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Write TBUF.
 *
 * @param[in] base: CAN module
 * @param[in] txBuf: pointer to transmission buffer
 * @param[in] length: number of buffer bytes
 * @return none
 */
void CAN_WriteTbuf(CAN_Type *base, const uint8_t *txBuf, uint8_t length)
{
    DEVICE_ASSERT(txBuf != NULL);

    uint8_t i = 0U;
    uint32_t *tbuf = (uint32_t *)&base->TBUF;

    for (i = 0U; (i < length) && (i < CAN_MAX_TBUF_LENGTH); i += 4U)
    {
        *tbuf++ = *(uint32_t *)(&txBuf[i]);
    }
}

/*!
 * @brief Write TBUF message data.
 *
 * @param[in] base: CAN module
 * @param[in] txData: pointer to transmission data
 * @param[in] length: number of message data bytes
 * @return none
 */
void CAN_WriteTbufData(CAN_Type *base, const uint8_t *txData, uint8_t length)
{
    DEVICE_ASSERT(txData != NULL);

    uint8_t i = 0U;
    uint32_t *data = (uint32_t *)&base->TBUF.DATA[0];

    for (i = 0U; i < length; i += 4U)
    {
        *data++ = *(uint32_t *)(&txData[i]);
    }
}

/*!
 * @brief Read RBUF.
 *
 * @param[in] base: CAN module
 * @param[out] rxBuf: pointer to receive buffer
 * @param[in] length: number of buffer bytes
 * @return none
 */
void CAN_ReadRbuf(CAN_Type *base, uint8_t *rxBuf, uint8_t length)
{
    DEVICE_ASSERT(rxBuf != NULL);

    uint8_t i = 0U;
    uint32_t *rbuf = (uint32_t *)&base->RBUF;

    for (i = 0U; (i < length) && (i < CAN_MAX_RBUF_LENGTH); i += 4U)
    {
        *(uint32_t *)(&rxBuf[i]) = *rbuf++;
    }
}

/*!
 * @brief Read RBUF message data.
 *
 * @param[in] base: CAN module
 * @param[out] rxData: pointer to receive data
 * @param[in] length: number of message data bytes
 * @return none
 */
void CAN_ReadRbufData(CAN_Type *base, uint8_t *rxData, uint8_t length)
{
    DEVICE_ASSERT(rxData != NULL);

    uint8_t i = 0U;
    uint32_t *data = (uint32_t *)&base->RBUF.DATA[0];

    for (i = 0U; i < length; i += 4U)
    {
        *(uint32_t *)(&rxData[i]) = *data++;
    }
}

/*!
 * @brief Compute message payload size.
 *
 * @param[in] dlcValue: DLC value
 * @return Payload size
 */
uint8_t CAN_ComputePayloadSize(uint8_t dlcValue)
{
    uint8_t size = 0U;

    if (dlcValue <= 8U)
    {
        size = dlcValue;
    }
    else
    {
        switch (dlcValue)
        {
        case CAN_MSG_DLC_12_BYTES:
            size = 12U;
            break;

        case CAN_MSG_DLC_16_BYTES:
            size = 16U;
            break;

        case CAN_MSG_DLC_20_BYTES:
            size = 20U;
            break;

        case CAN_MSG_DLC_24_BYTES:
            size = 24U;
            break;

        case CAN_MSG_DLC_32_BYTES:
            size = 32U;
            break;

        case CAN_MSG_DLC_48_BYTES:
            size = 48U;
            break;

        case CAN_MSG_DLC_64_BYTES:
            size = 64U;
            break;

        default:
            break;
        }
    }

    return size;
}

/*!
 * @brief Compute message DLC value.
 *
 * @param[in] payloadSize: payload size
 * @return DLC value
 */
uint8_t CAN_ComputeDlcValue(uint8_t payloadSize)
{
    uint8_t dlcValue = 0U;

    if (payloadSize <= 8U)
    {
        dlcValue = payloadSize;
    }
    else if (payloadSize <= 12U)
    {
        dlcValue = (uint8_t)CAN_MSG_DLC_12_BYTES;
    }
    else if (payloadSize <= 16U)
    {
        dlcValue = (uint8_t)CAN_MSG_DLC_16_BYTES;
    }
    else if (payloadSize <= 20U)
    {
        dlcValue = (uint8_t)CAN_MSG_DLC_20_BYTES;
    }
    else if (payloadSize <= 24U)
    {
        dlcValue = (uint8_t)CAN_MSG_DLC_24_BYTES;
    }
    else if (payloadSize <= 32U)
    {
        dlcValue = (uint8_t)CAN_MSG_DLC_32_BYTES;
    }
    else if (payloadSize <= 48U)
    {
        dlcValue = (uint8_t)CAN_MSG_DLC_48_BYTES;
    }
    else if (payloadSize <= 64U)
    {
        dlcValue = (uint8_t)CAN_MSG_DLC_64_BYTES;
    }
    else
    {
    }

    return dlcValue;
}

/*!
 * @brief Start transmit frame in the TBUF.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @param[in] amount: transmit secondary all or one frames
 *            - CAN_TRANSMIT_SEC_ALL
 *            - CAN_TRANSMIT_SEC_ONE
 * @return none
 */
void CAN_StartTransmit(CAN_Type *base, can_transmit_buff_t type, can_transmit_sec_amount_t amount)
{
    if (CAN_TRANSMIT_PRIMARY == type)
    {
        base->CTRL0 |= CAN_CTRL0_TPE_Msk;
    }
    else
    {
        if (CAN_TRANSMIT_SEC_ALL == amount)
        {
            base->CTRL0 |= CAN_CTRL0_TSALL_Msk;    /* Send all messages */
        }
        else
        {
            base->CTRL0 |= CAN_CTRL0_TSONE_Msk;    /* Send one message only */
        }
    }
}

/*!
 * @brief Abort transmitting.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return none
 */
void CAN_AbortTransmit(CAN_Type *base, can_transmit_buff_t type)
{
    if (CAN_TRANSMIT_PRIMARY == type)
    {
        base->CTRL0 |= CAN_CTRL0_TPA_Msk;
    }
    else
    {
        base->CTRL0 |= CAN_CTRL0_TSA_Msk;
    }
}

/*!
 * @brief Set acceptance filter state.
 *
 * @param[in] base: CAN module
 * @param[in] index: filter index
 * @param[in] enable: filter enable state
 * @return none
 */
void CAN_SetFilterEnable(CAN_Type *base, uint8_t index, bool enable)
{
    DEVICE_ASSERT(index < CAN_FILTER_NUM_MAX);

    if (index < 16U)
    {
        if (enable)
        {
            base->ACFCTRL0 |= ((uint32_t)(1UL << index) << CAN_ACFCTRL0_ACFEN_Pos);
        }
        else
        {
            base->ACFCTRL0 &= ~((uint32_t)(1UL << index) << CAN_ACFCTRL0_ACFEN_Pos);
        }
    }
    else if (index < 48U)
    {
        if (enable)
        {
            base->ACFCTRL1 |= (uint32_t)(1UL << (index - 16U));
        }
        else
        {
            base->ACFCTRL1 &= ~(uint32_t)(1UL << (index - 16U));
        }
    }
    else if (index < CAN_FILTER_NUM_MAX)
    {
        if (enable)
        {
            base->ACFCTRL2 |= (uint32_t)(1UL << (index - 48U));
        }
        else
        {
            base->ACFCTRL2 &= ~(uint32_t)(1UL << (index - 48U));
        }
    }
}

/* =============================================  EOF  ============================================== */
