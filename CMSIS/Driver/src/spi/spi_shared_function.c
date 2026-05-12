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
 * @file spi_drv.c
 *
 * @brief This file provides spi integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "spi_hw.h"
#include "spi_master_drv.h"
#include "spi_slave_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Base pointers for SPI instances. */
SPI_Type *g_spiBase[SPI_INSTANCE_MAX] = SPI_BASE_PTRS;

/* Table to save SPI IRQ enumeration. */
IRQn_Type g_spiIrqId[SPI_INSTANCE_MAX] = SPI_IRQS;

/* Table to save SPI ckgen interface clocks. */
ckgen_clock_t g_spiBusClock[SPI_INSTANCE_MAX] = SPI_CKGEN_CLOCKS;

/* Table to save SPI soft resets. */
ckgen_softreset_t g_spiSoftReset[SPI_INSTANCE_MAX] = SPI_SOFT_RESETS;

/* Pointer to runtime state structure.*/
spi_state_t *g_spiStatePtr[SPI_INSTANCE_MAX] = {NULL, NULL, NULL};

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get SPI module base address.
 *
 * @param[in] instance: SPI module instance
 * @return SPI module base address
 */
SPI_Type *SPI_DRV_GetBase(uint32_t instance)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);

    return (g_spiBase[instance]);
}

/*!
 * @brief SPI interrupt handler.
 *
 * @param[in] instance: SPI instance
 * @return none
 */
void SPI_DRV_IRQHandler(uint32_t instance)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);

    const SPI_Type *base = g_spiBase[instance];

    if (SPI_IsMaster(base))
    {
        /* Master mode.*/
        SPI_DRV_MasterIRQHandler(instance);
    }
    else
    {
        /* Slave mode.*/
        SPI_DRV_SlaveIRQHandler(instance);
    }
}

/*!
 * @brief Fill up the TX buffer with data. This is not a public API as it is
 *        called from other driver functions.
 *
 * @param[in] instance: SPI instance
 * @return none
 */
void SPI_DRV_FillupTxBuffer(uint32_t instance)
{
    /* Instantiate local variable of type dspi_master_state_t and point to global state. */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    uint32_t wordToSend = 0U;

    /* Fill the TX buffer. */
    if (spiState->isPcsContinuous)
    {
        if (1U == spiState->txCount)
        {
            spiState->txCount = 0U;

            return;
        }
    }

    if (spiState->txBuff != NULL)
    {
        if (spiState->bitsPerFrame <= 8U)
        {
            wordToSend = *((const uint8_t *)(spiState->txBuff));
            spiState->txBuff += sizeof(uint8_t);
            spiState->txCount -= sizeof(uint8_t);
        }
        else if (spiState->bitsPerFrame <= 16U)
        {
            wordToSend = *((const uint16_t *)(spiState->txBuff));
            spiState->txBuff += sizeof(uint16_t);
            spiState->txCount -= sizeof(uint16_t);
        }
        else
        {
            wordToSend = *((const uint32_t *)(spiState->txBuff));
            spiState->txBuff += sizeof(uint32_t);
            spiState->txCount -= sizeof(uint32_t);
        }
    }

    SPI_WriteData(base, wordToSend);
}

/*!
 * @brief Read data and stored in buffer.
 *        This is not a public API as it is called from other driver functions.
 *
 * @param[in] instance: SPI instance
 * @return none
 */
void SPI_DRV_ReadRXBuffer(uint32_t instance)
{
    spi_state_t *spiState = g_spiStatePtr[instance];
    const SPI_Type *base = g_spiBase[instance];
    uint32_t receivedWord = 0U;

    receivedWord = SPI_ReadData(base);

    if (spiState->rxBuff != NULL)
    {
        if (spiState->bitsPerFrame <= 8U)
        {
            *(spiState->rxBuff) = (uint8_t)receivedWord;
            spiState->rxBuff += sizeof(uint8_t);
            spiState->rxCount -= sizeof(uint8_t);
        }
        else if (spiState->bitsPerFrame <= 16U)
        {
            *(uint16_t *)(spiState->rxBuff) = (uint16_t)receivedWord;
            spiState->rxBuff += sizeof(uint16_t);
            spiState->rxCount -= sizeof(uint16_t);
        }
        else
        {
            *(uint32_t *)(spiState->rxBuff) = receivedWord;
            spiState->rxBuff += sizeof(uint32_t);
            spiState->rxCount -= sizeof(uint32_t);
        }
    }
}

/*!
 * @brief Disable the TX empty interrupts.
 *
 * @param[in] instance: SPI instance
 * @return none
 */
void SPI_DRV_DisableTEIEInterrupts(uint32_t instance)
{
    SPI_Type *base = g_spiBase[instance];

    SPI_SetIntMode(base, SPI_INT_TRANSMIT_ERROR, false);
    SPI_SetIntMode(base, SPI_INT_RECEIVE_ERROR, false);
    (void)SPI_ClearStatusFlag(base, SPI_TRANSMIT_ERROR);
    (void)SPI_ClearStatusFlag(base, SPI_RECEIVE_ERROR);
}

/* =============================================  EOF  ============================================== */
