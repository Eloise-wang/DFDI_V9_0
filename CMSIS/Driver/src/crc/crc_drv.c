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
 * @file crc_drv.c
 *
 * @brief This file provides crc integration functions.
 *
 */

/* ===========================================  Include  ============================================ */
#include "crc_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */
#define CRC_DEFAULT_SEED      (0xFFFFUL)   /* Default seed value of CRC is 0xFFFF */
#define CRC_DEFAULT_POLY      (0x1021UL)   /* Default polynomial value of CRC is 0x1021 */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Save user defined crc_write_bytes_num_t option  */
static crc_write_bytes_num_t s_crcWriteBytesNumOnce = CRC_WRITE_1_BYTE_ONCE;
/* Table of base addresses for CRC instances */
static CRC_Type * const s_crcBase[CRC_INSTANCE_MAX] = CRC_BASE_PTRS;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initialize CRC driver based on configuration input.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] config: CRC configuration pointer
 * @return The result of execution
 *         - STATUS_SUCCESS: Initialize successful
 *         - STATUS_ERROR: Initialize failed
 */
status_t CRC_DRV_Init(uint8_t instance, const crc_user_config_t *config)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);
    status_t retStatus = STATUS_SUCCESS;

    /* enable CRC clock, and set the default configuration */
    CKGEN_DRV_Enable(CLK_CRC, true);
    CKGEN_DRV_SoftReset(SRST_CRC, true);
    /* configure CRC */
    retStatus = CRC_DRV_Configure(instance, config);

    return retStatus;
}

/*!
 * @brief De-initialize CRC driver.
 *
 * @param[in] instance: The CRC instance number
 * @return The result of execution
 *         - STATUS_SUCCESS: de-initialize successful
 */
status_t CRC_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);

    /* De-initialize CRC module */
    CKGEN_DRV_SoftReset(SRST_CRC, false);
    CKGEN_DRV_Enable(CLK_CRC, false);

    return STATUS_SUCCESS;
}

/*!
 * @brief Appends a block of bytes to the current CRC calculation.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] data: The Pointer to the data array
 * @param[in] sizeBytes: Number of the data array
 * @return none
 */
void CRC_DRV_WriteData(uint8_t instance, const uint8_t *data, uint32_t sizeBytes)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);
    DEVICE_ASSERT(data != NULL);
    uint32_t i = 0U, j = 0U;
    uint32_t sizeWords = 0U, sizeDWords = 0U;
    CRC_Type *base = s_crcBase[instance];

    /* Byte change to half-word */
    sizeWords = sizeBytes >> 1U;
    /* Byte change to word */
    sizeDWords = sizeBytes >> 2U;
    switch (s_crcWriteBytesNumOnce)
    {
    case CRC_WRITE_1_BYTE_ONCE:
        /* 8-bit write till end of data buffer */
        for (i = 0U; i < sizeBytes; i++)
        {
            CRC_SetDataLLReg(base, data[i]);
        }
        break;

    case CRC_WRITE_2_BYTE_ONCE:
        /* 16-bit write till end of number */
        for (i = 0U; i < sizeWords; i++)
        {
            CRC_SetDataLReg(base, ((((uint16_t) data[j]) << 8U) | ((uint16_t)data[j + 1U])));
            j += 2U;
        }
        /* 8-bit write till end of data buffer */
        for (; j < sizeBytes; j++)
        {
            CRC_SetDataLLReg(base, data[j]);
        }
        break;

    case CRC_WRITE_4_BYTE_ONCE:
        /* 32-bit write till end of number */
        for (i = 0U, j = 0U; i < sizeDWords; i++)
        {
            CRC_SetDataReg(base, ((((uint32_t) data[j]) << 24U) | ((uint32_t)data[j + 1U] << 16U) \
                                   | ((uint32_t)data[j + 2U] << 8U) | ((uint32_t)data[j + 3U])));
            j += 4U;
        }
        /* 16-bit write till end of number */
        if (2U <= (sizeBytes - j))
        {
            CRC_SetDataLReg(base, ((((uint16_t) data[j]) << 8U) | ((uint16_t)data[j + 1U])));
            j += 2U;
        }
        /* 8-bit write till end of data buffer */
        for (; j < sizeBytes; j++)
        {
            CRC_SetDataLLReg(base, data[j]);
        }
        break;

    default:
        break;
    }
}

/*!
 * @brief Gets the current result of the CRC calculation.
 *
 * @param[in] instance: The CRC instance number
 * @return Result of CRC calculation
 */
uint32_t CRC_DRV_GetCrcResult(uint8_t instance)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);
    const CRC_Type * base = s_crcBase[instance];

    /* Get result of CRC calculation */
    return CRC_GetCrcResult(base);
}

/*!
 * @brief Gets the configuration structure of the CRC module currently.
 *
 * @param[in] instance: The CRC instance number
 * @param[out] config: Pointer to structure of CRC configuration
 * @return Result of execution
 *         - STATUS_SUCCESS: Operation was successful
 */
status_t CRC_DRV_GetConfig(uint8_t instance, crc_user_config_t * const config)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);
    const CRC_Type *base = s_crcBase[instance];

    /* Gets CRC protocol */
    config->crcProtocolType = CRC_GetProtocolType(base);
    /* Get write transpose type */
    config->writeTransposeType = CRC_GetWriteTranspose(base);
    /* Get read transpose type */
    config->readTransposeType = CRC_GetReadTranspose(base);
    /* Get result XOR status */
    config->finalXOR = CRC_GetFXorMode(base);
    /* Get CRC write bytes num once value */
    config->writeBytesNumOnce = s_crcWriteBytesNumOnce;
    /* Get a polynomial */
    config->poly = CRC_GetPolyReg(base);
    if (CRC_PROTOCOL_32BIT == config->crcProtocolType)
    {
        /* get a seed - initial checksum */
        config->seed = CRC_GetDataReg(base);
    }
    else
    {
        /* get a seed - initial checksum */
        config->seed = (uint16_t)CRC_GetDataReg(base);
    }

    return STATUS_SUCCESS;
}

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
uint32_t CRC_DRV_GetCrc8(uint8_t instance, uint8_t data, bool newSeed, uint32_t seed)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);
    CRC_Type *base = s_crcBase[instance];

    /* If new seed is ture then write a seed to initial checksum */
    if (newSeed)
    {
        /* Set a seed */
        CRC_SetSeedReg(base, seed);
    }

    /* Write 8-bit data */
    CRC_SetDataLLReg(base, data);

    /* Return result of CRC calculation*/
    return CRC_GetCrcResult(base);
}

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
uint32_t CRC_DRV_GetCrc16(uint8_t instance, uint16_t data, bool newSeed, uint32_t seed)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);
    CRC_Type *base = s_crcBase[instance];

    /* If new seed is ture then write a seed to initial checksum */
    if (newSeed)
    {
        /* Set a seed */
        CRC_SetSeedReg(base, seed);
    }

    /* Write 16-bit data */
    CRC_SetDataLReg(base, data);

    /* Return result of CRC calculation*/
    return CRC_GetCrcResult(base);
}

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
uint32_t CRC_DRV_GetCrc32(uint8_t instance, uint32_t data, bool newSeed, uint32_t seed)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);
    CRC_Type *base = s_crcBase[instance];

    /* If new seed is ture then write a seed to initial checksum */
    if (newSeed)
    {
        /* Set a seed */
        CRC_SetSeedReg(base, seed);
    }

    /* Write 32-bit data */
    CRC_SetDataReg(base, data);

    /* Return result of CRC calculation*/
    return CRC_GetCrcResult(base);
}

/*!
 * @brief Gets the CRC default configuration structure.
 *
 * @param[out] config: Pointer to structure of CRC configuration
 * @return Result of execution
 *         - STATUS_SUCCESS: Operation was successful
 */
status_t CRC_DRV_GetDefaultConfig(crc_user_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    /* Gets CRC protocol */
    config->crcProtocolType = CRC_PROTOCOL_16BIT;
    /* Get write transpose type */
    config->writeTransposeType = CRC_TRANSPOSE_NONE;
    /* Get read transpose type */
    config->readTransposeType = CRC_TRANSPOSE_NONE;
    /* Get result XOR status */
    config->finalXOR = false;
    /* Get CRC write bytes num once value */
    config->writeBytesNumOnce = CRC_WRITE_1_BYTE_ONCE;
    /* Get a polynomial */
    config->poly = CRC_DEFAULT_POLY;
    /* get a seed - initial checksum */
    config->seed = CRC_DEFAULT_SEED;

    return STATUS_SUCCESS;
}

/*!
 * @brief Sets the CRC configuration structure.
 *
 * @param[in] instance: The CRC instance number
 * @param[in] userConfigPt: Pointer to structure of CRC configuration
 * @return Result of execution
 *         - STATUS_SUCCESS: Operation was successful
 */
status_t CRC_DRV_Configure(uint8_t instance, const crc_user_config_t * userConfigPtr)
{
    DEVICE_ASSERT(instance < CRC_INSTANCE_MAX);
    DEVICE_ASSERT(userConfigPtr != NULL);
    CRC_Type *base = s_crcBase[instance];

    /* config ProtocolType firstly */
    CRC_SetProtocolType(base, userConfigPtr->crcProtocolType);
    /* config write transpose type*/
    CRC_SetWriteTranspose(base, userConfigPtr->writeTransposeType);
    /* config read transpose type*/
    CRC_SetReadTranspose(base, userConfigPtr->readTransposeType);
    /* config enable/disable result xor */
    CRC_SetResultXorMode(base, userConfigPtr->finalXOR);
    /* Set a polynomial */
    CRC_SetPolyReg(base, userConfigPtr->poly);
    /* Set crc write bytes number at once */
    s_crcWriteBytesNumOnce = userConfigPtr->writeBytesNumOnce;
    /* Set a seed */
    CRC_SetSeedReg(base, userConfigPtr->seed);

    return STATUS_SUCCESS;
}

/* =============================================  EOF  ============================================== */
