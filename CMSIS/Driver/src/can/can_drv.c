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
 * @file can_drv.c
 *
 * @brief This file provides CAN driver interface functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "can_hw.h"

/* ============================================  Define  ============================================ */
#define CAN_IRQ_FLAG_MSK              (0x00D5FF01UL)    /* All interrupt flag mask macro */
#define CAN_ECC_IRQ_FLAG_MSK          (0x00140000UL)    /* Memory ECC interrupt flag mask macro */
#define CAN_ECC_IRQ_FLAG_OFFSET       (8UL)             /* CAN memory ECC interrupt flag offset */
#define CAN_ECC_INIT_TIMEOUT          (100UL)           /* CAN memory ECC initialization timeout */

/* ===========================================  Typedef  ============================================ */
/* CAN state struct */
typedef struct
{
    uint32_t                  interruptFlag;            /* CAN interrupt flag */
    bool                      initState;                /* CAN initialization state */
    bool                      interruptEn;              /* CAN interrupt enable */
    bool                      timeStampEn;              /* Time stamp enable */
    bool                      dmaRecvEn;                /* DMA receive enable */
    bool                      memEccEn;                 /* CAN memory ECC enable */
    semaphore_t               txCompleted;              /* Transmit completed */
    semaphore_t               rxCompleted;              /* Receive completed */
    can_transmit_sec_amount_t tsAmount;                 /* Transmit secondary all frames or one frame */
    can_callback_t            callback;                 /* CAN callback function pointer */
    can_callback_t            wakeupCallback;           /* CAN wakeup callback function pointer */
} can_state_t;

/* ==========================================  Variables  =========================================== */
/* Table of CAN base address */
static CAN_Type *const s_canBase[CAN_INSTANCE_MAX] = CAN_BASE_PTRS;

/* Table of CAN IRQs */
static IRQn_Type s_canIrq[CAN_INSTANCE_MAX] = CAN_IRQS;

/* Table of CAN wakeup IRQs */
static IRQn_Type s_canWakeupIrq[CAN_INSTANCE_MAX] = CAN_WAKEUP_IRQS;

/* Table of CAN ckgen interface clocks */
static ckgen_clock_t s_canCkgenClock[CAN_INSTANCE_MAX] = CAN_CKGEN_CLOCKS;

/* Table of CAN soft resets */
static ckgen_softreset_t s_canSoftReset[CAN_INSTANCE_MAX] = CAN_SOFT_RESETS;

/* Table of CAN state */
static can_state_t s_canState[CAN_INSTANCE_MAX] = {0};

/* ====================================  Functions declaration  ===================================== */
extern void CKGEN_SetCANTimeStampClkDiv(uint8_t instance, can_time_stamp_clk_div_t divider);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get CAN module base address.
 *
 * @param[in] instance: CAN module instance
 * @return CAN module base address
 */
CAN_Type* CAN_DRV_GetBase(uint8_t instance)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    return (s_canBase[instance]);
}

/*!
 * @brief CAN interrupt handler function.
 *
 * @param[in] instance: CAN module instance
 * @return none
 */
void CAN_IRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    CAN_Type *base = s_canBase[instance];
    can_state_t *state = &s_canState[instance];
    uint32_t flag = CAN_GetCtrl1(base);

    /* Read and clear flag */
    CAN_SetCtrl1(base, flag);
    state->interruptFlag = flag & CAN_IRQ_FLAG_MSK;

    /* Read and clear memory ECC error flag */
    if (state->memEccEn)
    {
        flag = CAN_GetVerMem(base);
        CAN_SetVerMem(base, flag);
        state->interruptFlag |= (flag & CAN_ECC_IRQ_FLAG_MSK) << CAN_ECC_IRQ_FLAG_OFFSET;
    }

    if (state->interruptFlag & (CAN_CTRL1_TPIF_Msk | CAN_CTRL1_TSIF_Msk))
    {
        (void)OSIF_SemaPost(&state->txCompleted);
    }
    else if (state->interruptFlag & CAN_CTRL1_RIF_Msk)
    {
        (void)OSIF_SemaPost(&state->rxCompleted);
    }

    /* Call back function */
    if (state->callback != NULL)
    {
        state->callback(instance, state->interruptFlag, CAN_GetKoer(base));
    }
}

/*!
 * @brief CAN wakeup interrupt handler function.
 *
 * @param[in] instance: CAN module instance
 * @return none
 */
void CAN_WakeupIRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    can_state_t *state = &s_canState[instance];

    /* Wakeup call back function */
    if (state->wakeupCallback != NULL)
    {
        state->wakeupCallback(instance, (uint32_t)CAN_EVENT_WAKEUP, 0U);
    }
}

/*!
 * @brief Set the CAN bitrate.
 *
 * @param[in] instance: CAN module instance
 * @param[in] bitrate: point to bitrate configuration
 * @return none
 */
void CAN_DRV_SetBitrate(uint8_t instance, const can_time_segment_t *bitrate)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(bitrate != NULL);

    CAN_Type *base = s_canBase[instance];
    bool reset = false;

    /* Check the RESET is 1 first */
    reset = CAN_GetResetState(base);
    if (false == reset)
    {
        CAN_SetReset(base, true);
    }

    CAN_SetSlowBitrate(base, bitrate);

    if (false == reset)
    {
        CAN_SetReset(base, false);
    }
}

/*!
 * @brief Get the CAN bitrate.
 *
 * @param[in] instance: CAN module instance
 * @param[out] bitrate: point to bitrate configuration
 * @return none
 */
void CAN_DRV_GetBitrate(uint8_t instance, can_time_segment_t *bitrate)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(bitrate != NULL);

    CAN_GetSlowBitrate(s_canBase[instance], bitrate);
}

/*!
 * @brief Set the CAN FD data bitrate.
 *
 * @param[in] instance: CAN module instance
 * @param[in] bitrate: point to bitrate configuration
 * @return none
 */
void CAN_DRV_SetBitrateFD(uint8_t instance, const can_time_segment_t *bitrate)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(bitrate != NULL);

    CAN_Type *base = s_canBase[instance];
    bool reset = false;

    /* Check the RESET is 1 first */
    reset = CAN_GetResetState(base);
    if (false == reset)
    {
        CAN_SetReset(base, true);
    }

    CAN_SetFastBitrate(base, bitrate);

    if (false == reset)
    {
        CAN_SetReset(base, false);
    }
}

/*!
 * @brief Get the CAN FD data bitrate.
 *
 * @param[in] instance: CAN module instance
 * @param[out] bitrate: pointer to bitrate configuration
 * @return none
 */
void CAN_DRV_GetBitrateFD(uint8_t instance, can_time_segment_t *bitrate)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(bitrate != NULL);

    CAN_GetFastBitrate(s_canBase[instance], bitrate);
}

/*!
 * @brief Set the CAN FD TDC and SSP offset.
 *
 * @param[in] instance: CAN module instance
 * @param[in] enable: TDC enable
 * @param[in] offset: SSP offset
 * @return none
 */
void CAN_DRV_SetTDCOffset(uint8_t instance, bool enable, uint8_t offset)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    CAN_Type *base = s_canBase[instance];
    bool reset = false;

    /* Check the RESET is 1 first */
    reset = CAN_GetResetState(base);
    if (false == reset)
    {
        CAN_SetReset(base, true);
    }

    CAN_SetTdc(base, enable);
    CAN_SetSspOffset(base, offset);

    if (false == reset)
    {
        CAN_SetReset(base, false);
    }
}

/*!
 * @brief Set the CAN filters (including code and mask).
 *
 * @param[in] instance: CAN module instance
 * @param[in] index: CAN filter index (0-59)
 * @param[in] enable: filter enable state
 * @param[in] config: point to filter configuration
 * @return none
 */
void CAN_DRV_SetFilter(uint8_t instance, uint8_t index, bool enable, const can_filter_config_t *config)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(index < CAN_FILTER_NUM_MAX);
    DEVICE_ASSERT(config != NULL);

    CAN_Type *base = s_canBase[instance];
    uint32_t mask = 0U;
    bool reset = false;

    /* Check the RESET is 1 first */
    reset = CAN_GetResetState(base);
    if (false == reset)
    {
        CAN_SetReset(base, true);
    }

    /* Enable filter */
    CAN_SetFilterEnable(base, index, enable);
    /* Configure filter */
    if (enable)
    {
        /* Select filter index */
        CAN_SetAcfIndex(base, index);
        /* Set filter code */
        CAN_SetAcfCode(base, config->code);
        /* Set filter mask */
        mask = config->mask & CAN_ACF_ACODE_Msk;
        if (CAN_MSG_ID_STD == config->idType)
        {
            mask |= CAN_ACF_AIDEE_Msk;
        }
        else if (CAN_MSG_ID_EXT == config->idType)
        {
            mask |= CAN_ACF_AIDEE_Msk | CAN_ACF_AIDE_Msk;
        }
        else
        {
            /* Both */
        }
        CAN_SetAcfMask(base, mask);
    }

    if (false == reset)
    {
        CAN_SetReset(base, false);
    }
}

/*!
 * @brief Install event callback function.
 *
 * @param[in] instance: CAN module instance
 * @param[in] callback: pointer to callback function
 * @return none
 */
void CAN_DRV_InstallEventCallback(uint8_t instance, const can_callback_t callback)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    s_canState[instance].callback = callback;
}

/*!
 * @brief Install wakeup callback function.
 *
 * @param[in] instance: CAN module instance
 * @param[in] callback: pointer to wakeup callback function
 * @return none
 */
void CAN_DRV_InstallWakeupCallback(uint8_t instance, const can_callback_t callback)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    s_canState[instance].wakeupCallback = callback;
}

/*!
 * @brief Enable or disable CAN interrupt.
 *
 * @param[in] instance: CAN module instance
 * @param[in] enable: interrupt enable state
 * @return none
 */
static void CAN_EnableIRQ(uint8_t instance, bool enable)
{
    if (enable)
    {
        NVIC_EnableIRQ(s_canIrq[instance]);
    }
    else
    {
        NVIC_DisableIRQ(s_canIrq[instance]);
        NVIC_ClearPendingIRQ(s_canIrq[instance]);
    }
    s_canState[instance].interruptEn = enable;
}

/*!
 * @brief Enable or disable CAN wakeup interrupt.
 *
 * @param[in] instance: CAN module instance
 * @param[in] enable: wakeup interrupt enable state
 * @return none
 */
static void CAN_EnableWakeupIRQ(uint8_t instance, bool enable)
{
    if (enable)
    {
        NVIC_EnableIRQ(s_canWakeupIrq[instance]);
    }
    else
    {
        NVIC_DisableIRQ(s_canWakeupIrq[instance]);
        NVIC_ClearPendingIRQ(s_canWakeupIrq[instance]);
    }
}

/*!
 * @brief Set software reset state.
 *
 * @param[in] instance: CAN module instance
 * @param[in] enable: reset state
 * @return none
 */
void CAN_DRV_SetReset(uint8_t instance, bool enable)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    CAN_SetReset(s_canBase[instance], enable);
}

/*!
 * @brief Initialize the CAN driver (including clock, bitrate, filter and interrupt settings).
 *
 * @param[in] instance: CAN module instance
 * @param[in] config: point to CAN configuration
 * @return initialize status
 *         - STATUS_SUCCESS: initialize successfully
 *         - STATUS_ERROR: initialize error
 */
status_t CAN_DRV_Init(uint8_t instance, const can_user_config_t *config)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];
    can_state_t *state = &s_canState[instance];
    uint8_t i = 0U;

    state->interruptFlag = 0U;

    /* Configure CAN clock and reset */
    CKGEN_DRV_Enable(s_canCkgenClock[instance], true);
    CKGEN_DRV_SoftReset(s_canSoftReset[instance], true);

    /* Reset CAN */
    CAN_SetReset(base, true);

    /* Set memory ECC */
    CAN_SetMemEcc(base, config->memEccEn);
    state->memEccEn = config->memEccEn;

    /* Set bitrate */
    CAN_SetSlowBitrate(base, &config->bitrate);
    if (config->fdModeEn)
    {
        CAN_SetFdIso(base, config->fdIsoEn);
        CAN_SetFastBitrate(base, &config->fdBitrate);
        CAN_SetTdc(base, config->tdcEn);
        CAN_SetSspOffset(base, config->sspOffset);
    }

    /* Wait memory ECC initialization done */
    if (config->memEccEn)
    {
        while (false == CAN_IsMemInitDone(base))
        {
            if (++i > CAN_ECC_INIT_TIMEOUT)
            {
                return STATUS_ERROR;
            }
        }
    }

    /* Set filters(use default setting or user define) */
    if (config->filterList != NULL)
    {
        for (i = 0U; i < config->filterNum; i++)
        {
            CAN_DRV_SetFilter(instance, i, true, &config->filterList[i]);
        }
    }

    /* Set DMA receive */
    CAN_SetDmaRecv(base, config->dmaRecvEn);
    state->dmaRecvEn = config->dmaRecvEn;

    /* Start synchronize */
    CAN_SetReset(base, false);

    /* Set CAN configuration */
    if (CAN_LISTEN_ONLY_MODE == config->canMode)
    {
        CAN_SetListenOnlyMode(base, true);
    }
    else if (CAN_LOOPBACK_INT_MODE == config->canMode)
    {
        CAN_SetLoopbackIntMode(base, true);
    }
    else if (CAN_LOOPBACK_EXT_MODE == config->canMode)
    {
        CAN_SetLoopbackExtMode(base, true);
        CAN_SetSelfAck(base, config->selfAckEn);
    }
    else
    {
        CAN_SetListenOnlyMode(base, false);
    }
    /* EWL, TPSS, TSSS must be set after setting RESET to 0 */
    CAN_SetErrorWarningLimit(base, config->errorWarningLimit);
    CAN_SetTpss(base, config->tpss);
    CAN_SetTsss(base, config->tsss);
    CAN_SetAfwl(base, (uint8_t)(CAN_RECEIVE_FIFO_COUNT - 1U));
    CAN_SetTransSecMode(base, config->tsMode);
    CAN_SetRbufOverflowMode(base, config->rom);
    CAN_SetBusOffRecDisable(base, config->busOffRecDisable);

    /* Set CAN time stamp */
    CAN_SetTimeStamp(base, false);
    if (config->timeStampEn)
    {
        CKGEN_SetCANTimeStampClkDiv(instance, config->timeStampClkDiv);
        CAN_SetTimeStampClkSrc(base, config->timeStampClkSrc);
        CAN_SetTimeStampPos(base, config->timeStampPos);
        CAN_SetTimeStamp(base, true);
        CAN_SetTimeStampCount(base, true);
    }
    state->tsAmount = config->tsAmount;
    state->timeStampEn = config->timeStampEn;

    /* Create semaphores for blocking interfaces */
    if (OSIF_SemaCreate(&state->txCompleted, 0) != STATUS_SUCCESS)
    {
        return STATUS_ERROR;
    }
    else if (OSIF_SemaCreate(&state->rxCompleted, 0) != STATUS_SUCCESS)
    {
        (void)OSIF_SemaDestroy(&state->txCompleted);
        return STATUS_ERROR;
    }

    /* Set interrupt enable and callback function */
    if (config->dmaRecvEn)
    {
        /* If enable DMA receive, the receive done interrupt is not needed */
        CAN_SetIntMask(base, config->interruptMask & (~CAN_CTRL1_RIE_Msk));
    }
    else
    {
        CAN_SetIntMask(base, config->interruptMask);
    }
    CAN_SetEccIntMask(base, config->memEccIntMask);
    CAN_SetLowPassFilter(base, config->lowPassFilterEn);
    CAN_SetWakeup(base, config->wakeupIntEn);

    CAN_DRV_InstallEventCallback(instance, config->callback);
    CAN_EnableIRQ(instance, config->interruptEn);
    CAN_DRV_InstallWakeupCallback(instance, config->wakeupCallback);
    CAN_EnableWakeupIRQ(instance, config->wakeupIntEn);

    state->initState = true;

    return ret;
}

/*!
 * @brief Deinitialize the CAN module.
 *
 * @param[in] instance: CAN module instance
 * @return deinitialize status
 *         - STATUS_SUCCESS: deinitialize successfully
 *         - STATUS_ERROR: deinitialize error
 */
status_t CAN_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];
    can_state_t *state = &s_canState[instance];

    /* Reset CAN */
    if (state->initState)
    {
        state->initState = false;
        CAN_SetReset(base, true);
    }
    (void)OSIF_SemaDestroy(&state->txCompleted);
    (void)OSIF_SemaDestroy(&state->rxCompleted);

    /* Disable CAN interrupt */
    CAN_DRV_InstallEventCallback(instance, NULL);
    CAN_EnableIRQ(instance, false);
    CAN_DRV_InstallWakeupCallback(instance, NULL);
    CAN_EnableWakeupIRQ(instance, false);

    /* Disable CAN reset and interface clock */
    CKGEN_DRV_SoftReset(s_canSoftReset[instance], false);
    CKGEN_DRV_Enable(s_canCkgenClock[instance], false);

    return ret;
}

/*!
 * @brief Initialize CAN DMA receive.
 *
 * @param[in] instance: CAN module instance
 * @param[in] dmaChannel: DMA Channel instance
 * @param[in] destAddr: destination address
 * @param[in] size: total number of bytes (4-byte times)
 * @param[in] callback: pointer to DMA callback function
 * @return initialize DMA status
 *         - STATUS_SUCCESS: initialize DMA successfully
 *         - STATUS_ERROR: initialize DMA error
 */
status_t CAN_DRV_InitDMARecv(uint8_t instance, uint8_t dmaChannel,
                             uint32_t destAddr, uint8_t size, dma_callback_t callback)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(dmaChannel < DMA_VIRTUAL_CH_MAX);
    DEVICE_ASSERT((size > 0U) && (size <= CAN_MAX_RBUF_LENGTH));

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];

    if (!s_canState[instance].dmaRecvEn)
    {
        ret = STATUS_ERROR;
    }
    else
    {
        /* Configure DMA transfer */
        (void)DMA_DRV_ConfigTransfer(dmaChannel, DMA_TRANSFER_PERIPH2MEM, (uint32_t)&base->RBUF, \
            destAddr, DMA_TRANSFER_SIZE_4B, size);
        DMA_DRV_SetSrcOffset(dmaChannel, 0x4U);
        (void)DMA_DRV_InstallCallback(dmaChannel, callback, (void*)((uint32_t)instance));
        /* Start DMA channel */
        (void)DMA_DRV_StartChannel(dmaChannel);
    }

    return ret;
}

/*!
 * @brief Start next DMA receive.
 *
 * @param[in] dmaChannel: DMA Channel instance
 * @param[in] destAddr: destination address
 * @param[in] size: total number of bytes (4-byte times)
 * @return none
 */
void CAN_DRV_StartNextDMA(uint8_t dmaChannel, uint32_t destAddr, uint8_t size)
{
    DEVICE_ASSERT(dmaChannel < DMA_VIRTUAL_CH_MAX);
    DEVICE_ASSERT((size > 0U) && (size <= CAN_MAX_RBUF_LENGTH));

    DMA_DRV_SetDestAddr(dmaChannel, destAddr, destAddr + size);
    (void)DMA_DRV_StartChannel(dmaChannel);
}

/*!
 * @brief Get CAN default configuration.
 *
 * @param[out] config: pointer to CAN configuration
 * @return none
 */
void CAN_DRV_GetDefaultConfig(can_user_config_t *config)
{
    DEVICE_ASSERT(config != NULL);

    can_time_segment_t bitrate = {0x22U, 0x0BU, 0x0BU, 0x01U};

    config->canMode = CAN_NORMAL_MODE;
    config->errorWarningLimit = 0x0BU;
    config->interruptEn = true;
    config->interruptMask = CAN_IRQ_ALL_ENABLE_MSK;
    config->bitrate = bitrate;

    config->fdModeEn = false;
    config->filterNum = 0U;
}

/*!
 * @brief Set TBUF directly.
 *
 * @param[in] instance: CAN module instance
 * @param[in] txBuf: pointer to transmission buffer
 * @param[in] length: number of buffer bytes
 * @param[in] type: transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return none
 */
void CAN_DRV_SetTbuf(uint8_t instance, const uint8_t *txBuf, uint8_t length, can_transmit_buff_t type)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(txBuf != NULL);
    DEVICE_ASSERT((length > 0U) && (length <= CAN_MAX_TBUF_LENGTH));

    CAN_Type *base = s_canBase[instance];

    /* Select transmit buffer */
    CAN_SetTbufSelect(base, type);

    /* Write transmit buffer */
    CAN_WriteTbuf(base, txBuf, length);

    if (CAN_TRANSMIT_SECONDARY == type)
    {
        /* Select next secondary buffer */
        CAN_SelectNextTsbuf(base);
    }
}

/*!
 * @brief Set a message information to TBUF.
 *
 * @param[in] instance: CAN module instance
 * @param[in] info: message information pointer
 * @param[in] type: transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return none
 */
void CAN_DRV_SetMsgToTbuf(uint8_t instance, const can_msg_info_t *info, can_transmit_buff_t type)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(info != NULL);
    DEVICE_ASSERT(info->DATA != NULL);

    uint32_t id = 0U, ctrl = 0U;
    uint8_t dataLength = 0U;
    CAN_Type *base = s_canBase[instance];

    /* Select transmit buffer */
    CAN_SetTbufSelect(base, type);

    /* Set message ID, TTSEN, ESI bits */
    id = (info->ID & CAN_INFO_ID_Msk);
    /* Set TTSEN bit */
    if (s_canState[instance].timeStampEn)
    {
        id |= CAN_INFO_TTSEN_Msk;
    }
    /* Set ESI bit */
    if (info->ESI)
    {
        id |= CAN_INFO_ESI_Msk;
    }
    CAN_SetTbufId(base, id);

    /* Set message control bits */
    ctrl = (uint32_t)(info->DLC & CAN_INFO_DLC_Msk);
    if (info->FDF)
    {
        ctrl |= CAN_INFO_FDF_Msk;
        if (info->BRS)
        {
            ctrl |= CAN_INFO_BRS_Msk;
        }
    }
    else if (info->RTR)
    {
        ctrl |= CAN_INFO_RTR_Msk;
    }
    else
    {
    }
    if (info->IDE)
    {
        ctrl |= CAN_INFO_IDE_Msk;
    }
    CAN_SetTbufCtrl(base, ctrl);

    /* Set message data bits */
    if (((uint8_t)CAN_MSG_DATA_FRAME == info->RTR) && (info->DLC > 0U))
    {
        dataLength = CAN_ComputePayloadSize(info->DLC);
        CAN_WriteTbufData(base, (&info->DATA[0]), dataLength);
    }

    if (CAN_TRANSMIT_SECONDARY == type)
    {
        /* Select next secondary buffer */
        CAN_SelectNextTsbuf(base);
    }
}

/*!
 * @brief Set amount for transmit secondary buffer.
 *
 * @param[in] instance: CAN module instance
 * @param[in] amount: transmit secondary buffer amount
 *            - CAN_TRANSMIT_SEC_ALL
 *            - CAN_TRANSMIT_SEC_ONE
 * @return none
 */
void CAN_DRV_SetTransSecAmount(uint8_t instance, can_transmit_sec_amount_t amount)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    s_canState[instance].tsAmount = amount;
}

/*!
 * @brief Get transmit secondary buffer status.
 *
 * @param[in] instance: CAN module instance
 * @return transmit secondary buffer status
 */
can_tsbuf_status_t CAN_DRV_GetTsbufStatus(uint8_t instance)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    return (can_tsbuf_status_t)CAN_GetTsbufStatus(s_canBase[instance]);
}

/*!
 * @brief Check transmit is busy.
 *
 * @param[in] instance: CAN module instance
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return transmit busy flag
 *         - false: not busy or STB is not full
 *         - true: is busy or STB is full
 */
bool CAN_DRV_IsTransmitBusy(uint8_t instance, can_transmit_buff_t type)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    return CAN_IsTransmitBusy(s_canBase[instance], type);
}

/*!
 * @brief Check whether is transmitting.
 *
 * @param[in] instance: CAN module instance
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return transmitting status
 *         - false: not transmitting
 *         - true: is transmitting
 */
bool CAN_DRV_IsTransmitting(uint8_t instance, can_transmit_buff_t type)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    return CAN_IsTransmitting(s_canBase[instance], type);
}

/*!
 * @brief Send a CAN frame using the transmit buffer type.
 *
 * @param[in] instance: CAN module instance
 * @param[in] info: pointer to message information
 * @param[in] type: transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return send status
 *         - STATUS_SUCCESS: send successfully
 *         - STATUS_ERROR: send error
 *         - STATUS_BUSY: send busy
 */
status_t CAN_DRV_Send(uint8_t instance, const can_msg_info_t *info, can_transmit_buff_t type)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(info != NULL);

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];
    can_state_t *state = &s_canState[instance];

    /* Check CAN is in the reset or standby status */
    if (CAN_GetResetState(base) || CAN_GetStandbyState(base))
    {
        return STATUS_ERROR;
    }

    /* Check transmission is idle or the TBUF is not full, when transmitting secondary all */
    if ((CAN_TRANSMIT_SECONDARY == type) && (CAN_TRANSMIT_SEC_ALL == state->tsAmount))
    {
        if (CAN_IsTransmitBusy(base, type))
        {
            ret = STATUS_BUSY;
        }
    }
    else if (CAN_IsTransmitting(base, type))
    {
        ret = STATUS_BUSY;
    }

    /* Fill transmit buffer, than enable transmission */
    if (STATUS_SUCCESS == ret)
    {
        CAN_DRV_SetMsgToTbuf(instance, info, type);
        CAN_StartTransmit(base, type, state->tsAmount);
    }

    return ret;
}

/*!
 * @brief Send a CAN frame using the transmit buffer type in a blocking way.
 *
 * @param[in] instance: CAN module instance
 * @param[in] info: pointer to message information
 * @param[in] type: transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @param[in] timeout: timeout value
 * @return send status
 *         - STATUS_SUCCESS: send successfully
 *         - STATUS_ERROR: send error
 *         - STATUS_BUSY: send busy
 *         - STATUS_TIMEOUT: send timeout
 */
status_t CAN_DRV_SendBlocking(uint8_t instance, const can_msg_info_t *info, can_transmit_buff_t type, uint32_t timeout)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(info != NULL);

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];
    can_state_t *state = &s_canState[instance];

    /* Check CAN is in the reset or standby status */
    if (CAN_GetResetState(base) || CAN_GetStandbyState(base))
    {
        return STATUS_ERROR;
    }

    /* Check transmission is idle or the TBUF is not full, when transmitting secondary all */
    if ((CAN_TRANSMIT_SECONDARY == type) && (CAN_TRANSMIT_SEC_ALL == state->tsAmount))
    {
        if (CAN_IsTransmitBusy(base, type))
        {
            ret = STATUS_BUSY;
        }
    }
    else if (CAN_IsTransmitting(base, type))
    {
        ret = STATUS_BUSY;
    }

    /* Fill transmit buffer, than start transmission */
    if (STATUS_SUCCESS == ret)
    {
        CAN_DRV_SetMsgToTbuf(instance, info, type);
        CAN_StartTransmit(base, type, state->tsAmount);

        /* Wait for the interrupt of transmission complete */
        ret = OSIF_SemaWait(&state->txCompleted, timeout);
    }

    return ret;
}

/*!
 * @brief Get receive buffer status.
 *
 * @param[in] instance: CAN module instance
 * @return receive buffer status
 */
can_rbuf_status_t CAN_DRV_GetRbufStatus(uint8_t instance)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    return (can_rbuf_status_t)CAN_GetRbufStatus(s_canBase[instance]);
}

/*!
 * @brief Get RBUF directly.
 *
 * @param[in] instance: CAN module instance
 * @param[out] rxBuf: pointer to receive buffer
 * @param[in] length: number of buffer bytes
 * @return get receive buffer status
 *         - STATUS_SUCCESS: get receive buffer successfully
 *         - STATUS_ERROR: get receive buffer error
 */
status_t CAN_DRV_GetRbuf(uint8_t instance, uint8_t *rxBuf, uint8_t length)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuf != NULL);
    DEVICE_ASSERT((length > 0U) && (length <= CAN_MAX_RBUF_LENGTH));

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];

    if (0U == CAN_GetRbufStatus(base))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        /* Read receive buffer */
        CAN_ReadRbuf(base, rxBuf, length);
        /* Release receive buffer */
        CAN_ReleaseRbuf(base);
    }

    return ret;
}

/*!
 * @brief Get the CAN message information from reveive buffer.
 *
 * @param[out] info: pointer to message information
 * @param[in] buf: pointer to receive buffer
 * @return none
 */
void CAN_DRV_GetMsgFromRbuf(can_msg_info_t *info, const CAN_BufType *buf)
{
    DEVICE_ASSERT(info != NULL);
    DEVICE_ASSERT(info->DATA != NULL);
    DEVICE_ASSERT(buf != NULL);

    uint32_t ctrl = 0U, *data = (uint32_t *)&buf->DATA[0];
    uint8_t dataLength = 0U, i = 0U;

    info->ID  = buf->ID_ESI & CAN_INFO_ID_Msk;
    info->ESI = (uint8_t)(buf->ID_ESI >> CAN_INFO_ESI_Pos);
    ctrl = buf->CTRL;
    info->BRS = (uint8_t)((ctrl >> CAN_INFO_BRS_Pos) & 0x01U);
    info->FDF = (uint8_t)((ctrl >> CAN_INFO_FDF_Pos) & 0x01U);
    info->RTR = (uint8_t)((ctrl >> CAN_INFO_RTR_Pos) & 0x01U);
    info->IDE = (uint8_t)((ctrl >> CAN_INFO_IDE_Pos) & 0x01U);
    info->DLC = (uint8_t)(ctrl & CAN_INFO_DLC_Msk);

    if (((uint8_t)CAN_MSG_DATA_FRAME == info->RTR) && (info->DLC > 0U))
    {
        dataLength = CAN_ComputePayloadSize(info->DLC);
        if ((0U == info->FDF) && (dataLength > 8U))
        {
            dataLength = 8U;
        }
        for (i = 0U; i < dataLength; i += 4U)
        {
            *(uint32_t *)(&info->DATA[i]) = *data++;
        }
    }
    info->RTS = buf->TS[0];
}

/*!
 * @brief Receive a CAN frame.
 *
 * @param[in] instance: CAN module instance
 * @param[out] info: pointer to message information
 * @return receive status
 *         - STATUS_SUCCESS: receive successfully
 *         - STATUS_ERROR: receive error
 */
status_t CAN_DRV_Receive(uint8_t instance, can_msg_info_t *info)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(info != NULL);

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];

    /* Check if there is a message in the receive buffer */
    if (0U == CAN_GetRbufStatus(base))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        /* Get message information */
        CAN_DRV_GetMsgFromRbuf(info, &base->RBUF);
        /* Release receive buffer */
        CAN_ReleaseRbuf(base);
    }

    return ret;
}

/*!
 * @brief Receive a CAN frame in a blocking way.
 *
 * @param[in] instance: CAN module instance
 * @param[out] info: message information pointer
 * @param[in] timeout: timout value
 * @return receive status
 *         - STATUS_SUCCESS: receive successfully
 *         - STATUS_ERROR: receive error
 *         - STATUS_TIMEOUT: receive timeout
 */
status_t CAN_DRV_ReceiveBlocking(uint8_t instance, can_msg_info_t *info, uint32_t timeout)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);
    DEVICE_ASSERT(info != NULL);

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];

    /* Wait receive buffer */
    ret = OSIF_SemaWait(&s_canState[instance].rxCompleted, timeout);
    if (ret != STATUS_TIMEOUT)
    {
        /* Check if there is a message in the receive buffer */
        if (0U == CAN_GetRbufStatus(base))
        {
            ret = STATUS_ERROR;
        }
        else
        {
            /* Get message information */
            CAN_DRV_GetMsgFromRbuf(info, &base->RBUF);
            /* Release receive buffer */
            CAN_ReleaseRbuf(base);
        }
    }

    return ret;
}

/*!
 * @brief Abort a CAN transmission frame which has been requested.
 *
 * @param[in] instance: CAN module instance
 * @param[in] type: transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return abort status
 *         - STATUS_SUCCESS: abort successfully
 *         - STATUS_ERROR: abort error
 */
status_t CAN_DRV_AbortTransfer(uint8_t instance, can_transmit_buff_t type)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];

    if (CAN_IsTransmitting(base, type))
    {
        CAN_AbortTransmit(base, type);
    }
    else
    {
        ret = STATUS_ERROR;
    }

    return ret;
}

/*!
 * @brief Enter or exit transceiver standby mode.
 *
 * @param[in] instance: CAN module instance
 * @param[in] enable: standby enable state
 * @return set standby status
 *         - STATUS_SUCCESS: set standby successfully
 *         - STATUS_ERROR: set standby error
 */
status_t CAN_DRV_SetStandby(uint8_t instance, bool enable)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    status_t ret = STATUS_SUCCESS;
    CAN_Type *base = s_canBase[instance];

    if (CAN_IsTransmitting(base, CAN_TRANSMIT_PRIMARY) || CAN_IsTransmitting(base, CAN_TRANSMIT_SECONDARY))
    {
        ret = STATUS_ERROR;
    }
    else
    {
        CAN_SetStandby(base, enable);
    }

    return ret;
}

/*!
 * @brief Get interrupt flags.
 *
 * @param[in] instance: CAN module instance
 * @return interrupt flag
 */
uint32_t CAN_DRV_GetInterruptFlag(uint8_t instance)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    uint32_t flag = 0U;
    CAN_Type *base = s_canBase[instance];

    if (s_canState[instance].interruptEn)
    {
        flag = s_canState[instance].interruptFlag;
    }
    else
    {
        flag = (CAN_GetCtrl1(base) & CAN_IRQ_FLAG_MSK) | \
            ((CAN_GetVerMem(base) & CAN_ECC_IRQ_FLAG_MSK) << CAN_ECC_IRQ_FLAG_OFFSET);
    }

    return flag;
}

/*!
 * @brief Clear interrupt flags.
 *
 * @param[in] instance: CAN module instance
 * @return none
 */
void CAN_DRV_ClearInterruptFlag(uint8_t instance)
{
    DEVICE_ASSERT(instance < CAN_INSTANCE_MAX);

    CAN_Type *base = s_canBase[instance];

    if (s_canState[instance].interruptEn)
    {
        s_canState[instance].interruptFlag = 0U;
    }
    else
    {
        CAN_SetCtrl1(base, CAN_GetCtrl1(base));
        CAN_SetVerMem(base, CAN_GetVerMem(base));
    }
}

/* =============================================  EOF  ============================================== */
