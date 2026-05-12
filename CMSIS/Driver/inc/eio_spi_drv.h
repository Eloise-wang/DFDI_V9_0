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
 * @file eio_spi_drv.h
 *
 * @brief This file provides eio spi integration functions interface.
 *
 */

#ifndef _EIO_SPI_DRV_H
#define _EIO_SPI_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "eio.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Define the enum of the events which can trigger spi callback
 */
typedef enum
{
    EIO_SPI_EVENT_END_TRANSFER = 0U    /*!< spi end transfer event */
} eio_spi_event_t;

/*!< Callback for all peripherals which supports SPI features */
typedef void (*eio_spi_callback_t)(void *driverState, eio_spi_event_t event, void *userData);

/*!
 * @brief Order in which the data bits are transferred
 */
typedef enum
{
    EIO_SPI_TRANSFER_MSB_FIRST        = 0U,    /*!< Transmit data starting with most significant bit */
    EIO_SPI_TRANSFER_LSB_FIRST        = 1U     /*!< Transmit data starting with least significant bit */
} eio_spi_transfer_bit_order_t;

/*!
 * @brief @brief Size of transferred data in bytes
 */
typedef enum
{
    EIO_SPI_TRANSFER_1BYTE            = 1U,    /*!< Data size is 1-byte  */
    EIO_SPI_TRANSFER_2BYTE            = 2U,    /*!< Data size is 2-bytes */
    EIO_SPI_TRANSFER_4BYTE            = 4U     /*!< Data size is 4-bytes */
} eio_spi_transfer_size_t;

/*!
 * @brief Master configuration structure
 */
typedef struct
{
    uint32_t baudRate;                         /*!< Baud rate in hertz */
    eio_driver_type_t driverType;              /*!< Driver type: interrupts/polling/DMA */
    eio_spi_transfer_bit_order_t bitOrder;     /*!< Bit order: LSB-first / MSB-first */
    eio_spi_transfer_size_t transferSize;      /*!< Transfer size in bytes: 1/2/4 */
    uint8_t clockPolarity;                     /*!< Clock Polarity (CPOL) 0 = active-high clock; 1 = active-low clock */
    uint8_t clockPhase;                        /*!< Clock Phase (CPHA) 0 = sample on leading clock edge; 1 = sample on trailing clock edge */
    uint8_t mosiPin;                           /*!< eio pin to use as MOSI pin */
    uint8_t misoPin;                           /*!< eio pin to use as MISO pin */
    uint8_t sckPin;                            /*!< eio pin to use as SCK pin  */
    uint8_t ssPin;                             /*!< eio pin to use as SS pin   */
    eio_spi_callback_t callback;               /*!< User callback function. Note that this function will be
                                                    called from the interrupt service routine, so its
                                                    execution time should be as small as possible. It can be
                                                    NULL if it is not needed */
    void *callbackParam;                       /*!< param[in]eter for the callback function */
    uint8_t rxDMAChannel;                      /*!< Rx DMA channel number. Only used in DMA mode */
    uint8_t txDMAChannel;                      /*!< Tx DMA channel number. Only used in DMA mode */
} eio_spi_master_user_config_t;

/*!
 * @brief Slave configuration structure
 */
typedef struct
{
    eio_driver_type_t driverType;              /*!< Driver type: interrupts/polling/DMA */
    eio_spi_transfer_bit_order_t bitOrder;     /*!< Bit order: LSB-first / MSB-first */
    eio_spi_transfer_size_t transferSize;      /*!< Transfer size in bytes: 1/2/4 */
    uint8_t clockPolarity;                     /*!< Clock Polarity (CPOL) 0 = active-low clock; 1 = active-high clock */
    uint8_t clockPhase;                        /*!< Clock Phase (CPHA) 0 = sample on leading clock edge; 1 = sample on trailing clock edge */
    uint8_t mosiPin;                           /*!< eio pin to use as MOSI pin */
    uint8_t misoPin;                           /*!< eio pin to use as MISO pin */
    uint8_t sckPin;                            /*!< eio pin to use as SCK pin  */
    uint8_t ssPin;                             /*!< eio pin to use as SS pin   */
    eio_spi_callback_t callback;               /*!< User callback function. Note that this function will be
                                                    called from the interrupt service routine, so its
                                                    execution time should be as small as possible. It can be
                                                    NULL if it is not needed */
    void *callbackParam;                       /*!< param[in]eter for the callback function */
    uint8_t rxDMAChannel;                      /*!< Rx DMA channel number. Only used in DMA mode */
    uint8_t txDMAChannel;                      /*!< Tx DMA channel number. Only used in DMA mode */
} eio_spi_slave_user_config_t;

/*!
 * @brief Master internal context structure
 */
typedef struct
{
    eio_common_state_t eioCommon;              /*!<  Common eio drivers structure */
    const uint8_t *txData;                     /*!<  Transmit buffer. */
    uint8_t *rxData;                           /*!<  Receive buffer. */
    uint32_t txRemainingBytes;                 /*!<  Number of remaining bytes to be transmitted. */
    uint32_t rxRemainingBytes;                 /*!<  Number of remaining bytes to be received. */
    uint32_t dummyDmaData;                     /*!<  Dummy location for DMA transfers. */
    uint8_t rxDMAChannel;                      /*!<  Rx DMA channel number */
    uint8_t txDMAChannel;                      /*!<  Tx DMA channel number */
    eio_driver_type_t driverType;              /*!<  Driver type: interrupts/polling/DMA */
    eio_spi_transfer_bit_order_t bitOrder;     /*!<  Bit order: LSB-first / MSB-first */
    eio_spi_transfer_size_t transferSize;      /*!<  Transfer size in bytes: 1/2/4 */
    status_t status;                           /*!<  Current status of the driver */
    bool driverIdle;                           /*!<  Idle/busy state of the driver */
    semaphore_t idleSemaphore;                 /*!<  Semaphore used by blocking functions */
    bool blocking;                             /*!<  Specifies if the current transfer is blocking */
    bool master;                               /*!<  Specifies if the current instance was initialized as master */
    eio_spi_callback_t callback;               /*!<  User callback function */
    void *callbackParam;                       /*!<  param[in]eter for the callback function */
} eio_spi_master_state_t;

/*!< Slave internal context structure */
typedef eio_spi_master_state_t eio_spi_slave_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize the EIO_SPI master mode driver
 *        This function initializes the EIO_SPI driver in master mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_SPI master user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] master: Pointer to the EIO_SPI master driver context structure. The driver uses
 *                    this memory area for its internal logic. The application must make no
 *                    assumptions about the content of this structure, and must not free this
 *                    memory until the driver is de-initialized using EIO_SPI_DRV_MasterDeinit().
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterInit(uint32_t instance,
                                const eio_spi_master_user_config_t *userConfigPtr,
                                eio_spi_master_state_t *master);

/*!
 * @brief De-initialize the EIO_SPI master mode driver
 *        This function de-initializes the EIO_SPI driver in master mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterDeinit(eio_spi_master_state_t *master);

/*!
 * @brief Set the baud rate for any subsequent SPI communication
 *         This function sets the baud rate for the SPI master.
 *         Note that due to module limitation not any baud rate can be achieved. The driver
 *         will set a baud rate as close as possible to the requested baud rate, but there may
 *         still be substantial differences, for example if requesting a high baud rate while
 *         using a low-frequency EIO clock. The application should call
 *         EIO_SPI_DRV_MasterGetBaudRate() after EIO_SPI_DRV_MasterSetBaudRate() to check
 *         what baud rate was actually set.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] baudRate: the desired baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterSetBaudRate(eio_spi_master_state_t *master, uint32_t baudRate);

/*!
 * @brief Get the currently configured baud rate
 *        This function returns the currently configured SPI baud rate.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] baudRate: the current baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterGetBaudRate(eio_spi_master_state_t *master, uint32_t *baudRate);

/*!
 * @brief Perform a non-blocking SPI master transaction
 *        This function performs an SPI full-duplex transaction, transmit and receive in parallel.
 *        If only transmit or receive are required, it is possible to provide NULL pointers for
 *        txData or rxData. The transfer is non-blocking, the function only initiates the transfer
 *        and then returns, leaving the transfer to complete asynchronously).
 *        EIO_SPI_DRV_MasterGetStatus() can be called to check the status of the transfer.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] txData: pointer to the data to be transmitted
 * @param[in] rxData: pointer to the buffer where to store received data
 * @param[in] dataSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterTransfer(eio_spi_master_state_t *master,
                                    const uint8_t *txData,
                                    uint8_t *rxData,
                                    uint32_t dataSize);

/*!
 * @brief Perform a blocking SPI master transaction
 *        This function performs an SPI full-duplex transaction, transmit and receive in parallel.
 *        If only transmit or receive are required, it is possible to provide NULL pointers for
 *        txData or rxData. The transfer is blocking, the function only returns when the transfer
 *        is complete.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] txData: pointer to the data to be transmitted
 * @param[in] rxData: pointer to the buffer where to store received data
 * @param[in] dataSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterTransferBlocking(eio_spi_master_state_t *master,
                                            const uint8_t *txData,
                                            uint8_t *rxData,
                                            uint32_t dataSize,
                                            uint32_t timeout);

/*!
 * @brief Aborts a non-blocking SPI master transaction
 *       This function aborts a non-blocking SPI transfer.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterTransferAbort(eio_spi_master_state_t *master);

/*!
 * @brief Get the status of the current non-blocking SPI master transaction
 *        This function returns the current status of a non-blocking SPI master transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterGetStatus(eio_spi_master_state_t *master, uint32_t *bytesRemaining);

/*!
 * @brief Initialize the EIO_SPI slave mode driver
 *        This function initializes the EIO_SPI driver in slave mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_SPI slave user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure. The driver uses
 *                   this memory area for its internal logic. The application must make no
 *                   assumptions about the content of this structure, and must not free this
 *                   memory until the driver is de-initialized using EIO_SPI_DRV_SlaveDeinit().
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveInit(uint32_t instance,
                               const eio_spi_slave_user_config_t *userConfigPtr,
                               eio_spi_slave_state_t *slave);

/*!
 * @brief Returns default configuration structure for EIO_SPI master
 *
 * @param[in] userConfigPtr: Pointer to the EIO_SPI user configuration structure.
 * @return none
 */
void EIO_SPI_DRV_MasterGetDefaultConfig(eio_spi_master_user_config_t *userConfigPtr);

/*!
 * @brief Returns default configuration structure for EIO_SPI slave
 *
 * @param[in] userConfigPtr: Pointer to the EIO_SPI user configuration structure.
 * @return none
 */
void EIO_SPI_DRV_SlaveGetDefaultConfig(eio_spi_slave_user_config_t *userConfigPtr);

/*!
 * @brief De-initialize the EIO_SPI slave mode driver
 *        This function de-initializes the EIO_SPI driver in slave mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveDeinit(eio_spi_slave_state_t *slave);

/*!
 * @brief Perform a non-blocking SPI slave transaction
 *        This function performs an SPI full-duplex transaction, transmit and receive in parallel.
 *        If only transmit or receive are required, it is possible to provide NULL pointers for
 *        txData or rxData. The transfer is non-blocking, the function only initiates the transfer
 *        and then returns, leaving the transfer to complete asynchronously).
 *        EIO_SPI_DRV_SlaveGetStatus() can be called to check the status of the transfer.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @param[in] txData: pointer to the data to be transmitted
 * @param[in] rxData: pointer to the buffer where to store received data
 * @param[in] dataSize: length in bytes of the data to be transferred
 * @return  Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveTransfer(eio_spi_slave_state_t *slave,
                                   const uint8_t *txData,
                                   uint8_t *rxData,
                                   uint32_t dataSize);

/*!
 * @brief Perform a blocking SPI slave transaction
 *        This function performs an SPI full-duplex transaction, transmit and receive in parallel.
 *        If only transmit or receive are required, it is possible to provide NULL pointers for
 *        txData or rxData. The transfer is blocking, the function only returns when the transfer
 *        is complete.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @param[in] txData: pointer to the data to be transmitted
 * @param[in] rxData: pointer to the buffer where to store received data
 * @param[in] dataSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveTransferBlocking(eio_spi_slave_state_t *slave,
                                           const uint8_t *txData,
                                           uint8_t *rxData,
                                           uint32_t dataSize,
                                           uint32_t timeout);

/*!
 * @brief Aborts a non-blocking SPI slave transaction
 *        This function aborts a non-blocking SPI transfer.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveTransferAbort(eio_spi_slave_state_t *slave);

/*!
 * @brief Get the status of the current non-blocking SPI slave transaction
 *        This function returns the current status of a non-blocking SPI slave transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveGetStatus(eio_spi_slave_state_t *slave, uint32_t *bytesRemaining);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIO_SPI_DRV_H */

/* =============================================  EOF  ============================================== */
