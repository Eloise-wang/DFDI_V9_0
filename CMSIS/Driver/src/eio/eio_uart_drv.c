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
 * @file eio_uart_drv.c
 *
 * @brief This file provides eio uart integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "eio_hw.h"
#include "eio_comm.h"
#include "eio_uart_drv.h"
#include "ckgen_drv.h"
#include "dma_drv.h"

/* ============================================  Define  ============================================ */
/* Constraints used for baud rate computation */
#define UART_DIVIDER_MIN_VALUE  (0)
#define UART_DIVIDER_MAX_VALUE  (0xFF)

/* Shifters/Timers used for UART simulation The param[in]eter x represents the
   resourceIndex value for the current driver instance */
#define UART_TX_SHIFTER(x)     (x)
#define UART_RX_SHIFTER(x)     (x)
#define UART_TX_TIMER(x)       (x)
#define UART_RX_TIMER(x)       (x)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* First init about uart */
static bool s_uartInit = false;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Computes the baud rate divider for a target baud rate
 *
 * @param[in] baudRate: The desired baud rate in hertz
 * @param[out] divider: Point to baud rate divider
 * @param[in] inputClock: The frequency of the input clock
 * @return none
 */
static void EIO_UART_DRV_ComputeBaudRateDivider(uint32_t baudRate, uint16_t *divider, uint32_t inputClock)
{
    int32_t tmpDiv;

    /* Compute divider: ((input_clock / baud_rate) / 2) - 1. Round to nearest integer */
    tmpDiv = (((int32_t)inputClock + (int32_t)baudRate) / (2 * (int32_t)baudRate)) - 1;
    /* Enforce upper/lower limits */
    if (tmpDiv < UART_DIVIDER_MIN_VALUE)
    {
        tmpDiv = UART_DIVIDER_MIN_VALUE;
    }
    if (tmpDiv > UART_DIVIDER_MAX_VALUE)
    {
        tmpDiv = UART_DIVIDER_MAX_VALUE;
    }

    *divider = (uint16_t)tmpDiv;
}

/*!
 * @brief Configures the EIO module for UART Tx.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @param[in] userConfigPtr: Pointer to the EIO_UART user configuration structure
 * @param[in] inputClock: The frequency of the input clock
 * @return none
 */
static void EIO_UART_DRV_ConfigureTx(eio_uart_state_t *state,
                                     const eio_uart_user_config_t *userConfigPtr,
                                     uint32_t inputClock)
{
    EIO_Type *baseAddr;
    uint16_t divider;
    uint16_t bits;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Compute divider */
    EIO_UART_DRV_ComputeBaudRateDivider(userConfigPtr->baudRate, &divider, inputClock);
    bits = userConfigPtr->bitCount;

    /* Configure tx shifter */
    EIO_SetShifterConfig(baseAddr,
                         UART_TX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_0,
                         EIO_SHIFTER_STOP_BIT_1,
                         EIO_SHIFTER_SOURCE_PIN);
    EIO_SetShifterControl(baseAddr,
                          UART_TX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_TRANSMIT,
                          userConfigPtr->dataPin,             /* Output on tx pin */
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_OUTPUT,
                          UART_TX_TIMER(resourceIndex),
                          EIO_TIMER_POLARITY_POSEDGE);

    /* Configure tx timer */
    EIO_SetTimerCompare(baseAddr, UART_TX_TIMER(resourceIndex), \
		                    (uint16_t)((((uint16_t)(bits << 1U) - 1U) << 8U) + divider));

    EIO_SetTimerConfig(baseAddr,
                       UART_TX_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_ENABLED,
                       EIO_TIMER_STOP_BIT_TIM_DIS,
                       EIO_TIMER_ENABLE_TRG_HIGH,         /* Enable when Tx data is available */
                       EIO_TIMER_DISABLE_TIM_CMP,
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_CLK_SHIFT_TMR, /* Decrement on EIO clock */
                       EIO_TIMER_INITOUT_ONE);
    EIO_SetTimerControl(baseAddr,
                        UART_TX_TIMER(resourceIndex),
                        (uint8_t)((uint8_t)(UART_TX_SHIFTER(resourceIndex) << 2U) + 1U), /* Trigger on tx shifter status flag */
                        EIO_TRIGGER_POLARITY_LOW,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        0U,                                     /* Pin unused */
                        EIO_PIN_POLARITY_HIGH,
                        EIO_PIN_CONFIG_DISABLED,
                        EIO_TIMER_MODE_DISABLED);
}

/*!
 * @brief Configures the EIO module for UART Rx.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @param[in] userConfigPtr: Pointer to the EIO_UART user configuration structure
 * @param[in] inputClock: The frequency of the input clock
 * @return none
 */
static void EIO_UART_DRV_ConfigureRx(eio_uart_state_t *state,
                                     const eio_uart_user_config_t *userConfigPtr,
                                     uint32_t inputClock)
{
    EIO_Type *baseAddr;
    uint16_t divider;
    uint16_t bits;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Compute divider. */
    EIO_UART_DRV_ComputeBaudRateDivider(userConfigPtr->baudRate, &divider, inputClock);
    bits = userConfigPtr->bitCount;

    /* Configure rx shifter */
    EIO_SetShifterConfig(baseAddr,
                         UART_RX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_0,
                         EIO_SHIFTER_STOP_BIT_1,
                         EIO_SHIFTER_SOURCE_PIN);
    EIO_SetShifterControl(baseAddr,
                          UART_RX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->dataPin,             /* Input from rx pin */
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_DISABLED,
                          UART_RX_TIMER(resourceIndex),
                          EIO_TIMER_POLARITY_NEGEDGE);

    /* Configure rx timer */
    EIO_SetTimerCompare(baseAddr, UART_RX_TIMER(resourceIndex), \
                        (uint16_t)((((uint16_t)(bits << 1U) - 1U) << 8U) + divider));
    EIO_SetTimerConfig(baseAddr,
                       UART_RX_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_ENABLED,
                       EIO_TIMER_STOP_BIT_TIM_DIS,
                       EIO_TIMER_ENABLE_PIN_POSEDGE,         /* Enable when data is available */
                       EIO_TIMER_DISABLE_TIM_CMP,
                       EIO_TIMER_RESET_PIN_RISING,
                       EIO_TIMER_DECREMENT_CLK_SHIFT_TMR,    /* Decrement on EIO clock */
                       EIO_TIMER_INITOUT_ONE_RESET);
    EIO_SetTimerControl(baseAddr,
                        UART_RX_TIMER(resourceIndex),
                        0U,                                      /* Trigger unused */
                        EIO_TRIGGER_POLARITY_HIGH,
                        EIO_TRIGGER_SOURCE_EXTERNAL,
                        userConfigPtr->dataPin,                   /* Input from rx pin */
                        EIO_PIN_POLARITY_LOW,
                        EIO_PIN_CONFIG_DISABLED,
                        EIO_TIMER_MODE_DISABLED);
}

/*!
 * @brief End the current transfer.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_EndTransfer(eio_uart_state_t *state)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Disable transfer engine */
    switch (state->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Disable interrupts for Rx / Tx shifter */
        EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << UART_TX_SHIFTER(resourceIndex)), false);
        EIO_SetShifterErrorInterrupt(baseAddr, (uint8_t)(1U << UART_TX_SHIFTER(resourceIndex)), false);
        /* Disable timer interrupt  */
        EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << UART_TX_TIMER(resourceIndex)), false);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Nothing to do here */
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* For Tx we need to disable timer interrupt */
        EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << UART_TX_TIMER(resourceIndex)), false);
        /* Stop DMA channels */
        (void)DMA_DRV_StopChannel(state->dmaChannel);
        /* Disable the EIO DMA request */
        EIO_SetShifterDMARequest(baseAddr, (uint8_t)(1U << UART_TX_SHIFTER(resourceIndex)), false);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    state->remainingBytes = 0U;
    state->driverIdle = true;
    /* Signal transfer end for blocking transfers */
    if (state->blocking == true)
    {
        (void)OSIF_SemaPost(&(state->idleSemaphore));
    }
}

/*!
 * @brief Enables timers and shifters to start a transfer
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_EnableTransfer(eio_uart_state_t *state)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = state->eioCommon.resourceIndex;
    baseAddr = g_eioBase[state->eioCommon.instance];
    /* Enable timers and shifters */
    if (state->direction == EIO_UART_DIRECTION_RX)
    {
        /* In rx mode, discard any leftover rx. data */
        EIO_ClearShifterStatus(baseAddr, UART_RX_SHIFTER(resourceIndex));
        if (EIO_GetShifterMode(baseAddr, UART_RX_SHIFTER(resourceIndex)) == EIO_SHIFTER_MODE_MATCH_STORE)
        {
            EIO_SetShifterMode(baseAddr, UART_RX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_MATCH_STORE);
        }
        else
        {
            EIO_SetShifterMode(baseAddr, UART_RX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_RECEIVE);
        }
    }
    EIO_SetTimerMode(baseAddr, UART_TX_TIMER(resourceIndex), EIO_TIMER_MODE_8BIT_BAUD);
}

/*!
 * @brief Forcefully stops the current transfer
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_StopTransfer(eio_uart_state_t *state)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = state->eioCommon.resourceIndex;
    baseAddr = g_eioBase[state->eioCommon.instance];
    /* Clear any leftover error flags */
    EIO_ClearShifterErrorStatus(baseAddr, UART_TX_SHIFTER(resourceIndex));
    /* End the transfer */
    EIO_UART_DRV_EndTransfer(state);
    /* Re-enable shifter for Tx, to ensure correct idle state */
    if (state->direction == EIO_UART_DIRECTION_TX)
    {
        /* In tx mode restore start bit in case it was changed for end of transmission detection */
        EIO_SetShifterStartBit(baseAddr, UART_TX_SHIFTER(resourceIndex), EIO_SHIFTER_START_BIT_0);
        EIO_SetShifterMode(baseAddr, UART_TX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_TRANSMIT);
    }
}

/*!
 * @brief waits for the end of a blocking transfer
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return  Error or success status returned by API
 */
static status_t EIO_UART_DRV_WaitTransferEnd(eio_uart_state_t *state, uint32_t timeout)
{
    status_t osifError = STATUS_SUCCESS;

    switch (state->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Wait for transfer to be completed by the IRQ */
        osifError = OSIF_SemaWait(&(state->idleSemaphore), timeout);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Call EIO_UART_DRV_GetStatus() to do the transfer */
        while (EIO_UART_DRV_GetStatus(state, NULL) == STATUS_BUSY) {}
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Wait for transfer completion to be signaled by the DMA or IRQ */
        osifError = OSIF_SemaWait(&(state->idleSemaphore), timeout);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    /* Blocking transfer is over */
    state->blocking = false;
    if (osifError == STATUS_TIMEOUT)
    {
        /* Abort current transfer */
        state->status = STATUS_TIMEOUT;
        EIO_UART_DRV_StopTransfer(state);
    }

    return state->status;
}

/*!
 * @brief reads data received by the module
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_ReadData(eio_uart_state_t *state)
{
    const EIO_Type *baseAddr;
    uint32_t data;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(state->remainingBytes > 0U);
    DEVICE_ASSERT(state->rxData != NULL);

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Read data from shifter buffer */
    if (EIO_GetShifterMode(baseAddr, UART_RX_SHIFTER(resourceIndex)) == EIO_SHIFTER_MODE_MATCH_STORE)
    {
        data = EIO_ReadShifterBuffer(baseAddr, UART_RX_SHIFTER(resourceIndex),  EIO_SHIFTER_RW_MODE_BIT_SWAP );
        data >>= 32U - (uint32_t)(state->bitCount);
    }
    else
    {
        data = EIO_ReadShifterBuffer(baseAddr, UART_RX_SHIFTER(resourceIndex), EIO_SHIFTER_RW_MODE_NORMAL);
        data >>= 32U - (uint32_t)(state->bitCount);
    }
    if (state->bitCount <= 8U)
    {
        *(uint8_t *)state->rxData = (uint8_t)data;
        /* Update rx buffer pointer and remaining bytes count */
        state->rxData ++;
        state->remainingBytes -= 1U;
    }
    else
    {
        /* For more than 8 bits per word 2 bytes are needed */
        *(uint16_t *)state->rxData = (uint16_t)data;
        /* Update rx buffer pointer and remaining bytes count */
        state->rxData = &state->rxData[2U];
        state->remainingBytes -= 2U;
    }
}

/*!
 * @brief writes data to be transmitted by the module
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_WriteData(eio_uart_state_t *state)
{
    EIO_Type *baseAddr;
    uint32_t data;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(state->txData != NULL);

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    if (state->remainingBytes == 0U)
    {
        /* Done transmitting */
        return;
    }
    /* Read data from user buffer and update tx buffer pointer and remaining bytes count */
    if (state->bitCount <= 8U)
    {
        data = (uint32_t)(*(uint8_t *)state->txData);
        state->txData++;
        state->remainingBytes -= 1U;
    }
    else
    {
        /* For more than 8 bits per word 2 bytes are needed */
        data = (uint32_t)(*(uint16_t *)state->txData);
        state->txData = &state->txData[2U];
        state->remainingBytes -= 2U;
    }

    EIO_WriteShifterBuffer(baseAddr, UART_TX_SHIFTER(resourceIndex), data, EIO_SHIFTER_RW_MODE_NORMAL);
}

/*!
 * @brief Check status of the UART transmission. This function can be
 *        called either in an interrupt routine or directly in polling
 *        mode to advance the transfer.
 *
 * @param[in] stateStruct: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_CheckStatusTx(void *stateStruct)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    eio_uart_state_t *state;

    DEVICE_ASSERT(stateStruct != NULL);

    state = (eio_uart_state_t *)stateStruct;
    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* No need to check for Tx underflow since timer is controlled by the shifter status flag */
    /* Check for transfer end */
    if (state->remainingBytes == 0U)
    {
        if (EIO_GetTimerStatus(baseAddr, UART_TX_TIMER(resourceIndex)))
        {
            /* Clear timer status */
            EIO_ClearTimerStatus(baseAddr, UART_TX_TIMER(resourceIndex));
            state->txFlush--;
            if (state->txFlush == 0U)
            {
                /* Done flushing the Tx buffer, end transfer */
                /* Record success if there was no error */
                if (state->status == STATUS_BUSY)
                {
                    state->status = STATUS_SUCCESS;
                }
                EIO_UART_DRV_StopTransfer(state);
                /* Call callback to announce the end transfer event to the user */
                if (state->callback != NULL)
                {
                    state->callback(state, EIO_UART_EVENT_END_TRANSFER, state->callbackParam);
                }
            }
            else if (EIO_GetShifterStatus(baseAddr, UART_TX_SHIFTER(resourceIndex)))
            {
                /* txFlush == 1, but last byte was already transferred from buffer to shifter. There is a
                   danger that the transmission is over and we end up never reporting the end event.
                   To avoid this, send one extra dummy byte */
                /* Set start bit to 1 and send an 0xFF byte, this way the line will appear idle */
                EIO_SetShifterStartBit(baseAddr, UART_TX_SHIFTER(resourceIndex), EIO_SHIFTER_START_BIT_1);
                EIO_WriteShifterBuffer(baseAddr, UART_TX_SHIFTER(resourceIndex), 0xFFFFFFFFUL, EIO_SHIFTER_RW_MODE_NORMAL);
            }
            else
            {
                /* txFlush == 1, and last byte was not yet transferred from buffer to shifter.
                   No need to do anything, just wait for the next timer event. */
            }
        }
    }
    /* Check if transmitter needs more data */
    else if (EIO_GetShifterStatus(baseAddr, UART_TX_SHIFTER(resourceIndex)) && (state->remainingBytes > 0U))
    {
        EIO_UART_DRV_WriteData(state);
        if (state->remainingBytes == 0U)
        {
            /* Out of data, call callback to allow user to provide a new buffer */
            if (state->callback != NULL)
            {
                state->callback(state, EIO_UART_EVENT_TX_EMPTY, state->callbackParam);
            }
        }
        if (state->remainingBytes == 0U)
        {
            /* No more data, transmission will stop after the last bytes are sent.
               The timer event will indicate when the send is complete */
            /* Clear any previous timer events */
            EIO_ClearTimerStatus(baseAddr, UART_TX_TIMER(resourceIndex));
            if (state->driverType == EIO_DRIVER_TYPE_INTERRUPTS)
            {
                /* Transmission completed; disable interrupt */
                EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << UART_TX_SHIFTER(resourceIndex)), false);
                /* Enable timer interrupt to ensure that transfer is completed */
                EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << UART_TX_TIMER(resourceIndex)), true);
            }
        }
    }
    else
    {
        /* No relevant events - nothing to do */
    }
}

/*!
 * @brief Check status of the UART reception. This function can be
 *        called either in an interrupt routine or directly in polling
 *        mode to advance the transfer.
 *
 * @param[in] stateStruct: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_CheckStatusRx(void *stateStruct)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    eio_uart_state_t *state;

    DEVICE_ASSERT(stateStruct != NULL);

    state = (eio_uart_state_t *)stateStruct;
    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Check for errors */
    if (EIO_GetShifterErrorStatus(baseAddr, UART_RX_SHIFTER(resourceIndex)))
    {
        state->status = STATUS_UART_RX_OVERRUN;
        EIO_ClearShifterErrorStatus(baseAddr, UART_RX_SHIFTER(resourceIndex));
        state->remainingBytes = 0U;
        /* Continue processing events */
    }
    /* Check if data was received */
    else if (EIO_GetShifterStatus(baseAddr, UART_RX_SHIFTER(resourceIndex)))
    {
        EIO_UART_DRV_ReadData(state);
        if (state->remainingBytes == 0U)
        {
            /* Out of data, call callback to allow user to provide a new buffer */
            if (state->callback != NULL)
            {
                state->callback(state, EIO_UART_EVENT_RX_FULL, state->callbackParam);
            }
        }
    }
    else
    {
        /* No events - nothing to do */
    }
    /* Check if transfer is over */
    if (state->remainingBytes == 0U)
    {
        /* Record success if there was no error */
        if (state->status == STATUS_BUSY)
        {
            state->status = STATUS_SUCCESS;
        }
        /* Discard any leftover rx. data */
        EIO_ClearShifterStatus(baseAddr, UART_RX_SHIFTER(resourceIndex));
        /* End transfer */
        EIO_UART_DRV_StopTransfer(state);
        /* Call callback to announce the event to the user */
        if (state->callback != NULL)
        {
            state->callback(state, EIO_UART_EVENT_END_TRANSFER, state->callbackParam);
        }
    }
}

/*!
 * @brief Check status of the UART transfer. This function can be
 *        called either in an interrupt routine or directly in polling
 *        mode to advance the transfer.
 *
 * @param[in] stateStruct: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_CheckStatus(void *stateStruct)
{
    const eio_uart_state_t *state;

    DEVICE_ASSERT(stateStruct != NULL);

    state = (eio_uart_state_t *)stateStruct;
    if (state->direction == EIO_UART_DIRECTION_TX)
    {
        EIO_UART_DRV_CheckStatusTx(stateStruct);
    }
    else
    {
        EIO_UART_DRV_CheckStatusRx(stateStruct);
    }
}

/*!
 * @brief function called at the end of a DMA Tx transfer.
 *
 * @param[in] stateStruct: Pointer to the EIO_UART driver context structure
 * @param[in] status: DMA channel status
 * @return none
 */
static void EIO_UART_DRV_EndDmaTxTransfer(void *stateStruct, dma_chn_status_t status)
{
    eio_uart_state_t *state;
    uint8_t dmaChn;
    EIO_Type *baseAddr;

    DEVICE_ASSERT(stateStruct != NULL);

    state = (eio_uart_state_t *)stateStruct;
    baseAddr = g_eioBase[state->eioCommon.instance];

    if (status == DMA_CHN_ERROR)
    {
        /* DMA error, stop transfer */
        state->status = STATUS_ERROR;
        EIO_UART_DRV_StopTransfer(state);
        /* Call callback to announce the end transfer event to the user */
        if (state->callback != NULL)
        {
            state->callback(state, EIO_UART_EVENT_END_TRANSFER, state->callbackParam);
        }
    }
    else
    {
        /* Call callback to allow user to provide a new buffer */
        if (state->callback != NULL)
        {
            state->callback(state, EIO_UART_EVENT_TX_EMPTY, state->callbackParam);
        }
        if (state->remainingBytes == 0U)
        {
            /* No more data to transmit, transmission will stop */
            /* Enable timer interrupt to let IRQ ensure that transfer is completed */
            EIO_ClearTimerStatus(baseAddr, UART_TX_TIMER(state->eioCommon.resourceIndex));
            EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << UART_TX_TIMER(state->eioCommon.resourceIndex)), true);
        }
        else
        {
            /* There is more data to transfer, restart DMA channel */
            /* Update buffer address and size */
            dmaChn = state->dmaChannel;

            DMA_DRV_SetSrcAddr(dmaChn, (uint32_t)(state->txData), (uint32_t)((state->txData) + (state->remainingBytes)));
            DMA_DRV_SetTranferLength(dmaChn, state->remainingBytes);

            /* Now that this tx is set up, clear remaining bytes count */
            state->remainingBytes = 0U;
            /* Start the channel */
            (void)DMA_DRV_StartChannel(dmaChn);
        }
    }
}

/*!
 * @brief function called at the end of a DMA Rx transfer.
 *
 * @param[in] stateStruct: Pointer to the EIO_UART driver context structure
 * @param[in] status: DMA channel status
 * @return none
 */
static void EIO_UART_DRV_EndDmaRxTransfer(void *stateStruct, dma_chn_status_t status)
{
    eio_uart_state_t *state;
    uint8_t dmaChn;

    DEVICE_ASSERT(stateStruct != NULL);

    state = (eio_uart_state_t *)stateStruct;

    if (status == DMA_CHN_ERROR)
    {
        /* DMA error, stop transfer */
        state->status = STATUS_ERROR;
    }
    else
    {
        /* Call callback to allow user to provide a new buffer */
        if (state->callback != NULL)
        {
            state->callback(state, EIO_UART_EVENT_RX_FULL, state->callbackParam);
        }
    }
    if (state->remainingBytes == 0U)
    {
        /* No more data to transmit, reception will stop */
        if (state->status == STATUS_BUSY)
        {
            state->status = STATUS_SUCCESS;
        }
        EIO_UART_DRV_StopTransfer(state);
        /* Call callback to announce the event to the user */
        if (state->callback != NULL)
        {
            state->callback(state, EIO_UART_EVENT_END_TRANSFER, state->callbackParam);
        }
    }
    else
    {
        /* There is more data to transfer, restart DMA channel */
        /* Update buffer address and size */
        dmaChn = state->dmaChannel;

        DMA_DRV_SetDestAddr(dmaChn, (uint32_t)(state->rxData), (uint32_t)((state->rxData) + (state->remainingBytes)));
        DMA_DRV_SetTranferLength(dmaChn, state->remainingBytes);
        /* Now that this tx is set up, clear remaining bytes count */
        state->remainingBytes = 0U;
        /* Start the channel */
        (void)DMA_DRV_StartChannel(dmaChn);
    }
}

/*!
 * @brief Computes the address of the register used for DMA Tx transfer
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return  the address of the register
 */
static uint32_t EIO_UART_DRV_ComputeTxRegAddr(const eio_uart_state_t *state)
{
    uint32_t addr;
    const EIO_Type *baseAddr;
    uint8_t shifter;

    baseAddr = g_eioBase[state->eioCommon.instance];
    shifter = UART_TX_SHIFTER(state->eioCommon.resourceIndex);
    addr = (uint32_t)(&(baseAddr->SHIFTBUF[shifter]));
    return addr;
}

/*!
 * @brief Computes the address of the register used for DMA Rx transfer
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return  the address of the register
 */
static uint32_t EIO_UART_DRV_ComputeRxRegAddr(const eio_uart_state_t *state)
{
    uint32_t addr;
    const EIO_Type *baseAddr;
    uint8_t shifter;
    uint32_t byteCount;

    if (state->bitCount <= 8U)
    {
        byteCount = 1U;
    }
    else
    {
        byteCount = 2U;
    }
    baseAddr = g_eioBase[state->eioCommon.instance];
    shifter = UART_RX_SHIFTER(state->eioCommon.resourceIndex);

    if (EIO_GetShifterMode(baseAddr, shifter) == EIO_SHIFTER_MODE_MATCH_STORE)
    {
        addr = (uint32_t)(&(baseAddr->SHIFTBUFBIS[shifter])) + (sizeof(uint32_t) - byteCount);
    }
    else
    {
        addr = (uint32_t)(&(baseAddr->SHIFTBUF[shifter])) + (sizeof(uint32_t) - byteCount);
    }

    return addr;
}

/*!
 * @brief Starts a Tx DMA transfer
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_StartTxDmaTransfer(eio_uart_state_t *state)
{
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    EIO_Type *baseAddr;
    dma_transfer_size_t dmaTransferSize;

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Configure the transfer control descriptor for the previously allocated channel */
    if (state->bitCount <= 8U)
    {
        dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    }
    else
    {
        dmaTransferSize = DMA_TRANSFER_SIZE_2B;
    }

    (void)DMA_DRV_ConfigTransfer(state->dmaChannel,
                           DMA_TRANSFER_MEM2PERIPH,
                           (uint32_t)(state->txData),
                           EIO_UART_DRV_ComputeTxRegAddr(state),
                           dmaTransferSize,
                           state->remainingBytes);

    /* Now that this transfer is set up, clear remaining bytes count */
    state->remainingBytes = 0U;

    /* Setup callback for DMA tx transfer end */
    (void)DMA_DRV_InstallCallback(state->dmaChannel,
                                  (dma_callback_t)(EIO_UART_DRV_EndDmaTxTransfer),
                                  (void*)(state));
    /* Start tx DMA channel */
    (void)DMA_DRV_StartChannel(state->dmaChannel);
    /* Enable EIO DMA requests */
    EIO_SetShifterDMARequest(baseAddr, (uint8_t)(1U << UART_TX_SHIFTER(resourceIndex)), true);
}

/*!
 * @brief Starts a Rx DMA transfer
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure
 * @return none
 */
static void EIO_UART_DRV_StartRxDmaTransfer(eio_uart_state_t *state)
{
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    EIO_Type *baseAddr;
    dma_transfer_size_t dmaTransferSize;

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Configure the transfer control descriptor for the previously allocated channel */
    if (state->bitCount <= 8U)
    {
        dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    }
    else
    {
        dmaTransferSize = DMA_TRANSFER_SIZE_2B;
    }

    (void)DMA_DRV_ConfigTransfer(state->dmaChannel,
                           DMA_TRANSFER_PERIPH2MEM,
                           EIO_UART_DRV_ComputeRxRegAddr(state),
                           (uint32_t)(state->rxData),
                           dmaTransferSize,
                           state->remainingBytes);

    /* Now that this transfer is set up, clear remaining bytes count */
    state->remainingBytes = 0U;

    /* Setup callback for DMA tx transfer end */
    (void)DMA_DRV_InstallCallback(state->dmaChannel,
                                  (dma_callback_t)(EIO_UART_DRV_EndDmaRxTransfer),
                                  (void*)(state));

    /* Enable EIO DMA requests */
    EIO_SetShifterDMARequest(baseAddr, (uint8_t)(1U << UART_RX_SHIFTER(resourceIndex)), true);

    /* Start tx DMA channel */
    (void)DMA_DRV_StartChannel(state->dmaChannel);
}

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
                           eio_uart_state_t *state)
{
    uint32_t inputClock;
    status_t clkErr;
    status_t retCode;
    status_t osifError = STATUS_SUCCESS;
    uint8_t dmaReq;
    EIO_Type *baseAddr;

    DEVICE_ASSERT(state != NULL);
    DEVICE_ASSERT(instance < EIO_INSTANCE_COUNT);
    /* Check that device was initialized */
    DEVICE_ASSERT(g_eioDeviceStatePtr[instance] != NULL);
    baseAddr = g_eioBase[state->eioCommon.instance];
    /* Get the protocol clock frequency */
    clkErr = CKGEN_DRV_GetFreq(g_eioClock[instance], &inputClock);
    DEVICE_ASSERT(clkErr == STATUS_SUCCESS);

    DEVICE_ASSERT(inputClock > 0U);

    /* Instruct the resource allocator that we need one shifter/timer */
    state->eioCommon.resourceCount = 1U;
    /* Common EIO driver initialization */
    retCode = EIO_DRV_InitDriver(instance, (eio_common_state_t *)state);
    if (retCode != STATUS_SUCCESS)
    {   /* Initialization failed, not enough resources */
        return retCode;
    }
    /* Initialize the semaphore */
    if (userConfigPtr->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        osifError = OSIF_SemaCreate(&(state->idleSemaphore), 0U);
        DEVICE_ASSERT(osifError == STATUS_SUCCESS);
    }

    /* Initialize driver-specific context structure */
    state->rxData = NULL;
    state->txData = NULL;
    state->remainingBytes = 0U;
    state->callback = userConfigPtr->callback;
    state->callbackParam = userConfigPtr->callbackParam;
    state->blocking = false;
    state->driverType = userConfigPtr->driverType;
    state->direction = userConfigPtr->direction;
    state->status = STATUS_SUCCESS;
    state->driverIdle = true;
    state->bitCount = userConfigPtr->bitCount;

    if (state->direction == EIO_UART_DIRECTION_TX)
    {
        if (state->driverType == EIO_DRIVER_TYPE_DMA)
        {
            /* Enable EIO DMA requests */
            EIO_SetShifterDMARequest(baseAddr, (uint8_t)(1U << UART_TX_SHIFTER(state->eioCommon.resourceIndex)), true);
        }
        /* Configure device for UART Tx mode */
        EIO_UART_DRV_ConfigureTx(state, userConfigPtr, inputClock);
    }
    else
    {
        if (state->driverType == EIO_DRIVER_TYPE_DMA)
        {
            /* Enable EIO DMA requests */
            EIO_SetShifterDMARequest(baseAddr, (uint8_t)(1U << UART_RX_SHIFTER(state->eioCommon.resourceIndex)), true);
        }
        /* Configure device for UART Rx mode */
        EIO_UART_DRV_ConfigureRx(state, userConfigPtr, inputClock);
    }

    /* Set up transfer engine */
    switch (state->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        if (state->direction == EIO_UART_DIRECTION_TX)
        {
            state->eioCommon.isr = EIO_UART_DRV_CheckStatusTx;
        }
        else
        {
            state->eioCommon.isr = EIO_UART_DRV_CheckStatusRx;
        }
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Nothing to do here, FLEXIO_UART_DRV_GetStatus() will handle the transfer */
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Store DMA channel number */
        state->dmaChannel = userConfigPtr->dmaChannel;
        /* Configure DMA request source */
        dmaReq = g_eioDMASrc[instance][UART_TX_SHIFTER(state->eioCommon.resourceIndex)];
        (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->dmaChannel, dmaReq, false);

        /* For Tx we will still need interrupt to signal end of transfer */
        if (state->direction == EIO_UART_DIRECTION_TX)
        {
            state->eioCommon.isr = EIO_UART_DRV_CheckStatusTx;
        }
        s_uartInit = true;
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    (void)clkErr;
    (void)osifError;
    return STATUS_SUCCESS;
}

/*!
 * @brief De-initialize the EIO_UART driver
 *        This function de-initializes the EIO_UART driver. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_Deinit(eio_uart_state_t *state)
{
    DEVICE_ASSERT(state != NULL);

    /* Check if driver is busy */
    if (!state->driverIdle)
    {
        return STATUS_BUSY;
    }

    /* Destroy the semaphore */
    if (state->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        (void)OSIF_SemaDestroy(&(state->idleSemaphore));
    }
    s_uartInit = false;
    return EIO_DRV_DeinitDriver((eio_common_state_t *)state);
}

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
status_t EIO_UART_DRV_SetConfig(eio_uart_state_t *state,
                                uint32_t baudRate,
                                uint8_t bitCount)
{
    EIO_Type *baseAddr;
    uint16_t divider;
    uint32_t inputClock;
    status_t clkErr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(state != NULL);
    DEVICE_ASSERT(baudRate > 0U);
    DEVICE_ASSERT(bitCount > 0U);
    DEVICE_ASSERT(bitCount <= 16U);
    /* For DMA transfers bitCount must 8 */
    DEVICE_ASSERT(!((state->driverType == EIO_DRIVER_TYPE_DMA) && (bitCount != 8U)));

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Check if driver is busy */
    if (!state->driverIdle)
    {
        return STATUS_BUSY;
    }
    /* Get the protocol clock frequency */
    clkErr = CKGEN_DRV_GetFreq(g_eioClock[state->eioCommon.instance], &inputClock);
    DEVICE_ASSERT(clkErr == STATUS_SUCCESS);
    DEVICE_ASSERT(inputClock > 0U);

    /* Compute divider */
    EIO_UART_DRV_ComputeBaudRateDivider(baudRate, &divider, inputClock);

    /* Configure tx/rx timer */
    EIO_SetTimerCompare(baseAddr, UART_TX_TIMER(resourceIndex), (uint16_t)((uint16_t)(((uint16_t)((uint16_t)bitCount << 1U) - 1U) << 8U) + divider));

    state->bitCount = bitCount;

    (void)clkErr;
    return STATUS_SUCCESS;
}

/*!
 * @brief Get the currently configured baud rate
 *        This function returns the currently configured UART baud rate.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @param[in] baudRate: the current baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_GetBaudRate(eio_uart_state_t *state, uint32_t *baudRate)
{
    const EIO_Type *baseAddr;
    uint32_t inputClock;
    uint16_t divider;
    uint16_t timerCmp;
    status_t clkErr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(state != NULL);
    DEVICE_ASSERT(baudRate != NULL);

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Get the protocol clock frequency */
    clkErr = CKGEN_DRV_GetFreq(g_eioClock[state->eioCommon.instance], &inputClock);
    DEVICE_ASSERT(clkErr == STATUS_SUCCESS);
    DEVICE_ASSERT(inputClock > 0U);

    /* Get the currently configured divider */
    timerCmp = EIO_GetTimerCompare(baseAddr, UART_TX_TIMER(resourceIndex));
    divider = (uint16_t)(timerCmp & 0x00FFU);

    /* Compute baud rate: input_clock / (2 * (divider + 1)). Round to nearest integer */
    *baudRate = (inputClock + (uint32_t)divider + 1U) / (2U * ((uint32_t)divider + 1U));

    (void)clkErr;
    return STATUS_SUCCESS;
}

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
                               uint32_t txSize)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(state != NULL);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);
    DEVICE_ASSERT(state->direction == EIO_UART_DIRECTION_TX);
    /* If 2 bytes per word are needed, then the size must be even */
    DEVICE_ASSERT((state->bitCount <= 8U) || ((txSize & 1U) == 0U));

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Check if driver is busy */
    if (!state->driverIdle)
    {
        return STATUS_BUSY;
    }

    state->txData = (uint8_t *)txBuff;
    state->remainingBytes = txSize;
    state->status = STATUS_BUSY;
    state->driverIdle = false;
    /* Number of bytes to flush after the last byte is copied in the tx shifter buffer */
    state->txFlush = (uint8_t)((txSize == 1U) ? 1U : 2U);
    /* Enable transfer engine */
    switch (state->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Enable timers and shifters */
        EIO_UART_DRV_EnableTransfer(state);
        /* Enable interrupts for Tx shifter */
        EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << UART_TX_SHIFTER(resourceIndex)), true);
        EIO_SetShifterErrorInterrupt(baseAddr, (uint8_t)(1U << UART_TX_SHIFTER(resourceIndex)), true);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Enable timers and shifters */
        EIO_UART_DRV_EnableTransfer(state);
        /* Call EIO_UART_DRV_CheckStatus once to send the first byte */
        EIO_UART_DRV_CheckStatus(state);
        break;

    case EIO_DRIVER_TYPE_DMA:
        if (s_uartInit == true)
        {
            EIO_UART_DRV_StartTxDmaTransfer(state);
            EIO_ClearShifterStatus(baseAddr, UART_TX_SHIFTER(resourceIndex));
            s_uartInit = false;
        }
        else
        {
            if (state->remainingBytes == 1U)
            {
                EIO_WriteShifterBuffer(baseAddr, UART_TX_SHIFTER(resourceIndex), (*(state->txData)), EIO_SHIFTER_RW_MODE_NORMAL);
                state->remainingBytes = 0U;
                EIO_ClearTimerStatus(baseAddr, UART_TX_TIMER(state->eioCommon.resourceIndex));
                EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << UART_TX_TIMER(state->eioCommon.resourceIndex)), true);
            }
            else
            {
                state->remainingBytes -= 1U;
                state->txFlush = (uint8_t)((state->remainingBytes == 1U) ? 1U : 2U);
                EIO_WriteShifterBuffer(baseAddr, UART_TX_SHIFTER(resourceIndex), (*(state->txData)), EIO_SHIFTER_RW_MODE_NORMAL);
                state->txData++;
                EIO_UART_DRV_StartTxDmaTransfer(state);
                EIO_ClearShifterStatus(baseAddr, UART_TX_SHIFTER(resourceIndex));
            }
        }
        /* Enable timers and shifters */
        EIO_UART_DRV_EnableTransfer(state);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    return STATUS_SUCCESS;
}

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
                                       uint32_t timeout)
{
    /* Check if driver is busy */
    if (!state->driverIdle)
    {
        return STATUS_BUSY;
    }
    /* Mark transfer as blocking */
    if (state->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        state->blocking = true;
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemaWait(&(state->idleSemaphore), 0U);
    }
    /* Call EIO_UART_DRV_SendData to start transfer */
    /* We already did the busy check so no need to check return code of EIO_UART_DRV_SendData */
    (void)EIO_UART_DRV_SendData(state, txBuff, txSize);

    /* Wait for transfer to end */
    return EIO_UART_DRV_WaitTransferEnd(state, timeout);
}

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
                                  uint32_t rxSize)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(state != NULL);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);
    DEVICE_ASSERT(state->direction == EIO_UART_DIRECTION_RX);
    /* If 2 bytes per word are needed, then the size must be even */
    DEVICE_ASSERT((state->bitCount <= 8U) || ((rxSize & 1U) == 0U));

    baseAddr = g_eioBase[state->eioCommon.instance];
    resourceIndex = state->eioCommon.resourceIndex;

    /* Check if driver is busy */
    if (!state->driverIdle)
    {
        return STATUS_BUSY;
    }

    state->rxData = rxBuff;
    state->remainingBytes = rxSize;
    state->status = STATUS_BUSY;
    state->driverIdle = false;

    /* Enable timers and shifters */
    EIO_UART_DRV_EnableTransfer(state);
    /* Enable transfer engine */
    switch (state->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Enable interrupts for Rx shifter */
        EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << UART_RX_SHIFTER(resourceIndex)), true);
        EIO_SetShifterErrorInterrupt(baseAddr, (uint8_t)(1U << UART_RX_SHIFTER(resourceIndex)), true);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Call EIO_UART_DRV_CheckStatus once to send the first byte */
        EIO_UART_DRV_CheckStatus(state);
        break;

    case EIO_DRIVER_TYPE_DMA:
        EIO_UART_DRV_StartRxDmaTransfer(state);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    return STATUS_SUCCESS;
}

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
        uint8_t *rxBuff,
        uint32_t rxSize,
        uint32_t timeout)
{
    /* Check if driver is busy */
    if (!state->driverIdle)
    {
        return STATUS_BUSY;
    }
    /* Mark transfer as blocking */
    if (state->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        state->blocking = true;
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemaWait(&(state->idleSemaphore), 0U);
    }
    /* Call EIO_UART_DRV_ReceiveData to start transfer */
    /* We already did the busy check so no need to check return code of EIO_UART_DRV_ReceiveData */
    (void)EIO_UART_DRV_ReceiveData(state, rxBuff, rxSize);

    /* Wait for transfer to end */
    return EIO_UART_DRV_WaitTransferEnd(state, timeout);
}

/*!
 * @brief Aborts a non-blocking UART transfer
 *        This function aborts a non-blocking UART transfer.
 *
 * @param[in] state: Pointer to the EIO_UART driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_TransferAbort(eio_uart_state_t *state)
{
    DEVICE_ASSERT(state != NULL);

    /* Check if driver is busy */
    if (state->driverIdle)
    {
        return STATUS_SUCCESS;
    }

    state->status = STATUS_UART_ABORTED;
    EIO_UART_DRV_StopTransfer(state);

    return STATUS_SUCCESS;
}

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
 *        @note In DMA mode, this param[in]eter may not be accurate, in case the transfer completes
 *              right after calling this function; in this edge-case, the param[in]eter will reflect
 *              the initial transfer size, due to automatic reloading of the major loop count
 *              in the DMA transfer descriptor.
 * @return Error or success status returned by API
 */
status_t EIO_UART_DRV_GetStatus(eio_uart_state_t *state, uint32_t *bytesRemaining)
{
    uint32_t remainingBytes;

    DEVICE_ASSERT(state != NULL);

    /* Initialize with the actual remaining bytes count */
    remainingBytes = state->remainingBytes;

    if (!state->driverIdle)
    {
        switch (state->driverType)
        {
        case EIO_DRIVER_TYPE_POLLING:
            /* In polling mode advance the UART transfer here */
            EIO_UART_DRV_CheckStatus(state);
            break;

        case EIO_DRIVER_TYPE_DMA:
            /* In DMA mode just update the remaining count.
            DO NOT write state->remainingBytes directly !!! */
            remainingBytes = DMA_DRV_GetRemainingBytes(state->dmaChannel);
            break;

        default:
            /* Nothing */
            break;
        }
    }

    if (bytesRemaining != NULL)
    {
        *bytesRemaining = remainingBytes;
    }

    if (!state->driverIdle)
    {
        return STATUS_BUSY;
    }
    else
    {
        return state->status;
    }
}

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
                                  uint32_t rxSize)
{
    DEVICE_ASSERT(state != NULL);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    state->rxData = rxBuff;
    state->remainingBytes = rxSize;

    return STATUS_SUCCESS;
}

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
                                  uint32_t txSize)
{
    DEVICE_ASSERT(state != NULL);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    state->txData = (uint8_t *)txBuff;
    state->remainingBytes = txSize;
    /* In case of continuous transmission there are always 2 bytes to flush */
    state->txFlush = 2;

    return STATUS_SUCCESS;
}

/*!
 * @brief Returns default configuration structure for EIO_UART.
 *
 * @param[in] userConfigPtr: Pointer to the EIO_UART user configuration structure
 * @return none
 */
void EIO_UART_DRV_GetDefaultConfig(eio_uart_user_config_t *userConfigPtr)
{
    DEVICE_ASSERT(userConfigPtr != NULL);

    userConfigPtr->driverType = EIO_DRIVER_TYPE_INTERRUPTS;
    userConfigPtr->baudRate = 115200U;
    userConfigPtr->bitCount = 8U;
    userConfigPtr->direction = EIO_UART_DIRECTION_TX;
    userConfigPtr->dataPin = 0U;
    userConfigPtr->callback = NULL;
    userConfigPtr->callbackParam = NULL;
    userConfigPtr->dmaChannel = 255U;
}

/* =============================================  EOF  ============================================== */
