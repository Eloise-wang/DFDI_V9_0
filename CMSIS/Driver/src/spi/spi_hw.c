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
 * @file spi_hw.c
 *
 * @brief This file provides spi integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "spi_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */
/*!
 * @brief Clears the SPI status flag, which can write "1" to clear.
 *
 * @param[in] base: SPI base pointer
 * @param[in] statusFlag: Select which status will be cleared
 * @return STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_ClearStatusFlag(SPI_Type *base, spi_status_flag_t statusFlag)
{
    status_t returnCode = STATUS_SUCCESS;

    switch (statusFlag)
    {
    /* These flags cannot write "1" to clear */
    case SPI_TX_DATA_FLAG:
    case SPI_RX_DATA_FLAG:
    case SPI_MASTER_BUSY:
    case SPI_MODULE_IDLE:
        returnCode = STATUS_ERROR;
        break;

    case SPI_MODE_FAULT_ERROR:
        SPI_SoftwareReset(base);
        break;

    /* Write "1" to clear independently */
    case SPI_TRANSMIT_ERROR:
    case SPI_RECEIVE_ERROR:
    case SPI_DATA_MATCH:
        base->STATUS = ((uint32_t)1U << (uint32_t)statusFlag);
        break;

    /* Write all the w1c status */
    case SPI_ALL_STATUS:
        base->STATUS = (uint32_t)SPI_ALL_STATUS;
        break;

    default:
        break;
    }

    return returnCode;
}

/*!
 * @brief SPI Module softreset. And it only reset master engine/buffer/flag logic,
 *        slave buffer/flag logic, the configuration of control registers(CFG0/
 *        CFG1/CFG2/CMD) will not be reset.
 *
 * @param[in] base: SPI base pointer
 * @return none
 */
void SPI_SoftwareReset(SPI_Type *base)
{
    SET_BIT32(base->CMD, SPI_CMD_SWRST_Msk);
    CLEAR_BIT32(base->CMD, SPI_CMD_SWRST_Msk);
}

/*!
 * @brief Set CS_HOLD, CS_SETUP or CS_IDLE time.
 *
 * @param[in] base:SPI base pointer
 * @param[in] whichDelay: Select which CS time
 * @param[in] delay: The 8-bit delay value 0x00 to 0xFF (255).
 * @return none
 */
void SPI_SetDelay(SPI_Type *base, spi_delay_type_t whichDelay, uint8_t delay)
{
    switch (whichDelay)
    {
    /* Set CS_HOLD */
    case SPI_SCK_TO_PCS:
        MODIFY_REG32(base->CFG0, SPI_CFG0_CS_HOLD_Msk, SPI_CFG0_CS_HOLD_Pos, delay);
        break;

    /* Set CS_SETUP */
    case SPI_PCS_TO_SCK:
        MODIFY_REG32(base->CFG0, SPI_CFG0_CS_SETUP_Msk, SPI_CFG0_CS_SETUP_Pos, delay);
        break;

    /* Set CS_IDLE */
    case SPI_BETWEEN_TRANSFER:
        MODIFY_REG32(base->CFG1, SPI_CFG1_CS_IDLE_Msk, SPI_CFG1_CS_IDLE_Pos, delay);
        break;

    default:
        break;
    }
}

/*!
 * @brief Configures the SPI PCS polarity.
 *
 * @param[in] base: SPI base pointer
 * @param[in] whichPcs: Select which PCS to configured
 *                     -SPI_PCS0
 *                     -SPI_PCS1
 *                     -SPI_PCS2
 *                     -SPI_PCS3
 * @param[in] pcsPolarity: Set PCS as active high or low
 *                     -SPI_ACTIVE_LOW
 *                     -SPI_ACTIVE_HIGH
 * @return none
 */
void SPI_SetPcsPolarityMode(SPI_Type *base, spi_which_pcs_t whichPcs, spi_signal_polarity_t pcsPolarity)
{
    uint32_t cfg2Val = 0;

    /* Clear the PCS polarity bit */
    cfg2Val = (base->CFG2) & (~((uint32_t)1U << (SPI_CFG2_PCS0POL_Pos + (uint32_t)whichPcs)));

    /* Configure the PCS polarity bit */
    cfg2Val |= (uint32_t)pcsPolarity << (SPI_CFG2_PCS0POL_Pos + (uint32_t)whichPcs);

    base->CFG2 = cfg2Val;
}

/*!
 * @brief Configure the SPI baud rate in bits per second.
 *
 * @param[in] base: Module base pointer of type SPI_Type.
 * @param[in] bitsPerSec: The desired baud rate in bits per second
 * @param[in] sourceClockInHz: SPI functional source input clock in Hertz
 * @return  The actual calculated baud rate. This function may also return a "0" if the
 *          bitPerSec is greater than sourceClockInHz/2
 */
uint32_t SPI_SetBaudRate(SPI_Type *base, uint32_t bitsPerSec, uint32_t sourceClockInHz)
{
    DEVICE_ASSERT(bitsPerSec != 0U);

    /* calculated baud rate */
    uint32_t actualFreq = 0U;
    uint16_t divisor = 0U;

    /* bitPerSec must be smaller than sourceClockInHz/2 */
    if (bitsPerSec > (sourceClockInHz >> 1U))
    {
        actualFreq = 0U;
    }
    else
    {
        /* Get SCK_HIGH and SCK_LOW */
        divisor = (sourceClockInHz / bitsPerSec) - 2U;
        divisor = (divisor >> 1U) & SPI_CFG0_SCK_HIGH_Msk;

        /* Set SCK_HIGH and SCK_LOW */
        MODIFY_REG32(base->CFG0, SPI_CFG0_SCK_HIGH_Msk, SPI_CFG0_SCK_HIGH_Pos, divisor);
        MODIFY_REG32(base->CFG0, SPI_CFG0_SCK_LOW_Msk, SPI_CFG0_SCK_LOW_Pos, divisor);

        /* Actual calculated baud rate
           SCK = function_sck/(sck_high + sck_low + 2) */
        actualFreq = sourceClockInHz / ((divisor << 1U) + 2U);
    }

    return actualFreq;
}

/*!
 * @brief Set SPI frame size(4~32 bits per frame)
 *
 * @param[in] base: SPI base pointer
 * @param[in] frameSize: SPI frame size, support 4~32 bits
 *                      -4: 4 bits
 *                      -5: 5 bits
 *                      -...
 *                      -32: 32 bits
 * @return none
 */
void SPI_SetFrameSize(SPI_Type *base, uint32_t frameSize)
{
    if (frameSize <= 4U)
    {
        frameSize = 3U;
    }
    else
    {
        frameSize = frameSize - 1U;
    }
    MODIFY_REG32(base->CFG1, SPI_CFG1_FRMSIZE_Msk, SPI_CFG1_FRMSIZE_Pos, frameSize);
}

/* =============================================  EOF  ============================================== */
