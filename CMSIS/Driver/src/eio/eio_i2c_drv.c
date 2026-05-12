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
 * @file eio_i2c_drv.c
 *
 * @brief This file provides eio i2c integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "eio_hw.h"
#include "eio_comm.h"
#include "eio_i2c_drv.h"
#include "ckgen_drv.h"
#include "dma_drv.h"

/* ============================================  Define  ============================================ */
/*!
 * @brief Constraints used for baud rate computation
 */
#define DIVIDER_MIN_VALUE  (1)
#define DIVIDER_MAX_VALUE  (0xFF)

/*!
 * @brief Shifters/Timers used for I2C simulation The param[in]eter x represents the
 * resourceIndex value for the current driver instance
 */
#define I2C_TX_SHIFTER(x)     (x)
#define I2C_RX_SHIFTER(x)     (uint8_t)((x) + 1UL)
#define I2C_SCL_TIMER(x)      (x)
#define I2C_CONTROL_TIMER(x)  (uint8_t)((x) + 1UL)

#define EIO_IIC_TIMEOUT_VALUE (0x20000UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Computes the baud rate divider for a target baud rate.
 *
 * @param[in] baudRate: The desired baud rate in hertz
 * @param[out] divider: Point to baud rate divider
 * @param[in] inputClock: The frequency of the input clock
 * @return none
 */
static void EIO_I2C_DRV_MasterComputeBaudRateDivider(uint32_t baudRate, uint16_t *divider, uint32_t inputClock)
{
    int32_t tmpDiv;

    /* Compute divider: ((input_clock / baud_rate) / 2) - 1 - 1. The extra -1 is from the
       timer reset setting used for clock stretching. Round to nearest integer */
    tmpDiv = (((int32_t)inputClock + (int32_t)baudRate) / (2 * (int32_t)baudRate)) - 2;
    /* Enforce upper/lower limits */
    if (tmpDiv < DIVIDER_MIN_VALUE)
    {
        tmpDiv = DIVIDER_MIN_VALUE;
    }
    if (tmpDiv > DIVIDER_MAX_VALUE)
    {
        tmpDiv = DIVIDER_MAX_VALUE;
    }

    *divider = (uint16_t)tmpDiv;
}

/*!
 * @brief configures the EIO module as I2C master
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @param[in] inputClock: The frequency of the input clock
 * @param[in] baudRate: The desired baud rate in hertz
 * @return none
 */
static void EIO_I2C_DRV_MasterConfigure(const eio_i2c_master_state_t *master, uint32_t inputClock, uint32_t baudRate)
{
    EIO_Type *baseAddr;
    uint16_t divider;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Compute divider.*/
    EIO_I2C_DRV_MasterComputeBaudRateDivider(baudRate, &divider, inputClock);

    /* Configure tx shifter */
    EIO_SetShifterConfig(baseAddr,
                         I2C_TX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_0,
                         EIO_SHIFTER_STOP_BIT_1,
                         EIO_SHIFTER_SOURCE_PIN);
    /* Shifter disabled and pin enabled causes the pin to be held low.
       So disable pin too, will be enabled at transfer time.  */
    EIO_SetShifterControl(baseAddr,
                          I2C_TX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_TRANSMIT,
                          master->sdaPin,                    /* output on SDA pin */
                          EIO_PIN_POLARITY_LOW,
                          EIO_PIN_CONFIG_OPEN_DRAIN,
                          I2C_CONTROL_TIMER(resourceIndex),                     /* use control timer to drive the shifter */
                          EIO_TIMER_POLARITY_POSEDGE);

    /* Configure rx shifter */
    EIO_SetShifterConfig(baseAddr,
                         I2C_RX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_DISABLED,
                         EIO_SHIFTER_STOP_BIT_0,
                         EIO_SHIFTER_SOURCE_PIN);
    EIO_SetShifterControl(baseAddr,
                          I2C_RX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_RECEIVE,
                          master->sdaPin,                    /* input from SDA pin */
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_DISABLED,
                          I2C_CONTROL_TIMER(resourceIndex),                     /* use control timer to drive the shifter */
                          EIO_TIMER_POLARITY_NEGEDGE);

    /* Configure SCL timer */
    EIO_SetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex), divider);
    EIO_SetTimerConfig(baseAddr,
                       I2C_SCL_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_ENABLED,
                       EIO_TIMER_STOP_BIT_TIM_DIS,
                       EIO_TIMER_ENABLE_TRG_HIGH,         /* enable when Tx data is available */
                       EIO_TIMER_DISABLE_NEVER,
                       EIO_TIMER_RESET_PIN_OUT,           /* reset if output equals pin (for clock stretching) */
                       EIO_TIMER_DECREMENT_CLK_SHIFT_TMR, /* decrement on EIO clock */
                       EIO_TIMER_INITOUT_ZERO);
    EIO_SetTimerControl(baseAddr,
                        I2C_SCL_TIMER(resourceIndex),
                        (uint8_t)((uint8_t)(I2C_TX_SHIFTER(resourceIndex) << 2U) + 1U), /* trigger on tx shifter status flag */
                        EIO_TRIGGER_POLARITY_LOW,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        master->sclPin,                  /* output on SCL pin */
                        EIO_PIN_POLARITY_HIGH,
                        EIO_PIN_CONFIG_OPEN_DRAIN,    /* enable output */
                        EIO_TIMER_MODE_DISABLED);

    /* Configure control timer for shifters */
    EIO_SetTimerCompare(baseAddr, I2C_CONTROL_TIMER(resourceIndex), 0x000FU);
    EIO_SetTimerConfig(baseAddr,
                       I2C_CONTROL_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_ENABLED,
                       EIO_TIMER_STOP_BIT_TIM_CMP,
                       EIO_TIMER_ENABLE_TIM_ENABLE,       /* enable on SCL timer enable */
                       EIO_TIMER_DISABLE_TIM_DISABLE,     /* disable on SCL timer disable */
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_PIN_SHIFT_PIN, /* decrement on SCL pin input */
                       EIO_TIMER_INITOUT_ONE);
    EIO_SetTimerControl(baseAddr,
                        I2C_CONTROL_TIMER(resourceIndex),
                        (uint8_t)((uint8_t)(I2C_TX_SHIFTER(resourceIndex) << 2U) + 1U), /* trigger on tx shifter status flag */
                        EIO_TRIGGER_POLARITY_LOW,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        master->sclPin,                      /* use SCL pin as input */
                        EIO_PIN_POLARITY_LOW,
                        EIO_PIN_CONFIG_DISABLED,
                        EIO_TIMER_MODE_DISABLED);
}

/*!
 * @brief configures the number of SCL clocks needed for the entire transmission
 *
 * @param[in] baseAddr: EIO base pointer
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_MasterSetBytesNo(EIO_Type *baseAddr, eio_i2c_master_state_t *master)
{
    uint16_t timerCmp;
    uint32_t edgeNo;        /* total number of clock edges */
    uint32_t counter;       /* edge counter per timer reload */
    uint32_t reloads;       /* number of timer reloads */
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = master->eioCommon.resourceIndex;
    /* Compute number of SCL edges, including address */
    edgeNo = (master->txRemainingBytes * 18U) + 2U;
    reloads = (uint32_t)((edgeNo + 255U) / 256U);
    counter = (uint32_t)((uint32_t)(edgeNo + (reloads - 1)) / (uint32_t)reloads); /* round up */

    /* Set number of ticks in high part of timer compare register */
    timerCmp = EIO_GetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex));
    timerCmp = (uint16_t)((timerCmp & 0x00FFU) | (uint16_t)(((counter - 1U) & 0xFFU) << 8U));
    EIO_SetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex), timerCmp);

    /* Store reload information */
    master->eventCount = (uint16_t) reloads;
    master->lastReload = (uint8_t)(edgeNo - ((reloads - 1U) * counter) - 1U);
    /* Handle no reload case */
    if (reloads == 1U)
    {
        EIO_SetTimerDisable(baseAddr, I2C_SCL_TIMER(resourceIndex), EIO_TIMER_DISABLE_TIM_CMP);
    }
}

/*!
 * @brief send address byte
 *
 * @param[in] baseAddr: EIO base pointer
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_MasterSendAddress(EIO_Type *baseAddr, const eio_i2c_master_state_t *master)
{
    uint8_t addrByte;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = master->eioCommon.resourceIndex;
    /* Address byte: slave 7-bit address + D = 0(transmit) or 1 (receive) */
    addrByte = (uint8_t)((uint8_t)(master->slaveAddress << 1U) + (uint8_t)(master->receive ? 1U : 0U));
    EIO_WriteShifterBuffer(baseAddr, I2C_TX_SHIFTER(resourceIndex), (uint32_t)addrByte << 24U, EIO_SHIFTER_RW_MODE_BIT_SWAP);
}

/*!
 * @brief Handles data reception
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_ReadData(eio_i2c_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t data;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Read data from rx shifter */
    data = (uint8_t)EIO_ReadShifterBuffer(baseAddr, I2C_RX_SHIFTER(resourceIndex), EIO_SHIFTER_RW_MODE_BIT_SWAP);

    if (master->addrReceived == false)
    {
        /* This was the address byte */
        master->addrReceived = true;
        if (master->receive == true)
        {
            /* Send ACK from now on */
            EIO_SetShifterStopBit(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_0);
        }
    }
    else
    {
        master->rxRemainingBytes--;
        if (master->receive == true)
        {
            /* Put data in user buffer */
            *(master->rxData) = data;
            master->rxData++;
        }
    }
    if ((master->receive == true) && (master->rxRemainingBytes == 1U))
    {
        /* Send NACK for last byte */
        EIO_SetShifterStopBit(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_1);
        /* Also instruct rx shifter to expect NACK */
        EIO_SetShifterStopBit(baseAddr, I2C_RX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_1);
    }
}

/*!
 * @brief Handles data transmission
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_WriteData(eio_i2c_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint32_t data;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* If txRemainingBytes == 0 the transmission is over */
    if (master->txRemainingBytes == 0U)
    {
        return;
    }

    master->txRemainingBytes--;

    if (master->txRemainingBytes == 0U)
    {
        /* Done transmitting */
        if (master->sendStop == true)
        {
            /* Transmit stop condition */
            data = 0x00U;
        }
        else
        {
            /* Do not transmit stop condition */
            data = 0xFFU;
        }
    }
    else if (master->receive == true)
    {
        /* Transmit 0xFF to leave the line free while receiving */
        data = 0xFFU;
    }
    else
    {
        /* Read data from user buffer */
        data =  *(master->txData);
        master->txData++;
    }

    /* Shift data before bit-swapping it to get the relevant bits in the lower part of the shifter */
    data <<= 24U;
    EIO_WriteShifterBuffer(baseAddr, I2C_TX_SHIFTER(resourceIndex), data, EIO_SHIFTER_RW_MODE_BIT_SWAP);
}

/*!
 * @brief End the current transfer
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_MasterEndTransfer(eio_i2c_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Restore Rx stop bit, in case it was changed by a receive */
    EIO_SetShifterStopBit(baseAddr, I2C_RX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_0);
    /* Restore Tx stop bit, in case it was changed by a receive */
    EIO_SetShifterStopBit(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_1);
    /* Clear Rx status in case there is a character left in the buffer */
    EIO_ClearShifterStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex));

    /* Disable transfer engine */
    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Disable interrupts for Rx and Tx shifters */
        EIO_SetShifterInterrupt(baseAddr,
                                (uint8_t)((1U << I2C_TX_SHIFTER(resourceIndex)) | (1U << I2C_RX_SHIFTER(resourceIndex))),
                                false);
        EIO_SetShifterErrorInterrupt(baseAddr,
                                     (uint8_t)((1U << I2C_TX_SHIFTER(resourceIndex)) | (1U << I2C_RX_SHIFTER(resourceIndex))),
                                     false);
        /* Disable interrupt for SCL timer */
        EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << I2C_SCL_TIMER(resourceIndex)), false);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Nothing to do here */
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Disable error interrupt for Rx shifter */
        EIO_SetShifterErrorInterrupt(baseAddr, (uint8_t)(1U << I2C_RX_SHIFTER(resourceIndex)), false);
        /* Disable interrupt for SCL timer */
        EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << I2C_SCL_TIMER(resourceIndex)), false);
        /* Stop DMA channels */
        (void)DMA_DRV_StopChannel(master->txDMAChannel);
        (void)DMA_DRV_StopChannel(master->rxDMAChannel);
        /* Disable EIO DMA requests for both shifters */
        EIO_SetShifterDMARequest(baseAddr, (uint8_t)((1U << I2C_TX_SHIFTER(resourceIndex)) | (1U << I2C_RX_SHIFTER(resourceIndex))), false);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    master->driverIdle = true;

    /* Signal transfer end for blocking transfers */
    if (master->blocking == true)
    {
        (void)OSIF_SemaPost(&(master->idleSemaphore));
    }
}

/*!
 * @brief Enables timers and shifters to start a transfer
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_MasterEnableTransfer(eio_i2c_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = master->eioCommon.resourceIndex;
    baseAddr = g_eioBase[master->eioCommon.instance];

    /* enable timers and shifters */
    EIO_SetShifterMode(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_TRANSMIT);
    EIO_SetShifterMode(baseAddr, I2C_RX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_RECEIVE);

    EIO_SetTimerMode(baseAddr, I2C_CONTROL_TIMER(resourceIndex), EIO_TIMER_MODE_16BIT);
    EIO_SetTimerMode(baseAddr, I2C_SCL_TIMER(resourceIndex), EIO_TIMER_MODE_8BIT_BAUD);

    /* enable Tx pin */
    EIO_SetShifterPinConfig(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_PIN_CONFIG_OPEN_DRAIN);

}

/*!
 * @brief Forcefully stops the current transfer
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_MasterStopTransfer(eio_i2c_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = master->eioCommon.resourceIndex;
    baseAddr = g_eioBase[master->eioCommon.instance];

    EIO_SetTimerMode(baseAddr, I2C_SCL_TIMER(resourceIndex), EIO_TIMER_MODE_DISABLED);
    EIO_SetTimerMode(baseAddr, I2C_CONTROL_TIMER(resourceIndex), EIO_TIMER_MODE_DISABLED);

    /* clear any leftover error flags */
    EIO_ClearShifterErrorStatus(baseAddr, I2C_TX_SHIFTER(resourceIndex));
    EIO_ClearShifterErrorStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex));
    /* discard any leftover rx. data */
    EIO_ClearShifterStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex));
    /* Clear timer status */
    EIO_ClearTimerStatus(baseAddr, I2C_SCL_TIMER(resourceIndex));

    EIO_SetShifterStopBit(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_1);
    /* end the transfer */
    EIO_I2C_DRV_MasterEndTransfer(master);

    /* Check receive overflow */
    if ((master->rxRemainingBytes != 0U) && (master->status == STATUS_SUCCESS) && (master->receive == true) && \
        (master->driverType != EIO_DRIVER_TYPE_DMA))
    {
        master->status = STATUS_I2C_RX_OVERRUN;
    }
}

/*!
 * @brief waits for the end of a blocking transfer
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return none
 */
static status_t EIO_I2C_DRV_MasterWaitTransferEnd(eio_i2c_master_state_t *master, uint32_t timeout)
{
    status_t osifError = STATUS_SUCCESS;

    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Wait for transfer to be completed by the IRQ */
        osifError = OSIF_SemaWait(&(master->idleSemaphore), timeout);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Call EIO_I2C_DRV_MasterGetStatus() to do the transfer */
        while (EIO_I2C_DRV_MasterGetStatus(master, NULL) == STATUS_BUSY)
        {
        }
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Wait for transfer to be completed by the DMA IRQ */
        osifError = OSIF_SemaWait(&(master->idleSemaphore), timeout);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    /* blocking transfer is over */
    master->blocking = false;
    if (osifError == STATUS_TIMEOUT)
    {
        /* abort current transfer */
        master->status = STATUS_TIMEOUT;
        EIO_I2C_DRV_MasterStopTransfer(master);
    }

    return master->status;
}

/*!
 * @brief Checks if the current Rx shifter error is NACK or RX_OVERFLOW
 *        If there is a Tx event active it is an indication that module was not
 *        serviced for a long time - chances are this is an overflow.
 *        It is not certain, and it is even possible to have both NACK and overflow,
 *        but there is no way to tell, so we chose the safe option (if it is an
 *        overflow and we abort the transfer we may block the I2C bus).
 *
 * @param[in] baseAddr: EIO base pointer
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static inline bool EIO_I2C_DRV_MasterCheckNack(const EIO_Type *baseAddr, const eio_i2c_master_state_t *master)
{
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = master->eioCommon.resourceIndex;

    return !(EIO_GetShifterStatus(baseAddr, I2C_TX_SHIFTER(resourceIndex)));
}

/*!
 * @brief Check status of the I2C bus.
 *        If either SDA or SCL is low, the bus is busy.
 *
 * @param[in] baseAddr: EIO base pointer
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static inline bool EIO_I2C_DRV_MasterBusBusy(const EIO_Type *baseAddr, const eio_i2c_master_state_t *master)
{
    uint8_t pinMask = (uint8_t)((1U << master->sdaPin) | (1U << master->sclPin));
    if ((EIO_GetPinData(baseAddr) & pinMask) == pinMask)
    {
        /* both pins are high, bus is not busy */
        return false;
    }
    else
    {
        /* bus is busy */
        return true;
    }
}

/*!
 * @brief Check status of the I2C transfer. This function can be
 *        called either in an interrupt routine or directly in polling
 *        mode to advance the I2C transfer.
 *
 * @param[in] stateStruct: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_MasterCheckStatus(void *stateStruct)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    eio_i2c_master_state_t *master;
    uint16_t timerCmp;

    DEVICE_ASSERT(stateStruct != NULL);

    master = (eio_i2c_master_state_t *)stateStruct;
    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check for errors */
    if (EIO_GetShifterErrorStatus(baseAddr, I2C_TX_SHIFTER(resourceIndex)))
    {
        master->status = STATUS_I2C_TX_UNDERRUN;
        EIO_ClearShifterErrorStatus(baseAddr, I2C_TX_SHIFTER(resourceIndex));
        /* don't stop the transfer, continue processing events */
    }
    if (EIO_GetShifterErrorStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex)))
    {
        /* Device limitation: not possible to tell the difference between NACK and receive overflow */
        if (EIO_I2C_DRV_MasterCheckNack(baseAddr, master))
        {
            master->status = STATUS_I2C_RECEIVED_NACK;
            /* Force the transfer to stop */
            EIO_I2C_DRV_MasterStopTransfer(master);
            /* Call callback to announce the event to the user */
            if (master->callback != NULL)
            {
                master->callback(EIO_I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }

            return;
        }
        else
        {
            master->status = STATUS_I2C_RX_OVERRUN;
            EIO_ClearShifterErrorStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex));
            /* don't stop the transfer, continue processing events */
        }
    }
    /* Check if data was received */
    if (EIO_GetShifterStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex)))
    {
        EIO_I2C_DRV_ReadData(master);
    }
    /* Check if transmitter needs more data */
    if (EIO_GetShifterStatus(baseAddr, I2C_TX_SHIFTER(resourceIndex)))
    {
        EIO_I2C_DRV_WriteData(master);
        if (master->txRemainingBytes == 0U)
        {
            /* Done transmitting, disable Tx interrupt */
            EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << I2C_TX_SHIFTER(resourceIndex)), false);
        }
    }
    /* Check if the transfer is over */
    if (EIO_GetTimerStatus(baseAddr, I2C_SCL_TIMER(resourceIndex)))
    {
        master->eventCount--;
        /* Clear timer status */
        EIO_ClearTimerStatus(baseAddr, I2C_SCL_TIMER(resourceIndex));
        if (master->eventCount == 2U)
        {
            /* Adjust number of ticks in high part of timer compare register  for the last reload */
            timerCmp = EIO_GetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex));
            timerCmp = (uint16_t)((uint16_t)((uint32_t)timerCmp & 0x00FFU) | (uint16_t)(((uint32_t)(master->lastReload) & 0xFFU) << 8U));
            EIO_SetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex), timerCmp);
        }
        if (master->eventCount == 1U)
        {
            /* Timer will disable on the next countdown complete */
            EIO_SetTimerDisable(baseAddr, I2C_SCL_TIMER(resourceIndex), EIO_TIMER_DISABLE_TIM_CMP);
        }

        if (master->eventCount == 0U)
        {
            EIO_SetTimerDisable(baseAddr, I2C_SCL_TIMER(resourceIndex), EIO_TIMER_DISABLE_NEVER);

            /* Record success if there was no error */
            if (master->status == STATUS_BUSY)
            {
                master->status = STATUS_SUCCESS;
            }
            /* End transfer. In case of race condition between Tx_shifter and timer_end events,
               it is possible for the clock to be restarted. So we use forced stop to avoid this. */
            EIO_I2C_DRV_MasterStopTransfer(master);
            /* Call callback to announce the event to the user */
            if (master->callback != NULL)
            {
                master->callback(EIO_I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }
        }
    }
}

/*!
 * @brief Computes the address of the register used for DMA tx transfer
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return Computes the address of the register used for DMA tx transfer
 */
static inline uint32_t EIO_I2C_DRV_MasterComputeTxRegAddr(const eio_i2c_master_state_t *master)
{
    uint32_t addr;
    const EIO_Type *baseAddr;
    uint8_t shifter;

    baseAddr = g_eioBase[master->eioCommon.instance];
    shifter = I2C_TX_SHIFTER(master->eioCommon.resourceIndex);
    addr = (uint32_t)(&(baseAddr->SHIFTBUFBIS[shifter])) + (sizeof(uint32_t) - 1U);

    return addr;
}

/*!
 * @brief Computes the address of the register used for DMA rx transfer
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return Computes the address of the register used for DMA rx transfer
 */
static inline uint32_t EIO_I2C_DRV_MasterComputeRxRegAddr(const eio_i2c_master_state_t *master)
{
    uint32_t addr;
    const EIO_Type *baseAddr;
    uint8_t shifter;

    baseAddr = g_eioBase[master->eioCommon.instance];
    shifter = I2C_RX_SHIFTER(master->eioCommon.resourceIndex);
    addr = (uint32_t)(&(baseAddr->SHIFTBUFBIS[shifter]));

    return addr;
}

/*!
 * @brief End a DMA transfer
 *
 * @param[in] stateStruct: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_MasterEndDmaTransfer(void *stateStruct)
{
    eio_i2c_master_state_t *master;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    EIO_Type *baseAddr;
    uint16_t timerCmp;

    DEVICE_ASSERT(stateStruct != NULL);

    master = (eio_i2c_master_state_t *)stateStruct;
    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check for DMA transfer errors */
    if ((DMA_DRV_GetChannelStatus(master->txDMAChannel) == DMA_CHN_ERROR) ||
            (DMA_DRV_GetChannelStatus(master->rxDMAChannel) == DMA_CHN_ERROR))
    {
        master->status = STATUS_ERROR;
        /* Force the transfer to stop */
        EIO_I2C_DRV_MasterStopTransfer(master);
        /* Call callback to announce the event to the user */
        if (master->callback != NULL)
        {
            master->callback(EIO_I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
        }
        return;
    }

    /* Check for NACK */
    if (master->addrReceived == false)
    {
        /* This was the address byte */
        master->addrReceived = true;
        EIO_ClearShifterErrorStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex));
    }
    else if (EIO_GetShifterErrorStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex)))
    {
        EIO_ClearShifterErrorStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex));

        master->status = STATUS_I2C_RECEIVED_NACK;
        /* Force the transfer to stop */
        EIO_I2C_DRV_MasterStopTransfer(master);
        /* Call callback to announce the event to the user */
        if (master->callback != NULL)
        {
            master->callback(EIO_I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
        }

        return;
    }


    /* Check if the transfer is over */
    if (EIO_GetTimerStatus(baseAddr, I2C_SCL_TIMER(resourceIndex)))
    {
        master->eventCount--;
        /* Clear timer status */
        EIO_ClearTimerStatus(baseAddr, I2C_SCL_TIMER(resourceIndex));
        if (master->eventCount == 2U)
        {
            /* Adjust number of ticks in high part of timer compare register  for the last reload */
            timerCmp = EIO_GetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex));
            timerCmp = (uint16_t)((uint32_t)timerCmp & 0x00FFU) | (uint16_t)(((uint32_t)(master->lastReload) & 0xFFU) << 8U);
            EIO_SetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex), timerCmp);
        }
        if (master->eventCount == 1U)
        {
            /* Timer will disable on the next countdown complete */
            EIO_SetTimerDisable(baseAddr, I2C_SCL_TIMER(resourceIndex), EIO_TIMER_DISABLE_TIM_CMP);
        }

        if (master->eventCount == 0U)
        {
            EIO_SetTimerDisable(baseAddr, I2C_SCL_TIMER(resourceIndex), EIO_TIMER_DISABLE_NEVER);
            /* Record success if there was no error */
            if (master->status == STATUS_BUSY)
            {
                master->status = STATUS_SUCCESS;
            }
            /* End transfer */
            EIO_I2C_DRV_MasterStopTransfer(master);
            /* Call callback to announce the event to the user */
            if (master->callback != NULL)
            {
                master->callback(EIO_I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }
        }
    }
}

/*!
 * @brief end a DMA transfer call back
 *
 * @param[in] stateStruct: Pointer to the eio_I2C master driver context structure
 * @param[in] status: Channel status for DMA channel
 * @return none
 */
static void EIO_I2C_DRV_MasterDmaReceiveCallback(void *stateStruct, dma_chn_status_t status)
{
    DEVICE_ASSERT(stateStruct != NULL);

    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    EIO_Type *baseAddr;
    eio_i2c_master_state_t *master;

    master = (eio_i2c_master_state_t *)stateStruct;
    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    if ((master->rxRemainingBytes == 1) && (master->receive == true))
    {
        /* Send NACK for last byte */
        EIO_SetShifterStopBit(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_1);
        /* Also instruct rx shifter to expect NACK */
        EIO_SetShifterStopBit(baseAddr, I2C_RX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_1);
        /* wait for last byte transfer to shifter */
        uint32_t timeout = EIO_IIC_TIMEOUT_VALUE;
        while (EIO_GetShifterStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex)) == false)
        {
            timeout --;
            if (timeout == 0)
            {
                master->status = STATUS_ERROR;
                /* Force the transfer to stop */
                EIO_I2C_DRV_MasterStopTransfer(master);
                break;
            }
        }

        master->rxRemainingBytes --;
        /* Read data from rx shifter */
        master->rxData[(master->dmaReceiveBytes) - 1] = (uint8_t)EIO_ReadShifterBuffer(baseAddr, I2C_RX_SHIFTER(resourceIndex), EIO_SHIFTER_RW_MODE_BIT_SWAP);
    }

    EIO_I2C_DRV_MasterEndDmaTransfer(master);
}

/*!
 * @brief end a DMA transfer call back
 *
 * @param[in] stateStruct: Pointer to the eio_I2C master driver context structure
 * @param[in] status: Channel status for DMA channel
 * @return none
 */
static void EIO_I2C_DRV_MasterDmaSendCallback(void *stateStruct, dma_chn_status_t status)
{
    DEVICE_ASSERT(stateStruct != NULL);

    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    EIO_Type *baseAddr;
    eio_i2c_master_state_t *master;

    master = (eio_i2c_master_state_t *)stateStruct;
    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* last byte for stop condition */
    if (master->txRemainingBytes == 1)
    {
        uint32_t timeout = EIO_IIC_TIMEOUT_VALUE;
        /* wait for last byte transfer to shifter */
        while (EIO_GetShifterStatus(baseAddr, I2C_TX_SHIFTER(resourceIndex)) == false)
        {
            timeout --;
            if (timeout == 0)
            {
                master->status = STATUS_ERROR;
                /* Force the transfer to stop */
                EIO_I2C_DRV_MasterStopTransfer(master);
                break;
            }
        }

        EIO_I2C_DRV_WriteData(master);
    }
    EIO_I2C_DRV_MasterEndDmaTransfer(master);
}

/*!
 * @brief Starts a DMA transfer
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @return none
 */
static void EIO_I2C_DRV_MasterStartDmaTransfer(eio_i2c_master_state_t *master)
{
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    EIO_Type *baseAddr;

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    if (master->receive == true)
    {
        /* if receiving, send 0xFF to keep the line clear */
        master->dummyDmaIdle = 0xFFU;
        (void)DMA_DRV_ConfigTransfer(master->txDMAChannel,
                                     DMA_TRANSFER_MEM2PERIPH,
                                     (uint32_t)(&(master->dummyDmaIdle)),
                                     EIO_I2C_DRV_MasterComputeTxRegAddr(master),
                                     DMA_TRANSFER_SIZE_1B,
                                     (master->txRemainingBytes) - 1);

        DMA_DRV_SetSrcOffset(master->txDMAChannel, 0U);

        if (master->rxRemainingBytes == 1UL)
        {
            /* Send NACK for last byte */
            EIO_SetShifterStopBit(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_1);
            /* Also instruct rx shifter to expect NACK */
            EIO_SetShifterStopBit(baseAddr, I2C_RX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_1);
        }
        else
        {
            /* Send NACK for not last byte */
            EIO_SetShifterStopBit(baseAddr, I2C_TX_SHIFTER(resourceIndex), EIO_SHIFTER_STOP_BIT_0);
        }
    }
    else
    {
        (void)DMA_DRV_ConfigTransfer(master->txDMAChannel,
                                     DMA_TRANSFER_MEM2PERIPH,
                                     (uint32_t)(master->txData),
                                     EIO_I2C_DRV_MasterComputeTxRegAddr(master),
                                     DMA_TRANSFER_SIZE_1B,
                                     (master->txRemainingBytes) - 1);
    }
    /* last byte for stop condition */
    master->txRemainingBytes = 1U;

    /* Setup callback for DMA tx transfer end */
    (void)DMA_DRV_InstallCallback(master->txDMAChannel,
                                  (dma_callback_t)(EIO_I2C_DRV_MasterDmaSendCallback),
                                  (void*)(master));

    if (master->receive == true)
    {
        master->dmaReceiveBytes = master->rxRemainingBytes;

        if (master->rxRemainingBytes == 1UL)
        {
            (void)DMA_DRV_ConfigTransfer(master->rxDMAChannel,
                                         DMA_TRANSFER_PERIPH2MEM,
                                         EIO_I2C_DRV_MasterComputeRxRegAddr(master),
                                         (uint32_t)(master->rxData),
                                         DMA_TRANSFER_SIZE_1B,
                                         master->rxRemainingBytes);
            master->rxRemainingBytes = 0U;
        }
        else
        {
            (void)DMA_DRV_ConfigTransfer(master->rxDMAChannel,
                                         DMA_TRANSFER_PERIPH2MEM,
                                         EIO_I2C_DRV_MasterComputeRxRegAddr(master),
                                         (uint32_t)(master->rxData),
                                         DMA_TRANSFER_SIZE_1B,
                                         (master->rxRemainingBytes) - 1);
            /* Send NACK for last byte */
            master->rxRemainingBytes = 1U;
        }
    }
    else
    {
        (void)DMA_DRV_ConfigTransfer(master->rxDMAChannel,
                                     DMA_TRANSFER_PERIPH2MEM,
                                     EIO_I2C_DRV_MasterComputeRxRegAddr(master),
                                     (uint32_t)(&(master->dummyDmaReceive)),
                                     DMA_TRANSFER_SIZE_1B,
                                     master->rxRemainingBytes);

        DMA_DRV_SetDestOffset(master->rxDMAChannel, 0U);
        master->rxRemainingBytes = 0U;
    }

    /* Setup callback for DMA rx transfer end */
    (void)DMA_DRV_InstallCallback(master->rxDMAChannel,
                                  (dma_callback_t)(EIO_I2C_DRV_MasterDmaReceiveCallback),
                                  (void*)(master));
}

/*!
 * @brief Perform a send or receive transaction on the I2C bus
 *
 * @param[in] master: Pointer to the eio_I2C master driver context structure
 * @param[in] size: Data byte num
 * @param[in] sendStop: Whether to send stop bit
 * @param[in] receive: send or receive
 * @return Error or success status returned by API
 */
static status_t EIO_I2C_DRV_MasterStartTransfer(eio_i2c_master_state_t *master,
        uint32_t size,
        bool sendStop,
        bool receive)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(master != NULL);

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check if bus is busy */
    if (EIO_I2C_DRV_MasterBusBusy(baseAddr, master))
    {
        return STATUS_I2C_BUS_BUSY;
    }

    /* Tx - one extra byte for stop condition */
    master->txRemainingBytes = size + 1U;
    master->rxRemainingBytes = size;
    master->status = STATUS_BUSY;
    master->driverIdle = false;
    master->sendStop = sendStop;
    master->receive = receive;
    master->addrReceived = false;
    /* Configure device for requested number of bytes, keeping the existing baud rate */
    EIO_I2C_DRV_MasterSetBytesNo(baseAddr, master);
    /* Enable timers and shifters */
    EIO_I2C_DRV_MasterEnableTransfer(master);

    uint32_t timeout = EIO_IIC_TIMEOUT_VALUE;
    /* Enable transfer engine */
    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Send address to start transfer */
        EIO_I2C_DRV_MasterSendAddress(baseAddr, master);
        /* Enable interrupt for Tx and Rx shifters */
        EIO_SetShifterInterrupt(baseAddr,
                                (uint8_t)((1U << I2C_TX_SHIFTER(resourceIndex)) | (1U << I2C_RX_SHIFTER(resourceIndex))),
                                true);
        EIO_SetShifterErrorInterrupt(baseAddr,
                                     (uint8_t)((1U << I2C_TX_SHIFTER(resourceIndex)) | (1U << I2C_RX_SHIFTER(resourceIndex))),
                                     true);
        /* Enable interrupt for SCL timer */
        EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << I2C_SCL_TIMER(resourceIndex)), true);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Send address to start transfer */
        EIO_I2C_DRV_MasterSendAddress(baseAddr, master);
        /* Nothing to do here, EIO_I2C_DRV_MasterGetStatus() will handle the transfer */
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Enable error interrupt for Rx shifter - for NACK detection */
        EIO_SetShifterErrorInterrupt(baseAddr, (uint8_t)(1U << I2C_RX_SHIFTER(resourceIndex)), true);
        /* Enable interrupt for SCL timer - for end of transfer detection */
        EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << I2C_SCL_TIMER(resourceIndex)), true);
        /* Send address to start transfer */
        EIO_I2C_DRV_MasterSendAddress(baseAddr, master);
        /* Enable EIO DMA requests for tx shifters */
        EIO_SetShifterDMARequest(baseAddr, (uint8_t)(1U << I2C_TX_SHIFTER(resourceIndex)), true);
        EIO_I2C_DRV_MasterStartDmaTransfer(master);
        /* start tx dma channel */
        (void)DMA_DRV_StartChannel(master->txDMAChannel);
        /* discard any leftover rx. data by send slave address*/
        while (EIO_GetShifterStatus(baseAddr, I2C_RX_SHIFTER(resourceIndex)) == false)
        {
            timeout --;
            if (timeout == 0)
            {
                master->status = STATUS_ERROR;
                /* Force the transfer to stop */
                EIO_I2C_DRV_MasterStopTransfer(master);
                break;
            }
        }
        (void)EIO_ReadShifterBuffer(baseAddr, I2C_RX_SHIFTER(resourceIndex), EIO_SHIFTER_RW_MODE_BIT_SWAP);
        /* Enable EIO DMA requests for rx shifters */
        EIO_SetShifterDMARequest(baseAddr, (uint8_t)(1U << I2C_RX_SHIFTER(resourceIndex)), true);
        /* start rx dma channel */
        (void)DMA_DRV_StartChannel(master->rxDMAChannel);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    return STATUS_SUCCESS;
}

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
                                eio_i2c_master_state_t *master)
{
    uint32_t inputClock;
    status_t clkErr;
    status_t retCode;
    status_t osifError = STATUS_SUCCESS;
    uint8_t dmaReqTx;
    uint8_t dmaReqRx;

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(instance < EIO_INSTANCE_COUNT);

    /* Check that device was initialized */
    DEVICE_ASSERT(g_eioDeviceStatePtr[instance] != NULL);

    /* Get the protocol clock frequency */
    clkErr = CKGEN_DRV_GetFreq(g_eioClock[instance], &inputClock);
    DEVICE_ASSERT(clkErr == STATUS_SUCCESS);
    DEVICE_ASSERT(inputClock > 0U);

    /* Instruct the resource allocator that we need two shifters/timers */
    master->eioCommon.resourceCount = 2U;
    /* Common EIO driver initialization */
    retCode = EIO_DRV_InitDriver(instance, &(master->eioCommon));
    if (retCode != STATUS_SUCCESS)
    {   /* Initialization failed, not enough resources */
        return retCode;
    }

    /* Initialize the semaphore */
    if (userConfigPtr->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        osifError = OSIF_SemaCreate(&(master->idleSemaphore), 0U);
        DEVICE_ASSERT(osifError == STATUS_SUCCESS);
    }

    /* Initialize driver-specific context structure */
    master->driverType = userConfigPtr->driverType;
    master->slaveAddress = userConfigPtr->slaveAddress;
    master->sdaPin = userConfigPtr->sdaPin;
    master->sclPin = userConfigPtr->sclPin;
    master->callback = userConfigPtr->callback;
    master->callbackParam = userConfigPtr->callbackParam;
    master->blocking = false;
    master->driverIdle = true;
    master->status = STATUS_SUCCESS;

    /* Configure device for I2C mode */
    EIO_I2C_DRV_MasterConfigure(master, inputClock, userConfigPtr->baudRate);

    /* Set up transfer engine */
    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        master->eioCommon.isr = EIO_I2C_DRV_MasterCheckStatus;
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Nothing to do here, EIO_I2C_DRV_MasterGetStatus() will handle the transfer */
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Store DMA channel numbers */
        master->txDMAChannel = userConfigPtr->txDMAChannel;
        master->rxDMAChannel = userConfigPtr->rxDMAChannel;
        /* Configure DMA request sources */
        dmaReqTx = g_eioDMASrc[instance][I2C_TX_SHIFTER(master->eioCommon.resourceIndex)];
        dmaReqRx = g_eioDMASrc[instance][I2C_RX_SHIFTER(master->eioCommon.resourceIndex)];
        (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->txDMAChannel, dmaReqTx, false);
        (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->rxDMAChannel, dmaReqRx, false);

        /* For DMA transfers we use timer interrupts to signal transfer end */
        master->eioCommon.isr = EIO_I2C_DRV_MasterEndDmaTransfer;
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
 * @brief De-initialize the EIO_I2C master mode driver
 *        This function de-initializes the EIO_I2C driver in master mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterDeinit(eio_i2c_master_state_t *master)
{
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    /* Destroy the semaphore */
    if (master->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        (void)OSIF_SemaDestroy(&(master->idleSemaphore));
    }
    return EIO_DRV_DeinitDriver(&(master->eioCommon));
}

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
status_t EIO_I2C_DRV_MasterSetBaudRate(eio_i2c_master_state_t *master, uint32_t baudRate)
{
    EIO_Type *baseAddr;
    uint16_t divider;
    uint16_t timerCmp;
    uint32_t inputClock;
    status_t clkErr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(master != NULL);

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }
    /* Get the protocol clock frequency */
    clkErr = CKGEN_DRV_GetFreq(g_eioClock[master->eioCommon.instance], &inputClock);
    DEVICE_ASSERT(clkErr == STATUS_SUCCESS);
    DEVICE_ASSERT(inputClock > 0U);

    /* Compute divider */
    EIO_I2C_DRV_MasterComputeBaudRateDivider(baudRate, &divider, inputClock);

    /* Configure timer divider in the lower 8 bits of TIMCMP[CMP] */
    timerCmp = EIO_GetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex));
    timerCmp = (uint16_t)((timerCmp & 0xFF00U) | divider);
    EIO_SetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex), timerCmp);

    (void)clkErr;
    return STATUS_SUCCESS;
}

/*!
 * @brief Get the currently configured baud rate
 *        This function returns the currently configured I2C baud rate.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] baudRate: the current baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterGetBaudRate(eio_i2c_master_state_t *master, uint32_t *baudRate)
{
    const EIO_Type *baseAddr;
    uint32_t inputClock;
    uint16_t divider;
    uint16_t timerCmp;
    status_t clkErr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(baudRate != NULL);

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Get the protocol clock frequency */
    clkErr = CKGEN_DRV_GetFreq(g_eioClock[master->eioCommon.instance], &inputClock);
    DEVICE_ASSERT(clkErr == STATUS_SUCCESS);
    DEVICE_ASSERT(inputClock > 0U);

    /* Get the currently configured divider */
    timerCmp = EIO_GetTimerCompare(baseAddr, I2C_SCL_TIMER(resourceIndex));
    divider = (uint16_t)(timerCmp & 0x00FFU);

    /* Compute baud rate: input_clock / (2 * (divider + 2)). Round to nearest integer */
    *baudRate = (inputClock + divider + 2U) / (2U * ((uint32_t)divider + 2U));

    (void)clkErr;
    return STATUS_SUCCESS;
}

/*!
 * @brief Set the slave address for any subsequent I2C communication
 *        This function sets the slave address which will be used for any future transfer.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @param[in] address: slave address, 7-bit
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterSetSlaveAddr(eio_i2c_master_state_t *master, uint16_t address)
{
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    master->slaveAddress = address;

    return STATUS_SUCCESS;
}

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
                                    bool sendStop)
{
    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    /* Initialize transfer data */
    master->txData = txBuff;
    /* Start the transfer */
    return EIO_I2C_DRV_MasterStartTransfer(master, txSize, sendStop, false);
}

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
        const uint8_t *txBuff,
        uint32_t txSize,
        bool sendStop,
        uint32_t timeout)
{
    status_t status;

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    /* Mark transfer as blocking */
    if (master->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        master->blocking = true;
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemaWait(&(master->idleSemaphore), 0U);
    }
    master->txData = txBuff;
    /* Start the transfer */
    status = EIO_I2C_DRV_MasterStartTransfer(master, txSize, sendStop, false);
    if (status != STATUS_SUCCESS)
    {
        /* Transfer could not be started */
        master->blocking = false;
        return status;
    }

    /* Wait for transfer to end */
    return EIO_I2C_DRV_MasterWaitTransferEnd(master, timeout);
}

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
                                       bool sendStop)
{
    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    /* Initialize transfer data */
    master->rxData = rxBuff;
    /* Start the transfer */
    return EIO_I2C_DRV_MasterStartTransfer(master, rxSize, sendStop, true);
}

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
        uint32_t timeout)
{
    status_t status;

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    /* Mark transfer as blocking */
    if (master->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        master->blocking = true;
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemaWait(&(master->idleSemaphore), 0U);
    }
    /* Initialize transfer data */
    master->rxData = rxBuff;
    /* Start the transfer */
    status = EIO_I2C_DRV_MasterStartTransfer(master, rxSize, sendStop, true);
    if (status != STATUS_SUCCESS)
    {
        /* Transfer could not be started */
        master->blocking = false;
        return status;
    }

    /* Wait for transfer to end */
    return EIO_I2C_DRV_MasterWaitTransferEnd(master, timeout);
}


/*!
 * @brief Aborts a non-blocking I2C master transaction
 *        This function aborts a non-blocking I2C transfer.
 *
 * @param[in] master: Pointer to the EIO_I2C master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2C_DRV_MasterTransferAbort(eio_i2c_master_state_t *master)
{
    DEVICE_ASSERT(master != NULL);

    /* Warning: an ongoing transfer can't be aborted safely due to device limitation;
       there is no way to know the exact stage of the transfer, and if we disable the module
       during the ACK bit (transmit) or during a 0 data bit (receive) the slave will hold
       the SDA line low forever and block the I2C bus. NACK reception is the only exception,
       as there is no slave to hold the line low. Therefore this function should only be used
       in extreme circumstances, and the application must have a way to reset the I2C slave
    */

    /* Check if driver is busy */
    if (master->driverIdle)
    {
        return STATUS_SUCCESS;
    }

    master->status = STATUS_I2C_ABORTED;
    EIO_I2C_DRV_MasterStopTransfer(master);

    return STATUS_SUCCESS;
}

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
status_t EIO_I2C_DRV_MasterGetStatus(eio_i2c_master_state_t *master, uint32_t *bytesRemaining)
{
    uint32_t remainingBytes;

    DEVICE_ASSERT(master != NULL);

    /* Use rxRemainingBytes even for transmit; byte is not transmitted
       until rx shifter confirms the ACK */
    remainingBytes = master->rxRemainingBytes;

    if (!master->driverIdle)
    {
        switch (master->driverType)
        {
        case EIO_DRIVER_TYPE_POLLING:
            /* In polling mode advance the I2C transfer here */
            EIO_I2C_DRV_MasterCheckStatus(master);
            break;

        case EIO_DRIVER_TYPE_DMA:
            /* In DMA mode just update the remaining count.
            DO NOT write master->rxRemainingBytes directly !!! */
            //remainingBytes = EIO_DMA_DRV_GetRemainingMajorIterationsCount(master->rxDMAChannel);
            remainingBytes = DMA_DRV_GetRemainingBytes(master->rxDMAChannel);
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

    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }
    else
    {
        return master->status;
    }
}

/*!
 * @brief Returns default configuration structure for EIO_I2C
 *
 * @param[in] userConfigPtr: Pointer to the EIO_I2C user configuration structure.
 * @return none
 */
void EIO_I2C_DRV_GetDefaultConfig(eio_i2c_master_user_config_t *userConfigPtr)
{
    DEVICE_ASSERT(userConfigPtr != NULL);

    userConfigPtr->slaveAddress = 32U;
    userConfigPtr->driverType = EIO_DRIVER_TYPE_INTERRUPTS;
    userConfigPtr->baudRate = 100000U;
    userConfigPtr->sdaPin = 0U;
    userConfigPtr->sclPin = 1U;
    userConfigPtr->callback = NULL;
    userConfigPtr->callbackParam = NULL;
    userConfigPtr->rxDMAChannel = 255U;
    userConfigPtr->txDMAChannel = 255U;
}
/* =============================================  EOF  ============================================== */
