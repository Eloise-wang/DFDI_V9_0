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
 * @file flash_drv.h
 *
 * @brief This file provides flash integration functions interface.
 *
 */

#ifndef _FLASH_DRV_H
#define _FLASH_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
/*!< FLASH status error bits */
#define FLASH_STAT_ERROR_BITS                   (FLASH_STAT_DFDIF_Msk       | FLASH_STAT_PVIOLF_Msk     | \
                                                 FLASH_STAT_VERIFYERR_Msk   | FLASH_STAT_ACCERRF_Msk    | \
                                                 FLASH_STAT_COLLERRF_Msk    | FLASH_STAT_FRAMECC1F_Msk  | \
                                                 FLASH_STAT_FRAMECC2F_Msk)
/*!< Clear FLASH error bits */
#define FLASH_CLEAR_STAT_ERROR_BITS()           FLASH->STAT |= FLASH_STAT_ERROR_BITS;

/* ===========================================  Typedef  ============================================ */
/*!< Callback typedef for FLASH module */
typedef void (*flash_callback_t)(void);

/*!
 * @brief FLASH user Configuration Structure.
 */
typedef struct
{
    uint32_t pFlashBase;          /*!< The base address of P-Flash memory */
    uint32_t pFlashSize;          /*!< The size in byte of P-Flash memory */
    uint32_t dFlashBase;          /*!< The base address of D-Flash memory */
    uint32_t flexRAMBase;         /*!< The base address of FlexRAM */
    flash_callback_t callback;    /*!< Call back function */
} flash_user_config_t;

/*!
 * @brief FLASH Configuration Structure.
 */
typedef struct
{
    uint32_t pFlashBase;          /*!< The base address of P-Flash memory */
    uint32_t pFlashSize;          /*!< The size in byte of P-Flash memory */
    uint32_t dFlashBase;          /*!< The base address of D-Flash memory */
    uint32_t dFlashSize;          /*!< The size in byte of D-Flash memory */
    uint32_t flexRAMBase;         /*!< The base address of FlexRAM */
    uint32_t flexRAMSize;         /*!< CSE flexRAM size */
    flash_callback_t callback;    /*!< Call back function */
} flash_config_t;

/*!
 * @brief FlexRAM Function control Code.
 */
typedef enum
{
    FUN_ENABLE                         = 0x00U,  /*!< FlexRAM used for CSE */
    FUN_DISABLE                        = 0xFFU   /*!< FlexRAM used for RAM */
} flash_flexram_function_code_t;

/*!
 * @brief FLASH command.
 */
typedef enum
{
    FLASH_ERASE_PAGE                    = 0x01U,    /*!< FLASH page erase command */
    FLASH_ERASE_BLOCK                   = 0x02U,    /*!< FLASH block erase command */
    FLASH_WRITE_PROTECT_ERASE           = 0x03U,    /*!< FLASH write protection erase command */
    FLASH_ERASE_ALL_BLOCK               = 0x04U,    /*!< FLASH erase all block erase command */
    FLASH_PROGRAM_PHRASE                = 0x10U,    /*!< FLASH program command */
    FLASH_PROGRAM_SECTION               = 0x11U,    /*!< FLASH section prigram command */
    FLASH_PROGRAM_ONCE                  = 0x12U,    /*!< FLASH once field program command */
    FLASH_VERIFY_SECTION                = 0x20U,    /*!< FLASH verify section command */
    FLASH_VERIFY_BLOCK                  = 0x21U,    /*!< FLASH block verify command */
    FLASH_VERIFY_ALL_BLOCK              = 0x22U,    /*!< FLASH all block verify command */
    FLASH_SECURITY_BY_PASS              = 0x23U,    /*!< FLASH bypass security command */
    FLASH_ERASE_PARTITION               = 0x30U,    /*!< FLASH erase partition command */
    FLASH_PROGRAM_PARTITION             = 0x31U,    /*!< FLASH program partition command */
    FLASH_SET_FLEXRAM                   = 0x32U,    /*!< FLASH set flexRAM function command */
    FLASH_PROGRAM_CSE                   = 0x33U,    /*!< FLASH set UID or secret key */
}flash_command_t;

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Unlock the FLASH register.
 *
 * This API sets key value 0xac7840 and 0x01234567 to KEYUNLK register to unlock FLASH.
 * Unlock must be done firstly before write FLASH register. if the key value is wrong,
 * FLASH will be locked and can't write FLASH register
 *
 * @param[in] none
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_TIMEOUT: Operation timeout was occurred
 */
status_t FLASH_DRV_UnlockCtrl(void);

/*!
 * @brief Lock the FLASH register.
 *
 * This API sets any key value except(0xac7840 and 0x01234567) to KEYUNLK register
 * to lock FLASH.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_LockCtrl(void);

/*!
 * @brief Initializes FLASH module.
 *
 * @param[in] userConfig: The user configuration structure pointer
 * @param[in] config: The FLASH configuration structure pointer
 * @return none
 */
void FLASH_DRV_Init(const flash_user_config_t * const userConfig, flash_config_t * const config);

/*!
 * @brief Get P-Flash write protection status.
 *
 * @param[out] protectStatus: Return the current status value of the P-Flash write
 *                          Protection register. Each bit is corresponding to
 *                          protection of 1/32 of the total P-Flash. The lowest
 *                          bit is corresponding to the lowest address area of P-Flash.
 *                          The highest bit is corresponding to the highest address
 *                          area of P-Flash
 *                          Each bit has the following value:
 *                          - 0: this area is protected
 *                          - 1: this area is unprotected
 * @return none
 */
void FLASH_DRV_GetPFlashProtection(uint32_t *protectStatus);

/*!
 * @brief Set P-Flash write protection status.
 *
 * @param[in] protectStatus: Write Protection value which set to the P-Flash write
 *                          protection register. Each bit is corresponding to protection
 *                          of 1/32 of the total P-Flash. The lowest bit is corresponding
 *                          to the lowest address area of P-Flash.The highest bit is
 *                          corresponding to the highest address area of P-Flash.
 *                          Each bit has the following meanings:
 *                          - 0: this area is protected
 *                          - 1: this area is unprotected
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 */
status_t FLASH_DRV_SetPFlashProtection(uint32_t protectStatus);

/*!
 * @brief Get D-Flash write protection status.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[out] protectStatus: Return the the D-Flash write Protection register value.
 *                           Each bit is corresponding to protection status of 1/8
 *                           of the D-Flash. The lowest bit is corresponding to the
 *                           lowest address area of D-Flash. The highest bit is
 *                           corresponding to the highest address area of D-Flash.
 *                           There are two possible cases as below:
 *                           - 0 : this area is protected
 *                           - 1 : this area is unprotected
 * @return operation status
 *        - STATUS_SUCCESS:     Operation was successful
 *        - STATUS_UNSUPPORTED: Operation was unsupported
 */
status_t FLASH_DRV_GetDFlashProtection(const flash_config_t * config, uint8_t * protectStatus);

/*!
 * @brief Set D-Flash write protection status.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] protectStatus: Write Protection value which set to the D-Flash write
 *                          protection register. Each bit is corresponding to
 *                          protection status of 1/8 of the total D-Flash. The least
 *                          significant bit is corresponding to the lowest address
 *                          area of D-Flash. The most significant bit is corresponding
 *                          to the highest address area of D-Flash.
 *                          There are two possible cases as below:
 *                          - 0 : this area is protected
 *                          - 1 : this area is unprotected
 * @return operation status
 *        - STATUS_SUCCESS:     Operation was successful
 *        - STATUS_ERROR:       Operation failure was occurred
 *        - STATUS_UNSUPPORTED: Operation was unsupported
 */
status_t FLASH_DRV_SetDFlashProtection(const flash_config_t * config, uint8_t protectStatus);

/*!
 * @brief Get FLASH security state.
 *
 * @param[out] securityState: get the current FLASH security status
 * @return operation status: none
 */
void FLASH_DRV_GetSecurityState(uint8_t * securityState);

/*!
 * @brief FLASH bypass security.
 *
 * This API Bypass FLASH securityby match user's back door key with
 * the back door key which saved in the information area.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] key: Point to the the user back door key
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_SecurityBypass(const flash_config_t *config, const uint8_t * key);

/*!
 * @brief Erase FLASH all blocks(P-FLash & D-FLash).
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_EraseAllBlock(const flash_config_t *config);

/*!
 * @brief Verify FLASH all blocks(P-FLash & D-FLash).
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_VerifyAllBlock(const flash_config_t *config);

/*!
 * @brief Flash erase sector.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] addr: Erase sector start address
 * @param[in] size: Erase size in bytes
 * @return operation status
 *        - STATUS_SUCCESS:     Operation was successful.
 *        - STATUS_ERROR:       Operation was failure.
 *        - STATUS_UNSUPPORTED: Operation was unsupported.
 *        - STATUS_BUSY:        Operation was busy.
 */
status_t FLASH_DRV_EraseSector(const flash_config_t *config, uint32_t addr, uint32_t size);

/*!
 * @brief FLASH section verify.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] addr: Start address for verify operation
 * @param[in] size: Verify size in 64-bit
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - STATUS_ERROR:   Operation was failure.
 *        - STATUS_BUSY:    Operation was busy.
 */
status_t FLASH_DRV_VerifySection(const flash_config_t *config, uint32_t addr, uint32_t size);

/*!
 * @brief Abort a current operation of FLASH erase sector command.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_AbortErase(void);

/*!
 * @brief FLASH program.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] addr: Program start address
 * @param[in] size: Program size in byte
 * @param[in] data: Pointer of data source address from which will be programmed
 * @return operation status
 *        - STATUS_SUCCESS:     Operation was successful.
 *        - STATUS_ERROR:       Operation was failure.
 *        - STATUS_UNSUPPORTED: Operation was unsupported.
 *        - STATUS_BUSY:        Operation was busy.
 */
status_t FLASH_DRV_Program(const flash_config_t *config, uint32_t addr, uint32_t size, const uint8_t *data);

/*!
 * @brief FLASH program section.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] addr: Program start address
 * @param[in] size: Program size in 64-bit
 * @return operation status
 *        - STATUS_SUCCESS:     Operation was successful
 *        - STATUS_ERROR:       Operation was failure
 *        - STATUS_UNSUPPORTED: Operation was unsupported
 *        - STATUS_BUSY:        Operation was busy
 */
status_t FLASH_DRV_ProgramSection(const flash_config_t *config, uint32_t addr, uint16_t size);

/*!
 * @brief FLASH erase block.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] addr: Erase start address
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_EraseBlock(const flash_config_t *config, uint32_t addr);

/*!
 * @brief FLASH verify block.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] addr: Verify start address
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_VerifyBlock(const flash_config_t *config, uint32_t addr);

/*!
 * @brief Erase FLASH write protection info page.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_EraseWriteProtection(const flash_config_t *config);

/*!
 * @brief Set write protection data to specified info address.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] data: Pointer of source write protection data address
 * @param[in] size: Size in byte to be programmed
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    peration was busy
 */
status_t FLASH_DRV_SetWriteProtection(const flash_config_t *config, const uint8_t *data, uint32_t size);

/*!
 * @brief Set FLASH security to specified info address.
 *
 * This API can program security byte to which address space is 0x00200010~0x00200017.
 * It only can be erased by erase all blocks unprotected command before programming.
 * security byte. Suggest to call this API when code will not be changed. Once read
 * protection enabled and erase all blocks unprotected disable take effect at the same
 * time after reset, The debugger will can't program and erase flash.
 * Bit[1:0]: Read protection
 *           2'b11: no read protection
 *           else : read protection, Debugger access memory resource will not be allowed.
 * Bit[3:2]: Erase all blocks unprotected enable
             2'b11: enable
 *           else : disable, debugger can't remove read protection when flash is in read
 *                 protection. this bit does not matter when has no read protection.
 * Bit[5:4]: Back door key enable
 *           2b'11: disable
 *           else : enable
 * Bit[7:6]: Grant key enable
 *           2'b11: enable
 *           else : disable
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] data: Pointer of source security byte address
 * @param[in] size: Size in byte to be programmed
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_SetSecurity(const flash_config_t *config, const uint8_t *data, uint32_t size);

/*!
 * @brief Set grant key data to specified info address.
 *
 * This API can set grant key by program option page which address space is
 * 0x00200000 and length is 8 bytes. It only can be erased by erase all blocks
 * unprotected command before programming.
 * Grant key should be enabled before factory failure analysis and default
 * grant key is enabled. User can call API FLASH_SetSecurity to program grant
 * key enable or disable.
 * Grant key should not config with all '1' or '0', otherwise factory verify
 * grant key will fail.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] data: Pointer of source grant key address
 * @param[in] size: Size in byte to be programmed
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_SetGrantKey(const flash_config_t *config, const uint8_t *data, uint32_t size);

/*!
 * @brief Set back door key data to specified info address.
 *
 * This API can set back door key by program option page which address space is
 * 0x00200008 and length is 8 bytes. It only can be erased by erase all blocks
 * unprotected command before programming.
 * Back door key should be enabled before verify back door key and default back
 * door key is disable. User can call API FLASH_SetSecurity to program back
 * door key enable or disable.
 * Back door key should not config with all '1' or '0', otherwise execute back
 * back door key caommand verify will fail.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] data: Pointer of source back door key address
 * @param[in] size: Size in byte to be programmed
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_SetBackDoorKey(const flash_config_t *config, const uint8_t *data, uint32_t size);

/*!
 * @brief Read a certain size of data from the flash start specified address.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] addr: Read start address
 * @param[out] data: Buffer for saving read data
 * @param[in] size: read size in bytes
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 */
status_t FLASH_DRV_Read(const flash_config_t *config, uint32_t addr, uint8_t *data, uint32_t size);

/*!
 * @brief FLASH read 64-bit data from once program field specified index.
 *
 * @param[in] index: The read index. It can be from 0x0U to 0xFFU
 * @param[out] data: Pointer to the address for saving read data from once program field
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_ReadOnce(uint8_t index, uint8_t *data);

/*!
 * @brief FLASH write 64-bit data to once program field.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] index: The write index. It can be from 0x0U to 0xFFU
 * @param[in] data: Pointer to the source data address which will be programmed to once
 *                        command program field
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_ProgramOnce(const flash_config_t *config, uint8_t index, const uint8_t *data);

/*!
 * @brief Set flexRAM function.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] funcCode: FlexRam function code. It can be:
 *                            - 0x00U: FlexRAM used for CSE
 *                            - 0xFFU: FlexRAM used for RAM
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_SetFlexRamFunction(const flash_config_t *config, flash_flexram_function_code_t funcCode);

/*!
 * @brief Set Partition.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @param[in] dFlashPartCode: reserved parameter
 * @param[in] cseKeySize: CSE Key Size Code, it should be 0 if not config as CSE function.
 *            It can be:
 *             - 0x00U: Number of User Keys is Zero, CSE key size is 0 Bytes
 *             - 0x01U: Number of User Keys is 1 to 6 keys, CSE key size is 128 Bytes
 *             - 0x02U: Number of User Keys is 1 to 12 keys, CSE key size is 256 Bytes
 *             - 0x03U: Number of User Keys is 1 to 24 keys, CSE key size is 512 Bytes
* @param[in] sfe: Security Flag Extension, it should be false if not onfig as CSE
 *           function
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_Partition(const flash_config_t *config, uint8_t dFlashPartCode, uint8_t cseKeySize, bool sfe);

/*!
 * @brief Erase Partition.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_ErasePartition(const flash_config_t *config);

/*!
 * @brief Enable the command complete interrupt.
 *
 * @param[in] none
 * @return none
 */
status_t FLASH_DRV_EnableCompleteIRQ(void);

/*!
 * @brief Disable the command complete interrupt.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_DisableCompleteIRQ(void);

/*!
 * @brief Enable the collision error interrupt.
 *
 * @param[in] none
 * @return none
 */
status_t FLASH_DRV_EnableCollisionIRQ(void);

/*!
 * @brief Disable the read collision error interrupt.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_DisableCollisionIRQ(void);

/*!
 * @brief Enable ECC 2-bit error interrupt.
 *
 * @param[in] none
 * @return none
 */
status_t FLASH_DRV_EnableECCIRQ(void);

/*!
 * @brief Disable ECC 2-bit error interrupt.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_DisableECCIRQ(void);

/*!
 * @brief Return ECC 2-bit error address.
 *
 * @param[in] none
 * @return ECC 2-bit error address
 */
uint32_t FLASH_DRV_GetECCErrorAddress(void);

/*!
 * @brief Clear ECC 2-bit error address.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_ClearECCErrorAddress(void);

/*!
 * @brief Install commamd completed interrupt call back function.
 *
 * @param[in] callback: call back function pointer
 * @return none
 */
void FLASH_DRV_InstallCallback(flash_callback_t callback);

/*!
 * @brief Install ECC 2-bit error interrupt call back function.
 *
 * @param[in] callback: call back function pointer
 * @return none
 */
void FLASH_DRV_InstallECCCallback(flash_callback_t callback);

/*!
 * @brief Install collision error interrupt call back function.
 *
 * @param[in] callback: call back function pointer
 * @return none
 */
void FLASH_DRV_InstallCollCallback(flash_callback_t callback);

/*!
 * @brief Command completed interrupt handle function.
 *
 * @param[in] none
 * @return none
 */
void FLASH_CompleteIRQHandler(void);

/*!
 * @brief ECC 2-bit error interrupt handle function.
 *
 * @param[in] none
 * @return none
 */
void FLASH_ECCIRQHandler(void);

/*!
 * @brief Collision error interrupt handle function.
 *
 * @param[in] none
 * @return none
 */
void FLASH_CollisionIRQHandler(void);

/*!
 * @brief Get ECC 2-bit error interrupt flag.
 *
 * @param[in] none
 * @return interrupt flag
 */
uint8_t FLASH_DRV_GetECCIRQFlag(void);

/*!
 * @brief Clear ECC 2-bit error interrupt flag.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_ClearECCIRQFlag(void);

/*!
* @brief Get FLASH collision error interrupt flag.
 *
 * @param[in] none
 * @return interrupt flag
 */
uint8_t FLASH_DRV_GetCollIRQFlag(void);

/*!
 * @brief Clear FLASH collision error interrupt flag.
 *
 * @param[in] none
 * @return interrupt flag
 */
void FLASH_DRV_ClearCollIRQFlag(void);

/*!
 * @brief Get default flash configuration.
 *
 * @param[in] config: Pointer to the user configuration structure
 * @return none
 */
void FLASH_DRV_GetDefaultConfig(flash_user_config_t *const config);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_DRV_H */

/* =============================================  EOF  ============================================== */
