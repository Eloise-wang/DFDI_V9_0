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
 * @file spi_shared_function.h
 *
 * @brief This file provides spi integration shared functions interface.
 *
 */

#ifndef _SPI_SHARED_FUNCTION_H
#define _SPI_SHARED_FUNCTION_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "osif.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< Callback for SPI */
typedef void (*spi_callback_t)(void *driverState, uint32_t event, void *userData);

/*!
 * @brief Define the enum of the events which can trigger SPI callback
 */
typedef enum
{
    SPI_EVENT_END_TRANSFER = 0U,
    SPI_EVENT_DATA_MATCH   = 1U,
	SPI_EVENT_MODE_FAULT_ERROR = 2U,
} spi_event_t;

/*!
 * @brief SPI Peripheral Chip Select(PCS) configuration
 */
typedef enum
{
    SPI_PCS0 = 0U, /*!< PCS0 */
    SPI_PCS1 = 1U, /*!< PCS1 */
    SPI_PCS2 = 2U, /*!< PCS2 */
    SPI_PCS3 = 3U  /*!< PCS3 */
} spi_which_pcs_t;

/*!
 * @brief  SPI PCS polarity configuration
 */
typedef enum
{
    SPI_ACTIVE_LOW  = 0U,  /*!< PCS active low (idle is high). */
    SPI_ACTIVE_HIGH = 1U   /*!< PCS active high (idle is low). */
} spi_signal_polarity_t;

/*!
 * @brief SPI clock phase configuration.
 */
typedef enum
{
    SPI_CLOCK_PHASE_1ST_EDGE = 0U, /*!< Data captured on SCK 1st edge, changed on 2nd. */
    SPI_CLOCK_PHASE_2ND_EDGE = 1U  /*!< Data changed on SCK 1st edge, captured on 2nd. */
} spi_clock_phase_t;

/*!
 * @brief SPI Clock (SCK) polarity configuration.
 */
typedef enum
{
    SPI_SCK_ACTIVE_HIGH = 0U, /*!< SCK is Active High (idle is low). */
    SPI_SCK_ACTIVE_LOW  = 1U  /*!< SCK is Active Low (idle is high). */
} spi_sck_polarity_t;

/*!
 * @brief Type of SPI transfer using interrupts or DMA.
 */
typedef enum
{
    SPI_USING_DMA = 0U,       /*!< SPI use DMA to transfer */
    SPI_USING_INTERRUPTS      /*!< SPI use interrupts to transfer */
} spi_transfer_type;

/*!
 * @brief Type of error reported by SPI
 */
typedef enum
{
    SPI_TRANSFER_OK = 0U,    /*!< Transfer OK */
    SPI_TRANSMIT_FAIL,       /*!< Error during transmission */
    SPI_RECEIVE_FAIL         /*!< Error during reception */
} transfer_status_t;

/*!
 * @brief Runtime state structure for the SPI master driver.
 */
typedef struct
{
    uint16_t bitsPerFrame;               /*!< Number of bits per frame: 4 to 32 bits */
    bool isPcsContinuous;                /*!< Select PCS continous mode */
    bool isBlocking;                     /*!< Save the blocking status */
    uint32_t spiSrcClk;                  /*!< SPI module fucntion clock source */
    volatile bool isTransferInProgress;  /*!< True if transfer is active */
    const uint8_t *txBuff;               /*!< Transmit buffer base pointer */
    uint8_t *rxBuff;                     /*!< Receive buffer base pointer */
    volatile uint16_t txCount;           /*!< Number of bytes remaining to transmit  */
    volatile uint16_t rxCount;           /*!< Number of bytes remaining to receive */
    volatile bool msb;                   /*!< True if first bit is MSB and false if first bit is LSB */
    uint8_t rxDMAChannel;                /*!< Select DMA rx channel */
    uint8_t txDMAChannel;                /*!< Select DMA tx channel */
    spi_transfer_type transferType;      /*!< Type of SPI transfer: interrupt or DMA mode */
    semaphore_t spiSemaphore;            /*!< The semaphore used for blocking transfers */
    transfer_status_t status;            /*!< The current transfer status */
    spi_callback_t callback;             /*!< Calling the callback function when transfer complete */
    void *callbackParam;                 /*!< Select additional callback parameters if it's necessary */
    uint32_t dummy;                      /*!< This field is used for the cases when TX is NULL and SPI is in DMA mode */
} spi_state_t;

/* ==========================================  Variables  =========================================== */
/*!< Table of base pointers for SPI instances. */
extern SPI_Type *g_spiBase[SPI_INSTANCE_MAX];

/*!< Table to save SPI IRQ enumeration numbers defined in the CMSIS header file. */
extern IRQn_Type g_spiIrqId[SPI_INSTANCE_MAX];

/*!< Table to save SPI ckgen interface clocks. */
extern ckgen_clock_t g_spiBusClock[SPI_INSTANCE_MAX];

/*!< Table to save SPI soft resets. */
extern ckgen_softreset_t g_spiSoftReset[SPI_INSTANCE_MAX];

/*!< Pointer to runtime state structure.*/
extern spi_state_t *g_spiStatePtr[SPI_INSTANCE_MAX];

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Get SPI module base address.
 *
 * @param[in] instance: SPI module instance
 * @return SPI module base address
 */
SPI_Type *SPI_DRV_GetBase(uint32_t instance);

/*!
 * @brief SPI interrupt handler.
 *
 * @param[in] instance: SPI instance
 * @return none
 */
void SPI_DRV_IRQHandler(uint32_t instance);

/*!
 * @brief Fill up the TX buffer with data. This is not a public API as it is
 *        called from other driver functions.
 *
 * @param[in] instance: SPI instance
 * @return none
 */
void SPI_DRV_FillupTxBuffer(uint32_t instance);

/*!
 * @brief Read data and stored in buffer.
 *        This is not a public API as it is called from other driver functions.
 *
 * @param[in] instance: SPI instance
 * @return none
 */
void SPI_DRV_ReadRXBuffer(uint32_t instance);

/*!
 * @brief Disable the TX empty interrupts.
 *
 * @param[in] instance: SPI instance
 * @return none
 */
void SPI_DRV_DisableTEIEInterrupts(uint32_t instance);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SPI_SHARED_FUNCTION_H */

/* =============================================  EOF  ============================================== */
