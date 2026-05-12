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
 * @file dma_hw.h
 *
 * @brief This file provides dma hardware integration interface.
 *
 */

#ifndef _DMA_HW_H
#define _DMA_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "dma_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Clear DMA channel interrupt status.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @return The interrupt status flags in register before cleared by software
 */
uint32_t DMA_ClearIntStatus(DMA_ChannelType *base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enable/Disable DMA channel
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] enable: Enable/Disable DMA channel
 * @return none
 */
static inline void DMA_SetChannel(DMA_ChannelType *base, bool enable)
{
    MODIFY_REG32(base->CHAN_ENABLE, DMA_CHANNEL_CHAN_ENABLE_CHAN_ENABLE_Msk, \
                 DMA_CHANNEL_CHAN_ENABLE_CHAN_ENABLE_Pos, enable);
}

/*!
 * @brief DMA Top warm reset
 *
 * @param[in] none
 * @return none
 */
static inline void DMA_TopWarmRst(void)
{
    DMA0_TOP_RST->TOP_RST |= DMA_TOP_RST_WARM_RST_Msk;
}

/*!
 * @brief DMA_Top Hard reset
 *
 * @param[in] none
 * @return    none
 */
static inline void DMA_TopHardRst(void)
{
    DMA0_TOP_RST->TOP_RST |= DMA_TOP_RST_HARD_RST_Msk;
    DMA0_TOP_RST->TOP_RST &= ~DMA_TOP_RST_HARD_RST_Msk;
}

/*!
 * @brief DMA channel warm reset.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @return none
 */
static inline void DMA_ChannelWarmRst(DMA_ChannelType *base)
{
    base->RST |= DMA_CHANNEL_RST_WARM_RST_Msk;
}

/*!
 * @brief DMA channel hard reset.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @return none
 */
static inline void DMA_ChannelHardRst(DMA_ChannelType *base)
{
    base->RST |= DMA_CHANNEL_RST_HARD_RST_Msk;
    base->RST &= ~DMA_CHANNEL_RST_HARD_RST_Msk;
}

/*!
 * @brief Set DMA channel source start address.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] startAddr: DMA source start address, 32bit
 * @return none
 */
static inline void DMA_SetSrcStartAddr(DMA_ChannelType *base, uint32_t startAddr)
{
    WRITE_REG32(base->SSTART_ADDR, startAddr);
}

/*!
 * @brief Set DMA channel source end address.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] srcEndAddr:DMA source end address, 32bit
 * @return none
 */
static inline void DMA_SetSrcEndAddr(DMA_ChannelType *base, uint32_t srcEndAddr)
{
    WRITE_REG32(base->SEND_ADDR, srcEndAddr);
}

/*!
 * @brief Set DMA channel destination start address.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] destStartAddr:DMA destination start address, 32bit
 * @return none
 */
static inline void DMA_SetDestStartAddr(DMA_ChannelType *base, uint32_t destStartAddr)
{
    WRITE_REG32(base->DSTART_ADDR, destStartAddr);
}

/*!
 * @brief Set DMA channel destination end address.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] destEndAddr: DMA destination end address, 32bit
 * @return none
 */
static inline void DMA_SetDestEndAddr(DMA_ChannelType *base, uint32_t destEndAddr)
{
    WRITE_REG32(base->DEND_ADDR, destEndAddr);
}

/*!
 * @brief Set DMA channel source transfer size.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] srcTransferSize: DMA channel source transfer size
 *                 -DMA_TRANSFER_SIZE_1B
 *                 -DMA_TRANSFER_SIZE_2B
 *                 -DMA_TRANSFER_SIZE_4B
 * @return none
 */
static inline void DMA_SetSrcTransferSize(DMA_ChannelType *base, dma_transfer_size_t srcTransferSize)
{
    MODIFY_REG32(base->CONFIG, DMA_CHANNEL_CONFIG_SSIZE_Msk, \
                 DMA_CHANNEL_CONFIG_SSIZE_Pos, srcTransferSize);
}

/*!
 * @brief Set DMA channel destination transfer size.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] destTransferSize: DMA channel destination transfer size
 *                 -DMA_TRANSFER_SIZE_1B
 *                 -DMA_TRANSFER_SIZE_2B
 *                 -DMA_TRANSFER_SIZE_4B
 * @return none
 */
static inline void DMA_SetDestTransferSize(DMA_ChannelType *base, dma_transfer_size_t destTransferSize)
{
    MODIFY_REG32(base->CONFIG, DMA_CHANNEL_CONFIG_DSIZE_Msk, \
                 DMA_CHANNEL_CONFIG_DSIZE_Pos, destTransferSize);
}

/*!
 * @brief Set DMA channel source address offset.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] srcOffset: source address offset
 * @return none
 */
static inline void DMA_SetSrcAddrOffSet(DMA_ChannelType *base, uint16_t srcOffset)
{
    MODIFY_REG32(base->ADDR_OFFSET, DMA_CHANNEL_ADDR_OFFSET_SOFFSET_Msk, \
                 DMA_CHANNEL_ADDR_OFFSET_SOFFSET_Pos, srcOffset);
}

/*!
 * @brief Set DMA destination address offset.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] destOffset: destination address offset
 * @return none
 */
static inline void DMA_SetDestAddrOffSet(DMA_ChannelType *base, uint16_t destOffset)
{
    MODIFY_REG32(base->ADDR_OFFSET, DMA_CHANNEL_ADDR_OFFSET_DOFFSET_Msk, \
                 DMA_CHANNEL_ADDR_OFFSET_DOFFSET_Pos, destOffset);
}

/*!
 * @brief Set DMA channel transfer length by bytes.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] bytesLength: DMA channel transfer length(0~32767 byte, bit 15 should be 0)
 * @return none
 */
static inline void DMA_SetTranferLength(DMA_ChannelType *base, uint16_t bytesLength)
{
    MODIFY_REG32(base->CHAN_LENGTH, DMA_CHANNEL_CHAN_LENGTH_CHAN_LENGTH_Msk, \
                 DMA_CHANNEL_CHAN_LENGTH_CHAN_LENGTH_Pos, bytesLength);
}

/*!
 * @brief Get DMA channel total transfer length by bytes.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @return DMA channel transfer length
 */
static inline uint32_t DMA_GetTranferLength(DMA_ChannelType *base)
{
    return (base->CHAN_LENGTH & DMA_CHANNEL_CHAN_LENGTH_CHAN_LENGTH_Msk);
}

/*!
 * @brief Get number of bytes transfered by DMA channel
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @return DMA channel transfered bytes number
 */
static inline uint32_t DMA_GetTransferedBytes(DMA_ChannelType *base)
{
    return (base->DATA_TRANS_NUM & DMA_CHANNEL_DATA_TRANS_NUM_DATA_TRANS_NUM_Msk);
}

/*!
 * @brief Set DMA channel priority.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] channelPriority: DMA chanenl priority
 *                 -DMA_CHN_PRIORITY_LOW
 *                 -DMA_CHN_PRIORITY_MEDIUM
 *                 -DMA_CHN_PRIORITY_HIGH
 *                 -DMA_CHN_PRIORITY_VERY_HIGH
 * @return none
 */
static inline void DMA_SetChannelPriority(DMA_ChannelType *base, dma_channel_priority_t channelPriority)
{
    MODIFY_REG32(base->CONFIG, DMA_CHANNEL_CONFIG_CHAN_PRIORITY_Msk, \
                 DMA_CHANNEL_CONFIG_CHAN_PRIORITY_Pos, channelPriority);
}

/*!
 * @brief Set DMA channel circular mode
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] enable: enable/disable DMA channel circular mode
 * @return none
 */
static inline void DMA_SetCircularMode(DMA_ChannelType *base, bool enable)
{
    MODIFY_REG32(base->CONFIG, DMA_CHANNEL_CONFIG_CHAN_CIRCULAR_Msk, \
                 DMA_CHANNEL_CONFIG_CHAN_CIRCULAR_Pos, enable);
}

/*!
 * @brief Set DMA channel FIFO move fast function.
 *        When DMA read from peripheral, it must be disable FIFO fast function.
 *        When DMA read from memory, it must be enable FIFO fast function.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] enable: Enable or disable move data to FIFO early
 *                 -true: move data to DMA FIFO early, ignored DMA request
 *                 -false: move data to DMA FIFO by DMA request
 * @return none
 */
static inline void DMA_SetFIFOFastFunction(DMA_ChannelType *base, bool enable)
{
    MODIFY_REG32(base->CONFIG, DMA_CHANNEL_CONFIG_CHAN_DIR_Msk, \
                 DMA_CHANNEL_CONFIG_CHAN_DIR_Pos, enable);
}

/*!
 * @brief Get the number of the data left in the DMA channel's internal FIFO
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @return DMA Channel FIFO left data bytes number
 */
static inline uint32_t DMA_GetInterFIFODataLeftBytes(DMA_ChannelType *base)
{
    return (base->FIFO_LEFT_NUM & DMA_CHANNEL_FIFO_LEFT_NUM_FIFO_LEFT_NUM_Msk);
}

/*!
 * @brief Flush DMA channel data
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @return none
 */
static inline void DMA_FlushChannelFIFO(DMA_ChannelType *base)
{
    SET_BIT32(base->RST, DMA_CHANNEL_RST_FLUSH_Msk);
}

/*!
 * @brief Enable/Disable DMA channel transfer finish interrupt
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] enable: enable/disable DMA channel finish interrupt
 * @return none
 */
static inline void DMA_SetFinishInterrupt(DMA_ChannelType *base, bool enable)
{
    MODIFY_REG32(base->INTEN, DMA_CHANNEL_INTEN_FINISH_INTERRUPT_ENABLE_Msk, \
                 DMA_CHANNEL_INTEN_FINISH_INTERRUPT_ENABLE_Pos, enable);
}

/*!
 * @brief Enable DMA channel Error interrupt
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] enable: enable/disable DMA transfer Error interrupt
 *                  - ENABLE
 *                  - DISABLE
 * @return none
 */
static inline void DMA_SetErrorInterrupt(DMA_ChannelType *base, bool enable)
{
    MODIFY_REG32(base->INTEN, DMA_CHANNEL_INTEN_TRANS_ERROR_INTERRUPT_ENABLE_Msk, \
                 DMA_CHANNEL_INTEN_TRANS_ERROR_INTERRUPT_ENABLE_Pos, enable);
}

/*!
 * @brief Configure DMA channel trigger from CTU module.
 *        Only DMA channel 0~3 support trigger function.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] enable: Enable or disable DMA channel trigger
 */
static inline void DMA_SetChannelTrigger(DMA_ChannelType *base, bool enable)
{
    MODIFY_REG32(base->DMAMUX_CFG, DMA_CHANNEL_DMAMUX_CFG_TRIG_EN_Msk, \
                 DMA_CHANNEL_DMAMUX_CFG_TRIG_EN_Pos, enable);
}

/*!
 * @brief Configures the DMA request for the DMA channel.
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] source: DMA request source.The DMA request sources(dma_request_source_t)
 *                    are defined in the file dma_drv.h
 */
static inline void DMA_SetChannelSource(DMA_ChannelType *base, uint8_t source)
{
    MODIFY_REG32(base->DMAMUX_CFG, DMA_CHANNEL_DMAMUX_CFG_REQ_ID_Msk, \
                 DMA_CHANNEL_DMAMUX_CFG_REQ_ID_Pos, source);
}

/*!
 * @brief Set DMA debug function
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] enable: enable or disable debug fucntion
 *                 -true: In debug mode, DMA continues to operate
 *                 -false: In debug mode, DMA will be disabled when CPU is halted
 * @return none
 */
static inline void DMA_SetChannelDebug(DMA_ChannelType *base, bool enable)
{
    MODIFY_REG32(base->CHAN_ENABLE, DMA_CHANNEL_CHAN_ENABLE_EDBG_Msk, DMA_CHANNEL_CHAN_ENABLE_EDBG_Pos, enable);
}

/*!
 * @brief DMA channel transfer pause/resume
 *
 * @param[in] base: Base address for DMA channel
 *                 -DMA0_CHANNEL0
 *                 -DMA0_CHANNEL1
 *                 -...
 *                 -DMA0_CHANNEL15
 * @param[in] enable: enable/disable DMA paulse
 *                  - true: DMA channel transfer pause
 *                  - false: DMA channel transfer resume
 * @return none
 */
static inline void DMA_SetChannelPause(DMA_ChannelType *base, bool enable)
{
    MODIFY_REG32(base->STOP, DMA_CHANNEL_STOP_STOP_Msk, DMA_CHANNEL_STOP_STOP_Pos, enable);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DMA_HW_H*/

/* =============================================  EOF  ============================================== */
