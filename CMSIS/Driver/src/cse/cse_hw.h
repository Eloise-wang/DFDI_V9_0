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
 * @file cse_hw.h
 *
 * @brief This file provides CSE hardware integration functions.
 *
 */

#ifndef _CSE_HW_H
#define _CSE_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "cse_drv.h"

/* ============================================  Define  ============================================ */
/*!
 * @brief Error code as SHE specification.
 */
#define CSE_NO_ERROR                                    (0x0001UL)
#define CSE_NO_ERROR_NO_SECURE_BOOT                     (0x0021UL) 
#define CSE_SEQUENCE_ERROR                              (0x0002UL)
#define CSE_KEY_NOT_AVAILABLE                           (0x0004UL)
#define CSE_KEY_INVALID                                 (0x0008UL)
#define CSE_KEY_EMPTY                                   (0x0010UL)
#define CSE_NO_SECURE_BOOT                              (0x0020UL)
#define CSE_KEY_WRITE_PROTECTED                         (0x0040UL)
#define CSE_KEY_UPDATE_ERROR                            (0x0080UL)
#define CSE_RNG_SEED                                    (0x0100UL)
#define CSE_NO_DEBUGGING                                (0x0200UL)
#define CSE_MEMORY_FAILURE                              (0x0400UL)
#define CSE_GENERAL_ERROR                               (0x1000UL)

/*!
 * @brief CSE_PRAM offset of the page length parameter.
 */
#define FEATURE_CSE_PAGE_LENGTH_OFFSET                  (0x000EUL)
#define FEATURE_CSE_MESSAGE_LENGTH_OFFSET               (0x000CUL)
#define FEATURE_CSE_MAC_LENGTH_OFFSET                   (0x0008UL)
#define FEATURE_CSE_BOOT_SIZE_OFFSET                    (0x001CUL)
#define FEATURE_CSE_BOOT_FLAVOR_OFFSET                  (0x001BUL)
#define FEATURE_CSE_FLASH_START_ADDRESS_OFFSET          (0x0010UL)
#define FEATURE_CSE_VERIFICATION_STATUS_OFFSET          (0x0014UL)
#define FEATURE_CSE_ERROR_BITS_OFFSET                   (0x0004UL)
#define FEATURE_CSE_SREG_OFFSET                         (0x002FUL)

/*!
 * @brief CSE_PRAM offset of page.
 */
#define FEATURE_CSE_PAGE_0_OFFSET                       (0x0000UL)
#define FEATURE_CSE_PAGE_1_OFFSET                       (0x0010UL)
#define FEATURE_CSE_PAGE_2_OFFSET                       (0x0020UL)
#define FEATURE_CSE_PAGE_3_OFFSET                       (0x0030UL)
#define FEATURE_CSE_PAGE_4_OFFSET                       (0x0040UL)
#define FEATURE_CSE_PAGE_5_OFFSET                       (0x0050UL)
#define FEATURE_CSE_PAGE_6_OFFSET                       (0x0060UL)
#define FEATURE_CSE_PAGE_7_OFFSET                       (0x0070UL)

#define CSE_PAGE_SIZE_IN_BYTES                          (16UL)    
#define CSE_DATA_PAGES_AVAILABLE                        (7UL)
#define CSE_DATA_BYTES_AVAILABLE                        (112UL)
#define CSE_BYTES_TO_FROM_PAGES_SHIFT                   (4UL)
#define CSE_BYTES_TO_FROM_BITS_SHIFT                    (3UL)
#define CSE_M1_SIZE_IN_BYTES                            (16UL)
#define CSE_M2_SIZE_IN_BYTES                            (32UL)
#define CSE_M3_SIZE_IN_BYTES                            (16UL)
#define CSE_M4_SIZE_IN_BYTES                            (32UL)
#define CSE_M5_SIZE_IN_BYTES                            (16UL)

#define CSE_UPPER_HALF_MASK                             (0xFFFF0000UL)
#define CSE_UPPER_HALF_SHIFT                            (0x10UL)
#define CSE_LOWER_HALF_MASK                             (0xFFFFUL)
#define CSE_LOWER_HALF_SHIFT                            (0UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Writes command in bytes to PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be written
 * @param[in] bytes: The buffer containing the bytes to be written
 * @param[in] numBytes: The number in bytes to be written
 * @return none
 */
void CSE_WriteCommandBytes(uint8_t offset, const uint8_t *bytes, uint8_t numBytes);

/*!
 * @brief Writes a command half word to PRAM.
 *
 * @param[in] offset: The offset in half word at which the bytes shall be written
 * @param[in] halfWord: The buffer containing the half word to be written
 * @return none
 */
void CSE_WriteCommandHalfWord(uint8_t offset, uint16_t halfWord);

/*!
 * @brief Writes a command byte to PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be written
 * @param[in] bytes: The buffer containing a byte to be written
 * @return none
 */
void CSE_WriteCommandByte(uint8_t offset, uint8_t byte);

/*!
 * @brief Writes command in words to PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be written
 * @param[in] words: The buffer containing the words to be written
 * @param[in] numWords: The number of words to be written
 * @return none
 */
void CSE_WriteCommandWords(uint8_t offset, const uint32_t *words, uint8_t numWords);

/*!
 * @brief Read command in bytes from PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be read
 * @param[out] bytes: The buffer containing the bytes read from PRAM
 * @param[in] numBytes: The number in bytes to be read
 * @return none
 */
void CSE_ReadCommandBytes(uint8_t offset, uint8_t *bytes, uint8_t numBytes);

/*!
 * @brief Read a command half word from CSE_PRAM at a 16-bit aligned offset.
 *
 * @param[in] offset: CSE PRAM offset
 * @return none
 */
uint16_t CSE_ReadCommandHalfWord(uint8_t offset);

/*!
 * @brief Read command in bytes from PRAM.
 *
 * @param[in] offset: The offset in bytes at which the bytes shall be read
 * @param[out] bytes: The buffer containing the bytes read from PRAM
 * @return Value of command byte
 */
uint8_t CSE_ReadCommandByte(uint8_t offset);

/*!
 * @brief Reads command words from PRAM.
 *
 * @param[in] offset: The offset (in bytes) from which the words shall be read
 * @param[out] words: The buffer containing the words read.
 * @param[in] numWords: The number of words to be read.
 * @return none
*/
void CSE_ReadCommandWords(uint8_t offset, uint32_t *words, uint8_t numWords);

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
                         cse_call_sequence_t callSeq, cse_key_id_t keyId);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enables/Disables the command completion interrupt.
 *
 * @param[in] enable Enable/Disable the command completion interrupt
 * @return none
 */
static inline void CSE_SetInterrupt(bool enable)
{
    MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_CSECCIE_Msk, FLASH_CNFG_CSECCIE_Pos, enable);
}

/*!
 * @brief Writes the command header to PRAM.
 * @param[in] funcId: the ID of the operation to be started
 * @param[in] funcFormat: specifies how the data is transferred to/from the CSE
 * @param[in] callSeq: specifies if the information is the first or a following function call
 * @param[in] keyId: keyID that will be used to perform the requested cryptographic operation
 * @return none
 */
static inline void CSE_WriteCommandHeader(cse_cmd_t funcId, cse_func_format_t funcFormat,
                                          cse_call_sequence_t callSeq, cse_key_id_t keyId)
{
    CSE_PRAM->RAMn[0].DATA_32 = CSE_PRAM_RAMn_DATA_32_BYTE_0(keyId) | \
                                CSE_PRAM_RAMn_DATA_32_BYTE_1(callSeq) | \
                                CSE_PRAM_RAMn_DATA_32_BYTE_2(funcFormat) | \
                                CSE_PRAM_RAMn_DATA_32_BYTE_3(funcId);
}

/*!
 * @brief Waits for the completion of a CSE command.
 *
 * @param[in] none
 * return none
 */
static inline void CSE_WaitCommandCompletion(void)
{
	while (FLASH->CSESTAT & FLASH_CSESTAT_BSY_Msk)
	{
		/* Wait until the CCIF flag is set */
	}
}

/*!
 * @brief Reads the error bits from PRAM.
 *
 * @param[in] none
 * @return Error Code after command execution
 */
static inline status_t CSE_ReadErrorBits(void)
{
	uint16_t errBits = CSE_ReadCommandHalfWord(FEATURE_CSE_ERROR_BITS_OFFSET);
	status_t stat = STATUS_ERROR;

	switch (errBits)
	{
	case CSE_NO_ERROR:
	case CSE_NO_ERROR_NO_SECURE_BOOT:
		stat = STATUS_SUCCESS;
		break;
	
	case CSE_SEQUENCE_ERROR:
		stat = STATUS_SEC_SEQUENCE_ERROR;
		break;
	
	case CSE_KEY_NOT_AVAILABLE:
		stat = STATUS_SEC_KEY_NOT_AVAILABLE;
		break;
	
	case CSE_KEY_INVALID:
		stat = STATUS_SEC_KEY_INVALID;
		break;
	
	case CSE_KEY_EMPTY:
		stat = STATUS_SEC_KEY_EMPTY;
		break;
	
	case CSE_NO_SECURE_BOOT:
		stat = STATUS_SEC_NO_SECURE_BOOT;
		break;
	
	case CSE_KEY_WRITE_PROTECTED:
		stat = STATUS_SEC_KEY_WRITE_PROTECTED;
		break;
	
	case CSE_KEY_UPDATE_ERROR:
		stat = STATUS_SEC_KEY_UPDATE_ERROR;
		break;
	
	case CSE_RNG_SEED:
		stat = STATUS_SEC_RNG_SEED;
		break;
	
	case CSE_NO_DEBUGGING:
		stat = STATUS_SEC_NO_DEBUGGING;
		break;
	
	case CSE_MEMORY_FAILURE:
		stat = STATUS_SEC_MEMORY_FAILURE;
		break;
	
	case CSE_GENERAL_ERROR:
	default:
		stat = STATUS_ERROR;
		break;
	
	}
	
	return stat;
}

/*!
 * @brief Returns the content of the status register.
 *
 * @param[in] none
 * @return Value of the status register
 */
static inline uint32_t CSE_GetStatus(void)
{
    return (uint32_t)(FLASH->CSESTAT);
}

/*!
 * @brief Return command completed status.
 *
 * param[in] none
 * @return 1: Command is completed
 *         0: Command is not completed
 */
static inline uint8_t CSE_GetCmdCompleteStatus(void)
{
    return (uint8_t)((FLASH->CSESTAT & FLASH_CSESTAT_CCIF_Msk) >> FLASH_CSESTAT_CCIF_Pos);
}

/*!
 * @brief Clear command completed interrupt flag.
 *
 * @param[in] none
 * @return none
 */
static inline void CSE_ClearCmdCompleteStatus(void)
{
    FLASH->CSESTAT |= FLASH_CSESTAT_CCIF_Msk;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CSE_HW_H */

/* =============================================  EOF  ============================================== */
