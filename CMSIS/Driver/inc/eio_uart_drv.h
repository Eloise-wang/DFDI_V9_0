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
 * @file eio_uart_drv.h
 *
 * @brief This file provides eio uart integration functions interface.
 *
 */

#ifndef _EIO_UART_DRV_H
#define _EIO_UART_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "eio.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Define the enum of the events which can trigger UART callback
 */
typedef enum
{
    EIO_UART_EVENT_RX_FULL      = 0U,    /*!< Rx buffer is full */
    EIO_UART_EVENT_TX_EMPTY     = 1U,    /*!< Tx buffer is empty */
    EIO_UART_EVENT_END_TRANSFER = 2U,    /*!< The current transfer is ending */
    EIO_UART_EVENT_ERROR        = 3U     /*!< An error occured during transfer */
} eio_uart_event_t;

/*!
 * @brief Callback for all peripherals which support UART features
 */
typedef void (*eio_uart_callback_t)(void *driverState, eio_uart_event_t event, void *userData);

/*!
 * @brief eio_uart driver direction (tx or rx)
 */
typedef enum
{
    EIO_UART_DIRECTION_TX       = 1U,    /*!< Tx UART driver */
    EIO_UART_DIRECTION_RX       = 0U     /*!< Rx UART driver */
} eio_uart_driver_direction_t;

/*!
 * @brief Driver configuration structure
 */
typedef struct
{
    eio_driver_type_t driverType;               /*!< Driver type: interrupts/polling/DMA */
    uint32_t baudRate;                          /*!< Baud rate in hertz */
    uint8_t bitCount;                           /*!< Number of bits per word */
    eio_uart_driver_direction_t direction;      /*!< Driver direction: Tx or Rx */
    uint8_t dataPin;                            /*!< eio pin to use as Tx or Rx pin */
    eio_uart_callback_t callback;               /*!< User callback function. Note that this function will be
                                                     called from the interrupt service routine, so its
                                                     execution time should be as small as possible. It can be
                                                     NULL if it is not needed */
    void *callbackParam;                        /*!< param[in]eter for the callback function */
    uint8_t dmaChannel;                         /*!< DMA channel number. Only used in DMA mode */
} eio_uart_user_config_t;

/*!
 * @brief Driver internal context structure
 */
typedef struct
{
    eio_common_state_t eioCommon;              /*!< Common eio drivers structure */
    uint8_t *rxData;                           /*!< Receive buffer. */
    const uint8_t *txData;                     /*!< Transmit buffer. */
    uint32_t remainingBytes;                   /*!< Number of remaining bytes to be transferred. */
    uint8_t dmaChannel;                        /*!< DMA channel number */
    eio_uart_callback_t callback;              /*!< User callback function */
    void *callbackParam;                       /*!< param[in]eter for the callback function */
    eio_uart_driver_direction_t direction;     /*!< Driver direction: Tx or Rx */
    eio_driver_type_t driverType;              /*!< Driver type: interrupts/polling/DMA */
    status_t status;                           /*!< Current status of the driver */
    bool driverIdle;                           /*!< Idle/busy state of the receiver */
    semaphore_t idleSemaphore;                 /*!< Semaphore used by blocking functions */
    bool blocking;                             /*!< Specifies if the current transfer is blocking */
    uint8_t bitCount;                          /*!< Number of bits per word */
    uint8_t txFlush;                           /*!< Used for flushing Tx buffer before ending a transmission */
} eio_uart_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize the EIO_UART driver
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_UART user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] state: Pointer to the EIO_UART driver context structure. The driver uses
 *                   this memory area for its internal logic. The application must make no
 *                   assumptions about the content of this structure, and must not free this
 *                   memory until the driver is de-initialized using EIO_UART_DRV_Deinit().
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_Init(uint32_t instance,
                           const eio_uart_user_config_t *userConfigPtr,
                           eio_uart_state_t *state);

/*!
 * @brief De-initialize the EIO_UART driver
 *        This function de-initializes the EIO_UART driver. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_Deinit(eio_uart_state_t *state);

/*!
 * @brief Set the baud rate and bit width for any subsequent UART communication
 *        This function sets the baud rate and bit width for the UART driver.
 *        Note that due to module limitation not any baud rate can be achieved. The driver
 *        will set a baud rate as close as possible to the requested baud rate, but there may
 *        still be substantial differences, for example if requesting a high baud rate while
 *        using a low-frequency EIO clock. The application should call
 *        EIO_UART_DRV_GetBaudRate() after EIO_UART_DRV_SetConfig() to check
 *        what baud rate was actually set.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] baudRate: the desired baud rate in hertz
 * @param[in] bitCount: number of bits per word
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_SetConfig(eio_uart_state_t * state,
                                uint32_t baudRate,
                                uint8_t bitCount);

/*!
 * @brief Get the currently configured baud rate
 *        This function returns the currently configured UART baud rate.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] baudRate: the current baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_GetBaudRate(eio_uart_state_t *state, uint32_t *baudRate);

/*!
 * @brief Perform a non-blocking UART transmission
 *        This function sends a block of data and returns immediately.
 *        The rest of the transmission is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_UART_DRV_GetTransmitStatus() function (if
 *        the driver is initialized in polling mode).
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_SendData(eio_uart_state_t *state,
                               const uint8_t *txBuff,
                               uint32_t txSize);

/*!
 * @brief Perform a blocking UART transmission
 *        This function sends a block of data and
 *        only returns when the transmission is complete.
 *
 * @param[in] state: pointer to the EIO_UART driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return  Error or success status returned by API
 */
status_t EIO_UART_DRV_SendDataBlocking(eio_uart_state_t *state,
                                       const uint8_t *txBuff,
                                       uint32_t txSize,
                                       uint32_t timeout);

/*!
 * @brief Perform a non-blocking UART reception
 *        This function receives a block of data and returns immediately.
 *        The rest of the transmission is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_UART_DRV_GetReceiveStatus() function (if
 *        the driver is initialized in polling mode).
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] rxBuff: pointer to the receive buffer
 * @param[in] rxSize: length in bytes of the data to be received
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_ReceiveData(eio_uart_state_t *state,
                                  uint8_t *rxBuff,
                                  uint32_t rxSize);

/*!
 * @brief Perform a blocking UART reception
 *        This function receives a block of data and
 *        only returns when the transmission is complete.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] rxBuff: pointer to the receive buffer
 * @param[in] rxSize: length in bytes of the data to be received
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_ReceiveDataBlocking(eio_uart_state_t *state,
        uint8_t * rxBuff,
        uint32_t rxSize,
        uint32_t timeout);

/*!
 * @brief Aborts a non-blocking UART transfer
 *        This function aborts a non-blocking UART transfer.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_TransferAbort(eio_uart_state_t *state);

/*!
 * @brief Get the status of the current non-blocking UART transfer
 *        This function returns the current status of a non-blocking UART transfer.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 *            @note In DMA mode, this param[in]eter may not be accurate, in case the transfer completes
 *             right after calling this function; in this edge-case, the param[in]eter will reflect
 *             the initial transfer size, due to automatic reloading of the major loop count
 *             in the DMA transfer descriptor.
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_GetStatus(eio_uart_state_t *state, uint32_t *bytesRemaining);

/*!
 * @brief Provide a buffer for receiving data
 *        This function can be used to provide a new buffer for receiving data to the driver.
 *        It can be called from the user callback when event STATUS_UART_RX_OVERRUN is reported.
 *        This way the reception will continue without interruption.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_SetRxBuffer(eio_uart_state_t *state,
                                  uint8_t *rxBuff,
                                  uint32_t rxSize);

/*!
 * @brief Provide a buffer for transmitting data
 *        This function can be used to provide a new buffer for transmitting data to the driver
 *        It can be called from the user callback when event STATUS_UART_TX_UNDERRUN is reported
 *        This way the transmission will continue without interruption.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] txBuff: pointer to the buffer containing transmit data
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_SetTxBuffer(eio_uart_state_t *state,
                                  const uint8_t *txBuff,
                                  uint32_t txSize);

/*!
 * @brief Returns default configuration structure for EIO_UART.
 *
 * @param[in] userConfigPtr: Pointer to the EIO_UART user configuration structure
 * @return none
 */
void EIO_UART_DRV_GetDefaultConfig(eio_uart_user_config_t *userConfigPtr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIO_UART_DRV_H */

/* =============================================  EOF  ============================================== */
