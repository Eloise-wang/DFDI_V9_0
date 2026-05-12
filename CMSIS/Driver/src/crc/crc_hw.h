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
 * @file crc_hw.h
 *
 * @brief CRC access register inline function definition.
 *
 */

#ifndef _CRC_HW_H
#define _CRC_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Include  ============================================ */
#include "crc_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Sets seed value for CRC module.
 *
 * @param[in] base: CRC base pointer
 * @param[in] seedValue: New seed data for CRC module
 * @return none
 */
void CRC_SetSeedReg(CRC_Type * const base, uint32_t seedValue);

/*!
 * @brief Get the current result of CRC calculation.
 *
 * @param[in] base: CRC base pointer
 * @return none
 */
uint32_t CRC_GetCrcResult(const CRC_Type * const base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Config CRC CTRL register.
 *
 * @param[in] base: CRC base pointer
 * @param[in] value: config value
 * @return none
 */
static inline void CRC_SetCtrlReg(CRC_Type * const base, uint32_t value)
{
    base->CTRL = value;
}

/*!
 * @brief Set CRC Poly register.
 *
 * @param[in] base: CRC base pointer
 * @param[in] value: polynomial value
 * @return none
 */
static inline void CRC_SetPolyReg(CRC_Type * const base, uint32_t value)
{
    base->POLY = value;
}

/*!
 * @brief Get CRC Poly register value.
 *
 * @param[in] base: CRC base pointer
 * @return polynomial value
 */
static inline uint32_t CRC_GetPolyReg(const CRC_Type * const base)
{
    return (base->POLY);
}

/*!
 * @brief Sets the 32 bits of CRC data register.
 *
 * @param[in] base: CRC base pointer
 * @param[in] value: data value
 * @return none
 */
static inline void CRC_SetDataReg(CRC_Type * const base, uint32_t value)
{
    base->DATAn.DATA32 = value;
}

/*!
 * @brief Get CRC Data register value.
 *
 * @param[in] base: CRC base pointer
 * @return data value
 */
static inline uint32_t CRC_GetDataReg(const CRC_Type * const base)
{
    return (base->DATAn.DATA32);
}

/*!
 * @brief Sets the low 16 bits of CRC data register.
 *
 * @param[in] base: CRC base pointer
 * @param[in] value: data value
 * @return none
 */
static inline void CRC_SetDataLReg(CRC_Type * const base, uint16_t value)
{
    base->DATAn.DATA16.L = value;
}

/*!
 * @brief Sets the low 8 bits of CRC data register.
 *
 * @param[in] base: CRC base pointer
 * @param[in] value: data value
 * @return none
 */
static inline void CRC_SetDataLLReg(CRC_Type * const base, uint8_t value)
{
    base->DATAn.DATA8.LL = value;
}

/*!
 * @brief Select crc protocol mode between crc16 or crc32.
 *
 * @param[in] base: CRC base pointer
 * @param[in] type: crc check mode select
 * @return none
 */
static inline void CRC_SetProtocolType(CRC_Type * const base, crc_protocol_type_t type)
{
    MODIFY_REG32(base->CTRL, CRC_CTRL_TCRC_Msk, CRC_CTRL_TCRC_Pos, type);
}

/*!
 * @brief Get crc protocol mode between crc16 or crc32.
 *
 * @param[in] base: CRC base pointer
 * @return crc protocol mode
 */
static inline crc_protocol_type_t CRC_GetProtocolType(const CRC_Type * const base)
{
    return ((crc_protocol_type_t)((base->CTRL & CRC_CTRL_TCRC_Msk) >> CRC_CTRL_TCRC_Pos));
}

/*!
 * @brief Select seed or data mode.
 *
 * @param[in] base: CRC base pointer
 * @param[in] mode: mode select
 * @return none
 */
static inline void CRC_SetSeedOrDataMode(CRC_Type * const base, crc_data_type_t mode)
{
    MODIFY_REG32(base->CTRL, CRC_CTRL_WAS_Msk, CRC_CTRL_WAS_Pos, mode);
}

/*!
 * @brief Set CRC write transpose mode.
 *
 * @param[in] base: CRC base pointer
 * @param[in] transType: write transpose mode
 * @return none
 */
static inline void CRC_SetWriteTranspose(CRC_Type * const base, crc_transpose_t transType)
{
    MODIFY_REG32(base->CTRL, CRC_CTRL_TOTW_Msk, CRC_CTRL_TOTW_Pos, transType);
}

/*!
 * @brief Get CRC write transpose mode.
 *
 * @param[in] base: CRC base pointer
 * @return write transpose mode
 */
static inline crc_transpose_t CRC_GetWriteTranspose(const CRC_Type * const base)
{
    return ((crc_transpose_t)((base->CTRL & CRC_CTRL_TOTW_Msk) >> CRC_CTRL_TOTW_Pos));
}

/*!
 * @brief Set CRC read transpose mode.
 *
 * @param[in] base: CRC base pointer
 * @param[in] transType: read transpose mode
 * @return none
 */
static inline void CRC_SetReadTranspose(CRC_Type * const base, crc_transpose_t transType)
{
    MODIFY_REG32(base->CTRL, CRC_CTRL_TOTR_Msk, CRC_CTRL_TOTR_Pos, transType);
}

/*!
 * @brief Get CRC read transpose mode.
 *
 * @param[in] base: CRC base pointer
 * @return read transpose mode
 */
static inline crc_transpose_t CRC_GetReadTranspose(const CRC_Type * const base)
{
    return ((crc_transpose_t)((base->CTRL & CRC_CTRL_TOTR_Msk) >> CRC_CTRL_TOTR_Pos));
}

/*!
 * @brief Set CRC result xor mode.
 *
 * @param[in] base: CRC base pointer
 * @param[in] enable: result xor mode
 *            - ENABLE: read crc result with xor
 *            - DISABLE: read crc result without xor
 * @return none
 */
static inline void CRC_SetResultXorMode(CRC_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL, CRC_CTRL_FXOR_Msk, CRC_CTRL_FXOR_Pos, enable);
}

/*!
 * @brief Get CRC read xor mode.
 *
 * @param[in] base: CRC base pointer
 * @return read xor mode
 */
static inline bool CRC_GetFXorMode(const CRC_Type * const base)
{
    return (((base->CTRL & CRC_CTRL_FXOR_Msk) >> CRC_CTRL_FXOR_Pos) != 0U);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CRC_HW_H */

/* =============================================  EOF  ============================================== */
