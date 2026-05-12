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
 * @file uart_drv.c
 *
 * @brief This file provides uart integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "uart_irq.h"
#include "uart_hw.h"
#include "../dma/dma_hw.h"
#include "../ckgen/ckgen_hw.h"

/* ============================================  Define  ============================================ */
#define UART_SAMPLE_CNT_4_VALUE       (4UL)
#define UART_SAMPLE_CNT_8_VALUE       (8UL)
#define UART_SAMPLE_CNT_16_VALUE      (16UL)

#define UART_USE_DMA_TRANSMIT_LEN_MAX (32768U)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Pointer to uart state */
static uart_state_t * s_uartStatePtr[UART_INSTANCE_MAX] = {NULL};

/* Table of base addresses for uart instances. */
static UART_Type * const s_uartBase[UART_INSTANCE_MAX] = UART_BASE_PTRS;

/* Table to save UART IRQ numbers. */
static const IRQn_Type s_uartIrqId[UART_INSTANCE_MAX] = UART_IRQS;

/* Table to save UART module functional clock names. */
static const clock_names_t s_uartClkNames[UART_INSTANCE_MAX] = UART_CLOCK_NAMES;

/* ====================================  Functions declaration  ===================================== */
static status_t UART_StartSendDataUsingInt(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);
static void UART_CompleteSendDataUsingInt(uint32_t instance);
static status_t UART_StartReceiveDataUsingInt(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);
static void UART_CompleteReceiveDataUsingInt(uint32_t instance);

static void UART_StopTxDma(uint32_t instance);
static void UART_StopRxDma(uint32_t instance);
static status_t UART_StartSendDataUsingDma(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);
static void UART_TxDmaCallback(void *parameter, dma_chn_status_t status);
static status_t UART_StartReceiveDataUsingDma(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);
static void UART_RxDmaCallback(void *parameter, dma_chn_status_t status);

static void UART_PutData(uint32_t instance);
static void UART_GetData(uint32_t instance);
static void UART_RxIrqHandler(uint32_t instance);
static void UART_TxIrqHandler(uint32_t instance);
static void UART_TxCompleteIrqHandler(uint32_t instance);
static void UART_ErrIrqHandler(uint32_t instance);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get UART module base address.
 *
 * @param[in] instance: UART module instance
 * @return UART module base address
 */
UART_Type *UART_DRV_GetBase(uint32_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    return (s_uartBase[instance]);
}

/*!
 * @brief Initializes the UART configuration structure with
 *        default values.
 *
 * @param[out] uartUserConfig: Store UART default configuration structure
 * @return none
 */
void UART_DRV_GetDefaultConfig(uart_user_config_t *uartUserConfig)
{
    DEVICE_ASSERT(uartUserConfig != NULL);

    uartUserConfig->transferType = UART_USING_INTERRUPTS;
    uartUserConfig->baudRate = 9600U;
    uartUserConfig->parityMode = UART_PARITY_DISABLED;
    uartUserConfig->stopBitCount = UART_ONE_STOP_BIT;
    uartUserConfig->bitCountPerChar = UART_8_BITS_PER_CHAR;
    uartUserConfig->rxDMAChannel = 0U;
    uartUserConfig->txDMAChannel = 0U;
}

/*!
 * @brief UART initialize.
 *
 * @param[in] instance: UART instance
 * @param[in] uartStatePtr: UART state pointer
 * @param[in] uartConfig: UART configuration pointer which contains the configuration
 *                        information for the specified UART
 * @return none
 */
status_t UART_DRV_Init(uint32_t instance, uart_state_t *uartStatePtr, const uart_user_config_t *uartUserConfig)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(uartStatePtr != NULL);
    DEVICE_ASSERT(uartUserConfig != NULL);

    /* For 9 bits per char, parity bit cannot be enabled */
    DEVICE_ASSERT((UART_9_BITS_PER_CHAR == uartUserConfig->bitCountPerChar) \
                  && (UART_PARITY_DISABLED == uartUserConfig->parityMode));

    status_t osStatusRxSem = STATUS_SUCCESS;
    status_t osStatusTxSem = STATUS_SUCCESS;
    UART_Type *base = s_uartBase[instance];
    uint32_t idx = 0U, uartSourceClock = 0U;
    clock_names_t instanceClkName = s_uartClkNames[instance];
    uint8_t *clearStructPtr = NULL;
    uint32_t busClock;

    /* Get the bus clock */
    (void)CKGEN_DRV_GetFreq(BUS_CLK, &busClock);

    /* Get the UART function clock */
    (void)CKGEN_DRV_GetFreq(instanceClkName, &uartSourceClock);

    /* Check if current instance function clock is gated off */
    DEVICE_ASSERT(uartSourceClock > 0U);

    /* Check if current instance is already initialized. */
    DEVICE_ASSERT(NULL == s_uartStatePtr[instance]);

    /* Clear the state struct */
    clearStructPtr = (uint8_t *)uartStatePtr;
    for (idx = 0U; idx < sizeof(uart_state_t); idx++)
    {
        clearStructPtr[idx] = 0U;
    }

    /* Save runtime state struct pointer */
    s_uartStatePtr[instance] = uartStatePtr;

    /* Check clock config */
    if ((UART_USING_DMA == uartUserConfig->transferType) && (uartSourceClock != busClock))
    {
        /* Not support invalid clock config */
        return STATUS_UNSUPPORTED;
    }

    /* Save the transfer information for runtime retrieval */
    uartStatePtr->transferType = uartUserConfig->transferType;
    uartStatePtr->bitCountPerChar = uartUserConfig->bitCountPerChar;

    uartStatePtr->rxDMAChannel = uartUserConfig->rxDMAChannel;
    uartStatePtr->txDMAChannel = uartUserConfig->txDMAChannel;

    /* Set UART IRQHandler */
    g_uartIsr[instance] = UART_DRV_IRQHandler;

    /* Enable the modue bus clock and release module from reset state */
    UART_Init(instance);

    /* initialize the parameters of the UART config structure */
    (void)UART_DRV_SetBaudRate(instance, uartUserConfig->baudRate);

    UART_SetBitCountPerChar(base, uartUserConfig->bitCountPerChar);

    UART_SetParityMode(base, uartUserConfig->parityMode);
    UART_SetStopBitCount(base, uartUserConfig->stopBitCount);

    /* Enable UART FIFO */
    UART_SetFIFO(base, true);

    /* initialize transmitStatus and receiveStatus */
    uartStatePtr->transmitStatus = STATUS_SUCCESS;
    uartStatePtr->receiveStatus = STATUS_SUCCESS;

    /* Create the synchronization objects */
    osStatusRxSem = OSIF_SemaCreate(&uartStatePtr->rxComplete, 0U);
    osStatusTxSem = OSIF_SemaCreate(&uartStatePtr->txComplete, 0U);
    if ((STATUS_ERROR == osStatusRxSem) || (STATUS_ERROR == osStatusTxSem))
    {
        return STATUS_ERROR;
    }

    /* Set the Callback from config struct */
    uartStatePtr->rxCallback = uartUserConfig->rxCallback;
    uartStatePtr->rxCallbackParam = uartUserConfig->rxCallbackParam;
    uartStatePtr->txCallback = uartUserConfig->txCallback;
    uartStatePtr->txCallbackParam = uartUserConfig->txCallbackParam;

    /* Enable UART NVIC interrupt */
    NVIC_EnableIRQ(s_uartIrqId[instance]);

    /* Enable the UART transmitter */
    UART_SetTransmitterCmd(base, true);

    /* Enable the UART receiver */
    UART_SetReceiverCmd(base, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Uninitialize UART module.
 *
 * @param[in] instance: UART instance
 * @return STATUS_SUCCESS or STATUS_ERROR
 */
status_t UART_DRV_Deinit(uint32_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    clock_names_t instanceClkName = s_uartClkNames[instance];
    uint32_t uartSourceClock = 0U;
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    (void)CKGEN_DRV_GetFreq(instanceClkName, &uartSourceClock);

    /* Check if current instance is already de-initialized or is gated.*/
    DEVICE_ASSERT(s_uartStatePtr[instance] != NULL);
    DEVICE_ASSERT(uartSourceClock > 0U);

    /* Destroy the synchronization objects */
    (void)OSIF_SemaDestroy(&uartState->rxComplete);
    (void)OSIF_SemaDestroy(&uartState->txComplete);

    /* Disable UART NVIC interrupt. */
    NVIC_DisableIRQ(s_uartIrqId[instance]);
    NVIC_ClearPendingIRQ(s_uartIrqId[instance]);

    /* Disable the modue bus clock and set module at reset state */
    UART_DeInit(instance);

    /* Clear pointer to the state structure */
    s_uartStatePtr[instance] = NULL;

    /* Clear pointer to the UART IRQHandler */
    g_uartIsr[instance] = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Install receive data callback function.
 *
 * @param[in] instance: UART instance
 * @param[in] function: The receive data callback function to be installed
 * @param[in] callbackParam: The parameter of receive data callback function
 * @return Last callback funtion pointer
 */
uart_callback_t UART_DRV_InstallRxCallback(uint32_t instance, uart_callback_t function, void *callbackParam)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    uart_state_t * uartState = (uart_state_t *)s_uartStatePtr[instance];

    uart_callback_t currentCallback = uartState->rxCallback;
    uartState->rxCallback = function;
    uartState->rxCallbackParam = callbackParam;

    return currentCallback;
}

/*!
 * @brief Install tranmit data callback function.
 *
 * @param[in] instance: UART instance
 * @param[in] function: The tranmit data callback function to be installed
 * @param[in] callbackParam: The parameter of tranmit data callback function
 * @return Last callback funtion pointer
 */
uart_callback_t UART_DRV_InstallTxCallback(uint32_t instance, uart_callback_t function, void *callbackParam)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    uart_state_t * uartState = (uart_state_t *)s_uartStatePtr[instance];

    uart_callback_t currentCallback = uartState->txCallback;
    uartState->txCallback = function;
    uartState->txCallbackParam = callbackParam;

    return currentCallback;
}

/*!
 * @brief UART send data using blocking method, which will not return until the
 *        transmit is complete.
 * @param[in] instance: UART instance
 * @param[in] txBuff: The tx data buffer pointer
 * @param[in] txSize: The tx data buffer bytes size
 * @param[in] timeout：Configurate timeout
 * @return The transmit status
 */
status_t UART_DRV_SendDataBlocking(uint32_t instance, const uint8_t *txBuff, uint32_t txSize, uint32_t timeout)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);

    uart_state_t * uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;
    status_t syncStatus = STATUS_SUCCESS;

    /* Check buff point & data length */
    if ((NULL == txBuff) || (0 == txSize))
    {
        return STATUS_UNSUPPORTED;
    }

    /* Indicates this is a blocking transaction. */
    uartState->isTxBlocking = true;

    DEVICE_ASSERT((UART_USING_INTERRUPTS == uartState->transferType) \
                  || (UART_USING_DMA == uartState->transferType));

    if (UART_USING_INTERRUPTS == uartState->transferType)
    {
        /* Start the transmission process using interrupts */
        retVal = UART_StartSendDataUsingInt(instance, txBuff, txSize);
    }
    else
    {
        /* Start the transmission process using DMA */
        retVal = UART_StartSendDataUsingDma(instance, txBuff, txSize);
    }

    if (STATUS_SUCCESS == retVal)
    {
        /* Wait until the transmit is complete. */
        syncStatus = OSIF_SemaWait(&uartState->txComplete, timeout);

        /* Finish the transmission if timeout expired */
        if (STATUS_TIMEOUT == syncStatus)
        {
            uartState->isTxBlocking = false;
            uartState->transmitStatus = STATUS_TIMEOUT;

            if (UART_USING_INTERRUPTS == uartState->transferType)
            {
                UART_CompleteSendDataUsingInt(instance);
            }
            else
            {
                UART_StopTxDma(instance);
            }
        }
    }

    return uartState->transmitStatus;
}

/*!
 * @brief UART send data using polling mode.
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: The tx data buffer pointer
 * @param[in] txSize: The tx data buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
status_t UART_DRV_SendDataPolling(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check the validity of the parameters */
    DEVICE_ASSERT(txSize > 0U);
    DEVICE_ASSERT((UART_7_BITS_PER_CHAR == uartState->bitCountPerChar) \
                  || (UART_8_BITS_PER_CHAR == uartState->bitCountPerChar) \
                  || (0U == (txSize & 1U)));

    /* Check buff point & data length */
    if ((NULL == txBuff) || (0 == txSize))
    {
        return STATUS_UNSUPPORTED;
    }

    /* Check driver is not busy transmitting data from a previous asynchronous call */
    if (uartState->isTxBusy)
    {
        return STATUS_BUSY;
    }

    while (txSize > 0U)
    {
        while (!UART_GetStatusFlag(base, UART_TX_DATA_NOT_FULL))
        {
            /* Do nothing */
        }

        uartState->txBuff = txBuff;
        UART_PutData(instance);

        if ((UART_7_BITS_PER_CHAR == uartState->bitCountPerChar)
            || (UART_8_BITS_PER_CHAR == uartState->bitCountPerChar))
        {
            ++txBuff;
            --txSize;
        }
        else
        {
            ++txBuff;
            ++txBuff;
            txSize -= 2U;
        }
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief UART send data using non-blocking method (interrupt or DMA).
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: The tx data buffer pointer
 * @param[in] txSize: The tx data buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
status_t UART_DRV_SendData(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);

    status_t retVal = STATUS_SUCCESS;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check buff point & data length */
    if ((NULL == txBuff) || (0 == txSize))
    {
        return STATUS_UNSUPPORTED;
    }

    /* Indicates this is a non-blocking transaction. */
    uartState->isTxBlocking = false;

    DEVICE_ASSERT((UART_USING_INTERRUPTS == uartState->transferType) \
                  || (UART_USING_DMA == uartState->transferType));

    if (UART_USING_INTERRUPTS == uartState->transferType)
    {
        /* Start the transmission process using interrupts */
        retVal = UART_StartSendDataUsingInt(instance, txBuff, txSize);
    }
    else
    {
        /* Start the transmission process using DMA */
        retVal = UART_StartSendDataUsingDma(instance, txBuff, txSize);
    }

    return retVal;
}

/*!
 * @brief Get transmit status.
 *
 * @param[in] instance: UART instance
 * @param[out] bytesRemaining: Remaining bytes to be sent
 * @return Transmit status
 */
status_t UART_DRV_GetTransmitStatus(uint32_t instance, uint32_t *bytesRemaining)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (bytesRemaining != NULL)
    {
        if (uartState->isTxBusy)
        {
            /* Fill in the bytes not transferred yet. */
            if (UART_USING_INTERRUPTS == uartState->transferType)
            {
                /* In interrupt communication, the remaining bytes are
                   retrieved from the state structure */
                *bytesRemaining = uartState->txSize;;
            }
            else
            {
                /* In DMA communication, the remaining bytes are retrieved
                   from the current DMA channel */
                *bytesRemaining = DMA_DRV_GetRemainingBytes(uartState->txDMAChannel);
            }
        }
        else
        {
            *bytesRemaining = 0U;
        }
    }

    return uartState->transmitStatus;
}

/*!
 * @brief Terminates an non-blocking UART transmission early.
 *
 * @param[in] instance: UART instance
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_AbortSendingData(uint32_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check if a transfer is running. */
    if (!uartState->isTxBusy)
    {
        return STATUS_SUCCESS;
    }

    /* Update the tx status */
    uartState->transmitStatus = STATUS_UART_ABORTED;

    /* Stop the running transfer. */
    if (UART_USING_INTERRUPTS == uartState->transferType)
    {
        UART_CompleteSendDataUsingInt(instance);
    }
    else
    {
        UART_StopTxDma(instance);
    }

    return STATUS_SUCCESS;
}

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
status_t UART_DRV_ReceiveDataBlocking(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize, uint32_t timeout)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);

    uart_state_t * uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;
    status_t syncStatus = STATUS_SUCCESS;

    /* Check buff point & data length */
    if ((NULL == rxBuff) || (0 == rxSize))
    {
        return STATUS_UNSUPPORTED;
    }

    /* Indicates this is a blocking transaction. */
    uartState->isRxBlocking = true;

    DEVICE_ASSERT((UART_USING_INTERRUPTS == uartState->transferType) \
                  || (UART_USING_DMA == uartState->transferType));

    if (UART_USING_INTERRUPTS == uartState->transferType)
    {
         /* Start the reception process using interrupts */
         retVal = UART_StartReceiveDataUsingInt(instance, rxBuff, rxSize);
    }
    else
    {
        /* Start the reception process using DMA */
        retVal = UART_StartReceiveDataUsingDma(instance, rxBuff, rxSize);
    }

    if (STATUS_SUCCESS == retVal)
    {
        /* Wait until the receive is complete. */
        syncStatus = OSIF_SemaWait(&uartState->rxComplete, timeout);

        /* Finish the reception if timeout expired */
        if (STATUS_TIMEOUT == syncStatus)
        {
            uartState->isRxBlocking = false;
            uartState->receiveStatus = STATUS_TIMEOUT;

            if (UART_USING_INTERRUPTS== uartState->transferType)
            {
                UART_CompleteReceiveDataUsingInt(instance);
            }
            else
            {
                UART_StopRxDma(instance);
            }
        }
    }

    return uartState->receiveStatus;
}

/*!
 * @brief UART receive data using polling mode.
 *
 * @param[in] instance: UART instance
 * @param[out] rxBuff: The rx data buffer pointer
 * @param[in] rxSize: The rx data buffer bytes size
 * @return Receive status
 */
status_t UART_DRV_ReceiveDataPolling(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);

    uint8_t tmpByte = 0U;
    status_t retVal = STATUS_SUCCESS;
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check the validity of the parameters */
    DEVICE_ASSERT(rxSize > 0U);
    DEVICE_ASSERT((uartState->bitCountPerChar != UART_9_BITS_PER_CHAR) \
                  || (0U == (rxSize & 1U)));

    /* Check buff point & data length */
    if ((NULL == rxBuff) || (0 == rxSize))
    {
        return STATUS_UNSUPPORTED;
    }

    /* Check driver is not busy receiving data from a previous asynchronous call */
    if (uartState->isRxBusy)
    {
        return STATUS_BUSY;
    }

    /* Enable the UART receiver */
    UART_SetReceiverCmd((UART_Type *)base, true);

    while (rxSize > 0U)
    {
        while (!UART_GetStatusFlag(base, UART_RX_DATA_READY))
        {
            /* Do nothing */
        }

        uartState->rxBuff = rxBuff;
        UART_GetData(instance);

        if ((UART_7_BITS_PER_CHAR == uartState->bitCountPerChar) \
            || (UART_8_BITS_PER_CHAR == uartState->bitCountPerChar))
        {
            ++rxBuff;
            --rxSize;
        }
        else
        {
            ++rxBuff;
            ++rxBuff;
            rxSize -= 2U;
        }

        /* Check for errors on received data */
        if (UART_GetStatusFlag(base, UART_FRAME_ERR))
        {
            retVal = STATUS_UART_FRAMING_ERROR;
            /* Disable the UART receiver */
            UART_SetReceiverCmd((UART_Type *)base, false);
            /* Clear the flag */
            (void)UART_ClearStatusFlag(base, UART_FRAME_ERR);
            break;
        }
        else if (UART_GetStatusFlag(base, UART_NOISE_ERR))
        {
            retVal = STATUS_UART_NOISE_ERROR;
            /* Disable the UART receiver */
            UART_SetReceiverCmd((UART_Type *)base, false);
            /* Clear the flag */
            (void)UART_ClearStatusFlag(base, UART_NOISE_ERR);
            break;
        }
        else if (UART_GetStatusFlag(base, UART_PARITY_ERR))
        {
            retVal = STATUS_UART_PARITY_ERROR;
            /* Disable the UART receiver */
            UART_SetReceiverCmd((UART_Type *)base, false);
            /* Clear the flag */
            (void)UART_ClearStatusFlag(base, UART_PARITY_ERR);
            break;
        }
        else if (UART_GetStatusFlag(base, UART_RX_OVERRUN))
        {
            retVal = STATUS_UART_RX_OVERRUN;
            /* Disable the UART receiver */
            UART_SetReceiverCmd((UART_Type *)base, false);
            /* Clear the flag */
            (void)UART_ClearStatusFlag(base, UART_RX_OVERRUN);
            break;
        }
        else
        {
            /* No error condition - avoid MISRA violation */
        }
    }

    if (STATUS_SUCCESS == retVal)
    {
        /* Disable the UART receiver */
        UART_SetReceiverCmd((UART_Type *)base, false);
    }

    /* Read dummy to clear RX data ready flag */
    UART_Getchar(base, &tmpByte);

    return retVal;
}

/*!
 * @brief UART receive data using non-blocking method(interrupt or DMA).
 *
 * @param[in] instance: UART instance
 * @param[out] rxBuff: The rx data buffer pointer
 * @param[in] rxSize: The rx data buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
status_t UART_DRV_ReceiveData(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);

    status_t retVal = STATUS_SUCCESS;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check buff point & data length */
    if ((NULL == rxBuff) || (0 == rxSize))
    {
        return STATUS_UNSUPPORTED;
    }

    /* Indicates this is a non-blocking transaction. */
    uartState->isRxBlocking = false;

    DEVICE_ASSERT((UART_USING_INTERRUPTS == uartState->transferType) \
                  || (UART_USING_DMA == uartState->transferType));

    if (UART_USING_INTERRUPTS == uartState->transferType)
    {
        /* Start the reception process using interrupts */
        retVal = UART_StartReceiveDataUsingInt(instance, rxBuff, rxSize);
    }
    else
    {
        /* Start the reception process using DMA */
        retVal = UART_StartReceiveDataUsingDma(instance, rxBuff, rxSize);
    }

    return retVal;
}

/*!
 * @brief Get receive status.
 *
 * @param[in] instance: UART instance
 * @param[out] bytesRemaining: Remaining bytes to receive
 * @return Receive status
 */
status_t UART_DRV_GetReceiveStatus(uint32_t instance, uint32_t *bytesRemaining)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (bytesRemaining != NULL)
    {
        if (uartState->isRxBusy)
        {
            /* Fill in the bytes transferred. */
            if (UART_USING_INTERRUPTS == uartState->transferType)
            {
                /* In interrupt-based communication, the remaining bytes are retrieved
                   from the state structure */
                *bytesRemaining = uartState->rxSize;
            }
            else
            {
                /* In DMA communication, the remaining bytes are retrieved
                   from the current DMA channel */
                *bytesRemaining = DMA_DRV_GetRemainingBytes(uartState->rxDMAChannel);
            }
        }
        else
        {
            *bytesRemaining = 0;
        }
    }

    return uartState->receiveStatus;
}

/*!
 * @brief Terminates a non-blocking receive early.
 *
 * @param[in] instance: UART instance
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_AbortReceivingData(uint32_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check if a transfer is running. */
    if (!uartState->isRxBusy)
    {
        return STATUS_SUCCESS;
    }

    /* Update the rx status */
    uartState->receiveStatus = STATUS_UART_ABORTED;

    /* Stop the running transfer. */
    if (UART_USING_INTERRUPTS == uartState->transferType)
    {
        UART_CompleteReceiveDataUsingInt(instance);
    }
    else
    {
        UART_StopRxDma(instance);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Set UART baud rate.
 *
 * @param[in] instance: UART instance
 * @param[in] desiredBaudRate: UART state pointer
 * @return STATUS_BUSY, STATUS_ERROR or STATUS_SUCCESS
 */
status_t UART_DRV_SetBaudRate(uint32_t instance, uint32_t desiredBaudRate)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    uint32_t uartSourceClock = 0U, sampleCntValue = UART_SAMPLE_CNT_16_VALUE;
    uart_sample_cnt_t sampleCntReg = UART_SMP_CNT16;
    clock_names_t instanceClkName = s_uartClkNames[instance];
    UART_Type *base = s_uartBase[instance];
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    float divisor = 0;

    if (uartState != NULL)
    {
        /* Check if there is an ongoing transfer */
        if ((uartState->isTxBusy) || (uartState->isRxBusy))
        {
            return STATUS_BUSY;
        }
    }

    /* Get the UART clock as configured in the clock manager */
    (void)CKGEN_DRV_GetFreq(instanceClkName, &uartSourceClock);

    /* Check if current instance is clock gated off. */
    DEVICE_ASSERT(uartSourceClock > 0U);
    /* Check if the desired baud rate can be configured with the current protocol clock. */
    DEVICE_ASSERT(uartSourceClock >= (desiredBaudRate * UART_SAMPLE_CNT_4_VALUE));

    /* calculate sampleCntReg */
    if (uartSourceClock > (desiredBaudRate * UART_SAMPLE_CNT_4_VALUE))
    {
        sampleCntReg = UART_SMP_CNT4;
        sampleCntValue = UART_SAMPLE_CNT_4_VALUE;
        if (uartSourceClock > (desiredBaudRate * UART_SAMPLE_CNT_8_VALUE))
        {
            sampleCntReg = UART_SMP_CNT8;
            sampleCntValue = UART_SAMPLE_CNT_8_VALUE;
            if (uartSourceClock > (desiredBaudRate * UART_SAMPLE_CNT_16_VALUE))
            {
                sampleCntReg = UART_SMP_CNT16;
                sampleCntValue = UART_SAMPLE_CNT_16_VALUE;
            }
        }
    }
    else
    {
        return STATUS_ERROR;
    }

    /* calculate the baud rate */
    divisor = ((float)uartSourceClock / (desiredBaudRate * sampleCntValue));

    /* program the sampleCntReg value */
    UART_SetSampleCounter(base, sampleCntReg);

    /* write the divisor value */
    UART_SetBaudRateDivisor(base, divisor);

    return STATUS_SUCCESS;
}

/*!
 * @brief Get UART baud rate.
 *
 * @param[in] instance: UART instance
 * @param[out] configuredBaudRate: Return the UART configured baud rate
 * @return none
 */
void UART_DRV_GetBaudRate(uint32_t instance, uint32_t *configuredBaudRate)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(configuredBaudRate != NULL);

    uint16_t sampleCntReg = 0U;
    float divisor = 0;
    uint32_t uartSourceClock = 0U;
    clock_names_t instanceClkName = s_uartClkNames[instance];
    UART_Type *base = s_uartBase[instance];

    /* Get the UART clock */
    (void)CKGEN_DRV_GetFreq(instanceClkName, &uartSourceClock);

    sampleCntReg = UART_GetSampleCounter(base);
    divisor = UART_GetBaudRateDivisor(base);

    *configuredBaudRate = (uint32_t)(uartSourceClock / ((sampleCntReg) * divisor));
}

/*!
 * @brief Set the driver internal tx buffer.
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: The tx data buffer pointer
 * @param[in] txSize: The tx data buffer bytes size
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_SetTxBuffer(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    /* Check buff point & data length */
    if ((NULL == txBuff) || (0 == txSize))
    {
        return STATUS_UNSUPPORTED;
    }

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uartState->txBuff = txBuff;
    uartState->txSize = txSize;

    return STATUS_SUCCESS;
}

/*!
 * @brief Set the driver internal rx buffer.
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: The rx data buffer pointer
 * @param[in] txSize: The rx data buffer bytes size
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_SetRxBuffer(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    /* Check buff point & data length */
    if ((NULL == rxBuff) || (0 == rxSize))
    {
        return STATUS_UNSUPPORTED;
    }

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uartState->rxBuff = rxBuff;
    uartState->rxSize = rxSize;

    return STATUS_SUCCESS;
}

/*!
 * @brief UART interrupt handler.
 *
 * @param[in] instance: UART instance
 * @return none
 */
void UART_DRV_IRQHandler(uint32_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    UART_ErrIrqHandler(instance);

    /* Handle receive not empty interrupt */
    if (UART_GetIntMode(base, UART_INT_RX_NOT_EMPTY))
    {
        if (UART_GetStatusFlag(base, UART_RX_DATA_READY))
        {
            UART_RxIrqHandler(instance);
        }
    }

    /* Handle transmit not full interrupt */
    if (UART_GetIntMode(base, UART_INT_TX_NOT_FULL))
    {
        if (UART_GetStatusFlag(base, UART_TX_DATA_NOT_FULL))
        {
            UART_TxIrqHandler(instance);
        }
    }

    /* Handle transmit complete interrupt */
    if (UART_GetIntMode(base, UART_INT_TX_COMPLETE))
    {
        if (UART_GetStatusFlag(base, UART_TX_COMPLETE))
        {
            UART_TxCompleteIrqHandler(instance);
        }
    }

    /* Handle receive data match interrupt */
    if (UART_GetIntMode(base, UART_INT_DATA_MATCH))
    {
        if (UART_GetStatusFlag(base, UART_DATA_MATCH))
        {
            (void)UART_ClearStatusFlag(base, UART_DATA_MATCH);

            /* Invoke callback if there is one */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, (uint32_t)UART_EVENT_DATA_MATCH, uartState->rxCallbackParam);
            }
        }
    }
}

/*!
 * @brief UART rx not empty interrupt handler function.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_RxIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Get data and put in receive buffer  */
    UART_GetData(instance);

    /* Update the internal state */
    if (uartState->bitCountPerChar != UART_9_BITS_PER_CHAR)
    {
        ++uartState->rxBuff;
        --uartState->rxSize;
    }
    else
    {
        uartState->rxBuff = &uartState->rxBuff[2];
        uartState->rxSize -= 2U;
    }

    /* Check if this was the last byte in the current buffer */
    if (0U == uartState->rxSize)
    {
        /* Invoke callback if there is one (callback may reset the rx buffer
           for continuous reception) */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_RX_FULL, uartState->rxCallbackParam);
        }
    }

    /* Finish reception if this was the last byte received */
    if (0U == uartState->rxSize)
    {
        /* Complete transfer (disable rx logic) */
        UART_CompleteReceiveDataUsingInt(instance);

        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_END_TRANSFER, uartState->rxCallbackParam);
        }
    }
}

/*!
 * @brief UART tx not full interrupt handler function.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_TxIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Check if there are any more bytes to send */
    if (uartState->txSize > 0U)
    {
        /* Transmit the data */
        UART_PutData(instance);

        /* Update the internal state */
        if (uartState->bitCountPerChar != UART_9_BITS_PER_CHAR)
        {
            /* enable tx complete interrupt */
            if (1U == uartState->txSize)
            {
                UART_SetIntMode(base, UART_INT_TX_COMPLETE, true);
            }

            ++uartState->txBuff;
            --uartState->txSize;
        }
        else
        {
            /* enable tx complete interrupt */
            if (2U == uartState->txSize)
            {
                UART_SetIntMode(base, UART_INT_TX_COMPLETE, true);
            }

            uartState->txBuff = &uartState->txBuff[2];
            uartState->txSize -= 2U;
        }

        /* Check if this was the last byte in the current buffer */
        if (0U == uartState->txSize)
        {
            /* Invoke callback if there is one (callback may reset the tx buffer
               for continuous transmission)*/
            if (uartState->txCallback != NULL)
            {
                uartState->txCallback(uartState, (uint32_t)UART_EVENT_TX_EMPTY,
                                      uartState->txCallbackParam);
            }

            /* If there's no new data, disable tx not full interrupt */
            if (0U == uartState->txSize)
            {
                UART_SetIntMode(base, UART_INT_TX_NOT_FULL, false);
            }
        }
    }
}

/*!
 * @brief UART tx complete interrupt handler function.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_TxCompleteIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (0U == uartState->txSize)
    {
        if (UART_USING_INTERRUPTS == uartState->transferType)
        {
            /* Complete the interrupt based transfer */
            UART_CompleteSendDataUsingInt(instance);
        }
        else
        {
            /* Complete the DMA based transfer */
            UART_StopTxDma(instance);
        }
        /* Invoke callback if there is one */
        if (uartState->txCallback != NULL)
        {
            uartState->txCallback(uartState, (uint32_t)UART_EVENT_END_TRANSFER, uartState->txCallbackParam);
        }
    }
}

/*!
 * @brief UART error interrupt handler function.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_ErrIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Handle receive overrun interrupt */
    if (UART_GetStatusFlag(base, UART_RX_OVERRUN))
    {
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_RX_OVERRUN;
        if (UART_USING_INTERRUPTS == uartState->transferType)
        {
            /* Complete the transfer (disable rx logic) */
            UART_CompleteReceiveDataUsingInt(instance);
        }
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_StopRxDma(instance);
        }
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_ERROR, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }

    /* Handle parity error interrupt */
    if (UART_GetStatusFlag(base, UART_PARITY_ERR))
    {
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_PARITY_ERROR;
        if (UART_USING_INTERRUPTS == uartState->transferType)
        {
            /* Complete the transfer (disable rx logic) */
            UART_CompleteReceiveDataUsingInt(instance);
        }
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_StopRxDma(instance);
        }
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_ERROR, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }

    /* Handle framing error interrupt */
    if (UART_GetStatusFlag(base, UART_FRAME_ERR))
    {
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_FRAMING_ERROR;
        if (UART_USING_INTERRUPTS == uartState->transferType)
        {
            /* Complete the transfer (disable rx logic) */
            UART_CompleteReceiveDataUsingInt(instance);
        }
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_StopRxDma(instance);
        }
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_ERROR, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }

    /* Handle break error interrupt */
    if (UART_GetStatusFlag(base, UART_BREAK_ERR))
    {
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_BREAK_ERROR;
        if (UART_USING_INTERRUPTS == uartState->transferType)
        {
            /* Complete the transfer (disable rx logic) */
            UART_CompleteReceiveDataUsingInt(instance);
        }
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_StopRxDma(instance);
        }
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_ERROR, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }

    /* Handle noise error interrupt */
    if (UART_GetStatusFlag(base, UART_NOISE_ERR))
    {
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_NOISE_ERROR;
        if (UART_USING_INTERRUPTS == uartState->transferType)
        {
            /* Complete transfer (disable rx logic) */
            UART_CompleteReceiveDataUsingInt(instance);
        }
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_StopRxDma(instance);
        }
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_ERROR, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }
}

/*!
 * @brief Set UART Tx buffer and enable TX interrupt.
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: UART tx buffer base pointer
 * @param[in] txSize: UART rx buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
static status_t UART_StartSendDataUsingInt(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check it's not busy transmitting data from a previous function call */
    if (uartState->isTxBusy)
    {
        return STATUS_BUSY;
    }

    /* Check the validity of the parameters */
    DEVICE_ASSERT(txSize > 0U);
    DEVICE_ASSERT((UART_7_BITS_PER_CHAR == uartState->bitCountPerChar) \
                  || (UART_8_BITS_PER_CHAR == uartState->bitCountPerChar) \
                  || (0U == (txSize & 1U)));

    /* initialize the module driver state structure */
    uartState->txBuff = txBuff;
    uartState->txSize = txSize;
    uartState->isTxBusy = true;
    uartState->transmitStatus = STATUS_BUSY;

    /* Enable tx not full interrupt */
    UART_SetIntMode(base, UART_INT_TX_NOT_FULL, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Start UART DMA transfer.
 *
 * @param[in] instance: UART instance
 * @param[in] txBuff: UART tx buffer base pointer
 * @param[in] txSize: UART rx buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
static status_t UART_StartSendDataUsingDma(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    dma_transfer_size_t dmaTransferSize = DMA_TRANSFER_SIZE_1B;

    /* Check it's not busy transmitting data from a previous function call */
    if (uartState->isTxBusy)
    {
        return STATUS_BUSY;
    }

    DEVICE_ASSERT((txSize > 0U) && (txSize < UART_USE_DMA_TRANSMIT_LEN_MAX));

    /* Enable tx DMA requests */
    UART_SetTxDmaCmd(base, true);

    /* Update state structure */
    uartState->txBuff = txBuff;
    uartState->txSize = 0U;
    uartState->isTxBusy = true;
    uartState->transmitStatus = STATUS_BUSY;

    /* Set DMA channel transfer size */
    if (UART_9_BITS_PER_CHAR == uartState->bitCountPerChar)
    {
        dmaTransferSize = DMA_TRANSFER_SIZE_2B;
    }
    else
    {
        dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    }

    /* Configure the DMA transfer control */
    (void)DMA_DRV_ConfigTransfer(uartState->txDMAChannel,
                                 DMA_TRANSFER_MEM2PERIPH,
                                 (uint32_t)txBuff,
                                 (uint32_t)(&(base->RBR)),
                                 dmaTransferSize,
                                 txSize);

    /* Call driver function to end the transmission when the DMA transfer is done */
    (void)DMA_DRV_InstallCallback(uartState->txDMAChannel, (dma_callback_t)(UART_TxDmaCallback), (void*)(instance));

    /* Start the DMA channel */
    (void)DMA_DRV_StartChannel(uartState->txDMAChannel);

    /* Enable the UART transmitter */
    UART_SetTransmitterCmd(base, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Complete UART interrupt transfer.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_CompleteSendDataUsingInt(uint32_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (STATUS_BUSY == uartState->transmitStatus)
    {
        /* If the transfer is completed, update the transmit status */
        uartState->transmitStatus = STATUS_SUCCESS;
    }
    else
    {
        /* If the transfer is aborted or timed out, disable tx not full interrupt */
        UART_SetIntMode(base, UART_INT_TX_NOT_FULL, false);
    }

    /* Disable transmit complete interrupt */
    UART_SetIntMode(base, UART_INT_TX_COMPLETE, false);

    /* Update the internal busy flag */
    uartState->isTxBusy = false;

    /* Signal the synchronous completion object. */
    if (uartState->isTxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->txComplete);
    }
}

/*!
 * @brief UART DMA transfer callback function.
 *
 * @param[in] parameter: Function parameter
 * @param[in] status: UART tansfer DMA channel status
 * @return none
 */
static void UART_TxDmaCallback(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = ((uint32_t)parameter);
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check if the DMA transfer completed with errors */
    if (DMA_CHN_ERROR == status)
    {
        /* Update the status */
        uartState->transmitStatus = STATUS_ERROR;
        /* Stop the transfer */
        UART_StopTxDma(instance);
        /* Notify the application that an error occurred */
        if (uartState->txCallback != NULL)
        {
            uartState->txCallback(uartState, (uint32_t)UART_EVENT_ERROR, uartState->txCallbackParam);
        }
    }
    else
    {
        /* Invoke callback if there is one */
        if (uartState->txCallback != NULL)
        {
            /* Allow the user to provide a new buffer, for continuous transmission */
            uartState->txCallback(uartState, (uint32_t)UART_EVENT_TX_EMPTY, uartState->txCallbackParam);
        }

        /* If the callback has updated the tx buffer, update the DMA descriptor
           to continue the transfer;otherwise, stop the current transfer. */
        if (uartState->txSize > 0U)
        {
            /* Set the source address and the number of minor loops
               (bytes to be transfered) */
            DMA_DRV_SetSrcAddr(uartState->txDMAChannel,
                               (uint32_t)(uartState->txBuff),
                               (uint32_t)(uartState->txBuff + uartState->txSize));
            DMA_DRV_SetTranferLength(uartState->txDMAChannel, uartState->txSize);

            /* Now that this tx is set up, clear remaining bytes count */
            uartState->txSize = 0U;

            /* Re-start the channel */
            (void)DMA_DRV_StartChannel(uartState->txDMAChannel);
        }
        else
        {
            UART_TxCompleteIrqHandler(instance);
        }
    }
}

/*!
 * @brief Set UART Rx buffer and enable RX interrupt.
 *
 * @param[in] instance: UART instance
 * @param[out] rxBuff: UART rx buffer base pointer
 * @param[in] rxSize: UART rx buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
static status_t UART_StartReceiveDataUsingInt(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Check it's not busy receiving data from a previous function call */
    if (uartState->isRxBusy)
    {
        return STATUS_BUSY;
    }

    /* Check the validity of the parameters */
    DEVICE_ASSERT(rxSize > 0U);
    DEVICE_ASSERT((uartState->bitCountPerChar != UART_9_BITS_PER_CHAR) \
                  || (0U == (rxSize & 1U)));

    /* Initialize the module driver state struct to indicate transfer in progress
     * and with the buffer and byte count data. */
    uartState->isRxBusy = true;
    uartState->rxBuff = rxBuff;
    uartState->rxSize = rxSize;
    uartState->receiveStatus = STATUS_BUSY;

    /* Enable the receiver */
    UART_SetReceiverCmd(base, true);

    /* Enable error interrupts */
    UART_SetErrorInterrupts(base, true);

    /* Enable receive data not empty interrupt */
    UART_SetIntMode(base, UART_INT_RX_NOT_EMPTY, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Start UART DMA receive.
 *
 * @param[in] instance: UART instance
 * @param[out] rxBuff: UART rx buffer base pointer
 * @param[in] rxSize: UART rx buffer bytes size
 * @return STATUS_BUSY or STATUS_SUCCESS
 */
static status_t UART_StartReceiveDataUsingDma(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    dma_transfer_size_t dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    uint32_t buffAddr = 0U;
    uint8_t tmpByte = 0U;

    /* Check it's not busy transmitting data from a previous function call */
    if (uartState->isRxBusy)
    {
        return STATUS_BUSY;
    }

    /* Check buff address in memmory */
    buffAddr = (uint32_t)rxBuff;
    if ((buffAddr < 0x20000000U) || (buffAddr > 0x2000EFFFU))
    {
        return STATUS_UNSUPPORTED;
    }


    DEVICE_ASSERT((rxSize > 0U) && (rxSize < UART_USE_DMA_TRANSMIT_LEN_MAX));

    /* Enable rx DMA requests for the current instance */
    UART_SetRxDmaCmd(base, true);

    /* Read dummy to clear RX data ready flag */
    UART_Getchar(base, &tmpByte);

    /* Read dummy to clear RX data ready flag */
    UART_Getchar(base, &tmpByte);

    /* Clear the flags */
    UART_DRV_ClearErrorFlags(base);

    /* Set DMA channel transfer size */
    if (UART_9_BITS_PER_CHAR == uartState->bitCountPerChar)
    {
        dmaTransferSize = DMA_TRANSFER_SIZE_2B;
    }
    else
    {
        dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    }

    /* Configure the transfer control descriptor */
    (void)DMA_DRV_ConfigTransfer(uartState->rxDMAChannel,
                                 DMA_TRANSFER_PERIPH2MEM,
                                 (uint32_t)(&(base->RBR)),
                                 (uint32_t)rxBuff,
                                 dmaTransferSize,
                                 rxSize);

    /* Call driver function to end the reception when the DMA transfer is done */
    (void)DMA_DRV_InstallCallback(uartState->rxDMAChannel, (dma_callback_t)(UART_RxDmaCallback), (void*)(instance));

    /* Start the DMA channel */
    (void)DMA_DRV_StartChannel(uartState->rxDMAChannel);

    /* Update the state structure */
    uartState->rxBuff = rxBuff;
    uartState->rxSize = 0U;
    uartState->isRxBusy = true;
    uartState->receiveStatus = STATUS_BUSY;

    /* Enable error interrupts */
    UART_SetErrorInterrupts(base, true);

    /* Enable the receiver */
    UART_SetReceiverCmd(base, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Complete UART interrupt receive.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_CompleteReceiveDataUsingInt(uint32_t instance)
{
    DEVICE_ASSERT(instance < UART_INSTANCE_MAX);

    uint8_t tmpByte = 0U;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Disable receiver */
    UART_SetReceiverCmd(base, false);

    /* Disable error interrupts */
    UART_SetErrorInterrupts(base, false);

    /* Read dummy to clear rx data ready flag */
    UART_Getchar(base, &tmpByte);

    /* Disable receive data not full interrupt. */
    UART_SetIntMode(base, UART_INT_RX_NOT_EMPTY, false);

    /* Signal the synchronous completion object. */
    if (uartState->isRxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->rxComplete);
        uartState->isRxBlocking = false;
    }

    /* Update the information of the module driver state */
    uartState->isRxBusy = false;
    if (STATUS_BUSY == uartState->receiveStatus)
    {
        uartState->receiveStatus = STATUS_SUCCESS;
    }
}

/*!
 * @brief UART DMA receive callback function.
 *
 * @param[in] parameter: Function parameter
 * @param[in] status: UART tansfer DMA channel status
 * @return none
 */
static void UART_RxDmaCallback(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = ((uint32_t)parameter);
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    if (DMA_CHN_ERROR == status)
    {
        /* Update the status */
        uartState->receiveStatus = STATUS_ERROR;
        /* Stop the transfer */
        UART_StopRxDma(instance);
        /* Notify the application that an error occurred */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_ERROR, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }

    /* Return if an error occurred; error cases are treated by the interrupt handler */
    if (uartState->receiveStatus != STATUS_BUSY)
    {
        return;
    }

    /* Invoke callback if there is one */
    if (uartState->rxCallback != NULL)
    {
        /* Allow the user to provide a new buffer inside the callback,
           to continue the reception */
        uartState->rxCallback(uartState, (uint32_t)UART_EVENT_RX_FULL, uartState->rxCallbackParam);
    }

    /* If the callback has updated the rx buffer, update the DMA descriptor
       to continue the transfer;otherwise, stop the current transfer.*/
    if (uartState->rxSize > 0U)
    {
        /* Set the source address and the number of minor loops (bytes to be transfered) */
        DMA_DRV_SetDestAddr(uartState->rxDMAChannel,
                            (uint32_t)(uartState->rxBuff),
                            (uint32_t)(uartState->rxBuff + uartState->rxSize));

        DMA_DRV_SetTranferLength(uartState->rxDMAChannel, uartState->rxSize);

        /* Now that this rx is set up, clear remaining bytes count */
        uartState->rxSize = 0U;

        /* Re-start the channel */
        (void)DMA_DRV_StartChannel(uartState->rxDMAChannel);
    }
    else
    {
        /* Stop the reception */
        UART_StopRxDma(instance);

        /* Invoke the callback to notify the end of the transfer */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, (uint32_t)UART_EVENT_END_TRANSFER, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }
}

/*!
 * @brief UART write data to RBR register.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_PutData(uint32_t instance)
{
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];
    uint16_t data = 0U;
    const uint8_t *txBuff = uartState->txBuff;

    if ((UART_7_BITS_PER_CHAR == uartState->bitCountPerChar) \
        || (UART_8_BITS_PER_CHAR == uartState->bitCountPerChar))
    {
        /* Transmit the data */
        UART_Putchar(base, *txBuff);
    }
    else /* 9-bit per char */
    {
        /* Create a 16-bits integer from two bytes */
        data = (uint16_t)(*txBuff);
        ++txBuff;
        data |= (uint16_t)(((uint16_t)(*txBuff)) << 8U);

        /* Transmit the data */
        UART_Putchar9(base, data);
    }
}

/*!
 * @brief UART read data from RBR register.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_GetData(uint32_t instance)
{
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    const UART_Type *base = s_uartBase[instance];
    uint16_t data = 0U;
    uint8_t *rxBuff = uartState->rxBuff;

    if ((UART_7_BITS_PER_CHAR == uartState->bitCountPerChar) \
        || (UART_8_BITS_PER_CHAR == uartState->bitCountPerChar))
    {
        /* Receive the data */
        UART_Getchar(base, rxBuff);
    }
    else  /* 9-bits */
    {
        /* Receive the data */
        UART_Getchar9(base, &data);

        /* Write the least significant bits to the receive buffer */
        *rxBuff = (uint8_t)(data & 0xFFU);
        ++rxBuff;
        /* Write the ninth bit to the subsequent byte in the rx buffer */
        *rxBuff = (uint8_t)(data >> 8U);
    }
}

/*!
 * @brief UART stop DMA tansfer.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_StopTxDma(uint32_t instance)
{
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Disable tx DMA requests for the current instance */
    UART_SetTxDmaCmd(base, false);

    /* Stop the dma channel */
    (void)DMA_DRV_StopChannel(uartState->txDMAChannel);

    /* Disable transmit complete interrupt */
    UART_SetIntMode(base, UART_INT_TX_COMPLETE, false);

    /* Signal the synchronous completion object. */
    if (uartState->isTxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->txComplete);
    }

    if (STATUS_BUSY == uartState->transmitStatus)
    {
        /* If the transfer is completed, update the transmit status */
        uartState->transmitStatus = STATUS_SUCCESS;
    }

    /* Update the internal busy flag */
    uartState->isTxBusy = false;
}

/*!
 * @brief UART stop DMA receive.
 *
 * @param[in] instance: UART instance
 * @return none
 */
static void UART_StopRxDma(uint32_t instance)
{
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uint8_t tmpByte = 0U;

    /* Disable receiver */
    UART_SetReceiverCmd(base, false);

    /* Disable error interrupts */
    UART_SetErrorInterrupts(base, false);

    /* Disable rx DMA requests */
    UART_SetRxDmaCmd(base, false);

    /* Disable FIFO */
    UART_SetFIFO(base,false);

    /* Read dummy to clear RX data ready flag */
    UART_Getchar(base, &tmpByte);

    /* Read dummy to clear RX data ready flag */
    UART_Getchar(base, &tmpByte);

    /* Enable FIFO */
    UART_SetFIFO(base,true);

    /* Stop the DMA channel */
    (void)DMA_DRV_StopChannel(uartState->rxDMAChannel);

    /* Signal the synchronous completion object. */
    if (uartState->isRxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->rxComplete);
        uartState->isRxBlocking = false;
    }

    /* Update the internal driver status */
    if (STATUS_BUSY == uartState->receiveStatus)
    {
        uartState->receiveStatus = STATUS_SUCCESS;
    }

    /* Update the information of the module driver state */
    uartState->isRxBusy = false;
}

/* =============================================  EOF  ============================================== */
