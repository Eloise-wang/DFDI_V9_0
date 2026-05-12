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
 * @file mpu_hw.h
 *
 * @brief This file provides mpu hardware integration functions interface.
 *
 */

#ifndef _MPU_HW_H
#define _MPU_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "mpu_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Sets region start and end address
 * Please note that using this function will clear the valid bit of the region
 * and a further validation might be needed.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @param[in] startAddr: region start addr
 * @param[in] endAddr: region end addr
 * @return none
 */
void MPU_SetRegionAddr(MPU_Type * const base, uint8_t regionNum, uint32_t startAddr, uint32_t endAddr);

/*!
 * @brief Sets access permission for master in region descriptor.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @param[in] masterAccRight: Pointer to master number
 * @return none
 */
void MPU_SetMasterAccessRight(MPU_Type * const base, uint8_t regionNum, const mpu_master_access_right_t * masterAccRight);

/*!
 * @brief Initializes the MPU module and all regions will be invalid after cleared access permission.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @return none
 */
void MPU_InitRegion(MPU_Type * const base, uint8_t regionNum);

/*!
 * @brief Reports the MPU access error detail information of error slave port.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] slaveNum: the slave port number
 * @param[out] errInfoPtr: the pointer to the MPU access error information
 * @return none
 */
void MPU_GetErrorInfo(const MPU_Type * const base, uint8_t slaveNum, mpu_access_err_info_t * errInfoPtr);

/*!
 * @brief Reports the default master access rights.
 *
 * @param[in] masterNum: the master number
 * @param[out] masterAccRight: the pointer to the master access rights
 * @return none
 */
void MPU_GetDefaultMasterAccRight(uint8_t masterNum, mpu_master_access_right_t * masterAccRight);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enables/Disables the MPU module.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] enable：enable State
 *                    - true: Enable the MPU module
 *                    - false: Disable the MPU module
 * @return none
 */
static inline void MPU_Enable(MPU_Type * const base, bool enable)
{
    if (enable == true)
    {
        base->CESR |= MPU_CESR_VLD_Msk;
    }
    else
    {
        base->CESR &= ~MPU_CESR_VLD_Msk;
    }

    (void)base->CESR;
}

/*!
 * @brief Enables/Disables the region descriptor
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @param[in] enable: enable Valid state
 *                    -true: Region descriptor is valid
 *                    -false: Region descriptor is invalid
 * @return none
 */
static inline void MPU_SetRegionValidCmd(MPU_Type * const base, uint8_t regionNum, bool enable)
{
    if (enable == true)
    {
        base->WORD3_RGD[regionNum] |= MPU_RGD0_WORD3_VLD_Msk;
    }
    else
    {
        base->WORD3_RGD[regionNum] &= ~MPU_RGD0_WORD3_VLD_Msk;
    }
}

#if MPU_HAS_PROCESS_IDENTIFIER
/*!
 * @brief Sets the process identifier.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @param[in] pid: the process identifier
 * @return none
 */
static inline void MPU_SetProcessIdentifier(MPU_Type * const base, uint8_t regionNum, uint8_t pid)
{
    base->WORD3_RGD[regionNum] &= ~MPU_RGD0_WORD3_PID_Msk;
    base->WORD3_RGD[regionNum] |= (((uint32_t)(((uint32_t)(pid)) << MPU_RGD0_WORD3_PID_Pos)) & MPU_RGD0_WORD3_PID_Msk);
}

/*!
 * @brief Sets the process identifier mask.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @param[in] pidMask: the process identifier mask
 * @return none
 */
static inline void MPU_SetProcessIdentifierMask(MPU_Type * const base, uint8_t regionNum, uint8_t pidMask)
{
    base->WORD3_RGD[regionNum] &= ~MPU_RGD0_WORD3_PIDMASK_Msk;
    base->WORD3_RGD[regionNum] |= (((uint32_t)(((uint32_t)(pidMask)) << MPU_RGD0_WORD3_PIDMASK_Pos)) & MPU_RGD0_WORD3_PIDMASK_Msk);
}

/*!
 * @brief set mpu master process id.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] masterNum: the master number
 * @param[in] pid: mpu master process id
 * @return  none
 */
static inline void MPU_SetMasterProcessIdentifier(MPU_Type * const base, uint8_t masterNum, uint8_t pid)
{
    if (masterNum == MPU_MASTER_CORE)
    {
        MODIFY_REG32(base->MPID, MPU_MPID_M0PID_Msk, MPU_MPID_M0PID_Pos, pid);
    }
    else if (masterNum == MPU_MASTER_DEBUGGER)
    {
        MODIFY_REG32(base->MPID, MPU_MPID_M1PID_Msk, MPU_MPID_M1PID_Pos, pid);
    }
    else
    {
        /* DMA Master No PID  */
    }
}
#endif

/*!
 * @brief Gets the error status of a specified slave port.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] slaveNum: the slave port number
 * @return The slave ports error status
 *                  - true: error happens in this slave port
 *                  - false: error didn't happen in this slave port
 */
static inline bool MPU_GetErrorStatus(const MPU_Type * const base, uint8_t slaveNum)
{
    return (bool)((base->CESR & (MPU_CESR_SPERR0_Msk << slaveNum)) != 0U);
}

/*!
 * @brief Clears the error flag of a specified slave port.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] slaveNum: the slave port number
 * @return none
 */
static inline void MPU_ClearErrorFlag(MPU_Type * const base, uint8_t slaveNum)
{
    uint32_t temp = 0U;
    /* to prevent mistaken writing 1 to clear */
    temp = base->CESR;
    temp &= ~(MPU_CESR_SPERR0_Msk | MPU_CESR_SPERR1_Msk | MPU_CESR_SPERR2_Msk);
    temp |= (MPU_CESR_SPERR0_Msk << slaveNum);

    base->CESR = temp;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MPU_HW_H */

/* =============================================  EOF  ============================================== */
