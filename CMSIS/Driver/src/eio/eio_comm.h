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
 * AutoChips Inc. (C) 2020. All rights reserved.
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
 * @file eio_comm.h
 *
 * @brief This file provides eio comm integration functions interfaces.
 *
 */

#ifndef _EIO_COMM_H
#define _EIO_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "eio.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/* Table of base addresses for EIO instances. */
extern EIO_Type * const g_eioBase[EIO_INSTANCE_COUNT];

/* Pointer to device state structures. This structure contains data common to all drivers on one device */
extern eio_device_state_t *g_eioDeviceStatePtr[EIO_INSTANCE_COUNT];

/* Table for EIO IRQ numbers */
extern const IRQn_Type g_eioIrqId[EIO_INSTANCE_COUNT];

/* EIO clock sources, for getting the input clock frequency */
extern const clock_names_t g_eioClock[EIO_INSTANCE_COUNT];

/* EIO DMA request sources */
extern const uint8_t g_eioDMASrc[EIO_INSTANCE_COUNT][EIO_MAX_SHIFTER_COUNT];

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes an instance of EIO driver
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] driver: Pointer to the EIO common driver context structure
 * @return Error or success status returned by API
 */
status_t EIO_DRV_InitDriver(uint32_t instance, eio_common_state_t *driver);

/*!
 * @brief De-initializes an instance of EIO driver
 *
 * @param[in] driver: Pointer to the EIO common driver context structure
 * @return Error or success status returned by API
 */
status_t EIO_DRV_DeinitDriver(const eio_common_state_t *driver);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIO_COMM_H */

/* =============================================  EOF  ============================================== */
