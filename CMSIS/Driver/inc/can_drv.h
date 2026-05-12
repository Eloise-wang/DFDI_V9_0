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
 * @file can_drv.h
 *
 * @brief This file provides CAN driver definition and declaration.
 *
 */

#ifndef _CAN_DRV_H
#define _CAN_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "ckgen_drv.h"
#include "dma_drv.h"
#include "osif.h"

/* ============================================  Define  ============================================ */
#define CAN_MAX_RBUF_LENGTH           (76UL)        /*!< The number of max receive buffer bytes */
#define CAN_MAX_TBUF_LENGTH           (72UL)        /*!< The number of max transmit buffer bytes */

/*!< Enable all interrupt mask except ECC */
#define CAN_IRQ_ALL_ENABLE_MSK        (CAN_CTRL1_EIE_Msk  | CAN_CTRL1_TSIE_Msk | CAN_CTRL1_TPIE_Msk |\
                                      CAN_CTRL1_RAFIE_Msk | CAN_CTRL1_RFIE_Msk | CAN_CTRL1_ROIE_Msk |\
                                      CAN_CTRL1_RIE_Msk   | CAN_CTRL1_EPIE_Msk | CAN_CTRL1_ALIE_Msk | CAN_CTRL1_BEIE_Msk)

/* ===========================================  Typedef  ============================================ */
/*!< CAN callback function define */
typedef void (*can_callback_t)(uint8_t instance, uint32_t event, uint32_t koer);

/*!
 * @brief CAN message frame type.
 */
typedef enum
{
    CAN_MSG_DATA_FRAME = 0U,                        /*!< Data frame */
    CAN_MSG_REMOTE_FRAME                            /*!< Remote frame */
} can_msg_frame_type_t;

/*!
 * @brief CAN message ID type.
 */
typedef enum
{
    CAN_MSG_ID_STD = 0U,                            /*!< Standard ID */
    CAN_MSG_ID_EXT,                                 /*!< Extended ID */
    CAN_MSG_ID_BOTH                                 /*!< Both */
} can_msg_id_type_t;

/*!
 * @brief CAN message DLC define.
 */
typedef enum
{
    CAN_MSG_DLC_12_BYTES = 9U,                      /*!< Message DLC 12 bytes */
    CAN_MSG_DLC_16_BYTES,                           /*!< Message DLC 16 bytes */
    CAN_MSG_DLC_20_BYTES,                           /*!< Message DLC 20 bytes */
    CAN_MSG_DLC_24_BYTES,                           /*!< Message DLC 24 bytes */
    CAN_MSG_DLC_32_BYTES,                           /*!< Message DLC 32 bytes */
    CAN_MSG_DLC_48_BYTES,                           /*!< Message DLC 48 bytes */
    CAN_MSG_DLC_64_BYTES                            /*!< Message DLC 64 bytes */
} can_msg_dlc_t;

/*!
 * @brief CAN work mode.
 */
typedef enum
{
    CAN_NORMAL_MODE = 0U,                           /*!< Normal mode */
    CAN_LISTEN_ONLY_MODE,                           /*!< Listen only mode */
    CAN_LOOPBACK_INT_MODE,                          /*!< Loopback internal mode */
    CAN_LOOPBACK_EXT_MODE                           /*!< Loopback external mode */
} can_work_mode_t;

/*!
 * @brief CAN transmit buffer type.
 */
typedef enum
{
    CAN_TRANSMIT_PRIMARY = 0U,                      /*!< Transmit primary buffer */
    CAN_TRANSMIT_SECONDARY                          /*!< Transmit secondary buffer */
} can_transmit_buff_t;

/*!
 * @brief CAN transmit secondary amount.
 */
typedef enum
{
    CAN_TRANSMIT_SEC_ALL = 0U,                      /*!< Transmit secondary all messages */
    CAN_TRANSMIT_SEC_ONE                            /*!< Transmit secondary one message */
} can_transmit_sec_amount_t;

/*!
 * @brief CAN transmit secondary operation mode.
 */
typedef enum
{
    CAN_TSMODE_FIFO = 0U,                           /*!< Transmit secondary fifo mode */
    CAN_TSMODE_PRIORITY                             /*!< Transmit secondary priority mode */
} can_transmit_sec_mode_t;

/*!
 * @brief CAN transmit secondary buffer status.
 */
typedef enum
{
    CAN_TSSTAT_EMPTY = 0U,                          /*!< STB is empty */
    CAN_TSSTAT_LESS_HALF,                           /*!< STB is less than or equal to half full */
    CAN_TSSTAT_MORE_HALF,                           /*!< STB is more than half full */
    CAN_TSSTAT_FULL                                 /*!< STB is full */
} can_tsbuf_status_t;

/*!
 * @brief CAN receive buffer status.
 */
typedef enum
{
    CAN_RSTAT_EMPTY = 0U,                           /*!< RB is empty */
    CAN_RSTAT_LESS_ALMOST,                          /*!< > empty and < almost full */
    CAN_RSTAT_ALMOST_FULL,                          /*!< >= almost full but not full */
    CAN_RSTAT_FULL                                  /*!< RB is full */
} can_rbuf_status_t;

/*!
 * @brief CAN receive overflow mode.
 */
typedef enum
{
    CAN_ROM_OVER_WRITE = 0U,                        /*!< Oldest message will be overwrite */
    CAN_ROM_DISCARD                                 /*!< New message will not be store */
} can_rbuf_overflow_mode_t;

/*!
 * @brief CAN time stamp clock source.
 */
typedef enum
{
    CAN_TIME_STAMP_CLK_SRC_EXT = 0U,                /*!< Time stamp external clock */
    CAN_TIME_STAMP_CLK_SRC_BIT                      /*!< Time stamp bit time clock */
} can_time_stamp_clk_src_t;

/*!
 * @brief CAN time stamp postion.
 */
typedef enum
{
    CAN_TIME_STAMP_SOF = 0U,                        /*!< Time stamp position SOF */
    CAN_TIME_STAMP_EOF                              /*!< Time stamp position EOF */
} can_time_stamp_pos_t;

/*!
 * @brief CAN interrupt event.
 */
typedef enum
{
    CAN_EVENT_ABORT            = (1U << CAN_CTRL1_AIF_Pos),            /*!< Abort interrupt flag */
    CAN_EVENT_ERROR            = (1U << CAN_CTRL1_EIF_Pos),            /*!< Error interrupt flag */
    CAN_EVENT_TRANS_SEC_DONE   = (1U << CAN_CTRL1_TSIF_Pos),           /*!< Transmit secondary interrupt flag */
    CAN_EVENT_TRANS_PRI_DONE   = (1U << CAN_CTRL1_TPIF_Pos),           /*!< Transmit primary interrupt flag */
    CAN_EVENT_RBUF_ALMOST_FULL = (1U << CAN_CTRL1_RAFIF_Pos),          /*!< Rbuf almost full interrupt flag */
    CAN_EVENT_RBUF_FULL        = (1U << CAN_CTRL1_RFIF_Pos),           /*!< Rbuf full interrupt flag */
    CAN_EVENT_RBUF_OVERRUN     = (1U << CAN_CTRL1_ROIF_Pos),           /*!< Rbuf overrun interrupt flag */
    CAN_EVENT_RECEIVE_DONE     = (1U << CAN_CTRL1_RIF_Pos),            /*!< Receive interrupt flag */
    CAN_EVENT_BUS_ERROR        = (1U << CAN_CTRL1_BEIF_Pos),           /*!< Bus error interrupt flag */
    CAN_EVENT_ARBIT_LOST       = (1U << CAN_CTRL1_ALIF_Pos),           /*!< Arbitration lost interrupt flag */
    CAN_EVENT_ERROR_PASSIVE    = (1U << CAN_CTRL1_EPIF_Pos),           /*!< Error passive interrupt flag */
    CAN_EVENT_ECC_WARNING      = (1U << (CAN_VERMEM_MDWIF_Pos + 8U)),  /*!< Memory ECC warning interrupt flag */
    CAN_EVENT_ECC_ERROR        = (1U << (CAN_VERMEM_MDEIF_Pos + 8U)),  /*!< Memory ECC error interrupt flag */
    CAN_EVENT_WAKEUP           = 0x40000000U                           /*!< Wakeup interrupt flag */
} can_event_type_t;

/*!
 * @brief CAN kind of bus error.
 */
typedef enum
{
    CAN_KOER_NO_ERROR = 0U,                         /*!< No error */
    CAN_KOER_BIT_ERROR,                             /*!< Bit error */
    CAN_KOER_FORM_ERROR,                            /*!< Form error */
    CAN_KOER_STUFF_ERROR,                           /*!< Stuff error */
    CAN_KOER_ACK_ERROR,                             /*!< Acknowledgement error */
    CAN_KOER_CRC_ERROR,                             /*!< CRC error */
    CAN_KOER_OTHER_ERROR,                           /*!< Other error */
    CAN_KOER_RESERVE                                /*!< Reserved */
} can_bus_error_type_t;

/*!
 * @brief CAN normal bitrate define or user define.
 */
typedef enum
{
    CAN_BITRATE_1M = 0U,
    CAN_BITRATE_800K,
    CAN_BITRATE_500K,
    CAN_BITRATE_250K,
    CAN_BITRATE_125K,
    CAN_BITRATE_100K,
    CAN_BITRATE_50K,
    CAN_BITRATE_NUM
} can_bitrate_t;                                    /*!< CAN normal bitrate */

/*!
 * @brief CAN FD data bitrate define or user define.
 */
typedef enum
{
    CAN_FD_BITRATE_50K = 0U,
    CAN_FD_BITRATE_100K,
    CAN_FD_BITRATE_1M,
    CAN_FD_BITRATE_2M,
    CAN_FD_BITRATE_4M,
    CAN_FD_BITRATE_6M,
    CAN_FD_BITRATE_8M,
    CAN_FD_BITRATE_NUM
} can_fd_bitrate_t;                                 /*!< CAN FD data bitrate */

/*!
 * @brief CAN message information.
 */
typedef struct can_msg_info_s
{
    uint32_t ID;                                    /*!< CAN identifier */
    uint32_t RTS;                                   /*!< Receive time stamp */
    uint8_t  ESI;                                   /*!< Error state indicator */
    uint8_t  DLC;                                   /*!< Data length code */
    uint8_t  BRS;                                   /*!< Bit rate switch */
    uint8_t  FDF;                                   /*!< FD format indicator */
    uint8_t  RTR;                                   /*!< Remote transmission request */
    uint8_t  IDE;                                   /*!< Identifier extension */
    uint8_t  *DATA;                                 /*!< Data */
} can_msg_info_t;

/*!
 * @brief CAN bitrate time segment struct.
 * Bitrate = CAN_CLOCK / ((PRESC + 1) * (1 + (SEG_1 + 1) + (SEG_2 + 1)))
 * Sample point = (1 + (SEG_1 + 1)) / (1 + (SEG_1 + 1) + (SEG_2 + 1))
 */
typedef struct
{
    uint8_t SEG_1;                                  /*!< Bit timing segment 1 */
    uint8_t SEG_2;                                  /*!< Bit timing segment 2 */
    uint8_t SJW;                                    /*!< Synchronization jump width */
    uint8_t PRESC;                                  /*!< Prescaler */
} can_time_segment_t;

/*!
 * @brief CAN filter configuration struct.
 */
typedef struct
{
    uint32_t code;                                  /*!< Code data 29bit */
    uint32_t mask;                                  /*!< Mask data 29bit */
    can_msg_id_type_t idType;                       /*!< Mask id type */
} can_filter_config_t;

/*!
 * @brief CAN configuration struct.
 */
typedef struct
{
    uint8_t filterNum;                              /*!< Set filter number */
    can_work_mode_t canMode;                        /*!< CAN mode */
    bool interruptEn;                               /*!< Interrupt enable */
    can_transmit_sec_mode_t tsMode;                 /*!< Transmit secondary operation mode */
    can_transmit_sec_amount_t tsAmount;             /*!< Transmit secondary amount */
    bool tpss;                                      /*!< Transmission primary single shot mode for PTB */
    bool tsss;                                      /*!< Transmission secondary single shot mode for STB */
    can_rbuf_overflow_mode_t rom;                   /*!< Receive buffer overflow mode */
    bool selfAckEn;                                 /*!< Self-acknowledge enable for LBME */
    uint8_t errorWarningLimit;                      /*!< Error warning limit */
    bool fdModeEn;                                  /*!< FD mode enable */
    bool fdIsoEn;                                   /*!< FD ISO mode enable */
    bool tdcEn;                                     /*!< TDC enable (when fd bitrate >= 1Mbps) */
    uint8_t sspOffset;                              /*!< SSP offset */
    bool timeStampEn;                               /*!< Time stamp enable */
    can_time_stamp_clk_src_t timeStampClkSrc;       /*!< Time stamp clock source */
    can_time_stamp_clk_div_t timeStampClkDiv;       /*!< Time stamp external clock divider */
    can_time_stamp_pos_t timeStampPos;              /*!< Time stamp position */
    bool dmaRecvEn;                                 /*!< DMA receive enable */
    bool memEccEn;                                  /*!< CAN memory ECC enable */
    bool wakeupIntEn;                               /*!< Wakeup interrupt enable */
    bool lowPassFilterEn;                           /*!< Low pass filter enable */
    bool busOffRecDisable;                          /*!< Auto bus off recovery disable */
    uint32_t interruptMask;                         /*!< Interrupt enable mask */
    uint32_t memEccIntMask;                         /*!< CAN memory ECC interrupt enable mask */
    can_time_segment_t bitrate;                     /*!< Normal bitrate setting */
    can_time_segment_t fdBitrate;                   /*!< FD data bitrate setting */
    can_filter_config_t *filterList;                /*!< Filter controller list */
    can_callback_t callback;                        /*!< CAN callback function pointer */
    can_callback_t wakeupCallback;                  /*!< CAN wakeup callback function pointer */
} can_user_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Get CAN module base address.
 *
 * @param[in] instance: CAN module instance
 * @return CAN module base address
 */
CAN_Type* CAN_DRV_GetBase(uint8_t instance);

/*!
 * @brief Set the CAN bitrate.
 *
 * @param[in] instance: CAN module instance
 * @param[in] bitrate: point to bitrate configuration
 * @return none
 */
void CAN_DRV_SetBitrate(uint8_t instance, const can_time_segment_t *bitrate);

/*!
 * @brief Get the CAN bitrate.
 *
 * @param[in] instance: CAN module instance
 * @param[out] bitrate: point to bitrate configuration
 * @return none
 */
void CAN_DRV_GetBitrate(uint8_t instance, can_time_segment_t *bitrate);

/*!
 * @brief Set the CAN FD data bitrate.
 *
 * @param[in] instance: CAN module instance
 * @param[in] bitrate: point to bitrate configuration
 * @return none
 */
void CAN_DRV_SetBitrateFD(uint8_t instance, const can_time_segment_t *bitrate);

/*!
 * @brief Get the CAN FD data bitrate.
 *
 * @param[in] instance: CAN module instance
 * @param[out] bitrate: point to bitrate configuration
 * @return none
 */
void CAN_DRV_GetBitrateFD(uint8_t instance, can_time_segment_t *bitrate);

/*!
 * @brief Set the CAN FD TDC and SSP offset.
 *
 * @param[in] instance: CAN module instance
 * @param[in] enable: TDC enable
 * @param[in] offset: SSP offset
 * @return none
 */
void CAN_DRV_SetTDCOffset(uint8_t instance, bool enable, uint8_t offset);

/*!
 * @brief Set the CAN filters (including code and mask).
 *
 * @param[in] instance: CAN module instance
 * @param[in] index: CAN filter index (0-59)
 * @param[in] enable: filter enable state
 * @param[in] config: point to filter configuration
 * @return none
 */
void CAN_DRV_SetFilter(uint8_t instance, uint8_t index, bool enable, const can_filter_config_t *config);

/*!
 * @brief Install event callback function.
 *
 * @param[in] instance: CAN module instance
 * @param[in] callback: call back function pointer
 * @return none
 */
void CAN_DRV_InstallEventCallback(uint8_t instance, const can_callback_t callback);

/*!
 * @brief Install wakeup callback function.
 *
 * @param[in] instance: CAN module instance
 * @param[in] callback: wakeup call back function pointer
 * @return none
 */
void CAN_DRV_InstallWakeupCallback(uint8_t instance, const can_callback_t callback);

/*!
 * @brief Set software reset state.
 *
 * @param[in] instance: CAN module instance
 * @param[in] enable: reset state
 * @return none
 */
void CAN_DRV_SetReset(uint8_t instance, bool enable);

/*!
 * @brief Initialize the CAN driver (including clock, bitrate, filter and interrupt settings).
 *
 * @param[in] instance: CAN module instance
 * @param[in] config: point to CAN configuration
 * @return initialize status
 *         - STATUS_SUCCESS: initialize successfully
 *         - STATUS_ERROR: initialize error
 */
status_t CAN_DRV_Init(uint8_t instance, const can_user_config_t *config);

/*!
 * @brief Deinitialize the CAN module.
 *
 * @param[in] instance: CAN module instance
 * @return deinitialize status
 *         - STATUS_SUCCESS: deinitialize successfully
 *         - STATUS_ERROR: deinitialize error
 */
status_t CAN_DRV_Deinit(uint8_t instance);

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
                             uint32_t destAddr, uint8_t size, dma_callback_t callback);

/*!
 * @brief Start next DMA receive.
 *
 * @param[in] dmaChannel: DMA Channel instance
 * @param[in] destAddr: destination address
 * @param[in] size: total number of bytes (4-byte times)
 * @return none
 */
void CAN_DRV_StartNextDMA(uint8_t dmaChannel, uint32_t destAddr, uint8_t size);

/*!
 * @brief Get CAN default configuration.
 *
 * @param[out] config: point to CAN configuration
 * @return none
 */
void CAN_DRV_GetDefaultConfig(can_user_config_t *config);

/*!
 * @brief Set TBUF directly.
 *
 * @param[in] instance: CAN module instance
 * @param[in] txBuf: pointer to transmission sram buffer
 * @param[in] length: number of buffer bytes
 * @param[in] type: transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return none
 */
void CAN_DRV_SetTbuf(uint8_t instance, const uint8_t *txBuf, uint8_t length, can_transmit_buff_t type);

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
void CAN_DRV_SetMsgToTbuf(uint8_t instance, const can_msg_info_t *info, can_transmit_buff_t type);

/*!
 * @brief Set amount for transmit secondary buffer.
 *
 * @param[in] instance: CAN module instance
 * @param[in] amount: transmit secondary buffer amount
 *            - CAN_TRANSMIT_SEC_ALL
 *            - CAN_TRANSMIT_SEC_ONE
 * @return none
 */
void CAN_DRV_SetTransSecAmount(uint8_t instance, can_transmit_sec_amount_t amount);

/*!
 * @brief Get transmit secondary buffer status.
 *
 * @param[in] instance: CAN module instance
 * @return transmit secondary buffer status
 */
can_tsbuf_status_t CAN_DRV_GetTsbufStatus(uint8_t instance);

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
bool CAN_DRV_IsTransmitBusy(uint8_t instance, can_transmit_buff_t type);

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
bool CAN_DRV_IsTransmitting(uint8_t instance, can_transmit_buff_t type);

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
status_t CAN_DRV_Send(uint8_t instance, const can_msg_info_t *info, can_transmit_buff_t type);

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
status_t CAN_DRV_SendBlocking(uint8_t instance, const can_msg_info_t *info, can_transmit_buff_t type, uint32_t timeout);

/*!
 * @brief Get receive buffer status.
 *
 * @param[in] instance: CAN module instance
 * @return receive buffer status
 */
can_rbuf_status_t CAN_DRV_GetRbufStatus(uint8_t instance);

/*!
 * @brief Get RBUF directly.
 *
 * @param[in] instance: CAN module instance
 * @param[out] rxBuf: pointer to receive sram buffer
 * @param[in] length: number of buffer bytes
 * @return get receive buffer status
 *         - STATUS_SUCCESS: get receive buffer successfully
 *         - STATUS_ERROR: get receive buffer error
 */
status_t CAN_DRV_GetRbuf(uint8_t instance, uint8_t *rxBuf, uint8_t length);

/*!
 * @brief Get the CAN message information from buffer.
 *
 * @param[out] info: pointer to message information
 * @param[in] buf: pointer to receive buffer
 * @return none
 */
void CAN_DRV_GetMsgFromRbuf(can_msg_info_t *info, const CAN_BufType *buf);

/*!
 * @brief Receive a CAN frame.
 *
 * @param[in] instance: CAN module instance
 * @param[out] info: pointer to message information
 * @return receive status
 *         - STATUS_SUCCESS: receive successfully
 *         - STATUS_ERROR: receive error
 */
status_t CAN_DRV_Receive(uint8_t instance, can_msg_info_t *info);

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
status_t CAN_DRV_ReceiveBlocking(uint8_t instance, can_msg_info_t *info, uint32_t timeout);

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
status_t CAN_DRV_AbortTransfer(uint8_t instance, can_transmit_buff_t type);

/*!
 * @brief Enter or exit transceiver standby mode.
 *
 * @param[in] instance: CAN module instance
 * @param[in] enable: standby enable state
 * @return set standby status
 *         - STATUS_SUCCESS: set standby successfully
 *         - STATUS_ERROR: set standby error
 */
status_t CAN_DRV_SetStandby(uint8_t instance, bool enable);

/*!
 * @brief Get interrupt flags.
 *
 * @param[in] instance: CAN module instance
 * @return interrupt flag
 */
uint32_t CAN_DRV_GetInterruptFlag(uint8_t instance);

/*!
 * @brief Clear interrupt flags.
 *
 * @param[in] instance: CAN moduleinstance
 * @return none
 */
void CAN_DRV_ClearInterruptFlag(uint8_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAN_DRV_H */

/* =============================================  EOF  ============================================== */
