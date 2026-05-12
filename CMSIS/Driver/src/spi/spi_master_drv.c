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
 * @file spi_master_drv.c
 *
 * @brief This file provides spi master integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "spi_master_drv.h"
#include "dma_drv.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */
#define SPI_MICROSECONDS (1000000UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table to save PCC clock source, for getting the input clcok frequency. */
static const clock_names_t s_spiClock[SPI_INSTANCE_MAX] = SPI_CLOCK_NAMES;

/* ====================================  Functions declaration  ===================================== */
/* This function initialize a new SPI transfer */
static status_t SPI_MasterStartTransfer(uint32_t instance,
                                        const uint8_t *sendBuffer,
                                        uint8_t *receiveBuffer,
                                        uint16_t transferByteCount);

/* This function cleans up state structure and hardware after a transfer is complete .*/
static void SPI_MasterCompleteTransfer(uint32_t instance);

/* Callback for DMA transfer done.*/
static void SPI_MasterCompletDMATransfer(void *parameter, dma_chn_status_t status);

/* Callback for RX DMA channel*/
static void SPI_MasterCompleteRX(void *parameter, dma_chn_status_t status);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get default configuration for SPI master.
 *
 * @param[out] spiConfig: Pointer to configuration structure which is filled
 *             with default configuration
 * @return none
 */
void SPI_DRV_MasterGetDefaultConfig(spi_master_config_t *spiConfig)
{
    DEVICE_ASSERT(spiConfig != NULL);

    spiConfig->bitsPerSec       = 50000U;
    spiConfig->whichPcs         = SPI_PCS0;
    spiConfig->pcsPolarity      = SPI_ACTIVE_LOW;
    spiConfig->isPcsContinuous  = false;
    spiConfig->bitcount         = 8U;
    spiConfig->spiSrcClk        = 8000000U;
    spiConfig->clkPhase         = SPI_CLOCK_PHASE_1ST_EDGE;
    spiConfig->clkPolarity      = SPI_SCK_ACTIVE_HIGH;
    spiConfig->msbFirst         = true;
    spiConfig->transferType     = SPI_USING_INTERRUPTS;
    spiConfig->rxDMAChannel     = 0U;
    spiConfig->txDMAChannel     = 0U;
    spiConfig->callback         = NULL;
    spiConfig->callbackParam    = NULL;
}

/*!
 * @brief Initializes a SPI instance.
 *
 * @param[in] instance: SPI instance
 * @param[in] spiState: The pointer to the SPI master driver state structure
 * @param[in] spiConfig: The data structure containing information about a device on the SPI bus
 * @return An error code or STATUS_SUCCESS.
 */
status_t SPI_DRV_MasterInit(uint32_t instance, spi_state_t *spiState, const spi_master_config_t *spiConfig)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(spiState != NULL);
    DEVICE_ASSERT(spiConfig != NULL);

    SPI_Type *base = g_spiBase[instance];
    status_t errorCode = STATUS_SUCCESS;

    /* Save runtime structure pointers */
    g_spiStatePtr[instance] = spiState;

    /* Enable SPI bus clock and release module from reset state */
    CKGEN_DRV_Enable(g_spiBusClock[instance], true);
    CKGEN_DRV_SoftReset(g_spiSoftReset[instance], true);

    /* Set for master mode */
    SPI_SetMasterSlaveMode(base, SPI_MASTER);
    /* Set Pin configuration such that SOUT as MOSI, SIN as MISO */
    SPI_SetPinConfigMode(base, SPI_SOUT_MOSI_SIN_MISO);

    /* Configure bus for this device.*/
    errorCode = SPI_DRV_MasterConfigureBus(instance, spiConfig, NULL);
    if (errorCode != STATUS_SUCCESS)
    {
        return errorCode;
    }
    /* When TX is null the value sent on the bus will be 0 */
    spiState->dummy = 0U;
    /* Initialize the semaphore */
    errorCode = OSIF_SemaCreate(&(spiState->spiSemaphore), 0U);
    DEVICE_ASSERT(STATUS_SUCCESS == errorCode);

    /* Enable SPI interrupt. */
    NVIC_EnableIRQ(g_spiIrqId[instance]);

    /* Finally, enable SPI */
    SPI_SetEnable(base, true);

    return errorCode;
}

/*!
 * @brief De-initializes SPI.
 *
 * @param[in] instance: SPI instance
 * @return STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_DRV_MasterDeinit(uint32_t instance)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    /* Instantiate local variable of type spi_state_t and point to global state */
    const spi_state_t *spiState = g_spiStatePtr[instance];
    status_t errorCode = STATUS_SUCCESS;

    /* Check if a transfer is still in progress */
    DEVICE_ASSERT(false == spiState->isTransferInProgress);

    /* Disable SPI bus clock and set SPI module at reset state */
    CKGEN_DRV_SoftReset(g_spiSoftReset[instance], false);
    CKGEN_DRV_Enable(g_spiBusClock[instance], false);

    /* Disable the SPI interrupt */
    NVIC_EnableIRQ(g_spiIrqId[instance]);
    /* Clear the state pointer. */
    g_spiStatePtr[instance] = NULL;

    /* Destroy the semaphore */
    errorCode = OSIF_SemaDestroy(&(spiState->spiSemaphore));
    DEVICE_ASSERT(STATUS_SUCCESS == errorCode);

    return errorCode;
}

/*!
 * @brief Configures the SPI master mode CS timing delay options.
 *
 * @param[in] instance: SPI instance
 * @param[in] delayBetwenTransfers: Minimum delay between 2 transfers(CS_IDLE) in microseconds
 * @param[in] delaySCKtoPCS: Minimum delay between SCK and CS(CS_HOLD) in microseconds
 * @param[in] delayPCStoSCK: Minimum delay between CS and SCK(CS_SETUP) in microseconds
 * @return STATUS_SUCCESS
 */
status_t SPI_DRV_MasterSetDelay(uint32_t instance, uint32_t delayBetwenTransfers,
                                uint32_t delaySCKtoPCS, uint32_t delayPCStoSCK)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    /* Instantiate local variable of type spi_state_t and point to global state */
    const spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    uint32_t realDelayBetwenTransfers = 0U, realDelaySCKtoPCS= 0U, realDelayPCStoSCK= 0U;

    /* Disable SPI module */
    SPI_SetEnable(base, false);

    realDelayBetwenTransfers = delayBetwenTransfers * spiState->spiSrcClk / (uint32_t)SPI_MICROSECONDS;
    realDelaySCKtoPCS = delaySCKtoPCS * spiState->spiSrcClk / (uint32_t)SPI_MICROSECONDS;
    realDelayPCStoSCK = delayPCStoSCK * spiState->spiSrcClk / (uint32_t)SPI_MICROSECONDS;

    SPI_SetDelay(base, SPI_SCK_TO_PCS, (uint8_t)realDelaySCKtoPCS);
    SPI_SetDelay(base, SPI_PCS_TO_SCK, (uint8_t)realDelayPCStoSCK);
    SPI_SetDelay(base, SPI_BETWEEN_TRANSFER, (uint8_t)realDelayBetwenTransfers);

    /* Enable SPI module */
    SPI_SetEnable(base, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Configures the SPI bus physical parameters.
 *
 * @param[in] instance: SPI module instance
 * @param[in] spiConfig: Pointer to the spiConfig structure
 * @param[out] calculatedBaudRate: The calculated baud rate which will return for user
 * @return STATUS_SUCCESS The transfer has completed successfully, or
 *         STATUS_ERROR if driver is error and needs to clean error.
 */
status_t SPI_DRV_MasterConfigureBus(uint32_t instance,
                                    const spi_master_config_t *spiConfig,
                                    uint32_t *calculatedBaudRate)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(spiConfig != NULL);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    /* Instantiate local variable of type spi_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    uint32_t baudRate = 0U;
    status_t retVal = STATUS_SUCCESS;
    uint32_t inputClock = 0;

    /* bit counts per frame must be 4~32 bits */
    if ((spiConfig->bitcount < 4U) || (spiConfig->bitcount > 32U))
    {
        return STATUS_ERROR;
    }

    /* if app no set source clock, then auto get and set */
    inputClock = spiConfig->spiSrcClk;
    if (0 == inputClock)
    {
        /* Check the protocol clock frequency */
        retVal = CKGEN_DRV_GetFreq(s_spiClock[instance], &inputClock);
        DEVICE_ASSERT(STATUS_SUCCESS == retVal);
        DEVICE_ASSERT(inputClock > 0U);
        (void) retVal;
    }

    /* Configure internal state structure for SPI */
    spiState->bitsPerFrame = spiConfig->bitcount;
    spiState->spiSrcClk = inputClock;
    spiState->isPcsContinuous = spiConfig->isPcsContinuous;
    spiState->msb = spiConfig->msbFirst;
    /* Save transfer type DMA/Interrupt */
    spiState->transferType = spiConfig->transferType;
    /* Update transfer status */
    spiState->isTransferInProgress = false;
    spiState->isBlocking = false;

    /* Store DMA channel number used in transfer */
    spiState->rxDMAChannel = spiConfig->rxDMAChannel;
    spiState->txDMAChannel = spiConfig->txDMAChannel;
    /* Store callback */
    spiState->callback = spiConfig->callback;
    spiState->callbackParam = spiConfig->callbackParam;

    /* Select which CS */
    SPI_SetPcs(base, spiConfig->whichPcs);
    /* Configure the desired PCS polarity */
    SPI_SetPcsPolarityMode(base, spiConfig->whichPcs, spiConfig->pcsPolarity);

    /* Configure the baud rate */
    baudRate = SPI_SetBaudRate(base, spiConfig->bitsPerSec, spiState->spiSrcClk);
    /* If the baud rate return is "0", it means there was an error */
    if (0U == baudRate)
    {
        return STATUS_ERROR;
    }
    /* If the user wishes to know the calculated baud rate, then pass it back */
    if (calculatedBaudRate != NULL)
    {
        *calculatedBaudRate = baudRate;
    }

    /* Set bits per frame */
    SPI_SetFrameSize(base, spiState->bitsPerFrame);
    /* Data line width 1 bit, standard SPI */
    SPI_SetDataWidth(base, SPI_DATA_WIDTH_1BIT);
    /* Set CS continuous or disContinuous mode */
    SPI_SetCSOE(base, true);
    SPI_SetContinuousCS(base, spiConfig->isPcsContinuous);
    /* Set MSB or LSB mode */
    SPI_SetTxMSB(base, spiConfig->msbFirst);
    SPI_SetRxMSB(base, spiConfig->msbFirst);
    /* Set CLK phase and polarity */
    SPI_SetCPHA(base, spiConfig->clkPhase);
    SPI_SetCPOL(base, spiConfig->clkPolarity);

    /* Reset SPI status, but confifuration still valid */
    SPI_SoftwareReset(base);

    return STATUS_SUCCESS;
}

/*!
 * @brief SPI using interrupt or DMA transfer by blocking mode, which will not return
 *        untill timeout or transfer finish.
 *
 * @param[in] instance: SPI module instance
 * @param[in] sendBuffer: The pointer to the data buffer of the data to send.
 * @param[in] receiveBuffer: Pointer to the buffer where the received bytes are stored.
 * @param[in] transferByteCount: The number of bytes to send and receive which is equal to size of send or receive buffers
 * @param[in] timeout: A timeout for the transfer in milliseconds. If the transfer takes longer than
 *        this amount of time, the transfer is aborted and a STATUS_TIMEOUT error returned.
 * @return STATUS_SUCCESS The transfer was successful, or
 *         STATUS_BUSY Cannot perform transfer because a transfer is already in progress, or
 *         STATUS_TIMEOUT The transfer timed out and was aborted.
 */
status_t SPI_DRV_MasterTransferBlocking(uint32_t instance,
                                        const uint8_t *sendBuffer,
                                        uint8_t *receiveBuffer,
                                        uint16_t transferByteCount,
                                        uint32_t timeout)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    /* Instantiate local variable of type spi_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    status_t error = STATUS_SUCCESS, osifError = STATUS_SUCCESS;

    /* If the transfer count is zero, then return immediately.*/
    if (0U == transferByteCount)
    {
        return error;
    }

    /* Check if another transfer is in progress */
    if (!SPI_GetStatusFlag(base, SPI_MODULE_IDLE))
    {
        return STATUS_BUSY;
    }

    /* Dummy wait to ensure the semaphore is 0, no need to check result */
    (void)OSIF_SemaWait(&(spiState->spiSemaphore), 0U);
    spiState->isBlocking = true;

    error = SPI_MasterStartTransfer(instance, sendBuffer, receiveBuffer,
                                        transferByteCount);
    /* Start the transfer process, if it returns an error code, return this back to user */
    if (error != STATUS_SUCCESS)
    {
        /* Disable interrupt requests*/
        SPI_SetIntMode(base, SPI_INT_TX_DATA, false);
        SPI_SetIntMode(base, SPI_INT_RX_DATA, false);
        SPI_DRV_DisableTEIEInterrupts(instance);

        spiState->isBlocking = false;
        return error;
    }

    /* As this is a synchronous transfer, wait until the transfer is complete.*/
    osifError = OSIF_SemaWait(&(spiState->spiSemaphore), timeout);

    /* If a timeout occurs, stop the transfer by setting the isTransferInProgress to false and
     * disabling interrupts, then return the timeout error status.
     */
    if (STATUS_TIMEOUT == osifError)
    {
        /* Set isBlocking variable to false to avoid dummy semaphore post. */
        spiState->isBlocking = false;
        /* Complete transfer. */
        SPI_MasterCompleteTransfer(instance);
        return STATUS_TIMEOUT;
    }

    SPI_DRV_DisableTEIEInterrupts(instance);

    return error;
}

/*!
 * @brief SPI using interrupt or DMA transfer by no-blocking mode, which will
 *        return after start transfer.The user needs to check whether the transfer
 *        is complete using the SPI_DRV_MasterGetTransferStatus function.
 *
 * @param[in] instance: SPI module instance
 * @param[in] sendBuffer: The pointer to the data buffer of the data to send.
 * @param[in] receiveBuffer: Pointer to the buffer where the received bytes are stored.
 * @param[in] transferByteCount: The number of bytes to send and receive which is equal
 *            to size of send or receive buffers
 * @return STATUS_SUCCESS The transfer was successful, or
 *         STATUS_BUSY Cannot perform transfer because a transfer is already in progress, or
 *         STATUS_TIMEOUT The transfer timed out and was aborted.
 */
status_t SPI_DRV_MasterTransfer(uint32_t instance,
                                const uint8_t *sendBuffer,
                                uint8_t *receiveBuffer,
                                uint16_t transferByteCount)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    status_t errorCode = STATUS_SUCCESS;

    /* If the transfer count is zero, then return immediately.*/
    if (0U == transferByteCount)
    {
        return STATUS_SUCCESS;
    }

    /* Start the transfer process, if it returns an error code, return this back to user */
    errorCode = SPI_MasterStartTransfer(instance, sendBuffer, receiveBuffer, transferByteCount);

    return errorCode;
}

/*!
 * @brief Returns whether the previous SPI using interrupt or DMA transfer
 *        (non-blocking) transfer is completed.
 *
 * @param[in] instance: SPI module instance
 * @param[in] bytesRemained: Remaining bytes to be transferred
 * @return STATUS_BUSY, STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_DRV_MasterGetTransferStatus(uint32_t instance, uint32_t *bytesRemained)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    /* Instantiate local variable of type spi_state_t and point to global state */
    const spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];

    /* Fill in the bytes transferred.*/
    if (bytesRemained != NULL)
    {
        *bytesRemained = spiState->rxCount;
    }

    if (SPI_TRANSFER_OK == spiState->status)
    {
        /* Check SPI module is busy or not. */
        if (!SPI_GetStatusFlag(base, SPI_MODULE_IDLE))
        {
            return STATUS_BUSY;
        }

        return (status_t)(spiState->isTransferInProgress ? STATUS_BUSY : STATUS_SUCCESS);
    }
    else
    {
        return STATUS_ERROR;
    }
}

/*!
 * @brief Terminates asynchronous transfer early(no-blocking)
 *
 * @param[in] instance: SPI module instance
 * @return STATUS_SUCCESS
 */
status_t SPI_DRV_MasterAbortTransfer(uint32_t instance)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    /* Stop the running transfer. */
    SPI_MasterCompleteTransfer(instance);

    return STATUS_SUCCESS;
}

/*!
 * @brief Select the chip to communicate with.
 *
 * The main purpose of this function is to set the PCS and the appropriate polarity.
 * @param[in] instance: SPI module instance
 * @param[in] whichPcs: selected chip
 * @param[in] polarity: chip select line polarity
 * @return STATUS_SUCCESS The transfer has completed successfully, or
 *         STATUS_ERROR if driver is error and needs to clean error.
 */
status_t SPI_DRV_SetPcs(uint32_t instance, spi_which_pcs_t whichPcs, spi_signal_polarity_t polarity)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);
    DEVICE_ASSERT((uint32_t)whichPcs < 4U);

    SPI_Type *base = g_spiBase[instance];

    SPI_SetPcsPolarityMode(base, whichPcs, polarity);
    SPI_SetPcs(base, whichPcs);

    return STATUS_SUCCESS;
}

/*!
 * @brief SPI start using interrupt or DMA transfer.
 *
 * @param[in] instance: SPI module instance
 * @param[in] sendBuffer: The pointer to the data buffer of the data to send.
 * @param[in] receiveBuffer: Pointer to the buffer where the received bytes are stored.
 * @param transferByteCount: The number of bytes to send and receive which is equal to size of send or receive buffers
 * @return STATUS_SUCCESS, STATUS_BUSY or STATUS_ERROR
 */
static status_t SPI_MasterStartTransfer(uint32_t instance,
                                        const uint8_t *sendBuffer,
                                        uint8_t *receiveBuffer,
                                        uint16_t transferByteCount)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    /* Instantiate local variable of type dspi_master_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    dma_transfer_size_t dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    uint8_t frameByteSize = 1U;

    /* Get DMA transfer size and frame size by bytes */
    if (spiState->bitsPerFrame <= 8U)
    {
        frameByteSize = 1U;
        dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    }
    else if (spiState->bitsPerFrame <= 16U)
    {
        frameByteSize = 2U;
        dmaTransferSize = DMA_TRANSFER_SIZE_2B;
    }
    else
    {
        frameByteSize = 4U;
        dmaTransferSize = DMA_TRANSFER_SIZE_4B;
    }

    /* The number of transferred bytes should be divisible by frame byte size */
    if ((uint32_t)(transferByteCount % frameByteSize) != (uint32_t)0)
    {
        return STATUS_ERROR;
    }

    /* Check SPI module is busy or not. */
    if (!SPI_GetStatusFlag(base, SPI_MODULE_IDLE))
    {
        return STATUS_BUSY;
    }

    /* Update SPI status */
    spiState->status = SPI_TRANSFER_OK;

    /* Clear all interrupts sources */
    (void)SPI_ClearStatusFlag(base, SPI_ALL_STATUS);

    /* Enable fault interrupts sources */
    SPI_SetIntMode(base, SPI_INT_TRANSMIT_ERROR, true);
    if (receiveBuffer != NULL)
    {
        SPI_SetIntMode(base, SPI_INT_RECEIVE_ERROR, true);
        spiState->rxCount = transferByteCount;
    }

    if (SPI_USING_INTERRUPTS == spiState->transferType)
    {
        /* Fill out the other members of the run-time state structure. */
        spiState->txBuff = (const uint8_t *)sendBuffer;
        spiState->rxBuff = (uint8_t *)receiveBuffer;
        spiState->txCount = transferByteCount;

        /*For continuous mode an extra word must be written to negate the PCS */
        if (spiState->isPcsContinuous)
        {
            spiState->txCount++;
        }

        /* Update transfer status */
        spiState->isTransferInProgress = true;
        /* Enable RX data interrupt if RX buffer is not NULL. */
        if (spiState->rxBuff != NULL)
        {
            SPI_SetIntMode(base, SPI_INT_RX_DATA, true);
        }
        /* Enable the TX data interrupt. */
        SPI_SetIntMode(base, SPI_INT_TX_DATA, true);
    }
    else
    {
        /* Enable SPI DMA request */
        if (receiveBuffer != NULL)
        {
            SPI_SetRxDmaCmd(base, true);
        }
        SPI_SetTxDmaCmd(base, true);

        /* Configure TX DMA channel */
        if (sendBuffer != NULL)
        {
            (void)DMA_DRV_ConfigTransfer(spiState->txDMAChannel,
                                         DMA_TRANSFER_MEM2PERIPH,
                                         (uint32_t)sendBuffer,
                                         (uint32_t)(&(base->DATA)),
                                         dmaTransferSize,
                                         (uint32_t)transferByteCount);
        }
        else
        {
            (void)DMA_DRV_ConfigTransfer(spiState->txDMAChannel,
                                         DMA_TRANSFER_PERIPH2PERIPH,
                                         (uint32_t)(&(spiState->dummy)),
                                         (uint32_t)(&(base->DATA)),
                                         dmaTransferSize,
                                         (uint32_t)transferByteCount);
        }

        /* Configure RX DMA channel */
        if (receiveBuffer != NULL)
        {
            (void)DMA_DRV_ConfigTransfer(spiState->rxDMAChannel,
                                         DMA_TRANSFER_PERIPH2MEM,
                                         (uint32_t)(&(base->DATA)),
                                         (uint32_t)receiveBuffer,
                                         dmaTransferSize,
                                         (uint32_t)transferByteCount);
            (void)DMA_DRV_InstallCallback(spiState->rxDMAChannel, (SPI_MasterCompleteRX), (void*)(instance));
            /* Start RX channel */
            (void)DMA_DRV_StartChannel(spiState->rxDMAChannel);
        }

        /* If RX buffer is null the transfer is done when all bytes were sent. */
        (void)DMA_DRV_InstallCallback(spiState->txDMAChannel, (SPI_MasterCompletDMATransfer), (void*)(instance));

        /* Start TX channel */
        (void)DMA_DRV_StartChannel(spiState->txDMAChannel);

        /* Update transfer status */
        spiState->isTransferInProgress = true;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable DMA request or interrupt after complete transfer.
 * @param[in] instance: SPI module instance
 * @param none
 */
static void SPI_MasterCompleteTransfer(uint32_t instance)
{
    /* instantiate local variable of type dspi_master_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];

    /* The transfer is complete.*/
    spiState->isTransferInProgress = false;

    /* Release CS if using CS continuous mode */
    if (spiState->isPcsContinuous)
    {
        /* Set CS negative */
        SPI_DRV_MasterReleaseCS(instance);
    }

    if (SPI_USING_DMA == spiState->transferType)
    {
        /* Disable SPI DMA request */
        SPI_SetRxDmaCmd(base, false);
        SPI_SetTxDmaCmd(base, false);

        /* Stop the DMA channel */
        (void)DMA_DRV_StopChannel(spiState->rxDMAChannel);
        (void)DMA_DRV_StopChannel(spiState->txDMAChannel);
    }
    else
    {
        /* Disable (clear) interrupt requests */
        SPI_SetIntMode(base, SPI_INT_RX_DATA, false);
        SPI_SetIntMode(base, SPI_INT_TX_DATA, false);
    }

    SPI_DRV_DisableTEIEInterrupts(instance);

    if (spiState->isBlocking)
    {
        (void)OSIF_SemaPost(&(spiState->spiSemaphore));
        spiState->isBlocking = false;
    }

    /* Calling user callback function */
    if (spiState->callback != NULL)
    {
        spiState->callback(spiState, (uint32_t)SPI_EVENT_END_TRANSFER, spiState->callbackParam);
    }

}

/*!
 * @brief Finish up a transfer DMA.
 * @param[in] parameter: Maybe use SPI module instance
 * @param[in] status: The DMA channel status
 * @return none
 */
static void SPI_MasterCompletDMATransfer(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter;
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];

    if (DMA_CHN_ERROR == status)
    {
        (void)SPI_DRV_MasterAbortTransfer(instance);
        spiState->status = SPI_TRANSMIT_FAIL;
    }
    else
    {
        /* Enable tx interrupt to catch the end of the transfer. */
        spiState->txCount = 0U;
        spiState->rxCount = 0U;
        SPI_SetIntMode(base, SPI_INT_TX_DATA, true);
    }
}

/*!
 * @brief Check if errors are detected on RX channel
 * @param[in] parameter: Maybe use SPI module instance
 * @param[in] status: The DMA channel status
 * @return none
 */
static void SPI_MasterCompleteRX(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter;
    spi_state_t *spiState = g_spiStatePtr[instance];

    if (DMA_CHN_ERROR == status)
    {
        (void)SPI_DRV_MasterAbortTransfer(instance);
        spiState->status = SPI_TRANSMIT_FAIL;
    }
}

/*!
 * @brief Interrupt handler for SPI master mode, which will update the state
 *        stored in the spi_state_t structs to transfer data.This is not a
 *        public API as it is called whenever an interrupt occurs.
 * @param[in] instance: SPI module instance
 * @return none
 */
void SPI_DRV_MasterIRQHandler(uint32_t instance)
{
    /* Instantiate local variable of type dspi_master_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];

    /* If an error is detected the transfer will be aborted */
    if ((bool)SPI_GetStatusFlag(base, SPI_TRANSMIT_ERROR) && (spiState->txBuff != NULL))
    {
        (void)SPI_DRV_MasterAbortTransfer(instance);
        (void)SPI_ClearStatusFlag(base, SPI_TRANSMIT_ERROR);
        spiState->status = SPI_TRANSMIT_FAIL;
        return;
    }

    if (SPI_GetStatusFlag(base, SPI_RECEIVE_ERROR) && (spiState->rxBuff != NULL))
    {
        (void)SPI_DRV_MasterAbortTransfer(instance);
        (void)SPI_ClearStatusFlag(base, SPI_RECEIVE_ERROR);
        spiState->status = SPI_RECEIVE_FAIL;
        return;
    }

    /* RECEIVE IRQ handler: Check read buffer only if there are remaining bytes to read. */
    if (SPI_GetStatusFlag(base,SPI_RX_DATA_FLAG))
    {
        if (spiState->rxCount != 0U)
        {
            SPI_DRV_ReadRXBuffer(instance);
        }
        else
        {
            /* Dummy read */
            (void)SPI_ReadData(base);
        }
    }

    /* Transmit data */
    if (SPI_GetStatusFlag(base,SPI_TX_DATA_FLAG))
    {
        if ((spiState->txCount != 0U))
        {
            SPI_DRV_FillupTxBuffer(instance);
        }
    }

    if ((0U == spiState->txCount) || (SPI_GetRxOnly(base)))
    {
        /* Check if we're done with this transfer.*/
        if ((0U == spiState->rxCount) || (SPI_GetTxOnly(base)))
        {
            if (false == SPI_GetStatusFlag(base, SPI_MASTER_BUSY))
            {
                /* Disable TX flag. Software buffer is empty.*/
                SPI_SetIntMode(base, SPI_INT_TX_DATA, false);
                SPI_MasterCompleteTransfer(instance);
            }
        }
    }

    if (SPI_GetStatusFlag(base,SPI_DATA_MATCH) && SPI_GetIntMode(base,SPI_INT_DATA_MATCH))
    {
        (void)SPI_ClearStatusFlag(base, SPI_DATA_MATCH);
        /* Calling user callback function */
        if (spiState->callback != NULL)
        {
            spiState->callback(spiState, (uint32_t)SPI_EVENT_DATA_MATCH, spiState->callbackParam);
        }
    }
}

/*!
* @brief Release SPI Master CS to stop continuous selection of Slave
*
* @param[in] instance: SPI module instance
* @return none
*/
void SPI_DRV_MasterReleaseCS(uint32_t instance)
{
    uint32_t timeOut = 1000U;
    SPI_Type *base = g_spiBase[instance];

    if ((SPI_IsMaster(base)) && (SPI_IsCsContinuousMode(base)))
    {
        while ((true == SPI_GetStatusFlag(base, SPI_MASTER_BUSY)) && (timeOut))
        {
            timeOut--;
        }
        SPI_ReleaseCS(base);
    }
    else
    {
        /* do nothing */
    }
}

/* =============================================  EOF  ============================================== */
