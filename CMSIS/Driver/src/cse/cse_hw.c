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
 * @file cse_hw.c
 *
 * @brief This file provides cse hardware access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "cse_hw.h"

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Writes command in bytes to PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be written
 * @param[in] bytes: The buffer containing the bytes to be written
 * @param[in] numBytes: The number in bytes to be written
 * @return none
 */
void CSE_WriteCommandBytes(uint8_t offset, const uint8_t *bytes, uint8_t numBytes)
{
    DEVICE_ASSERT(bytes != NULL);

    uint8_t i = 0U;
    
    while ((i + 3U) < numBytes)
    {
        CSE_PRAM->RAMn[(offset + i) >> 2U].DATA_32 = CSE_PRAM_RAMn_DATA_32_BYTE_0(bytes[i + 3U]) | \
                                                     CSE_PRAM_RAMn_DATA_32_BYTE_1(bytes[i + 2U]) | \
                                                     CSE_PRAM_RAMn_DATA_32_BYTE_2(bytes[i + 1U]) | \
                                                     CSE_PRAM_RAMn_DATA_32_BYTE_3(bytes[i]);
        i = (uint8_t)(i + 4U);
    }
    while (i < numBytes)
    {
        CSE_WriteCommandByte(offset + i, bytes[i]);
        i++;
    }
}

/*!
 * @brief Writes a command half word to PRAM.
 *
 * @param[in] offset: The offset in half word at which the bytes shall be written
 * @param[in] halfWord: The buffer containing the half word to be written
 * @return none
 */
void CSE_WriteCommandHalfWord(uint8_t offset, uint16_t halfWord)
{
    uint32_t tmp = 0U;

    tmp = CSE_PRAM->RAMn[(offset >> 2U)].DATA_32;

    if ((offset & 2U) != 0U)
    {
        tmp = tmp & ~CSE_LOWER_HALF_MASK;
        tmp = tmp | ((((uint32_t) halfWord) << CSE_LOWER_HALF_SHIFT) & CSE_LOWER_HALF_MASK);
    }
    else
    {
        tmp = tmp & ~CSE_UPPER_HALF_MASK;
        tmp = tmp | ((((uint32_t) halfWord) << CSE_UPPER_HALF_SHIFT) & CSE_UPPER_HALF_MASK);
    }

    CSE_PRAM->RAMn[(offset >> 2U)].DATA_32 = tmp;
}

/*!
 * @brief Writes a command byte to PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be written
 * @param[in] bytes: The buffer containing a byte to be written
 * @return none
 */
void CSE_WriteCommandByte(uint8_t offset, uint8_t byte)
{
    switch (offset & 0x3U)
    {
    case 0x0U:
        CSE_PRAM->RAMn[offset >> 2U].ACCESS8BIT.DATA_8HU = byte;
        break;
		
    case 0x1U:
        CSE_PRAM->RAMn[offset >> 2U].ACCESS8BIT.DATA_8HL = byte;
        break;
		
    case 0x2U:
        CSE_PRAM->RAMn[offset >> 2U].ACCESS8BIT.DATA_8LU = byte;
        break;
		
    case 0x3U:
        CSE_PRAM->RAMn[offset >> 2U].ACCESS8BIT.DATA_8LL = byte;
        break;
		
    default:
        /* Impossible to get here */
        break;
		
    }
}

/*!
 * @brief Writes command in words to PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be written
 * @param[in] words: The buffer containing the words to be written
 * @param[in] numWords: The number of words to be written
 * @return none
 */
void CSE_WriteCommandWords(uint8_t offset, const uint32_t *words, uint8_t numWords)
{
    DEVICE_ASSERT(NULL != words);

    uint8_t i = 0U;
    uint8_t alignedOffset = (uint8_t)(offset >> 2U);
 
    while (i < numWords)
    {
        CSE_PRAM->RAMn[alignedOffset + i].DATA_32 = words[i];
        i++;
    }
}

/*!
 * @brief Read command in bytes from PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be read
 * @param[out] bytes: The buffer containing the bytes read from PRAM
 * @param[in] numBytes: The number in bytes to be read
 * @return none
 */
void CSE_ReadCommandBytes(uint8_t offset, uint8_t *bytes, uint8_t numBytes)
{
    DEVICE_ASSERT(NULL != bytes);

    uint8_t i = 0U;
    uint32_t tmp = 0U;
    
    while ((i + 3U) < numBytes)
    {
        tmp = CSE_PRAM->RAMn[(offset + i) >> 2U].DATA_32;
        bytes[i + 3U] = (uint8_t)((tmp & CSE_PRAM_RAMn_DATA_32_BYTE_0_MASK) >> CSE_PRAM_RAMn_DATA_32_BYTE_0_SHIFT);
        bytes[i + 2U] = (uint8_t)((tmp & CSE_PRAM_RAMn_DATA_32_BYTE_1_MASK) >> CSE_PRAM_RAMn_DATA_32_BYTE_1_SHIFT);
        bytes[i + 1U] = (uint8_t)((tmp & CSE_PRAM_RAMn_DATA_32_BYTE_2_MASK) >> CSE_PRAM_RAMn_DATA_32_BYTE_2_SHIFT);
        bytes[i]      = (uint8_t)((tmp & CSE_PRAM_RAMn_DATA_32_BYTE_3_MASK) >> CSE_PRAM_RAMn_DATA_32_BYTE_3_SHIFT);
        i = (uint8_t)(i + 4U);
    }
    while (i < numBytes)
    {
        bytes[i] = CSE_ReadCommandByte(offset + i);
        i++;
    }
}

/*!
 * @brief Read a command half word from CSE_PRAM at a 16-bit aligned offset.
 *
 * @param[in] offset: CSE PRAM offset
 * @return none
 *
 */
uint16_t CSE_ReadCommandHalfWord(uint8_t offset)
{
    uint16_t halfWord = 0U;

    if ((offset & 2U) != 0U)
    {
        halfWord = (uint16_t)((CSE_PRAM->RAMn[(offset >> 2U)].DATA_32 & CSE_UPPER_HALF_MASK) >> CSE_UPPER_HALF_SHIFT);
    }
    else
    {
        halfWord = (uint16_t)((CSE_PRAM->RAMn[(offset >> 2U)].DATA_32 & CSE_LOWER_HALF_MASK) >> CSE_LOWER_HALF_SHIFT);
    }

    return halfWord;
}

/*!
 * @brief Read command in bytes from PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be read
 * @param[out] bytes: The buffer containing the bytes read from PRAM
 * @return Value of command byte
 */
uint8_t CSE_ReadCommandByte(uint8_t offset)
{
    uint8_t byte = 0U;

    switch (offset & 0x3U)
    {
    case 0x0U:
        byte = CSE_PRAM->RAMn[offset >> 2U].ACCESS8BIT.DATA_8HU;
        break;
		
    case 0x1U:
        byte = CSE_PRAM->RAMn[offset >> 2U].ACCESS8BIT.DATA_8HL;
        break;
		
    case 0x2U:
        byte = CSE_PRAM->RAMn[offset >> 2U].ACCESS8BIT.DATA_8LU;
        break;
		
    case 0x3U:
        byte = CSE_PRAM->RAMn[offset >> 2U].ACCESS8BIT.DATA_8LL;
        break;
		
    default:
        /* Impossible to get here */
        break;
		
    }

    return byte;
}

/*!
 * @brief Reads command words from PRAM.
 *
 * @param[in] offset: The offset (in bytes) from which the words shall be read
 * @param[out] words: The buffer containing the words read.
 * @param[in] numWords: The number of words to be read.
 * @return none
*/
void CSE_ReadCommandWords(uint8_t offset, uint32_t *words, uint8_t numWords)
{
    DEVICE_ASSERT(NULL != words);

    uint8_t i = 0U;
    uint8_t alignedOffset = (uint8_t)(offset >> 2U);

    while (i < numWords)
    {
        words[i] = CSE_PRAM->RAMn[alignedOffset + i].DATA_32;
        i++;
    }
}

/*!
 * @brief Writes the command header to PRAM and waits for excute completion.
 *
 * @param[in] funcId: funciton ID
 * @param[in] funcFormat: The offset in bytes at which the bytes shall be read
 * @param[in] callSeq: The offset in bytes at which the bytes shall be read
 * @param[in] keyId: The offset in bytes at which the bytes shall be read
 * @return none
 */
void CSE_WriteCmdAndWait(cse_cmd_t funcId, cse_func_format_t funcFormat,
                         cse_call_sequence_t callSeq, cse_key_id_t keyId)
{
    CSE_PRAM->RAMn[0].DATA_32 = CSE_PRAM_RAMn_DATA_32_BYTE_0(keyId) | \
                                CSE_PRAM_RAMn_DATA_32_BYTE_1(callSeq) | \
                                CSE_PRAM_RAMn_DATA_32_BYTE_2(funcFormat) | \
                                CSE_PRAM_RAMn_DATA_32_BYTE_3(funcId);
    CSE_WaitCommandCompletion();
}

/* =============================================  EOF  ============================================== */
