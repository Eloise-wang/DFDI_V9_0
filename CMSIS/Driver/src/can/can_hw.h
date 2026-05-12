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
 * @file can_hw.h
 *
 * @brief This file provides CAN hardware definition and declaration.
 *
 */

#ifndef _CAN_HW_H
#define _CAN_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "can_drv.h"

/* ============================================  Define  ============================================ */
#define CAN_MAX_S_SEG_1    (0xFFUL)        /*!< CAN max slow bitrate seg 1 */
#define CAN_MAX_S_SEG_2    (0x7FUL)        /*!< CAN max slow bitrate seg 2 */
#define CAN_MAX_S_SJW      (0x7FUL)        /*!< CAN max slow bitrate sjw */
#define CAN_MAX_S_PRESC    (0xFFUL)        /*!< CAN max slow bitrate prescaler */
#define CAN_MAX_F_SEG_1    (0x1FUL)        /*!< CAN max fast bitrate seg 1 */
#define CAN_MAX_F_SEG_2    (0xFUL)         /*!< CAN max fast bitrate seg 2 */
#define CAN_MAX_F_SJW      (0xFUL)         /*!< CAN max fast bitrate sjw */
#define CAN_MAX_F_PRESC    (0xFFUL)        /*!< CAN max fast bitrate prescaler */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Write TBUF.
 *
 * @param[in] base: CAN module
 * @param[in] txBuf: pointer to transmission buffer
 * @param[in] length: number of buffer bytes
 * @return none
 */
void CAN_WriteTbuf(CAN_Type *base, const uint8_t *txBuf, uint8_t length);

/*!
 * @brief Write TBUF message data.
 *
 * @param[in] base: CAN module
 * @param[in] txData: pointer to transmission data
 * @param[in] length: number of message data bytes
 * @return none
 */
void CAN_WriteTbufData(CAN_Type *base, const uint8_t *txData, uint8_t length);

/*!
 * @brief Read RBUF.
 *
 * @param[in] base: CAN module
 * @param[out] rxBuf: pointer to receive buffer
 * @param[in] length: number of buffer bytes
 * @return none
 */
void CAN_ReadRbuf(CAN_Type *base, uint8_t *rxBuf, uint8_t length);

/*!
 * @brief Read RBUF message data.
 *
 * @param[in] base: CAN module
 * @param[out] rxData: pointer to receive data
 * @param[in] length: number of message data bytes
 * @return none
 */
void CAN_ReadRbufData(CAN_Type *base, uint8_t *rxData, uint8_t length);

/*!
 * @brief Compute message payload size.
 *
 * @param[in] dlcValue: DLC value
 * @return Payload size
 */
uint8_t CAN_ComputePayloadSize(uint8_t dlcValue);

/*!
 * @brief Compute message DLC value.
 *
 * @param[in] payloadSize: payload size
 * @return DLC value
 */
uint8_t CAN_ComputeDlcValue(uint8_t payloadSize);

/*!
 * @brief Start transmit frame in the TBUF.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @param[in] amount: transmit secondary all or one frames
 *            - CAN_TRANSMIT_SEC_ALL
 *            - CAN_TRANSMIT_SEC_ONE
 * @return none
 */
void CAN_StartTransmit(CAN_Type *base, can_transmit_buff_t type, can_transmit_sec_amount_t amount);

/*!
 * @brief Abort transmitting.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return none
 */
void CAN_AbortTransmit(CAN_Type *base, can_transmit_buff_t type);

/*!
 * @brief Set acceptance filter state.
 *
 * @param[in] base: CAN module
 * @param[in] index: filter index
 * @param[in] enable: filter enable state
 * @return none
 */
void CAN_SetFilterEnable(CAN_Type *base, uint8_t index, bool enable);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Check transmit is busy.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return transmit busy flag
 *         - false: not busy or STB is not full
 *         - true: is busy or STB is full
 */
static inline bool CAN_IsTransmitBusy(CAN_Type *base, can_transmit_buff_t type)
{
    if (CAN_TRANSMIT_PRIMARY == type)
    {
        return (((base->CTRL0 & CAN_CTRL0_TPE_Msk) >> CAN_CTRL0_TPE_Pos) != 0U);
    }
    else
    {
        return (bool)((uint32_t)CAN_TSSTAT_FULL == ((base->CTRL0 & CAN_CTRL0_TSSTAT_Msk) >> CAN_CTRL0_TSSTAT_Pos));
    }
}

/*!
 * @brief Check whether is transmitting.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return transmitting status
 *         - false: not transmitting
 *         - true: is transmitting
 */
static inline bool CAN_IsTransmitting(CAN_Type *base, can_transmit_buff_t type)
{
    if (CAN_TRANSMIT_PRIMARY == type)
    {
        return (((base->CTRL0 & CAN_CTRL0_TPE_Msk) >> CAN_CTRL0_TPE_Pos) != 0U);
    }
    else
    {
        return ((base->CTRL0 & (CAN_CTRL0_TSALL_Msk | CAN_CTRL0_TSONE_Msk)) != 0U);
    }
}

/*!
 * @brief Check transmission complete flag.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return Transmit status
 */
static inline bool CAN_IsTransmitDone(CAN_Type *base, can_transmit_buff_t type)
{
    if (CAN_TRANSMIT_PRIMARY == type)
    {
        return (((base->CTRL1 & CAN_CTRL1_TPIF_Msk) >> CAN_CTRL1_TPIF_Pos) != 0U);
    }
    else
    {
        return (((base->CTRL1 & CAN_CTRL1_TSIF_Msk) >> CAN_CTRL1_TSIF_Pos) != 0U);
    }
}

/*!
 * @brief Clear transmission complete flag.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *            - CAN_TRANSMIT_PRIMARY
 *            - CAN_TRANSMIT_SECONDARY
 * @return none
 */
static inline void CAN_ClearTransmitFlag(CAN_Type *base, can_transmit_buff_t type)
{
    if (CAN_TRANSMIT_PRIMARY == type)
    {
        base->CTRL1 |= CAN_CTRL1_TPIF_Msk;
    }
    else
    {
        base->CTRL1 |= CAN_CTRL1_TSIF_Msk;
    }
}

/*!
 * @brief Get RBUF message ID.
 *
 * @param[in] base: CAN module
 * @return Message id
 */
static inline uint32_t CAN_GetRbufId(CAN_Type *base)
{
    return (base->RBUF.ID_ESI);
}

/*!
 * @brief Set RBUF message control.
 *
 * @param[in] base: CAN module
 * @return Message control
 */
static inline uint32_t CAN_GetRbufCtrl(CAN_Type *base)
{
    return (base->RBUF.CTRL);
}

/*!
 * @brief Set TBUF message ID.
 *
 * @param[in] base: CAN module
 * @param[in] id: message id
 * @return none
 */
static inline void CAN_SetTbufId(CAN_Type *base, uint32_t id)
{
    base->TBUF.ID_ESI = id;
}

/*!
 * @brief Set TBUF message control.
 *
 * @param[in] base: CAN module
 * @param[in] ctrl: message control
 * @return none
 */
static inline void CAN_SetTbufCtrl(CAN_Type *base, uint32_t ctrl)
{
    base->TBUF.CTRL = ctrl;
}

/*!
 * @brief Get transmission time stamp value.
 *
 * @param[in] base: CAN module
 * @return Transmission time stamp value
 */
static inline uint32_t CAN_GetTransTimeStamp(CAN_Type *base)
{
    return (base->TBUF.TS[0U]);
}

/*!
 * @brief Check bus off status.
 *
 * @param[in] base: CAN module
 * @return Bus off status
 */
static inline bool CAN_IsBusOff(CAN_Type *base)
{
    return ((base->CTRL0 & CAN_CTRL0_BUSOFF_Msk) != 0U);
}

/*!
 * @brief Forced exit from bus off status.
 *
 * @param[in] base: CAN module
 * @return none
 */
static inline void CAN_ExitBusOff(CAN_Type *base)
{
    base->CTRL0 &= ~CAN_CTRL0_BUSOFF_Msk;
}

/*!
 * @brief Check transmit active status.
 *
 * @param[in] base: CAN module
 * @return Tactive status
 */
static inline bool CAN_IsTransActive(CAN_Type *base)
{
    return (((base->CTRL0 & CAN_CTRL0_TACTIVE_Msk) >> CAN_CTRL0_TACTIVE_Pos) != 0U);
}

/*!
 * @brief Check receive active status.
 *
 * @param[in] base: CAN module
 * @return Ractive status
 */
static inline bool CAN_IsRecvActive(CAN_Type *base)
{
    return (((base->CTRL0 & CAN_CTRL0_RACTIVE_Msk) >> CAN_CTRL0_RACTIVE_Pos) != 0U);
}

/*!
 * @brief Set transmission secondary single shot mode.
 *
 * @param[in] base: CAN module
 * @param[in] enable: tpss mode
 * @return none
 */
static inline void CAN_SetTsss(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_TSSS_Msk, CAN_CTRL0_TSSS_Pos, enable);
}

/*!
 * @brief Set transmission primary single shot mode.
 *
 * @param[in] base: CAN module
 * @param[in] enable: tpss mode
 * @return none
 */
static inline void CAN_SetTpss(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_TPSS_Msk, CAN_CTRL0_TPSS_Pos, enable);
}

/*!
 * @brief Set loopback internal mode.
 *
 * @param[in] base: CAN module
 * @param[in] enable: LBMI mode
 * @return none
 */
static inline void CAN_SetLoopbackIntMode(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_LBMI_Msk, CAN_CTRL0_LBMI_Pos, enable);
}

/*!
 * @brief Set loopback external mode.
 *
 * @param[in] base: CAN module
 * @param[in] enable: LBME mode
 * @return none
 */
static inline void CAN_SetLoopbackExtMode(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_LBME_Msk, CAN_CTRL0_LBME_Pos, enable);
}

/*!
 * @brief Set software reset state.
 *
 * @param[in] base: CAN module
 * @param[in] enable: reset state
 * @return none
 */
static inline void CAN_SetReset(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_RESET_Msk, CAN_CTRL0_RESET_Pos, enable);
}

/*!
 * @brief Get software reset state.
 *
 * @param[in] base: CAN module
 * @return Reset state
 */
static inline bool CAN_GetResetState(CAN_Type *base)
{
    return (((base->CTRL0 & CAN_CTRL0_RESET_Msk) >> CAN_CTRL0_RESET_Pos) != 0U);
}

/*!
 * @brief Set transceiver standby state.
 *
 * @param[in] base: CAN module
 * @param[in] enable: standby enable state
 * @return none
 */
static inline void CAN_SetStandby(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_STBY_Msk, CAN_CTRL0_STBY_Pos, enable);
}

/*!
 * @brief Get transceiver standby state.
 *
 * @param[in] base: CAN module
 * @return Standby state
 */
static inline bool CAN_GetStandbyState(CAN_Type *base)
{
    return (((base->CTRL0 & CAN_CTRL0_STBY_Msk) >> CAN_CTRL0_STBY_Pos) != 0U);
}

/*!
 * @brief Set listen only mode.
 *
 * @param[in] base: CAN module
 * @param[in] enable: LOM mode
 * @return none
 */
static inline void CAN_SetListenOnlyMode(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_LOM_Msk, CAN_CTRL0_LOM_Pos, enable);
}

/*!
 * @brief Set transmit buffer select.
 *
 * @param[in] base: CAN module
 * @param[in] type: CAN transmit buffer type
 *              - CAN_TRANSMIT_PRIMARY
 *              - CAN_TRANSMIT_SECONDARY
 * @return none
 */
static inline void CAN_SetTbufSelect(CAN_Type *base, can_transmit_buff_t type)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_TBSEL_Msk, CAN_CTRL0_TBSEL_Pos, (uint32_t)type);
}

/*!
 * @brief Get transmit secondary buffer status.
 *
 * @param[in] base: CAN module
 * @return Transmit secondary buffer status
 */
static inline uint8_t CAN_GetTsbufStatus(CAN_Type *base)
{
    return (uint8_t)((base->CTRL0 & CAN_CTRL0_TSSTAT_Msk) >> CAN_CTRL0_TSSTAT_Pos);
}

/*!
 * @brief Check CAN bus idle status.
 *
 * @param[in] base: CAN module
 * @return Bus idle status
 */
static inline bool CAN_IsIdle(CAN_Type *base)
{
    return (((base->CTRL0 & CAN_CTRL0_IDLE_Msk) >> CAN_CTRL0_IDLE_Pos) != 0U);
}

/*!
 * @brief Set automatic recovering from bus off disable state.
 *
 * @param[in] base: CAN module
 * @param[in] enable: bus off recovery disable state
 * @return none
 */
static inline void CAN_SetBusOffRecDisable(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_BOREC_Msk, CAN_CTRL0_BOREC_Pos, enable);
}

/*!
 * @brief Set transmit secondary operation mode.
 *
 * @param[in] base: CAN module
 * @param[in] mode: Transmit secondary operation mode
 *              - CAN_TSMODE_FIFO
 *              - CAN_TSMODE_PRIORITY
 * @return none
 */
static inline void CAN_SetTransSecMode(CAN_Type *base, can_transmit_sec_mode_t mode)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_TSMODE_Msk, CAN_CTRL0_TSMODE_Pos, (uint32_t)mode);
}

/*!
 * @brief Select next transmit secondary buffer.
 *
 * @param[in] base: CAN module
 * @return none
 */
static inline void CAN_SelectNextTsbuf(CAN_Type *base)
{
    base->CTRL0 |= CAN_CTRL0_TSNEXT_Msk;
}

/*!
 * @brief Set CANFD ISO mode.
 *
 * @param[in] base: CAN module
 * @param[in] enable: CAN FD ISO mode
 * @return none
 */
static inline void CAN_SetFdIso(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_FDISO_Msk, CAN_CTRL0_FDISO_Pos, enable);
}

/*!
 * @brief Get receive buffer status.
 *
 * @param[in] base: CAN module
 * @return Receive buffer status
 */
static inline uint8_t CAN_GetRbufStatus(CAN_Type *base)
{
    return (uint8_t)((base->CTRL0 & CAN_CTRL0_RSTAT_Msk) >> CAN_CTRL0_RSTAT_Pos);
}

/*!
 * @brief Set DMA receive enable.
 *
 * @param[in] base: CAN module
 * @param[in] enable: DMA receive enable state
 * @return none
 */
static inline void CAN_SetDmaRecv(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_DREN_Msk, CAN_CTRL0_DREN_Pos, enable);
}

/*!
 * @brief Set receive buffer store all data frames.
 *
 * @param[in] base: CAN module
 * @param[in] enable: receive buffer store all data frames state
 * @return none
 */
static inline void CAN_SetRball(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_RBALL_Msk, CAN_CTRL0_RBALL_Pos, enable);
}

/*!
 * @brief Release receive buffer.
 *
 * @param[in] base: CAN module
 * @return none
 */
static inline void CAN_ReleaseRbuf(CAN_Type *base)
{
    base->CTRL0 |= CAN_CTRL0_RREL_Msk;
}

/*!
 * @brief Get receive buffer overflow flag.
 *
 * @param[in] base: CAN module
 * @return Receive buffer overflow flag
 */
static inline uint8_t CAN_GetRbufOverflow(CAN_Type *base)
{
    return (uint8_t)((base->CTRL0 & CAN_CTRL0_ROV_Msk) >> CAN_CTRL0_ROV_Pos);
}

/*!
 * @brief Set receive buffer overflow mode.
 *
 * @param[in] base: CAN module
 * @param[in] mode: receive buffer overflow mode
 *              - CAN_ROM_OVER_WRITE
 *              - CAN_ROM_DISCARD
 * @return none
 */
static inline void CAN_SetRbufOverflowMode(CAN_Type *base, can_rbuf_overflow_mode_t mode)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_ROM_Msk, CAN_CTRL0_ROM_Pos, (uint32_t)mode);
}

/*!
 * @brief Set self acknowledge when LBME = 1.
 *
 * @param[in] base: CAN module
 * @param[in] enable: sack enable state
 * @return none
 */
static inline void CAN_SetSelfAck(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL0, CAN_CTRL0_SACK_Msk, CAN_CTRL0_SACK_Pos, enable);
}

/*!
 * @brief Set controller 1 register.
 *
 * @param[in] base: CAN module
 * @param[in] value: CAN controller 1 register value
 * @return none
 */
static inline void CAN_SetCtrl1(CAN_Type *base, uint32_t value)
{
    base->CTRL1 = value;
}

/*!
 * @brief Get CAN controller 1 register.
 *
 * @param[in] base: CAN module
 * @return Controller 1 register value
 */
static inline uint32_t CAN_GetCtrl1(CAN_Type *base)
{
    return base->CTRL1;
}

/*!
 * @brief Get transmit secondary buffer full flag.
 *
 * @param[in] base: CAN module
 * @return Transmit secondary buffer full flag
 */
static inline bool CAN_GetTsbufFullFlag(CAN_Type *base)
{
    return (((base->CTRL1 & CAN_CTRL1_TSFF_Msk) >> CAN_CTRL1_TSFF_Pos) != 0U);
}

/*!
 * @brief Set interrupt enable mask.
 *
 * @param[in] base: CAN module
 * @param[in] intMask: interrupt enable mask
 * @return none
 */
static inline void CAN_SetIntMask(CAN_Type *base, uint32_t intMask)
{
    MODIFY_REG32(base->CTRL1, CAN_IRQ_ALL_ENABLE_MSK, 0, intMask);
}

/*!
 * @brief Get error passive flag.
 *
 * @param[in] base: CAN module
 * @return Error passive flag
 */
static inline bool CAN_GetErrorPassiveFlag(CAN_Type *base)
{
    return (((base->CTRL1 & CAN_CTRL1_EPASS_Msk) >> CAN_CTRL1_EPASS_Pos) != 0U);
}

/*!
 * @brief Get error warning limit flag.
 *
 * @param[in] base: CAN module
 * @return Error warning limit flag
 */
static inline bool CAN_GetErrorWarningFlag(CAN_Type *base)
{
    return (((base->CTRL1 & CAN_CTRL1_EWARN_Msk) >> CAN_CTRL1_EWARN_Pos) != 0U);
}

/*!
 * @brief Set error warning limit.
 *
 * @param[in] base: CAN module
 * @param[in] ewl: error warning limit (0-15)
 * @return none
 */
static inline void CAN_SetErrorWarningLimit(CAN_Type *base, uint8_t ewl)
{
    MODIFY_REG32(base->CTRL1, CAN_CTRL1_EWL_Msk, CAN_CTRL1_EWL_Pos, ewl);
}

/*!
 * @brief Set Rbuf almost full warning limit.
 *
 * @param[in] base: CAN module
 * @param[in] afwl: almost full warning limit (1 - 12)
 * @return none
 */
static inline void CAN_SetAfwl(CAN_Type *base, uint8_t afwl)
{
    MODIFY_REG32(base->CTRL1, CAN_CTRL1_AFWL_Msk, CAN_CTRL1_AFWL_Pos, afwl);
}

/*!
 * @brief Set CAN normal slow bitrate.
 *
 * @param[in] base: CAN module
 * @param[in] bitrate: pointer to bitrate configuration
 * @return none
 */
static inline void CAN_SetSlowBitrate(CAN_Type *base, const can_time_segment_t *bitrate)
{
    DEVICE_ASSERT(bitrate != NULL);
    DEVICE_ASSERT(bitrate->SEG_2 <= CAN_MAX_S_SEG_2);
    DEVICE_ASSERT(bitrate->SJW <= CAN_MAX_S_SJW);

    base->SBITRATE = *(uint32_t *)bitrate;
}

/*!
 * @brief Get CAN normal slow bitrate.
 *
 * @param[in] base: CAN module
 * @param[in] bitrate: pointer to bitrate configuration
 * @return none
 */
static inline void CAN_GetSlowBitrate(CAN_Type *base, can_time_segment_t *bitrate)
{
    DEVICE_ASSERT(bitrate != NULL);

    bitrate->SEG_1 = (uint8_t)((base->SBITRATE & CAN_SBITRATE_S_SEG_1_Msk) >> CAN_SBITRATE_S_SEG_1_Pos);
    bitrate->SEG_2 = (uint8_t)((base->SBITRATE & CAN_SBITRATE_S_SEG_2_Msk) >> CAN_SBITRATE_S_SEG_2_Pos);
    bitrate->SJW =   (uint8_t)((base->SBITRATE & CAN_SBITRATE_S_SJW_Msk) >> CAN_SBITRATE_S_SJW_Pos);
    bitrate->PRESC = (uint8_t)((base->SBITRATE & CAN_SBITRATE_S_PRESC_Msk) >> CAN_SBITRATE_S_PRESC_Pos);
}

/*!
 * @brief Set CAN FD data bitrate.
 *
 * @param[in] base: CAN module
 * @param[in] bitrate: pointer to bitrate configuration
 * @return none
 */
static inline void CAN_SetFastBitrate(CAN_Type *base, const can_time_segment_t *bitrate)
{
    DEVICE_ASSERT(bitrate != NULL);
    DEVICE_ASSERT(bitrate->SEG_1 <= CAN_MAX_F_SEG_1);
    DEVICE_ASSERT(bitrate->SEG_2 <= CAN_MAX_F_SEG_2);
    DEVICE_ASSERT(bitrate->SJW <= CAN_MAX_F_SJW);

    base->FBITRATE = *(uint32_t *)bitrate;
}

/*!
 * @brief Get CAN FD data bitrate.
 *
 * @param[in] base: CAN module
 * @param[in] bitrate: pointer to bitrate configuration
 * @return none
 */
static inline void CAN_GetFastBitrate(CAN_Type *base, can_time_segment_t *bitrate)
{
    DEVICE_ASSERT(bitrate != NULL);

    bitrate->SEG_1 = (uint8_t)((base->FBITRATE & CAN_FBITRATE_F_SEG_1_Msk) >> CAN_FBITRATE_F_SEG_1_Pos);
    bitrate->SEG_2 = (uint8_t)((base->FBITRATE & CAN_FBITRATE_F_SEG_2_Msk) >> CAN_FBITRATE_F_SEG_2_Pos);
    bitrate->SJW =   (uint8_t)((base->FBITRATE & CAN_FBITRATE_F_SJW_Msk) >> CAN_FBITRATE_F_SJW_Pos);
    bitrate->PRESC = (uint8_t)((base->FBITRATE & CAN_FBITRATE_F_PRESC_Msk) >> CAN_FBITRATE_F_PRESC_Pos);
}

/*!
 * @brief Get arbitration lost capture.
 *
 * @param[in] base: CAN module
 * @return Arbitration lost capture position
 */
static inline uint8_t CAN_GetArbitLostCap(CAN_Type *base)
{
    return (uint8_t)((base->ERRINFO & CAN_ERRINFO_ALC_Msk) >> CAN_ERRINFO_ALC_Pos);
}

/*!
 * @brief Get kind of bus error.
 *
 * @param[in] base: CAN module
 * @return Kind of bus error
 */
static inline uint8_t CAN_GetKoer(CAN_Type *base)
{
    return (uint8_t)((base->ERRINFO & CAN_ERRINFO_KOER_Msk) >> CAN_ERRINFO_KOER_Pos);
}

/*!
 * @brief Set SSP offset.
 *
 * @param[in] base: CAN module
 * @param[in] offset: SSP offset (1-0x7f)
 * @return none
 */
static inline void CAN_SetSspOffset(CAN_Type *base, uint8_t offset)
{
    MODIFY_REG32(base->ERRINFO, CAN_ERRINFO_SSPOFF_Msk, CAN_ERRINFO_SSPOFF_Pos, offset);
}

/*!
 * @brief Set TDC enable.
 *
 * @param[in] base: CAN module
 * @param[in] enable: TDC enable state
 * @return none
 */
static inline void CAN_SetTdc(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->ERRINFO, CAN_ERRINFO_TDCEN_Msk, CAN_ERRINFO_TDCEN_Pos, enable);
}

/*!
 * @brief Get receive error count.
 *
 * @param[in] base: CAN module
 * @return Receive error count
 */
static inline uint8_t CAN_GetReCount(CAN_Type *base)
{
    return (uint8_t)((base->ERRINFO & CAN_ERRINFO_RECNT_Msk) >> CAN_ERRINFO_RECNT_Pos);
}

/*!
 * @brief Get transmit error count.
 *
 * @param[in] base: CAN module
 * @return Transmit error count
 */
static inline uint8_t CAN_GetTeCount(CAN_Type *base)
{
    return (uint8_t)((base->ERRINFO & CAN_ERRINFO_TECNT_Msk) >> CAN_ERRINFO_TECNT_Pos);
}

/*!
 * @brief Set time stamp enable.
 *
 * @param[in] base: CAN module
 * @param[in] enable: time stamp enable state
 * @return none
 */
static inline void CAN_SetTimeStamp(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->ACFCTRL0, CAN_ACFCTRL0_TIMEEN_Msk, CAN_ACFCTRL0_TIMEEN_Pos, enable);
}

/*!
 * @brief Set time stamp position.
 *
 * @param[in] base: CAN module
 * @param[in] pos: time stamp position
 *              - CAN_TIME_STAMP_SOF
 *              - CAN_TIME_STAMP_EOF
 * @return none
 */
static inline void CAN_SetTimeStampPos(CAN_Type *base, can_time_stamp_pos_t pos)
{
    MODIFY_REG32(base->ACFCTRL0, CAN_ACFCTRL0_TIMEPOS_Msk, CAN_ACFCTRL0_TIMEPOS_Pos, (uint32_t)pos);
}

/*!
 * @brief Set time stamp counter enable.
 *
 * @param[in] base: CAN module
 * @param[in] enable: time stamp counter enable state
 * @return none
 */
static inline void CAN_SetTimeStampCount(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->ACFCTRL0, CAN_ACFCTRL0_TCEN_Msk, CAN_ACFCTRL0_TCEN_Pos, enable);
}

/*!
 * @brief Set time stamp clock source.
 *
 * @param[in] base: CAN module
 * @param[in] src: time stamp clock source
 *              - CAN_TIME_STAMP_CLK_SRC_EXT
 *              - CAN_TIME_STAMP_CLK_SRC_BIT
 * @return none
 */
static inline void CAN_SetTimeStampClkSrc(CAN_Type *base, can_time_stamp_clk_src_t src)
{
    MODIFY_REG32(base->ACFCTRL0, CAN_ACFCTRL0_TCSS_Msk, CAN_ACFCTRL0_TCSS_Pos, (uint32_t)src);
}

/*!
 * @brief Set acceptance filter index.
 *
 * @param[in] base: CAN module
 * @param[in] index: filter index
 * @return none
 */
static inline void CAN_SetAcfIndex(CAN_Type *base, uint8_t index)
{
    DEVICE_ASSERT(index < CAN_FILTER_NUM_MAX);

    MODIFY_REG32(base->ACFCTRL0, CAN_ACFCTRL0_ACFADR_Msk, CAN_ACFCTRL0_ACFADR_Pos, index);
}

/*!
 * @brief Set filter data code.
 *
 * @param[in] base: CAN module
 * @param[in] code: filter data code
 * @return none
 */
static inline void CAN_SetAcfCode(CAN_Type *base, uint32_t code)
{
    base->ACFCTRL0 &= (~CAN_ACFCTRL0_SELMASK_Msk);
    base->ACF = (code & CAN_ACF_ACODE_Msk);
}

/*!
 * @brief Set filter data mask + AIDE + AIDEE.
 *
 * @param[in] base: CAN module
 * @param[in] mask: filter data mask + AIDE + AIDEE
 * @return none
 */
static inline void CAN_SetAcfMask(CAN_Type *base, uint32_t mask)
{
    base->ACFCTRL0 |= CAN_ACFCTRL0_SELMASK_Msk;
    base->ACF = mask & (CAN_ACF_ACODE_Msk | CAN_ACF_AIDE_Msk | CAN_ACF_AIDEE_Msk);
}

/*!
 * @brief Set VERMEM register.
 *
 * @param[in] base: CAN module
 * @param[in] value: VERMEM register value
 * @return none
 */
static inline void CAN_SetVerMem(CAN_Type *base, uint32_t value)
{
    base->VERMEM = value;
}

/*!
 * @brief Get VERMEM register.
 *
 * @param[in] base: CAN module
 * @return VERMEM register value
 */
static inline uint32_t CAN_GetVerMem(CAN_Type *base)
{
    return base->VERMEM;
}

/*!
 * @brief Get CAN version.
 *
 * @param[in] base: CAN module
 * @return CAN version
 */
static inline uint32_t CAN_GetVersion(CAN_Type *base)
{
    return (base->VERMEM & CAN_VERMEM_VERSION_Msk);
}

/*!
 * @brief Set memory ECC protect enable.
 *
 * @param[in] base: CAN module
 * @param[in] enable: ECC protect enable state
 * @return none
 */
static inline void CAN_SetMemEcc(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->VERMEM, CAN_VERMEM_MEEN_Msk, CAN_VERMEM_MEEN_Pos, enable);
}

/*!
 * @brief Get memory ECC protect enable state.
 *
 * @param[in] base: CAN module
 * @return ECC protect enable state
 */
static inline bool CAN_GetEccState(CAN_Type *base)
{
    return (((base->VERMEM & CAN_VERMEM_MEEN_Msk) >> CAN_VERMEM_MEEN_Pos) != 0U);
}

/*!
 * @brief Set memory ECC interrupt enable mask.
 *
 * @param[in] base: CAN module
 * @param[in] IntMask: ECC interrupt enable mask
 * @return none
 */
static inline void CAN_SetEccIntMask(CAN_Type *base, uint32_t IntMask)
{
    MODIFY_REG32(base->VERMEM, (CAN_VERMEM_MDWIE_Msk | CAN_VERMEM_MDEIE_Msk), 0, IntMask);
}

/*!
 * @brief Check memory ECC is initialized done.
 *
 * @param[in] base: CAN module
 * @return CAN memory initialization status
 */
static inline bool CAN_IsMemInitDone(CAN_Type *base)
{
    return (((base->VERMEM & CAN_VERMEM_MEID_Msk) >> CAN_VERMEM_MEID_Pos) != 0U);
}

/*!
 * @brief Get memory ECC error status.
 *
 * @param[in] base: CAN module
 * @return ECC error status
 */
static inline uint8_t CAN_GetEccStatus(CAN_Type *base)
{
    return (uint8_t)((base->VERMEM & (CAN_VERMEM_ACFA_Msk | CAN_VERMEM_TXS_Msk | \
        CAN_VERMEM_HELOC_Msk)) >> CAN_VERMEM_ACFA_Pos);
}

/*!
 * @brief Set memory error bit position 1.
 *
 * @param[in] base: CAN module
 * @param[in] position: memory error bit postion 1
 * @return none
 */
static inline void CAN_SetMebp1(CAN_Type *base, uint8_t position)
{
    MODIFY_REG32(base->MEMES, CAN_MEMES_MEBP1_Msk, CAN_MEMES_MEBP1_Pos, position);
}

/*!
 * @brief Set memory 1st error enable.
 *
 * @param[in] base: CAN module
 * @param[in] enable: memory 1st error enable state
 * @return none
 */
static inline void CAN_SetMe1ee(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->MEMES, CAN_MEMES_ME1EE_Msk, CAN_MEMES_ME1EE_Pos, enable);
}

/*!
 * @brief Set memory error bit position 2.
 *
 * @param[in] base: CAN module
 * @param[in] position: memory error bit postion 2
 * @return none
 */
static inline void CAN_SetMebp2(CAN_Type *base, uint8_t position)
{
    MODIFY_REG32(base->MEMES, CAN_MEMES_MEBP2_Msk, CAN_MEMES_MEBP2_Pos, position);
}

/*!
 * @brief Set memory 2nd error enable.
 *
 * @param[in] base: CAN module
 * @param[in] enable: memory 2nd error enable state
 * @return none
 */
static inline void CAN_SetMe2ee(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->MEMES, CAN_MEMES_ME2EE_Msk, CAN_MEMES_ME2EE_Pos, enable);
}

/*!
 * @brief Set memory error counter.
 *
 * @param[in] base: CAN module
 * @param[in] count: memory error counter
 * @return none
 */
static inline void CAN_SetMeeec(CAN_Type *base, uint8_t count)
{
    MODIFY_REG32(base->MEMES, CAN_MEMES_MEEEC_Msk, CAN_MEMES_MEEEC_Pos, count);
}

/*!
 * @brief Set memory no error counter.
 *
 * @param[in] base: CAN module
 * @param[in] count: memory no error counter
 * @return none
 */
static inline void CAN_SetMenec(CAN_Type *base, uint8_t count)
{
    MODIFY_REG32(base->MEMES, CAN_MEMES_MENEC_Msk, CAN_MEMES_MENEC_Pos, count);
}

/*!
 * @brief Set memory error location.
 *
 * @param[in] base: CAN module
 * @param[in] location: memory error location
 * @return none
 */
static inline void CAN_SetMel(CAN_Type *base, uint8_t location)
{
    MODIFY_REG32(base->MEMES, CAN_MEMES_MEL_Msk, CAN_MEMES_MEL_Pos, location);
}

/*!
 * @brief Set memory error side.
 *
 * @param[in] base: CAN module
 * @param[in] side: memory error side
 * @return none
 */
static inline void CAN_SetMes(CAN_Type *base, uint8_t side)
{
    MODIFY_REG32(base->MEMES, CAN_MEMES_MES_Msk, CAN_MEMES_MES_Pos, side);
}

/*!
 * @brief Set low pass filter enable for low power mode.
 *
 * @param[in] base: CAN module
 * @param[in] enable: low pass filter enable state
 * @return none
 */
static inline void CAN_SetLowPassFilter(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->WAKEUP, CAN_WAKEUP_LPFEN_Msk, CAN_WAKEUP_LPFEN_Pos, enable);
}

/*!
 * @brief Set wakeup enable for low power mode.
 *
 * @param[in] base: CAN module
 * @param[in] enable: wakeup enable state
 * @return none
 */
static inline void CAN_SetWakeup(CAN_Type *base, bool enable)
{
    MODIFY_REG32(base->WAKEUP, CAN_WAKEUP_WUEN_Msk, CAN_WAKEUP_WUEN_Pos, enable);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAN_HW_H */

/* =============================================  EOF  ============================================== */
