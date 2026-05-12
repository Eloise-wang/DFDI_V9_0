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
 * @file cse_drv.c
 *
 * @brief This file provides CSE integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "cse_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Pointer to runtime state structure */
static cse_state_t *s_cseStatePtr = NULL;

/* =====================================  Functions declaration  ==================================== */
static void CSE_InitState(cse_key_id_t keyId, cse_cmd_t cmd, const uint8_t *inBuff, uint8_t *outBuff, uint32_t length);
static void CSE_StartEncDecECBCmd(void);
static void CSE_ContinueEncDecECBCmd(void);
static void CSE_StartEncDecCBCCmd(void);
static void CSE_ContinueEncDecCBCCmd(void);
static void CSE_StartGenMACCmd(void);
static void CSE_ContinueGenMACCmd(void);
static void CSE_StartVerifMACCmd(void);
static void CSE_ContinueVerifMACCmd(void);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Calculates value to the first number multiple of roundTo.
 *
 * @param[in] value: rouding value
 * @param[in] roundTo: original value
 * @return Rounding results value
 */
static uint32_t CSE_RoundTo(uint32_t value, uint32_t roundTo)
{
    return (value + (roundTo - 1U)) & (~(roundTo - 1U));
}

/*!
 * @brief CSE driver initialize.
 *
 * @param[in] instance: state Pointer to the state structure
 * @return none
 */
void CSE_DRV_Init(cse_state_t *state)
{
    DEVICE_ASSERT(NULL != state);

    s_cseStatePtr = state;
    s_cseStatePtr->cmdInProgress = false;
}

/*!
 * @brief Uninitialize CSE module.
 *
 * @param[in] none
 * @return none
 */
void CSE_DRV_Deinit(void)
{
    s_cseStatePtr = NULL;
}

/*!
 * @brief Initializes the internal state of the driver.
 *
 * @param[in] keyId: Key ID
 * @param[in] cmd: CSE command
 * @param[in] inBuff: pointer to input buffer
 * @param[in] outBuff: pointer to output buffer
 * @param[in] length: init length
 * @return none
 */
static void CSE_InitState(cse_key_id_t keyId, cse_cmd_t cmd, const uint8_t *inBuff,
                          uint8_t *outBuff, uint32_t length)
{
    s_cseStatePtr->cmdInProgress = true;
    s_cseStatePtr->cmd = cmd;
    s_cseStatePtr->inputBuff = inBuff;
    s_cseStatePtr->outputBuff = outBuff;
    s_cseStatePtr->keyId = keyId;
    s_cseStatePtr->fullSize = length;
    s_cseStatePtr->index = 0U;
    s_cseStatePtr->errCode = STATUS_SUCCESS;
    s_cseStatePtr->seq = CSE_CALL_SEQ_FIRST;
}

/*!
 * @brief Performs the AES-128 encryption in ECB mode of the input plain text with the Key ID and returns the cipher text.
 *
 * @param[in] KeyID: Key ID that used to perform encryption in ECB mode
 * @param[in] plainText: pointer to the plain text buffer
 * @param[in] length: number in bytes of cipher text message to be encrypted
 * @param[out] cipherText: pointer to the cipher text buffer
 * @param[in] timeout: timeout value in milliseconds
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_EncryptECB(cse_key_id_t keyId, const uint8_t *plainText, uint32_t length,
                            uint8_t *cipherText, uint32_t timeout)
{
    DEVICE_ASSERT(NULL != plainText);
    DEVICE_ASSERT(NULL != cipherText);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_ENC_ECB, plainText, cipherText, length);
    CSE_StartEncDecECBCmd();

    while (s_cseStatePtr->cmdInProgress)
    {
        /* Wait until the execution of the command is complete */
        CSE_WaitCommandCompletion();
        CSE_ContinueEncDecECBCmd();
    }

    return s_cseStatePtr->errCode;
}

/*!
 * @brief Performs the AES-128 decryption in ECB mode of the input cipher text with the Key ID and returns the plain text.
 *
 * @param[in] KeyID: Key ID that used to perform encryption in ECB mode
 * @param[in] cipherText: pointer to the cipher text buffer
 * @param[in] length: number in bytes of cipher text message to be decrypted
 * @param[out] plainText: pointer to the plain text buffer
 * @param[in] timeout: timeout value in milliseconds
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_DecryptECB(cse_key_id_t keyId, const uint8_t *cipherText, uint32_t length,
                            uint8_t *plainText, uint32_t timeout)
{
    DEVICE_ASSERT(NULL != plainText);
    DEVICE_ASSERT(NULL != cipherText);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    CSE_InitState(keyId, CSE_CMD_DEC_ECB, cipherText, plainText, length);

    CSE_StartEncDecECBCmd();

    while (s_cseStatePtr->cmdInProgress)
    {
        CSE_WaitCommandCompletion();
        CSE_ContinueEncDecECBCmd();
    }

    return s_cseStatePtr->errCode;
}

/*!
 * @brief Performs the AES-128 encryption in CBC mode of the input plain text with the Key ID and returns the cipher text.
 *
 * @param[in] KeyID: Key ID that used to perform encryption in ECB mode
 * @param[in] plainText: pointer to the plain text buffer
 * @param[in] length: number in bytes of cipher text message to be encrypted
 * @param[in] iv: pointer to the initialization vector buffer
 * @param[out] cipherText: pointer to the cipher text buffer
 * @param[in] timeout: timeout value in milliseconds
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_EncryptCBC(cse_key_id_t keyId, const uint8_t *plainText, uint32_t length,
                            const uint8_t *iv, uint8_t *cipherText, uint32_t timeout)
{
    DEVICE_ASSERT(NULL != plainText);
    DEVICE_ASSERT(NULL != cipherText);
    DEVICE_ASSERT(NULL != iv);
    DEVICE_ASSERT(s_cseStatePtr != NULL);

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_ENC_CBC, plainText, cipherText, length);
    s_cseStatePtr->iv = iv;

    CSE_StartEncDecCBCCmd();

    while (s_cseStatePtr->cmdInProgress)
    {
        CSE_WaitCommandCompletion();
        CSE_ContinueEncDecCBCCmd();
    }

    return s_cseStatePtr->errCode;
}

/*!
 * @brief Performs the AES-128 decryption in CBC mode of the input cipher text with the Key ID and returns the plain text.
 *
 * @param[in] KeyID: Key ID that used to perform encryption in CBC mode
 * @param[in] cipherText: pointer to the cipher text buffer
 * @param[in] length: number in bytes of cipher text message to be decrypted
 * @param[in] iv: pointer to the initialization vector buffer
 * @param[out] plainText: pointer to the plain text buffer
 * @param[in] timeout: timeout value in milliseconds
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_DecryptCBC(cse_key_id_t keyId, const uint8_t *cipherText, uint32_t length,
                            const uint8_t *iv, uint8_t *plainText, uint32_t timeout)
{
    DEVICE_ASSERT(NULL != plainText);
    DEVICE_ASSERT(NULL != cipherText);
    DEVICE_ASSERT(NULL != iv);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_DEC_CBC, cipherText, plainText, length);
    s_cseStatePtr->iv = iv;

    CSE_StartEncDecCBCCmd();

    while (s_cseStatePtr->cmdInProgress)
    {
        CSE_WaitCommandCompletion();
        CSE_ContinueEncDecCBCCmd();
    }

    return s_cseStatePtr->errCode;
}

/*!
 * @brief Generate the MAC of a given message using CMAC with AES-128.
 *
 * @param[in] KeyID: Key ID that used to perform generate CMAC
 * @param[in] msg: pointer to the message buffer
 * @param[in] msgLen: number in bits of message which CMAC will be computed
 * @param[out] cmac: pointer to the CMAC buffer
 * @param[in] timeout: timeout value in milliseconds
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_GenerateMAC(cse_key_id_t keyId, const uint8_t *msg, uint32_t msgLen,
                             uint8_t *cmac, uint32_t timeout)
{
    DEVICE_ASSERT(NULL != msg);
    DEVICE_ASSERT(NULL != cmac);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_GENERATE_MAC, msg, cmac, CSE_RoundTo(msgLen, 0x8U) >> CSE_BYTES_TO_FROM_BITS_SHIFT);
    s_cseStatePtr->msgLen = msgLen;

    CSE_StartGenMACCmd();

    while (s_cseStatePtr->cmdInProgress)
    {
        CSE_WaitCommandCompletion();
        CSE_ContinueGenMACCmd();
    }

    return s_cseStatePtr->errCode;
}

/*!
 * @brief Generate the MAC of a given message(locate in FLASH) using CMAC with AES-128.
 *
 * @param[in] KeyID: Key ID that used to perform generate CMAC
 * @param[in] msg: pointer to the message buffer that pointing to Flash memory
 * @param[in] msgLen: number in bits of message which CMAC will be computed
 * @param[out] cmac: pointer to the CMAC buffer
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_GenerateMACAddrMode(cse_key_id_t keyId, const uint8_t *msg, uint32_t msgLen, uint8_t *cmac)
{
    DEVICE_ASSERT(NULL != msg);
    DEVICE_ASSERT(NULL != cmac);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandWords(FEATURE_CSE_FLASH_START_ADDRESS_OFFSET, (uint32_t *)&msg, 1U);
    CSE_WriteCommandWords(FEATURE_CSE_MESSAGE_LENGTH_OFFSET, &msgLen, 1U);
    CSE_WriteCmdAndWait(CSE_CMD_GENERATE_MAC, CSE_FUNC_FORMAT_ADDR, CSE_CALL_SEQ_FIRST, keyId);

    stat = CSE_ReadErrorBits();
    /* Read the resulted MAC */
    if (STATUS_SUCCESS == stat)
    {
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_2_OFFSET, cmac, CSE_PAGE_SIZE_IN_BYTES);
    }

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Verifies the MAC of a given message using CMAC with AES-128.
 *
 * @param[in] KeyID: Key ID that used to perform verify MAC
 * @param[in] msg: pointer to the message buffer
 * @param[in] msgLen: number in bits of message which CMAC will be verified
 * @param[in] mac: pointer to the CMAC buffer which CMAC will be verified
 * @param[in] macLen: number in bits of CMAC which CMAC will be verified
 * @param[out] verifStatus: status of MAC verification command
 *             - true: verification operation passed
 *             - false: verification operation failed
 * @param[in] timeout: timeout value in milliseconds
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_VerifyMAC(cse_key_id_t keyId, const uint8_t *msg, uint32_t msgLen, const uint8_t *mac,
                           uint16_t macLen, bool *verifStatus, uint32_t timeout)
{
    DEVICE_ASSERT(NULL != msg);
    DEVICE_ASSERT(NULL != mac);
    DEVICE_ASSERT(NULL != verifStatus);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_VERIFY_MAC, msg, NULL, CSE_RoundTo(msgLen, 0x8U) >> CSE_BYTES_TO_FROM_BITS_SHIFT);
    s_cseStatePtr->msgLen = msgLen;
    s_cseStatePtr->verifStatus = verifStatus;
    s_cseStatePtr->macWritten = false;
    s_cseStatePtr->mac = mac;
    s_cseStatePtr->macLen = macLen;

    CSE_StartVerifMACCmd();

    while (s_cseStatePtr->cmdInProgress)
    {
        /* Wait until the execution of the command is complete */
        CSE_WaitCommandCompletion();
        CSE_ContinueVerifMACCmd();
    }

    return s_cseStatePtr->errCode;
}

/*!
 * @brief Verifies the MAC of a given message(locate in FLASH) using CMAC with AES-128.
 *
 * @param[in] KeyID: Key ID that used to perform CMAC
 * @param[in] msg: pointer to the message buffer that pointing to Flash memory
 * @param[in] msgLen: number in bits of message which CMAC will be verified
 * @param[in] mac: pointer to the CMAC buffer which CMAC will be verified
 * @param[in] macLen: number in bits of CMAC which CMAC will be verified
 * @param[out] verifStatus: status of MAC verification command
 *             - true: verification operation passed
 *             - false: verification operation failed
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_VerifyMACAddrMode(cse_key_id_t keyId, const uint8_t *msg, uint32_t msgLen,
                                   const uint8_t *mac, uint16_t macLen, bool *verifStatus)
{
    DEVICE_ASSERT(NULL != msg);
    DEVICE_ASSERT(NULL != mac);
    DEVICE_ASSERT(NULL != verifStatus);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandWords(FEATURE_CSE_FLASH_START_ADDRESS_OFFSET, (uint32_t *)&msg, 1U);
    /* Write the MAC to be verified */
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_2_OFFSET, mac, CSE_PAGE_SIZE_IN_BYTES);
    /* Write the size of the message (in bits) */
    CSE_WriteCommandWords(FEATURE_CSE_MESSAGE_LENGTH_OFFSET, &msgLen, 1U);
    /* Write the number of bits of the MAC to be compared */
    CSE_WriteCommandHalfWord(FEATURE_CSE_MAC_LENGTH_OFFSET, macLen);
    /* Write the command header. This will trigger the command execution */
    CSE_WriteCmdAndWait(CSE_CMD_VERIFY_MAC, CSE_FUNC_FORMAT_ADDR, CSE_CALL_SEQ_FIRST, keyId);

    stat = CSE_ReadErrorBits();

    /* Read the result of the MAC verification */
    if (STATUS_SUCCESS == stat)
    {
        *verifStatus = (CSE_ReadCommandHalfWord(FEATURE_CSE_VERIFICATION_STATUS_OFFSET) == 0U);
    }

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Updates an internal key to CSE module.
 *
 * @param[in] KeyID: the key to be updated
 * @param[in] m1: pointer to the 128-bit M1 message buffer
 * @param[in] m2: pointer to the 128-bit M2 message buffer
 * @param[in] m3: pointer to the 128-bit M3 message buffer
 * @param[in] m4: pointer to the 128-bit M4 message buffer
 * @param[in] m5: pointer to the 128-bit M5 message buffer
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_LoadKey(cse_key_id_t keyId, const uint8_t *m1, const uint8_t *m2,
                         const uint8_t *m3, uint8_t *m4, uint8_t *m5)
{
    DEVICE_ASSERT(NULL != m1);
    DEVICE_ASSERT(NULL != m2);
    DEVICE_ASSERT(NULL != m3);
    DEVICE_ASSERT(NULL != m4);
    DEVICE_ASSERT(NULL != m5);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    /* Write the values of M1-M3 */
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, m1, CSE_M1_SIZE_IN_BYTES);
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_2_OFFSET, m2, CSE_M2_SIZE_IN_BYTES);
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_4_OFFSET, m3, CSE_M3_SIZE_IN_BYTES);
    /* Write the command header. This will trigger the command execution */
    CSE_WriteCommandHeader(CSE_CMD_LOAD_KEY, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, keyId);

    CSE_WaitCommandCompletion();

    stat = CSE_ReadErrorBits();

    /* Read the obtained M4 and M5 */
    if (STATUS_SUCCESS == stat)
    {
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_5_OFFSET, m4, CSE_M4_SIZE_IN_BYTES);
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_7_OFFSET, m5, CSE_M5_SIZE_IN_BYTES);
    }

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Updates the RAM key memory slot with a 128-bit plaintext.
 *
 * @param[in] plainKey: pointer to the 128-bit plain text
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_LoadPlainKey(const uint8_t *plainKey)
{
    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    /* Write the bytes of the key */
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, plainKey, CSE_PAGE_SIZE_IN_BYTES);
    /* Write the command header. This will trigger the command execution */
    CSE_WriteCommandHeader(CSE_CMD_LOAD_PLAIN_KEY, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_RAM_KEY);

    CSE_WaitCommandCompletion();

    stat = CSE_ReadErrorBits();
    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Exports the RAM_KEY into a format protected by SECRET_KEY.
 *
 * @param[out] m1: pointer to the 128-bit M1 message buffer that export from RAM KEY slot
 * @param[out] m2: pointer to the 128-bit M2 message buffer that export from RAM KEY slot
 * @param[out] m3: pointer to the 128-bit M3 message buffer that export from RAM KEY slot
 * @param[out] m4: pointer to the 128-bit M4 message buffer that export from RAM KEY slot
 * @param[out] m5: pointer to the 128-bit M5 message buffer that export from RAM KEY slot
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_ExportRAMKey(uint8_t *m1, uint8_t *m2, uint8_t *m3, uint8_t *m4, uint8_t *m5)
{
    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    /* Write the command header. This will trigger the command execution */
    CSE_WriteCommandHeader(CSE_CMD_EXPORT_RAM_KEY, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_RAM_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
    if (STATUS_SUCCESS == stat)
    {
        /* Read the M1-M5 values associated with the key */
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, m1, CSE_M1_SIZE_IN_BYTES);
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_2_OFFSET, m2, CSE_M2_SIZE_IN_BYTES);
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_4_OFFSET, m3, CSE_M3_SIZE_IN_BYTES);
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_5_OFFSET, m4, CSE_M4_SIZE_IN_BYTES);
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_7_OFFSET, m5, CSE_M5_SIZE_IN_BYTES);
    }

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Initializes the PRNG.
 *
 * @param[in] none
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_InitRNG(void)
{
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandHeader(CSE_CMD_INIT_RNG, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Extends the seed of the PRNG.
 *
 * @param[in] entropy: pointer to a 128-bit buffer containing the entropy
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_ExtendSeed(const uint8_t *entropy)
{
    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, entropy, CSE_PAGE_SIZE_IN_BYTES);
    CSE_WriteCommandHeader(CSE_CMD_EXTEND_SEED, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Extends the seed of the PRNG.
 *
 * @param[out] entropy: pointer to a 128-bit buffer containing the PRNG value
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_GenerateRND(uint8_t *rnd)
{
    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandHeader(CSE_CMD_RND, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
    /* Read the resulted random bytes */
    if (STATUS_SUCCESS == stat)
    {
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, rnd, CSE_PAGE_SIZE_IN_BYTES);
    }

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Mark failure boot verification.
 *
 * @param[in] none
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_BootFailure(void)
{
    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandHeader(CSE_CMD_BOOT_FAILURE, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Mark successful boot verification.
 *
 * @param[in] none
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_BootOK(void)
{
    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandHeader(CSE_CMD_BOOT_OK, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Define bootcode size and secure boot type.
 *
 * @param[in] bootSize: number of blocks of 128-bit data to check on boot
 * @param[in] bootFlavor: secure boot type
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_BootDefine(uint32_t bootSize, cse_boot_flavor_t bootFlavor)
{
    uint8_t flavor = (uint8_t)bootFlavor;
    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandWords(FEATURE_CSE_BOOT_SIZE_OFFSET, &bootSize, 1U);
    CSE_WriteCommandByte(FEATURE_CSE_BOOT_FLAVOR_OFFSET, flavor);
    CSE_WriteCommandHeader(CSE_CMD_BOOT_DEFINE, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Get the UID from CSE module.
 *
 * @param[in] challenge: pointer to the 128-bit buffer containing challenge data
 * @param[out] uid: pointer to 120 bit buffer containing UID data
 * @param[out] sreg: Value of the status register.
 * @param[out] mac：pointer to the 128 bit buffer containing MAC data
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_GetID(const uint8_t *challenge, uint8_t *uid, uint8_t *sreg, uint8_t *mac)
{
    DEVICE_ASSERT(NULL != challenge);
    DEVICE_ASSERT(NULL != uid);
    DEVICE_ASSERT(NULL != sreg);
    DEVICE_ASSERT(NULL != mac);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    /* Write the challenge */
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, challenge, CSE_PAGE_SIZE_IN_BYTES);
    CSE_WriteCommandHeader(CSE_CMD_GET_ID, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();

    stat = CSE_ReadErrorBits();
    if (STATUS_SUCCESS == stat)
    {
        /* Read the UID */
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_2_OFFSET, uid, (uint8_t)(CSE_PAGE_SIZE_IN_BYTES));
        /* Read the value of the SREG register */
        *sreg = CSE_ReadCommandByte(FEATURE_CSE_SREG_OFFSET);
        /* Read the MAC over the UID and the SREG */
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_3_OFFSET, mac, CSE_PAGE_SIZE_IN_BYTES);
    }

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Get a random challenge data.
 *
 * @param[out] challenge: pointer to the 128-bit buffer containing challenge data
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_DbgChal(uint8_t *challenge)
{
    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandHeader(CSE_CMD_DBG_CHAL, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
    /* Read the challenge generated by the CSE module */
    if (STATUS_SUCCESS == stat)
    {
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, challenge, CSE_PAGE_SIZE_IN_BYTES);
    }

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Erases all keys that stored in FLASH memory.
 *
 * @param[in] authorization:  pointer to the 128-bit buffer containing the authorization value that get from CSE_DRV_DbgChal
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_DbgAuth(const uint8_t *authorization)
{
    DEVICE_ASSERT(NULL != authorization);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_ERROR;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    s_cseStatePtr->cmdInProgress = true;

    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, authorization, CSE_PAGE_SIZE_IN_BYTES);
    CSE_WriteCommandHeader(CSE_CMD_DBG_AUTH, CSE_FUNC_FORMAT_COPY, CSE_CALL_SEQ_FIRST, CSE_SECRET_KEY);
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();

    s_cseStatePtr->cmdInProgress = false;

    return stat;
}

/*!
 * @brief Compresses the given messages.
 *
 * @param[in] msg: pointer to the messages which will be compressed
 * @param[in] msgLen: number in bits of message
 * @param[out] mpCompress: pointer to the 128 bit buffer storing the compressed data
 * @param[in] timeout: timeout value in milliseconds
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_MPCompress(const uint8_t *msg, uint16_t msgLen, uint8_t *mpCompress, uint32_t timeout)
{
    DEVICE_ASSERT(NULL != msg);
    DEVICE_ASSERT(NULL != mpCompress);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    cse_call_sequence_t seq = CSE_CALL_SEQ_FIRST;
    status_t stat = STATUS_SUCCESS;
    uint32_t index = 0U;
    uint16_t numPagesLeft = msgLen;

    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    /* Loop and launch commands until the end of the message */
    while (numPagesLeft > 0U)
    {
        uint8_t numPages = (uint8_t)((numPagesLeft > CSE_DATA_PAGES_AVAILABLE) ? \
                                     CSE_DATA_PAGES_AVAILABLE : numPagesLeft);
        uint8_t numBytes = (uint8_t)(numPages << CSE_BYTES_TO_FROM_PAGES_SHIFT);

        /* Write the message */
        CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, &msg[index], numBytes);
        /* Write the size of the message */
        CSE_WriteCommandHalfWord(FEATURE_CSE_PAGE_LENGTH_OFFSET, msgLen);
        /* Write the command header. This will trigger the command execution */
        CSE_WriteCommandHeader(CSE_CMD_MP_COMPRESS, CSE_FUNC_FORMAT_COPY, seq, CSE_SECRET_KEY);
        /* Wait until the execution of the command is complete */
        CSE_WaitCommandCompletion();

        stat = CSE_ReadErrorBits();
			
        if (STATUS_SUCCESS != stat)
        {
            break;
        }

        numPagesLeft = (uint16_t)(numPagesLeft - numPages);
        index = (uint32_t)(index + numBytes);
        if (seq == CSE_CALL_SEQ_FIRST)
        {
            seq = CSE_CALL_SEQ_SUBSEQUENT;
        }
    }

    /* Read the result of the compression */
    if (STATUS_SUCCESS == stat)
    {
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, mpCompress, CSE_PAGE_SIZE_IN_BYTES);
    }

    return stat;
}

/*!
 * @brief Asynchronously performs the AES-128 encryption in ECB mode.
 *
 * @param[in] KeyID: Key ID that used to perform encryption in ECB mode
 * @param[in] plainText: pointer to the plain text buffer
 * @param[in] length: number in bytes of cipher text message to be encrypted
 * @param[out] cipherText: pointer to the cipher text buffer
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_EncryptECBAsync(cse_key_id_t keyId, const uint8_t *plainText, uint32_t length, uint8_t *cipherText)
{
    DEVICE_ASSERT(NULL != plainText);
    DEVICE_ASSERT(NULL != cipherText);
    DEVICE_ASSERT(NULL != s_cseStatePtr);
	
		status_t stat = STATUS_SUCCESS;
	
    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_ENC_ECB, plainText, cipherText, length);
    CSE_StartEncDecECBCmd();
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
		
    return stat;
}

/*!
 * @brief Asynchronously performs the AES-128 decryption in ECB mode.
 *
 * @param[in] KeyID: Key ID that used to perform encryption in ECB mode
 * @param[in] cipherText: pointer to the cipher text buffer
 * @param[in] length: number in bytes of cipher text message to be decrypted
 * @param[out] plainText: pointer to the plain text buffer
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_DecryptECBAsync(cse_key_id_t keyId, const uint8_t *cipherText, uint32_t length, uint8_t *plainText)
{
    DEVICE_ASSERT(NULL != plainText);
    DEVICE_ASSERT(NULL != cipherText);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_SUCCESS;
	
    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_DEC_ECB, cipherText, plainText, length);
    CSE_StartEncDecECBCmd();
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
		
    return stat;
}

/*!
 * @brief Start the AES-128 encryption in CBC mode of
 * the input plain text buffer, in an asynchronous manner.
 *
 * @param[in] keyId:key id
 * @param[in] plainText:plian text buffer
 * @param[in] length:text length
 * @param[in] iV:initial vector buffer
 * @param[in] cipherText:cipher text buffer
 * @return status_t
 */
status_t CSE_DRV_EncryptCBCAsync(cse_key_id_t keyId, const uint8_t *plainText, uint32_t length,
                                 const uint8_t *iv, uint8_t *cipherText)
{
    DEVICE_ASSERT(NULL != plainText);
    DEVICE_ASSERT(NULL != cipherText);
    DEVICE_ASSERT(NULL != iv);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_SUCCESS;	
	
    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_ENC_CBC, plainText, cipherText, length);
    s_cseStatePtr->iv = iv;
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
		
    return stat;
}

/*!
 * @brief Asynchronously performs the AES-128 decryption in CBC mode.
 *
 * @param[in] KeyID: Key ID that used to perform encryption in CBC mode
 * @param[in] cipherText: pointer to the cipher text buffer
 * @param[in] length: number in bytes of cipher text message to be decrypted
 * @param[in] iv: pointer to the initialization vector buffer
 * @param[out] plainText: pointer to the plain text buffer
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_DecryptCBCAsync(cse_key_id_t keyId, const uint8_t *cipherText, uint32_t length,
                                 const uint8_t *iv, uint8_t *plainText)
{
    DEVICE_ASSERT(NULL != plainText);
    DEVICE_ASSERT(NULL != cipherText);
    DEVICE_ASSERT(NULL != iv);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_SUCCESS;	
	
    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_DEC_CBC, cipherText, plainText, length);
    s_cseStatePtr->iv = iv;
    CSE_StartEncDecCBCCmd();

    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
		
    return stat;
}

/*!
 * @brief Asynchronously generate the MAC of a given message using CMAC with AES-128.
 *
 * @param[in] KeyID: Key ID that used to perform generate CMAC
 * @param[in] msg: pointer to the message buffer
 * @param[in] length: number in bits of message which CMAC will be computed
 * @param[out] cmac: pointer to the CMAC buffer
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_GenerateMACAsync(cse_key_id_t keyId, const uint8_t *msg, uint32_t msgLen, uint8_t *cmac)
{
    DEVICE_ASSERT(NULL != msg);
    DEVICE_ASSERT(NULL != cmac);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_SUCCESS;
	
    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_GENERATE_MAC, msg, cmac, \
                  CSE_RoundTo(msgLen, 0x8U) >> CSE_BYTES_TO_FROM_BITS_SHIFT);
    s_cseStatePtr->msgLen = msgLen;
    CSE_StartGenMACCmd();
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
		
    return stat;
}

/*!
 * @brief Asynchronously verifies the MAC of a given message using CMAC with AES-128.
 *
 * @param[in] KeyID: Key ID that used to perform CMAC
 * @param[in] msg: pointer to the message buffer
 * @param[in] msgLen: number in bits of message which CMAC will be verified
 * @param[in] cmac: pointer to the CMAC buffer which CMAC will be verified
 * @param[in] macLen: number in bits of CMAC which CMAC will be verified
 * @param[out] verifStatus: status of MAC verification command
 *             - true: verification operation passed
 *             - false: verification operation failed
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_VerifyMACAsync(cse_key_id_t keyId, const uint8_t *msg, uint32_t msgLen,
                                const uint8_t *mac, uint16_t macLen, bool *verifStatus)
{
    DEVICE_ASSERT(NULL != msg);
    DEVICE_ASSERT(NULL != mac);
    DEVICE_ASSERT(NULL != verifStatus);
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    status_t stat = STATUS_SUCCESS;
	
    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }

    CSE_InitState(keyId, CSE_CMD_VERIFY_MAC, msg, NULL, \
                  CSE_RoundTo(msgLen, 0x8) >> CSE_BYTES_TO_FROM_BITS_SHIFT);
    s_cseStatePtr->msgLen = msgLen;
    s_cseStatePtr->verifStatus = verifStatus;
    s_cseStatePtr->macWritten = false;
    s_cseStatePtr->mac = mac;
    s_cseStatePtr->macLen = macLen;
    CSE_StartVerifMACCmd();
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
		
    return stat;
}

/*!
 * @brief Asynchronously get operation status.
 *
 * @param[in] none
 * @return operation status
 */
status_t CSE_DRV_GetAsyncCmdStatus(void)
{
    status_t stat = STATUS_SUCCESS;
	
    if (s_cseStatePtr->cmdInProgress)
    {
        return STATUS_BUSY;
    }
    CSE_WaitCommandCompletion();
    stat = CSE_ReadErrorBits();
		
    return stat;
}

/*!
 * @brief Implementation of the CSE interrupt handler.
 *
 * @param[in] none
 * @return none
 *
 */
void CSE_IRQHandler(void)
{
    uint8_t stat = CSE_GetCmdCompleteStatus();

    CSE_ClearCmdCompleteStatus();
    if ((stat != 0U) && s_cseStatePtr->cmdInProgress)
    {
        switch (s_cseStatePtr->cmd)
        {
        case CSE_CMD_ENC_ECB:
        case CSE_CMD_DEC_ECB:
            CSE_ContinueEncDecECBCmd();
            break;
        case CSE_CMD_ENC_CBC:
        case CSE_CMD_DEC_CBC:
            CSE_ContinueEncDecCBCCmd();
            break;
        case CSE_CMD_GENERATE_MAC:
            CSE_ContinueGenMACCmd();
            break;
        case CSE_CMD_VERIFY_MAC:
            CSE_ContinueVerifMACCmd();
            break;
        default:
            /* Do nothing */
            break;
        }

        /* Finished operation, disable CSE interrupt */
        if (!s_cseStatePtr->cmdInProgress)
        {
            CSE_SetInterrupt(false);

            if (s_cseStatePtr->callback != NULL)
            {
                s_cseStatePtr->callback((uint32_t)s_cseStatePtr->cmd, s_cseStatePtr->callbackParam);
            }
        }
    }
}

/*!
 * @brief Performs the AES-128 encryption and decryption in ECB mode.
 *
 * @param none
 * @return none
 */
static void CSE_StartEncDecECBCmd(void)
{
    uint32_t numPagesLeft = (s_cseStatePtr->fullSize - s_cseStatePtr->index) >> \
                            CSE_BYTES_TO_FROM_PAGES_SHIFT;
    uint16_t numPages = (uint16_t)((numPagesLeft > CSE_DATA_PAGES_AVAILABLE) ? \
                                   CSE_DATA_PAGES_AVAILABLE : numPagesLeft);
    uint8_t numBytes = (uint8_t)(numPages << CSE_BYTES_TO_FROM_PAGES_SHIFT);

    /* Write the plain/cipher text */
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, &s_cseStatePtr->inputBuff[s_cseStatePtr->index], numBytes);
    /* Write the size of the plain/cipher text (in pages) */
    CSE_WriteCommandHalfWord(FEATURE_CSE_PAGE_LENGTH_OFFSET, numPages);
    s_cseStatePtr->partSize = numBytes;
    CSE_WriteCommandHeader(s_cseStatePtr->cmd, CSE_FUNC_FORMAT_COPY, s_cseStatePtr->seq, s_cseStatePtr->keyId);
}

/*!
 * @brief Start performs the AES-128 encryption and decryption in CBC mode.
 *
 * @param none
 * @return none
 */
static void CSE_StartEncDecCBCCmd(void)
{
    uint32_t numPagesLeft = (s_cseStatePtr->fullSize - s_cseStatePtr->index) >> \
                            CSE_BYTES_TO_FROM_PAGES_SHIFT;
    uint16_t numPages = (uint16_t)((numPagesLeft > CSE_DATA_PAGES_AVAILABLE) ? \
                                   CSE_DATA_PAGES_AVAILABLE : numPagesLeft);
    uint8_t numBytes = (uint8_t)(numPages << CSE_BYTES_TO_FROM_PAGES_SHIFT);

    if (s_cseStatePtr->seq == CSE_CALL_SEQ_FIRST)
    {
        numPages = (uint16_t)((numPagesLeft > (CSE_DATA_PAGES_AVAILABLE - 1U)) ? \
                              (CSE_DATA_PAGES_AVAILABLE - 1U) : numPagesLeft);
        numBytes = (uint8_t)(numPages << CSE_BYTES_TO_FROM_PAGES_SHIFT);

        /* Write the initialization vector */
        CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, s_cseStatePtr->iv, CSE_PAGE_SIZE_IN_BYTES);
        /* Write the plain/cipher text */
        CSE_WriteCommandBytes(FEATURE_CSE_PAGE_2_OFFSET, &s_cseStatePtr->inputBuff[s_cseStatePtr->index], numBytes);
    }
    else
    {
        /* Write the plain/cipher text */
        CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, &s_cseStatePtr->inputBuff[s_cseStatePtr->index], numBytes);
    }

    CSE_WriteCommandHalfWord(FEATURE_CSE_PAGE_LENGTH_OFFSET, \
                             (uint16_t)(s_cseStatePtr->fullSize >> CSE_BYTES_TO_FROM_PAGES_SHIFT));
    s_cseStatePtr->partSize = numBytes;
    CSE_WriteCommandHeader(s_cseStatePtr->cmd, CSE_FUNC_FORMAT_COPY, s_cseStatePtr->seq, s_cseStatePtr->keyId);
}

/*!
 * @brief Start generate the MAC using CMAC with AES-128.
 *
 * @param none
 * @return none
 */
static void CSE_StartGenMACCmd(void)
{
    uint8_t numBytes = (uint8_t)(((s_cseStatePtr->fullSize - s_cseStatePtr->index) >
                                  CSE_DATA_BYTES_AVAILABLE) ? CSE_DATA_BYTES_AVAILABLE : \
                                 (s_cseStatePtr->fullSize - s_cseStatePtr->index));

    /* Write the plain/cipher text */
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, &s_cseStatePtr->inputBuff[s_cseStatePtr->index], numBytes);
    /* Write the size of the message (in bits) */
    CSE_WriteCommandWords(FEATURE_CSE_MESSAGE_LENGTH_OFFSET, &s_cseStatePtr->msgLen, 1U);
    s_cseStatePtr->partSize = numBytes;

    /* Write the command header. This will trigger the command execution */
    CSE_WriteCommandHeader(s_cseStatePtr->cmd, CSE_FUNC_FORMAT_COPY, s_cseStatePtr->seq, s_cseStatePtr->keyId);
}

/*!
 * @brief Start vrify the MAC using CMAC with AES-128.
 *
 * @param none
 * @return none
 */
static void CSE_StartVerifMACCmd(void)
{
    uint8_t numBytes = (uint8_t)(((s_cseStatePtr->fullSize - s_cseStatePtr->index) > \
                                  CSE_DATA_BYTES_AVAILABLE) ? CSE_DATA_BYTES_AVAILABLE : \
                                 (s_cseStatePtr->fullSize - s_cseStatePtr->index));
    uint8_t macOffset = (uint8_t)CSE_RoundTo(numBytes, 0x10U);

    /* Write the plain/cipher text */
    CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, &s_cseStatePtr->inputBuff[s_cseStatePtr->index], numBytes);
    /* Write the size of the message (in bits) */
    CSE_WriteCommandWords(FEATURE_CSE_MESSAGE_LENGTH_OFFSET, &s_cseStatePtr->msgLen, 1U);
    /* Write the number of bits of the MAC to be compared */
    CSE_WriteCommandHalfWord(FEATURE_CSE_MAC_LENGTH_OFFSET, (uint16_t)s_cseStatePtr->macLen);

    /* If there is available space in CSE_PRAM, write the MAC to be verified */
    if ((macOffset + CSE_PAGE_SIZE_IN_BYTES) < CSE_DATA_BYTES_AVAILABLE)
    {
        CSE_WriteCommandBytes(FEATURE_CSE_PAGE_1_OFFSET + macOffset, s_cseStatePtr->mac, CSE_PAGE_SIZE_IN_BYTES);
        s_cseStatePtr->macWritten = true;
    }

    s_cseStatePtr->partSize = numBytes;

    /* Write the command header. This will trigger the command execution */
    CSE_WriteCommandHeader(s_cseStatePtr->cmd, CSE_FUNC_FORMAT_COPY, s_cseStatePtr->seq, s_cseStatePtr->keyId);
}

/*!
 * @brief Continues perform encryption/decryption using ECB mode.
 *
 * @param none
 * @return none
 */
static void CSE_ContinueEncDecECBCmd(void)
{
    s_cseStatePtr->errCode = CSE_ReadErrorBits();
    if (s_cseStatePtr->errCode != STATUS_SUCCESS)
    {
        /* Do not continue launching commands if an error occurred */
        s_cseStatePtr->cmdInProgress = false;
        return;
    }
    /* Get partial result */
    CSE_ReadCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, &s_cseStatePtr->outputBuff[s_cseStatePtr->index], \
                         (uint8_t)s_cseStatePtr->partSize);
    s_cseStatePtr->index += (uint8_t)s_cseStatePtr->partSize;
    /* Decide if more commands are needed */
    if (s_cseStatePtr->index >= s_cseStatePtr->fullSize)
    {
        s_cseStatePtr->cmdInProgress = false;
    }
    else
    {
        /* Continue launching commands */
        CSE_StartEncDecECBCmd();
    }
}

/*!
 * @brief Continues perform encryption/decryption using CBC mode.
 *
 * @param none
 * @return none
 */
static void CSE_ContinueEncDecCBCCmd(void)
{
    /* Read the status of the execution */
    s_cseStatePtr->errCode = CSE_ReadErrorBits();
    if (s_cseStatePtr->errCode != STATUS_SUCCESS)
    {
        /* Do not continue launching commands if an error occurred */
        s_cseStatePtr->cmdInProgress = false;
        return;
    }

    /* Get partial result */
    if (s_cseStatePtr->seq == CSE_CALL_SEQ_FIRST)
    {
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_2_OFFSET, &s_cseStatePtr->outputBuff[s_cseStatePtr->index], \
                             (uint8_t)s_cseStatePtr->partSize);
        s_cseStatePtr->seq = CSE_CALL_SEQ_SUBSEQUENT;
    }
    else
    {
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_1_OFFSET, &s_cseStatePtr->outputBuff[s_cseStatePtr->index], \
                             (uint8_t)s_cseStatePtr->partSize);
    }

    s_cseStatePtr->index += (uint8_t)s_cseStatePtr->partSize;

    if (s_cseStatePtr->index >= s_cseStatePtr->fullSize)
    {
        s_cseStatePtr->cmdInProgress = false;
    }
    else
    {
        /* Continue launching commands */
        CSE_StartEncDecCBCCmd();
    }
}

/*!
 * @brief Continues generate the MAC using CMAC with AES-128.
 *
 * @param none
 * @return none
 */
static void CSE_ContinueGenMACCmd(void)
{
    /* Read the status of the execution */
    s_cseStatePtr->errCode = CSE_ReadErrorBits();
    if (s_cseStatePtr->errCode != STATUS_SUCCESS)
    {
        /* Do not continue launching commands if an error occurred */
        s_cseStatePtr->cmdInProgress = false;
        return;
    }

    if (s_cseStatePtr->seq == CSE_CALL_SEQ_FIRST)
    {
        s_cseStatePtr->seq = CSE_CALL_SEQ_SUBSEQUENT;
    }

    s_cseStatePtr->index += (uint8_t)s_cseStatePtr->partSize;

    /* Decide if more commands are needed */
    if (s_cseStatePtr->index >= s_cseStatePtr->fullSize)
    {
        s_cseStatePtr->cmdInProgress = false;
        CSE_ReadCommandBytes(FEATURE_CSE_PAGE_2_OFFSET, s_cseStatePtr->outputBuff, CSE_PAGE_SIZE_IN_BYTES);
    }
    else
    {
        /* Continue launching commands */
        CSE_StartGenMACCmd();
    }
}

/*!
 * @brief Continues verify the MAC using CMAC with AES-128.
 *
 * @param none
 * @return none
 */
static void CSE_ContinueVerifMACCmd(void)
{
    /* Read the status of the execution */
    s_cseStatePtr->errCode = CSE_ReadErrorBits();
    if (s_cseStatePtr->errCode != STATUS_SUCCESS)
    {
        /* Do not continue launching commands if an error occurred */
        s_cseStatePtr->cmdInProgress = false;
        return;
    }

    if (s_cseStatePtr->seq == CSE_CALL_SEQ_FIRST)
    {
        s_cseStatePtr->seq = CSE_CALL_SEQ_SUBSEQUENT;
    }

    s_cseStatePtr->index += (uint8_t)s_cseStatePtr->partSize;

    /* Decide if more commands are needed */
    s_cseStatePtr->cmdInProgress = !s_cseStatePtr->macWritten;

    if (!s_cseStatePtr->cmdInProgress)
    {
        *(s_cseStatePtr->verifStatus) = (CSE_ReadCommandHalfWord(FEATURE_CSE_VERIFICATION_STATUS_OFFSET) == 0U);

    }
    else
    {
        /* Continue launching commands */
        CSE_StartVerifMACCmd();
    }
}

/*!
 * @brief Installs a callback function for CSE driver.
 *
 * @param[in] callbackFunc: The function to be invoked
 * @param[in] callbackParam: The parameter to be passed to the callback function
 * @return none
 */
void CSE_DRV_InstallCallback(security_callback_t callbackFunc, void *callbackParam)
{
    DEVICE_ASSERT(s_cseStatePtr != NULL);

    s_cseStatePtr->callback = callbackFunc;
    s_cseStatePtr->callbackParam = callbackParam;
}

/*!
 * @brief Cancels a previously launched asynchronous command
 *
 * @param none
 * @return none
 */
void CSE_DRV_CancelCommand(void)
{
    DEVICE_ASSERT(NULL != s_cseStatePtr);

    if (s_cseStatePtr->cmdInProgress)
    {
        CSE_WaitCommandCompletion();

        if ((s_cseStatePtr->cmd != CSE_CMD_ENC_ECB) && (s_cseStatePtr->cmd != CSE_CMD_DEC_ECB))
        {
            /* Was there any command already launched? If so, break the sequence */
            if (s_cseStatePtr->fullSize != s_cseStatePtr->partSize)
            {
                /* Write the command header. CallSeq is set to 0 in order to trigger a command
                 * that will generate a sequence error, breaking the chain of calls */
                CSE_WriteCommandHeader(s_cseStatePtr->cmd, CSE_FUNC_FORMAT_COPY, \
                                       CSE_CALL_SEQ_FIRST, s_cseStatePtr->keyId);

                /* Wait until the execution of the command is complete */
                CSE_WaitCommandCompletion();
            }
        }

        s_cseStatePtr->cmdInProgress = false;
    }
}

/*!
 * @brief Returns the content of the status register.
 *
 * @param[in] none
 * @return Value of the status register
 */
uint32_t CSE_DRV_GetStatus(void)
{
    return CSE_GetStatus();
}

/* =============================================  EOF  ============================================== */
