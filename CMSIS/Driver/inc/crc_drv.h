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
 * @file crc_drv.h
 *
 * @brief This file provides crc integration functions interface.
 *
 */

#ifndef _CRC_DRV_H
#define _CRC_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================  Include  ============================================= */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief CRC protocol enum
 */
typedef enum
{
    CRC_PROTOCOL_16BIT = 0x00U,     /*!< CRC Protocol 16bit mode */
    CRC_PROTOCOL_32BIT              /*!< CRC Protocol 32bit mode */
} crc_protocol_type_t;

/*!
 * @brief CRC data type enum
 */
typedef enum
{
    CRC_DATA_IS_DATA = 0x00U,       /*!< CRC DATA register is data */
    CRC_DATA_IS_SEED                /*!< CRC DATA register is seed */
} crc_data_type_t;

/*!
 * @brief CRC transpose enum
 */
typedef enum
{
    CRC_TRANSPOSE_NONE = 0x00U,   /*!< CRC write in without tranpose */
    CRC_TRANSPOSE_BITS,           /*!< CRC write in with bits transpose */
    CRC_TRANSPOSE_BITS_BYTES,     /*!< CRC write in with bits and bytes transpose */
    CRC_TRANSPOSE_BYTES           /*!< CRC write in with bytes transpose */
} crc_transpose_t;

/*!
 * @brief CRC write num once enumeration
 */
typedef enum
{
    CRC_WRITE_1_BYTE_ONCE = 0x00U,    /*!< CRC write 1 byte once */
    CRC_WRITE_2_BYTE_ONCE,            /*!< CRC write 2 byte once */
    CRC_WRITE_4_BYTE_ONCE             /*!< CRC write 4 byte once */
} crc_write_bytes_num_t;

/*!
 * @brief CRC configuration structure
 *
 * This structure holds the configuration settings for the crc
 */
typedef struct
{
    crc_protocol_type_t   crcProtocolType;               /*!< CRC 16/32 protocol type */
    crc_transpose_t       writeTransposeType;            /*!< CRC write in transpose type */
    crc_transpose_t       readTransposeType;             /*!< CRC read out transpose type */
    bool                  finalXOR;                      /*!< Enable/disable result XOR */
    crc_write_bytes_num_t writeBytesNumOnce;             /*!< CRC write bytes num once select */
    uint32_t              poly;                          /*!< CRC polynomial */
    uint32_t              seed;                          /*!< CRC seed*/
} crc_user_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize CRC driver based on configuration input.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] config: CRC configuration pointer
 * @return The result of execution
 *         - STATUS_SUCCESS: Initialize successful
 *         - STATUS_ERROR: Initialize failed
 */
status_t CRC_DRV_Init(uint8_t instance, const crc_user_config_t *config);

/*!
 * @brief De-initialize CRC driver.
 *
 * @param[in] instance: The CRC instance number
 * @return The result of execution
 *         - STATUS_SUCCESS: de-initialize successful
 */
status_t CRC_DRV_Deinit(uint8_t instance);

/*!
 * @brief Appends a block of bytes to the current CRC calculation.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] data: ThePointer to the data array
 * @param[in] sizeBytes: Number of the data array
 * @return none
 */
void CRC_DRV_WriteData(uint8_t instance, const uint8_t *data, uint32_t sizeBytes);

/*!
 * @brief Gets the current result of the CRC calculation.
 *
 * @param[in] instance: The CRC instance number
 * @return Result of CRC calculation
 */
uint32_t CRC_DRV_GetCrcResult(uint8_t instance);

/*!
 * @brief Gets the configuration structure of the CRC module currently.
 *
 * @param[in] instance: The CRC instance number
 * @param[out] config: Pointer to structure of CRC configuration
 * @return Result of execution
 *         - STATUS_SUCCESS: Operation was successful
 */
status_t CRC_DRV_GetConfig(uint8_t instance, crc_user_config_t * const config);

/*!
 * @brief Appends 8-bit data to the current CRC calculation and get new result.
          If the newSeed is ture, seed set and result are calculated from the new seed value.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] data: Input data for CRC calculation
 * @param[in] newSeed: Check if sets new seed to calculate
 * @param[in] seed: New seed value
 * @return Result of CRC calculation
 */
uint32_t CRC_DRV_GetCrc8(uint8_t instance, uint8_t data, bool newSeed, uint32_t seed);

/*!
 * @brief Appends 16-bit data to the current CRC calculation and get new result.
          If the newSeed is ture, seed set and result are calculated from the new seed value.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] data: Input data for CRC calculation
 * @param[in] newSeed: Check if sets new seed to calculate
 * @param[in] seed: New seed value
 * @return Result of CRC calculation
 */
uint32_t CRC_DRV_GetCrc16(uint8_t instance, uint16_t data, bool newSeed, uint32_t seed);

/*!
 * @brief Appends 32-bit data to the current CRC calculation and get new result.
          If the newSeed is ture, seed set and result are calculated from the new seed value.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] data: Input data for CRC calculation
 * @param[in] newSeed: Check if sets new seed to calculate
 * @param[in] seed: New seed value
 * @return Result of CRC calculation
 */
uint32_t CRC_DRV_GetCrc32(uint8_t instance, uint32_t data, bool newSeed, uint32_t seed);

/*!
 * @brief Gets the CRC default configuration structure.
 *
 * @param[out] config: Pointer to structure of CRC configuration
 * @return Result of execution
 *         - STATUS_SUCCESS: Operation was successful
 */
status_t CRC_DRV_GetDefaultConfig(crc_user_config_t * const config);

/*!
 * @brief Sets the CRC configuration structure.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] userConfigPt: Pointer to structure of CRC configuration
 * @return Result of execution
 *         - STATUS_SUCCESS: Operation was successful
 */
status_t CRC_DRV_Configure(uint8_t instance, const crc_user_config_t *userConfigPtr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CRC_DRV_H */

/* =============================================  EOF  ============================================== */
