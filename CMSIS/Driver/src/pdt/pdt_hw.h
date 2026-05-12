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
 * @file pdt_hw.h
 *
 * @brief This file provides PDT hardware integration functions.
 *
 */

#ifndef _PDT_HW_H
#define _PDT_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "pdt_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Config PDT pulse out width and output enable.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] config: pointer to pulse out configuration structure
 * @return none
 */
void PDT_ConfigPulseOut(PDT_Type * const base, const pdt_pulse_out_config_type_t *config);

/*!
 * @brief Config PDT timer.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] config: pointer to timer configuration structure
 * @return none
 */
void PDT_ConfigTimer(PDT_Type * const base, const pdt_timer_config_t *config);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set PDT multiplication factor for prescaler.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] mult: multiplication factor select for prescaler
 *            - PDT_CLK_PREMULT_FACT_AS_1
 *            - PDT_CLK_PREMULT_FACT_AS_10
 *            - PDT_CLK_PREMULT_FACT_AS_20
 *            - PDT_CLK_PREMULT_FACT_AS_40
 * @return none
 */
static inline void PDT_SetMultForPrescaler(PDT_Type * const base, pdt_clk_prescaler_mult_factor_t mult)
{
    MODIFY_REG32(base->SC, PDT_SC_MULT_Msk, PDT_SC_MULT_Pos, mult);
}

/*!
 * @brief Set PDT clock prescaler.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] psc: prescaler divide
 *            - PDT_CLK_PREDIV_BY_1
 *            - PDT_CLK_PREDIV_BY_2
 *            - PDT_CLK_PREDIV_BY_4
 *            - PDT_CLK_PREDIV_BY_8
 *            - PDT_CLK_PREDIV_BY_16
 *            - PDT_CLK_PREDIV_BY_32
 *            - PDT_CLK_PREDIV_BY_64
 *            - PDT_CLK_PREDIV_BY_128
 * @return none
 */
static inline void PDT_SetClockPrescaler(PDT_Type * const base, pdt_clock_prescaler_div_t psc)
{
    MODIFY_REG32(base->SC, PDT_SC_PRE_Msk, PDT_SC_PRE_Pos, psc);
}

/*!
 * @brief Set PDT count mode.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] state: continuous mode enable
 *            - true
 *            - false
 * @return none
 */
static inline void PDT_SetCountMode(PDT_Type * const base, bool state)
{
    MODIFY_REG32(base->SC, PDT_SC_CONT_Msk, PDT_SC_CONT_Pos, state);
}

/*!
 * @brief Set PDT load values cmd.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @return none
 */
static inline void PDT_LoadValuesCmd(PDT_Type * const base)
{
    MODIFY_REG32(base->SC, PDT_SC_LDOK_Msk, PDT_SC_LDOK_Pos, true);
}

/*!
 * @brief Set PDT load mode.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] mode: PDT load mode
 *            - PDT_LOAD_VAL_IMMEDIATELY
 *            - PDT_LOAD_VAL_AT_MODULO_COUNTER
 *            - PDT_LOAD_VAL_AT_NEXT_TRIGGER
 *            - PDT_LOAD_VAL_AT_MODULO_COUNTER_OR_NEXT_TRIGGER
 * @return none
 */
static inline void PDT_SetLoadMode(PDT_Type * const base, pdt_load_value_mode_t mode)
{
    MODIFY_REG32(base->SC, PDT_SC_LDMOD_Msk, PDT_SC_LDMOD_Pos, mode);
}

/*!
 * @brief Set PDT module interrupt enable.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PDT_SetInterruptEnable(PDT_Type * const base, bool state)
{
    MODIFY_REG32(base->SC, PDT_SC_PDTIE_Msk, PDT_SC_PDTIE_Pos, state);
}

/*!
 * @brief Get PDT timer interrupt flag.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @return PDT interrupt flag
 */
#define PDT_GetTimerIntFlag(base)         (READ_BIT32((base)->SC, PDT_SC_PDTIF_Msk) >> PDT_SC_PDTIF_Pos)

/*!
 * @brief Clear PDT timer interrupt flag.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @return none
 */
#define PDT_ClearTimerIntFlag(base)         CLEAR_BIT32((base)->SC, PDT_SC_PDTIF_Msk)

/*!
 * @brief PDT Enable.
 *
 * @param[in] base: the PDT base address
 *               - PDT0
 *               - PDT1
 * @return none
 */
static inline void PDT_Enable(PDT_Type * const base)
{
    MODIFY_REG32(base->SC, PDT_SC_PDTEN_Msk, PDT_SC_PDTEN_Pos, true);
}

/*!
 * @brief PDT Disable.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @return none
 */
static inline void PDT_Disable(PDT_Type * const base)
{
    MODIFY_REG32(base->SC, PDT_SC_PDTEN_Msk, PDT_SC_PDTEN_Pos, false);
}

/*!
 * @brief Set PDT SoftTrigger Cmd.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @return none
 */
static inline void PDT_SoftTriggerCmd(PDT_Type * const base)
{
    MODIFY_REG32(base->SC, PDT_SC_SWTRIG_Msk, PDT_SC_SWTRIG_Pos, true);
}

/*!
 * @brief Set PDT trigger input source.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] source: PDT trigger input source
 *            - PDT_HARDWARE_TRIGGER
 *            - PDT_SOFTWARE_TRIGGER
 * @return none
 */
static inline void PDT_SetTriggerSource(PDT_Type * const base, pdt_trigger_src_t source)
{
    MODIFY_REG32(base->SC, PDT_SC_TRGSEL_Msk, PDT_SC_TRGSEL_Pos, source);
}

/*!
 * @brief Set PDT Trigger delay bypass Enable.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] state: bypass enable
 *            - true
 *            - false
 * @return none
 */
static inline void PDT_SetTriggerDelayBypass(PDT_Type * const base, bool state)
{
    MODIFY_REG32(base->SC, PDT_SC_Bypass_En_Msk, PDT_SC_Bypass_En_Pos, state);
}

/*!
 * @brief Set PDT Timer Modulus Value.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] value: mod value
 * @return none
 */
static inline void PDT_SetTimerModulusValue(PDT_Type * const base, uint16_t value)
{
    MODIFY_REG32(base->MOD, PDT_MOD_MOD_Msk, PDT_MOD_MOD_Pos, value);
}

/*!
 * @brief Set PDT trigger Delay Value.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] chn: delay channel(0~7)
 * @param[in] value: trigger delay value
 * @return none
 */
static inline void PDT_SetTriggerDelayValue(PDT_Type * const base, uint8_t chn, uint16_t value)
{
    MODIFY_REG32(base->DLY[chn].DLY, PDT_DLY0_DLY0_Msk, PDT_DLY0_DLY0_Pos, value);
}

/*!
 * @brief Set PDT interrupt delay value.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] value: intrrupt delay value
 * @return none
 */
static inline void PDT_SetValueForTimerInterrupt(PDT_Type * const base, uint16_t value)
{
    MODIFY_REG32(base->IDLY, PDT_IDLY_IDLY_Msk, PDT_IDLY_IDLY_Pos, value);
}

/*!
 * @brief Set PDT trigger delay enable.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] dlyEnable: all 8 channel dly enable mask
 * @return none
 */
static inline void PDT_SetTriggerDelayEnable(PDT_Type * const base, uint8_t dlyEnable)
{
    MODIFY_REG32(base->DLYn_EN, PDT_DLY_EN_Msk, PDT_DLYn_EN_DLY0_En_Pos, dlyEnable);
}

/*!
 * @brief Set PDT pulse-out enable.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @param[in] state: pulse-out enable
 *            - true
 *            - false
 * @return none
 */
static inline void PDT_SetCmpPulseOutEnable(PDT_Type * const base, bool state)
{
    MODIFY_REG32(base->POEN, PDT_POEN_POEN_Msk, PDT_POEN_POEN_Pos, state);
}

/*!
 * @brief Set PDT pulse-out delay value for goes high.
 *
 * @param[in] base: PDT module
 *            - PDT0
 *            - PDT1
 * @param[in] value: delay value for goes high
 * @return none
 */
static inline void PDT_SetCmpPulseOutDelayForHigh(PDT_Type *base, uint16_t value)
{
    base->PULSE.ACCESS16BIT.DLY1 = value;
}

/*!
 * @brief Set PDT pulse-out delay value for goes low.
 *
 * @param[in] base: PDT module
 *            - PDT0
 *            - PDT1
 * @param[in] value: delay value for goes low
 * @return none
 */
static inline void PDT_SetCmpPulseOutDelayForLow(PDT_Type *base, uint16_t value)
{
   base->PULSE.ACCESS16BIT.DLY2 = value;
}

/*!
 * @brief Get timer value.
 *
 * @param[in] base: the PDT base address
 *            - PDT0
 *            - PDT1
 * @return timer counting value
 */
#define PDT_GetTimerValue(base)         (READ_BIT32((base)->CNT, PDT_CNT_CNT_Msk) >> PDT_CNT_CNT_Pos)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PDT_HW_H */

/* =============================================  EOF  ============================================== */
