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
 * @file spi_slave_drv.c
 *
 * @brief This file provides spi slave integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "spi_slave_drv.h"
#include "ckgen_drv.h"
#include "dma_drv.h"
#include "spm_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/* Callback for DMA transfer done.*/
static void SPI_DRV_SlaveCompleteDMATransfer(void* parameter, dma_chn_status_t status);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get default configuration for SPI slave.
 *
 * @param[out] spiConfig: Pointer to configuration structure which is filled
 *             with default configuration
 * @return none
 */
void SPI_DRV_SlaveGetDefaultConfig(spi_slave_config_t *spiConfig)
{
    DEVICE_ASSERT(spiConfig != NULL);

    spiConfig->whichPcs         = SPI_PCS0;
    spiConfig->pcsPolarity      = SPI_ACTIVE_LOW;
    spiConfig->bitcount         = 8U;
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
 * @brief Initializes SPI.
 *
 * @param[in] instance: SPI instance
 * @param[in] spiState: The pointer to the SPI slave driver spiState structure
 * @param[in] spiConfig: The data structure containing information about a device on the SPI bus
 * @return An error code or STATUS_SUCCESS.
 */
status_t SPI_DRV_SlaveInit(uint32_t instance, spi_state_t *spiState, const spi_slave_config_t *slaveConfig)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(spiState != NULL);
    DEVICE_ASSERT(slaveConfig != NULL);

    SPI_Type *base = g_spiBase[instance];
    status_t errorCode = STATUS_SUCCESS;

    spiState->msb = slaveConfig->msbFirst;
    spiState->bitsPerFrame = slaveConfig->bitcount;
    spiState->transferType = slaveConfig->transferType;
    spiState->isBlocking = false;
    /* Store DMA channels numbers used for DMA transfer */
    spiState->rxDMAChannel = slaveConfig->rxDMAChannel;
    spiState->txDMAChannel = slaveConfig->txDMAChannel;
    /* Store callback */
    spiState->callback = slaveConfig->callback;
    spiState->callbackParam = slaveConfig->callbackParam;
    /* Update isTransferInProgress flag */
    spiState->isTransferInProgress = false;

    /* Initialize the semaphore */
    errorCode = OSIF_SemaCreate(&(spiState->spiSemaphore), 0U);
    DEVICE_ASSERT(STATUS_SUCCESS == errorCode);
    g_spiStatePtr[instance] = spiState;

    /* Enable SPI bus clock and release module from reset spiState */
    CKGEN_DRV_Enable(g_spiBusClock[instance], true);
    CKGEN_DRV_SoftReset(g_spiSoftReset[instance], true);

    /* Configure spi to slave mode */
    SPI_SetMasterSlaveMode(base, SPI_SLAVE);
    /* Set Pin settings */
    SPI_SetPinConfigMode(base, SPI_SOUT_MOSI_SIN_MISO);
    /* Select which CS */
    SPI_SetPcs(base, slaveConfig->whichPcs);
    /* Set polarity */
    SPI_SetPcsPolarityMode(base, slaveConfig->whichPcs, slaveConfig->pcsPolarity);

    /* Set bits per frame */
    SPI_SetFrameSize(base, spiState->bitsPerFrame);
    /* Data line width 1 bit, standard SPI */
    SPI_SetDataWidth(base, SPI_DATA_WIDTH_1BIT);
    /* Set CS continuous or disContinuous mode */
    SPI_SetCSOE(base, true);
    // SPI_SetContinuousCS(base, spiConfig->isPcsContinuous);
    /* Set MSB or LSB mode */
    SPI_SetTxMSB(base, slaveConfig->msbFirst);
    SPI_SetRxMSB(base, slaveConfig->msbFirst);
    /* Set CLK phase and polarity */
    SPI_SetCPHA(base, slaveConfig->clkPhase);
    SPI_SetCPOL(base, slaveConfig->clkPolarity);

    /* Reset SPI status, but confifuration still valid */
    SPI_SoftwareReset(base);

    /* Enable SPI interrupt. */
    NVIC_EnableIRQ(g_spiIrqId[instance]);

    /* Enable SPI module */
    SPI_SetEnable(base, true);

    return errorCode;
}

/*!
 * @brief De-initializes SPI.
 *
 * @param[in] instance: SPI instance
 * @return STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_DRV_SlaveDeinit(uint32_t instance)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    /* Instantiate local variable of type spi_slave_state_t and point to global spiState */
    const spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];
    status_t errorCode = STATUS_SUCCESS;

    /* Check if a transfer is still in progress */
    DEVICE_ASSERT(false == spiState->isTransferInProgress);
    /* Destroy the semaphore */
    errorCode = OSIF_SemaDestroy(&(spiState->spiSemaphore));
    DEVICE_ASSERT(STATUS_SUCCESS == errorCode);

    /* Disable SPI bus clock and set SPI module at reset spiState */
    CKGEN_DRV_SoftReset(g_spiSoftReset[instance], false);
    CKGEN_DRV_Enable(g_spiBusClock[instance], false);

    /* Disable the interrupt*/
    NVIC_EnableIRQ(g_spiIrqId[instance]);
    /* Clear the spiState pointer. */
    g_spiStatePtr[instance] = NULL;

    return errorCode;
}

/*!
 * @brief SPI using interrupt or DMA transfer by blocking mode, which will not return
 *        untill timeout or transfer finish.
 *
 * @param[in] instance: SPI module instance
 * @param[in] sendBuffer: The pointer to the data buffer of the data to send.
 * @param[in] receiveBuffer: Pointer to the buffer where the received bytes are stored.
 * @param[in] transferByteCount: The number of bytes to send and receive which is equal to size
 *            of send or receive buffers
 * @param[in] timeout: A timeout for the transfer in milliseconds. If the transfer takes longer than
 *            this amount of time, the transfer is aborted and a STATUS_TIMEOUT error returned.
 * @return STATUS_SUCCESS The transfer was successful, or
 *         STATUS_BUSY Cannot perform transfer because a transfer is already in progress, or
 *         STATUS_TIMEOUT The transfer timed out and was aborted.
 */
status_t SPI_DRV_SlaveTransferBlocking(uint32_t instance,
                                       const uint8_t *sendBuffer,
                                       uint8_t *receiveBuffer,
                                       uint16_t transferByteCount,
                                       uint32_t timeout)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];
    const SPI_Type *base = g_spiBase[instance];
    status_t errorCode = STATUS_SUCCESS;

    /* Check if another transfer is in progress */
    if (!SPI_GetStatusFlag(base, SPI_MODULE_IDLE))
    {
        return STATUS_BUSY;
    }

    /* Dummy wait to ensure the semaphore is 0, no need to check result */
    (void)OSIF_SemaWait(&(spiState->spiSemaphore), 0U);
    spiState->isBlocking = true;

    errorCode = SPI_DRV_SlaveTransfer(instance, sendBuffer, receiveBuffer, transferByteCount);
    if (errorCode != STATUS_SUCCESS)
    {
        spiState->isBlocking = false;
        SPI_DRV_DisableTEIEInterrupts(instance);
        return errorCode;
    }

    /* As this is a synchronous transfer, wait until the transfer is complete.*/
    errorCode = OSIF_SemaWait(&(spiState->spiSemaphore), timeout);
    if (STATUS_TIMEOUT == errorCode)
    {
        /* Set isBlocking variable to false to avoid dummy semaphore post. */
        spiState->isBlocking = false;
        /* Complete transfer. */
        (void)SPI_DRV_SlaveAbortTransfer(instance);
        return STATUS_TIMEOUT;
    }

    SPI_DRV_DisableTEIEInterrupts(instance);

    return STATUS_SUCCESS;
}

/*!
 * @brief SPI using interrupt or DMA transfer by no-blocking mode, which will
 *        return after start transfer.The user needs to check whether the transfer
 *        is complete using the SPI_DRV_SlaveGetTransferStatus function.
 *
 * @param[in] instance: SPI module instance
 * @param[in] sendBuffer: The pointer to the data buffer of the data to send.
 * @param[in] receiveBuffer: Pointer to the buffer where the received bytes are stored.
 * @param[in] transferByteCount: The number of bytes to send and receive which is equal
 *            to size of send or receive buffers
 * @return STATUS_SUCCESS The transfer was successful, or
 *         STATUS_BUSY Cannot perform transfer because a transfer is already in progress
 */
status_t SPI_DRV_SlaveTransfer(uint32_t instance,
                               const uint8_t *sendBuffer,
                               uint8_t *receiveBuffer,
                               uint16_t transferByteCount)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);
    DEVICE_ASSERT(!((NULL == sendBuffer) && (NULL == receiveBuffer)));

    SPI_Type *base = g_spiBase[instance];
    spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];
    dma_transfer_size_t dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    const uint8_t *buffer = NULL;
    uint8_t frameByteSize = 1U; /* 1/2/4 bytes */
    spiState->dummy = 0xFFU;

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
    if ((uint32_t)(transferByteCount % frameByteSize) != 0U)
    {
        return STATUS_ERROR;
    }

    /* Check if SPI module isn't busy */
    if (spiState->isTransferInProgress)
    {
        return STATUS_BUSY;
    }

    /* Initialize the status of the current transfer */
    spiState->status = SPI_TRANSFER_OK;

    /* Clear all interrupts sources */
    (void)SPI_ClearStatusFlag(base, SPI_ALL_STATUS);
    /* Enable fault interrupts sources */
    SPI_SetIntMode(base, SPI_INT_TRANSMIT_ERROR, true);
    SPI_SetIntMode(base, SPI_INT_RECEIVE_ERROR, true);
    /* Fill out the other members of the run-time spiState structure. */
    spiState->txBuff = (const uint8_t *)sendBuffer;
    spiState->rxBuff = (uint8_t *)receiveBuffer;
    if (SPI_USING_INTERRUPTS == spiState->transferType)
    {
        if (spiState->txBuff != NULL)
        {
            spiState->txCount = transferByteCount;
        }
        else
        {
            spiState->txCount = 0;
        }

        if (spiState->rxBuff != NULL)
        {
            spiState->rxCount = transferByteCount;
        }
        else
        {
            spiState->rxCount = 0;
        }

        spiState->isPcsContinuous = false;
        spiState->isTransferInProgress = true;
        /* Enable interrupts for RX and TX only if it's necessary */
        if (spiState->txBuff != NULL)
        {
            SPI_SetIntMode(base, SPI_INT_TX_DATA, true);
        }
        if (spiState->rxBuff != NULL)
        {
            SPI_SetIntMode(base, SPI_INT_RX_DATA, true);
        }
    }
    else
    {
        /* Enable SPI DMA requests */
        SPI_SetRxDmaCmd(base, true);
        SPI_SetTxDmaCmd(base, true);

        if (receiveBuffer != NULL)
        {
            spiState->rxCount = transferByteCount;
            buffer = receiveBuffer;
        }
        else
        {
            spiState->rxCount = 0U;
            /* if there is no data to receive, use dummy data as destination for DMA transfer */
            buffer = (uint8_t *)(&(spiState->dummy));
        }
        (void)DMA_DRV_ConfigTransfer(spiState->rxDMAChannel,
                                     DMA_TRANSFER_PERIPH2MEM,
                                     (uint32_t)(&(base->DATA)),
                                     (uint32_t)buffer,
                                     dmaTransferSize,
                                     (uint32_t)transferByteCount);
        if (NULL == receiveBuffer)
        {
        	/* if there is no data to receive, don't increment destination offset */
        	DMA_DRV_SetDestOffset(spiState->rxDMAChannel, 0);
        }

        if (sendBuffer != NULL)
        {
            spiState->txCount = transferByteCount;
            buffer = sendBuffer;
        }
        else
        {
            spiState->txCount = 0U;
            /* if there is no data to send, use dummy data as source for DMA transfer */
            buffer = (uint8_t *)(&(spiState->dummy));
        }
        (void)DMA_DRV_ConfigTransfer(spiState->txDMAChannel,
                                     DMA_TRANSFER_MEM2PERIPH,
                                     (uint32_t)buffer,
                                     (uint32_t)(&(base->DATA)),
                                     dmaTransferSize,
                                     (uint32_t)transferByteCount);
        if (NULL == sendBuffer)
        {
             /* if there is no data to transmit, don't increment source offset */
             DMA_DRV_SetSrcOffset(spiState->txDMAChannel, 0);
        }

        (void)DMA_DRV_InstallCallback(spiState->rxDMAChannel, (SPI_DRV_SlaveCompleteDMATransfer), (void*)(instance));

        spiState->isTransferInProgress = true;

        /* Start RX channel */
        (void)DMA_DRV_StartChannel(spiState->rxDMAChannel);
        /* Start TX channel */
        (void)DMA_DRV_StartChannel(spiState->txDMAChannel);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Interrupt handler for SPI slave mode, which will update the state
 *        stored in the spi_state_t structs to transfer data.This is not a
 *        public API as it is called whenever an interrupt occurs.
 * @param[in] instance: SPI module instance
 * @return none
 */
void SPI_DRV_SlaveIRQHandler(uint32_t instance)
{
    SPI_Type *base = g_spiBase[instance];
    spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];

    /* If an error is detected the transfer will be aborted */
    if ((bool)SPI_GetStatusFlag(base, SPI_TRANSMIT_ERROR) && (spiState->txBuff != NULL))
    {
        (void)SPI_DRV_SlaveAbortTransfer(instance);
        spiState->status = SPI_TRANSMIT_FAIL;
        return;
    }
    if (SPI_GetStatusFlag(base, SPI_RECEIVE_ERROR) && (spiState->rxBuff != NULL))
    {
        (void)SPI_DRV_SlaveAbortTransfer(instance);
        spiState->status = SPI_RECEIVE_FAIL;
        return;
    }

    /* Receive data */
    if (SPI_GetStatusFlag(base, SPI_RX_DATA_FLAG))
    {
        if ((spiState->rxCount != (uint8_t)0))
        {
            if (POWER_MODE_STOP2 == SPM_DRV_GetPowerControlMode())
            {
                (void)SPI_ReadData(base);
                (void)SPM_DRV_SetPowerMode(POWER_MODE_RUN);
            }
            else
            {
                SPI_DRV_ReadRXBuffer(instance);
            }
        }
        else
        {
            /* Dummy read */
            (void)SPI_ReadData(base);
        }
    }
    /* Transmit data */
    if (SPI_GetStatusFlag(base, SPI_TX_DATA_FLAG))
    {
        if ((spiState->txCount != 0U))
        {
            SPI_DRV_FillupTxBuffer(instance);
        }
    }

    /* If all bytes are sent disable TX interrupt */
    if (0U == spiState->txCount)
    {
        SPI_SetIntMode(base, SPI_INT_TX_DATA, false);
    }
    /* If all bytes are received disable RX interrupt */
    if (0U == spiState->rxCount)
    {
        SPI_SetIntMode(base, SPI_INT_RX_DATA, false);
    }
    if ((0U == spiState->rxCount) || (SPI_GetTxOnly(base)))
    {
        if ((0U == spiState->txCount) || (SPI_GetRxOnly(base)))
        {
            /* Disable fault interrupts sources */
            SPI_SetIntMode(base, SPI_INT_TRANSMIT_ERROR, false);
            SPI_SetIntMode(base, SPI_INT_RECEIVE_ERROR, false);

            /* Call the callback if it is defined */
            if (spiState->callback != NULL)
            {
                spiState->callback(spiState, (uint32_t)SPI_EVENT_END_TRANSFER, spiState->callbackParam);
            }

            /* If the transfer is blocking post the semaphore */
            if (spiState->isBlocking)
            {
                (void)OSIF_SemaPost(&(spiState->spiSemaphore));
                spiState->isBlocking = false;
            }

            /* Update internal spiState of the transfer */
            spiState->isTransferInProgress = false;
        }
    }

    if (SPI_GetStatusFlag(base, SPI_DATA_MATCH) && SPI_GetIntMode(base, SPI_INT_DATA_MATCH))
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
 * @brief Terminates asynchronous transfer early(no-blocking)
 *
 * @param[in] instance: SPI module instance
 * @return STATUS_SUCCESS
 */
status_t SPI_DRV_SlaveAbortTransfer(uint32_t instance)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    SPI_Type *base = g_spiBase[instance];
    spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];

    if (SPI_USING_INTERRUPTS == spiState->transferType)
    {
        /* Disable interrupts */
        SPI_SetIntMode(base, SPI_INT_TX_DATA, false);
        SPI_SetIntMode(base, SPI_INT_RX_DATA, false);
    }
    else
    {
        /* Disable SPI DMA request */
        SPI_SetRxDmaCmd(base, false);
        SPI_SetTxDmaCmd(base, false);

        /* Stop the DMA channel */
        (void)DMA_DRV_StopChannel(spiState->rxDMAChannel);
        (void)DMA_DRV_StopChannel(spiState->txDMAChannel);
    }

    SPI_DRV_DisableTEIEInterrupts(instance);

    spiState->isTransferInProgress = false;
    if (spiState->isBlocking)
    {
        (void)OSIF_SemaPost(&(spiState->spiSemaphore));
        spiState->isBlocking = false;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Returns whether the previous SPI using interrupt or DMA transfer
 *        (non-blocking) transfer is completed.
 *
 * @param[in] instance: SPI module instance
 * @param[in] bytesRemained: Remaining bytes to be transferred
 * @return STATUS_BUSY, STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_DRV_SlaveGetTransferStatus(uint32_t instance, uint32_t * bytesRemained)
{
    DEVICE_ASSERT(instance < SPI_INSTANCE_MAX);
    DEVICE_ASSERT(g_spiStatePtr[instance] != NULL);

    const spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];

    /* Fill in the bytes transferred.*/
    if (bytesRemained != NULL)
    {
        *bytesRemained = spiState->txCount;
    }
    if (SPI_TRANSFER_OK == spiState->status)
    {
        return (status_t)(spiState->isTransferInProgress ? STATUS_BUSY : STATUS_SUCCESS);
    }
    else
    {
        return STATUS_ERROR;
    }
}

/*!
 * @brief Finish up a transfer DMA.
 * @param[in] parameter: Maybe use SPI module instance
 * @param[in] status: The DMA channel status
 * @return none
 */
static void SPI_DRV_SlaveCompleteDMATransfer(void* parameter, dma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter;
    spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];

    (void)status;
    (void)SPI_DRV_SlaveAbortTransfer(instance);

    /* Check RX and TX DMA channels. */
    if (DMA_DRV_GetChannelStatus(spiState->txDMAChannel) != DMA_CHN_NORMAL)
    {
        spiState->status = SPI_TRANSMIT_FAIL;
    }
    if (DMA_DRV_GetChannelStatus(spiState->rxDMAChannel) != DMA_CHN_NORMAL)
    {
        spiState->status = SPI_RECEIVE_FAIL;
    }

    if (spiState->callback != NULL)
    {
        spiState->callback(spiState, (uint32_t)SPI_EVENT_END_TRANSFER, spiState->callbackParam);
    }
}

/* =============================================  EOF  ============================================== */
