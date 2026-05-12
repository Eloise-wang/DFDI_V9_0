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
 * @file dma_drv.c
 *
 * @brief This file provides dma integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "dma_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */
#define DMA_CHANNEL_STATUS_ERR_MSK  (0xFCUL) /* Status register bit2~bit7 is error status */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Array of base addresses for DMA channel. */
static DMA_ChannelType * const s_dmaChannelBase[DMA_VIRTUAL_CH_MAX] = DMA_VIRTUAL_CH_BASE_PTRS;

/* Table to save DMA ckgen interface clocks. */
static ckgen_clock_t s_dmaBusClock[DMA_INSTANCE_MAX] = DMA_CKGEN_CLOCKS;

/* Table to save DMA soft resets. */
static ckgen_softreset_t s_dmaSoftReset[DMA_INSTANCE_MAX] = DMA_SOFT_RESETS;

/* DMA global structure to maintain DMA state */
static dma_state_t *s_virtDmaState = {NULL};

/* ====================================  Functions declaration  ===================================== */
/* Clear structure */
static void DMA_ClearStructure(uint8_t *sructPtr, size_t size);
/* Clear interrupt status */
static uint32_t DMA_ClearInterruptStatus(uint8_t virtualChannel);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initializes the DMA module.
 *
 * @param[in] dmaState: The pointer to the DMA module state structure
 * @param[in] chnStateArray: Array of pointers to run-time state structures for DMA channels
 * @param[in] chnConfigArray: Array of pointers to channel initialization structures
 * @param[in] chnCount: The number of DMA channels to be initialized
 * @return STATUS_ERROR or STATUS_SUCCESS
 */
status_t DMA_DRV_Init(dma_state_t *dmaState,
                      dma_chn_state_t * const chnStateArray[],
                      const dma_channel_config_t * const chnConfigArray[],
                      uint32_t chnCount)
{
    /* Check the state pointer is valid */
    DEVICE_ASSERT(dmaState != NULL);
    /* Check the chCount is valid */
    DEVICE_ASSERT(chnCount <= DMA_VIRTUAL_CH_MAX);

    uint32_t index = 0U;
    status_t dmaStatus = STATUS_SUCCESS;
    status_t chnInitStatus = STATUS_SUCCESS;

#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    uint32_t freq = 0U;
    status_t clockManagerStatus = STATUS_SUCCESS;
#endif

    /* Check the module has not already been initialized */
    DEVICE_ASSERT(NULL == s_virtDmaState);

#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    /* Check that DMA and DMA modules are clock gated on */
    clockManagerStatus = CLOCK_SYS_GetFreq(s_dmaClockNames[index], &freq);
    DEVICE_ASSERT(STATUS_SUCCESS == clockManagerStatus);
#endif /* (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT) */

    /* Save the dma state */
    s_virtDmaState = dmaState;

    /* Enable DMA module bus clock and set module release from reset state */
    for (index = 0U; index < DMA_INSTANCE_MAX; index++)
    {
        CKGEN_DRV_Enable(s_dmaBusClock[index], true);
        CKGEN_DRV_SoftReset(s_dmaSoftReset[index], true);
    }

    /* Initialize the channels based on configuration list */
    if ((chnStateArray != NULL) && (chnConfigArray != NULL))
    {
        for (index = 0U; index < chnCount; index++)
        {
            chnInitStatus = DMA_DRV_ChannelInit(chnStateArray[index], chnConfigArray[index]);
            if (chnInitStatus != STATUS_SUCCESS)
            {
                dmaStatus = chnInitStatus;
            }
        }
    }

    return dmaStatus;
}

/*!
 * @brief De-initializes the DMA module.
 *
 * @param[in] none
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_Deinit(void)
{
    uint32_t index = 0U;
    const dma_chn_state_t *chnState = NULL;

    if (s_virtDmaState != NULL)
    {
        /* Release all dma channel. */
        for (index = 0U; index < (uint32_t)DMA_VIRTUAL_CH_MAX; index++)
        {
            /* Release all channels. */
            chnState = s_virtDmaState->virtChnState[index];
            if (chnState != NULL)
            {
                (void) DMA_DRV_ReleaseChannel(chnState->virtChn);
            }
        }
    }

    /* Disable DMA moduel bus clock and set DMA module at reset state */
    for (index = 0U; index < DMA_INSTANCE_MAX; index++)
    {
        CKGEN_DRV_SoftReset(s_dmaSoftReset[index], false);
        CKGEN_DRV_Enable(s_dmaBusClock[index], false);
    }

    s_virtDmaState = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Initializes an DMA channel.
 *
 * @param[in] dmaChannelState: Pointer to the DMA channel state structure
 * @param[in] dmaChannelConfig: User configuration structure for DMA channel
 * @return STATUS_ERROR or STATUS_SUCCESS
 */
status_t DMA_DRV_ChannelInit(dma_chn_state_t *dmaChannelState, const dma_channel_config_t *dmaChannelConfig)
{
    /* Check the state and configuration structure pointers are valid */
    DEVICE_ASSERT((dmaChannelState != NULL) && (dmaChannelConfig != NULL));
    /* Check if the module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check the channel has not already been allocated */
    DEVICE_ASSERT(NULL == s_virtDmaState->virtChnState[dmaChannelConfig->virtChnConfig]);
    /* Check if the channel defined by user in the channel configuration structure is valid */
    DEVICE_ASSERT(dmaChannelConfig->virtChnConfig < DMA_VIRTUAL_CH_MAX);

    /* Get DMA channel from virtual channel */
    uint8_t dmaChannel = dmaChannelConfig->virtChnConfig;

    /* Get virtual channel value */
    uint8_t virtualChannel = dmaChannelConfig->virtChnConfig;

    /* Get status */
    status_t retStatus = STATUS_SUCCESS;

    /* Load the DMA channel pointer */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Clear the channel state structure */
    DMA_ClearStructure((uint8_t *)dmaChannelState, sizeof(dma_chn_state_t));

    /* Set DMA channel request source and trigger function */
    retStatus = DMA_DRV_SetChannelRequestAndTrigger(virtualChannel,
                                                    (uint8_t)dmaChannelConfig->source,
                                                    dmaChannelConfig->enableTrigger);

    if (STATUS_SUCCESS == retStatus)
    {
        /* Set virtual channel state */
        s_virtDmaState->virtChnState[virtualChannel] = dmaChannelState;
        /* Set virtual channel value */
        s_virtDmaState->virtChnState[virtualChannel]->virtChn = virtualChannel;
        /* Set virtual channel status to normal */
        s_virtDmaState->virtChnState[virtualChannel]->status = DMA_CHN_NORMAL;

        /* Enable error interrupt for this channel */
        DMA_SetErrorInterrupt(dmaChannelBase, true);

        /* Set the channel priority */
        DMA_SetChannelPriority(dmaChannelBase, dmaChannelConfig->channelPriority);

        /* Install the user callback */
        retStatus = DMA_DRV_InstallCallback(dmaChannelConfig->virtChnConfig, dmaChannelConfig->callback, dmaChannelConfig->callbackParam);

        /* Enable DMA channel NVIC interrupt. */
        NVIC_EnableIRQ((IRQn_Type)((uint32_t)DMA0_CHANNEL0_IRQn + virtualChannel));
    }

    return retStatus;
}

/*!
 * @brief Registers the callback function and the parameter for DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] callback: The pointer to the callback function
 * @param[in] parameter: The pointer to the callback function's parameter
 * @return STATUS_SUCCESS.
 */
status_t DMA_DRV_InstallCallback(uint8_t virtualChannel, dma_callback_t callback, void *parameter)
{
    /* Check the channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check the channel is allocated */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    s_virtDmaState->virtChnState[virtualChannel]->callback = callback;
    s_virtDmaState->virtChnState[virtualChannel]->parameter = parameter;

    return STATUS_SUCCESS;
}

/*!
 * @brief Releases an DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_ReleaseChannel(uint8_t virtualChannel)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check the DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;
    /* Get pointer to channel state */
    dma_chn_state_t *chnState = s_virtDmaState->virtChnState[virtualChannel];

    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(chnState != NULL);

    /* Get DMA channel base address */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Stop dma channel. */
    DMA_SetChannel(dmaChannelBase, false);

    /* Clear the channel state structure. */
    DMA_ClearStructure((uint8_t *)chnState, sizeof(dma_chn_state_t));

    s_virtDmaState->virtChnState[virtualChannel] = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Clear an DMA channel status.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return DMA channel status register value before cleared
 */
static uint32_t DMA_ClearInterruptStatus(uint8_t virtualChannel)
{
    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;
    uint32_t intStatus = 0U;

    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    intStatus = DMA_ClearIntStatus(dmaChannelBase);

    return intStatus;
}

/*!
 * @brief DMA channel interrupt handler function.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return none
 */
void DMA_DRV_IRQHandler(uint8_t virtualChannel)
{
    uint32_t intStatus = 0U;
    dma_chn_state_t *chnState = s_virtDmaState->virtChnState[virtualChannel];
    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;
    /* Get DMA channel base address */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Clear interrupt status */
    intStatus = DMA_ClearInterruptStatus(virtualChannel);

    /* Check DMA channel error status */
    if (intStatus & DMA_CHANNEL_STATUS_ERR_MSK)
    {
        /* If error is source/destination bus error, hardreset DMA channel */
        if ((intStatus & DMA_CHANNEL_STATUS_SBE_Msk) \
            || (intStatus & DMA_CHANNEL_STATUS_DBE_Msk))
        {
            DMA_ChannelHardRst(dmaChannelBase);
        }
        else
        {
            /* Do-nothing */
        }

        /* Set DMA channel error status */
        chnState->status = DMA_CHN_ERROR;
    }
    else
    {
        /* Do-nothing */
    }

    /* Calling user interrupt callback function */
    if (chnState != NULL)
    {
        if (chnState->callback != NULL)
        {
            chnState->callback(chnState->parameter, chnState->status);
        }
    }
}

/*!
 * @brief Configures a data transfer with DMA.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] type: Transfer type (M->M, P->M, M->P, P->P)
 * @param[in] srcAddr: A source register address or a source memory address
 * @param[in] destAddr: A destination register address or a destination memory address
 * @param[in] transferSize: The number of bytes to be transferred on every DMA write/read
 *            Source/Dest share the same write/read size
 * @param[in] dataBufferSize: The total number of bytes to be transferred
 * @return STATUS_ERROR or STATUS_SUCCESS
 */
status_t DMA_DRV_ConfigTransfer(uint8_t virtualChannel,
                                dma_transfer_type_t type,
                                uint32_t srcAddr,
                                uint32_t destAddr,
                                dma_transfer_size_t transferSize,
                                uint32_t dataBufferSize)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    /* Check if the value passed for 'transferSize' is valid */
    DEVICE_ASSERT(DMA_DRV_ValidTransferSize(transferSize));
#endif

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];
    uint32_t srcEndAddr = 0U, desEndAddr = 0U;
    status_t retStatus = STATUS_SUCCESS;

    /* Compute the transfer offset, based on transfer size.*/
    uint16_t transferOffset = (uint16_t) (1U << ((uint16_t)transferSize));

    /* Check that source address and destination address are valid */
    DEVICE_ASSERT(0U == (srcAddr % transferOffset));
    DEVICE_ASSERT(0U == (destAddr % transferOffset));

    /* The number of bytes to be transferred (buffer size) must
       be a multiple of the source read/destination write size
     */
    if ((dataBufferSize % transferOffset) != 0U)
    {
        retStatus = STATUS_ERROR;
    }

    if (STATUS_SUCCESS == retStatus)
    {
        /* Compute the end address */
        srcEndAddr = srcAddr + dataBufferSize;
        desEndAddr = destAddr + dataBufferSize;

        /* Configure source and destination addresses */
        DMA_SetSrcStartAddr(dmaChannelBase, srcAddr);
        DMA_SetSrcEndAddr(dmaChannelBase, srcEndAddr);
        DMA_SetDestStartAddr(dmaChannelBase, destAddr);
        DMA_SetDestEndAddr(dmaChannelBase, desEndAddr);

        /* Set transfer size (1, 2 or 4 bytes) */
        DMA_SetSrcTransferSize(dmaChannelBase, transferSize);
        DMA_SetDestTransferSize(dmaChannelBase, transferSize);

        /* Configure source/destination offset address. */
        switch (type)
        {
        case DMA_TRANSFER_PERIPH2MEM:
            DMA_SetSrcAddrOffSet(dmaChannelBase, 0U);
            DMA_SetDestAddrOffSet(dmaChannelBase, transferOffset);
            DMA_SetFIFOFastFunction(dmaChannelBase, false);
            break;

        case DMA_TRANSFER_MEM2PERIPH:
            DMA_SetSrcAddrOffSet(dmaChannelBase, transferOffset);
            DMA_SetDestAddrOffSet(dmaChannelBase, 0U);
            DMA_SetFIFOFastFunction(dmaChannelBase, false);
            break;

        case DMA_TRANSFER_MEM2MEM:
            DMA_SetSrcAddrOffSet(dmaChannelBase, transferOffset);
            DMA_SetDestAddrOffSet(dmaChannelBase, transferOffset);
            DMA_SetFIFOFastFunction(dmaChannelBase, true);
            break;

        case DMA_TRANSFER_PERIPH2PERIPH:
            DMA_SetSrcAddrOffSet(dmaChannelBase, 0U);
            DMA_SetDestAddrOffSet(dmaChannelBase, 0U);
            DMA_SetFIFOFastFunction(dmaChannelBase, false);
            break;

        default:
            /* This should never be reached - all the possible values have been handled. */
            break;
        }

        /* Set the total number of bytes to be transfered */
        DMA_SetTranferLength(dmaChannelBase, dataBufferSize);

        /* Enable transfer finifsh interrupt */
        DMA_SetFinishInterrupt(dmaChannelBase, true);

        /* Set virtual channel status to normal */
        s_virtDmaState->virtChnState[virtualChannel]->status = DMA_CHN_NORMAL;
    }

    return retStatus;
}

/*!
 * @brief Set DMA channel circular mode.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] enable: enable/disable DMA channel circular mode
 * @return none
 */
void DMA_DRV_SetCircularMode(uint8_t virtualChannel, bool enable)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;

    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    DMA_SetCircularMode(dmaChannelBase, enable);
}

/*!
 * @brief Starts an DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_StartChannel(uint8_t virtualChannel)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;

    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Enable current DMA channel */
    DMA_SetChannel(dmaChannelBase, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Stops the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_StopChannel(uint8_t virtualChannel)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;

    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Disable current DMA channel */
    DMA_SetChannel(dmaChannelBase, false);

    return STATUS_SUCCESS;
}

/*!
 * @brief Set DMA channel transfer length by bytes.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] bytesLength: DMA channel transfer length(0~32767 byte, bit 15 should be 0)
 * @return none
 */
void DMA_DRV_SetTranferLength(uint8_t virtualChannel, uint16_t bytesLength)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;

    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    DMA_SetTranferLength(dmaChannelBase, bytesLength);
}

/*!
 * @brief Get the remaining bytes to be transferred by DMA.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return The remaining bytes to be transferred by DMA
 */
uint32_t DMA_DRV_GetRemainingBytes(uint8_t virtualChannel)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;

    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    uint32_t remainingBtes = 0U;
    uint32_t totalBytes = DMA_GetTranferLength(dmaChannelBase);
    uint32_t transferredBytes = DMA_GetTransferedBytes(dmaChannelBase);

    remainingBtes = totalBytes - transferredBytes;

    return remainingBtes;
}

/*!
 * @brief Configures the DMA request and trigger function for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] request: DMA request source.The DMA request sources(dma_request_source_t)
 *                     are defined in the file dma_drv.h
 * @param[in] enableTrigger: Enable or disable DMA channel trigger from CTU module
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_SetChannelRequestAndTrigger(uint8_t virtualChannel, uint8_t request, bool enableTrigger)
{
    /* Check the virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < (uint32_t)DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);

    /* Save DMA channel */
    uint8_t dmaChannel =virtualChannel;

    /* Retrieve the appropriate DMA instance */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Set DMA channel source request and trigger */
    DMA_SetChannelSource(dmaChannelBase, request);
    DMA_SetChannelTrigger(dmaChannelBase, enableTrigger);

    return STATUS_SUCCESS;
}

/*!
 * @brief Configures the source address for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] startAddr: The pointer to the source start address
 * @param[in] endAddr: The pointer to the source end address
 * @return none
 */
void DMA_DRV_SetSrcAddr(uint8_t virtualChannel, uint32_t startAddr, uint32_t endAddr)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel */
    uint8_t dmaChannel = virtualChannel;
    /* Get DMA channel base pointer */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Set DMA channel source address */
    DMA_SetSrcStartAddr(dmaChannelBase, startAddr);
    DMA_SetSrcEndAddr(dmaChannelBase, endAddr);
}

/*!
 * @brief Configures the source address unsigned offset for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] offset: Unsigned-offset for source address
 * @return none
 */
void DMA_DRV_SetSrcOffset(uint8_t virtualChannel, uint16_t offset)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;

    /* Get DMA channel base pointer */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Set DMA channel source address unsigned offset */
    DMA_SetSrcAddrOffSet(dmaChannelBase, offset);
}

/*!
 * @brief Configures the destination address for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] startAddr: The pointer to the destination start address
 * @param[in] endAddr: The pointer to the destination end address
 * @return none
 */
void DMA_DRV_SetDestAddr(uint8_t virtualChannel, uint32_t startAddr, uint32_t endAddr)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel base pointer */
    uint8_t dmaChannel = virtualChannel;
    /* Get DMA channel base pointer */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Set DMA channel destination address */
    DMA_SetDestStartAddr(dmaChannelBase, startAddr);
    DMA_SetDestEndAddr(dmaChannelBase, endAddr);
}

/*!
 * @brief Configures the destination address unsigned offset for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] offset: Unsigned-offset for destination address
 * @return none
 */
void DMA_DRV_SetDestOffset(uint8_t virtualChannel, uint16_t offset)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;
    /* Get DMA channel base pointer */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Set DMA channel destination address unsigned offset */
    DMA_SetDestAddrOffSet(dmaChannelBase, offset);
}

/*!
 * @brief Disables/Enables the channel interrupt.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] interrupt: Half finish/finish/error interrupt
 * @param[in] enable: Enable (true)/Disable (false) interrupts for the current channel
 * @return none
 */
void DMA_DRV_ConfigureInterrupt(uint8_t virtualChannel, dma_channel_interrupt_t intSrc, bool enable)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    /* Get DMA channel from virtual channel*/
    uint8_t dmaChannel = virtualChannel;
    /* Get DMA channel base pointer */
    DMA_ChannelType *dmaChannelBase = s_dmaChannelBase[dmaChannel];

    /* Disables/Enables the channel interrupt. */
    switch (intSrc)
    {
    case DMA_CHN_ERR_INT:
        /* Enable channel error interrupt */
        DMA_SetErrorInterrupt(dmaChannelBase, enable);
        break;

    case DMA_CHN_FINISH_INT:
        /* Enable channel transfer finish interrupt */
        DMA_SetFinishInterrupt(dmaChannelBase, enable);
        break;

    default:
        /* This branch should never be reached if driver API is used properly */
        break;
    }
}

/*!
 * @brief Gets the DMA channel status.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return Channel status
 */
dma_chn_status_t DMA_DRV_GetChannelStatus(uint8_t virtualChannel)
{
    /* Check that virtual channel number is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);
    /* Check that DMA module is initialized */
    DEVICE_ASSERT(s_virtDmaState != NULL);
    /* Check that virtual channel is initialized */
    DEVICE_ASSERT(s_virtDmaState->virtChnState[virtualChannel] != NULL);

    return s_virtDmaState->virtChnState[virtualChannel]->status;
}

/*!
 * @brief Gets the DMA channel base pointer.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return DMA channel base pointer
 */
DMA_ChannelType *DMA_DRV_GetDmaRegBaseAddr(uint32_t virtualChannel)
{
    /* Check that instance is valid */
    DEVICE_ASSERT(virtualChannel < DMA_VIRTUAL_CH_MAX);

    return s_dmaChannelBase[virtualChannel];
}

/*!
 * @brief Clear the structure.
 *
 * @param[in] sructPtr: The pointer to the structure
 * @param[in] size: Bytes size of the structures
 * @return none
 */
static void DMA_ClearStructure(uint8_t *sructPtr, size_t size)
{
    while (size > 0U)
    {
        *sructPtr = 0;
        sructPtr ++;
        size --;
    }
}

/* =============================================  EOF  ============================================== */
