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
 * @file lin_drv.h
 *
 * @brief This file provides LIN driver definition and declaration.
 *
 */

#ifndef _LIN_DRV_H
#define _LIN_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "osif.h"

/* ============================================  Define  ============================================ */
#define SLAVE           (0UL)        /*!< LIN slave mode */
#define MASTER          (1UL)        /*!< LIN master mode */
#define MAKE_PARITY     (0UL)        /*!< Make parity for PID */
#define CHECK_PARITY    (1UL)        /*!< Check parity for PID */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Callback function for getting time interval in nanoseconds
 */
typedef uint32_t (* lin_timer_get_time_interval_t)(uint32_t *nanoSeconds);

/*!
 * @brief LIN callback function
 */
typedef void (* lin_callback_t)(uint8_t instance, void *linState);

/*!
 * @brief LIN break length for LIN master send.
 */
typedef enum
{
    BREAK_LENGTH_13BIT = 0U,         /*!< break length 13bit */
    BREAK_LENGTH_14BIT,
    BREAK_LENGTH_15BIT,
    BREAK_LENGTH_16BIT,
    BREAK_LENGTH_17BIT,
    BREAK_LENGTH_18BIT,
    BREAK_LENGTH_19BIT,
    BREAK_LENGTH_20BIT,
    BREAK_LENGTH_21BIT,
    BREAK_LENGTH_22BIT,
    BREAK_LENGTH_23BIT,
    BREAK_LENGTH_24BIT,
    BREAK_LENGTH_25BIT,
    BREAK_LENGTH_26BIT,
    BREAK_LENGTH_27BIT,
    BREAK_LENGTH_28BIT
} lin_break_length_t;

/*!
 * @brief LIN break threshold for LIN slave detect.
 */
typedef enum
{
    BREAK_THRESHOLD_10BIT = 0U,      /*!< break threshold 10bit */
    BREAK_THRESHOLD_11BIT            /*!< break threshold 11bit */
} lin_break_threshold_t;

/*!
 * @brief LIN event identifier enumeration define
 */
typedef enum {
    LIN_NO_EVENT             = 0x00U,    /*!< No event */
    LIN_WAKEUP_SIGNAL        = 0x01U,    /*!< Received a wakeup signal */
    LIN_BAUDRATE_ADJUSTED    = 0x02U,    /*!< Baudrate was adjusted to master's baudrate */
    LIN_RECV_BREAK_FIELD_OK  = 0x03U,    /*!< Received the correct break field */
    LIN_SYNC_OK              = 0x04U,    /*!< Sync byte is correct */
    LIN_SYNC_ERROR           = 0x05U,    /*!< Sync byte is error */
    LIN_PID_OK               = 0x06U,    /*!< PID correct */
    LIN_PID_ERROR            = 0x07U,    /*!< PID error */
    LIN_FRAME_ERROR          = 0x08U,    /*!< Frame error */
    LIN_READBACK_ERROR       = 0x09U,    /*!< Readback data is error */
    LIN_CHECKSUM_ERROR       = 0x0AU,    /*!< Checksum byte is error */
    LIN_TX_COMPLETED         = 0x0BU,    /*!< Send data completed */
    LIN_RX_COMPLETED         = 0x0CU,    /*!< Receive data completed */
    LIN_RX_OVERRUN           = 0x0DU     /*!< Receive overrun flag */
} lin_event_id_t;

/*!
 * @brief LIN node state enumeration define
 */
typedef enum {
    LIN_NODE_STATE_UNINIT               = 0x00U,    /*!< Uninitialized state */
    LIN_NODE_STATE_SLEEP_MODE           = 0x01U,    /*!< Sleep mode state */
    LIN_NODE_STATE_IDLE                 = 0x02U,    /*!< Idle state */
    LIN_NODE_STATE_SEND_BREAK_FIELD     = 0x03U,    /*!< Send break field state */
    LIN_NODE_STATE_RECV_SYNC            = 0x04U,    /*!< Receive sync byte state */
    LIN_NODE_STATE_SEND_PID             = 0x05U,    /*!< Send PID state */
    LIN_NODE_STATE_RECV_PID             = 0x06U,    /*!< Receive PID state */
    LIN_NODE_STATE_RECV_DATA            = 0x07U,    /*!< Receive data state */
    LIN_NODE_STATE_RECV_DATA_COMPLETED  = 0x08U,    /*!< Receive data completed state */
    LIN_NODE_STATE_SEND_DATA            = 0x09U,    /*!< Send data state */
    LIN_NODE_STATE_SEND_DATA_COMPLETED  = 0x0AU     /*!< Send data completed state */
} lin_node_state_t;

/*!
 * @brief LIN user configuration structure
 */
typedef struct {
    uint32_t baudRate;                          /*!< LIN bus baudrate */
    bool nodeFunction;                          /*!< Master or slave node */
    bool autobaudEnable;                        /*!< Autobaud function enable */
    lin_timer_get_time_interval_t timerGetTimeIntervalCallback; /*!< Nanosecond timer callback function */
    uint8_t *classicPID;                        /*!< PIDs list using classical checksum */
    uint8_t numOfClassicPID;                    /*!< Number of PIDs using classical checksum */
    lin_break_length_t breakLength;             /*!< LIN break length for master */
    lin_break_threshold_t breakThreshold;       /*!< LIN break detect threshold for slave */
    bool syncBaudEnable;                        /*!< Auto sync baudrate enable for slave */
} lin_user_config_t;

/*!
 * @brief LIN current state structure
 */
typedef struct {
    const uint8_t *txBuff;                      /*!< The buffer of transmitted data */
    uint8_t *rxBuff;                            /*!< The buffer of received data */
    uint8_t cntByte;                            /*!< The count size of bytes already transmitted or received */
    volatile uint8_t txSize;                    /*!< Byte size need to be transmitted */
    volatile uint8_t rxSize;                    /*!< Byte size need to be received */
    uint8_t checkSum;                           /*!< Checksum byte */
    volatile bool isTxBusy;                     /*!< Transmitting busy state */
    volatile bool isRxBusy;                     /*!< Receiving busy state */
    volatile bool isBusBusy;                    /*!< Bus busy state */
    volatile bool isTxBlocking;                 /*!< Transmit blocking state */
    volatile bool isRxBlocking;                 /*!< Receive blocking state */
    lin_callback_t Callback;                    /*!< Callback function */
    uint8_t currentId;                          /*!< Current ID */
    uint8_t currentPid;                         /*!< Current PID */
    volatile lin_event_id_t currentEventId;     /*!< Current event ID */
    volatile lin_node_state_t currentNodeState; /*!< Current node state */
    volatile uint32_t timeoutCounter;           /*!< Timeout counter value */
    volatile bool timeoutCounterFlag;           /*!< Timeout counter flag */
    volatile bool baudrateEvalEnable;           /*!< Baudrate evaluation process enable */
    volatile uint8_t fallingEdgeInterruptCount; /*!< Falling edge count of a sync byte */
    uint32_t linSourceClockFreq;                /*!< Source clock frequency */
    semaphore_t txCompleted;                    /*!< The flag for transmission complete */
    semaphore_t rxCompleted;                    /*!< The flag for reception complete */
} lin_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief LIN interrupt handler function
 *
 * @param[in] instance: LIN module instance
 * @return none
 */
void LIN_DRV_IRQHandler(uint8_t instance);

/*!
 * @brief Install LIN callback function
 *
 * @param[in] instance: LIN module instance
 * @param[in] callback: callback function pointer
 * @return former LIN callback function
 */
lin_callback_t LIN_DRV_InstallCallback(uint8_t instance, lin_callback_t callback);

/*!
 * @brief Enable LIN interrupt
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: enable interrupt successfully
 */
status_t LIN_DRV_EnableIRQ(uint8_t instance);

/*!
 * @brief Disable LIN interrupt
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: disable interrupt successfully
 */
status_t LIN_DRV_DisableIRQ(uint8_t instance);

/*!
 * @brief Initialize the LIN driver
 *
 * @param[in] instance: LIN module instance
 * @param[in] linConfig: pointer to LIN configuration
 * @param[in] linState: pointer to LIN state
 * @return initialize status
 *         - STATUS_SUCCESS: initialize successfully
 *         - STATUS_ERROR: initialize error
 */
status_t LIN_DRV_Init(uint8_t instance, lin_user_config_t *linConfig, lin_state_t *linState);

/*!
 * @brief Deinitialize the LIN module
 *
 * @param[in] instance: CAN module instance
 * @return none
 */
void LIN_DRV_Deinit(uint8_t instance);

/*!
 * @brief Get LIN default configuration
 *
 * @param[in] master: master or slave mode
 * @param[out] linConfig: point to LIN configuration
 * @return none
 */
void LIN_DRV_GetDefaultConfig(bool master, lin_user_config_t *linConfig);

/*!
 * @brief Send LIN frame data
 *
 * @param[in] instance: LIN module instance
 * @param[in] txBuf: pointer to transmit buffer
 * @param[in] txSize: transmit size
 * @return send frame data status
 *         - STATUS_SUCCESS: send successfully
 *         - STATUS_ERROR: send error
 *         - STATUS_BUSY: send busy
 */
status_t LIN_DRV_SendFrameData(uint8_t instance, const uint8_t *txBuf, uint8_t txSize);

/*!
 * @brief Send LIN frame data in a blocking way
 *
 * @param[in] instance: LIN module instance
 * @param[in] txBuf: pointer to transmit buffer
 * @param[in] txSize: transmit size
 * @param[in] timeout: timeout value
 * @return send frame data status
 *         - STATUS_SUCCESS: send successfully
 *         - STATUS_ERROR: send error
 *         - STATUS_BUSY: send busy
 *         - STATUS_TIMEOUT: send timeout
 */
status_t LIN_DRV_SendFrameDataBlocking(uint8_t instance, const uint8_t *txBuf, uint8_t txSize, uint32_t timeout);

/*!
 * @brief Get LIN transmit status
 *
 * @param[in] instance: LIN module instance
 * @param[out] leftBytes: bytes needed to transmit
 * @return get transmit status
 *         - STATUS_SUCCESS: get transmit successfully
 *         - STATUS_TIMEOUT: get transmit timeout
 *         - STATUS_BUSY: get transmit busy
 */
status_t LIN_DRV_GetTransmitStatus(uint8_t instance, uint8_t *leftBytes);

/*!
 * @brief Send LIN header for master
 *
 * @param[in] instance: LIN module instance
 * @param[in] id: frame identifier
 * @return send header status
 *         - STATUS_SUCCESS: send header successfully
 *         - STATUS_ERROR: send header error
 *         - STATUS_BUSY: send header busy
 */
status_t LIN_DRV_MasterSendHeader(uint8_t instance, uint8_t id);

/*!
 * @brief Abort LIN transmit frame data
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: abort transfer data successfully
 */
status_t LIN_DRV_AbortTransferData(uint8_t instance);

/*!
 * @brief Receive LIN frame data
 *
 * @param[in] instance: LIN module instance
 * @param[in] rxBuf: pointer to receive buffer
 * @param[in] rxSize: receive size
 * @return receive frame data status
 *         - STATUS_SUCCESS: receive successfully
 *         - STATUS_ERROR: receive error
 *         - STATUS_BUSY: receive busy
 */
status_t LIN_DRV_ReceiveFrameData(uint8_t instance, uint8_t *rxBuf, uint8_t rxSize);

/*!
 * @brief Receive LIN frame data in a blocking way
 *
 * @param[in] instance: LIN module instance
 * @param[in] rxBuf: pointer to receive buffer
 * @param[in] rxSize: receive size
 * @param[in] timeout: timeout value
 * @return receive frame data status
 *         - STATUS_SUCCESS: receive successfully
 *         - STATUS_ERROR: receive error
 *         - STATUS_BUSY: receive busy
 *         - STATUS_TIMEOUT: receive timeout
 */
status_t LIN_DRV_ReceiveFrameDataBlocking(uint8_t instance, uint8_t *rxBuf, uint8_t rxSize, uint32_t timeout);

/*!
 * @brief Get LIN receive status
 *
 * @param[in] instance: LIN module instance
 * @param[out] leftBytes: bytes needed to receive
 * @return get receive status
 *         - STATUS_SUCCESS: get receive status successfully
 *         - STATUS_TIMEOUT: get receive status timeout
 *         - STATUS_BUSY: get receive status busy
 */
status_t LIN_DRV_GetReceiveStatus(uint8_t instance, uint8_t *leftBytes);

/*!
 * @brief Goto LIN sleep mode
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: goto sleep mode successfully
 */
status_t LIN_DRV_GotoSleepMode(uint8_t instance);

/*!
 * @brief Goto LIN idle state
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: goto idle successfully
 */
status_t LIN_DRV_GotoIdleState(uint8_t instance);

/*!
 * @brief Send LIN wakeup signal
 *
 * @param[in] instance: LIN module instance
 * @return send wakeup signal status
 *         - STATUS_SUCCESS: send wakeup signal successfully
 *         - STATUS_BUSY: send wakeup signal busy
 */
status_t LIN_DRV_SendWakeupSignal(uint8_t instance);

/*!
 * @brief Get LIN current node state
 *
 * @param[in] instance: LIN module instance
 * @return LIN current node state
 */
lin_node_state_t LIN_DRV_GetCurrentNodeState(uint8_t instance);

/*!
 * @brief Set timeout counter for timer interrupt
 *
 * @param[in] instance: LIN module instance
 * @param[in] timeout: timeout counter
 * @return none
 */
void LIN_DRV_SetTimeoutCounter(uint8_t instance, uint32_t timeout);

/*!
 * @brief Timer interrupt callback function
 *
 * @param[in] instance: LIN module instance
 * @return none
 */
void LIN_DRV_TimeoutService(uint8_t instance);

/*!
 * @brief Process identifier parity
 *
 * @param[in] pid: ID or PID
 * @param[in] type: 0: make parity 1: check parity
 * @return PID or ID
 */
uint8_t LIN_DRV_ProcessParity(uint8_t pid, uint8_t type);

/*!
 * @brief Make checksum byte
 *
 * @param[in] buf: pointer to buffer
 * @param[in] size: buffer size
 * @param[in] pid: protected identifier
 * @return checksum byte
 */
uint8_t LIN_DRV_MakeChecksumByte(const uint8_t *buf, uint8_t size, uint8_t pid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LIN_DRV_H */

/* =============================================  EOF  ============================================== */
