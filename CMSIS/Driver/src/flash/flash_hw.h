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
 * @file flash_hw.h
 *
 * @brief FLASH access register inline function definition.
 *
 */

#ifndef _FLASH_HW_H
#define _FLASH_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "flash_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set flash key sequence register.
 *
 * @param[in] keyValue: unlock flash controler key value
 * @return none
 */
static inline void FLASH_SetKey(uint32_t keyValue)
{
    FLASH->KEYUNLK = keyValue;
}

/*!
 * @brief Get the flash controler lock status.
 *
 * param[in] none
 * @return 0: Flash control is unlocked
 *         1: Flash control is locked
 */
static inline uint8_t FLASH_GetLockStatus(void)
{
    return (uint8_t)((FLASH->STAT & FLASH_STAT_LOCK_Msk) >> FLASH_STAT_LOCK_Pos);
}

/*!
 * @brief Get the flash read protection status.
 *
 * param[in] none
 * @return 2b'11: Read protection is disabled
 *         2b'00/2b'01/2b'10: Read protection is enabled
 */
static inline uint8_t FLASH_GetReadProtectStatus(void)
{
    return (uint8_t)((FLASH->SEC & FLASH_SEC_RDPROT_Msk) >> FLASH_SEC_RDPROT_Pos);
}

/*!
 * @brief Get the backdoor key verify enable status.
 *
 * param[in] none
 * @return 2b'11: Backdoor key can not disable read protection
 *         2b'00/2b'01/2b'10: Backdoor key can enable read protection
 */
static inline uint8_t FLASH_GetBackdoorKeyStatus(void)
{
    return (uint8_t)((FLASH->SEC & FLASH_SEC_BDKEN_Msk) >> FLASH_SEC_BDKEN_Pos);
}

/*!
 * @brief Get command completed status.
 *
 * param[in] none
 * @return 1: Command is completed
 *         0: Command is not completed
 */
static inline uint8_t FLASH_GetCmdCompleteStatus(void)
{
    return (uint8_t)((FLASH->STAT & FLASH_STAT_CCIF_Msk) >> FLASH_STAT_CCIF_Pos);
}

/*!
 * @brief Set FLASH operation command.
 *
 * param[in] cmd: FLASH command
 * @return none
 */
static inline void FLASH_SetCommand(flash_command_t cmd)
{
    FLASH->CMD = (uint32_t)cmd;
}

/*!
 * @brief Set low 32-bit data when program flash.
 *
 * param[in] data: The low 32-bit data
 * @return none
 */
static inline void FLASH_SetLowData(uint32_t data)
{
    FLASH->DATA0 = data;
}

/*!
 * @brief Set high 32-bit data when program flash.
 *
* param[in] data: The high 32-bit data
 * @return none
 */
static inline void FLASH_SetHighData(uint32_t data)
{
    FLASH->DATA1 = data;
}

/*!
 * @brief get low 32-bit data.
 *
 * param[in] none
 * @return none
 */
static inline uint32_t FLASH_GetLowData(void)
{
    return (FLASH->DATA0);
}

/*!
 * @brief Get high 32-bit data.
 *
 * param[in] none
 * @return none
 */
static inline uint32_t FLASH_GetHighData(void)
{
    return (FLASH->DATA1);
}

/*!
 * @brief Set FLASH operation start address.
 *
 * @param[in] addr: start address
 * @return none
 */
static inline void FLASH_SetAddress(uint32_t addr)
{
    FLASH->ADDR = addr;
}

/*!
 * @brief Set FLASH operation length in 64-bit.
 *
 * @param[in] len: program/erase length
 * @return none
 */
static inline void FLASH_SetLength(uint32_t len)
{
    FLASH->LEN = len;
}

/*!
 * @brief Trigger the FLASH operation command to execute.
 *
 * @param[in] none
 * @return none
 */
static inline void FLASH_TrigCmdExecute(void)
{
    FLASH->CST = FLASH_CST_START_Msk;
}

/*!
 * @brief Get Trigger the FLASH operation command execute status.
 *
 * @param[in] none
 * @return Triggrt command enable bit value
 */
static inline uint8_t FLASH_GetTrigCmdExecuteStatus(void)
{
    return (uint8_t)(FLASH->CST & FLASH_CST_START_Msk);
}

/*!
 * @brief Abort FLASH page erase operation.
 *
 * @param[in] none
 * @return none
 */
static inline void FLASH_AbortPageErase(void)
{
    FLASH->CNFG |= FLASH_CNFG_PERABORT_Msk;
}

/*!
 * @brief Get abort page erase status
 *
 * @param[in] none
 * @return none
 */
static inline uint8_t FLASH_GetAbortPageEraseStatus(void)
{
    return (uint8_t)((FLASH->CNFG & FLASH_CNFG_PERABORT_Msk) >> FLASH_CNFG_PERABORT_Pos);
}

/*!
 * @brief Get the error status.
 *
 * @param[in] none
 * @return status value
 */
static inline uint32_t FLASH_GetStatus(void)
{
    return (FLASH->STAT & FLASH_STAT_ERROR_BITS);
}

/*!
 * @brief Set P-Flash write protection status.
 *
 * @param[in] protectStatus: P-Flash write protection status value
 * @return none
 */
static inline void FLASH_SetPFlashProtReg(uint32_t protectStatus)
{
    FLASH->PPROT = protectStatus;
}

/*!
 * @brief Get P-Flash write protection status.
 *
 * @param[in] none
 * @return P-Flash write protection status value
 */
static inline uint32_t FLASH_GetPFlashProtStatus(void)
{
    return (FLASH->PPROT);
}

/*!
 * @brief Set D-Flash write protection status.
 *
 * @param[in] protectStatus: D-Flash write protection status value
 * @return none
 */
static inline void FLASH_SetDFlashProtReg(uint8_t protectStatus)
{
    FLASH->DPROT = protectStatus;
}

/*!
 * @brief Get D-Flash write protection status.
 *
 * @param[in] none
 * @return D-Flash write protection status value
 */
static inline uint32_t FLASH_GetDFlashProtStatus(void)
{
    return (FLASH->DPROT & FLASH_DPROT_DPROT_Msk);
}

/*!
* @brief Get partition information code.
 *
 * @param[in] none
 * @return partition code
 */
static inline uint8_t FLASH_GetDFlashPartitionCode(void)
{
    return (uint8_t)(FLASH->PART & FLASH_PART_DPART_Msk);
}

/*!
 * @brief Set command complete interrupt register.
 *
 * @param[in] enable: command complete interrupt enable status
 * @return none
 */
static inline void FLASH_EnableCompleteIRQ(bool enable)
{
    MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_CCIE_Msk, FLASH_CNFG_CCIE_Pos, enable);
}

/*!
 * @brief Set ECC 2-bit error interrupt register.
 *
 * @param[in] enable: ECC 2-bit error interrupt enable status
 * @return none
 */
static inline void FLASH_EnableECCIRQ(bool enable)
{
    MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_DFDIE_Msk, FLASH_CNFG_DFDIE_Pos, enable);
}

/*!
 * @brief Set collision error interrupt register.
 *
 * @param[in] enable: collision interrupt enable status
 * @return none
 */
static inline void FLASH_EnableCollisionIRQ(bool enable)
{
    MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_COLLE_Msk, FLASH_CNFG_COLLE_Pos, enable);
}

/*!
 * @brief Get Flash ECC 2-bit error interrupt flag.
 *
 * @param[in] none
 * @return none
 */
static inline uint8_t FLASH_GetECCIRQFlag(void)
{
    return (uint8_t)((FLASH->STAT & FLASH_STAT_DFDIF_Msk) >> FLASH_STAT_DFDIF_Pos);
}

/*!
 * @brief Clear Flash ECC 2-bit error interrupt flag.
 *
 * @param[in] none
 * @return none
 */
static inline void FLASH_ClearECCIRQFlag(void)
{
    FLASH->STAT |= FLASH_STAT_DFDIF_Msk;
}

/*!
 * @brief Get FlexRAM ECC 1-bit error flag.
 *
 * @param[in] none
 * @return none
 */
static inline uint8_t FLASH_GetFRAMECC1BitFlag(void)
{
    return (uint8_t)((FLASH->STAT & FLASH_STAT_FRAMECC1F_Msk) >> FLASH_STAT_FRAMECC1F_Pos);
}

/*!
 * @brief Clear FlexRAM ECC 1-bit error flag.
 *
 * @param[in] none
 * @return none
 */
static inline void FLASH_ClearFRAMECC1BitFlag(void)
{
    FLASH->STAT |= FLASH_STAT_FRAMECC1F_Msk;
}

/*!
 * @brief Get FlexRAM ECC 2-bit error flag.
 *
 * @param[in] none
 * @return none
 */
static inline uint8_t FLASH_GetFRAMECC2BitFlag(void)
{
    return (uint8_t)((FLASH->STAT & FLASH_STAT_FRAMECC2F_Msk) >> FLASH_STAT_FRAMECC2F_Pos);
}

/*!
 * @brief Clear FlexRAM ECC 2-bit error flag.
 *
 * @param[in] none
 * @return none
 */
static inline void FLASH_ClearFRAMECC2BitFlag(void)
{
    FLASH->STAT |= FLASH_STAT_FRAMECC2F_Msk;
}

/*!
 * @brief Get Flash || FlexRAM ECC 2-bit error address.
 *
 * @param[in] none
 * @return none
 */
static inline uint32_t FLASH_GetECCErrorAddress(void)
{
    return (FLASH->DFADR);
}

/*!
 * @brief Clear ECC 2-bit error address.
 *
 * @param[in] none
 * @return none
 */
static inline void FLASH_ClearECCErrorAddress(void)
{
    FLASH->DFADR = 0;
}

/*!
 * @brief Get collision error interrupt flag.
 *
 * @param[in] none
 * @return none
 */
static inline uint8_t FLASH_GetCollIRQFlag(void)
{
    return (uint8_t)((FLASH->STAT & FLASH_STAT_COLLERRF_Msk) >> FLASH_STAT_COLLERRF_Pos);
}

/*!
 * @brief Clear collision error interrupt flag.
 *
 * @param[in] none
 * @return none
 */
static inline void FLASH_ClearCollIRQFlag(void)
{
    FLASH->STAT |= FLASH_STAT_COLLERRF_Msk;
}

/*!
 * @brief force ECC 2-bit error detected, code should run in SRAM, usually used it for test.
 *
 * @param[in] none
 * @return none
 */
static inline void FLASH_ForceECCErrorDetect(void)
{
    FLASH->CNFG |= FLASH_CNFG_EMDFIE_Msk;
}

/*!
 * @brief Get flexRAM status.
 *
 * This API can get the flag which indicates the status of flexRAM
 *
 * @param[in] none
 * @return status value
 *         1: FlexRAM is avaliable as traditional RAM only.
 *         0: FlexRAM is not avaliable for traditional RAM operations
 */
static inline uint8_t FLASH_GetRAMReadyStatus(void)
{
    return (uint8_t)((FLASH->STAT & FLASH_STAT_RAMRDY_Msk) >> FLASH_STAT_RAMRDY_Pos);
}

/*!
 * @brief Get flash CNFG register value.
 *
 * @param[in] none.
 * @return CNFG register value
 */
static inline uint32_t FLASH_GetCnfg(void)
{
    return FLASH->CNFG;
}

#ifdef __cplusplus
}
#endif

#endif /* _FLASH_HW_H */

/* =============================================  EOF  ============================================== */
