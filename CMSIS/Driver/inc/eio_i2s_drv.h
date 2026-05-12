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
 * @file eio_i2s_drv.h
 *
 * @brief This file provides eio i2s integration functions interface.
 *
 */

#ifndef _EIO_I2S_DRV_H
#define _EIO_I2S_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "eio.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Define the enum of the events which can trigger I2S callback
 */
typedef enum
{
    EIO_I2S_EVENT_RX_FULL      = 0U,    /*!< Rx buffer is full */
    EIO_I2S_EVENT_TX_EMPTY     = 1U,    /*!< Tx buffer is empty */
    EIO_I2S_EVENT_END_TRANSFER = 2U,    /*!< The current transfer is ending */
    EIO_I2S_EVENT_ERROR        = 3U,    /*!< An error occurred during transfer */
} eio_i2s_event_t;

/*!< Callback for all peripherals which support i2s */
typedef void (*eio_i2s_callback_t)(eio_i2s_event_t event, void *userData);

/*!
 * @brief Master configuration structure
 */
typedef struct
{
    eio_driver_type_t driverType;           /*!< Driver type: interrupts/polling/DMA */
    uint32_t baudRate;                      /*!< Baud rate in hertz */
    uint8_t bitsWidth;                      /*!< Number of bits in a word - multiple of 8 */
    uint8_t txPin;                          /*!< eio pin to use for transmit */
    uint8_t rxPin;                          /*!< eio pin to use for receive */
    uint8_t sckPin;                         /*!< eio pin to use for serial clock */
    uint8_t wsPin;                          /*!< eio pin to use for word select */
    eio_i2s_callback_t callback;            /*!< User callback function. Note that this function will be
                                                 called from the interrupt service routine, so its
                                                 execution time should be as small as possible. It can be
                                                 NULL if it is not needed */
    void *callbackParam;                    /*!< param[in]eter for the callback function */
    uint8_t rxDMAChannel;                   /*!< Rx DMA channel number. Only used in DMA mode */
    uint8_t txDMAChannel;                   /*!< Tx DMA channel number. Only used in DMA mode */
} eio_i2s_master_user_config_t;

/*!
 * @brief Slave configuration structure
 */
typedef struct
{
    eio_driver_type_t driverType;           /*!< Driver type: interrupts/polling/DMA */
    uint8_t bitsWidth;                      /*!< Number of bits in a word - multiple of 8 */
    uint8_t txPin;                          /*!< eio pin to use for transmit */
    uint8_t rxPin;                          /*!< eio pin to use for receive */
    uint8_t sckPin;                         /*!< eio pin to use for serial clock */
    uint8_t wsPin;                          /*!< eio pin to use for word select */
    eio_i2s_callback_t callback;            /*!< User callback function. Note that this function will be
                                                 called from the interrupt service routine, so its
                                                 execution time should be as small as possible. It can be
                                                 NULL if it is not needed */
    void *callbackParam;                    /*!< param[in]eter for the callback function */
    uint8_t rxDMAChannel;                   /*!< Rx DMA channel number. Only used in DMA mode */
    uint8_t txDMAChannel;                   /*!< Tx DMA channel number. Only used in DMA mode */
} eio_i2s_slave_user_config_t;

/*!
 * @brief Master internal context structure
 */
typedef struct
{
    eio_common_state_t eioCommon;          /*!< Common eio drivers structure */
    const uint8_t *txData;                 /*!< Transmit buffer. */
    uint8_t *rxData;                       /*!< Receive buffer. */
    uint32_t txRemainingBytes;             /*!< Number of remaining bytes to be transmitted. */
    uint32_t rxRemainingBytes;             /*!< Number of remaining bytes to be received. */
    uint32_t dummyDmaData;                 /*!< Dummy location for DMA transfers. */
    uint8_t rxDMAChannel;                  /*!< Rx DMA channel number */
    uint8_t txDMAChannel;                  /*!< Tx DMA channel number */
    uint8_t bitsWidth;                     /*!< Number of bits in a word */
    uint8_t byteWidth;                     /*!< Number of bytes in a word */
    bool master;                           /*!< Current instance is in master mode */
    eio_driver_type_t driverType;          /*!< Driver type: interrupts/polling/DMA */
    status_t status;                       /*!< Current status of the driver */
    bool driverIdle;                       /*!< Idle/busy state of the driver */
    semaphore_t idleSemaphore;             /*!< Semaphore used by blocking functions */
    bool blocking;                         /*!< Specifies if the current transfer is blocking */
    eio_i2s_callback_t callback;           /*!< User callback function */
    void *callbackParam;                   /*!< param[in]eter for the callback function */
    bool clockContinueOutput;              /*!< clock Continue Output,use for AUDIO */
} eio_i2s_master_state_t;

/*!
 * @brief Slave internal context structure
 */
typedef eio_i2s_master_state_t eio_i2s_slave_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize the EIO_I2S master mode driver
 *        This function initializes the EIO_I2S driver in master mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_I2S master user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] master: Pointer to the EIO_I2S master driver context structure. The driver uses
 *                    this memory area for its internal logic. The application must make no
 *                    assumptions about the content of this structure, and must not free this
 *                    memory until the driver is de-initialized using EIO_I2S_DRV_MasterDeinit().
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterInit(uint32_t instance,
                                const eio_i2s_master_user_config_t * userConfigPtr,
                                eio_i2s_master_state_t * master);

/*!
 * @brief De-initialize the EIO_I2S master mode driver
 *        This function de-initializes the EIO_I2S driver in master mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterDeinit(eio_i2s_master_state_t *master);

/*!
 * @brief Set the baud rate and bit width for any subsequent I2S communication
 *        This function sets the baud rate (SCK frequency) and bit width for the I2S master.
 *        Note that due to module limitation not any baud rate can be achieved. The driver
 *        will set a baud rate as close as possible to the requested baud rate, but there may
 *        still be substantial differences, for example if requesting a high baud rate while
 *        using a low-frequency EIO clock. The application should call
 *        EIO_I2S_DRV_MasterGetBaudRate() after EIO_I2S_DRV_MasterSetConfig() to check
 *        what baud rate was actually set.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] baudRate: the desired baud rate in hertz
 * @param[in] bitsWidth: number of bits per word
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSetConfig(eio_i2s_master_state_t *master,
                                     uint32_t baudRate,
                                     uint8_t bitsWidth);

/*!
 * @brief Get the currently configured baud rate
 *        This function returns the currently configured I2S baud rate.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] baudRate: the current baud rate in hertz
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterGetBaudRate(eio_i2s_master_state_t *master, uint32_t *baudRate);

/*!
 * @brief Perform a non-blocking send transaction on the I2S bus
 *        This function starts the transmission of a block of data and returns immediately.
 *        The rest of the transmission is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2S_DRV_MasterGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2S_DRV_MasterGetStatus() to check the progress of the transmission.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSendData(eio_i2s_master_state_t *master,
                                    const uint8_t *txBuff,
                                    uint32_t txSize);

/*!
 * @brief Perform a blocking send transaction on the I2S bus
 *        This function sends a block of data, and
 *        only returns when the transmission is complete.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSendDataBlocking(eio_i2s_master_state_t *master,
                                            const uint8_t *txBuff,
                                            uint32_t txSize,
                                            uint32_t timeout);

/*!
 * @brief Perform a non-blocking receive transaction on the I2S bus
 *        This function starts the reception of a block of data and returns immediately.
 *        The rest of the reception is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2S_DRV_MasterGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2S_DRV_MasterGetStatus() to check the progress of the reception.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterReceiveData(eio_i2s_master_state_t *master,
                                       uint8_t *rxBuff,
                                       uint32_t rxSize);

/*!
 * @brief Perform a blocking receive transaction on the I2S bus
 *        This function receives a block of data and only returns when the reception is complete.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterReceiveDataBlocking(eio_i2s_master_state_t *master,
                                               uint8_t *rxBuff,
                                               uint32_t rxSize,
                                               uint32_t timeout);

/*!
 * @brief Aborts a non-blocking I2S master transaction
 *        This function aborts a non-blocking I2S transfer.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterTransferAbort(eio_i2s_master_state_t *master);

/*!
 * @brief Get the status of the current non-blocking I2S master transaction
 *        This function returns the current status of a non-blocking I2S master transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterGetStatus(eio_i2s_master_state_t *master, uint32_t *bytesRemaining);

/*!
 * @brief Provide a buffer for receiving data
 *        This function can be used to provide a new buffer for receiving data to the driver.
 *        It can be called from the user callback when event STATUS_I2S_RX_OVERRUN is reported.
 *        This way the reception will continue without interruption.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSetRxBuffer(eio_i2s_master_state_t *master,
                                       uint8_t *rxBuff,
                                       uint32_t rxSize);

/*!
 * @brief Provide a buffer for transmitting data
 *        This function can be used to provide a new buffer for transmitting data to the driver.
 *        It can be called from the user callback when event STATUS_I2S_TX_UNDERRUN is reported.
 *        This way the transmission will continue without interruption.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] txBuff: pointer to the buffer containing transmit data
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSetTxBuffer(eio_i2s_master_state_t *master,
                                       const uint8_t *txBuff,
                                       uint32_t txSize);

/*!
 * @brief Initialize the EIO_I2S slave mode driver
 *        This function initializes the EIO_I2S driver in slave mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_I2S slave user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure. The driver uses
 *                   this memory area for its internal logic. The application must make no
 *                   assumptions about the content of this structure, and must not free this
 *                   memory until the driver is de-initialized using EIO_I2S_DRV_SlaveDeinit().
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveInit(uint32_t instance,
                                const eio_i2s_slave_user_config_t *userConfigPtr,
                                eio_i2s_slave_state_t *slave);

/*!
 * @brief Set the bit width for any subsequent I2S communication
 *        This function sets the bit width for the I2S slave.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] bitsWidth: number of bits per word
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSetConfig(eio_i2s_slave_state_t *slave, uint8_t bitsWidth);

/*!
 * @brief Returns default configuration structure for EIO_I2S master
 *
 * @param[in] userConfigPtr: Pointer to the EIO_I2S user configuration structure.
 * @return none
 */
void EIO_I2S_DRV_MasterGetDefaultConfig(eio_i2s_master_user_config_t *userConfigPtr);

/*!
 * @brief Returns default configuration structure for EIO_I2S slave
 *
 * @param[in] userConfigPtr: Pointer to the EIO_I2S user configuration structure.
 * @return  none
 */
void EIO_I2S_DRV_SlaveGetDefaultConfig(eio_i2s_slave_user_config_t *userConfigPtr);

/*!
 * @brief De-initialize the EIO_I2S slave mode driver
 *        This function de-initializes the EIO_I2S driver in slave mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveDeinit(eio_i2s_slave_state_t *slave);

/*!
 * @brief Perform a non-blocking send transaction on the I2S bus
 *        This function starts the transmission of a block of data and returns immediately.
 *        The rest of the transmission is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2S_DRV_SlaveGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2S_DRV_SlaveGetStatus() to check the progress of the transmission.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSendData(eio_i2s_slave_state_t *slave,
                                   const uint8_t *txBuff,
                                   uint32_t txSize);

/*!
 * @brief Perform a blocking send transaction on the I2S bus
 *        This function sends a block of data, and
 *        only returns when the transmission is complete.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSendDataBlocking(eio_i2s_slave_state_t *slave,
                                           const uint8_t *txBuff,
                                           uint32_t txSize,
                                           uint32_t timeout);

/*!
 * @brief Perform a non-blocking receive transaction on the I2S bus
 *        This function starts the reception of a block of data and returns immediately.
 *        The rest of the reception is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2S_DRV_SlaveGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2S_DRV_SlaveGetStatus() to check the progress of the reception.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveReceiveData(eio_i2s_slave_state_t *slave,
                                      uint8_t *rxBuff,
                                      uint32_t rxSize);

/*!
 * @brief Perform a blocking receive transaction on the I2S bus
 *        This function receives a block of data and only returns when the reception is complete.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveReceiveDataBlocking(eio_i2s_slave_state_t *slave,
                                              uint8_t *rxBuff,
                                              uint32_t rxSize,
                                              uint32_t timeout);

/*!
 * @brief Aborts a non-blocking I2S slave transaction
 *        This function aborts a non-blocking I2S transfer.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveTransferAbort(eio_i2s_slave_state_t *slave);

/*!
 * @brief Get the status of the current non-blocking I2S slave transaction
 *        This function returns the current status of a non-blocking I2S slave transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveGetStatus(eio_i2s_slave_state_t *slave, uint32_t *bytesRemaining);

/*!
 * @brief Provide a buffer for receiving data
 *        This function can be used to provide a driver with a new buffer for receiving data.
 *        It can be called from the user callback when event STATUS_I2S_RX_OVERRUN is reported.
 *        This way the reception will continue without interruption.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSetRxBuffer(eio_i2s_slave_state_t *slave,
                                      uint8_t *rxBuff,
                                      uint32_t rxSize);

/*!
 * @brief Provide a buffer for transmitting data
 *        This function can be used to provide a driver with a new buffer for transmitting data.
 *        It can be called from the user callback when event STATUS_I2S_TX_UNDERRUN is reported.
 *        This way the transmission will continue without interruption.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] txBuff: pointer to the buffer containing transmit data
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSetTxBuffer(eio_i2s_slave_state_t *slave,
                                      const uint8_t *txBuff,
                                      uint32_t txSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIO_I2S_DRV_H */

/* =============================================  EOF  ============================================== */
