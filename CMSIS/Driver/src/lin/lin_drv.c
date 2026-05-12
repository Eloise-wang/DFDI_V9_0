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
 * @file lin_drv.c
 *
 * @brief This file provides LIN integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "../uart/uart_irq.h"
#include "../uart/uart_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */
#define LIN_SYNC_DATA  (0x55UL)       /* LIN SYNC DATA */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of UART base address */
static UART_Type * const s_linUartBase[UART_INSTANCE_MAX] = UART_BASE_PTRS;

/* Table of UART IRQs */
static IRQn_Type s_linUartIrq[UART_INSTANCE_MAX] = UART_IRQS;

/* Table of UART clock names */
static const clock_names_t s_linUartClkName[UART_INSTANCE_MAX] = UART_CLOCK_NAMES;

/* Table of LIN state structure pointer */
static lin_state_t *s_linState[UART_INSTANCE_MAX] = {NULL};

/* Table of LIN configuration structure pointer */
static lin_user_config_t *s_linConfig[UART_INSTANCE_MAX] = {NULL};

/* Table of LIN wakeup signal */
static uint8_t s_linWakeupSignal[UART_INSTANCE_MAX] = {0U};

/* ====================================  Functions declaration  ===================================== */
static uint8_t LIN_MakeChecksum(uint8_t instance, const uint8_t *buf, uint8_t size, uint8_t pid);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Process break detect
 *
 * @param[in] instance: LIN module instance
 * @param[in] linState: pointer to LIN state
 * @return none
 */
static void LIN_ProcessBreakDetect(uint8_t instance, lin_state_t *linState)
{
    DEVICE_ASSERT(linState != NULL);

    lin_user_config_t *linConfig = s_linConfig[instance];

    if ((bool)MASTER == linConfig->nodeFunction)  /* For master mode */
    {
        if (LIN_NODE_STATE_SEND_BREAK_FIELD == linState->currentNodeState)
        {
            /* If the master detect break correctly that send by itself, then send sync byte */
            linState->isBusBusy = true;
            linState->currentNodeState = LIN_NODE_STATE_SEND_PID;

            UART_Putchar(s_linUartBase[instance], (uint8_t)LIN_SYNC_DATA);
        }
    }
    else  /* For slave mode */
    {
        /* If the slave detect break, then receive sync byte */
        linState->isBusBusy = true;
        linState->currentEventId = LIN_RECV_BREAK_FIELD_OK;
        if (linState->Callback != NULL)
        {
            linState->Callback(instance, linState);
        }
        if (linConfig->syncBaudEnable)
        {
            /* If enable auto sync baudrate function, the sync byte 0x55 can't be received */
            linState->currentNodeState = LIN_NODE_STATE_RECV_PID;
        }
        else
        {
            linState->currentNodeState = LIN_NODE_STATE_RECV_SYNC;
        }
    }
}

/*!
 * @brief Process receive PID
 *
 * @param[in] instance: LIN module instance
 * @param[in] linState: pointer to LIN state
 * @return none
 */
static inline void LIN_ProcessPid(uint32_t instance, lin_state_t *linState)
{
    linState->currentEventId = LIN_PID_OK;
    if (true == linState->isRxBlocking)
    {
        linState->currentNodeState = LIN_NODE_STATE_RECV_DATA;
        linState->isBusBusy = true;
        linState->isRxBusy = true;
    }
    else
    {
        linState->isBusBusy = false;
        if (linState->Callback != NULL)
        {
            linState->Callback(instance, linState);
        }
    }
}

/*!
 * @brief Process receive PID error
 *
 * @param[in] instance: LIN module instance
 * @param[in] linState: pointer to LIN state
 * @return none
 */
static inline void LIN_ProcessPidError(uint32_t instance, lin_state_t *linState)
{
    linState->currentEventId = LIN_PID_ERROR;
    if (linState->Callback != NULL)
    {
        linState->Callback(instance, linState);
    }

    (void)LIN_DRV_GotoIdleState(instance);
}

/*!
 * @brief Process frame header
 *
 * @param[in] instance: LIN module instance
 * @param[in] linState: pointer to LIN state
 * @param[in] rxByte: received byte
 * @return none
 */
static void LIN_ProcessFrameHeader(uint32_t instance, lin_state_t *linState, uint8_t rxByte)
{
    DEVICE_ASSERT(linState != NULL);

    switch (linState->currentNodeState)
    {
    case LIN_NODE_STATE_RECV_SYNC:
        if (LIN_SYNC_DATA == rxByte)
        {
            /* If receive sync byte correctly, then receive the pid byte */
            linState->currentEventId = LIN_SYNC_OK;
            linState->currentNodeState = LIN_NODE_STATE_RECV_PID;
        }
        else
        {
            /* If receive sync byte error, then go to idle state */
            linState->currentEventId = LIN_SYNC_ERROR;
            if (linState->Callback != NULL)
            {
                linState->Callback(instance, linState);
            }

            (void)LIN_DRV_GotoIdleState(instance);
        }
        break;

    case LIN_NODE_STATE_SEND_PID:
        if (LIN_SYNC_DATA == rxByte)
        {
            /* If the master receive sync byte correctly that send by itself, then send pid byte */
            linState->currentNodeState = LIN_NODE_STATE_RECV_PID;
            UART_Putchar(s_linUartBase[instance], linState->currentPid);
        }
        else
        {
            /* If receive sync byte error, then go to idle state */
            linState->currentEventId = LIN_SYNC_ERROR;
            linState->isBusBusy = false;
            linState->currentNodeState = LIN_NODE_STATE_IDLE;
            if (linState->Callback != NULL)
            {
                linState->Callback(instance, linState);
            }
        }
        break;

    case LIN_NODE_STATE_RECV_PID:
        if ((bool)MASTER == s_linConfig[instance]->nodeFunction)  /* For master mode */
        {
            if (rxByte == linState->currentPid)
            {
                /* If the master receive pid byte correctly that send by itself, then receive or send data */
                LIN_ProcessPid(instance, linState);
            }
            else
            {
                /* If receive pid byte error */
                linState->currentEventId = LIN_PID_ERROR;
                linState->isBusBusy = false;
                linState->currentNodeState = LIN_NODE_STATE_IDLE;
                if (linState->Callback != NULL)
                {
                    linState->Callback(instance, linState);
                }
            }
        }
        else  /* For slave mode */
        {
            /* Check parity bits of pid */
            linState->currentId = LIN_DRV_ProcessParity(rxByte, (uint8_t)CHECK_PARITY);
            linState->currentPid = rxByte;
            if (linState->currentId != 0xFFU)
            {
                /* If the slave receive pid byte correctly, run callback function, then receive or send data */
                LIN_ProcessPid(instance, linState);
            }
            else
            {
                /* If receive pid byte error, then go to idle state */
                LIN_ProcessPidError(instance, linState);
            }
        }
        break;

    default:
        break;
    }
}

/*!
 * @brief Process receive frame data done
 *
 * @param[in] instance: LIN module instance
 * @param[in] linState: pointer to LIN state
 * @return none
 */
static inline void LIN_ProcessReceiveFrameDone(uint32_t instance, lin_state_t *linState)
{
    linState->currentEventId = LIN_RX_COMPLETED;
    linState->currentNodeState = LIN_NODE_STATE_RECV_DATA_COMPLETED;
    if (linState->Callback != NULL)
    {
        linState->Callback(instance, linState);
    }
    if (false == linState->isRxBlocking)
    {
        linState->isBusBusy = false;
        linState->isRxBusy = false;
        if (linState->currentNodeState != LIN_NODE_STATE_SLEEP_MODE)
        {
            (void)LIN_DRV_GotoIdleState(instance);
        }
    }
    else
    {
        /* Post receive complete flag */
        (void)OSIF_SemaPost(&linState->rxCompleted);
    }
}

/*!
 * @brief Process receive frame data
 *
 * @param[in] instance: LIN module instance
 * @param[in] linState: pointer to LIN state
 * @param[in] rxByte: received byte
 * @return none
 */
static void LIN_ProcessReceiveFrameData(uint32_t instance, lin_state_t *linState, uint8_t rxByte)
{
    DEVICE_ASSERT(linState != NULL);

    /* Save frame data or checksum */
    if (linState->rxSize > (linState->cntByte + 1U))
    {
        *(linState->rxBuff) = rxByte;
        linState->rxBuff++;
    }
    else
    {
        linState->checkSum = rxByte;
    }

    linState->cntByte++;
    /* Check recieve completely */
    if (linState->cntByte == linState->rxSize)
    {
        linState->rxBuff -= linState->rxSize - 1U;
        /* Check the checksum */
        if (LIN_MakeChecksum(instance, linState->rxBuff, linState->rxSize - 1U, linState->currentPid) == \
            linState->checkSum)
        {
            /* If the checksum byte is correct, run callback function, then go to idle state */
            LIN_ProcessReceiveFrameDone(instance, linState);
        }
        else
        {
            /* If the checksum byte is error, then go to idle state */
            linState->currentEventId = LIN_CHECKSUM_ERROR;
            if (linState->Callback != NULL)
            {
                linState->Callback(instance, linState);
            }
            linState->isRxBusy = false;

            (void)LIN_DRV_GotoIdleState(instance);
        }
    }
}

/*!
 * @brief Process send frame data
 *
 * @param[in] instance: LIN module instance
 * @param[in] linState: pointer to LIN state
 * @param[in] rxByte: received byte
 * @return none
 */
static void LIN_ProcessSendFrameData(uint32_t instance, lin_state_t *linState, uint8_t rxByte)
{
    DEVICE_ASSERT(linState != NULL);

    uint8_t leftSize = (uint8_t)(linState->txSize - linState->cntByte);

    /* Check recieve byte that send by itself */
    if (((1U == leftSize) && (linState->checkSum != rxByte)) || \
        ((leftSize != 1U) && (*linState->txBuff != rxByte)))
    {
        /* If the receive byte is error, then go to idle state */
        linState->currentEventId = LIN_READBACK_ERROR;
        if (linState->Callback != NULL)
        {
            linState->Callback(instance, linState);
        }
        if (false == linState->isTxBlocking)
        {
            linState->isTxBusy = false;
            (void)LIN_DRV_GotoIdleState(instance);
        }
        return;
    }
    else
    {
        linState->txBuff++;
        linState->cntByte++;
    }

    if (linState->cntByte < linState->txSize)
    {
        /* If sending data is not complete, continue send data or checksum */
        if (1U == (linState->txSize - linState->cntByte))
        {
            UART_Putchar(s_linUartBase[instance], linState->checkSum);
        }
        else
        {
            UART_Putchar(s_linUartBase[instance], *linState->txBuff);
        }
    }
    else
    {
        /* If sending data is completed, run callback function, then go to idle state */
        linState->currentEventId = LIN_TX_COMPLETED;
        linState->currentNodeState = LIN_NODE_STATE_SEND_DATA_COMPLETED;
        if (linState->Callback != NULL)
        {
            linState->Callback(instance, linState);
        }
        if (false == linState->isTxBlocking)
        {
            linState->isTxBusy = false;
            if (linState->currentNodeState != LIN_NODE_STATE_SLEEP_MODE)
            {
                (void)LIN_DRV_GotoIdleState(instance);
            }
        }
        else
        {
            /* Post transmit complete flag */
            (void)OSIF_SemaPost(&linState->txCompleted);
        }
    }
}

/*!
 * @brief Check wakeup signal
 *
 * @param[in] instance: LIN module instance
 * @param[in] linState: pointer to LIN state
 * @param[in] rxByte: received byte
 * @return none
 */
static void LIN_ProcessFrame(uint32_t instance, lin_state_t *linState, uint8_t rxByte)
{
    DEVICE_ASSERT(linState != NULL);

    switch (linState->currentNodeState)
    {
    case LIN_NODE_STATE_RECV_SYNC:
    case LIN_NODE_STATE_SEND_PID:
    case LIN_NODE_STATE_RECV_PID:
        LIN_ProcessFrameHeader(instance, linState, rxByte);
        break;

    case LIN_NODE_STATE_RECV_DATA:
        LIN_ProcessReceiveFrameData(instance, linState, rxByte);
        break;

    case LIN_NODE_STATE_SEND_DATA:
        LIN_ProcessSendFrameData(instance, linState, rxByte);
        break;

    default:
        break;
    }
}

/*!
 * @brief LIN interrupt handler function
 *
 * @param[in] instance: LIN module instance
 * @return none
 */
void LIN_IRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_state_t *linState = s_linState[instance];
    UART_Type *base = s_linUartBase[instance];
    uint32_t lsr0 = UART_GetLSR0(base), lsr1 = UART_GetLSR1(base);
    uint8_t rxByte = 0U;

    /* Check LIN wakeup flag */
    if (lsr1 & UART_LSR1_LINWAK_Msk)
    {
        (void)UART_ClearStatusFlag(base, UART_LIN_WAKEUP);

        if (LIN_NODE_STATE_SLEEP_MODE == linState->currentNodeState)
        {
            linState->currentEventId = LIN_WAKEUP_SIGNAL;
            if (linState->Callback != NULL)
            {
                linState->Callback(instance, linState);
            }

            (void)LIN_DRV_GotoIdleState(instance);
        }
    }

    /* Check UART frame error flag */
    if (lsr0 & UART_LSR0_FE_Msk)
    {
        (void)UART_ClearStatusFlag(base, UART_FRAME_ERR);

        linState->currentEventId = LIN_FRAME_ERROR;
        if ((LIN_NODE_STATE_SEND_DATA == linState->currentNodeState) || \
            (LIN_NODE_STATE_RECV_DATA == linState->currentNodeState))
        {
            if (linState->Callback != NULL)
            {
                linState->Callback(instance, linState);
            }
        }

        (void)LIN_DRV_GotoIdleState(instance);
    }
    /* Check UART overrun flag */
    else if (lsr0 & UART_LSR0_OE_Msk)
    {
        (void)UART_ClearStatusFlag(base, UART_RX_OVERRUN);

        linState->currentEventId = LIN_RX_OVERRUN;
        if (linState->Callback != NULL)
        {
            linState->Callback(instance, linState);
        }
    }
    /* Check LIN sync error flag */
    else if (lsr1 & UART_LSR1_SYNERR_Msk)
    {
        (void)UART_ClearStatusFlag(base, UART_LIN_SYNC_ERR);

        linState->currentEventId = LIN_SYNC_ERROR;
        if (linState->Callback != NULL)
        {
            linState->Callback(instance, linState);
        }

        (void)LIN_DRV_GotoIdleState(instance);
    }
    else
    {
    }

    /* Check LIN received break flag */
    if (lsr1 & UART_LSR1_FBRK_Msk)
    {
        (void)UART_ClearStatusFlag(base, UART_LIN_BREAK);
        LIN_ProcessBreakDetect(instance, linState);
    }
    /* Check LIN received data flag */
    else if (lsr0 & UART_LSR0_DR_Msk)
    {
        UART_Getchar(base, &rxByte);

        LIN_ProcessFrame(instance, linState, rxByte);
    }
    else
    {
    }
}

/*!
 * @brief Install LIN callback function
 *
 * @param[in] instance: LIN module instance
 * @param[in] callback: callback function pointer
 * @return former LIN callback function
 */
lin_callback_t LIN_DRV_InstallCallback(uint8_t instance, lin_callback_t callback)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_state_t *linState = s_linState[instance];
    lin_callback_t formerCallback = linState->Callback;

    linState->Callback = callback;

    return formerCallback;
}

/*!
 * @brief Enable LIN interrupt
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: enable interrupt successfully
 */
status_t LIN_DRV_EnableIRQ(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    NVIC_EnableIRQ(s_linUartIrq[instance]);

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable LIN interrupt
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: disable interrupt successfully
 */
status_t LIN_DRV_DisableIRQ(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    NVIC_DisableIRQ(s_linUartIrq[instance]);
    NVIC_ClearPendingIRQ(s_linUartIrq[instance]);

    return STATUS_SUCCESS;
}

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
status_t LIN_DRV_Init(uint8_t instance, lin_user_config_t *linConfig, lin_state_t *linState)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(linConfig != NULL);
    DEVICE_ASSERT(linState != NULL);

    status_t ret = STATUS_SUCCESS;
    uint32_t srcClkFreq = 0U, linCtrl;
    UART_Type *base = s_linUartBase[instance];

    (void)CKGEN_DRV_GetFreq(s_linUartClkName[instance], &srcClkFreq);

    DEVICE_ASSERT(srcClkFreq > 0U);
    DEVICE_ASSERT(NULL == s_linState[instance]);

    s_linConfig[instance] = linConfig;
    s_linState[instance] = linState;
    linState->linSourceClockFreq = srcClkFreq;

    /* Initialize UART */
    UART_Init(instance);

    if (OSIF_SemaCreate(&linState->txCompleted, 0) != STATUS_SUCCESS)
    {
        ret = STATUS_ERROR;
    }
    else if (OSIF_SemaCreate(&linState->rxCompleted, 0) != STATUS_SUCCESS)
    {
        (void)OSIF_SemaDestroy(&linState->txCompleted);
        ret = STATUS_ERROR;
    }
    else
    {
        /* Configure UART */
        (void)UART_DRV_SetBaudRate(instance, linConfig->baudRate);
        UART_SetBitCountPerChar(base, UART_8_BITS_PER_CHAR);
        UART_SetParityMode(base, UART_PARITY_DISABLED);
        UART_SetStopBitCount(base, UART_ONE_STOP_BIT);
        UART_SetFIFO(base, true);
        UART_SetTransmitterCmd(base, true);
        UART_SetReceiverCmd(base, true);

        /* Initialize LIN */
        if ((bool)MASTER == linConfig->nodeFunction)
        {
            linCtrl = UART_LINCR_LINEN_Msk | UART_LINCR_LBRKDL_Msk;
            UART_SetLINBreakLength(base, linConfig->breakLength);
        }
        else
        {
            linCtrl = UART_LINCR_LINEN_Msk;
            if (BREAK_THRESHOLD_11BIT == linConfig->breakThreshold)
            {
                linCtrl |= UART_LINCR_LBRKDL_Msk;
            }
            if (linConfig->syncBaudEnable)
            {
                linCtrl |= UART_LINCR_LABAUDEN_Msk;
            }
        }
        UART_SetLINCtrl(base, (uint8_t)linCtrl);

        linState->currentNodeState = LIN_NODE_STATE_IDLE;
        linState->isTxBusy = false;
        linState->isRxBusy = false;
        linState->isBusBusy = false;
        linState->isRxBlocking = false;
        linState->isTxBlocking = false;
        linState->timeoutCounterFlag = false;
        linState->timeoutCounter = 0U;

        if (linConfig->baudRate > 10000U)
        {
            s_linWakeupSignal[instance] = 0x80U;
        }
        else
        {
            s_linWakeupSignal[instance] = 0xF8U;
        }

        /* Initialize interrupt */
        if (linConfig->syncBaudEnable)
        {
            UART_SetIntMode(base, UART_INT_LIN_SYNC_ERR, true);
        }
        UART_SetIntMode(base, UART_INT_LIN_BREAK, true);
        UART_SetIntMode(base, UART_INT_RX_NOT_EMPTY, true);
        UART_SetIntMode(base, UART_INT_FRAME_ERR, true);
        UART_SetIntMode(base, UART_INT_RX_OVERRUN, true);

        g_uartIsr[instance] = (uart_isr_t)LIN_IRQHandler;
        (void)LIN_DRV_EnableIRQ(instance);
    }

    return ret;
}

/*!
 * @brief Deinitialize the LIN module
 *
 * @param[in] instance: CAN module instance
 * @return none
 */
void LIN_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_state_t *linState = s_linState[instance];
    UART_Type *base = s_linUartBase[instance];

    DEVICE_ASSERT(linState != NULL);
    DEVICE_ASSERT(linState->linSourceClockFreq > 0U);

    /* Disable UART transmitter and receiver */
    UART_SetTransmitterCmd(base, false);
    UART_SetReceiverCmd(base, false);

    (void)OSIF_SemaDestroy(&linState->txCompleted);
    (void)OSIF_SemaDestroy(&linState->rxCompleted);

    /* Disable UART interrupt */
    (void)LIN_DRV_DisableIRQ(instance);
    UART_DeInit(instance);

    linState->currentNodeState = LIN_NODE_STATE_UNINIT;
    s_linState[instance] = NULL;
}

/*!
 * @brief Get LIN default configuration
 *
 * @param[in] master: master or slave mode
 * @param[out] linConfig: point to LIN configuration
 * @return none
 */
void LIN_DRV_GetDefaultConfig(bool master, lin_user_config_t *linConfig)
{
    DEVICE_ASSERT(linConfig != NULL);

    linConfig->baudRate = 19200U;
    linConfig->autobaudEnable = false;
    linConfig->timerGetTimeIntervalCallback = NULL;
    if (true == master)
    {
        linConfig->nodeFunction = (bool)MASTER;
        linConfig->breakLength = BREAK_LENGTH_13BIT;
    }
    else
    {
        linConfig->nodeFunction = (bool)SLAVE;
        linConfig->breakThreshold = BREAK_THRESHOLD_11BIT;
        linConfig->syncBaudEnable = true;
    }
}

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
status_t LIN_DRV_SendFrameData(uint8_t instance, const uint8_t *txBuf, uint8_t txSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(txBuf != NULL);

    status_t ret = STATUS_SUCCESS;
    lin_state_t *linState = s_linState[instance];

    /* Check transmit size and node state */
    if ((txSize > 8U) || (0U == txSize) || (LIN_NODE_STATE_SLEEP_MODE == linState->currentNodeState))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        /* Check bus busy state */
        if (linState->isBusBusy)
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Prepare transmit data and checksum */
            linState->checkSum = LIN_MakeChecksum(instance, txBuf, txSize, linState->currentPid);
            linState->txBuff = txBuf;
            linState->txSize = (uint8_t)(txSize + 1U);
            linState->cntByte = 0U;
            linState->currentNodeState = LIN_NODE_STATE_SEND_DATA;
            linState->currentEventId = LIN_NO_EVENT;
            linState->isBusBusy = true;
            linState->isTxBusy = true;

            /* Start transmitting data */
            UART_Putchar(s_linUartBase[instance], *linState->txBuff);
        }
    }

    return ret;
}

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
status_t LIN_DRV_SendFrameDataBlocking(uint8_t instance, const uint8_t *txBuf, uint8_t txSize, uint32_t timeout)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(txBuf != NULL);

    status_t ret = STATUS_SUCCESS;
    lin_state_t *linState = s_linState[instance];

    /* Check transmit size and node state */
    if ((txSize > 8U) || (0U == txSize) || (LIN_NODE_STATE_SLEEP_MODE == linState->currentNodeState))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        /* Check bus busy state */
        if (linState->isBusBusy)
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Prepare transmit data and checksum */
            linState->checkSum = LIN_MakeChecksum(instance, txBuf, txSize, linState->currentPid);
            linState->txBuff = txBuf;
            linState->txSize = (uint8_t)(txSize + 1U);
            linState->cntByte = 0U;
            linState->currentNodeState = LIN_NODE_STATE_SEND_DATA;
            linState->currentEventId = LIN_NO_EVENT;
            linState->isBusBusy = true;
            linState->isTxBusy = true;
            linState->isTxBlocking = true;

            /* Start transmitting data */
            UART_Putchar(s_linUartBase[instance], *linState->txBuff);

            /* Wait transmission complete */
            ret = OSIF_SemaWait(&linState->txCompleted, timeout);

            linState->isTxBlocking = false;
            linState->isTxBusy = false;
            (void)LIN_DRV_GotoIdleState(instance);
        }
    }

    return ret;
}

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
status_t LIN_DRV_GetTransmitStatus(uint8_t instance, uint8_t *leftBytes)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(leftBytes != NULL);

    status_t ret = STATUS_SUCCESS;
    lin_state_t *linState = s_linState[instance];

    *leftBytes = (uint8_t)(linState->txSize - linState->cntByte);

    if ((LIN_NO_EVENT == linState->currentEventId) && (*leftBytes != 0U))
    {
        if (false == linState->timeoutCounterFlag)
        {
            ret = STATUS_BUSY;
        }
        else
        {
            ret = STATUS_TIMEOUT;
        }
    }

    return ret;
}

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
status_t LIN_DRV_MasterSendHeader(uint8_t instance, uint8_t id)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    status_t ret = STATUS_SUCCESS;
    lin_state_t *linState = s_linState[instance];

    /* Check node state and id */
    if (((bool)SLAVE == s_linConfig[instance]->nodeFunction) || (id > 0x3FU) || \
        (LIN_NODE_STATE_SLEEP_MODE == linState->currentNodeState))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        /* Check if the LIN bus is busy */
        if (linState->isBusBusy)
        {
            ret = STATUS_BUSY;
        }
        else
        {
            linState->currentId = id;
            linState->currentPid = LIN_DRV_ProcessParity(id, (uint8_t)MAKE_PARITY);
            linState->currentNodeState = LIN_NODE_STATE_SEND_BREAK_FIELD;
            linState->currentEventId = LIN_NO_EVENT;
            linState->isBusBusy = true;

            /* Send break */
            UART_SendLINBreak(s_linUartBase[instance]);
        }
    }

    return ret;
}

/*!
 * @brief Abort LIN transmit frame data
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: abort transfer data successfully
 */
status_t LIN_DRV_AbortTransferData(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_state_t *linState = s_linState[instance];

    (void)LIN_DRV_GotoIdleState(instance);

    linState->isTxBusy = false;
    linState->isRxBusy = false;

    return STATUS_SUCCESS;
}

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
status_t LIN_DRV_ReceiveFrameData(uint8_t instance, uint8_t *rxBuf, uint8_t rxSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuf != NULL);

    status_t ret = STATUS_SUCCESS;
    lin_state_t *linState = s_linState[instance];

    /* Check receive size and node state */
    if ((rxSize > 8U) || (0U == rxSize) || (LIN_NODE_STATE_SLEEP_MODE == linState->currentNodeState))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        /* Check bus busy state */
        if (linState->isBusBusy)
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Prepare receive data */
            linState->rxBuff = rxBuf;
            linState->rxSize = (uint8_t)(rxSize + 1U);
            linState->cntByte = 0U;
            linState->currentNodeState = LIN_NODE_STATE_RECV_DATA;
            linState->currentEventId = LIN_NO_EVENT;
            linState->isBusBusy = true;
            linState->isRxBusy = true;
            linState->isRxBlocking = false;
        }
    }

    return ret;
}

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
status_t LIN_DRV_ReceiveFrameDataBlocking(uint8_t instance, uint8_t *rxBuf, uint8_t rxSize, uint32_t timeout)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuf != NULL);

    status_t ret = STATUS_SUCCESS;
    lin_state_t *linState = s_linState[instance];

    /* Check receive size and node state */
    if ((rxSize > 8U) || (0U == rxSize) || (LIN_NODE_STATE_SLEEP_MODE == linState->currentNodeState))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        /* Check bus busy state */
        if (linState->isBusBusy)
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Prepare receive data */
            linState->rxBuff = rxBuf;
            linState->rxSize = (uint8_t)(rxSize + 1U);
            linState->cntByte = 0U;
            linState->currentNodeState = LIN_NODE_STATE_RECV_DATA;
            linState->currentEventId = LIN_NO_EVENT;
            linState->isRxBlocking = true;

            /* Wait receive complete */
            ret = OSIF_SemaWait(&linState->rxCompleted, timeout);

            if (linState->currentNodeState != LIN_NODE_STATE_SLEEP_MODE)
            {
                linState->currentNodeState = LIN_NODE_STATE_IDLE;
            }
            linState->isRxBlocking = false;
            linState->isBusBusy = false;
            linState->isRxBusy = false;
        }
    }

    return ret;
}

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
status_t LIN_DRV_GetReceiveStatus(uint8_t instance, uint8_t *leftBytes)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(leftBytes != NULL);

    status_t ret = STATUS_SUCCESS;
    lin_state_t *linState = s_linState[instance];

    *leftBytes = (uint8_t)(linState->rxSize - linState->cntByte);

    if ((LIN_NO_EVENT == linState->currentEventId) && (*leftBytes != 0U))
    {
        if (false == linState->timeoutCounterFlag)
        {
            ret = STATUS_BUSY;
        }
        else
        {
            ret = STATUS_TIMEOUT;
        }
    }

    return ret;
}

/*!
 * @brief Goto LIN sleep mode
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: goto sleep mode successfully
 */
status_t LIN_DRV_GotoSleepMode(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_state_t *linState = s_linState[instance];
    UART_Type *base = s_linUartBase[instance];

    /* Set current node state to sleep mode */
    linState->currentNodeState = LIN_NODE_STATE_SLEEP_MODE;
    linState->isTxBusy  = false;
    linState->isRxBusy  = false;
    linState->isBusBusy = false;

    /* Enable LIN wakeup interrupt */
    UART_SetIntMode(base, UART_INT_LIN_WAKEUP, true);
    /* Set LIN to be sleep mode */
    UART_SetLINSleep(base, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Goto LIN idle state
 *
 * @param[in] instance: LIN module instance
 * @return STATUS_SUCCESS: goto idle successfully
 */
status_t LIN_DRV_GotoIdleState(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_state_t *linState = s_linState[instance];
    UART_Type *base = s_linUartBase[instance];

    linState->currentEventId = LIN_NO_EVENT;
    linState->currentNodeState = LIN_NODE_STATE_IDLE;
    linState->isBusBusy = false;

    if (UART_GetLINSleepState(base))
    {
        /* Exit from sleep mode */
        UART_SetLINSleep(base, false);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Send LIN wakeup signal
 *
 * @param[in] instance: LIN module instance
 * @return send wakeup signal status
 *         - STATUS_SUCCESS: send wakeup signal successfully
 *         - STATUS_BUSY: send wakeup signal busy
 */
status_t LIN_DRV_SendWakeupSignal(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    status_t ret = STATUS_SUCCESS;

    if (false == s_linState[instance]->isBusBusy)
    {
        /* Send LIN wakeup signal */
        UART_Putchar(s_linUartBase[instance], s_linWakeupSignal[instance]);
    }
    else
    {
        ret = STATUS_BUSY;
    }

    return ret;
}

/*!
 * @brief Get LIN current node state
 *
 * @param[in] instance: LIN module instance
 * @return LIN current node state
 */
lin_node_state_t LIN_DRV_GetCurrentNodeState(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_node_state_t ret = LIN_NODE_STATE_UNINIT;
    lin_state_t *linState = s_linState[instance];

    if (linState != NULL)
    {
        ret = linState->currentNodeState;
    }

    return ret;
}

/*!
 * @brief Set timeout counter for timer interrupt
 *
 * @param[in] instance: LIN module instance
 * @param[in] timeout: timeout counter
 * @return none
 */
void LIN_DRV_SetTimeoutCounter(uint8_t instance, uint32_t timeout)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_state_t *linState = s_linState[instance];

    linState->timeoutCounterFlag = false;
    linState->timeoutCounter = timeout;
}

/*!
 * @brief Timer interrupt callback function
 *
 * @param[in] instance: LIN module instance
 * @return none
 */
void LIN_DRV_TimeoutService(uint8_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    lin_state_t *linState = s_linState[instance];

    switch (linState->currentNodeState)
    {
    case LIN_NODE_STATE_SEND_DATA:
        if (0U == linState->timeoutCounter)
        {
            linState->timeoutCounterFlag = true;
            if (true == linState->isTxBlocking)
            {
                return;
            }
            if (linState->Callback != NULL)
            {
                linState->Callback(instance, linState);
            }
            linState->isTxBusy = false;

            (void)LIN_DRV_GotoIdleState(instance);
        }
        else
        {
            linState->timeoutCounter--;
        }
        break;

    case LIN_NODE_STATE_RECV_DATA:
        if (0U == linState->timeoutCounter)
        {
            linState->timeoutCounterFlag = true;
            if (true == linState->isRxBlocking)
            {
                return;
            }
            if (linState->Callback != NULL)
            {
                linState->Callback(instance, linState);
            }
            linState->isRxBusy = false;

            (void)LIN_DRV_GotoIdleState(instance);
        }
        else
        {
            linState->timeoutCounter--;
        }
        break;

    default:
        break;
    }
}

/*!
 * @brief Process identifier parity
 *
 * @param[in] pid: ID or PID
 * @param[in] type: 0: make parity 1: check parity
 * @return PID or ID
 */
uint8_t LIN_DRV_ProcessParity(uint8_t pid, uint8_t type)
{
    uint8_t id = (pid & 0x3FU), byte, ret;

    /* Calculate the two parity bits */
    byte = id | (((~((pid >> 1U) ^ (pid >> 3U) ^ (pid >> 4U) ^ (pid >> 5U))) << 7U) & 0x80U) | \
           (((pid ^ (pid >> 1U) ^ (pid >> 2U) ^ (pid >> 4U)) << 6U) & 0x40U);

    if (CHECK_PARITY == type)
    {
        if (pid != byte)
        {
            ret = 0xFFU;
        }
        else
        {
            ret = id;
        }
    }
    else
    {
        ret = byte;
    }

    return ret;
}

/*!
 * @brief Make checksum byte
 *
 * @param[in] buf: pointer to buffer
 * @param[in] size: buffer size
 * @param[in] pid: protected identifier
 * @return checksum byte
 */
uint8_t LIN_DRV_MakeChecksumByte(const uint8_t *buf, uint8_t size, uint8_t pid)
{
    DEVICE_ASSERT(buf != NULL);
    DEVICE_ASSERT((size > 0U) && (size < 9U));

    uint8_t i;
    uint16_t checksum;

    if ((pid != 0x3CU) && (pid != 0x7DU) && (pid != 0xFEU) && (pid != 0xBFU))
    {
        checksum = pid;
    }
    else
    {
        /* if the id is 0x3C, 0x3D, 0x3E, 0x3F, not add pid to checksum */
        checksum = 0U;
    }

    for (i = 0U; i < size; i++)
    {
        checksum += *buf++;
        if (checksum > 0xFFU)
        {
            checksum -= 0xFFU;
        }
    }

    return (uint8_t)(~checksum);
}

/*!
 * @brief Make checksum according to checksum type
 *
 * @param[in] instance: LIN module instance
 * @param[in] buf: pointer to buffer
 * @param[in] size: buffer size
 * @param[in] pid: protected identifier
 * @return checksum byte
 */
static uint8_t LIN_MakeChecksum(uint8_t instance, const uint8_t *buf, uint8_t size, uint8_t pid)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(buf != NULL);
    DEVICE_ASSERT((size > 0U) && (size < 9U));

    uint8_t id = pid, i;
    const uint8_t *classicPID = s_linConfig[instance]->classicPID;
    uint8_t numOfClassicPID = s_linConfig[instance]->numOfClassicPID;

    if (0xFFU == numOfClassicPID)
    {
        /* All frame use classical checksum */
        id = 0U;
    }
    else if (classicPID != NULL)
    {
        for (i = 0U; i < numOfClassicPID; i++)
        {
            if (id == classicPID[i])
            {
                id = 0U;
                break;
            }
        }
    }
    else
    {
    }

    return (LIN_DRV_MakeChecksumByte(buf, size, id));
}

/* =============================================  EOF  ============================================== */
