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
 * @file eio_spi_drv.c
 *
 * @brief This file provides eio spi module integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "eio_hw.h"
#include "eio_comm.h"
#include "eio_spi_drv.h"
#include "ckgen_drv.h"
#include "dma_drv.h"

/* ============================================  Define  ============================================ */
/* Constraints used for baud rate computation */
#define DIVIDER_MIN_VALUE (1)
#define DIVIDER_MAX_VALUE (0xFF)

/* Shifters/Timers used for SPI simulation The param[in]eter x represents the resourceIndex value for the current driver instance */
#define SPI_TX_SHIFTER(x)     (x)
#define SPI_RX_SHIFTER(x)     (uint8_t)((x) + 1UL)
#define SPI_SCK_TIMER(x)      (x)
#define SPI_SS_TIMER(x)       (uint8_t)((x) + 1UL)

/* Dummy data to send when user provides no data */
#define EIO_SPI_DUMMYDATA (0xFFFFFFFFUL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

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
static void EIO_SPI_DRV_MasterComputeBaudRateDivider(uint32_t baudRate,
                                                     uint16_t *divider,
                                                     uint32_t inputClock)
{
    int32_t tmpDiv;

    /* Compute divider: ((input_clock / baud_rate) / 2) - 1. Round to nearest integer */
    tmpDiv = (((int32_t)inputClock + (int32_t)baudRate) / (2 * (int32_t)baudRate)) - 1;
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
 * @brief configures the EIO module as SPI master
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @param[in] userConfigPtr: Pointer to the eio_SPI user configuration structure
 * @param[in] inputClock: The frequency of the input clock
 * @return none
 */
static void EIO_SPI_DRV_MasterConfigure(const eio_spi_master_state_t *master,
                                        const eio_spi_master_user_config_t *userConfigPtr,
                                        uint32_t inputClock)
{
    EIO_Type *baseAddr;
    eio_pin_polarity_t clockPolarity;        /* Polarity of clock signal */
    eio_timer_polarity_t txShifterPolarity;  /* Polarity of MOSI shifter */
    eio_timer_polarity_t rxShifterPolarity;  /* Polarity of MISO shifter */
    uint16_t divider;
    eio_shifter_stop_t stopBit;
    eio_shifter_start_t startBit;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;
    /* Compute divider.*/
    EIO_SPI_DRV_MasterComputeBaudRateDivider(userConfigPtr->baudRate, &divider, inputClock);
    /* add number of bits in the upper 8 bits of the divider. Formula is: TIMCMP[15:8] = (number of bits x 2) - 1 */
    divider += (uint16_t)((((uint16_t)(userConfigPtr->transferSize) * 8U * 2U) - 1U) << 8U);

    if (userConfigPtr->clockPolarity == 0U)
    {
        /* CPOL = 0 */
        clockPolarity = EIO_PIN_POLARITY_HIGH;
    }
    else
    {
        /* CPOL = 1 */
        clockPolarity = EIO_PIN_POLARITY_LOW;
    }

    if (userConfigPtr->clockPhase == 0U)
    {
        /* CPHA = 0 */
        txShifterPolarity = EIO_TIMER_POLARITY_NEGEDGE;
        rxShifterPolarity = EIO_TIMER_POLARITY_POSEDGE;
        stopBit = EIO_SHIFTER_STOP_BIT_DISABLED;
        startBit = EIO_SHIFTER_START_BIT_DISABLED;
    }
    else
    {
        /* CPHA = 1 */
        txShifterPolarity = EIO_TIMER_POLARITY_POSEDGE;
        rxShifterPolarity = EIO_TIMER_POLARITY_NEGEDGE;
        stopBit = EIO_SHIFTER_STOP_BIT_0;
        startBit = EIO_SHIFTER_START_BIT_DISABLED_SH;
    }
    /* Configure Tx shifter (MOSI) */
    EIO_SetShifterControl(baseAddr,
                          SPI_TX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->mosiPin,
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_OUTPUT,
                          SPI_SCK_TIMER(resourceIndex),
                          txShifterPolarity);
    EIO_SetShifterConfig(baseAddr,
                         SPI_TX_SHIFTER(resourceIndex),
                         startBit,
                         stopBit,
                         EIO_SHIFTER_SOURCE_PIN);

    /* Configure Rx shifter (MISO) */
    EIO_SetShifterControl(baseAddr,
                          SPI_RX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->misoPin,
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_DISABLED,
                          SPI_SCK_TIMER(resourceIndex),
                          rxShifterPolarity);
    EIO_SetShifterConfig(baseAddr,
                         SPI_RX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_DISABLED,
                         EIO_SHIFTER_STOP_BIT_DISABLED,
                         EIO_SHIFTER_SOURCE_PIN);

    /* Configure sck timer */
    EIO_SetTimerCompare(baseAddr, SPI_SCK_TIMER(resourceIndex), divider); /* set baud rate, and number of bits */
    EIO_SetTimerConfig(baseAddr,
                       SPI_SCK_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_ENABLED,
                       EIO_TIMER_STOP_BIT_TIM_DIS,
                       EIO_TIMER_ENABLE_TRG_HIGH,
                       EIO_TIMER_DISABLE_TIM_CMP,
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_CLK_SHIFT_TMR,
                       EIO_TIMER_INITOUT_ZERO);
    EIO_SetTimerControl(baseAddr,
                        SPI_SCK_TIMER(resourceIndex),
                        (uint8_t)((uint8_t)(SPI_TX_SHIFTER(resourceIndex) << 2U) + 1U), /* trigger on tx shifter status flag */
                        EIO_TRIGGER_POLARITY_LOW,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        userConfigPtr->sckPin,                     /* output on clock pin */
                        clockPolarity,                      /* used configured polarity */
                        EIO_PIN_CONFIG_OUTPUT,
                        EIO_TIMER_MODE_DISABLED);

    /* Configure SS timer */
    EIO_SetTimerCompare(baseAddr, SPI_SS_TIMER(resourceIndex), 0xFFFFU);
    EIO_SetTimerConfig(baseAddr,
                       SPI_SS_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_DISABLED,
                       EIO_TIMER_STOP_BIT_DISABLED,
                       EIO_TIMER_ENABLE_TIM_ENABLE,    /* enable when SCK timer is enabled */
                       EIO_TIMER_DISABLE_TIM_DISABLE,  /* disable when SCK timer is disabled */
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_CLK_SHIFT_TMR,
                       EIO_TIMER_INITOUT_ONE);
    EIO_SetTimerControl(baseAddr,
                        SPI_SS_TIMER(resourceIndex),
                        0U,                 /* trigger not used, using defaults */
                        EIO_TRIGGER_POLARITY_HIGH,
                        EIO_TRIGGER_SOURCE_EXTERNAL,
                        userConfigPtr->ssPin,
                        EIO_PIN_POLARITY_LOW,
                        EIO_PIN_CONFIG_OUTPUT,
                        EIO_TIMER_MODE_DISABLED);
}

/*!
 * @brief end a transfer
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static void EIO_SPI_DRV_MasterEndTransfer(eio_spi_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(master != NULL);

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Disable transfer engine */
    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Disable interrupts for Rx and Tx shifters */
        EIO_SetShifterInterrupt(baseAddr,
                                (uint8_t)((1U << SPI_TX_SHIFTER(resourceIndex)) | (1U << SPI_RX_SHIFTER(resourceIndex))),
                                false);
        EIO_SetShifterErrorInterrupt(baseAddr,
                                     (uint8_t)((1U << SPI_TX_SHIFTER(resourceIndex)) | (1U << SPI_RX_SHIFTER(resourceIndex))),
                                     false);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Nothing to do here */
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Stop DMA channels */
        (void)DMA_DRV_StopChannel(master->txDMAChannel);
        (void)DMA_DRV_StopChannel(master->rxDMAChannel);
        /* Disable EIO DMA requests for both shifters */
        EIO_SetShifterDMARequest(baseAddr,
                                (uint8_t)((1U << SPI_TX_SHIFTER(resourceIndex)) | (1U << SPI_RX_SHIFTER(resourceIndex))),
                                 false);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    master->driverIdle = true;
    master->txRemainingBytes = 0U;
    master->rxRemainingBytes = 0U;

    /* Signal transfer end for blocking transfers */
    if (master->blocking == true)
    {
        (void)OSIF_SemaPost(&(master->idleSemaphore));
    }
}


/*!
 * @brief Enables timers and shifters to start a transfer
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static void EIO_SPI_DRV_MasterEnableTransfer(eio_spi_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    resourceIndex = master->eioCommon.resourceIndex;
    baseAddr = g_eioBase[master->eioCommon.instance];
    EIO_SetShifterMode(baseAddr, SPI_TX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_TRANSMIT);
    EIO_SetShifterMode(baseAddr, SPI_RX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_RECEIVE);
    if (master->master)
    {
        EIO_SetTimerMode(baseAddr, SPI_SS_TIMER(resourceIndex), EIO_TIMER_MODE_16BIT);
        EIO_SetTimerMode(baseAddr, SPI_SCK_TIMER(resourceIndex), EIO_TIMER_MODE_8BIT_BAUD);
    }
    else
    {
        EIO_SetTimerMode(baseAddr, SPI_SCK_TIMER(resourceIndex), EIO_TIMER_MODE_16BIT);
    }
}

/*!
 * @brief Forcefully stops the current transfer
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static void EIO_SPI_DRV_MasterStopTransfer(eio_spi_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = master->eioCommon.resourceIndex;
    baseAddr = g_eioBase[master->eioCommon.instance];
    /* disable and re-enable timers and shifters to reset them */
    if (master->master)
    {
        EIO_SetTimerMode(baseAddr, SPI_SCK_TIMER(resourceIndex), EIO_TIMER_MODE_DISABLED);
        EIO_SetTimerMode(baseAddr, SPI_SS_TIMER(resourceIndex), EIO_TIMER_MODE_DISABLED);
    }
    else
    {
        EIO_SetTimerMode(baseAddr, SPI_SCK_TIMER(resourceIndex), EIO_TIMER_MODE_DISABLED);
    }
    /* clear any leftover error flags */
    EIO_ClearShifterErrorStatus(baseAddr, SPI_TX_SHIFTER(resourceIndex));
    EIO_ClearShifterErrorStatus(baseAddr, SPI_RX_SHIFTER(resourceIndex));
    /* discard any leftover rx. data */
    EIO_ClearShifterStatus(baseAddr, SPI_RX_SHIFTER(resourceIndex));
    /* end the transfer */
    EIO_SPI_DRV_MasterEndTransfer(master);
}

/*!
 * @brief waits for the end of a blocking transfer
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return none
 */
static status_t EIO_SPI_DRV_MasterWaitTransferEnd(eio_spi_master_state_t *master, uint32_t timeout)
{
    status_t osifError = STATUS_SUCCESS;

    DEVICE_ASSERT(master != NULL);

    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Wait for transfer to be completed by the IRQ */
        osifError = OSIF_SemaWait(&(master->idleSemaphore), timeout);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Call EIO_SPI_DRV_MasterGetStatus() to do the transfer */
        while (EIO_SPI_DRV_MasterGetStatus(master, NULL) == STATUS_BUSY)
        {
        }
        break;

    case EIO_DRIVER_TYPE_DMA:
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
        EIO_SPI_DRV_MasterStopTransfer(master);
    }

    return master->status;
}

/*!
 * @brief end a DMA transfer
 *
 * @param[in] stateStruct: Pointer to the eio_SPI master driver context structure
 * @param[in] status: Channel status for DMA channel
 * @return none
 */
static void EIO_SPI_DRV_MasterEndDmaTransfer(void *stateStruct, dma_chn_status_t status)
{
    eio_spi_master_state_t *master;

    DEVICE_ASSERT(stateStruct != NULL);

    master = (eio_spi_master_state_t *)stateStruct;

    /* Record success if there was no error */
    if (status == DMA_CHN_ERROR)
    {
        master->status = STATUS_ERROR;
    }
    else
    {
        master->status = STATUS_SUCCESS;
    }
    EIO_SPI_DRV_MasterStopTransfer(master);
    /* Call callback to announce the event to the user */
    if (master->callback != NULL)
    {
        master->callback(master, EIO_SPI_EVENT_END_TRANSFER, master->callbackParam);
    }
}

/*!
 * @brief configures the EIO module as SPI slave
 *
 * @param[in] slave: Pointer to the eio_SPI slave driver context structure
 * @param[in] userConfigPtr: Pointer to the eio_SPI user configuration structure
 * @return none
 */
static void EIO_SPI_DRV_SlaveConfigure(const eio_spi_slave_state_t *slave,
                                       const eio_spi_slave_user_config_t *userConfigPtr)
{
    EIO_Type *baseAddr;
    eio_pin_polarity_t clockPolarity;        /* Polarity of clock signal */
    eio_timer_polarity_t txShifterPolarity;  /* Polarity of MISO shifter */
    eio_timer_polarity_t rxShifterPolarity;  /* Polarity of MOSI shifter */
    eio_shifter_start_t startBit;
    eio_timer_start_t timerStartBit;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[slave->eioCommon.instance];
    resourceIndex = slave->eioCommon.resourceIndex;

    if (userConfigPtr->clockPolarity == 0U)
    {
        /* CPOL = 0 */
        clockPolarity = EIO_PIN_POLARITY_HIGH;
    }
    else
    {
        /* CPOL = 1 */
        clockPolarity = EIO_PIN_POLARITY_LOW;
    }

    if (userConfigPtr->clockPhase == 0U)
    {
        /* CPHA = 0 */
        txShifterPolarity = EIO_TIMER_POLARITY_NEGEDGE;
        rxShifterPolarity = EIO_TIMER_POLARITY_POSEDGE;
        startBit = EIO_SHIFTER_START_BIT_DISABLED;
        timerStartBit = EIO_TIMER_START_BIT_DISABLED;
    }
    else
    {
        /* CPHA = 1 */
        txShifterPolarity = EIO_TIMER_POLARITY_POSEDGE;
        rxShifterPolarity = EIO_TIMER_POLARITY_NEGEDGE;
        startBit = EIO_SHIFTER_START_BIT_DISABLED_SH;
        timerStartBit = EIO_TIMER_START_BIT_ENABLED;
    }

    /* Configure Slave Tx shifter (MISO) */
    EIO_SetShifterControl(baseAddr,
                          SPI_TX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->misoPin,
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_OUTPUT,
                          SPI_SCK_TIMER(resourceIndex),
                          txShifterPolarity);
    EIO_SetShifterConfig(baseAddr,
                         SPI_TX_SHIFTER(resourceIndex),
                         startBit,
                         EIO_SHIFTER_STOP_BIT_DISABLED,
                         EIO_SHIFTER_SOURCE_PIN);
    /* Configure Slave Rx shifter (MOSI) */
    EIO_SetShifterControl(baseAddr,
                          SPI_RX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->mosiPin,
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_DISABLED,
                          SPI_SCK_TIMER(resourceIndex),
                          rxShifterPolarity);
    EIO_SetShifterConfig(baseAddr,
                         SPI_RX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_DISABLED,
                         EIO_SHIFTER_STOP_BIT_DISABLED,
                         EIO_SHIFTER_SOURCE_PIN);

    /* Configure sck timer */
    EIO_SetTimerCompare(baseAddr, SPI_SCK_TIMER(resourceIndex), 0xFFFFU);

    EIO_SetTimerConfig(baseAddr,
                       SPI_SCK_TIMER(resourceIndex),
                       timerStartBit,
                       EIO_TIMER_STOP_BIT_DISABLED,
                       EIO_TIMER_ENABLE_TRG_POSEDGE,        /* enable on SS pin rising edge */
                       EIO_TIMER_DISABLE_TRG,               /* disable on SS pin falling edge */
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_PIN_SHIFT_PIN,   /* decrement on input pin - SCK */
                       EIO_TIMER_INITOUT_ZERO);
    EIO_SetTimerControl(baseAddr,
                        SPI_SCK_TIMER(resourceIndex),
                        (uint8_t)(userConfigPtr->ssPin << 1U), /* trigger on SS pin edge */
                        EIO_TRIGGER_POLARITY_LOW,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        userConfigPtr->sckPin,                 /* use SCK pin as input pin */
                        clockPolarity,
                        EIO_PIN_CONFIG_DISABLED,
                        EIO_TIMER_MODE_DISABLED);
}

/*!
 * @brief reads data received by the module
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static void EIO_SPI_DRV_ReadData(eio_spi_master_state_t *master)
{
    const EIO_Type *baseAddr;
    uint32_t data;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Read data from shifter buffer */
    if (master->bitOrder == EIO_SPI_TRANSFER_LSB_FIRST)
    {
        /* For data size < 4 bytes our data is in the upper part of the buffer and must be shifted */
        data = EIO_ReadShifterBuffer(baseAddr, SPI_RX_SHIFTER(resourceIndex), EIO_SHIFTER_RW_MODE_NORMAL);
        data >>= (32U - (8U * (uint32_t)(master->transferSize)));
    }
    else
    {
        data = EIO_ReadShifterBuffer(baseAddr, SPI_RX_SHIFTER(resourceIndex), EIO_SHIFTER_RW_MODE_BIT_SWAP);
    }

    if ((master->rxRemainingBytes > 0U) && (master->rxData != NULL))
    {
        switch (master->transferSize)
        {
        case EIO_SPI_TRANSFER_1BYTE:
            *(uint8_t *)master->rxData = (uint8_t)data;
            break;

        case EIO_SPI_TRANSFER_2BYTE:
            *(uint16_t *)master->rxData = (uint16_t)data;
            break;

        case EIO_SPI_TRANSFER_4BYTE:
            *(uint32_t *)master->rxData = (uint32_t)data;
            break;

        default:
            /* Not possible */
            break;
        }
        /* Update rx buffer pointer and remaining bytes count */
        master->rxData = &master->rxData[(master->transferSize)];
        master->rxRemainingBytes -= (uint32_t)(master->transferSize);
    }
    else
    {
        /* No data to receive, just ignore the read data */
    }
}

/*!
 * @brief writes data to be transmitted by the module
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static void EIO_SPI_DRV_WriteData(eio_spi_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint32_t data = EIO_SPI_DUMMYDATA;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    if (master->txRemainingBytes == 0U)
    {
        /* Done transmitting */
        return;
    }

    if ((master->txRemainingBytes > 0U) && (master->txData != NULL))
    {
        /* Read data from user buffer */
        switch (master->transferSize)
        {
        case EIO_SPI_TRANSFER_1BYTE:
            data = (uint32_t)(*(const uint8_t *)master->txData);
            break;

        case EIO_SPI_TRANSFER_2BYTE:
            data = (uint32_t)(*(const uint16_t *)master->txData);
            break;

        case EIO_SPI_TRANSFER_4BYTE:
            data = (uint32_t)(*(const uint32_t *)master->txData);
            break;

        default:
            /* Not possible */
            break;
        }
        /* Update tx buffer pointer and remaining bytes count */
        master->txData = &master->txData[master->transferSize];
        master->txRemainingBytes -= (uint32_t)(master->transferSize);
        /* Write data to shifter buffer */
        if (master->bitOrder == EIO_SPI_TRANSFER_LSB_FIRST)
        {
            EIO_WriteShifterBuffer(baseAddr, SPI_TX_SHIFTER(resourceIndex), data, EIO_SHIFTER_RW_MODE_NORMAL);
        }
        else
        {
            /* Shift data before bit-swapping it to get the relevant bits in the lower part of the shifter */
            data <<= 32U - (8U * (uint32_t)(master->transferSize));
            EIO_WriteShifterBuffer(baseAddr, SPI_TX_SHIFTER(resourceIndex), data, EIO_SHIFTER_RW_MODE_BIT_SWAP);
        }
    }
    else
    {
        /* Nothing to send, write dummy data in buffer */
        EIO_WriteShifterBuffer(baseAddr, SPI_TX_SHIFTER(resourceIndex), EIO_SPI_DUMMYDATA, EIO_SHIFTER_RW_MODE_NORMAL);
    }
}

/*!
 * @brief Check status of SPI master transfer. This function can be
 *        called either in an interrupt routine or directly in polling
 *        mode to advance the SPI transfer.
 *
 * @param[in] stateStruct: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static void EIO_SPI_DRV_MasterCheckStatus(void *stateStruct)
{
    eio_spi_master_state_t *master;
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(stateStruct != NULL);

    master = (eio_spi_master_state_t *)stateStruct;
    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check for errors */
    if (EIO_GetShifterErrorStatus(baseAddr, SPI_TX_SHIFTER(resourceIndex)))
    {
        master->status = STATUS_SPI_TX_UNDERRUN;
        /* Force the transfer to stop */
        EIO_SPI_DRV_MasterStopTransfer(master);
        /* Call callback to announce the event to the user */
        if (master->callback != NULL)
        {
            master->callback(master, EIO_SPI_EVENT_END_TRANSFER, master->callbackParam);
        }
        return;

    }
    if (EIO_GetShifterErrorStatus(baseAddr, SPI_RX_SHIFTER(resourceIndex)))
    {
        master->status = STATUS_SPI_RX_OVERRUN;
        /* Force the transfer to stop */
        EIO_SPI_DRV_MasterStopTransfer(master);
        /* Call callback to announce the event to the user */
        if (master->callback != NULL)
        {
            master->callback(master, EIO_SPI_EVENT_END_TRANSFER, master->callbackParam);
        }
        return;

    }
    /* Check if data was received */
    if (EIO_GetShifterStatus(baseAddr, SPI_RX_SHIFTER(resourceIndex)))
    {
        EIO_SPI_DRV_ReadData(master);
    }
    /* Check if transmitter needs more data */
    if (EIO_GetShifterStatus(baseAddr, SPI_TX_SHIFTER(resourceIndex)))
    {
        EIO_SPI_DRV_WriteData(master);
        if (master->txRemainingBytes == 0U)
        {
            /* No more data to transmit, disable tx interrupts */
            EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << SPI_TX_SHIFTER(resourceIndex)), false);
            EIO_SetShifterErrorInterrupt(baseAddr, (uint8_t)(1U << SPI_TX_SHIFTER(resourceIndex)), false);
        }
    }
    /* Check there is any data left */
    if ((master->txRemainingBytes == 0U) && (master->rxRemainingBytes == 0U))
    {
        /* Record success if there was no error */
        if (master->status == STATUS_BUSY)
        {
            master->status = STATUS_SUCCESS;
        }
        /* End transfer */
        EIO_SPI_DRV_MasterStopTransfer(master);
        /* Call callback to announce the event to the user */
        if (master->callback != NULL)
        {
            master->callback(master, EIO_SPI_EVENT_END_TRANSFER, master->callbackParam);
        }
    }
}

/*!
 * @brief Computes the address of the register used for DMA tx transfer
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static uint32_t EIO_SPI_DRV_MasterComputeTxRegAddr(const eio_spi_master_state_t *master)
{
    uint32_t addr;
    const EIO_Type *baseAddr;
    uint8_t shifter;

    baseAddr = g_eioBase[master->eioCommon.instance];
    shifter = SPI_TX_SHIFTER(master->eioCommon.resourceIndex);
    if (master->bitOrder == EIO_SPI_TRANSFER_LSB_FIRST)
    {
        addr = (uint32_t)(&(baseAddr->SHIFTBUF[shifter]));
    }
    else
    {
        addr = (uint32_t)(&(baseAddr->SHIFTBUFBIS[shifter])) + (sizeof(uint32_t) - (uint32_t)master->transferSize);
    }
    return addr;
}

/*!
 * @brief Computes the address of the register used for DMA rx transfer
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static uint32_t EIO_SPI_DRV_MasterComputeRxRegAddr(const eio_spi_master_state_t *master)
{
    uint32_t addr;
    const EIO_Type *baseAddr;
    uint8_t shifter;

    baseAddr = g_eioBase[master->eioCommon.instance];
    shifter = SPI_RX_SHIFTER(master->eioCommon.resourceIndex);
    if (master->bitOrder == EIO_SPI_TRANSFER_LSB_FIRST)
    {
        addr = (uint32_t)(&(baseAddr->SHIFTBUF[shifter])) + (sizeof(uint32_t) - (uint32_t)master->transferSize);
    }
    else
    {
        addr = (uint32_t)(&(baseAddr->SHIFTBUFBIS[shifter]));
    }
    return addr;
}

/*!
 * @brief Starts a DMA transfer
 *
 * @param[in] master: Pointer to the eio_SPI master driver context structure
 * @return none
 */
static void EIO_SPI_DRV_MasterStartDmaTransfer(eio_spi_master_state_t *master)
{
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    EIO_Type *baseAddr;
    uint32_t addr;
    /* Table to map flexio_spi transfer sizes to EDMA transfer sizes */
    static const dma_transfer_size_t dmaTransferSize[EIO_SPI_TRANSFER_4BYTE] =
    {DMA_TRANSFER_SIZE_1B, DMA_TRANSFER_SIZE_2B, DMA_TRANSFER_SIZE_4B, DMA_TRANSFER_SIZE_4B};


    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    if (master->txData != NULL)
    {
        addr = (uint32_t)(master->txData);
    }
    else
    {
        /* if there is no data to transmit, use dummy data as source for DMA transfer */
        master->dummyDmaData = EIO_SPI_DUMMYDATA;
        addr = (uint32_t)(&(master->dummyDmaData));
    }

    /* Configure the transfer control descriptor for the Tx channel */
    (void)DMA_DRV_ConfigTransfer(master->txDMAChannel,
                           DMA_TRANSFER_MEM2PERIPH,
                           addr,
                           EIO_SPI_DRV_MasterComputeTxRegAddr(master),
                           dmaTransferSize[(uint32_t)master->transferSize - 1U],
                           master->txRemainingBytes);

    if (master->txData == NULL)
    {
        /* if there is no data to transmit, don't increment source offset */
        DMA_DRV_SetSrcOffset(master->txDMAChannel, 0U);
    }

    if (master->rxData != NULL)
    {
        addr = (uint32_t)(master->rxData);
    }
    else
    {
        /* if there is no data to receive, use dummy data as destination for DMA transfer */
        addr = (uint32_t)(&(master->dummyDmaData));
    }

    /* Configure the transfer control descriptor for the Rx channel */
    (void)DMA_DRV_ConfigTransfer(master->rxDMAChannel,
                           DMA_TRANSFER_PERIPH2MEM,
                           EIO_SPI_DRV_MasterComputeRxRegAddr(master),
                           addr,
                           dmaTransferSize[(uint32_t)master->transferSize - 1U],
                           master->rxRemainingBytes);

    if (master->rxData == NULL)
    {
        /* if there is no data to receive, don't increment destination offset */
        DMA_DRV_SetDestOffset(master->rxDMAChannel, 0U);
    }
    /* Setup callback for DMA transfer end */
    (void)DMA_DRV_InstallCallback(master->rxDMAChannel,
                                  (dma_callback_t)(EIO_SPI_DRV_MasterEndDmaTransfer),
                                  (void*)(master));
    /* Start both DMA channels */
    (void)DMA_DRV_StartChannel(master->txDMAChannel);
    (void)DMA_DRV_StartChannel(master->rxDMAChannel);
        /* Enable EIO DMA requests for both shifters */
    EIO_SetShifterDMARequest(baseAddr, (uint8_t)((1U << SPI_TX_SHIFTER(resourceIndex)) | (1U << SPI_RX_SHIFTER(resourceIndex))), true);
}

/*!
 * @brief Initialize the EIO_SPI master mode driver
 *        This function initializes the EIO_SPI driver in master mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_SPI master user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] master: Pointer to the EIO_SPI master driver context structure. The driver uses
 *                    this memory area for its internal logic. The application must make no
 *                    assumptions about the content of this structure, and must not free this
 *                    memory until the driver is de-initialized using EIO_SPI_DRV_MasterDeinit().
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterInit(uint32_t instance,
                                const eio_spi_master_user_config_t *userConfigPtr,
                                eio_spi_master_state_t *master)
{
    status_t retCode;
    uint32_t inputClock;
    status_t clkErr;
    status_t osifError = STATUS_SUCCESS;
    uint8_t dmaReqTx;
    uint8_t dmaReqRx;

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(instance < EIO_INSTANCE_COUNT);
    /* Check that device was initialized */
    DEVICE_ASSERT(g_eioDeviceStatePtr[instance] != NULL);
    DEVICE_ASSERT((userConfigPtr->transferSize == EIO_SPI_TRANSFER_1BYTE) ||
                  (userConfigPtr->transferSize == EIO_SPI_TRANSFER_2BYTE) ||
                  (userConfigPtr->transferSize == EIO_SPI_TRANSFER_4BYTE));

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
    master->bitOrder = userConfigPtr->bitOrder;
    master->transferSize = userConfigPtr->transferSize;
    master->callback = userConfigPtr->callback;
    master->callbackParam = userConfigPtr->callbackParam;
    master->blocking = false;
    master->driverIdle = true;
    master->master = true;
    master->status = STATUS_SUCCESS;

    /* Set up transfer engine */
    switch (userConfigPtr->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        master->eioCommon.isr = EIO_SPI_DRV_MasterCheckStatus;
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Nothing to do here, FLEXIO_SPI_DRV_MasterGetStatus() will handle the transfer */
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Store DMA channel numbers */
        master->rxDMAChannel = userConfigPtr->rxDMAChannel;
        master->txDMAChannel = userConfigPtr->txDMAChannel;
        /* Configure DMA request sources */
        dmaReqTx = g_eioDMASrc[instance][SPI_TX_SHIFTER(master->eioCommon.resourceIndex)];
        dmaReqRx = g_eioDMASrc[instance][SPI_RX_SHIFTER(master->eioCommon.resourceIndex)];
        (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->txDMAChannel, dmaReqTx, false);
        (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->rxDMAChannel, dmaReqRx, false);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }
    /* Configure device for SPI mode */
    EIO_SPI_DRV_MasterConfigure(master, userConfigPtr, inputClock);
    (void)clkErr;
    (void)osifError;
    return STATUS_SUCCESS;
}


/*!
 * @brief De-initialize the EIO_SPI master mode driver
 *        This function de-initializes the EIO_SPI driver in master mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterDeinit(eio_spi_master_state_t *master)
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
 * @brief Set the baud rate for any subsequent SPI communication
 *         This function sets the baud rate for the SPI master.
 *         Note that due to module limitation not any baud rate can be achieved. The driver
 *         will set a baud rate as close as possible to the requested baud rate, but there may
 *         still be substantial differences, for example if requesting a high baud rate while
 *         using a low-frequency EIO clock. The application should call
 *         EIO_SPI_DRV_MasterGetBaudRate() after EIO_SPI_DRV_MasterSetBaudRate() to check
 *         what baud rate was actually set.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] baudRate: the desired baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterSetBaudRate(eio_spi_master_state_t *master, uint32_t baudRate)
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
    EIO_SPI_DRV_MasterComputeBaudRateDivider(baudRate, &divider, inputClock);

    /* Configure timer divider in the lower 8 bits of TIMCMP[CMP] */
    timerCmp = EIO_GetTimerCompare(baseAddr, SPI_SCK_TIMER(resourceIndex));
    timerCmp = (uint16_t)((timerCmp & 0xFF00U) | divider);
    EIO_SetTimerCompare(baseAddr, SPI_SCK_TIMER(resourceIndex), timerCmp);

    (void)clkErr;
    return STATUS_SUCCESS;
}

/*!
 * @brief Get the currently configured baud rate
 *        This function returns the currently configured SPI baud rate.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] baudRate: the current baud rate in hertz
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterGetBaudRate(eio_spi_master_state_t *master, uint32_t *baudRate)
{
    const EIO_Type *baseAddr;
    uint16_t divider;
    uint16_t timerCmp;
    uint32_t inputClock;
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
    timerCmp = EIO_GetTimerCompare(baseAddr, SPI_SCK_TIMER(resourceIndex));
    divider = (uint16_t)(timerCmp & 0x00FFU);

    /* Compute baud rate: input_clock / (2 * (divider + 1)). Round to nearest integer */
    *baudRate = (inputClock + divider + 1U) / (2U * ((uint32_t)divider + 1U));

    (void)clkErr;
    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a non-blocking SPI master transaction
 *        This function performs an SPI full-duplex transaction, transmit and receive in parallel.
 *        If only transmit or receive are required, it is possible to provide NULL pointers for
 *        txData or rxData. The transfer is non-blocking, the function only initiates the transfer
 *        and then returns, leaving the transfer to complete asynchronously).
 *        EIO_SPI_DRV_MasterGetStatus() can be called to check the status of the transfer.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] txData: pointer to the data to be transmitted
 * @param[in] rxData: pointer to the buffer where to store received data
 * @param[in] dataSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterTransfer(eio_spi_master_state_t *master,
                                    const uint8_t *txData,
                                    uint8_t *rxData,
                                    uint32_t dataSize)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT((dataSize % (uint32_t)(master->transferSize)) == 0U);
    DEVICE_ASSERT((txData != NULL) || (rxData != NULL));

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }
    /* Initialize transfer data */
    master->txData = txData;
    master->rxData = rxData;
    master->txRemainingBytes = dataSize;
    master->rxRemainingBytes = dataSize;
    master->driverIdle = false;
    master->status = STATUS_BUSY;
    /* disable and re-enable timers and shifters to reset them */
    EIO_SetShifterMode(baseAddr, SPI_TX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_DISABLED);
    EIO_SetShifterMode(baseAddr, SPI_RX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_DISABLED);
    /* Enable transfer engine */
    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Enable interrupts for Rx and Tx shifters */
        EIO_SetShifterInterrupt(baseAddr,
                                (uint8_t)((1U << SPI_TX_SHIFTER(resourceIndex)) | (1U << SPI_RX_SHIFTER(resourceIndex))),
                                true);
        EIO_SetShifterErrorInterrupt(baseAddr,
                                     (uint8_t)((1U << SPI_TX_SHIFTER(resourceIndex)) | (1U << SPI_RX_SHIFTER(resourceIndex))),
                                     true);
        /* Enable timers and shifters */
        EIO_SPI_DRV_MasterEnableTransfer(master);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Enable timers and shifters */
        EIO_SPI_DRV_MasterEnableTransfer(master);
        /* Call EIO_SPI_DRV_MasterCheckStatus once to send the first byte */
        EIO_SPI_DRV_MasterCheckStatus(master);
        break;

    case EIO_DRIVER_TYPE_DMA:
        EIO_SPI_DRV_MasterStartDmaTransfer(master);
        /* Enable timers and shifters */
        EIO_SPI_DRV_MasterEnableTransfer(master);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a blocking SPI master transaction
 *        This function performs an SPI full-duplex transaction, transmit and receive in parallel.
 *        If only transmit or receive are required, it is possible to provide NULL pointers for
 *        txData or rxData. The transfer is blocking, the function only returns when the transfer
 *        is complete.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] txData: pointer to the data to be transmitted
 * @param[in] rxData: pointer to the buffer where to store received data
 * @param[in] dataSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterTransferBlocking(eio_spi_master_state_t *master,
                                            const uint8_t *txData,
                                            uint8_t *rxData,
                                            uint32_t dataSize,
                                            uint32_t timeout)
{
    status_t status;

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    /* mark transfer as blocking */
    if (master->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        master->blocking = true;
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemaWait(&(master->idleSemaphore), 0U);
    }
    status = EIO_SPI_DRV_MasterTransfer(master, txData, rxData, dataSize);
    if (status != STATUS_SUCCESS)
    {
        /* Transfer could not be started */
        master->blocking = false;
        return status;
    }

    /* Wait for transfer to end */
    return EIO_SPI_DRV_MasterWaitTransferEnd(master, timeout);
}

/*!
 * @brief Aborts a non-blocking SPI master transaction
 *       This function aborts a non-blocking SPI transfer.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterTransferAbort(eio_spi_master_state_t *master)
{
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (master->driverIdle)
    {
        return STATUS_SUCCESS;
    }

    master->status = STATUS_SPI_ABORTED;
    EIO_SPI_DRV_MasterStopTransfer(master);

    return STATUS_SUCCESS;
}

/*!
 * @brief Get the status of the current non-blocking SPI master transaction
 *        This function returns the current status of a non-blocking SPI master transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] master: Pointer to the EIO_SPI master driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_MasterGetStatus(eio_spi_master_state_t *master, uint32_t *bytesRemaining)
{
    uint32_t remainingBytes;

    DEVICE_ASSERT(master != NULL);

    /* Initialize with the actual remaining bytes count */
    remainingBytes = master->rxRemainingBytes;

    if (!master->driverIdle)
    {
        switch (master->driverType)
        {
        case EIO_DRIVER_TYPE_POLLING:
            /* In polling mode advance the SPI transfer here */
            EIO_SPI_DRV_MasterCheckStatus(master);
            break;

        case EIO_DRIVER_TYPE_DMA:
            /* In DMA mode just update the remaining count.
             DO NOT write master->txRemainingBytes directly !!! */
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

    return master->status;
}

/*!
 * @brief Initialize the EIO_SPI slave mode driver
 *        This function initializes the EIO_SPI driver in slave mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_SPI slave user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure. The driver uses
 *                   this memory area for its internal logic. The application must make no
 *                   assumptions about the content of this structure, and must not free this
 *                   memory until the driver is de-initialized using EIO_SPI_DRV_SlaveDeinit().
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveInit(uint32_t instance,
                               const eio_spi_slave_user_config_t *userConfigPtr,
                               eio_spi_slave_state_t *slave)
{
    status_t retCode;
    status_t osifError = STATUS_SUCCESS;
    uint8_t dmaReqTx;
    uint8_t dmaReqRx;

    DEVICE_ASSERT(slave != NULL);
    DEVICE_ASSERT(instance < EIO_INSTANCE_COUNT);
    /* Check that device was initialized */
    DEVICE_ASSERT(g_eioDeviceStatePtr[instance] != NULL);

    /* Instruct the resource allocator that we need two shifters/timers */
    slave->eioCommon.resourceCount = 2U;
    /* Common EIO driver initialization */
    retCode = EIO_DRV_InitDriver(instance, &(slave->eioCommon));
    if (retCode != STATUS_SUCCESS)
    {   /* Initialization failed, not enough resources */
        return retCode;
    }
    /* Initialize the semaphore */
    if (userConfigPtr->driverType != EIO_DRIVER_TYPE_POLLING)
    {
        osifError = OSIF_SemaCreate(&(slave->idleSemaphore), 0U);
        DEVICE_ASSERT(osifError == STATUS_SUCCESS);
    }

    /* Initialize driver context structure */
    slave->driverType = userConfigPtr->driverType;
    slave->bitOrder = userConfigPtr->bitOrder;
    slave->transferSize = userConfigPtr->transferSize;
    slave->callback = userConfigPtr->callback;
    slave->callbackParam = userConfigPtr->callbackParam;
    slave->blocking = false;
    slave->driverIdle = true;
    slave->master = false;
    slave->status = STATUS_SUCCESS;

    /* Set up transfer engine */
    switch (userConfigPtr->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        slave->eioCommon.isr = EIO_SPI_DRV_MasterCheckStatus;
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Nothing to do here, FLEXIO_SPI_DRV_MasterGetStatus() will handle the transfer */
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Store DMA channel numbers */
        slave->rxDMAChannel = userConfigPtr->rxDMAChannel;
        slave->txDMAChannel = userConfigPtr->txDMAChannel;
        /* Configure DMA request sources */
        dmaReqTx = g_eioDMASrc[instance][SPI_TX_SHIFTER(slave->eioCommon.resourceIndex)];
        dmaReqRx = g_eioDMASrc[instance][SPI_RX_SHIFTER(slave->eioCommon.resourceIndex)];
        (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->txDMAChannel, dmaReqTx, false);
        (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->rxDMAChannel, dmaReqRx, false);
        break;
    default:
        /* Impossible type - do nothing */
        break;
    }
    /* Configure device for SPI mode */
    EIO_SPI_DRV_SlaveConfigure(slave, userConfigPtr);

    (void)osifError;
    return STATUS_SUCCESS;
}

/*!
 * @brief Returns default configuration structure for EIO_SPI master
 *
 * @param[in] userConfigPtr: Pointer to the EIO_SPI user configuration structure.
 * @return none
 */
void EIO_SPI_DRV_MasterGetDefaultConfig(eio_spi_master_user_config_t *userConfigPtr)
{
    DEVICE_ASSERT(userConfigPtr != NULL);

    userConfigPtr->baudRate = 100000U;
    userConfigPtr->driverType = EIO_DRIVER_TYPE_INTERRUPTS;
    userConfigPtr->bitOrder = EIO_SPI_TRANSFER_MSB_FIRST;
    userConfigPtr->transferSize = EIO_SPI_TRANSFER_1BYTE;
    userConfigPtr->clockPolarity = 0U;
    userConfigPtr->clockPhase = 0U;
    userConfigPtr->mosiPin = 0U;
    userConfigPtr->misoPin = 1U;
    userConfigPtr->sckPin = 2U;
    userConfigPtr->ssPin = 3U;
    userConfigPtr->callback = NULL;
    userConfigPtr->callbackParam = NULL;
    userConfigPtr->rxDMAChannel = 255U;
    userConfigPtr->txDMAChannel = 255U;
}

/*!
 * @brief Returns default configuration structure for EIO_SPI slave
 *
 * @param[in] userConfigPtr: Pointer to the EIO_SPI user configuration structure.
 * @return none
 */
void EIO_SPI_DRV_SlaveGetDefaultConfig(eio_spi_slave_user_config_t *userConfigPtr)
{
    DEVICE_ASSERT(userConfigPtr != NULL);

    userConfigPtr->driverType = EIO_DRIVER_TYPE_INTERRUPTS;
    userConfigPtr->bitOrder = EIO_SPI_TRANSFER_MSB_FIRST;
    userConfigPtr->transferSize = EIO_SPI_TRANSFER_1BYTE;
    userConfigPtr->clockPolarity = 0U;
    userConfigPtr->clockPhase = 0U;
    userConfigPtr->mosiPin = 0U;
    userConfigPtr->misoPin = 1U;
    userConfigPtr->sckPin = 2U;
    userConfigPtr->ssPin = 3U;
    userConfigPtr->callback = NULL;
    userConfigPtr->callbackParam = NULL;
    userConfigPtr->rxDMAChannel = 255U;
    userConfigPtr->txDMAChannel = 255U;
}

/*!
 * @brief De-initialize the EIO_SPI slave mode driver
 *        This function de-initializes the EIO_SPI driver in slave mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveDeinit(eio_spi_slave_state_t *slave)
{
    return EIO_SPI_DRV_MasterDeinit(slave);
}

/*!
 * @brief Perform a non-blocking SPI slave transaction
 *        This function performs an SPI full-duplex transaction, transmit and receive in parallel.
 *        If only transmit or receive are required, it is possible to provide NULL pointers for
 *        txData or rxData. The transfer is non-blocking, the function only initiates the transfer
 *        and then returns, leaving the transfer to complete asynchronously).
 *        EIO_SPI_DRV_SlaveGetStatus() can be called to check the status of the transfer.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @param[in] txData: pointer to the data to be transmitted
 * @param[in] rxData: pointer to the buffer where to store received data
 * @param[in] dataSize: length in bytes of the data to be transferred
 * @return  Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveTransfer(eio_spi_slave_state_t *slave,
                                   const uint8_t *txData,
                                   uint8_t *rxData,
                                   uint32_t dataSize)
{
    return EIO_SPI_DRV_MasterTransfer(slave, txData, rxData, dataSize);
}

/*!
 * @brief Perform a blocking SPI slave transaction
 *        This function performs an SPI full-duplex transaction, transmit and receive in parallel.
 *        If only transmit or receive are required, it is possible to provide NULL pointers for
 *        txData or rxData. The transfer is blocking, the function only returns when the transfer
 *        is complete.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @param[in] txData: pointer to the data to be transmitted
 * @param[in] rxData: pointer to the buffer where to store received data
 * @param[in] dataSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveTransferBlocking(eio_spi_slave_state_t *slave,
                                           const uint8_t *txData,
                                           uint8_t *rxData,
                                           uint32_t dataSize,
                                           uint32_t timeout)
{
    return EIO_SPI_DRV_MasterTransferBlocking(slave, txData, rxData, dataSize, timeout);
}

/*!
 * @brief Aborts a non-blocking SPI slave transaction
 *        This function aborts a non-blocking SPI transfer.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveTransferAbort(eio_spi_slave_state_t *slave)
{
    return EIO_SPI_DRV_MasterTransferAbort(slave);
}

/*!
 * @brief Get the status of the current non-blocking SPI slave transaction
 *        This function returns the current status of a non-blocking SPI slave transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] slave: Pointer to the EIO_SPI slave driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_SPI_DRV_SlaveGetStatus(eio_spi_slave_state_t *slave, uint32_t *bytesRemaining)
{
    return EIO_SPI_DRV_MasterGetStatus(slave, bytesRemaining);
}

/* =============================================  EOF  ============================================== */
