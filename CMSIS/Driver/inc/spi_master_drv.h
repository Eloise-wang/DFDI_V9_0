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
 * @file spi_master_drv.h
 *
 * @brief This file provides spi master integration functions interface.
 *
 */

#ifndef _SPI_MASTER_DRV_H
#define _SPI_MASTER_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "..\src\spi\spi_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief  Data structure containing information about a device on the SPI bus
 */
typedef struct
{
    uint32_t bitsPerSec;                 /*!< Baud rate in bits per second */
    spi_which_pcs_t whichPcs;            /*!< Selects which PCS to use */
    spi_signal_polarity_t pcsPolarity;   /*!< PCS polarity */
    bool isPcsContinuous;                /*!< Keeps PCS asserted until transfer complete */
    uint16_t bitcount;                   /*!< Number of bits per frame, support 4~32 bits */
    uint32_t spiSrcClk;                  /*!< SPI functional clock */
    spi_clock_phase_t clkPhase;          /*!< Selects which phase of clock to capture data */
    spi_sck_polarity_t clkPolarity;      /*!< Selects clock polarity */
    bool msbFirst;                       /*!< Option to transmit MSB first */
    spi_transfer_type transferType;      /*!< Type of SPI transfer, using DMA or interrupt mode */
    uint8_t rxDMAChannel;                /*!< Select DMA rx channel. If DMA mode isn't used this field will be ignored. */
    uint8_t txDMAChannel;                /*!< Select DMA tx channel. If DMA mode isn't used this field will be ignored. */
    spi_callback_t callback;             /*!< Select the callback to transfer complete */
    void *callbackParam;                 /*!< Select additional callback parameters if it's necessary */
} spi_master_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Get default configuration for SPI master.
 *
 * @param[out] spiConfig: Pointer to configuration structure which is filled with default configuration
 */
void SPI_DRV_MasterGetDefaultConfig(spi_master_config_t *spiConfig);

/*!
 * @brief Initializes SPI.
 *
 * @param[in] instance: SPI instance
 * @param[in] spiState: The pointer to the SPI master driver state structure
 * @param[in] spiConfig: The data structure containing information about a device on the SPI bus
 * @return An error code or STATUS_SUCCESS.
 */
status_t SPI_DRV_MasterInit(uint32_t instance, spi_state_t *spiState, const spi_master_config_t *spiConfig);

/*!
 * @brief De-initializes SPI.
 *
 * @param[in] instance: SPI instance
 * @return STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_DRV_MasterDeinit(uint32_t instance);

/*!
 * @brief Configures the SPI master mode CS timing delay options.
 *
 * @param[in] instance: SPI instance
 * @param[in] delayBetwenTransfers: Minimum delay between 2 transfers(CS_IDLE) in microseconds
 * @param[in] delaySCKtoPCS: Minimum delay between SCK and CS(CS_HOLD) in microseconds
 * @param[in] delayPCStoSCK: Minimum delay between CS and SCK(CS_SETUP) in microseconds
 * @return STATUS_SUCCESS
 */
status_t SPI_DRV_MasterSetDelay(uint32_t instance, uint32_t delayBetwenTransfers,
                                uint32_t delaySCKtoPCS, uint32_t delayPCStoSCK);

/*!
 * @brief Configures the SPI bus physical parameters.
 *
 * @param[in] instance: SPI module instance
 * @param[in] spiConfig: Pointer to the spiConfig structure
 * @param[out] calculatedBaudRate: The calculated baud rate which will return for user
 * @return STATUS_SUCCESS The transfer has completed successfully, or
 *         STATUS_ERROR if driver is error and needs to clean error.
 */
status_t SPI_DRV_MasterConfigureBus(uint32_t instance,
                                    const spi_master_config_t *spiConfig,
                                    uint32_t *calculatedBaudRate);

/*!
 * @brief Select the chip to communicate with.
 *
 * The main purpose of this function is to set the PCS and the appropriate polarity.
 * @param[in] instance: SPI module instance
 * @param[in] whichPcs: selected chip
 * @param[in] polarity: chip select line polarity
 * @return STATUS_SUCCESS The transfer has completed successfully, or
 *         STATUS_ERROR if driver is error and needs to clean error.
 */
status_t SPI_DRV_SetPcs(uint32_t instance, spi_which_pcs_t whichPcs, spi_signal_polarity_t polarity);

/*!
 * @brief SPI using interrupt or DMA transfer by blocking mode, which will not return
 *        untill timeout or transfer finish.
 *
 * @param[in] instance: SPI module instance
 * @param[in] sendBuffer: The pointer to the data buffer of the data to send.
 * @param[in] receiveBuffer: Pointer to the buffer where the received bytes are stored.
 * @param transferByteCount: The number of bytes to send and receive which is equal to size of send or receive buffers
 * @param timeout: A timeout for the transfer in milliseconds. If the transfer takes longer than
 *        this amount of time, the transfer is aborted and a STATUS_TIMEOUT error returned.
 * @return STATUS_SUCCESS The transfer was successful, or
 *         STATUS_BUSY Cannot perform transfer because a transfer is already in progress, or
 *         STATUS_TIMEOUT The transfer timed out and was aborted.
 */
status_t SPI_DRV_MasterTransferBlocking(uint32_t instance,
                                        const uint8_t *sendBuffer,
                                        uint8_t *receiveBuffer,
                                        uint16_t transferByteCount,
                                        uint32_t timeout);

/*!
 * @brief SPI using interrupt or DMA transfer by no-blocking mode, which will
 *        return after start transfer.The user needs to check whether the transfer
 *        is complete using the SPI_DRV_MasterGetTransferStatus function.
 *
 * @param[in] instance: SPI module instance
 * @param[in] sendBuffer: The pointer to the data buffer of the data to send.
 * @param[in] receiveBuffer: Pointer to the buffer where the received bytes are stored.
 * @param[in] transferByteCount: The number of bytes to send and receive which is equal
 *            to size of send or receive buffers
 * @return STATUS_SUCCESS The transfer was successful, or
 *         STATUS_BUSY Cannot perform transfer because a transfer is already in progress, or
 *         STATUS_TIMEOUT The transfer timed out and was aborted.
 */
status_t SPI_DRV_MasterTransfer(uint32_t instance,
                                const uint8_t *sendBuffer,
                                uint8_t *receiveBuffer,
                                uint16_t transferByteCount);

/*!
 * @brief Returns whether the previous SPI using interrupt or DMA transfer
 *        (non-blocking) transfer is completed.
 *
 * @param[in] instance: SPI module instance
 * @param[in] bytesRemained: Remaining bytes to be transferred
 * @return STATUS_BUSY, STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_DRV_MasterGetTransferStatus(uint32_t instance, uint32_t *bytesRemained);

/*!
 * @brief Terminates asynchronous transfer early(no-blocking)
 *
 * @param[in] instance: SPI module instance
 * @return STATUS_SUCCESS
 */
status_t SPI_DRV_MasterAbortTransfer(uint32_t instance);

/*!
 * @brief Interrupt handler for SPI master mode, which will update the state
 *        stored in the spi_state_t structs to transfer data.This is not a
 *        public API as it is called whenever an interrupt occurs.
 * @param[in] instance: SPI module instance
 * @return none
 */
void SPI_DRV_MasterIRQHandler(uint32_t instance);

/*!
* @brief Release SPI Master CS to stop continuous selection of Slave
*
* @param[in] instance: SPI module instance
* @return none
*/
void SPI_DRV_MasterReleaseCS(uint32_t instance);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SPI_MASTER_DRV_H */

/* =============================================  EOF  ============================================== */
