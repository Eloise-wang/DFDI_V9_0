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
 * @file pbr_drv.c
 *
 * @brief This file provides pbr module integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "pbr_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set master privilege.
 *
 * @param[in] master: pbr master
 * @param[in] mtr: master trust for read
 * @param[in] mtw: master trust for write
 * @param[in] mpl: master privilege level
 * @return none
 */
void PBR_DRV_SetMasterPrivilege(pbr_master_t master, pbr_master_tr_t mtr, pbr_master_tw_t mtw, pbr_master_pl_t mpl)
{
    DEVICE_ASSERT(master < PBR_MASTER_MAX);
    DEVICE_ASSERT(mtr <= PBR_MASTER_TRUST_READ);
    DEVICE_ASSERT(mtw <= PBR_MASTER_TRUST_WRITE);
    DEVICE_ASSERT(mpl <= PBR_MASTER_NOT_FORCE_USER_MODE);

    if (PBR_MASTER_CORE == master)
    {
        PBR_SetCorePrivilege(mtr, mtw, mpl);
    }
    else if (PBR_MASTER_DEBUGGER == master)
    {
        PBR_SetDebuggerPrivilege(mtr, mtw, mpl);
    }
    else if (PBR_MASTER_DMA == master)
    {
        PBR_SetDMAPrivilege(mtr, mtw, mpl);
    }
    else
    {
    }
}

/*!
 * @brief Set peripheral access control.
 *
 * @param[in] peri: pbr peripheral
 * @param[in] sp: peripheral supervisor protect
 * @param[in] wp: peripheral write protect
 * @param[in] tp: peripheral trust protect
 * @return none
 */
void PBR_DRV_SetPeripheralAccess(pbr_peripheral_t peri, pbr_peri_sp_t sp, pbr_peri_wp_t wp, pbr_peri_tp_t tp)
{
    DEVICE_ASSERT(peri < PBR_PERI_MAX);
    DEVICE_ASSERT(sp <= PBR_PERI_REQ_SUPER_PRIV);
    DEVICE_ASSERT(wp <= PBR_PERI_WRITE_PROTECT);
    DEVICE_ASSERT(tp <= PBR_PERI_NOT_ALLOW_UNTRUST_MASTER);

    uint32_t value = 0U, offset = 0U, address = 0U;

    address = PBR_BASE + 0x10U + (uint32_t)peri / PERIPH_ONE_GROUP_NUM * 4U;
    value = *(__IOM uint32_t *)address;
    offset = (((uint32_t)peri % PERIPH_ONE_GROUP_NUM) * PERIPH_ACCESS_BIT_NUM);
    value &= ~(PERIPH_ACCESS_MASK << offset);
    value |= (((uint32_t)sp << PBR_PACRA_PWM0_SP_Pos) | ((uint32_t)wp << PBR_PACRA_PWM0_WP_Pos) | ((uint32_t)tp)) << offset;
    *(__IOM uint32_t *)address = value;
}

/* =============================================  EOF  ============================================== */
