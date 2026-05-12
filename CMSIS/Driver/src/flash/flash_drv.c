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
 * @file flash_drv.c
 *
 * @brief This file provides FLASH integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "flash_hw.h"

/* ============================================  Define  ============================================ */
/* Abort page erase wait count used in FLASH_DRV_AbortErase function */
#define ABORT_WAIT_CNT                   (0x40UL)

#define NUM_OF_WRITE_PROTECTION_BYTE     (5UL)      /* Write protection byte size(unit is byte) */
#define NUM_OF_SECURITY_OPTION_BYTE      (1UL)      /* security byte lentgh */
#define NUM_OF_GRANT_KEY_BYTE            (8UL)      /* Grant key byte size(unit is byte) */
#define NUM_OF_BACK_DOOR_KEY_BYTE        (8UL)      /* Back door key byte size(unit is byte) */
#define CSE_KEY_SIZE_CODE_MAX            (3UL)      /* CSE user key size */
#define FLASH_CMD_TIMEOUT                (2UL)      /* FLASH command timeout */

#define FLASH_NOT_SECURITY               (0x01UL)   /* Flash currently not in security state */
#define FLASH_SECURE_BACKDOOR_ENABLED    (0x02UL)   /* Flash is read protection and backdoor key access enabled */
#define FLASH_SECURE_BACKDOOR_DISABLED   (0x04UL)   /* Flash is read protection and backdoor key access disabled */

/* Flash info area base & end addresss */
#define FLASH_PF_INFO_BASE_ADDR          (0x00200000UL)
#define FLASH_GRANT_KEY_ADDR             (FLASH_PF_INFO_BASE_ADDR)
#define FLASH_BACK_DOOR_KEY_ADDR         (FLASH_PF_INFO_BASE_ADDR + 0x08UL)
#define FLASH_SECURITY_ADDR              (FLASH_PF_INFO_BASE_ADDR + 0x10UL)
#define FLASH_WRITE_PROTECTION_ADDR      (FLASH_PF_INFO_BASE_ADDR + 0x800UL)
#define FLASH_ONCE_PROGRAM_ADDR          (FLASH_PF_INFO_BASE_ADDR + 0x1000UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/*!
 * @brief Flash callback pointer.
 */
static flash_callback_t s_flashCallback = (flash_callback_t)NULL;
static flash_callback_t s_flashECCCallback = (flash_callback_t)NULL;
static flash_callback_t s_flashCollisionCallback = (flash_callback_t)NULL;

/*!
 * @brief Flash interrupt flag global variable.
 */
static uint8_t s_flashECCIRQFlag = 0U;
static uint8_t s_flashCollIRQFlag = 0U;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
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
status_t FLASH_DRV_UnlockCtrl(void)
{
    status_t ret = STATUS_SUCCESS;
    uint8_t ctrlLockBit = FLASH_GetLockStatus();
    uint32_t timeoutCount = 0U;

    while (1U == ctrlLockBit)
    {
        FLASH_SetKey(FLASH_UNLOCK_KEY1);
        FLASH_SetKey(FLASH_UNLOCK_KEY2);

        if (timeoutCount > 100U)
        {
            ret = STATUS_TIMEOUT;
            break;
        }
        ctrlLockBit = FLASH_GetLockStatus();
        timeoutCount++;
    }

    return ret;
}

/*!
 * @brief Lock the FLASH register.
 *
 * This API sets any key value except(0xac7840 and 0x01234567) to KEYUNLK register
 * to lock FLASH.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_LockCtrl(void)
{
    FLASH_SetKey(~FLASH_UNLOCK_KEY1);
    FLASH_SetKey(~FLASH_UNLOCK_KEY2);
}

/*!
 * @brief Gets D-Flash size from D-Flash Partition code.
 *
 * @param[in] config: FLASH configuration structure
 * @param[in] dFlashPartitionCode: D-Flash partition code
 * @return none.
 */
static void FLASH_GetDFlashPartitionSize(flash_config_t * const config, uint8_t dFlashPartitionCode)
{
    if ((dFlashPartitionCode <= 0x08) && (!(dFlashPartitionCode % 2)))
    {
        config->dFlashSize = (DFLASH_BLOCK_SIZE / 16) * dFlashPartitionCode;
    }
    else
    {
        config->dFlashSize = DFLASH_BLOCK_SIZE;
    }
}

/*!
 * @brief Initializes FLASH module.
 *
 * @param[in] userConfig: The user configuration structure pointer
 * @param[in] config: The FLASH configuration structure pointer
 * @return none
 */
void FLASH_DRV_Init(const flash_user_config_t * const userConfig, flash_config_t * const config)
{
    uint8_t dFlashPartitionCode = 0U;

    DEVICE_ASSERT(NULL != userConfig);
    DEVICE_ASSERT(NULL != config);

    config->pFlashBase = userConfig->pFlashBase;
    config->pFlashSize = userConfig->pFlashSize;
    config->dFlashBase = userConfig->dFlashBase;
    config->flexRAMBase   = userConfig->flexRAMBase;
    config->callback   = userConfig->callback;

    /* Get partition code from flash Configuration Register */
    dFlashPartitionCode = FLASH_GetDFlashPartitionCode();
    /* Get D-Flash size from D-Flash partition code */
    FLASH_GetDFlashPartitionSize(config, dFlashPartitionCode);
    if (config->dFlashSize < DFLASH_BLOCK_SIZE)
    {
        config->flexRAMSize = FLEX_RAM_SIZE;
    }
    else
    {
        config->flexRAMSize = 0U;
    }
}

/*!
 * @brief Perform start command sequence on FLASH.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 */
static status_t FLASH_StartCmdSequence(const flash_config_t *config)
{
    status_t  ret = STATUS_SUCCESS;
    uint32_t timeout = FLASH_CMD_TIMEOUT;

    SET_BIT32(MCM->MCCR, MCM_MCCR_CADIS_Msk);
    /* trigger command execute */
    FLASH_TrigCmdExecute();
    while (FLASH_GetTrigCmdExecuteStatus() && (timeout--));
    /* Wait till CCIF bit is set */
    while (0U == FLASH_GetCmdCompleteStatus())
    {
        if (NULL != config->callback)
        {
            (config->callback)();
        }
    }
    CLEAR_BIT32(MCM->MCCR, MCM_MCCR_CADIS_Msk);
    /* Check command execute error status */
    if (0U != FLASH_GetStatus())
    {
        ret = STATUS_ERROR;
    }

    return ret;
}

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
void FLASH_DRV_GetPFlashProtection(uint32_t *protectStatus)
{
    DEVICE_ASSERT(NULL != protectStatus);
    *protectStatus = FLASH_GetPFlashProtStatus();
}

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
status_t FLASH_DRV_SetPFlashProtection(uint32_t protectStatus)
{
    status_t ret = STATUS_SUCCESS;

    /* Write to register */
    FLASH_SetPFlashProtReg(protectStatus);

    /* Read the value of PPROT registers */
    if (protectStatus != FLASH_GetPFlashProtStatus())
    {
        ret = STATUS_ERROR;
    }

    return ret;
}

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
status_t FLASH_DRV_GetDFlashProtection(const flash_config_t * config, uint8_t * protectStatus)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    DEVICE_ASSERT(NULL != protectStatus);
    /* Check if D-Flash size is 0 */
    if (0U == config->dFlashSize)
    {
        ret = STATUS_UNSUPPORTED;
    }
    else
    {
        *protectStatus = FLASH_GetDFlashProtStatus();
    }

    return ret;
}

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
status_t FLASH_DRV_SetDFlashProtection(const flash_config_t * config, uint8_t protectStatus)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    /* Check if D-Flash size is 0 */
    if (0U == config->dFlashSize)
    {
        ret = STATUS_UNSUPPORTED;
    }
    else
    {
        FLASH_SetDFlashProtReg(protectStatus);
        if (protectStatus != FLASH_GetDFlashProtStatus())
        {
            ret = STATUS_ERROR;
        }
        else
        {
            /* Do nothing */
        }
    }

    return ret;
}

/*!
 * @brief Get FLASH security state.
 *
 * @param[out] securityState: get the current FLASH security status
 * @return operation status: none
 */
void FLASH_DRV_GetSecurityState(uint8_t * securityState)
{
    DEVICE_ASSERT(NULL != securityState);

    /* Check FLASH security bits in the security(SEC) register */
    if (0x03U == FLASH_GetReadProtectStatus())
    {
        /* FLASH in no security state */
        *securityState = FLASH_NOT_SECURITY;
    }
    else
    {
        /* Check verify backdoor key enable status when FLASH in secured state */
        if (0x03U == FLASH_GetBackdoorKeyStatus())
        {
            /* Verify backdoor key is disabled */
            *securityState = FLASH_SECURE_BACKDOOR_DISABLED;
        }
        else
        {
            /* Verify backdoor key is enabled */
            *securityState = FLASH_SECURE_BACKDOOR_ENABLED;
        }
    }
}

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
status_t FLASH_DRV_SecurityBypass(const flash_config_t *config, const uint8_t * key)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    DEVICE_ASSERT(NULL != key);
    /* Check if command is completed by CCIF */
    if (0U == FLASH_GetCmdCompleteStatus())
    {
        ret = STATUS_BUSY;
    }
    else
    {
        /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
        FLASH_CLEAR_STAT_ERROR_BITS();

        /* Set flash execute command */
        FLASH_SetCommand(FLASH_SECURITY_BY_PASS);

        /* use byte shift mode is convenient for code migration to
        * avoid address align in cortex M0
        */
        FLASH_SetLowData(key[0] | (key[1] << 8U) | (key[2] << 16U) | (key[3] << 24U));
        FLASH_SetHighData(key[4] | (key[5] << 8U) | (key[6] << 16U) | (key[7] << 24U));

        ret = FLASH_StartCmdSequence(config);
    }

    return ret;
}

/*!
 * @brief Erase FLASH all blocks(P-FLash & D-FLash).
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_EraseAllBlock(const flash_config_t *config)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    /* Check if command is completed by CCIF */
    if (0U == FLASH_GetCmdCompleteStatus())
    {
        ret = STATUS_BUSY;
    }
    else
    {
        /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
        FLASH_CLEAR_STAT_ERROR_BITS();

        /* Set flash execute command */
        FLASH_SetCommand(FLASH_ERASE_ALL_BLOCK);

        /* Calling FLASH start command sequence function to execute the command */
        ret = FLASH_StartCmdSequence(config);
    }

    return ret;
}

/*!
 * @brief Verify FLASH all blocks(P-FLash & D-FLash).
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_VerifyAllBlock(const flash_config_t *config)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    /* Check if command is completed by CCIF */
    if (0U == FLASH_GetCmdCompleteStatus())
    {
        ret = STATUS_BUSY;
    }
    else
    {
        /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
        FLASH_CLEAR_STAT_ERROR_BITS();

        /* Set flash execute command */
        FLASH_SetCommand(FLASH_VERIFY_ALL_BLOCK);

        /* Calling FLASH start command sequence function to execute the command */
        ret = FLASH_StartCmdSequence(config);
    }

    return ret;
}

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
status_t FLASH_DRV_EraseSector(const flash_config_t *config, uint32_t addr, uint32_t size)
{
    status_t ret = STATUS_SUCCESS;
    uint32_t sectorSize = 0;
    uint32_t temp = 0;
    uint32_t tempSize = size;

    DEVICE_ASSERT(NULL != config);
    temp = config->dFlashBase;
    if ((addr >= temp) && (addr < (temp + config->dFlashSize)))
    {
        sectorSize = (uint32_t)DFLASH_PAGE_SIZE;
    }
    else
    {
        temp = config->pFlashBase;
        if ((addr >= temp) && (addr < (temp + config->pFlashSize)))
        {
            sectorSize = (uint32_t)PFLASH_PAGE_SIZE;
        }
        else
        {
            ret = STATUS_ERROR;
            tempSize = 0U;
            sectorSize = 0U;
        }
    }

    /* Check if the tempSize is alignment with flash sector size */
    if (0U != (tempSize & (sectorSize - 1U)))
    {
        ret = STATUS_UNSUPPORTED;
    }

    while ((tempSize > 0U) && (STATUS_SUCCESS == ret))
    {
        /* Check if command is completed by CCIF */
        if (0U == FLASH_GetCmdCompleteStatus())
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
            FLASH_CLEAR_STAT_ERROR_BITS();

            /* Set flash execute command */
            FLASH_SetCommand(FLASH_ERASE_PAGE);
            FLASH_SetAddress(addr);

            /* Calling FLASH start command sequence function to execute the command */
            ret = FLASH_StartCmdSequence(config);

            /* Update size and address */
            tempSize -= sectorSize;
            addr += sectorSize;
        }
    }

    return ret;
}

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
status_t FLASH_DRV_VerifySection(const flash_config_t *config, uint32_t addr, uint32_t size)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);

    if (addr >= (config->pFlashBase + config->pFlashSize))
    {
        if ((addr < config->dFlashBase) || (addr >= (config->dFlashBase + config->dFlashSize)))
        {
            ret = STATUS_ERROR;
        }
    }

    if (STATUS_SUCCESS == ret)
    {
        /* Check if command is completed by CCIF */
        if (0U == FLASH_GetCmdCompleteStatus())
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
            FLASH_CLEAR_STAT_ERROR_BITS();

            /* Set flash execute command */
            FLASH_SetCommand(FLASH_VERIFY_SECTION);
            FLASH_SetAddress(addr);
            FLASH_SetLength(size);

            /* Calling FLASH start command sequence function to execute the command */
            ret = FLASH_StartCmdSequence(config);
        }
    }

    return ret;
}

/*!
 * @brief Abort a current operation of FLASH erase sector command.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_AbortErase(void)
{
    uint32_t count = ABORT_WAIT_CNT;

    if (0U == FLASH_GetCmdCompleteStatus())
    {
        FLASH_AbortPageErase();

        /* Wait till PERABORT bit is self cleared */
        while (FLASH_GetAbortPageEraseStatus() && (count > 0U))
        {
            count--;
        }
    }
}

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
status_t FLASH_DRV_Program(const flash_config_t *config, uint32_t addr, uint32_t size, const uint8_t *data)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    DEVICE_ASSERT(NULL != data);

    if (0U != (size & (PFLASH_WRITE_UNIT_SIZE - 1U)))
    {
        ret = STATUS_UNSUPPORTED;
    }
    else
    {
        if (addr >= (config->pFlashBase + config->pFlashSize))
        {
            if ((addr < config->dFlashBase) || (addr >= (config->dFlashBase + config->dFlashSize)))
            {
                ret = STATUS_ERROR;
            }
        }

        while ((size > 0U) && (STATUS_SUCCESS == ret))
        {
            /* Check if command is completed by CCIF */
            if (0U == FLASH_GetCmdCompleteStatus())
            {
                ret = STATUS_BUSY;
            }
            else
            {
                /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
                FLASH_CLEAR_STAT_ERROR_BITS();

                /* Set flash execute command */
                FLASH_SetCommand(FLASH_PROGRAM_PHRASE);
                FLASH_SetAddress(addr);

                FLASH_SetLowData(data[0] | (data[1] << 8U) | (data[2] << 16U) | (data[3] << 24U));
                FLASH_SetHighData(data[4] | (data[5] << 8U) | (data[6] << 16U) | (data[7] << 24U));

                /* Calling FLASH start command sequence function to execute the command */
                ret = FLASH_StartCmdSequence(config);

                /* Update program address for next operation */
                addr += PFLASH_WRITE_UNIT_SIZE;
                /* Update size for next operation */
                size -= PFLASH_WRITE_UNIT_SIZE;
                /* Increase the data source pointer by 8 */
                data += PFLASH_WRITE_UNIT_SIZE;
            }
        }
    }

    return ret;
}

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
status_t FLASH_DRV_ProgramSection(const flash_config_t *config, uint32_t addr, uint16_t size)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    /* Check RAMRDY bit of the FLASH status register */
    if (0U == FLASH_GetRAMReadyStatus())
    {
        ret = STATUS_UNSUPPORTED;
    }
    else
    {
        if (addr >= (config->pFlashBase + config->pFlashSize))
        {
            if ((addr < config->dFlashBase) || (addr >= (config->dFlashBase + config->dFlashSize)))
            {
                ret = STATUS_ERROR;
            }
        }

        if (ret == STATUS_SUCCESS)
        {
            /* Check if command is completed by CCIF */
            if (0U == FLASH_GetCmdCompleteStatus())
            {
                ret = STATUS_BUSY;
            }
            else
            {
                /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
                FLASH_CLEAR_STAT_ERROR_BITS();
                /* Set flash execute command */
                FLASH_SetCommand(FLASH_PROGRAM_SECTION);
                FLASH_SetAddress(addr);
                FLASH_SetLength(size);

                /* Calling FLASH start command sequence function to execute the command */
                ret = FLASH_StartCmdSequence(config);
            }
        }
    }

    return ret;
}

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
status_t FLASH_DRV_EraseBlock(const flash_config_t *config, uint32_t addr)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);

    if (addr >= (config->pFlashBase + config->pFlashSize))
    {
        if ((addr < config->dFlashBase) || (addr >= (config->dFlashBase + config->dFlashSize)))
        {
            ret = STATUS_ERROR;
        }
    }

    if (STATUS_SUCCESS == ret)
    {
        /* Check if command is completed by CCIF */
        if (0U == FLASH_GetCmdCompleteStatus())
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
            FLASH_CLEAR_STAT_ERROR_BITS();

            /* Set flash execute command */
            FLASH_SetCommand(FLASH_ERASE_BLOCK);
            FLASH_SetAddress(addr);
            /* Calling FLASH start command sequence function to execute the command */
            ret = FLASH_StartCmdSequence(config);
        }
    }

    return ret;
}

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
status_t FLASH_DRV_VerifyBlock(const flash_config_t *config, uint32_t addr)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);

    if (addr >= (config->pFlashBase + config->pFlashSize))
    {
        if ((addr < config->dFlashBase) || (addr >= (config->dFlashBase + config->dFlashSize)))
        {
            ret = STATUS_ERROR;
        }
    }

    if (STATUS_SUCCESS == ret)
    {
        /* Check if command is completed by CCIF */
        if (0U == FLASH_GetCmdCompleteStatus())
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
            FLASH_CLEAR_STAT_ERROR_BITS();

            /* Set flash execute command */
            FLASH_SetCommand(FLASH_VERIFY_BLOCK);
            FLASH_SetAddress(addr);

            /* Calling FLASH start command sequence function to execute the command */
            ret = FLASH_StartCmdSequence(config);
        }
    }

    return ret;
}

/*!
 * @brief Erase FLASH write protection info page.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_EraseWriteProtection(const flash_config_t *config)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);

    /* Check if command is completed by CCIF */
    if (0U == FLASH_GetCmdCompleteStatus())
    {
        ret = STATUS_BUSY;
    }
    else
    {
        /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
        FLASH_CLEAR_STAT_ERROR_BITS();

        /* Set flash execute command */
        FLASH_SetCommand(FLASH_WRITE_PROTECT_ERASE);
        FLASH_SetAddress(FLASH_WRITE_PROTECTION_ADDR);

        /* Calling FLASH start command sequence function to execute the command */
        ret = FLASH_StartCmdSequence(config);
    }

    return ret;
}

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
status_t FLASH_DRV_SetWriteProtection(const flash_config_t *config, const uint8_t *data, uint32_t size)
{
    /* define FLASH write protection buffer, FLASH program width is 64-bit */
    uint8_t protBuf[8] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
    uint32_t i = 0U;
    uint32_t temp = 0U;
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);

    temp = size;
    if (size >= NUM_OF_WRITE_PROTECTION_BYTE)
    {
        temp = NUM_OF_WRITE_PROTECTION_BYTE;
    }

    for (i = 0U; i < temp; i++)
    {
        protBuf[i] = data[i];
    }

    if (temp > 0U)
    {
        /* Check if command is completed by CCIF */
        if (0U == FLASH_GetCmdCompleteStatus())
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
            FLASH_CLEAR_STAT_ERROR_BITS();

            /* Set flash execute command */
            FLASH_SetCommand(FLASH_PROGRAM_PHRASE);
            FLASH_SetAddress(FLASH_WRITE_PROTECTION_ADDR);

            FLASH_SetLowData(protBuf[0] | (protBuf[1] << 8U) | (protBuf[2] << 16U) | (protBuf[3] << 24U));
            FLASH_SetHighData(protBuf[4] | (protBuf[5] << 8U) | (protBuf[6] << 16U) | (protBuf[7] << 24U));

            /* Calling FLASH start command sequence function to execute the command */
            ret = FLASH_StartCmdSequence(config);
        }
    }

    return ret;
}

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
 *           2'b11: enable
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
status_t FLASH_DRV_SetSecurity(const flash_config_t *config, const uint8_t *data, uint32_t size)
{
    /* define secrity buffer, FLASH program width is 64-bit */
    uint8_t secBuf[8] = {0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU};
    uint32_t i = 0U;
    uint32_t temp = 0U;
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != data);
    DEVICE_ASSERT(NULL != config);

    temp = size;
    if (temp >= NUM_OF_SECURITY_OPTION_BYTE)
    {
        temp = NUM_OF_SECURITY_OPTION_BYTE;
    }
    for (i = 0U; i < temp; i++)
    {
        secBuf[i] = data[i];
    }

    if (temp > 0U)
    {
        /* Check if command is completed by CCIF */
        if (0U == FLASH_GetCmdCompleteStatus())
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
            FLASH_CLEAR_STAT_ERROR_BITS();

            /* Set flash execute command */
            FLASH_SetCommand(FLASH_PROGRAM_PHRASE);
            FLASH_SetAddress(FLASH_SECURITY_ADDR);
            FLASH_SetLowData(secBuf[0] | (secBuf[1] << 8U) | (secBuf[2] << 16U) | (secBuf[3] << 24U));
            FLASH_SetHighData(secBuf[4] | (secBuf[5] << 8U) | (secBuf[6] << 16U) | (secBuf[7] << 24U));

            /* Calling FLASH start command sequence function to execute the command */
            ret = FLASH_StartCmdSequence(config);
        }
    }

    return ret;
}

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
status_t FLASH_DRV_SetGrantKey(const flash_config_t *config, const uint8_t *data, uint32_t size)
{
    uint8_t key[NUM_OF_GRANT_KEY_BYTE] = {0};
    uint32_t i = 0U;
    uint32_t temp = 0U;
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != data);
    DEVICE_ASSERT(NULL != config);

    temp = size;
    if (size >= NUM_OF_GRANT_KEY_BYTE)
    {
        temp = NUM_OF_GRANT_KEY_BYTE;
    }

    for (i = 0U; i < temp; i++)
    {
        key[i] = data[i];
    }

    if (temp > 0U)
    {
        /* Check if command is completed by CCIF */
        if (0U == FLASH_GetCmdCompleteStatus())
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
            FLASH_CLEAR_STAT_ERROR_BITS();

            /* Set flash execute command */
            FLASH_SetCommand(FLASH_PROGRAM_PHRASE);
            FLASH_SetAddress(FLASH_GRANT_KEY_ADDR);

            FLASH_SetLowData(key[0] | (key[1] << 8U) | (key[2] << 16U) | (key[3] << 24U));
            FLASH_SetHighData(key[4] | (key[5] << 8U) | (key[6] << 16U) | (key[7] << 24U));

            /* Calling FLASH start command sequence function to execute the command */
            ret = FLASH_StartCmdSequence(config);
        }
    }

    return ret;
}

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
status_t FLASH_DRV_SetBackDoorKey(const flash_config_t *config, const uint8_t *data, uint32_t size)
{
    uint8_t key[NUM_OF_BACK_DOOR_KEY_BYTE] = {0};
    uint32_t i = 0U;
    uint32_t temp = 0U;
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != data);
    DEVICE_ASSERT(NULL != config);

    temp = size;
    if (size >= NUM_OF_BACK_DOOR_KEY_BYTE)
    {
        temp = NUM_OF_BACK_DOOR_KEY_BYTE;
    }

    for (i = 0U; i < temp; i++)
    {
        key[i] = data[i];
    }

    if (temp > 0U)
    {
        /* Check if command is completed by CCIF */
        if (0U == FLASH_GetCmdCompleteStatus())
        {
            ret = STATUS_BUSY;
        }
        else
        {
            /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
            FLASH_CLEAR_STAT_ERROR_BITS();

            /* Set flash execute command */
            FLASH_SetCommand(FLASH_PROGRAM_PHRASE);
            FLASH_SetAddress(FLASH_BACK_DOOR_KEY_ADDR);

            FLASH_SetLowData(key[0] | (key[1] << 8U) | (key[2] << 16U) | (key[3] << 24U));
            FLASH_SetHighData(key[4] | (key[5] << 8U) | (key[6] << 16U) | (key[7] << 24U));

            /* Calling FLASH start command sequence function to execute the command */
            ret = FLASH_StartCmdSequence(config);
        }
    }

    return ret;
}

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
status_t FLASH_DRV_Read(const flash_config_t *config, uint32_t addr, uint8_t *data, uint32_t size)
{
    status_t ret = STATUS_SUCCESS;
    uint32_t i = 0U;

    DEVICE_ASSERT(NULL != config);
    DEVICE_ASSERT(NULL != data);
    /* Check if the destination is aligned or not */
    if (addr >= (config->pFlashBase + config->pFlashSize))
    {
        if ((addr < config->dFlashBase) || (addr >= (config->dFlashBase + config->dFlashSize)))
        {
            ret = STATUS_ERROR;
        }
    }

    if (STATUS_SUCCESS == ret)
    {
        for (i = 0U; i < size; i++)
        {
            data[i] = *(uint8_t*)(addr + i);
        }
    }

    return ret;
}

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
status_t FLASH_DRV_ReadOnce(uint8_t index, uint8_t *data)
{
    uint32_t temp = 0U;
    uint8_t i = 0U;

    DEVICE_ASSERT(NULL != data);

    temp = index * 8U + FLASH_ONCE_PROGRAM_ADDR;
    for (i = 0U; i < PFLASH_WRITE_UNIT_SIZE; i++)
    {
        data[i] = *(uint8_t *)(temp + i);
    }

    return STATUS_SUCCESS;
}

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
status_t FLASH_DRV_ProgramOnce(const flash_config_t *config, uint8_t index, const uint8_t *data)
{
    uint32_t temp = 0U;
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    DEVICE_ASSERT(NULL != data);

    temp = index * 8U + FLASH_ONCE_PROGRAM_ADDR;
    /* Check if command is completed by CCIF */
    if (0U == FLASH_GetCmdCompleteStatus())
    {
        ret = STATUS_BUSY;
    }
    else
    {
        /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
        FLASH_CLEAR_STAT_ERROR_BITS();

        /* Set flash execute command */
        FLASH_SetCommand(FLASH_PROGRAM_ONCE);
        FLASH_SetAddress(temp);

        FLASH_SetLowData(data[0] | (data[1] << 8U) | (data[2] << 16U) | (data[3] << 24U));
        FLASH_SetHighData(data[4] | (data[5] << 8U) | (data[6] << 16U) | (data[7] << 24U));

        /* Calling FLASH start command sequence function to execute the command */
        ret = FLASH_StartCmdSequence(config);
    }

    return ret;
}

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
status_t FLASH_DRV_SetFlexRamFunction(const flash_config_t *config, flash_flexram_function_code_t funcCode)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    /* Check if command is completed by CCIF */
    if (0U == FLASH_GetCmdCompleteStatus())
    {
        ret = STATUS_BUSY;
    }
    else
    {
        /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
        FLASH_CLEAR_STAT_ERROR_BITS();

        /* Set flash execute command */
        FLASH_SetCommand(FLASH_SET_FLEXRAM);
        FLASH_SetAddress((uint32_t)funcCode);
        /* Calling FLASH start command sequence function to execute the command */
        ret = FLASH_StartCmdSequence(config);
    }

    return ret;
}

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
status_t FLASH_DRV_Partition(const flash_config_t *config, uint8_t dFlashPartCode, uint8_t cseKeySize, bool sfe)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    DEVICE_ASSERT(cseKeySize <= CSE_KEY_SIZE_CODE_MAX);

    if (dFlashPartCode)// reserved parameter
    {

    }
    /* Check if command is completed by CCIF */
    if (0U == FLASH_GetCmdCompleteStatus())
    {
        ret = STATUS_BUSY;
    }
    else
    {
        /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
        FLASH_CLEAR_STAT_ERROR_BITS();
        /* Set flash execute command */
        FLASH_SetCommand(FLASH_PROGRAM_PARTITION);
        FLASH_SetLowData((uint32_t)(0x208U | ((uint32_t)sfe << 24U)));
        FLASH_SetHighData((uint32_t)(cseKeySize));

        /* Calling FLASH start command sequence function to execute the command */
        ret = FLASH_StartCmdSequence(config);
    }

    return ret;
}

/*!
 * @brief Erase Partition.
 *
 * @param[in] config: The FLASH configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR:   Operation was failure
 *        - STATUS_BUSY:    Operation was busy
 */
status_t FLASH_DRV_ErasePartition(const flash_config_t *config)
{
    status_t ret = STATUS_SUCCESS;

    DEVICE_ASSERT(NULL != config);
    /* Check if command is completed by CCIF */
    if (0U == FLASH_GetCmdCompleteStatus())
    {
        ret = STATUS_BUSY;
    }
    else
    {
        /* Write 1 to clear COLLERR & ACCERR & FPVIOL flag in FLASH status register */
        FLASH_CLEAR_STAT_ERROR_BITS();
        /* Set flash execute command */
        FLASH_SetCommand(FLASH_ERASE_PARTITION);

        /* Calling FLASH start command sequence function to execute the command */
        ret = FLASH_StartCmdSequence(config);
    }

    return ret;
}

/*!
 * @brief Enable the command complete interrupt.
 *
 * @param[in] none
 * @return none
 */
status_t FLASH_DRV_EnableCompleteIRQ(void)
{
    /* Enable the command complete interrupt */
    FLASH_EnableCompleteIRQ(true);
    NVIC_EnableIRQ(FLASH_IRQn);

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable the command complete interrupt.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_DisableCompleteIRQ(void)
{
    /* Disable the command complete interrupt */
    FLASH_EnableCompleteIRQ(false);
    NVIC_DisableIRQ(FLASH_IRQn);
    NVIC_ClearPendingIRQ(FLASH_IRQn);
}

/*!
 * @brief Enable the collision error interrupt.
 *
 * @param[in] none
 * @return none
 */
status_t FLASH_DRV_EnableCollisionIRQ(void)
{
    /* Enable the collision error interrupt */
    FLASH_EnableCollisionIRQ(true);
    NVIC_EnableIRQ(FLASH_COLLISION_IRQn);

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable the read collision error interrupt.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_DisableCollisionIRQ(void)
{
    /* Disable the collision error interrupt */
    FLASH_EnableCollisionIRQ(false);
    NVIC_DisableIRQ(FLASH_COLLISION_IRQn);
    NVIC_ClearPendingIRQ(FLASH_COLLISION_IRQn);
}

/*!
 * @brief Enable ECC 2-bit error interrupt.
 *
 * @param[in] none
 * @return none
 */
status_t FLASH_DRV_EnableECCIRQ(void)
{
    /* Enable the double bit fault detect interrupt */
    FLASH_EnableECCIRQ(true);
    NVIC_EnableIRQ(FLASH_ECC_IRQn);

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable ECC 2-bit error interrupt.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_DisableECCIRQ(void)
{
    /* Disable the 2-bit fault detect interrupt */
    FLASH_EnableECCIRQ(false);
    NVIC_DisableIRQ(FLASH_ECC_IRQn);
    NVIC_ClearPendingIRQ(FLASH_ECC_IRQn);
}

/*!
 * @brief Return ECC 2-bit error address.
 *
 * @param[in] none
 * @return ECC 2-bit error address
 */
uint32_t FLASH_DRV_GetECCErrorAddress(void)
{
    return FLASH_GetECCErrorAddress();
}

/*!
 * @brief Clear ECC 2-bit error address.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_ClearECCErrorAddress(void)
{
    FLASH_ClearECCErrorAddress();
}

/*!
 * @brief Install commamd completed interrupt call back function.
 *
 * @param[in] callback: call back function pointer
 * @return none
 */
void FLASH_DRV_InstallCallback(flash_callback_t callback)
{
    DEVICE_ASSERT(NULL != callback);

    s_flashCallback = callback;
}

/*!
 * @brief Install ECC 2-bit error interrupt call back function.
 *
 * @param[in] callback: call back function pointer
 * @return none
 */
void FLASH_DRV_InstallECCCallback(flash_callback_t callback)
{
    DEVICE_ASSERT(NULL != callback);

    s_flashECCCallback = callback;
}

/*!
 * @brief Install collision error interrupt call back function.
 *
 * @param[in] callback: call back function pointer
 * @return none
 */
void FLASH_DRV_InstallCollCallback(flash_callback_t callback)
{
    DEVICE_ASSERT(NULL != callback);

    s_flashCollisionCallback = callback;
}

/*!
 * @brief Command completed interrupt handle function.
 *
 * @param[in] none
 * @return none
 */
void FLASH_CompleteIRQHandler(void)
{
    if (NULL != s_flashCallback)
    {
        s_flashCallback();
    }
}

/*!
 * @brief ECC 2-bit error interrupt handle function.
 *
 * @param[in] none
 * @return none
 */
void FLASH_ECCIRQHandler(void)
{
    s_flashECCIRQFlag = FLASH_GetECCIRQFlag();

    FLASH_ClearECCIRQFlag();
    if (NULL != s_flashECCCallback)
    {
        s_flashECCCallback();
    }
}

/*!
 * @brief Collision error interrupt handle function.
 *
 * @param[in] none
 * @return none
 */
void FLASH_CollisionIRQHandler(void)
{
    s_flashCollIRQFlag = FLASH_GetCollIRQFlag();
    FLASH_ClearCollIRQFlag();

    if (NULL != s_flashCollisionCallback)
    {
        s_flashCollisionCallback();
    }
}

/*!
 * @brief Get ECC 2-bit error interrupt flag.
 *
 * @param[in] none
 * @return interrupt flag
 */
uint8_t FLASH_DRV_GetECCIRQFlag(void)
{
    uint32_t flag;

    if (FLASH_GetCnfg() & FLASH_CNFG_DFDIE_Msk)
    {
        flag = s_flashECCIRQFlag;
    }
    else
    {
        flag = FLASH_GetECCIRQFlag();
    }

    return flag;
}

/*!
 * @brief Clear ECC 2-bit error interrupt flag.
 *
 * @param[in] none
 * @return none
 */
void FLASH_DRV_ClearECCIRQFlag(void)
{
    FLASH_ClearECCIRQFlag();
}

/*!
 * @brief Get FLASH collision error interrupt flag.
 *
 * @param[in] none
 * @return interrupt flag
 */
uint8_t FLASH_DRV_GetCollIRQFlag(void)
{
    uint32_t flag;

    if (FLASH_GetCnfg() & FLASH_CNFG_COLLE_Msk)
    {
        flag = s_flashCollIRQFlag;
    }
    else
    {
        flag = FLASH_GetCollIRQFlag();
    }

    return flag;
}

/*!
 * @brief Clear FLASH collision error interrupt flag.
 *
 * @param[in] none
 * @return interrupt flag
 */
void FLASH_DRV_ClearCollIRQFlag(void)
{
    FLASH_ClearCollIRQFlag();
}

/*!
 * @brief Get default flash configuration.
 *
 * @param[in] config: Pointer to the user configuration structure
 * @return none
 */
void FLASH_DRV_GetDefaultConfig(flash_user_config_t *const config)
{
    DEVICE_ASSERT(NULL != config);

    config->pFlashBase  = PFLASH_BASE_ADDRESS;
    config->pFlashSize  = PFLASH_BLOCK_SIZE;
    config->dFlashBase  = DFLASH_BASE_ADDRESS;
    config->flexRAMBase = FLEXRAM_BASE_ADDRESS;
    config->callback    = NULL;
}

/* =============================================  EOF  ============================================== */
