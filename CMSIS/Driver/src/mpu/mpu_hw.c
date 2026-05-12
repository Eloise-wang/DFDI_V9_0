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
 * @file mpu_hw.c
 *
 * @brief This file provides mpu hardware access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "mpu_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Sets region start and end address,Please note that using this function will clear the valid bit of the region.
 *  
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @param[in] startAddr: region start addr
 * @param[in] endAddr: region end addr
 * @return none
 */
void MPU_SetRegionAddr(MPU_Type * const base, uint8_t regionNum, uint32_t startAddr, uint32_t endAddr)
{
    /* Write start address to RGD_WORD0 */
    base->WORD0_RGD[regionNum] = startAddr;

    /* Write end address to RGD_WORD1 */
    base->WORD1_RGD[regionNum] = endAddr;
}

/*!
 * @brief Sets access permission for master in region descriptor.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @param[in] masterAccRight: Pointer to master number
 * @return none
 */
void MPU_SetMasterAccessRight(MPU_Type * const base, uint8_t regionNum, const mpu_master_access_right_t * masterAccRight)
{
    uint32_t accRight;
    uint32_t masterNum;
    uint32_t temp;
    uint32_t accMask;

    accMask = 0U;
    masterNum = masterAccRight->masterNum;
    accRight = (uint32_t)masterAccRight->accessRight;

    accMask |= MPU_RGDAAC0_M0UR_Msk;
    accMask |= MPU_RGDAAC0_M0UW_Msk;
    accMask |= MPU_RGDAAC0_M0UX_Msk;
    accMask |= MPU_RGDAAC0_M0SR_Msk;
    accMask |= MPU_RGDAAC0_M0SW_Msk;
    accMask |= MPU_RGDAAC0_M0SX_Msk;
    accMask = accMask << (masterNum * MPU_RGDAAC0_M1UX_Pos);

    accRight = accRight << (masterNum * MPU_RGDAAC0_M1UX_Pos);

#if MPU_HAS_PROCESS_IDENTIFIER

    if (masterNum == MPU_MASTER_CORE)
    {
        accMask |= MPU_RGDAAC0_M0PE_Msk;
        if (masterAccRight->processIdentifierEnable == true)
        {
            accRight |= MPU_RGDAAC0_M0PE_Msk;
        }
    }
    if (masterNum == MPU_MASTER_DEBUGGER)
    {
        accMask |= MPU_RGDAAC0_M1PE_Msk;
        if (masterAccRight->processIdentifierEnable == true)
        {
            accRight |= MPU_RGDAAC0_M1PE_Msk;
        }
    }
#endif

    /* Set access rights */
    temp = base->RGDAAC[regionNum];
    temp = (temp & ~accMask) | accRight;
    base->RGDAAC[regionNum] = temp;
}

/*!
 * @brief Initializes the MPU module and all regions will be invalid after cleared access permission.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] regionNum: the region number
 * @return none
 */
void MPU_InitRegion(MPU_Type * const base, uint8_t regionNum)
{
    if (0U == regionNum)
    {
        /* Sets default access right for region 0 */
        base->RGDAAC[0U] = REGION_0_ACCESS_RIGHT_RESET_VALUE;
    }
    else
    {
        base->WORD3_RGD[regionNum] = 0U;
        base->WORD2_RGD[regionNum] = 0U;
        base->WORD1_RGD[regionNum] = END_ADDRESS_RESET_VALUE;
        base->WORD0_RGD[regionNum] = 0U;
    }
}

/*!
 * @brief Reports the MPU access error detail information of error slave port.
 *
 * @param[in] base: the MPU peripheral base address
 * @param[in] slaveNum: the slave port number
 * @param[out] errInfoPtr: the pointer to the MPU access error information
 * @return none
 */
void MPU_GetErrorInfo(const MPU_Type * const base, uint8_t slaveNum, mpu_access_err_info_t * errInfoPtr)
{
    uint32_t temp;

    /* Read the Error Detail Register for the slave port */
    temp = base->EDR[slaveNum];

    /* Report Error Region Number to user */
    errInfoPtr->accessCtr = (uint16_t)((temp & MPU_EDR0_EACD_Msk) >> MPU_EDR0_EACD_Pos);

    /* Report Error Master Number to user */
    errInfoPtr->master = (uint8_t)((temp & MPU_EDR0_EMN_Msk) >> MPU_EDR0_EMN_Pos);

    /* Report Error Attributes to user */
    errInfoPtr->attributes = (mpu_err_attributes_t)((temp & MPU_EDR0_EATTR_Msk) >> MPU_EDR0_EATTR_Pos);

    /* Report Error Read/Write to user */
    errInfoPtr->accessType = (mpu_err_access_type_t)((temp & MPU_EDR0_ERW_Msk) >> MPU_EDR0_ERW_Pos);

    /* Report Error Address to user */
    errInfoPtr->addr = base->EAR[slaveNum];

#if MPU_HAS_PROCESS_IDENTIFIER
    /* Report Error Process Identification to user */
    errInfoPtr->processorIdentification = (uint8_t)((temp & MPU_EDR0_EPID_Msk) >> MPU_EDR0_EPID_Pos);
#endif
}

/*!
 * @brief Reports the default master access rights.
 *
 * @param[in] masterNum: the master number
 * @param[out] masterAccRight: the pointer to the master access rights
 * @return none
 */
void MPU_GetDefaultMasterAccRight(uint8_t masterNum, mpu_master_access_right_t * masterAccRight)
{
    masterAccRight->accessRight = DEFAULT_PRIVILEGE_RIGHT;

#if MPU_HAS_PROCESS_IDENTIFIER
    masterAccRight->processIdentifierEnable = false;
#endif
}

/* =============================================  EOF  ============================================== */
