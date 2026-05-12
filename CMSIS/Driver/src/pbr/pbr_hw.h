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
 * @file pbr_hw.h
 *
 * @brief pbr access register inline function definition.
 *
 */

#ifndef _PBR_HW_H
#define _PBR_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "pbr_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set master core privilege.
 *
 * @param[in] mtr: master trust for read
 * @param[in] mtw: master trust for write
 * @param[in] mpl: master privilege level
 * @return none
 */
static inline void PBR_SetCorePrivilege(pbr_master_tr_t mtr, pbr_master_tw_t mtw, pbr_master_pl_t mpl)
{
    WRITE_REG32(PBR->MPR_CORE, ((uint32_t)mtr << PBR_MPR_CORE_MTR_Pos) | ((uint32_t)mtw << PBR_MPR_CORE_MTW_Pos) | (uint32_t)mpl);
}

/*!
 * @brief Set master debugger privilege.
 *
 * @param[in] mtr: master trust for read
 * @param[in] mtw: master trust for write
 * @param[in] mpl: master privilege level
 * @return none
 */
static inline void PBR_SetDebuggerPrivilege(pbr_master_tr_t mtr, pbr_master_tw_t mtw, pbr_master_pl_t mpl)
{
    WRITE_REG32(PBR->MPR_DEBUG, ((uint32_t)mtr << PBR_MPR_DEBUG_MTR_Pos) | ((uint32_t)mtw << PBR_MPR_DEBUG_MTW_Pos) | (uint32_t)mpl);
}

/*!
 * @brief Set master dma privilege.
 *
 * @param[in] mtr: master trust for read
 * @param[in] mtw: master trust for write
 * @param[in] mpl: master privilege level
 * @return none
 */
static inline void PBR_SetDMAPrivilege(pbr_master_tr_t mtr, pbr_master_tw_t mtw, pbr_master_pl_t mpl)
{
    WRITE_REG32(PBR->MPR_DMA, ((uint32_t)mtr << PBR_MPR_DMA_MTR_Pos) | ((uint32_t)mtw << PBR_MPR_DMA_MTW_Pos) | (uint32_t)mpl);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RCM_HW_H */

/* =============================================  EOF  ============================================== */
