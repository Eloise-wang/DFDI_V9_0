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
 * @file dma_drv.h
 *
 * @brief This file provides dma integration functions interface.
 *
 */

#ifndef _DMA_DRV_H
#define _DMA_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief DMA channel interrupts.
 */
typedef enum {
    DMA_CHN_ERR_INT = 0U,         /*!< Error interrupt */
    DMA_CHN_FINISH_INT            /*!< Tansfer finish interrupt. */
} dma_channel_interrupt_t;

/*!
 * @brief DMA channel priority setting
 */
typedef enum {
    DMA_CHN_PRIORITY_LOW = 0U,        /*!< 0: DMA priority low */
    DMA_CHN_PRIORITY_MEDIUM,          /*!< 1: DMA priority medium */
    DMA_CHN_PRIORITY_HIGH,            /*!< 2: DMA priority high */
    DMA_CHN_PRIORITY_VERY_HIGH        /*!< 3: DMA priority very high */
} dma_channel_priority_t;

/*!
 * @brief DMA transfer size configuration
 */
typedef enum {
    DMA_TRANSFER_SIZE_1B = 0x0U,
    DMA_TRANSFER_SIZE_2B = 0x1U,
    DMA_TRANSFER_SIZE_4B = 0x2U
} dma_transfer_size_t;

/*!
 * @brief Channel status for DMA channel.
 *        The user can get the status by callback parameter
 *        or by calling DMA_DRV_getStatus() function.
 */
typedef enum {
    DMA_CHN_NORMAL = 0U,           /*!< DMA channel normal state. */
    DMA_CHN_ERROR                  /*!< An error occurred in the DMA channel. */
} dma_chn_status_t;

/*!< Definition for the DMA channel callback function. */
typedef void (*dma_callback_t)(void *parameter, dma_chn_status_t status);

/*!
 * @brief Data structure for the DMA channel state.
 */
typedef struct {
    uint8_t virtChn;                     /*!< Virtual channel number. */
    dma_callback_t callback;             /*!< Callback function pointer for the DMA channel. It will
                                              be called at the DMA channel complete and DMA channel
                                              error. */
    void *parameter;                     /*!< Parameter for the callback function pointer. */
    volatile dma_chn_status_t status;    /*!< DMA channel status. */
} dma_chn_state_t;

/*!
 * @brief The user configuration structure for the an DMA driver channel.
 */
typedef struct {
    dma_channel_priority_t channelPriority;  /*!< DMA channel priority */
    uint8_t virtChnConfig;                   /*!< DMA virtual channel number */
    dma_request_source_t source;             /*!< Selects the source of the DMA request for this channel */
    dma_callback_t callback;                 /*!< Callback that will be registered for this channel */
    void *callbackParam;                     /*!< Parameter passed to the channel callback */
    bool enableTrigger;                      /*!< Enables the periodic trigger capability for the DMA channel. */
} dma_channel_config_t;

/*!
 * @brief Type for the DMA transfer.
 */
typedef enum {
    DMA_TRANSFER_PERIPH2MEM = 0U,   /*!< Transfer from peripheral to memory */
    DMA_TRANSFER_MEM2PERIPH,        /*!< Transfer from memory to peripheral */
    DMA_TRANSFER_MEM2MEM,           /*!< Transfer from memory to memory */
    DMA_TRANSFER_PERIPH2PERIPH      /*!< Transfer from peripheral to peripheral */
} dma_transfer_type_t;

/*!
 * @brief This bit only control DMA channel FIFO and DMA transfer always
 *        move from source address to destination address.
 */
typedef enum {
    DMA_READ_FROM_PERIPH = 0U,      /*!< Error interrupt */
    DMA_READ_FROM_MEM               /*!< Tansfer half finish interrupt. */
} dma_dirction_t;

/*!
 * @brief Runtime state structure for the DMA driver.
 */
typedef struct {
    dma_chn_state_t * volatile virtChnState[(uint32_t)DMA_VIRTUAL_CH_MAX];   /*!< Pointer array storing channel state. */
} dma_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes the DMA module.
 *
 * @param[in] dmaState: The pointer to the DMA module state structure
 * @param[in] chnStateArray: Array of pointers to run-time state structures for DMA channels
 * @param[in] chnConfigArray: Array of pointers to channel initialization structures
 * @param[in] chnCount: The number of DMA channels to be initialized
 * @return STATUS_ERROR or STATUS_SUCCESS.
 */
status_t DMA_DRV_Init(dma_state_t *dmaState,
                      dma_chn_state_t * const chnStateArray[],
                      const dma_channel_config_t * const chnConfigArray[],
                      uint32_t chnCount);

/*!
 * @brief De-initializes the DMA module.
 *
 * @param[in] none
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_Deinit(void);

/*!
 * @brief Initializes an DMA channel.
 *
 * @param[in] dmaChannelState: Pointer to the DMA channel state structure
 * @param[in] dmaChannelConfig: User configuration structure for DMA channel
 * @return STATUS_ERROR or STATUS_SUCCESS
 */
status_t DMA_DRV_ChannelInit(dma_chn_state_t *dmaChannelState, const dma_channel_config_t *dmaChannelConfig);

/*!
 * @brief Releases a DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_ReleaseChannel(uint8_t virtualChannel);

/*!
 * @brief Configures a data transfer with DMA.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] type: Transfer type (M->M, P->M, M->P, P->P)
 * @param[in] srcAddr: A source register address or a source memory address
 * @param[in] destAddr: A destination register address or a destination memory address
 * @param[in] transferSize: The number of bytes to be transferred on every DMA write/read
 *        Source/Dest share the same write/read size.
 * @param[in] dataBufferSize: The total number of bytes to be transferred
 * @return STATUS_ERROR or STATUS_SUCCESS
 */
status_t DMA_DRV_ConfigTransfer(uint8_t virtualChannel,
                                dma_transfer_type_t type,
                                uint32_t srcAddr,
                                uint32_t destAddr,
                                dma_transfer_size_t transferSize,
                                uint32_t dataBufferSize);

/*!
 * @brief Set DMA channel circular mode
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] enable: enable/disable DMA channel circular mode
 * @return none
 */
void DMA_DRV_SetCircularMode(uint8_t virtualChannel, bool enable);

/*!
 * @brief Starts an DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_StartChannel(uint8_t virtualChannel);

/*!
 * @brief Stops the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_StopChannel(uint8_t virtualChannel);

/*!
 * @brief Set DMA channel transfer length by bytes.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] bytesLength: DMA channel transfer length(0~32767 byte, bit 15 should be 0)
 * @return none
 */
void DMA_DRV_SetTranferLength(uint8_t virtualChannel, uint16_t bytesLength);
/*!
 * @brief Get the remaining bytes to be transferred by DMA.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return The remaining bytes to be transferred by DMA
 */
uint32_t DMA_DRV_GetRemainingBytes(uint8_t virtualChannel);

/*!
 * @brief Configures the DMA request and trigger function for the DMA channel
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] request: DMA request source
 * @param[in] enableTrigger: DMA channel periodic trigger
 * @return STATUS_SUCCESS
 */
status_t DMA_DRV_SetChannelRequestAndTrigger(uint8_t virtualChannel, uint8_t request, bool enableTrigger);

/*!
 * @brief Configures the source address for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] startAddr: The pointer to the source start address
 * @param[in] endAddr: The pointer to the source end address
 * @return none
 */
void DMA_DRV_SetSrcAddr(uint8_t virtualChannel, uint32_t startAddr, uint32_t endAddr);

/*!
 * @brief Configures the source address unsigned offset for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] offset: Unsigned-offset for source address
 * @return none
 */
void DMA_DRV_SetSrcOffset(uint8_t virtualChannel, uint16_t offset);

/*!
 * @brief Configures the destination address for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] startAddr: The pointer to the destination start address
 * @param[in] endAddr: The pointer to the destination end address
 * @return none
 */
void DMA_DRV_SetDestAddr(uint8_t virtualChannel, uint32_t startAddr, uint32_t endAddr);

/*!
 * @brief Configures the destination address unsigned offset for the DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] offset: Unsigned-offset for destination address
 * @return none
 */
void DMA_DRV_SetDestOffset(uint8_t virtualChannel, uint16_t offset);

/*!
 * @brief Disables/Enables the channel interrupt.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] interrupt: Half finish/finish/error interrupt
 * @param[in] enable: Enable (true)/Disable (false) interrupts for the current channel
 * @return none
 */
void DMA_DRV_ConfigureInterrupt(uint8_t virtualChannel, dma_channel_interrupt_t intSrc, bool enable);

/*!
 * @brief Registers the callback function and the parameter for DMA channel.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @param[in] callback: The pointer to the callback function
 * @param[in] parameter: The pointer to the callback function's parameter
 * @return STATUS_SUCCESS.
 */
status_t DMA_DRV_InstallCallback(uint8_t virtualChannel, dma_callback_t callback, void *parameter);

/*!
 * @brief Gets the DMA channel status.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return Channel status
 */
dma_chn_status_t DMA_DRV_GetChannelStatus(uint8_t virtualChannel);

/*!
 * @brief Gets the DMA channel base pointer.
 *
 * @param[in] virtualChannel: DMA virtual channel number
 * @return DMA channel base pointer
 */
DMA_ChannelType *DMA_DRV_GetDmaRegBaseAddr(uint32_t virtualChannel);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _DMA_DRV_H */

/* =============================================  EOF  ============================================== */
