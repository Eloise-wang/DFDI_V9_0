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
 * @file i2c_drv.h
 *
 * @brief This file provides i2c interface for I2C functions.
 *
 */

#ifndef _I2C_DRV_H
#define _I2C_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================  Include  ============================================= */
#include "device_register.h"
#include "osif.h"

/* ============================================  Define  ============================================ */
/*!
* @brief I2C 10bit address reserve mask bit macro.
*/
#define I2C_ADDEXT_PRIMARY_BYTE_FIX (0xF8UL)

/* ===========================================  Typedef  ============================================ */
/*!< Callback for I2C slave mode */
typedef void (*i2c_slave_callback_t)(uint32_t event, void *userData);

/*!< Callback for I2C master mode */
typedef void (*i2c_master_callback_t)(uint32_t event, void *userData);

/*!
 * @brief Define the enum of the events which can trigger I2C slave callback
 */
typedef enum
{
    I2C_SLAVE_EVENT_RX_FULL      = 0x00U,    /*!< Rx buffer is full */
    I2C_SLAVE_EVENT_TX_EMPTY     = 0x01U,    /*!< Tx buffer is empty */
    I2C_SLAVE_EVENT_TX_REQ       = 0x02U,    /*!< Tx request */
    I2C_SLAVE_EVENT_RX_REQ       = 0x03U,    /*!< Rx request */
    I2C_SLAVE_EVENT_STOP         = 0x04U     /*!< Slave detected stop */
} i2c_slave_event_t;

/*!
 * @brief  Define the enum of the events which can trigger I2C master callback
 */
typedef enum
{
    I2C_MASTER_EVENT_END_TRANSFER     = 0x05U    /*!< Master transferend event */
} i2c_master_event_t;

/*!
 * @brief Type of I2C transfer direction (write or read)
 */
typedef enum
{
    I2C_WRITE    = 0x00U,   /*!< I2C write transfer*/
    I2C_READ     = 0x01U    /*!< I2C read transfer*/
} i2c_dir_type_t;

/*!
 * @brief Type of I2C transfer(based on interrupts or DMA)
 */
typedef enum
{
    I2C_USING_DMA            = 0x00U,   /*!< The driver will use DMA to perform I2C transfer*/
    I2C_USING_INTERRUPTS     = 0x01U    /*!< The driver will use interrupt to perform I2C transfer*/
} i2c_transfer_type_t;

/*!
 * @brief I2C master/slave mode enum
 */
typedef enum
{
    I2C_SLAVE = 0x00U,      /*!< I2C slave mode */
    I2C_MASTER = 0x01U      /*!< I2C master mode */
} i2c_mode_type_t;

/*!
 * @brief Configuration structure that the user needs to set
 */
typedef struct
{
    uint32_t baudRate;                    /*!< Baud rate */
    uint16_t slaveAddress;                /*!< Slave address, 7-bit or 10-bit */
    bool syncEn;                          /*!< Enable/disable SCL sync */
    bool arbitration;                     /*!< Enable/disable arbitration */
    bool is10bitAddr;                     /*!< Select 7-bit or 10-bit slave address */
    i2c_transfer_type_t transferType;     /*!< Type of I2C transfer, DMA or interrupt */
    uint8_t dmaChannel;                   /*!< Channel number for DMA. It will be ignored when DMA mode isn't used */
    i2c_master_callback_t callback;       /*!< Master callback function */
    void *callbackParam;                  /*!< Parameter for the master callback function */
} i2c_master_user_config_t;

/*!
 * @brief Master internal context structure
 */
typedef struct
{
    uint32_t rxSize;                      /*!< Size of receive data buffer */
    uint32_t txSize;                      /*!< Size of transmit data buffer */
    uint32_t rxPointer;                   /*!< position of receive data in user defined buffer */
    uint16_t slaveAddress;                /*!< Slave address */
    uint8_t *rxBuff;                      /*!< Pointer to receive data buffer */
    const uint8_t *txBuff;                /*!< Pointer to transmit data buffer */
    uint8_t dmaChannel;                   /*!< Channel number for DMA */
    i2c_dir_type_t dirType;               /*!< Type of I2C transferdirection */
    i2c_transfer_type_t transferType;     /*!< Type of I2C transfer, DMA or interrupt */
    volatile status_t status;             /*!< Status of last driver operation */
    volatile bool i2cIdle;                /*!< Idle/busy state of the driver */
    bool sendStop;                        /*!< Specifies if STOP condition must be generated after current transfer*/
    bool is10bitAddr;                     /*!< Select 7-bit or 10-bit slave address */
    semaphore_t idleSemaphore;            /*!< Semaphore used by blocking functions */
    bool blocking;                        /*!< Specifies if the current trandfer is blocking */
    i2c_master_callback_t callback;       /*!< Master callback function */
    void *callbackParam;                  /*!< Parameter for the master callback function */
} i2c_master_state_t;

/*!
 * @brief Slave configuration structure
 */
typedef struct
{
    uint16_t slaveAddress;                         /*!< Slave address, 7-bit or 10-bit */
    bool is10bitAddr;                              /*!< Select 7-bit or 10-bit slave address */
    bool stretchEn;                                /*!< Enable/disable slave SCL stretch*/
    bool wakeupEn;                                 /*!< Enable/disable slave address match wakeup */
    bool slaveListening;                           /*!< Slave mode (always listening or on demand only) */
    i2c_transfer_type_t transferType;              /*!< Type of I2C transfer, DMA or interrupt */
    uint8_t dmaChannel;                            /*!< Channel number for DMA channel */
    i2c_slave_callback_t callback;                 /*!< Slave callback function */
    void *callbackParam;                           /*!< Parameter for the master callback function */
} i2c_slave_user_config_t;

/*!
 * @brief Slave internal context structure
 */
typedef struct
{
    uint32_t rxSize;                               /*!< Size of receive data buffer */
    uint32_t txSize;                               /*!< Size of transmit data buffer */
    uint32_t rxPointer;                            /*!< position of receive data in user defined buffer */
    uint8_t *rxBuff;                               /*!< Pointer to receive data buffer */
    const uint8_t *txBuff;                         /*!< Pointer to transmit data buffer */
    uint8_t dmaChannel;                            /*!< Channel number for DMA channel */
    i2c_transfer_type_t transferType;              /*!< Type of I2C transfer, DMA or interrupt */
    volatile status_t status;                      /*!< Status of last driver operation */
    volatile bool isTransferInProgress;            /*!< Slave is busy because of an ongoing transfer*/
    semaphore_t idleSemaphore;                     /*!< Semaphore used by blocking functions */
    bool blocking;                                 /*!< Specifies if the current trandfer is blocking */
    bool is10bitAddr;                              /*!< Select 7-bit or 10-bit slave address */
    bool slaveListening;                           /*!< Slave mode (always listening or on demand only) */
    i2c_slave_callback_t callback;                 /*!< Slave callback function */
    void *callbackParam;                           /*!< Parameter for the master callback function */
} i2c_slave_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Get I2C module base address.
 *
 * @param[in] instance: I2C module instance
 * @return I2C module base address
 */
I2C_Type *I2C_DRV_GetBase(uint8_t instance);

/*!
 * @brief Initialize the I2C master mode driver based on configuration input.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] userConfig: I2C master user configuration pointer
 * @param[in] master: I2C master driver pointer
 * @return The result of execution
 */
status_t I2C_DRV_MasterInit(uint8_t instance, const i2c_master_user_config_t *userConfig, \
                            i2c_master_state_t *master);

/*!
 * @brief De-initialize the I2C master mode driver.
 *
 * @param[in] instance: The I2C instance number
 * @return The result of execution
 */
status_t I2C_DRV_MasterDeinit(uint8_t instance);

/*!
 * @brief Get the currently configured baud rate.
 *
 * @param[in] instance: The I2C instance number
 * @param[out] baudRate: The current baud rate of I2C
 * @return None
 */
void I2C_DRV_MasterGetBaudRate(uint8_t instance, uint32_t *baudRate);

/*!
 * @brief Set the baud rate for I2C communication.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] baudRate: The current setting baud rate value
 * @return The result of execution
 */
status_t I2C_DRV_MasterSetBaudRate(uint8_t instance, uint32_t baudRate);

/*!
 * @brief Set the slave address for I2C communication.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] address: Slave address, 7-bit or 10-bit
 * @param[in] is10bitAddr: Enable/disable 10-bit address
 * @return None
 */
void I2C_DRV_MasterSetSlaveAddr(uint8_t instance, const uint16_t address, const bool is10bitAddr);

/*!
 * @brief Perform a non-blocking send transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @param[in] sendStop: If true generate stop condition after the transmission, otherwise not
 * @return The result of execution
 */
status_t I2C_DRV_MasterSendData(uint8_t instance, const uint8_t *txBuff, uint32_t txSize, bool sendStop);

/*!
 * @brief Perform a blocking send transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @param[in] sendStop: If true generate stop condition after the transmission, otherwise not
 * @param[in] timeout: Timeout for the transfer
 * @return The result of execution
 */
status_t I2C_DRV_MasterSendDataBlocking(uint8_t instance, const uint8_t *txBuff, uint32_t txSize, \
                                        bool sendStop, uint32_t timeout);

/*!
 * @brief Abort a non-blocking I2C master transmission or reception.
 *
 * @param[in] instance: The I2C instance number
 * @return The result of execution
 */
status_t I2C_DRV_MasterAbortTransferData(uint8_t instance);

/*!
 * @brief Perform a non-blocking receive transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @param[in] sendStop: Specifies whether or not to generate stop condition after the reception
 * @return The result of execution
 */
status_t I2C_DRV_MasterReceiveData(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize, bool sendStop);

/*!
 * @brief Perform a blocking receive transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @param[in] sendStop: Specifies whether or not to generate stop condition after the reception
 * @param[in] timeout: Timeout for the transfer
 * @return The result of execution
 */
status_t I2C_DRV_MasterReceiveDataBlocking(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize, \
                                           bool sendStop, uint32_t timeout);

/*!
 * @brief Return the current status of the I2C master transfer.The user can call this function to ascertain
 *        the state of the current transferin an  non-blocking transfer.
 *
 * @param[in] instance: The I2C instance number
 * @param[out] bytesRemaining: The number of remaining bytes in the active I2C transfer
 * @return The status of master
 */
status_t I2C_DRV_MasterGetTransferStatus(uint8_t instance, uint32_t *bytesRemaining);

/*!
 * @brief Handle master operation when I2C interrupt occurs.
 *
 * @param[in] instance: The I2C instance number
 * @return None
 */
void I2C_DRV_MasterIRQHandler(uint8_t instance);

/*!
 * @brief Initialize the I2C slave mode driver.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] userConfig: Pointer to the I2C slave user configuration structure
 * @param[in] slave: Pointer to the I2C slave driver context structure
 * @return The result of execution
 */
status_t I2C_DRV_SlaveInit(uint8_t instance, const i2c_slave_user_config_t *userConfig, i2c_slave_state_t *slave);

/*!
 * @brief De-initialize the I2C slave mode driver.
 *
 * @param[in] instance: The I2C instance number
 * @return The result of execution
 */
status_t I2C_DRV_SlaveDeinit(uint8_t instance);

/*!
 * @brief Provide a buffer for transmitting data.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @return The result of execution
 */
status_t I2C_DRV_SlaveSetTxBuffer(uint8_t instance, const uint8_t *txBuff, uint32_t txSize);

/*!
 * @brief Provide a buffer for receiving data.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @return The result of execution
 */
status_t I2C_DRV_SlaveSetRxBuffer(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize);

/*!
 * @brief Perform a non-blocking send transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @return The result of execution
 */
status_t I2C_DRV_SlaveSendData(uint8_t instance, const uint8_t *txBuff, uint32_t txSize);

/*!
 * @brief Perform a blocking send transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @param[in] timeout: Timeout for the transfer
 * @return The result of execution
 */
status_t I2C_DRV_SlaveSendDataBlocking(uint8_t instance, const uint8_t *txBuff, uint32_t txSize, uint32_t timeout);

/*!
 * @brief Perform a non-blocking receive transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @return The result of execution
 */
status_t I2C_DRV_SlaveReceiveData(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize);

/*!
 * @brief Perform a blocking receive transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @param[in] timeout: Timeout for the transfer
 * @return The result of execution
 */
status_t I2C_DRV_SlaveReceiveDataBlocking(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize, uint32_t timeout);

/*!
 * @brief Return the current status of the I2C slave transfer. The user can call this function to ascertain
 *        the state of the current transferin an non-blocking transfer.
 *
 * @param[in] instance: The I2C instance number
 * @param[out] bytesRemaining: The number of remaining bytes in the active I2C transfer
 * @return The status of slave
 */
status_t I2C_DRV_SlaveGetTransferStatus(uint8_t instance, uint32_t *bytesRemaining);

/*!
 * @brief Abort a non-blocking I2C slave transmission or reception.
 *
 * @param[in] instance: The I2C instance number
 * @return The result of execution
 */
status_t I2C_DRV_SlaveAbortTransferData(uint8_t instance);

/*!
 * @brief Handle slave operation when I2C interrupt occurs.
 *
 * @param[in] instance: The I2C instance number
 * @return None
 */
void I2C_DRV_SlaveIRQHandler(uint8_t instance);

/*!
 * @brief Handle operation when I2C interrupt occurs.
 *
 * @param[in] instance: The I2C instance number
 * @return None
 */
void I2C_DRV_IRQHandler(uint8_t instance);

/*!
 * @brief Gets the default configuration structure for master.
 *
 * @param[out] config: Pointer to configuration structure
 * @return None
 */
void I2C_DRV_MasterGetDefaultConfig(i2c_master_user_config_t *config);

/*!
 * @brief Gets the default configuration structure for slave.
 *
 * @param[out] config: Pointer to configuration structure
 * @return None
 */
void I2C_DRV_SlaveGetDefaultConfig(i2c_slave_user_config_t *config);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _I2C_DRV_H */

/* =============================================  EOF  ============================================== */
