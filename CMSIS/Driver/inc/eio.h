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
 * @file eio.h
 *
 * @brief This file provides eio integration functions interface.
 *
 */

#ifndef _EIO_H
#define _EIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "osif.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Driver type, Interrupts/Polling/DMA
 */
typedef enum
{
    EIO_DRIVER_TYPE_INTERRUPTS    = 0U,  /*!< Driver uses interrupts for data transfers */
    EIO_DRIVER_TYPE_POLLING       = 1U,  /*!< Driver is based on polling */
    EIO_DRIVER_TYPE_DMA           = 2U   /*!< Driver uses DMA for data transfers */
} eio_driver_type_t;

/*!< EIO interrupt service routine */
typedef void (*eio_isr_t)(void *isrparam);

/*!
 * @brief EIO common context structure
 */
typedef struct
{
    eio_isr_t isr;            /*!< Interrupt handler for this driver instance */
    uint32_t instance;        /*!< EIO device instance number */
    uint8_t resourceCount;    /*!< Count of internal resources used (shifters and timers) */
    uint8_t resourceIndex;    /*!< Index of first used internal resource instance (shifter and timer) */
} eio_common_state_t;

/*!
 * @brief EIO device context structure
 */
typedef struct
{
    uint8_t resourceAllocation;                                 /*!< Mask to keep track of resources allocated on current device */
    mutex_t resourceLock;                                       /*!< Mutex for guarding channel allocation. */
    eio_common_state_t *eioStatePtr[EIO_MAX_SHIFTER_COUNT];     /*!< Array of pointers to runtime state structures. Each EIO instance can have at most
                                                                   one driver instance per shifter. */
} eio_device_state_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes the EIO device
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] deviceState: Pointer to the EIO device context structure
 * @return Error or success status returned by API
 */
status_t EIO_DRV_InitDevice(uint32_t instance, eio_device_state_t *deviceState);

/*!
 * @brief De-initializes the EIO device
 *
 * @param[in] instance: EIO peripheral instance number
 * @return Error or success status returned by API
 */
status_t EIO_DRV_DeinitDevice(uint32_t instance);

/*!
 * @brief Resets the EIO device
 *
 * @param[in] instance: EIO peripheral instance number
 * @return Error or success status returned by API
 */
status_t EIO_DRV_Reset(uint32_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIO_H */

/* =============================================  EOF  ============================================== */
