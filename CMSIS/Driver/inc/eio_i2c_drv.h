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
 * @file eio_i2c_drv.h
 *
 * @brief This file provides eio i2c integration functions interface.
 *
 */

#ifndef _EIO_I2C_DRV_H
#define _EIO_I2C_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "eio.h"

/* ============================================  Define  ============================================ */
/*!< Length of the DMA scatter-gather chain for Tx. Two blocks are needed, but as the fist one is
   loaded directly in the TCD, only one block needs to be in RAM  */
#define EIO_I2C_DMA_TX_CHAIN_LENGTH      (1UL)

/*!< Length of the DMA scatter-gather chain for Rx. Six blocks are needed, but as the fist one is
   loaded directly in the TCD, only five blocks needs to be in RAM  */
#define EIO_I2C_DMA_RX_CHAIN_LENGTH      (5UL)

/*!< Total size of the DMA scatter-gather buffer. Add 1 because STCD_SIZE already compensates for the
   first stcd not being loaded in RAM */
#define EIO_I2C_DMA_BUF_SIZE   (STCD_SIZE(EIO_I2C_DMA_TX_CHAIN_LENGTH + EIO_I2C_DMA_RX_CHAIN_LENGTH + 1UL))

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Define the enum of the events which can trigger I2C slave callback
 */
typedef enum
{
    EIO_I2C_SLAVE_EVENT_RX_FULL  =   0U,    /*!< i2c slave rx full event */
    EIO_I2C_SLAVE_EVENT_TX_EMPTY =   1U,    /*!< i2c slave tx empty event */
    EIO_I2C_SLAVE_EVENT_TX_REQ   =   2U,    /*!< i2c slave tx request event */
    EIO_I2C_SLAVE_EVENT_RX_REQ   =   3U,    /*!< i2c slave rx request event */
    EIO_I2C_SLAVE_EVENT_STOP     =   4U     /*!< i2c slave stop event */
} eio_i2c_slave_event_t;

/*!
 * @brief Define the enum of the events which can trigger I2C master callback
 */
typedef enum
{
    EIO_I2C_MASTER_EVENT_END_TRANSFER = 4U    /*!< i2c master end transfer event */
} eio_i2c_master_event_t;


/*!< Callback for all peripherals which supports I2C features for slave mode */
typedef void (*eio_i2c_slave_callback_t)(eio_i2c_slave_event_t event, void *userData);

/*!< Callback for all peripherals which supports I2C features for master mode */
typedef void (*eio_i2c_master_callback_t)(eio_i2c_master_event_t event, void *userData);

/*!
 * @brief Master configuration structure
 */
typedef struct
{
    uint16_t slaveAddress;                /*!< Slave address, 7-bit */
    eio_driver_type_t driverType;         /*!< Driver type: interrupts/polling/DMA */
    uint32_t baudRate;                    /*!< Baud rate in hertz */
    uint8_t sdaPin;                       /*!< eio pin to use as I2C SDA pin */
    uint8_t sclPin;                       /*!< eio pin to use as I2C SCL pin */
    eio_i2c_master_callback_t callback;   /*!< User callback function. Note that this function will be
                                            called from the interrupt service routine, so its
                                            execution time should be as small as possible. It can be
                                            NULL if it is not needed */
    void *callbackParam;                  /*!< param[in]eter for the callback function */
    uint8_t rxDMAChannel;                 /*!< Rx DMA channel number. Only used in DMA mode */
    uint8_t txDMAChannel;                 /*!< Tx DMA channel number. Only used in DMA mode */
} eio_i2c_master_user_config_t;

/*!
 * @brief Master internal context structure
 */
typedef struct
{
    eio_common_state_t eioCommon;          /*!< Common eio drivers structure */
    const uint8_t *txData;                 /*!< Transmit buffer */
    uint8_t *rxData;                       /*!< Receive buffer */
    uint32_t txRemainingBytes;             /*!< Number of remaining bytes to be transmitted */
    uint32_t rxRemainingBytes;             /*!< Number of remaining bytes to be received */
    uint32_t dmaReceiveBytes;              /*!< Number of remaining bytes to be dma received */
    uint8_t rxDMAChannel;                  /*!< Rx DMA channel number */
    uint8_t txDMAChannel;                  /*!< Tx DMA channel number */
    eio_i2c_master_callback_t callback;    /*!< User callback function */
    void *callbackParam;                   /*!< param[in]eter for the callback function */
    uint16_t slaveAddress;                 /*!< Slave address */
    uint16_t eventCount;                   /*!< Number of timer events, needed for long transfers */
    eio_driver_type_t driverType;          /*!< Driver type: interrupts/polling/DMA */
    status_t status;                       /*!< Current status of the driver */
    bool receive;                          /*!< Transfer direction, true = receive, false = transmit */
    bool addrReceived;                     /*!< Indicated start of receive (after address transmission) */
    bool driverIdle;                       /*!< Idle/busy state of the driver */
    semaphore_t idleSemaphore;             /*!< Semaphore used by blocking functions */
    bool blocking;                         /*!< Specifies if the current transfer is blocking */
    bool sendStop;                         /*!< Specifies if STOP condition must be generated after current transfer */
    uint8_t sdaPin;                        /*!< eio pin to use as I2C SDA pin */
    uint8_t sclPin;                        /*!< eio pin to use as I2C SCL pin */
    uint8_t dummyDmaIdle;                  /*!< Dummy location for DMA transfers */
    uint8_t dummyDmaStop;                  /*!< Dummy location for DMA transfers */
    uint8_t dummyDmaReceive;               /*!< Dummy location for DMA transfers */
    uint8_t lastReload;                    /*!< Counter value for the last timer reload. */
} eio_i2c_master_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize the EIO_I2C master mode driver
 *        This function initializes the EIO_I2C driver in master mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_I2C master user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] master: Pointer to the EIO_I2C master driver context structure. The driver uses
 *                    this memory area for its internal logic. The application must make no
 *                    assumptions about the content of this structure, and must not free this
 *                    memory until the driver is de-initialized using EIO_I2C_DRV_MasterDeinit().
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterInit(uint32_t instance,
                                const eio_i2c_master_user_config_t *userConfigPtr,
                                eio_i2c_master_state_t *master);

/*!
 * @brief De-initialize the EIO_I2C master mode driver
 *        This function de-initializes the EIO_I2C driver in master mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterDeinit(eio_i2c_master_state_t *master);

/*!
 * @brief Set the baud rate for any subsequent I2C communication
 *        This function sets the baud rate (SCL frequency) for the I2C master.
 *        Note that due to module limitation not any baud rate can be achieved. The driver
 *        will set a baud rate as close as possible to the requested baud rate, but there may
 *        still be substantial differences, for example if requesting a high baud rate while
 *        using a low-frequency EIO clock. The application should call
 *        EIO_I2C_DRV_MasterGetBaudRate() after EIO_I2C_DRV_MasterSetBaudRate() to check
 *        what baud rate was actually set.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] baudRate: the desired baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterSetBaudRate(eio_i2c_master_state_t *master, uint32_t baudRate);

/*!
 * @brief Get the currently configured baud rate
 *        This function returns the currently configured I2C baud rate.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] baudRate: the current baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterGetBaudRate(eio_i2c_master_state_t *master, uint32_t *baudRate);

/*!
 * @brief Set the slave address for any subsequent I2C communication
 *        This function sets the slave address which will be used for any future transfer.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] address: slave address, 7-bit
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterSetSlaveAddr(eio_i2c_master_state_t *master, uint16_t address);

/*!
 * @brief Perform a non-blocking send transaction on the I2C bus
 *        This function starts the transmission of a block of data to the currently
 *        configured slave address and returns immediately.
 *        The rest of the transmission is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2C_DRV_MasterGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2C_DRV_MasterGetStatus() to check the progress of the transmission.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @param[in] sendStop:  specifies whether or not to generate stop condition after the transmission
 * @return  Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterSendData(eio_i2c_master_state_t *master,
                                    const uint8_t *txBuff,
                                    uint32_t txSize,
                                    bool sendStop);

/*!
 * @brief Perform a blocking send transaction on the I2C bus
 *        This function sends a block of data to the currently configured slave address, and
 *        only returns when the transmission is complete.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @param[in] sendStop: specifies whether or not to generate stop condition after the transmission
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return  Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterSendDataBlocking(eio_i2c_master_state_t *master,
                                            const uint8_t * txBuff,
                                            uint32_t txSize,
                                            bool sendStop,
                                            uint32_t timeout);

/*!
 * @brief Perform a non-blocking receive transaction on the I2C bus
 *        This function starts the reception of a block of data from the currently
 *        configured slave address and returns immediately.
 *        The rest of the transmission is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2C_DRV_MasterGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2C_DRV_MasterGetStatus() to check the progress of the reception.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @param[in] sendStop: specifies whether or not to generate stop condition after the reception
 * @return  Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterReceiveData(eio_i2c_master_state_t *master,
                                       uint8_t *rxBuff,
                                       uint32_t rxSize,
                                       bool sendStop);

/*!
 * @brief Perform a blocking receive transaction on the I2C bus
 *        This function receives a block of data from the currently configured slave address,
 *        and only returns when the transmission is complete.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @param[in] sendStop: specifies whether or not to generate stop condition after the reception
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterReceiveDataBlocking(eio_i2c_master_state_t *master,
                                               uint8_t *rxBuff,
                                               uint32_t rxSize,
                                               bool sendStop,
                                               uint32_t timeout);

/*!
 * @brief Aborts a non-blocking I2C master transaction
 *        This function aborts a non-blocking I2C transfer.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterTransferAbort(eio_i2c_master_state_t *master);

/*!
 * @brief Get the status of the current non-blocking I2C master transaction
 *        This function returns the current status of a non-blocking I2C master transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] bytesRemaining: The remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterGetStatus(eio_i2c_master_state_t *master, uint32_t *bytesRemaining);

/*!
 * @brief Returns default configuration structure for EIO_I2C
 *
 * @param[in] userConfigPtr: Pointer to the EIO_I2C user configuration structure.
 * @return none
 */
void EIO_I2C_DRV_GetDefaultConfig(eio_i2c_master_user_config_t *userConfigPtr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIO_I2C_DRV_H */

/* =============================================  EOF  ============================================== */
