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
 * @file mpu_drv.c
 *
 * @brief This file provides mpu integration functions interface.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "mpu_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of base addresses for MPU instances. */
static MPU_Type * const s_mpuBase[MPU_INSTANCE_MAX] = MPU_BASE_PTRS;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief The function sets the MPU regions according to user input and then enables the MPU,
 *        Please note that access rights for region 0 will always be configured.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] regionCnt: the number of configured regions
 * @param[in] userConfigArr: the pointer to the array of MPU user configure structure, see #mpu_user_config_t
 * @return operation status
 *         - STATUS_SUCCESS: Operation was successful
 *         - STATUS_ERROR: Operation failed
 */
status_t MPU_DRV_Init(uint32_t instance, uint8_t regionCnt, const mpu_user_config_t *userConfigArr)
{
    DEVICE_ASSERT(instance < MPU_INSTANCE_MAX);
    DEVICE_ASSERT((regionCnt > 0U) && (regionCnt <= MPU_RGD_COUNT));
    DEVICE_ASSERT(userConfigArr != NULL);

    MPU_Type * base = s_mpuBase[instance];
    status_t retStatus = STATUS_SUCCESS;
    uint8_t regionNum = 0U;

    /* De-initializes all region descriptor */
    MPU_DRV_Deinit(instance);

    for (regionNum = 0U; regionNum < regionCnt; regionNum++)
    {
        /* Sets region configuration */
        retStatus = MPU_DRV_SetRegionConfig(instance, regionNum, &userConfigArr[regionNum]);

        if (retStatus != STATUS_SUCCESS)
        {
            break;
        }
    }

    if (retStatus == STATUS_SUCCESS)
    {
        /* Enables the mpu module operation */
        MPU_Enable(base, true);
    }

    return retStatus;
}

/*!
 * @brief De-initializes the MPU region.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @return none
 */
void MPU_DRV_Deinit(uint32_t instance)
{
    DEVICE_ASSERT(instance < MPU_INSTANCE_MAX);

    MPU_Type * base = s_mpuBase[instance];
    uint8_t regionNum = 0U;
    uint8_t slaveNum = 0U;

    /* Disables the entire MPU module */
    MPU_Enable(base, false);

    for (slaveNum = 0U; slaveNum < MPU_SLAVE_COUNT; slaveNum++)
    {
        /* Clears slave error flag */
        MPU_ClearErrorFlag(base, slaveNum);
    }

    /* De-initializes all region descriptors */
    for (regionNum = 0U; regionNum < MPU_RGD_COUNT; regionNum++)
    {
        /* Resets the region configuration to default */
        MPU_InitRegion(base, regionNum);
    }
}

/*!
 * @brief Sets the region start and end address.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] regionNum: the region number
 * @param[in] startAddr: the region start address
 * @param[in] endAddr: the region end address.
 * @return none
 */
void MPU_DRV_SetRegionAddr(uint32_t instance, uint8_t regionNum, uint32_t startAddr, uint32_t endAddr)
{
    DEVICE_ASSERT(instance < MPU_INSTANCE_MAX);
    DEVICE_ASSERT(regionNum < MPU_RGD_COUNT);
    DEVICE_ASSERT(startAddr <= endAddr);

    MPU_Type * base = s_mpuBase[instance];

    /* Sets a region's start and end addresses */
    MPU_SetRegionAddr(base, regionNum, startAddr, endAddr);

    /* Re-enables the region descriptor valid bit */
    MPU_SetRegionValidCmd(base, regionNum, true);
}

/*!
 * @brief Sets the region configuration.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] regionNum: the region number
 * @param[in] userConfigPtr: the region configuration structure pointer
 * @return operation status
 *         - STATUS_SUCCESS: Operation was successful
 *         - STATUS_ERROR: Operation failed
 */
status_t MPU_DRV_SetRegionConfig(uint32_t instance, uint8_t regionNum, const mpu_user_config_t *userConfigPtr)
{
    DEVICE_ASSERT(instance < MPU_INSTANCE_MAX);
    DEVICE_ASSERT(regionNum < MPU_RGD_COUNT);
    DEVICE_ASSERT(userConfigPtr != NULL);
    DEVICE_ASSERT(userConfigPtr->startAddr <= userConfigPtr->endAddr);

    MPU_Type * base = s_mpuBase[instance];
    uint8_t masterIdx = 0U;
    status_t returnCode = STATUS_SUCCESS;

    /* Sets access right for masters */
    for (masterIdx = 0U; masterIdx < MPU_MASTER_COUNT; masterIdx++)
    {
        if (userConfigPtr->masterAccRight[masterIdx].masterNum < MPU_MASTER_COUNT)
        {
            MPU_SetMasterAccessRight(base,
                                     regionNum,
                                     &userConfigPtr->masterAccRight[masterIdx]);
        }
        else
        {
            returnCode = STATUS_ERROR;
            break;
        }
    }

    /* Configures address, process identifier except region 0 */
    if (regionNum > 0U)
    {
        if (returnCode == STATUS_SUCCESS)
        {
            /* Sets a region's start and end addresses */
            MPU_SetRegionAddr(base, regionNum, userConfigPtr->startAddr, userConfigPtr->endAddr);

#if MPU_HAS_PROCESS_IDENTIFIER
            /* Sets process identifier */
            MPU_SetProcessIdentifier(base, regionNum, userConfigPtr->processIdentifier);

            /* Sets process identifier mask */
            MPU_SetProcessIdentifierMask(base, regionNum, userConfigPtr->processIdMask);
#endif
            /* Enables the region descriptor valid bit */
            MPU_SetRegionValidCmd(base, regionNum, true);
        }
    }

    return returnCode;
}

/*!
 * @brief Configures access permission.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] regionNum: the MPU region number
 * @param[in] accessRightsPtr: the pointer to access permission structure
 * @return operation status
 *         - STATUS_SUCCESS: Operation was successful
 *         - STATUS_ERROR: Operation failed
 */
status_t MPU_DRV_SetMasterAccessRights(uint32_t instance,
                                       uint8_t regionNum,
                                       const mpu_master_access_right_t * accessRightsPtr)
{
    DEVICE_ASSERT(instance < MPU_INSTANCE_MAX);
    DEVICE_ASSERT(regionNum < MPU_RGD_COUNT);
    DEVICE_ASSERT(accessRightsPtr != NULL);

    MPU_Type * base = s_mpuBase[instance];
    status_t returnCode = STATUS_SUCCESS;

    /* Sets access rights for master */
    if (accessRightsPtr->masterNum < MPU_MASTER_COUNT)
    {
        MPU_SetMasterAccessRight(base, regionNum, accessRightsPtr);
    }
    else
    {
        returnCode = STATUS_ERROR;
    }

    return returnCode;
}

/*!
 * @brief Checks and gets the MPU access error detail information for a slave port.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] slavePortNum: the slave port number to get Error Detail
 * @param[out] errInfoPtr: the pointer to access error info structure
 * @return operation status
 *         - true: An error has occurred
 *         - false: No error has occurred
 */
bool MPU_DRV_GetDetailErrorAccessInfo(uint32_t instance, uint8_t slavePortNum, mpu_access_err_info_t *errInfoPtr)
{
    DEVICE_ASSERT(instance < MPU_INSTANCE_MAX);
    DEVICE_ASSERT(slavePortNum < MPU_SLAVE_COUNT);
    DEVICE_ASSERT(errInfoPtr != NULL);

    MPU_Type * base = s_mpuBase[instance];
    /* Gets slaver port error status */
    bool errorStatus = MPU_GetErrorStatus(base, slavePortNum);

    /* Check if there is access violation in the slave port */
    if (errorStatus)
    {
        /* Get the slave port detail error */
        MPU_GetErrorInfo(base, slavePortNum, errInfoPtr);

        /* Clears slave port error flag */
        MPU_ClearErrorFlag(base, slavePortNum);
    }

    return errorStatus;
}

/*!
 * @brief Gets default region configuration,Grants all access rights for masters and disable PID.
 *
 * @param[out] masterAccRight: the pointer to master configuration structure, see #mpu_master_access_right_t
 *                            the length of array should be defined by number of masters supported by hardware
 * @return The default region configuration, see #mpu_user_config_t
 */
mpu_user_config_t MPU_DRV_GetDefaultRegionConfig(mpu_master_access_right_t * masterAccRight)
{
    DEVICE_ASSERT(masterAccRight != NULL);

    uint8_t masterIdx = 0U;
    uint8_t masterNum[MPU_MASTER_COUNT] = MPU_MASTER;
    mpu_user_config_t regionConfig;

    /* Allocates entire memory */
    regionConfig.startAddr = 0x0U;
    regionConfig.endAddr = 0xFFFFFFFFU;

    /* All access rights of masters are allowed */
    for (masterIdx = 0U; masterIdx < MPU_MASTER_COUNT; masterIdx++)
    {
        MPU_GetDefaultMasterAccRight(masterNum[masterIdx], &masterAccRight[masterIdx]);
    }
    regionConfig.masterAccRight = masterAccRight;

#if MPU_HAS_PROCESS_IDENTIFIER
    /* Disables process identifier */
    regionConfig.processIdentifier = 0U;
    regionConfig.processIdMask = 0U;
#endif

    return regionConfig;
}

/*!
 * @brief Enables/Disables region descriptor
 *        Please note that region 0 should not be disabled.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] regionNum: the region number
 * @param[in] enable Valid state
 *            - true: Enable region
 *            - false: Disable region
 */
void MPU_DRV_EnableRegion(uint32_t instance, uint8_t regionNum, bool enable)
{
    DEVICE_ASSERT(instance < MPU_INSTANCE_MAX);
    DEVICE_ASSERT(regionNum < MPU_RGD_COUNT);
    DEVICE_ASSERT(regionNum > 0U);

    MPU_Type * base = s_mpuBase[instance];

    /* Enables/Disables region descriptor */
    MPU_SetRegionValidCmd(base, regionNum, enable);
}

#if MPU_HAS_PROCESS_IDENTIFIER

/*!
 * @brief Set MPU master process id.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] masterNum: the master number
 * @param[in] pid: mpu master process id
 * @return  none
 */
void MPU_DRV_SetMasterProcessIdentifier(uint32_t instance, uint8_t masterNum, uint8_t pid)
{
    DEVICE_ASSERT(instance < MPU_INSTANCE_MAX);

    MPU_Type * base = s_mpuBase[instance];

    MPU_SetMasterProcessIdentifier(base, masterNum, pid);
}
#endif

/* =============================================  EOF  ============================================== */
