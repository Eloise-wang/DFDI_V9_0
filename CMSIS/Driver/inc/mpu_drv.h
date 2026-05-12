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
 * @file mpu_drv.h
 *
 * @brief This file provides mpu integration functions interface.
 *
 */

#ifndef _MPU_DRV_H
#define _MPU_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
/*!< Reset value of all master access right in region 0 */
#define REGION_0_ACCESS_RIGHT_RESET_VALUE (0X0003FFFFUL)
/*!< Reset value of end address */
#define END_ADDRESS_RESET_VALUE           (0x1FUL)
/*!< Default value of privilege right */
#define DEFAULT_PRIVILEGE_RIGHT           (MPU_SUPERVISOR_USER_WRX)

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief MPU access error
 */
typedef enum
{
    MPU_ERR_TYPE_READ  = 0U,    /*!< MPU error type: read */
    MPU_ERR_TYPE_WRITE = 1U     /*!< MPU error type: write */
} mpu_err_access_type_t;

/*!
 * @brief MPU access error attributes
 */
typedef enum
{
    MPU_INSTRUCTION_ACCESS_IN_USER_MODE       = 1U,    /*!< Access instruction error in user mode */
    MPU_DATA_ACCESS_IN_USER_MODE              = 2U,    /*!< Access data error in user mode */
    MPU_INSTRUCTION_ACCESS_IN_SUPERVISOR_MODE = 4U,    /*!< Access instruction error in supervisor mode */
    MPU_DATA_ACCESS_IN_SUPERVISOR_MODE        = 8U     /*!< Access data error in supervisor mode */
} mpu_err_attributes_t;

/*!
 * @brief MPU detail error access info
 */
typedef struct
{
    uint8_t                master;                     /*!< Access error master */
    mpu_err_attributes_t   attributes;                 /*!< Access error attributes */
    mpu_err_access_type_t  accessType;                 /*!< Access error type */
    uint16_t               accessCtr;                  /*!< Access error control */
    uint32_t               addr;                       /*!< Access error address */
#if MPU_HAS_PROCESS_IDENTIFIER
    uint8_t                processorIdentification;    /*!< Access error processor identification */
#endif
} mpu_access_err_info_t;

/*!
 * @brief  MPU access rights
 */
typedef enum
{
    MPU_SUPERVISOR_NONE_USER_X       = 0x01U,     /*!< 0b00000001U : ---|--x */
    MPU_SUPERVISOR_NONE_USER_W       = 0x04U,     /*!< 0b00000100U : ---|w-- */
    MPU_SUPERVISOR_NONE_USER_WX      = 0x05U,     /*!< 0b00000101U : ---|w-x */
    MPU_SUPERVISOR_NONE_USER_R       = 0x02U,     /*!< 0b00000010U : ---|-r- */
    MPU_SUPERVISOR_NONE_USER_RX      = 0x03U,     /*!< 0b00000011U : ---|-rx */
    MPU_SUPERVISOR_NONE_USER_WR      = 0x06U,     /*!< 0b00000110U : ---|wr- */
    MPU_SUPERVISOR_NONE_USER_WRX     = 0x07U,     /*!< 0b00000111U : ---|wrx */
    MPU_SUPERVISOR_X_USER_NONE       = 0x08U,     /*!< 0b00001000U : --x|--- */
    MPU_SUPERVISOR_X_USER_W          = 0x0CU,     /*!< 0b00001100U : --x|w-- */
    MPU_SUPERVISOR_X_USER_WX         = 0x0DU,     /*!< 0b00001101U : --x|w-x */
    MPU_SUPERVISOR_X_USER_R          = 0x0AU,     /*!< 0b00001010U : --x|-r- */
    MPU_SUPERVISOR_X_USER_RX         = 0x0BU,     /*!< 0b00001011U : --x|-rx */
    MPU_SUPERVISOR_X_USER_WR         = 0x0EU,     /*!< 0b00001110U : --x|wr- */
    MPU_SUPERVISOR_X_USER_WRX        = 0x0FU,     /*!< 0b00001111U : --x|wrx */
    MPU_SUPERVISOR_W_USER_NONE       = 0x20U,     /*!< 0b00100000U : w--|--- */
    MPU_SUPERVISOR_W_USER_X          = 0x21U,     /*!< 0b00100001U : w--|--x */
    MPU_SUPERVISOR_W_USER_WX         = 0x25U,     /*!< 0b00100101U : w--|w-x */
    MPU_SUPERVISOR_W_USER_R          = 0x22U,     /*!< 0b00100010U : w--|-r- */
    MPU_SUPERVISOR_W_USER_RX         = 0x23U,     /*!< 0b00100011U : w--|-rx */
    MPU_SUPERVISOR_W_USER_WR         = 0x26U,     /*!< 0b00100110U : w--|wr- */
    MPU_SUPERVISOR_W_USER_WRX        = 0x27U,     /*!< 0b00100111U : w--|wrx */
    MPU_SUPERVISOR_WX_USER_NONE      = 0x28U,     /*!< 0b00101000U : w-x|--- */
    MPU_SUPERVISOR_WX_USER_X         = 0x29U,     /*!< 0b00101001U : w-x|--x */
    MPU_SUPERVISOR_WX_USER_W         = 0x2CU,     /*!< 0b00101100U : w-x|w-- */
    MPU_SUPERVISOR_WX_USER_R         = 0x2AU,     /*!< 0b00101010U : w-x|-r- */
    MPU_SUPERVISOR_WX_USER_RX        = 0x2BU,     /*!< 0b00101011U : w-x|-rx */
    MPU_SUPERVISOR_WX_USER_WR        = 0x2EU,     /*!< 0b00101110U : w-x|wr- */
    MPU_SUPERVISOR_WX_USER_WRX       = 0x2FU,     /*!< 0b00101111U : w-x|wrx */
    MPU_SUPERVISOR_R_USER_NONE       = 0x10U,     /*!< 0b00010000U : -r-|--- */
    MPU_SUPERVISOR_R_USER_X          = 0x11U,     /*!< 0b00010001U : -r-|--x */
    MPU_SUPERVISOR_R_USER_W          = 0x14U,     /*!< 0b00010100U : -r-|w-- */
    MPU_SUPERVISOR_R_USER_WX         = 0x15U,     /*!< 0b00010101U : -r-|w-x */
    MPU_SUPERVISOR_R_USER_RX         = 0x13U,     /*!< 0b00010011U : -r-|-rx */
    MPU_SUPERVISOR_R_USER_WR         = 0x16U,     /*!< 0b00010110U : -r-|wr- */
    MPU_SUPERVISOR_R_USER_WRX        = 0x17U,     /*!< 0b00010111U : -r-|wrx */
    MPU_SUPERVISOR_RX_USER_NONE      = 0x18U,     /*!< 0b00011000U : -rx|--- */
    MPU_SUPERVISOR_RX_USER_X         = 0x19U,     /*!< 0b00011001U : -rx|--x */
    MPU_SUPERVISOR_RX_USER_W         = 0x1CU,     /*!< 0b00011100U : -rx|w-- */
    MPU_SUPERVISOR_RX_USER_WX        = 0x1DU,     /*!< 0b00011101U : -rx|w_x */
    MPU_SUPERVISOR_RX_USER_R         = 0x1AU,     /*!< 0b00011010U : -rx|-r- */
    MPU_SUPERVISOR_RX_USER_WR        = 0x1EU,     /*!< 0b00011110U : -rx|wr- */
    MPU_SUPERVISOR_RX_USER_WRX       = 0x1FU,     /*!< 0b00011111U : -rx|wrx */
    MPU_SUPERVISOR_WR_USER_NONE      = 0x30U,     /*!< 0b00110000U : wr-|--- */
    MPU_SUPERVISOR_WR_USER_X         = 0x31U,     /*!< 0b00110001U : wr-|--x */
    MPU_SUPERVISOR_WR_USER_W         = 0x34U,     /*!< 0b00110100U : wr-|w-- */
    MPU_SUPERVISOR_WR_USER_WX        = 0x35U,     /*!< 0b00110101U : wr-|w-x */
    MPU_SUPERVISOR_WR_USER_R         = 0x32U,     /*!< 0b00110010U : wr-|-r- */
    MPU_SUPERVISOR_WR_USER_RX        = 0x33U,     /*!< 0b00110011U : wr-|-rx */
    MPU_SUPERVISOR_WR_USER_WRX       = 0x37U,     /*!< 0b00110111U : wr-|wrx */
    MPU_SUPERVISOR_WRX_USER_NONE     = 0x38U,     /*!< 0b00111000U : wrx|--- */
    MPU_SUPERVISOR_WRX_USER_X        = 0x39U,     /*!< 0b00111001U : wrx|--x */
    MPU_SUPERVISOR_WRX_USER_W        = 0x3CU,     /*!< 0b00111100U : wrx|w-- */
    MPU_SUPERVISOR_WRX_USER_WX       = 0x3DU,     /*!< 0b00111101U : wrx|w-x */
    MPU_SUPERVISOR_WRX_USER_R        = 0x3AU,     /*!< 0b00111010U : wrx|-r- */
    MPU_SUPERVISOR_WRX_USER_RX       = 0x3BU,     /*!< 0b00111011U : wrx|-rx */
    MPU_SUPERVISOR_WRX_USER_WR       = 0x3EU,     /*!< 0b00111110U : wrx|wr- */
    MPU_SUPERVISOR_USER_NONE         = 0x00U,     /*!< 0b00000000U : ---|--- */
    MPU_SUPERVISOR_USER_X            = 0x09U,     /*!< 0b00001001U : --x|--x */
    MPU_SUPERVISOR_USER_W            = 0x24U,     /*!< 0b00100100U : w--|w-- */
    MPU_SUPERVISOR_USER_WX           = 0x2DU,     /*!< 0b00101101U : w_x|w_x */
    MPU_SUPERVISOR_USER_R            = 0x12U,     /*!< 0b00010010U : -r-|-r- */
    MPU_SUPERVISOR_USER_RX           = 0x1BU,     /*!< 0b00011011U : -rx|-rx */
    MPU_SUPERVISOR_USER_WR           = 0x36U,     /*!< 0b00110110U : wr-|wr- */
    MPU_SUPERVISOR_USER_WRX          = 0x3FU      /*!< 0b00111111U : wrx|wrx */
} mpu_access_rights_t;

/*!
 * @brief MPU master access rights.
 */
typedef struct
{
    uint8_t                     masterNum;                  /*!< Master number */
    mpu_access_rights_t         accessRight;                /*!< Access right */
#if MPU_HAS_PROCESS_IDENTIFIER
    bool                        processIdentifierEnable;    /*!< Enables or disables process identifier */
#endif
} mpu_master_access_right_t;

/*!
 * @brief MPU user region configuration structure.
 */
typedef struct
{
    uint32_t                            startAddr;            /*!< Memory region start address */
    uint32_t                            endAddr;              /*!< Memory region end address */
    const mpu_master_access_right_t     *masterAccRight;      /*!< Access permission for masters */
#if MPU_HAS_PROCESS_IDENTIFIER
    uint8_t                             processIdentifier;    /*!< Process identifier */
    uint8_t                             processIdMask;        /*!< Process identifier mask. The setting bit will */
#endif
} mpu_user_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
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
status_t MPU_DRV_Init(uint32_t instance, uint8_t regionCnt, const mpu_user_config_t *userConfigArr);

/*!
 * @brief De-initializes the MPU region.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @return none
 */
void MPU_DRV_Deinit(uint32_t instance);

/*!
 * @brief Sets the region start and end address.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] regionNum: the region number
 * @param[in] startAddr: the region start address
 * @param[in] endAddr: the region end address
 * @return none
 */
void MPU_DRV_SetRegionAddr(uint32_t instance, uint8_t regionNum, uint32_t startAddr, uint32_t endAddr);

/*!
 * @brief Sets the region configuration.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] regionNum: the region number
 * @param[in] userConfigPtr: the region configuration structure pointer
 * @return operation status
 *        - STATUS_SUCCESS: Operation was successful
 *        - STATUS_ERROR: Operation failed
 */
status_t MPU_DRV_SetRegionConfig(uint32_t instance, uint8_t regionNum, const mpu_user_config_t *userConfigPtr);

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
                                       const mpu_master_access_right_t * accessRightsPtr);

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
bool MPU_DRV_GetDetailErrorAccessInfo(uint32_t instance, uint8_t slavePortNum, mpu_access_err_info_t *errInfoPtr);

/*!
 * @brief Gets default region configuration. Grants all access rights for masters and disable PID.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[out] masterAccRight: the pointer to master configuration structure, see #mpu_master_access_right_t
 *                            the length of array should be defined by number of masters supported by hardware
 * @return The default region configuration, see #mpu_user_config_t
 */
mpu_user_config_t MPU_DRV_GetDefaultRegionConfig(mpu_master_access_right_t * masterAccRight);

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
void MPU_DRV_EnableRegion(uint32_t instance, uint8_t regionNum, bool enable);

/*!
 * @brief set mpu master process id.
 *
 * @param[in] instance: the MPU peripheral instance number
 * @param[in] masterNum: the master number
 * @param[in] pid: mpu master process id
 * @return  none
 */
void MPU_DRV_SetMasterProcessIdentifier(uint32_t instance, uint8_t masterNum, uint8_t pid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MPU_DRV_H */

/* =============================================  EOF  ============================================== */
