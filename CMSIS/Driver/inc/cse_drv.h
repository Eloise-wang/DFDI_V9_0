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
 * @file cse_drv.h
 *
 * @brief This file provides CSE integration functions interface.
 *
 */

#ifndef _CSE_DRV_H
#define _CSE_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< Callback typedef for security module */
typedef void (*security_callback_t)(uint32_t completedCmd, void *callbackParam);

typedef uint8_t cse_status_t;

/*!
 * @brief Specify the KeyID to be used to implement the requested cryptographic operation.
 */
typedef enum {
    CSE_SECRET_KEY = 0x0U,
    CSE_MASTER_ECU,
    CSE_BOOT_MAC_KEY,
    CSE_BOOT_MAC,
    CSE_KEY_1,
    CSE_KEY_2,
    CSE_KEY_3,
    CSE_KEY_4,
    CSE_KEY_5,
    CSE_KEY_6,
    CSE_KEY_7,
    CSE_KEY_8,
    CSE_KEY_9,
    CSE_KEY_10,
    CSE_RAM_KEY = 0xFU,
    CSE_KEY_11  = 0x14U,
    CSE_KEY_12,
    CSE_KEY_13,
    CSE_KEY_14,
    CSE_KEY_15,
    CSE_KEY_16,
    CSE_KEY_17
} cse_key_id_t;

/*!
 * @brief CSE commands which follow the same values as the SHE command definition.
 */
typedef enum {
    CSE_CMD_ENC_ECB = 0x1U,
    CSE_CMD_ENC_CBC,
    CSE_CMD_DEC_ECB,
    CSE_CMD_DEC_CBC,
    CSE_CMD_GENERATE_MAC,
    CSE_CMD_VERIFY_MAC,
    CSE_CMD_LOAD_KEY,
    CSE_CMD_LOAD_PLAIN_KEY,
    CSE_CMD_EXPORT_RAM_KEY,
    CSE_CMD_INIT_RNG,
    CSE_CMD_EXTEND_SEED,
    CSE_CMD_RND,
    CSE_CMD_RESERVED_1,
    CSE_CMD_BOOT_FAILURE,
    CSE_CMD_BOOT_OK,
    CSE_CMD_GET_ID,
    CSE_CMD_BOOT_DEFINE,
    CSE_CMD_DBG_CHAL,
    CSE_CMD_DBG_AUTH,
    CSE_CMD_CANCEL,
    CSE_CMD_MP_COMPRESS,
    CSE_CMD_RESERVED_2
} cse_cmd_t;

/*!
 * @brief Specifies how the data is transferred to/from the CSE.
 */
typedef enum {
    CSE_FUNC_FORMAT_COPY,
    CSE_FUNC_FORMAT_ADDR
} cse_func_format_t;

/*!
 * @brief Specifies if the information is the first or a following function call.
 */
typedef enum {
    CSE_CALL_SEQ_FIRST,
    CSE_CALL_SEQ_SUBSEQUENT
} cse_call_sequence_t;

/*!
 * @brief Specifies the boot type for the BOOT_DEFINE command.
 */
typedef enum {
    CSE_BOOT_STRICT,
    CSE_BOOT_SERIAL,
    CSE_BOOT_PARALLEL,
    CSE_BOOT_NOT_DEFINED
} cse_boot_flavor_t;

/*!
 * @brief Internal driver state information.
 */
typedef struct {
    bool cmdInProgress;           /*!< Specifies if a command is in progress */
    cse_cmd_t cmd;                /*!< Specifies the type of the command in execution */
    const uint8_t *inputBuff;     /*!< Specifies the input of the command in execution */
    uint8_t *outputBuff;          /*!< Specifies the output of the command in execution */
    uint32_t index;               /*!< Specifies the index in the input buffer of the command in execution */
    uint32_t fullSize;            /*!< Specifies the size of the input of the command in execution */
    uint32_t partSize;            /*!< Specifies the size of the chunck of the input currently processed */
    cse_key_id_t keyId;           /*!< Specifies the key used for the command in execution */
    status_t errCode;             /*!< Specifies the error code of the last executed command */
    const uint8_t *iv;            /*!< Specifies the IV of the command in execution (for encryption/decryption using CBC mode) */
    cse_call_sequence_t seq;      /*!< Specifies if the information is the first or a following function call. */
    uint32_t msgLen;              /*!< Specifies the message size (in bits) for the command in execution (for MAC generation/verification) */
    bool *verifStatus;            /*!< Specifies the result of the last executed MAC verification command */
    bool macWritten;              /*!< Specifies if the MAC to be verified was written in CSE_PRAM for a MAC verification command */
    const uint8_t *mac;           /*!< Specifies the MAC to be verified for a MAC verification command */
    uint32_t macLen;              /*!< Specifies the number of bits of the MAC to be verified for a MAC verification command */
    security_callback_t callback; /*!< The callback invoked when an asynchronous command is completed */
    void *callbackParam;          /*!< User parameter for the command completion callback */
} cse_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief CSE driver initialize.
 *
 * @param[in] instance: state Pointer to the state structure
 * @return none
 */
void CSE_DRV_Init(cse_state_t *state);

/*!
 * @brief Uninitialize CSE module and disables the CSE interrupt.
 *
 * @param[in] none
 * @return none
 */
void CSE_DRV_Deinit(void);

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
                            uint8_t *cipherText, uint32_t timeout);

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
                            uint8_t *plainText, uint32_t timeout);

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
                            const uint8_t *iv, uint8_t *cipherText, uint32_t timeout);

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
                            const uint8_t *iv, uint8_t *plainText, uint32_t timeout);

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
                             uint8_t *cmac, uint32_t timeout);

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
status_t CSE_DRV_GenerateMACAddrMode(cse_key_id_t keyId, const uint8_t *msg, uint32_t msgLen, uint8_t *cmac);

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
                           uint16_t macLen, bool *verifStatus, uint32_t timeout);

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
                                   const uint8_t *mac, uint16_t macLen, bool *verifStatus);

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
                         const uint8_t *m3, uint8_t *m4, uint8_t *m5);

/*!
 * @brief Updates the RAM key memory slot with a 128-bit plaintext.
 *
 * @param[in] plainKey: pointer to the 128-bit plain text
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_LoadPlainKey(const uint8_t *plainKey);

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
status_t CSE_DRV_ExportRAMKey(uint8_t *m1, uint8_t *m2, uint8_t *m3, uint8_t *m4, uint8_t *m5);

/*!
 * @brief Initializes the PRNG.
 *
 * @param[in] none
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_InitRNG(void);

/*!
 * @brief Extends the seed of the PRNG.
 *
 * @param[in] entropy: pointer to a 128-bit buffer containing the entropy
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_ExtendSeed(const uint8_t *entropy);

/*!
 * @brief Extends the seed of the PRNG.
 *
 * @param[out] entropy: pointer to a 128-bit buffer containing the PRNG value
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_GenerateRND(uint8_t *rnd);

/*!
 * @brief Mark failure boot verification.
 *
 * @param[in] none
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_BootFailure(void);

/*!
 * @brief Mark successful boot verification.
 *
 * @param[in] none
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_BootOK(void);

/*!
 * @brief Define bootcode size and secure boot type.
 *
 * @param[in] bootSize: number of blocks of 128-bit data to check on boot
 * @param[in] bootFlavor: secure boot type
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_BootDefine(uint32_t bootSize, cse_boot_flavor_t bootFlavor);

/*!
 * @brief Returns the content of the status register.
 *
 * @param[in] none
 * @return Value of the status register
 */
uint32_t CSE_DRV_GetStatus(void);

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
status_t CSE_DRV_GetID(const uint8_t *challenge, uint8_t *uid, uint8_t *sreg, uint8_t *mac);

/*!
 * @brief Get a random challenge data.
 *
 * @param[out] challenge: pointer to the 128-bit buffer containing challenge data
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_DbgChal(uint8_t *challenge);

/*!
 * @brief Erases all keys that stored in FLASH memory.
 *
 * @param[in] authorization:  pointer to the 128-bit buffer containing the authorization value that get from CSE_DRV_DbgChal
 * @return operation status
 *         - STATUS_SUCCESS: operation was successful
 *         - Other Valule: operation was fail
 */
status_t CSE_DRV_DbgAuth(const uint8_t *authorization);

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
status_t CSE_DRV_MPCompress(const uint8_t *msg, uint16_t msgLen, uint8_t *mpCompress, uint32_t timeout);

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
status_t CSE_DRV_EncryptECBAsync(cse_key_id_t keyId, const uint8_t *plainText, uint32_t length, uint8_t *cipherText);

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
status_t CSE_DRV_DecryptECBAsync(cse_key_id_t keyId, const uint8_t *cipherText, uint32_t length, uint8_t *plainText);

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
                                 const uint8_t *iv, uint8_t *cipherText);

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
                                 const uint8_t *iv, uint8_t *plainText);

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
status_t CSE_DRV_GenerateMACAsync(cse_key_id_t keyId, const uint8_t *msg, uint32_t msgLen, uint8_t *cmac);

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
                                const uint8_t *mac, uint16_t macLen, bool *verifStatus);

/*!
 * @brief Asynchronously get operation status.
 *
 * @param[in] none
 * @return operation status
 */
status_t CSE_DRV_GetAsyncCmdStatus(void);

/*!
 * @brief Installs a callback function for CSE driver
 *
 * @param[in] callbackFunc: The function to be invoked
 * @param[in] callbackParam: The parameter to be passed to the callback function
 * @return none
 */
void CSE_DRV_InstallCallback(security_callback_t callbackFunc, void *callbackParam);

/*!
 * @brief Cancels a previously launched asynchronous command.
 *
 * @param none
 * @return none
 */
void CSE_DRV_CancelCommand(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CSE_DRV_H */

/* =============================================  EOF  ============================================== */
