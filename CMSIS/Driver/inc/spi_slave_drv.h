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
 * @file SPI_slave_drv.h
 *
 * @brief This file provides SPI slave integration functions interface.
 *
 */

#ifndef _SPI_SLAVE_DRV_H
#define _SPI_SLAVE_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "..\src\spi\spi_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief User configuration structure for the SPI slave driver.
 */
typedef struct
{
    spi_signal_polarity_t pcsPolarity;   /*!< PCS polarity */
    uint16_t bitcount;                   /*!< Number of bits per frame, 4~32 bits */
    spi_clock_phase_t clkPhase;          /*!< Selects which phase of clock to capture data */
    spi_which_pcs_t whichPcs;
    spi_sck_polarity_t clkPolarity;      /*!< Selects clock polarity */
    bool msbFirst;                       /*!< Option to transmit LSB first */
    spi_transfer_type transferType;      /*!< Type of SPI transfer */
    uint8_t rxDMAChannel;                /*!< Slect DMA rx channel. If DMA mode isn't used this field will be ignored. */
    uint8_t txDMAChannel;                /*!< Slect DMA tx channel. If DMA mode isn't used this field will be ignored. */
    spi_callback_t callback;             /*!< Select the callback to transfer complete */
    void *callbackParam;                 /*!< Select additional callback parameters if it's necessary */
} spi_slave_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Get default configuration for SPI slave.
 *
 * @param[out] spiConfig: Pointer to configuration structure which is filled with default configuration
 */
void SPI_DRV_SlaveGetDefaultConfig(spi_slave_config_t *spiConfig);

/*!
 * @brief Initializes SPI.
 *
 * @param[in] instance: SPI instance
 * @param[in] spiState: The pointer to the SPI slave driver state structure
 * @param[in] spiConfig: The data structure containing information about a device on the SPI bus
 * @return An error code or STATUS_SUCCESS.
 */
status_t SPI_DRV_SlaveInit(uint32_t instance, spi_state_t *spiState, const spi_slave_config_t *slaveConfig);

/*!
 * @brief De-initializes SPI.
 *
 * @param[in] instance: SPI instance
 * @return STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_DRV_SlaveDeinit(uint32_t instance);

/*!
 * @brief SPI using interrupt or DMA transfer by blocking mode, which will not return
 *        untill timeout or transfer finish.
 *
 * @param[in] instance: SPI module instance
 * @param[in] sendBuffer: The pointer to the data buffer of the data to send.
 * @param[in] receiveBuffer: Pointer to the buffer where the received bytes are stored.
 * @param[in] transferByteCount: The number of bytes to send and receive which is equal to size
 *            of send or receive buffers
 * @param[in] timeout: A timeout for the transfer in milliseconds. If the transfer takes longer than
 *            this amount of time, the transfer is aborted and a STATUS_TIMEOUT error returned.
 * @return STATUS_SUCCESS The transfer was successful, or
 *         STATUS_BUSY Cannot perform transfer because a transfer is already in progress, or
 *         STATUS_TIMEOUT The transfer timed out and was aborted.
 */
status_t SPI_DRV_SlaveTransferBlocking(uint32_t instance,
                                       const uint8_t *sendBuffer,
                                       uint8_t *receiveBuffer,
                                       uint16_t transferByteCount,
                                       uint32_t timeout);

/*!
 * @brief SPI using interrupt or DMA transfer by no-blocking mode, which will
 *        return after start transfer.The user needs to check whether the transfer
 *        is complete using the SPI_DRV_SlaveGetTransferStatus function.
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
status_t SPI_DRV_SlaveTransfer(uint32_t instance,
                               const uint8_t *sendBuffer,
                               uint8_t *receiveBuffer,
                               uint16_t transferByteCount);

/*!
 * @brief Aborts the transfer that started by a non-blocking call transfer function.
 *
 * This function stops the transfer which was started by the calling the SPI_DRV_SlaveTransfer() function.
 *
 * @param instance The instance number of SPI peripheral
 *
 * @return  STATUS_SUCCESS if everything is OK.
 *
 */
status_t SPI_DRV_SlaveAbortTransfer(uint32_t instance);

/*!
 * @brief Returns whether the previous transfer is finished.
 *
 * When performing an a-sync transfer, the user can call this function to ascertain
 * the state of the current transfer: in progress (or busy) or complete (success).
 * In addition, if the transfer is still in progress, the user can get the number
 * of bytes that have been transferred up to now.
 *
 * @param instance The instance number of the SPI peripheral.
 * @param bytesRemained Pointer to value that is filled in with the number of
 *  frames that have been sent in the active transfer. A frame is defined as the
 *  number of bits per frame.
 *
 * @return STATUS_SUCCESS The transfer has completed successfully, or
 *         STATUS_BUSY The transfer is still in progress.
 *         STATUS_ERROR if driver is error and needs to clean error.
 */
status_t SPI_DRV_SlaveGetTransferStatus(uint32_t instance, uint32_t *bytesRemained);

/*!
 * @brief Interrupt handler for SPI slave mode.
 *        This handler uses the buffers stored in the SPI_slave_state_t structs to transfer data.
 *
 * @param instance The instance number of the SPI peripheral.

 */
void SPI_DRV_SlaveIRQHandler(uint32_t instance);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SPI_SLAVE_DRV_H */

/* =============================================  EOF  ============================================== */
