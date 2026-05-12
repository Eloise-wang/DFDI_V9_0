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
 * @file eio_i2s_drv.c
 *
 * @brief This file provides eio i2s module integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "eio_hw.h"
#include "eio_comm.h"
#include "eio_i2s_drv.h"
#include "ckgen_drv.h"
#include "dma_drv.h"

/* ============================================  Define  ============================================ */
/* Constraints used for baud rate computation */
#define DIVIDER_MIN_VALUE (1)
#define DIVIDER_MAX_VALUE (0xFF)

/*!
 * @brief Shifters/Timers used for I2S simulation The param[in]eter x represents the
 * resourceIndex value for the current driver instance
 */
#define I2S_TX_SHIFTER(x)           (x)
#define I2S_RX_SHIFTER(x)           (uint8_t)((x) + 1UL)
#define I2S_SCK_TIMER(x)            (x)
#define I2S_WS_TIMER(x)             (uint8_t)((x) + 1UL)
#define I2S_WS_TIMER_RISING_BLK(x)  (uint8_t)((x) + 2UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* First init about i2s in clockContinue mode*/
static bool s_i2sInit = false;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Computes the baud rate divider for a target baud rate
 *
 * @param[in] bitsWidth: bits width
 * @return  the baud rate divider for a target baud rate
 */
static uint8_t EIO_I2S_DRV_ComputeByteWidth(uint8_t bitsWidth)
{
    uint8_t byteWidth;

    if (bitsWidth <= 8U)
    {
        byteWidth = 1U;
    }
    else if (bitsWidth <= 16U)
    {
        byteWidth = 2U;
    }
    else
    {
        byteWidth = 4U;
    }

    return byteWidth;
}

/*!
 * @brief Computes the baud rate divider for a target baud rate
 *
 * @param[in] baudRate: The desired baud rate in hertz
 * @param[out] divider: Point to baud rate divider
 * @param[in] inputClock: The frequency of the input clock
 * @return none
 */
static void EIO_I2S_DRV_MasterComputeBaudRateDivider(uint32_t baudRate,
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
 * @brief configures the EIO module as I2S master
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @param[in] userConfigPtr: Pointer to the eio_I2S user configuration structure
 * @param[in] inputClock: The frequency of the input clock
 * @return none
 */
static void EIO_I2S_DRV_MasterConfigure(eio_i2s_master_state_t *master,
                                        const eio_i2s_master_user_config_t *userConfigPtr,
                                        uint32_t inputClock)
{
    EIO_Type *baseAddr;
    uint16_t divider;
    uint16_t bits;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Compute divider. */
    EIO_I2S_DRV_MasterComputeBaudRateDivider(userConfigPtr->baudRate, &divider, inputClock);
    bits = userConfigPtr->bitsWidth;

    /* Configure tx shifter */
    EIO_SetShifterConfig(baseAddr,
                         I2S_TX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_DISABLED_SH,
                         EIO_SHIFTER_STOP_BIT_DISABLED,
                         EIO_SHIFTER_SOURCE_PIN);
    EIO_SetShifterControl(baseAddr,
                          I2S_TX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->txPin,             /* output on tx pin */
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_DISABLED,       /* disable for now, will enable on transmit */
                          I2S_SCK_TIMER(resourceIndex),         /* use clock timer to drive the shifter */
                          EIO_TIMER_POLARITY_POSEDGE);

    /* Configure rx shifter */
    EIO_SetShifterConfig(baseAddr,
                         I2S_RX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_DISABLED,
                         EIO_SHIFTER_STOP_BIT_DISABLED,
                         EIO_SHIFTER_SOURCE_PIN);
    EIO_SetShifterControl(baseAddr,
                          I2S_RX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->rxPin,                    /* output to rx pin */
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_DISABLED,
                          I2S_SCK_TIMER(resourceIndex),     /* use control timer to drive the shifter */
                          EIO_TIMER_POLARITY_NEGEDGE);

    /* Configure SCK timer */
    EIO_SetTimerCompare(baseAddr, I2S_SCK_TIMER(resourceIndex), (uint16_t)((uint16_t)(((uint16_t)(bits << 1U) - 1U) << 8U) + divider));
    EIO_SetTimerConfig(baseAddr,
                       I2S_SCK_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_ENABLED,
                       EIO_TIMER_STOP_BIT_DISABLED,
                       EIO_TIMER_ENABLE_TRG_HIGH,         /* enable when Tx data is available */
                       EIO_TIMER_DISABLE_NEVER,
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_CLK_SHIFT_TMR, /* decrement on EIO clock */
                       EIO_TIMER_INITOUT_ONE);
    EIO_SetTimerControl(baseAddr,
                        I2S_SCK_TIMER(resourceIndex),
                        (uint8_t)((uint8_t)(I2S_TX_SHIFTER(resourceIndex) << 2U) + 1U),  /* trigger on tx shifter status flag */
                        EIO_TRIGGER_POLARITY_LOW,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        userConfigPtr->sckPin,                 /* output on SCK pin */
                        EIO_PIN_POLARITY_LOW,
                        EIO_PIN_CONFIG_OUTPUT,              /* enable output */
                        EIO_TIMER_MODE_DISABLED);

    /* Configure WS timer */
    EIO_SetTimerCompare(baseAddr, I2S_WS_TIMER(resourceIndex), (uint16_t)((bits * ((divider + 1U) * 2U)) - 1U));
    EIO_SetTimerConfig(baseAddr,
                       I2S_WS_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_DISABLED,
                       EIO_TIMER_STOP_BIT_DISABLED,
                       EIO_TIMER_ENABLE_TIM_ENABLE,       /* enable when SCK timer is enabled */
                       EIO_TIMER_DISABLE_NEVER,
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_CLK_SHIFT_TMR, /* decrement on EIO clock */
                       EIO_TIMER_INITOUT_ONE);
    EIO_SetTimerControl(baseAddr,
                        I2S_WS_TIMER(resourceIndex),
                        0U,                                  /* trigger not used */
                        EIO_TRIGGER_POLARITY_HIGH,
                        EIO_TRIGGER_SOURCE_EXTERNAL,
                        userConfigPtr->wsPin,                /* output on WS pin */
                        EIO_PIN_POLARITY_LOW,
                        EIO_PIN_CONFIG_OUTPUT,            /* enable output */
                        EIO_TIMER_MODE_DISABLED);
}

/*!
 * @brief configures the EIO module as I2S slave
 *
 * @param[in] slave: Pointer to the eio_I2S slave driver context structure.
 * @param[in] userConfigPtr: Pointer to the eio_I2S user configuration structure
 * @return none
 */
static void EIO_I2S_DRV_SlaveConfigure(const eio_i2s_slave_state_t *slave,
                                       const eio_i2s_slave_user_config_t *userConfigPtr)
{
    EIO_Type *baseAddr;
    uint16_t bits;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[slave->eioCommon.instance];
    resourceIndex = slave->eioCommon.resourceIndex;
    bits = userConfigPtr->bitsWidth;


    /* Configure tx shifter */
    EIO_SetShifterConfig(baseAddr,
                         I2S_TX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_DISABLED,
                         EIO_SHIFTER_STOP_BIT_DISABLED,
                         EIO_SHIFTER_SOURCE_PIN);
    EIO_SetShifterControl(baseAddr,
                          I2S_TX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->txPin,             /* output on tx pin */
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_DISABLED,           /* disable for now, will enable on transmit *///EIO_PIN_CONFIG_DISABLED
                          I2S_WS_TIMER(resourceIndex),     /* use clock timer to drive the shifter */
                          EIO_TIMER_POLARITY_POSEDGE);

    /* Configure rx shifter */
    EIO_SetShifterConfig(baseAddr,
                         I2S_RX_SHIFTER(resourceIndex),
                         EIO_SHIFTER_START_BIT_DISABLED,
                         EIO_SHIFTER_STOP_BIT_DISABLED,
                         EIO_SHIFTER_SOURCE_PIN);
    EIO_SetShifterControl(baseAddr,
                          I2S_RX_SHIFTER(resourceIndex),
                          EIO_SHIFTER_MODE_DISABLED,
                          userConfigPtr->rxPin,                    /* output to rx pin */
                          EIO_PIN_POLARITY_HIGH,
                          EIO_PIN_CONFIG_DISABLED,
                          I2S_WS_TIMER(resourceIndex),     /* use control timer to drive the shifter */
                          EIO_TIMER_POLARITY_NEGEDGE);

    /* Configure SCK timer */
    EIO_SetTimerCompare(baseAddr, I2S_SCK_TIMER(resourceIndex), (uint16_t)((uint16_t)(bits << 2U) - 3U));
    EIO_SetTimerConfig(baseAddr,
                       I2S_SCK_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_DISABLED,
                       EIO_TIMER_STOP_BIT_DISABLED,
                       EIO_TIMER_ENABLE_PIN_POSEDGE,      /* enable on WS positive edge */
                       EIO_TIMER_DISABLE_TIM_CMP,
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_TRIG_SHIFT_TRIG, /* decrement on SCK input  */
                       EIO_TIMER_INITOUT_ONE);
    EIO_SetTimerControl(baseAddr,
                        I2S_SCK_TIMER(resourceIndex),
                        (uint8_t)(userConfigPtr->sckPin << 1U), /* trigger on SCK pin */
                        EIO_TRIGGER_POLARITY_LOW,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        userConfigPtr->wsPin,                 /* use WS input pin */
                        EIO_PIN_POLARITY_LOW,
                        EIO_PIN_CONFIG_DISABLED,
                        EIO_TIMER_MODE_DISABLED);
    /* configure WS_TIMER_RISING_BLK timer */
    EIO_SetTimerCompare(baseAddr, I2S_WS_TIMER_RISING_BLK(resourceIndex), (uint16_t)((uint16_t)(bits << 2U) - 4U));
    EIO_SetTimerConfig(baseAddr,
                       I2S_WS_TIMER_RISING_BLK(resourceIndex),
                       EIO_TIMER_START_BIT_DISABLED,
                       EIO_TIMER_STOP_BIT_DISABLED,
                       EIO_TIMER_ENABLE_PIN_POSEDGE_TRG_HIGH,
                       EIO_TIMER_DISABLE_TRG,
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_PIN_SHIFT_PIN,
                       EIO_TIMER_INITOUT_ONE);
    EIO_SetTimerControl(baseAddr,
                        I2S_WS_TIMER_RISING_BLK(resourceIndex),
                        (uint8_t)((uint8_t)(I2S_SCK_TIMER(resourceIndex) << 2U) + 3U), /* SCK timer trigger output */
                        EIO_TRIGGER_POLARITY_HIGH,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        userConfigPtr->sckPin,                 /* use sck input pin */
                        EIO_PIN_POLARITY_HIGH,
                        EIO_PIN_CONFIG_DISABLED,
                        EIO_TIMER_MODE_DISABLED);

    /* Configure WS timer */
    EIO_SetTimerCompare(baseAddr, I2S_WS_TIMER(resourceIndex), (uint16_t)((uint16_t)(bits << 1U) - 1U));
    EIO_SetTimerConfig(baseAddr,
                       I2S_WS_TIMER(resourceIndex),
                       EIO_TIMER_START_BIT_DISABLED,
                       EIO_TIMER_STOP_BIT_DISABLED,
                       EIO_TIMER_ENABLE_PIN_POSEDGE_TRG_HIGH, /* enable at first falling edge after first rising edge of bclk */
                       EIO_TIMER_DISABLE_TIM_CMP_TRG_LOW,
                       EIO_TIMER_RESET_NEVER,
                       EIO_TIMER_DECREMENT_PIN_SHIFT_PIN,
                       EIO_TIMER_INITOUT_ONE);
    EIO_SetTimerControl(baseAddr,
                        I2S_WS_TIMER(resourceIndex),
                        (uint8_t)((uint8_t)(I2S_WS_TIMER_RISING_BLK(resourceIndex) << 2U) + 3U), /* I2S_WS_TIMER_RISING_BLK timer trigger output */
                        EIO_TRIGGER_POLARITY_HIGH,
                        EIO_TRIGGER_SOURCE_INTERNAL,
                        userConfigPtr->sckPin,               /* SCK input pin */
                        EIO_PIN_POLARITY_LOW,
                        EIO_PIN_CONFIG_DISABLED,          /* enable output */
                        EIO_TIMER_MODE_DISABLED);
}

/*!
 * @brief End the current transfer
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @return none
 */
static void EIO_I2S_DRV_MasterEndTransfer(eio_i2s_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Disable transfer engine */
    switch (master->driverType)
    {
        case EIO_DRIVER_TYPE_INTERRUPTS:
            /* Disable interrupts for Rx and Tx shifters */
            EIO_SetShifterInterrupt(baseAddr,
                                 (uint8_t)((1U << I2S_TX_SHIFTER(resourceIndex)) | (1U << I2S_RX_SHIFTER(resourceIndex))),
                                 false);
            EIO_SetShifterErrorInterrupt(baseAddr,
                                 (uint8_t)((1U << I2S_TX_SHIFTER(resourceIndex)) | (1U << I2S_RX_SHIFTER(resourceIndex))),
                                 false);
            break;

        case EIO_DRIVER_TYPE_POLLING:
            /* Nothing to do here */
            break;

        case EIO_DRIVER_TYPE_DMA:
            /* Stop DMA channels */
            (void)DMA_DRV_StopChannel(master->txDMAChannel);
            (void)DMA_DRV_StopChannel(master->rxDMAChannel);
            /* Disable EIO Tx and Rx DMA requests */
            EIO_SetShifterDMARequest(baseAddr, (uint8_t)((1U << I2S_TX_SHIFTER(resourceIndex)) | (1U << I2S_RX_SHIFTER(resourceIndex))), false);
            break;

        default:
            /* Impossible type - do nothing */
            break;
    }

    master->driverIdle = true;
    master->txData = NULL;
    master->rxData = NULL;
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
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @return none
 */
static void EIO_I2S_DRV_MasterEnableTransfer(eio_i2s_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = master->eioCommon.resourceIndex;
    baseAddr = g_eioBase[master->eioCommon.instance];
    /* enable timers and shifters */
    EIO_SetShifterMode(baseAddr, I2S_TX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_TRANSMIT);
    EIO_SetShifterMode(baseAddr, I2S_RX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_RECEIVE);
    EIO_SetTimerMode(baseAddr, I2S_WS_TIMER(resourceIndex), EIO_TIMER_MODE_16BIT);
    if (master->master)
    {
        EIO_SetTimerMode(baseAddr, I2S_SCK_TIMER(resourceIndex), EIO_TIMER_MODE_8BIT_BAUD);
    }
    else
    {
        EIO_SetTimerMode(baseAddr, I2S_SCK_TIMER(resourceIndex), EIO_TIMER_MODE_16BIT);
        EIO_SetTimerMode(baseAddr, I2S_WS_TIMER_RISING_BLK(resourceIndex), EIO_TIMER_MODE_16BIT);
    }
}

/*!
 * @brief Stops the current transfer.
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @return none
 */
static void EIO_I2S_DRV_MasterStopTransfer(eio_i2s_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    resourceIndex = master->eioCommon.resourceIndex;
    baseAddr = g_eioBase[master->eioCommon.instance];

    if (!master->clockContinueOutput)
    {
        /* disable timers and shifters */
        EIO_SetTimerMode(baseAddr, I2S_SCK_TIMER(resourceIndex), EIO_TIMER_MODE_DISABLED);
        EIO_SetTimerMode(baseAddr, I2S_WS_TIMER(resourceIndex), EIO_TIMER_MODE_DISABLED);
    }

    EIO_SetShifterMode(baseAddr, I2S_TX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_DISABLED);
    EIO_SetShifterMode(baseAddr, I2S_RX_SHIFTER(resourceIndex), EIO_SHIFTER_MODE_DISABLED);
    if (!master->master)
    {
        EIO_SetTimerMode(baseAddr, I2S_WS_TIMER_RISING_BLK(resourceIndex), EIO_TIMER_MODE_DISABLED);
    }

    if (!master->clockContinueOutput)
    {
        /* Disable pin output */
        EIO_SetShifterPinConfig(baseAddr, I2S_TX_SHIFTER(resourceIndex), EIO_PIN_CONFIG_DISABLED);
    }

    /* clear any leftover error flags */
    EIO_ClearShifterErrorStatus(baseAddr, I2S_TX_SHIFTER(resourceIndex));
    EIO_ClearShifterErrorStatus(baseAddr, I2S_RX_SHIFTER(resourceIndex));
    /* discard any leftover rx. data */
    EIO_ClearShifterStatus(baseAddr, I2S_RX_SHIFTER(resourceIndex));

    /* end the transfer */
    EIO_I2S_DRV_MasterEndTransfer(master);
}

/*!
 * @brief waits for the end of a blocking transfer
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return none
 */
static status_t EIO_I2S_DRV_MasterWaitTransferEnd(eio_i2s_master_state_t *master, uint32_t timeout)
{
    status_t osifError = STATUS_SUCCESS;

    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Wait for transfer to be completed by the IRQ */
        osifError = OSIF_SemaWait(&(master->idleSemaphore), timeout);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Call EIO_I2S_DRV_MasterGetStatus() to do the transfer */
        while (EIO_I2S_DRV_MasterGetStatus(master, NULL) == STATUS_BUSY) {}
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
        EIO_I2S_DRV_MasterStopTransfer(master);
    }

    return master->status;
}

/*!
 * @brief reads data received by the module
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @return none
 */
static void EIO_I2S_DRV_ReadData(eio_i2s_master_state_t *master)
{
    const EIO_Type *baseAddr;
    uint32_t data;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Read data from shifter buffer */
    data = EIO_ReadShifterBuffer(baseAddr, I2S_RX_SHIFTER(resourceIndex), EIO_SHIFTER_RW_MODE_BIT_SWAP);

    if (master->rxRemainingBytes > 0U)
    {
        if (master->rxData != NULL)
        {
            switch (master->byteWidth)
            {
            case 1U:
                *(uint8_t *)master->rxData = (uint8_t)data;
                break;

            case 2U:
                *(uint16_t *)master->rxData = (uint16_t)data;
                break;

            default:
                *(uint32_t *)master->rxData = (uint32_t)data;
                break;
            }
            /* Update rx buffer pointer */
            master->rxData = &master->rxData[master->byteWidth];
        }
        /* Update remaining bytes count even if buffer is null */
        master->rxRemainingBytes -= (uint32_t)(master->byteWidth);
    }
    else
    {
        /* No data to receive, just ignore the read data */
    }
}

/*!
 * @brief writes data to be transmitted by the module
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @return none
 */
static void EIO_I2S_DRV_WriteData(eio_i2s_master_state_t *master)
{
    EIO_Type *baseAddr;
    uint32_t data;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    if (master->txRemainingBytes == 0U)
    {
        /* Done transmitting */
        return;
    }

    /* Read data from user buffer and update tx buffer pointer and remaining bytes count */
    switch (master->byteWidth)
    {
    case 1U:
        data = (uint32_t)(*(const uint8_t *)master->txData);
        break;

    case 2U:
        data = (uint32_t)(*(const uint16_t *)master->txData);
        break;

    default:
        data = (uint32_t)(*(const uint32_t *)master->txData);
        break;
    }
    master->txData = &master->txData[master->byteWidth];
    master->txRemainingBytes -= (uint32_t)(master->byteWidth);

    /* Write data to shifter buffer */
    /* Shift data before bit-swapping it to get the relevant bits in the lower part of the shifter */
    data <<= 32U - (uint32_t)(master->bitsWidth);
    EIO_WriteShifterBuffer(baseAddr, I2S_TX_SHIFTER(resourceIndex), data, EIO_SHIFTER_RW_MODE_BIT_SWAP);
}

/*!
 * @brief Check status of the I2S transfer. This function can be
 *        called either in an interrupt routine or directly in polling
 *        mode to advance the I2S transfer.
 *
 * @param[in] stateStruct: Pointer to the eio_I2S master driver context structure
 * @return none
 */
static void EIO_I2S_DRV_MasterCheckStatus(void *stateStruct)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    eio_i2s_master_state_t *master;

    DEVICE_ASSERT(stateStruct != NULL);

    master = (eio_i2s_master_state_t *)stateStruct;
    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check for errors */
    if (master->txData != NULL)
    {
        if (EIO_GetShifterErrorStatus(baseAddr, I2S_TX_SHIFTER(resourceIndex)))
        {
            master->status = STATUS_I2S_TX_UNDERRUN;
            EIO_ClearShifterErrorStatus(baseAddr, I2S_TX_SHIFTER(resourceIndex));
            /* Continue processing events */
        }
    }
    if (master->rxData != NULL)
    {
        if (EIO_GetShifterErrorStatus(baseAddr, I2S_RX_SHIFTER(resourceIndex)))
        {
            master->status = STATUS_I2S_RX_OVERRUN;
            EIO_ClearShifterErrorStatus(baseAddr, I2S_RX_SHIFTER(resourceIndex));
            /* Continue processing events */
        }
    }
    /* Check if data was received */
    if (EIO_GetShifterStatus(baseAddr, I2S_RX_SHIFTER(resourceIndex)))
    {
        EIO_I2S_DRV_ReadData(master);
        if ((master->rxData != NULL) && (master->rxRemainingBytes == 0U))
        {
            /* Out of rx space, call callback to allow user to provide a new buffer */
            if (master->callback != NULL)
            {
                master->callback(EIO_I2S_EVENT_RX_FULL, master->callbackParam);
            }
        }
    }
    /* Check if transmitter needs more data */
    if (master->txData != NULL)
    {
        if (EIO_GetShifterStatus(baseAddr, I2S_TX_SHIFTER(resourceIndex)))
        {
            EIO_I2S_DRV_WriteData(master);
            if (master->txRemainingBytes == 0U)
            {
                master->txData = NULL;
                /* Out of data, call callback to allow user to provide a new buffer */
                if (master->callback != NULL)
                {
                    master->callback(EIO_I2S_EVENT_TX_EMPTY, master->callbackParam);
                }
                if ((master->txRemainingBytes == 0U) && (master->driverType == EIO_DRIVER_TYPE_INTERRUPTS))
                {
                    /* Still no more data to transmit, transmission will stop */
                    /* disable tx interrupts */
                    EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << I2S_TX_SHIFTER(resourceIndex)), false);
                    EIO_SetShifterErrorInterrupt(baseAddr, (uint8_t)(1U << I2S_TX_SHIFTER(resourceIndex)), false);
                    /* Enable rx interrupt to signal end of transfer */
                    EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << I2S_RX_SHIFTER(resourceIndex)), true);
                }
            }
        }
    }

    /* Check if transfer is over */
    if (master->rxRemainingBytes == 0U)
    {
        /* Record success if there was no error */
        if (master->status == STATUS_BUSY)
        {
            master->status = STATUS_SUCCESS;
        }
        /* End transfer */
        EIO_I2S_DRV_MasterStopTransfer(master);
        /* Call callback to announce the event to the user */
        if (master->callback != NULL)
        {
            master->callback(EIO_I2S_EVENT_END_TRANSFER, master->callbackParam);
        }
    }
}

/*!
 * @brief function called at the end of a DMA Tx transfer.
 *
 * @param[in] stateStruct: Pointer to the EIO_I2S driver context structure
 * @param[in] status: DMA channel status
 * @return none
 */
static void EIO_I2S_DRV_MasterEndDmaTxTransfer(void *stateStruct, dma_chn_status_t status)
{
    eio_i2s_master_state_t *master;
    uint8_t dmaChn;

    DEVICE_ASSERT(stateStruct != NULL);

    master = (eio_i2s_master_state_t *)stateStruct;

    /* Call callback to allow user to provide a new buffer */
    if (status == DMA_CHN_ERROR)
    {
        /* DMA error, stop transfer */
        master->status = STATUS_ERROR;
        /* End transfer */
        EIO_I2S_DRV_MasterStopTransfer(master);
        /* Call callback to announce the event to the user */
        if (master->callback != NULL)
        {
            master->callback(EIO_I2S_EVENT_END_TRANSFER, master->callbackParam);
        }
        return;
    }
    else
    {
        if (master->callback != NULL)
        {
            master->callback(EIO_I2S_EVENT_TX_EMPTY, master->callbackParam);
        }
    }
    if (master->txRemainingBytes == 0U)
    {
        /* No more data to transmit, transmission will stop */
        master->txData = NULL;
    }
    else
    {
        /* There is more data to transfer, restart DMA channel */
        /* Update buffer address and size */
        dmaChn = master->txDMAChannel;
        DMA_DRV_SetSrcAddr(dmaChn, (uint32_t)(master->txData), (uint32_t)((master->txData) + (master->txRemainingBytes)));
        DMA_DRV_SetTranferLength(dmaChn, master->txRemainingBytes);
        /* Now that this tx is set up, clear remaining bytes count */
        master->txRemainingBytes = 0U;
        /* Start the channel */
        (void)DMA_DRV_StartChannel(dmaChn);
    }
}

/*!
 * @brief function called at the end of a DMA Rx transfer.
 *
 * @param[in] stateStruct: Pointer to the EIO_I2S driver context structure
 * @param[in] status: DMA channel status
 * @return none
 */
static void EIO_I2S_DRV_MasterEndDmaRxTransfer(void *stateStruct, dma_chn_status_t status)
{
    eio_i2s_master_state_t *master;
    uint8_t dmaChn;
    uint32_t addr;

    DEVICE_ASSERT(stateStruct != NULL);

    (void)status;
    master = (eio_i2s_master_state_t *)stateStruct;

    /* Check if this was a reception */
    if (master->rxData != NULL)
    {
        if (status == DMA_CHN_ERROR)
        {
            /* DMA error, stop transfer */
            master->status = STATUS_ERROR;
        }
        else
        {
            /* Call callback to allow user to provide a new buffer */
            if (master->callback != NULL)
            {
                master->callback(EIO_I2S_EVENT_RX_FULL, master->callbackParam);
            }
        }
    }
    if (master->rxRemainingBytes == 0U)
    {
        /* No more data, end transfer */
        if (master->status == STATUS_BUSY)
        {
            master->status = STATUS_SUCCESS;
        }
        /* End transfer */
        EIO_I2S_DRV_MasterStopTransfer(master);
        /* Call callback to announce the event to the user */
        if (master->callback != NULL)
        {
            master->callback(EIO_I2S_EVENT_END_TRANSFER, master->callbackParam);
        }
    }
    else
    {
        /* There is more data to transfer, restart DMA channel */
        /* Update buffer address and size */
        dmaChn = master->rxDMAChannel;
        if (master->rxData != NULL)
        {
            addr = (uint32_t)(master->rxData);
        }
        else
        {
            /* if there is no data to receive, use dummy data as destination for DMA transfer */
            addr = (uint32_t)(&(master->dummyDmaData));
        }
        DMA_DRV_SetDestAddr(dmaChn, addr, addr + (master->rxRemainingBytes));
        DMA_DRV_SetTranferLength(dmaChn, master->rxRemainingBytes);
        /* Now that this rx is set up, clear remaining bytes count */
        master->rxRemainingBytes = 0U;
        /* Start the channel */
        (void)DMA_DRV_StartChannel(dmaChn);
    }
}


/*!
 * @brief Computes the address of the register used for DMA tx transfer
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @return none
 */
static uint32_t EIO_I2S_DRV_MasterComputeTxRegAddr(const eio_i2s_master_state_t *master)
{
    uint32_t addr;
    const EIO_Type *baseAddr;
    uint8_t shifter;

    baseAddr = g_eioBase[master->eioCommon.instance];
    shifter = I2S_TX_SHIFTER(master->eioCommon.resourceIndex);
    addr = (uint32_t)(&(baseAddr->SHIFTBUFBIS[shifter])) + (sizeof(uint32_t) - master->byteWidth);
    return addr;
}

/*!
 * @brief Computes the address of the register used for DMA rx transfer
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @return none
 */
static uint32_t EIO_I2S_DRV_MasterComputeRxRegAddr(const eio_i2s_master_state_t *master)
{
    uint32_t addr;
    const EIO_Type *baseAddr;
    uint8_t shifter;

    baseAddr = g_eioBase[master->eioCommon.instance];
    shifter = I2S_RX_SHIFTER(master->eioCommon.resourceIndex);
    addr = (uint32_t)(&(baseAddr->SHIFTBUFBIS[shifter]));
    return addr;
}

/*!
 * @brief Starts a DMA transfer
 *
 * @param[in] master: Pointer to the eio_I2S master driver context structure
 * @return  none
 */
static void EIO_I2S_DRV_MasterStartDmaTransfer(eio_i2s_master_state_t *master)
{
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */
    EIO_Type *baseAddr;
    uint32_t addr;
    uint8_t requestMask = 0U;

    /* Table to map EIO_i2s transfer sizes to EDMA transfer sizes */
    static const dma_transfer_size_t dmaTransferSize[4U] =
        {DMA_TRANSFER_SIZE_1B, DMA_TRANSFER_SIZE_2B, DMA_TRANSFER_SIZE_4B, DMA_TRANSFER_SIZE_4B};

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Configure Tx channel if this is a transmission */
    if (master->txData != NULL)
    {
        /* Configure the transfer control descriptor for the Tx channel */
        (void)DMA_DRV_ConfigTransfer(master->txDMAChannel,
                           DMA_TRANSFER_MEM2PERIPH,
                           (uint32_t)(master->txData),
                           EIO_I2S_DRV_MasterComputeTxRegAddr(master),
                           dmaTransferSize[(uint32_t)master->byteWidth - 1U],
                           master->txRemainingBytes);

        /* Now that this tx is set up, clear remaining bytes count */
        master->txRemainingBytes = 0U;

        /* Setup callback for DMA tx transfer end */
        (void)DMA_DRV_InstallCallback(master->txDMAChannel,
                                       (dma_callback_t)(EIO_I2S_DRV_MasterEndDmaTxTransfer),
                                       (void*)(master));
        /* Start tx DMA channel */
        (void)DMA_DRV_StartChannel(master->txDMAChannel);
        requestMask = (uint8_t)(1U << I2S_TX_SHIFTER(resourceIndex));
    }
    /* Configure Rx channel; if this is a transmission we still use Rx for the "end transfer" event */
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
                           EIO_I2S_DRV_MasterComputeRxRegAddr(master),
                           addr,
                           dmaTransferSize[(uint32_t)master->byteWidth - 1U],
                           master->rxRemainingBytes);

    if (master->rxData == NULL)
    {
        /* if there is no data to receive, don't increment destination offset */
        DMA_DRV_SetDestOffset(master->rxDMAChannel, 0U);
    }
    /* Now that this rx is set up, clear remaining bytes count */
    master->rxRemainingBytes = 0U;

    /* Setup callback for DMA rx transfer end */
    (void)DMA_DRV_InstallCallback(master->rxDMAChannel,
                                   (dma_callback_t)(EIO_I2S_DRV_MasterEndDmaRxTransfer),
                                   (void*)(master));
    /* Start rx DMA channel */
    (void)DMA_DRV_StartChannel(master->rxDMAChannel);
    requestMask |= (uint8_t)(1U << I2S_RX_SHIFTER(resourceIndex));

    /* Enable EIO DMA requests */
    EIO_SetShifterDMARequest(baseAddr, requestMask, true);
}

/*!
 * @brief Initialize the EIO_I2S master mode driver
 *        This function initializes the EIO_I2S driver in master mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_I2S master user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] master: Pointer to the EIO_I2S master driver context structure. The driver uses
 *                    this memory area for its internal logic. The application must make no
 *                    assumptions about the content of this structure, and must not free this
 *                    memory until the driver is de-initialized using EIO_I2S_DRV_MasterDeinit().
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterInit(uint32_t instance,
                                const eio_i2s_master_user_config_t *userConfigPtr,
                                eio_i2s_master_state_t *master)
{
    uint32_t inputClock;
    status_t clkErr;
    status_t retCode;
    status_t osifError = STATUS_SUCCESS;
    uint8_t dmaReqTx;
    uint8_t dmaReqRx;
    EIO_Type *baseAddr = g_eioBase[instance];

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
    retCode = EIO_DRV_InitDriver(instance, (eio_common_state_t *)master);
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
    master->bitsWidth = userConfigPtr->bitsWidth;
    master->byteWidth = EIO_I2S_DRV_ComputeByteWidth(userConfigPtr->bitsWidth);
    master->driverIdle = true;
    master->callback = userConfigPtr->callback;
    master->callbackParam = userConfigPtr->callbackParam;
    master->blocking = false;
    master->txData = NULL;
    master->txRemainingBytes = 0U;
    master->rxData = NULL;
    master->rxRemainingBytes = 0U;
    master->master = true;
    master->status = STATUS_SUCCESS;

    /* Configure device for I2S mode */
    EIO_I2S_DRV_MasterConfigure(master, userConfigPtr, inputClock);

    /* Set up transfer engine */
    switch (master->driverType)
    {
        case EIO_DRIVER_TYPE_INTERRUPTS:
            master->eioCommon.isr = EIO_I2S_DRV_MasterCheckStatus;
            break;

        case EIO_DRIVER_TYPE_POLLING:
            /* Nothing to do here, EIO_I2S_DRV_MasterGetStatus() will handle the transfer */
            break;

        case EIO_DRIVER_TYPE_DMA:
            /* Store DMA channel numbers */
            master->rxDMAChannel = userConfigPtr->rxDMAChannel;
            master->txDMAChannel = userConfigPtr->txDMAChannel;
            /* Configure DMA request sources */
            dmaReqTx = g_eioDMASrc[instance][I2S_TX_SHIFTER(master->eioCommon.resourceIndex)];
            dmaReqRx = g_eioDMASrc[instance][I2S_RX_SHIFTER(master->eioCommon.resourceIndex)];
            (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->txDMAChannel, dmaReqTx, false);
            (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->rxDMAChannel, dmaReqRx, false);
            EIO_SetShifterDMARequest(baseAddr, (uint8_t)(((1U << I2S_TX_SHIFTER(master->eioCommon.resourceIndex))) | ((1U << I2S_RX_SHIFTER(master->eioCommon.resourceIndex)))), true);
            break;

        default:
            /* Impossible type - do nothing */
            break;
    }
    s_i2sInit = true;
    (void)clkErr;
    (void)osifError;
    return STATUS_SUCCESS;
}

/*!
 * @brief De-initialize the EIO_I2S master mode driver
 *        This function de-initializes the EIO_I2S driver in master mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterDeinit(eio_i2s_master_state_t *master)
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

    return EIO_DRV_DeinitDriver((eio_common_state_t *)master);
}

/*!
 * @brief Set the baud rate and bit width for any subsequent I2S communication
 *        This function sets the baud rate (SCK frequency) and bit width for the I2S master.
 *        Note that due to module limitation not any baud rate can be achieved. The driver
 *        will set a baud rate as close as possible to the requested baud rate, but there may
 *        still be substantial differences, for example if requesting a high baud rate while
 *        using a low-frequency EIO clock. The application should call
 *        EIO_I2S_DRV_MasterGetBaudRate() after EIO_I2S_DRV_MasterSetConfig() to check
 *        what baud rate was actually set.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] baudRate: the desired baud rate in hertz
 * @param[in] bitsWidth: number of bits per word
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSetConfig(eio_i2s_master_state_t *master,
                                     uint32_t baudRate,
                                     uint8_t bitsWidth)
{
    EIO_Type *baseAddr;
    uint16_t divider;
    uint32_t inputClock;
    status_t clkErr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(bitsWidth <= 32U);
    /* for DMA transfers bitsWidth must 8, 16, or 32 */
    DEVICE_ASSERT(!((master->driverType == EIO_DRIVER_TYPE_DMA) &&
                    (bitsWidth != 8U) && (bitsWidth != 16U) && (bitsWidth != 32U)
                   ));

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
    EIO_I2S_DRV_MasterComputeBaudRateDivider(baudRate, &divider, inputClock);

    /* Configure SCK timer */
    EIO_SetTimerCompare(baseAddr, I2S_SCK_TIMER(resourceIndex), (uint16_t)((((uint16_t)((uint16_t)bitsWidth << 1U) - 1U) << 8U) + divider));
    /* Configure WS timer */
    EIO_SetTimerCompare(baseAddr, I2S_WS_TIMER(resourceIndex), (uint16_t)((bitsWidth * ((divider + 1U) * 2U)) - 1U));

    master->bitsWidth = bitsWidth;
    master->byteWidth = EIO_I2S_DRV_ComputeByteWidth(bitsWidth);

    (void)clkErr;

    return STATUS_SUCCESS;
}

/*!
 * @brief Get the currently configured baud rate
 *        This function returns the currently configured I2S baud rate.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] baudRate: the current baud rate in hertz
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterGetBaudRate(eio_i2s_master_state_t *master, uint32_t *baudRate)
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
    timerCmp = EIO_GetTimerCompare(baseAddr, I2S_SCK_TIMER(resourceIndex));
    divider = (uint16_t)(timerCmp & 0x00FFU);

    /* Compute baud rate: input_clock / (2 * (divider + 1)). Round to nearest integer */
    *baudRate = (inputClock + (uint32_t)divider + 1U) / (2U * ((uint32_t)divider + 1U));

    (void)clkErr;

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a non-blocking send transaction on the I2S bus
 *        This function starts the transmission of a block of data and returns immediately.
 *        The rest of the transmission is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2S_DRV_MasterGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2S_DRV_MasterGetStatus() to check the progress of the transmission.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSendData(eio_i2s_master_state_t *master,
                                    const uint8_t *txBuff,
                                    uint32_t txSize)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);
    DEVICE_ASSERT((txSize % (uint32_t)(master->byteWidth)) == 0U);

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    master->txData = txBuff;
    master->txRemainingBytes = txSize;
    /* also count received data, it tells us when tx is actually completed */
    if (master->clockContinueOutput)
    {
        if (s_i2sInit)
        {
            master->rxRemainingBytes = txSize;
            s_i2sInit = false;
        }
        else
        {
            master->rxRemainingBytes = txSize + master->byteWidth;
        }
    }
    else
    {
        master->rxRemainingBytes = txSize;
    }
    master->rxData = NULL;
    master->status = STATUS_BUSY;
    master->driverIdle = false;

    /* Check buff address in memmory*/
    if (master->driverType == EIO_DRIVER_TYPE_DMA)
    {
        if (((uint32_t)(master->txData) < 0x20000000U) || ((uint32_t)(master->txData) > 0x2000EFFFU))
        {
            return STATUS_UNSUPPORTED;
        }
        if (((uint32_t)(&(master->dummyDmaData)) < 0x20000000U) || ((uint32_t)(&(master->dummyDmaData)) > 0x2000EFFFU))
        {
            return STATUS_UNSUPPORTED;
        }
    }
    /* Enable pin output */
    EIO_SetShifterPinConfig(baseAddr, I2S_TX_SHIFTER(resourceIndex), EIO_PIN_CONFIG_OUTPUT);
    /* Enable transfer engine */
    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        /* Enable interrupts for Tx shifter */
        EIO_SetShifterInterrupt(baseAddr,
                                (uint8_t)(1U << I2S_TX_SHIFTER(resourceIndex)),
                                true);
        EIO_SetShifterErrorInterrupt(baseAddr,
                                     (uint8_t)(1U << I2S_TX_SHIFTER(resourceIndex)),
                                     true);
        /* Enable timers and shifters */
        EIO_I2S_DRV_MasterEnableTransfer(master);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        /* Enable timers and shifters */
        EIO_I2S_DRV_MasterEnableTransfer(master);
        /* Call EIO_I2S_DRV_MasterCheckStatus once to send the first byte */
        EIO_I2S_DRV_MasterCheckStatus(master);
        break;

    case EIO_DRIVER_TYPE_DMA:
        EIO_I2S_DRV_MasterStartDmaTransfer(master);
        /* Enable timers and shifters */
        EIO_I2S_DRV_MasterEnableTransfer(master);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a blocking send transaction on the I2S bus
 *        This function sends a block of data, and
 *        only returns when the transmission is complete.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSendDataBlocking(eio_i2s_master_state_t *master,
                                            const uint8_t *txBuff,
                                            uint32_t txSize,
                                            uint32_t timeout)
{
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
    /* Call EIO_I2S_DRV_MasterSendData to start transfer */
    (void)EIO_I2S_DRV_MasterSendData(master, txBuff, txSize);

    /* Wait for transfer to end */
    return EIO_I2S_DRV_MasterWaitTransferEnd(master, timeout);
}

/*!
 * @brief Perform a non-blocking receive transaction on the I2S bus
 *        This function starts the reception of a block of data and returns immediately.
 *        The rest of the reception is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2S_DRV_MasterGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2S_DRV_MasterGetStatus() to check the progress of the reception.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterReceiveData(eio_i2s_master_state_t *master,
                                       uint8_t *rxBuff,
                                       uint32_t rxSize)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);
    DEVICE_ASSERT((rxSize % (uint32_t)(master->byteWidth)) == 0U);

    baseAddr = g_eioBase[master->eioCommon.instance];
    resourceIndex = master->eioCommon.resourceIndex;

    /* Check if driver is busy */
    if (!master->driverIdle)
    {
        return STATUS_BUSY;
    }

    master->rxData = rxBuff;
    master->rxRemainingBytes = rxSize;
    master->txData = NULL;
    master->txRemainingBytes = 0U;
    master->status = STATUS_BUSY;
    master->driverIdle = false;
    /* Enable transfer engine */
    switch (master->driverType)
    {
    case EIO_DRIVER_TYPE_INTERRUPTS:
        EIO_I2S_DRV_MasterEnableTransfer(master);
        /* Enable interrupts for Rx shifter */
        EIO_SetShifterInterrupt(baseAddr,
                                (uint8_t)(1U << I2S_RX_SHIFTER(resourceIndex)),
                                true);
        EIO_SetShifterErrorInterrupt(baseAddr,
                                     (uint8_t)(1U << I2S_RX_SHIFTER(resourceIndex)),
                                     true);
        break;

    case EIO_DRIVER_TYPE_POLLING:
        EIO_I2S_DRV_MasterEnableTransfer(master);
        /* Call EIO_I2S_DRV_MasterCheckStatus once to send the first byte */
        EIO_I2S_DRV_MasterCheckStatus(master);
        break;

    case EIO_DRIVER_TYPE_DMA:
        /* Check buff address in memory*/
        if (((uint32_t)(master->rxData) < 0x20000000U) || ((uint32_t)(master->rxData) > 0x2000EFFFU))
        {
            return STATUS_UNSUPPORTED;
        }
        if (((uint32_t)(&(master->dummyDmaData)) < 0x20000000U) || ((uint32_t)(&(master->dummyDmaData)) > 0x2000EFFFU))
        {
            return STATUS_UNSUPPORTED;
        }
        EIO_I2S_DRV_MasterStartDmaTransfer(master);
        EIO_I2S_DRV_MasterEnableTransfer(master);
        break;

    default:
        /* Impossible type - do nothing */
        break;
    }

    /* For master we need to send a dummy char to start the clock.
       For slave just put a 0 in the buffer to keep the tx line clear while receiving. */
    EIO_WriteShifterBuffer(baseAddr, I2S_TX_SHIFTER(resourceIndex), 0x0U, EIO_SHIFTER_RW_MODE_BIT_SWAP);

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a blocking receive transaction on the I2S bus
 *         This function receives a block of data and only returns when the reception is complete.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterReceiveDataBlocking(eio_i2s_master_state_t *master,
                                               uint8_t *rxBuff,
                                               uint32_t rxSize,
                                               uint32_t timeout)
{
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
    /* Call EIO_I2S_DRV_MasterReceiveData to start transfer */
    (void)EIO_I2S_DRV_MasterReceiveData(master, rxBuff, rxSize);

    /* Wait for transfer to end */
    return EIO_I2S_DRV_MasterWaitTransferEnd(master, timeout);
}

/*!
 * @brief Aborts a non-blocking I2S master transaction
 *        This function aborts a non-blocking I2S transfer.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterTransferAbort(eio_i2s_master_state_t *master)
{
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (master->driverIdle)
    {
        return STATUS_SUCCESS;
    }

    master->status = STATUS_I2S_ABORTED;
    EIO_I2S_DRV_MasterStopTransfer(master);

    return STATUS_SUCCESS;
}

/*!
 * @brief Get the status of the current non-blocking I2S master transaction
 *        This function returns the current status of a non-blocking I2S master transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterGetStatus(eio_i2s_master_state_t *master, uint32_t *bytesRemaining)
{
    uint32_t remainingBytes;

    DEVICE_ASSERT(master != NULL);

    /* Use rxRemainingBytes even for transmit; byte is not transmitted
    until rx shifter reports a receive event */
    remainingBytes = master->rxRemainingBytes;

    if (!master->driverIdle)
    {
        switch (master->driverType)
        {
        case EIO_DRIVER_TYPE_POLLING:
            /* In polling mode advance the I2S transfer here */
            EIO_I2S_DRV_MasterCheckStatus(master);
            break;

        case EIO_DRIVER_TYPE_DMA:
            /* In DMA mode just update the remaining count.
            DO NOT write master->rxRemainingBytes directly !!! */
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
 * @brief Provide a buffer for receiving data
 *        This function can be used to provide a new buffer for receiving data to the driver.
 *        It can be called from the user callback when event STATUS_I2S_RX_OVERRUN is reported.
 *        This way the reception will continue without interruption.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSetRxBuffer(eio_i2s_master_state_t *master,
                                       uint8_t *rxBuff,
                                       uint32_t rxSize)
{
    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    master->rxData = rxBuff;
    master->rxRemainingBytes = rxSize;

    return STATUS_SUCCESS;
}

/*!
 * @brief Provide a buffer for transmitting data
 *        This function can be used to provide a new buffer for transmitting data to the driver.
 *        It can be called from the user callback when event STATUS_I2S_TX_UNDERRUN is reported.
 *        This way the transmission will continue without interruption.
 *
 * @param[in] master: Pointer to the EIO_I2S master driver context structure.
 * @param[in] txBuff: pointer to the buffer containing transmit data
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_MasterSetTxBuffer(eio_i2s_master_state_t *master,
                                       const uint8_t *txBuff,
                                       uint32_t txSize)
{
    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    master->txData = txBuff;
    master->txRemainingBytes = txSize;
    /* for transmit we also count received bytes for end condition */
    master->rxRemainingBytes += txSize;

    return STATUS_SUCCESS;
}

/*!
 * @brief Initialize the EIO_I2S slave mode driver
 *        This function initializes the EIO_I2S driver in slave mode.
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] userConfigPtr: Pointer to the EIO_I2S slave user configuration structure. The function
 *                           reads configuration data from this structure and initializes the
 *                           driver accordingly. The application may free this structure after
 *                           the function returns.
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure. The driver uses
 *                   this memory area for its internal logic. The application must make no
 *                   assumptions about the content of this structure, and must not free this
 *                   memory until the driver is de-initialized using EIO_I2S_DRV_SlaveDeinit().
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveInit(uint32_t instance,
                               const eio_i2s_slave_user_config_t *userConfigPtr,
                               eio_i2s_slave_state_t *slave)
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
    slave->eioCommon.resourceCount = 3U;
    /* Common EIO driver initialization */
    retCode = EIO_DRV_InitDriver(instance, (eio_common_state_t *)slave);
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
    /* Initialize driver-specific context structure */
    slave->driverType = userConfigPtr->driverType;
    slave->bitsWidth = userConfigPtr->bitsWidth;
    slave->byteWidth = EIO_I2S_DRV_ComputeByteWidth(userConfigPtr->bitsWidth);
    slave->driverIdle = true;
    slave->callback = userConfigPtr->callback;
    slave->callbackParam = userConfigPtr->callbackParam;
    slave->blocking = false;
    slave->txData = NULL;
    slave->txRemainingBytes = 0U;
    slave->rxData = NULL;
    slave->rxRemainingBytes = 0U;
    slave->master = false;
    slave->status = STATUS_SUCCESS;

    EIO_Type *baseAddr = g_eioBase[instance];
    /* Configure device for I2S mode */
    EIO_I2S_DRV_SlaveConfigure(slave, userConfigPtr);

    /* Set up transfer engine */
    switch (slave->driverType)
    {
        case EIO_DRIVER_TYPE_INTERRUPTS:
            slave->eioCommon.isr = EIO_I2S_DRV_MasterCheckStatus;
            break;

        case EIO_DRIVER_TYPE_POLLING:
            /* Nothing to do here, EIO_I2S_DRV_MasterGetStatus() will handle the transfer */
            break;

        case EIO_DRIVER_TYPE_DMA:
            /* Store DMA channel numbers */
            slave->rxDMAChannel = userConfigPtr->rxDMAChannel;
            slave->txDMAChannel = userConfigPtr->txDMAChannel;
            /* Configure DMA request sources */
            dmaReqTx = g_eioDMASrc[instance][I2S_TX_SHIFTER(slave->eioCommon.resourceIndex)];
            dmaReqRx = g_eioDMASrc[instance][I2S_RX_SHIFTER(slave->eioCommon.resourceIndex)];
            (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->txDMAChannel, dmaReqTx, false);
            (void)DMA_DRV_SetChannelRequestAndTrigger(userConfigPtr->rxDMAChannel, dmaReqRx, false);
            EIO_SetShifterDMARequest(baseAddr, (uint8_t)(((1U << I2S_TX_SHIFTER(slave->eioCommon.resourceIndex))) | ((1U << I2S_RX_SHIFTER(slave->eioCommon.resourceIndex)))), true);
            break;

        default:
            /* Impossible type - do nothing */
            break;
    }

    (void)osifError;
    return STATUS_SUCCESS;
}


/*!
 * @brief Set the bit width for any subsequent I2S communication
 *        This function sets the bit width for the I2S slave.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] bitsWidth: number of bits per word
 * @return  Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSetConfig(eio_i2s_slave_state_t *slave,
                                    uint8_t bitsWidth)
{
    EIO_Type *baseAddr;
    uint8_t resourceIndex;    /* Index of first used internal resource instance (shifter and timer) */

    DEVICE_ASSERT(slave != NULL);
    DEVICE_ASSERT(bitsWidth <= 32U);
    /* for DMA transfers bitsWidth must 8, 16, or 32 */
    DEVICE_ASSERT(!((slave->driverType == EIO_DRIVER_TYPE_DMA) &&
                    (bitsWidth != 8U) && (bitsWidth != 16U) && (bitsWidth != 32U)
                   ));

    baseAddr = g_eioBase[slave->eioCommon.instance];
    resourceIndex = slave->eioCommon.resourceIndex;

    /* Check if driver is busy */
    if (!slave->driverIdle)
    {
        return STATUS_BUSY;
    }

    slave->bitsWidth = bitsWidth;
    slave->byteWidth = EIO_I2S_DRV_ComputeByteWidth(bitsWidth);

    /* Configure SCK timer */
    EIO_SetTimerCompare(baseAddr, I2S_SCK_TIMER(resourceIndex), (uint16_t)((uint16_t)((uint16_t)bitsWidth << 2U) - 3U));
    /* Configure I2S_WS_TIMER_RISING_BLK timer */
    EIO_SetTimerCompare(baseAddr, I2S_WS_TIMER_RISING_BLK(resourceIndex), (uint16_t)((uint16_t)((uint16_t)bitsWidth << 2U) - 4U));
    /* Configure WS timer */
    EIO_SetTimerCompare(baseAddr, I2S_WS_TIMER(resourceIndex), (uint16_t)((uint16_t)((uint16_t)bitsWidth << 1U) - 1U));

    return STATUS_SUCCESS;
}

/*!
 * @brief Returns default configuration structure for EIO_I2S master
 *
 * @param[in] userConfigPtr: Pointer to the EIO_I2S user configuration structure.
 * @return none
 */
void EIO_I2S_DRV_MasterGetDefaultConfig(eio_i2s_master_user_config_t *userConfigPtr)
{
    DEVICE_ASSERT(userConfigPtr != NULL);

    userConfigPtr->driverType = EIO_DRIVER_TYPE_INTERRUPTS;
    userConfigPtr->baudRate = 256000U;   /* 8 kHz sampling rate, 16 bits per sample */
    userConfigPtr->bitsWidth = 16U;
    userConfigPtr->txPin = 0U;
    userConfigPtr->rxPin = 1U;
    userConfigPtr->sckPin = 2U;
    userConfigPtr->wsPin = 3U;
    userConfigPtr->callback = NULL;
    userConfigPtr->callbackParam = NULL;
    userConfigPtr->rxDMAChannel = 255U;
    userConfigPtr->txDMAChannel = 255U;
}

/*!
 * @brief Returns default configuration structure for EIO_I2S slave
 *
 * @param[in] userConfigPtr: Pointer to the EIO_I2S user configuration structure.
 * @return  none
 */
void EIO_I2S_DRV_SlaveGetDefaultConfig(eio_i2s_slave_user_config_t *userConfigPtr)
{
    DEVICE_ASSERT(userConfigPtr != NULL);

    userConfigPtr->driverType = EIO_DRIVER_TYPE_INTERRUPTS;
    userConfigPtr->bitsWidth = 16U;
    userConfigPtr->txPin = 0U;
    userConfigPtr->rxPin = 1U;
    userConfigPtr->sckPin = 2U;
    userConfigPtr->wsPin = 3U;
    userConfigPtr->callback = NULL;
    userConfigPtr->callbackParam = NULL;
    userConfigPtr->rxDMAChannel = 255U;
    userConfigPtr->txDMAChannel = 255U;
}

/*!
 * @brief De-initialize the EIO_I2S slave mode driver
 *        This function de-initializes the EIO_I2S driver in slave mode. The driver can't be used
 *        again until reinitialized. The context structure is no longer needed by the driver and
 *        can be freed after calling this function.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveDeinit(eio_i2s_slave_state_t *slave)
{
    return EIO_I2S_DRV_MasterDeinit(slave);
}

/*!
 * @brief Perform a non-blocking send transaction on the I2S bus
 *        This function starts the transmission of a block of data and returns immediately.
 *        The rest of the transmission is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2S_DRV_SlaveGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2S_DRV_SlaveGetStatus() to check the progress of the transmission.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSendData(eio_i2s_slave_state_t *slave,
                                   const uint8_t *txBuff,
                                   uint32_t txSize)
{
    return EIO_I2S_DRV_MasterSendData(slave, txBuff, txSize);
}

/*!
 * @brief Perform a blocking send transaction on the I2S bus
 *        This function sends a block of data, and
 *        only returns when the transmission is complete.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] txBuff: pointer to the data to be transferred
 * @param[in] txSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSendDataBlocking(eio_i2s_slave_state_t *slave,
                                           const uint8_t *txBuff,
                                           uint32_t txSize,
                                           uint32_t timeout)
{
    return EIO_I2S_DRV_MasterSendDataBlocking(slave, txBuff, txSize, timeout);
}

/*!
 * @brief Perform a non-blocking receive transaction on the I2S bus
 *        This function starts the reception of a block of data and returns immediately.
 *        The rest of the reception is handled by the interrupt service routine (if the driver
 *        is initialized in interrupt mode) or by the EIO_I2S_DRV_SlaveGetStatus function (if
 *        the driver is initialized in polling mode).
 *        Use EIO_I2S_DRV_SlaveGetStatus() to check the progress of the reception.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveReceiveData(eio_i2s_slave_state_t *slave,
                                      uint8_t *rxBuff,
                                      uint32_t rxSize)
{
    return EIO_I2S_DRV_MasterReceiveData(slave, rxBuff, rxSize);
}

/*!
 * @brief Perform a blocking receive transaction on the I2S bus
 *        This function receives a block of data and only returns when the reception is complete.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @param[in] timeout: timeout for the transfer in milliseconds
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveReceiveDataBlocking(eio_i2s_slave_state_t *slave,
                                              uint8_t *rxBuff,
                                              uint32_t rxSize,
                                              uint32_t timeout)
{
    return EIO_I2S_DRV_MasterReceiveDataBlocking(slave, rxBuff, rxSize, timeout);
}

/*!
 * @brief Aborts a non-blocking I2S slave transaction
 *        This function aborts a non-blocking I2S transfer.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveTransferAbort(eio_i2s_slave_state_t *slave)
{
    return EIO_I2S_DRV_MasterTransferAbort(slave);
}


/*!
 * @brief Get the status of the current non-blocking I2S slave transaction
 *        This function returns the current status of a non-blocking I2S slave transaction.
 *        A return code of STATUS_BUSY means the transfer is still in progress.
 *        Otherwise the function returns a status reflecting the outcome of the last transfer.
 *        When the driver is initialized in polling mode this function also advances the transfer
 *        by checking and handling the transmit and receive events, so it must be called
 *        frequently to avoid overflows or underflows.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] bytesRemaining: the remaining number of bytes to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveGetStatus(eio_i2s_slave_state_t *slave, uint32_t *bytesRemaining)
{
    return EIO_I2S_DRV_MasterGetStatus(slave, bytesRemaining);
}

/*!
 * @brief Provide a buffer for receiving data
 *        This function can be used to provide a driver with a new buffer for receiving data.
 *        It can be called from the user callback when event STATUS_I2S_RX_OVERRUN is reported.
 *        This way the reception will continue without interruption.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] rxBuff: pointer to the buffer where to store received data
 * @param[in] rxSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSetRxBuffer(eio_i2s_slave_state_t *slave,
                                      uint8_t *rxBuff,
                                      uint32_t rxSize)
{
    return EIO_I2S_DRV_MasterSetRxBuffer(slave, rxBuff, rxSize);
}

/*!
 * @brief Provide a buffer for transmitting data
 *        This function can be used to provide a driver with a new buffer for transmitting data.
 *        It can be called from the user callback when event STATUS_I2S_TX_UNDERRUN is reported.
 *        This way the transmission will continue without interruption.
 *
 * @param[in] slave: Pointer to the EIO_I2S slave driver context structure.
 * @param[in] txBuff: pointer to the buffer containing transmit data
 * @param[in] txSize: length in bytes of the data to be transferred
 * @return Error or success status returned by API
 */
status_t EIO_I2S_DRV_SlaveSetTxBuffer(eio_i2s_slave_state_t *slave,
                                      const uint8_t *txBuff,
                                      uint32_t txSize)
{
    return EIO_I2S_DRV_MasterSetTxBuffer(slave, txBuff, txSize);
}

/* =============================================  EOF  ============================================== */
