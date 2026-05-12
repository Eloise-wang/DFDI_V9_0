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
 * @file uart_drv.h
 *
 * @brief This file provides uart integration functions interface.
 *
 */

#ifndef _UART_DRV_H
#define _UART_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "osif.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< Callback for UART module */
typedef void (*uart_callback_t)(void *driverState, uint32_t event, void *userData);

/*!
 * @brief Define the enum of the events which can trigger UART callback.
 */
typedef enum
{
    UART_EVENT_RX_FULL      = 0x00U,    /*!< Rx buffer is full */
    UART_EVENT_TX_EMPTY     = 0x01U,    /*!< Tx buffer is empty */
    UART_EVENT_END_TRANSFER = 0x02U,    /*!< The current transfer is ending */
    UART_EVENT_ERROR        = 0x03U,    /*!< An error occured during transfer */
    UART_EVENT_DATA_MATCH   = 0x04U,    /*!< Rx data match */
    UART_EVENT_IDLE_LINE    = 0x05U,    /*!< Idle line */
} uart_event_t;

/*!
 * @brief no-blocking transfer type.
 */
typedef enum
{
    UART_USING_DMA = 0U,    /*!< Use DMA to perform UART transfer */
    UART_USING_INTERRUPTS   /*!< Use interrupts to perform UART transfer */
} uart_transfer_type_t;

/*!
 * @brief UART number of bits in a character.
 */
typedef enum
{
    UART_7_BITS_PER_CHAR = 0x0U,  /*!< 7-bit data characters */
    UART_8_BITS_PER_CHAR = 0x1U,  /*!< 8-bit data characters */
    UART_9_BITS_PER_CHAR = 0x2U   /*!< 9-bit data characters */
} uart_bit_count_per_char_t;

/*!
 * @brief UART parity mode.
 */
typedef enum
{
    UART_PARITY_DISABLED = 0x0U, /*!< parity disabled */
    UART_PARITY_EVEN     = 0x1U, /*!< parity enabled, type even */
    UART_PARITY_ODD      = 0x2U  /*!< parity enabled, type odd */
} uart_parity_mode_t;

/*!
 * @brief UART number of stop bits.
 */
typedef enum
{
    UART_ONE_STOP_BIT = 0x0U, /*!< one stop bit */
    UART_TWO_STOP_BIT = 0x1U  /*!< two stop bits */
} uart_stop_bit_count_t;

/*!
 * @brief UART runtime state.
 */
typedef struct
{
    const uint8_t *txBuff;                     /*!< The buffer of transmit data */
    uint8_t *rxBuff;                           /*!< The buffer of received data */
    volatile uint32_t txSize;                  /*!< The remaining number of bytes to be transmitted */
    volatile uint32_t rxSize;                  /*!< The remaining number of bytes to be received */
    volatile bool isTxBusy;                    /*!< True if transmit is active */
    volatile bool isRxBusy;                    /*!< True if receive is active */
    volatile bool isTxBlocking;                /*!< True if transmit is blocking */
    volatile bool isRxBlocking;                /*!< True if receive is blocking */
    uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits(7/8/9 bits) in a char */
    uart_callback_t rxCallback;                /*!< Callback for data receive */
    void *rxCallbackParam;                     /*!< Receive callback parameter pointer.*/
    uart_callback_t txCallback;                /*!< Callback for data send */
    void *txCallbackParam;                     /*!< Transmit callback parameter pointer.*/
    uart_transfer_type_t transferType;         /*!< Type of UART transfer (interrupt or dma) */
    uint8_t rxDMAChannel;                      /*!< DMA channel for UART receive. */
    uint8_t txDMAChannel;                      /*!< DMA channel for UART transmit. */
    semaphore_t rxComplete;                    /*!< Synchronization object for blocking Rx timeout condition */
    semaphore_t txComplete;                    /*!< Synchronization object for blocking Tx timeout condition */
    volatile status_t transmitStatus;          /*!< Status of last transmit */
    volatile status_t receiveStatus;           /*!< Status of last receive */
    uint8_t clockConfigType;                   /*!< Clock config type, use for DMA RX */
} uart_state_t;

/*!
 * @brief UART configuration structure.
 */
typedef struct
{
    uint32_t baudRate;                         /*!< UART baud rate */
    uart_parity_mode_t parityMode;             /*!< parity mode, disabled, even, odd */
    uart_stop_bit_count_t stopBitCount;        /*!< number of stop bits, 1 or 2 stop bits */
    uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits in a character (7, 8 or 9) */
    uart_transfer_type_t transferType;         /*!< Type of UART transfer (interrupt or dma) */
    uint8_t rxDMAChannel;                      /*!< DMA rx channel for DMA transfer */
    uint8_t txDMAChannel;                      /*!< DMA tx channel for DMA transfer */
    uart_callback_t rxCallback;                /*!< Callback for data receive */
    void *rxCallbackParam;                     /*!< Receive callback parameter pointer.*/
    uart_callback_t txCallback;                /*!< Callback for data send */
    void *txCallbackParam;                     /*!< Transmit callback parameter pointer.*/
} uart_user_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Get UART module base address.
 *
 * @param[in] instance: UART module instance
 * @return UART module base address
 */
UART_Type * UART_DRV_GetBase(uint32_t instance);

/*!
 * @brief Initializes the UART configuration structure with default values.
 *
 * @param[out] uartUserConfig: Initializes a configuration structure received from the application
 *                             with default values
 * @return none
 */
void UART_DRV_GetDefaultConfig(uart_user_config_t *uartUserConfig);

/*!
 * @brief Initializes an UART operation instance.
 *
 * @param[in] instance: UART instance number
 * @param[in] uartUserConfig: user configuration structure of type uart_user_config_t
 * @param[in] uartStatePtr: pointer to the UART driver state structure
 * @return STATUS_SUCCESS if successful;
 *         STATUS_ERROR if an error occurred
 */
status_t UART_DRV_Init(uint32_t instance, uart_state_t *uartStatePtr, const uart_user_config_t *uartUserConfig);

/*!
 * @brief Shuts down the UART by disabling interrupts and transmitter/receiver.
 *
 * @param[in] instance: UART instance number
 * @return STATUS_SUCCESS if successful;
 *         STATUS_ERROR if an error occurred
 */
status_t UART_DRV_Deinit(uint32_t instance);

/*!
 * @brief Installs callback function for the UART receive.
 *        After a callback is installed, it bypasses part of the UART IRQHandler logic.
 *        Therefore, the callback needs to handle the indexes of txBuff and txSize.
 *
 * @param[in] instance: The UART instance number.
 * @param[in] function: The UART receive callback function.
 * @param[in] rxBuff: The receive buffer used inside IRQHandler.
 *            This buffer must be kept as long as the callback is alive.
 * @param[in] callbackParam: The UART receive callback parameter pointer.
 * @return Former UART receive callback function pointer.
 */
uart_callback_t UART_DRV_InstallRxCallback(uint32_t instance, uart_callback_t function, void *callbackParam);

/*!
 * @brief Installs callback function for the UART transmit.
 *        After a callback is installed, it bypasses part of the UART IRQHandler logic.
 *        Therefore, the callback needs to handle the indexes of txBuff and txSize.
 *
 * @param[in] instance: The UART instance number
 * @param[in] function: The UART transmit callback function
 * @param[in] txBuff: The transmit buffer used inside IRQHandler.
 *            This buffer must be kept as long as the callback is alive
 * @param[in] callbackParam: The UART transmit callback parameter pointer
 * @return Former UART transmit callback function pointer.
 */
uart_callback_t UART_DRV_InstallTxCallback(uint32_t instance, uart_callback_t function, void *callbackParam);

/*!
 * @brief Sends data out through the UART module using a blocking method.
 *        Blocking means that the function does not return until the transmission is complete.
 *
 * @param[in] instance: UART instance number
 * @param[in] txBuff: source buffer containing 8-bit data chars to send
 * @param[in] txSize: the number of bytes to send
 * @param[in] timeout: timeout value in milliseconds
 * @return STATUS_SUCCESS if successful;
 *         STATUS_TIMEOUT if the timeout was reached;
 *         STATUS_BUSY if the resource is busy;
 *         STATUS_ERROR if an error occurred
 */
status_t UART_DRV_SendDataBlocking(uint32_t instance, const uint8_t *txBuff, uint32_t txSize, uint32_t timeout);

/*!
 * @brief UART send data using polling mode
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: The tx data buffer pointer
 * @param[in] txSize: The tx data buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
status_t UART_DRV_SendDataPolling(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);

/*!
 * @brief UART send data using non-blocking method (interrupt or DMA)
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: The tx data buffer pointer
 * @param[in] txSize: The tx data buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
status_t UART_DRV_SendData(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);

/*!
 * @brief Get transmit status
 *
 * @param[in] instance: UART instance
 * @param[out] bytesRemaining: Remaining bytes to be sent
 * @return Transmit status
 */
status_t UART_DRV_GetTransmitStatus(uint32_t instance, uint32_t *bytesRemaining);

/*!
 * @brief Terminates an non-blocking UART transmission early.
 *
 * @param[in] instance: UART instance
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_AbortSendingData(uint32_t instance);

/*!
 * @brief UART receive data using blocking method, which will not return until
 *        the receive is complete.
 *
 * @param[in] instance: UART instance
 * @param[out] rxBuff: The rx data buffer pointer
 * @param[in] rxSize: The rx data buffer bytes size
 * @param[in] timeout：Configurate timeout
 * @return The receive status
 */
status_t UART_DRV_ReceiveDataBlocking(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize, uint32_t timeout);

/*!
 * @brief UART receive data using polling mode.
 *
 * @param[in] instance: UART instance
 * @param[out] rxBuff: The rx data buffer pointer
 * @param[in] rxSize: The rx data buffer bytes size
 * @return Receive status
 */
status_t UART_DRV_ReceiveDataPolling(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);

/*!
 * @brief UART receive data using non-blocking method(interrupt or DMA).
 *
 * @param[in] instance: UART instance
 * @param[out] rxBuff: The rx data buffer pointer
 * @param[in] rxSize: The rx data buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
status_t UART_DRV_ReceiveData(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);

/*!
 * @brief Get receive status.
 *
 * @param[in] instance: UART instance
 * @param[out] bytesRemaining: Remaining bytes to receive
 * @return Receive status
 */
status_t UART_DRV_GetReceiveStatus(uint32_t instance, uint32_t *bytesRemaining);

/*!
 * @brief Terminates a non-blocking receive early.
 *
 * @param[in] instance: UART instance
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_AbortReceivingData(uint32_t instance);

/*!
 * @brief Set UART baud rate.
 *
 * @param[in] instance: UART instance
 * @param[in] desiredBaudRate: UART state pointer
 * @return STATUS_BUSY, STATUS_ERROR or STATUS_SUCCESS
 */
status_t UART_DRV_SetBaudRate(uint32_t instance, uint32_t desiredBaudRate);

/*!
 * @brief Get UART baud rate.
 *
 * @param[in] instance: UART instance
 * @param[out] configuredBaudRate: Return the UART configured baud rate
 * @return none
 */
void UART_DRV_GetBaudRate(uint32_t instance, uint32_t *configuredBaudRate);

/*!
 * @brief Set the driver internal tx buffer.
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: The tx data buffer pointer
 * @param[in] txSize: The tx data buffer bytes size
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_SetTxBuffer(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);

/*!
 * @brief Set the driver internal rx buffer.
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: The rx data buffer pointer
 * @param[in] txSize: The rx data buffer bytes size
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_SetRxBuffer(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _UART_DRV_H */

/* =============================================  EOF  ============================================== */
