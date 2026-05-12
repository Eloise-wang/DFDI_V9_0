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
 * @file acmp_hw.h
 *
 * @brief Analog comparator module access register inline function definition.
 *
 */

#ifndef _ACMP_HW_H
#define _ACMP_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "acmp_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Set ACMP polling mode.
 *
 * @param[in] base: acmp module base
 * @param[in] polling: polling mode
 *                      - ACMP_NONE_POLLING
 *                      - ACMP_POSITIVE_POLLING
 *                      - ACMP_NEGATIVE_POLLING
 * @return none
 */
void ACMP_SetPollingMode(ACMP_Type *base, const acmp_input_polling_t polling);

/*!
 * @brief Get ACMP polling mode.
 *
 * @param[in] base: acmp module base
 * @return current polling mode
 *                      - ACMP_NONE_POLLING
 *                      - ACMP_POSITIVE_POLLING
 *                      - ACMP_NEGATIVE_POLLING
 */
acmp_input_polling_t ACMP_GetPollingMode(const ACMP_Type *base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set ACMP output pin enable flag.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable output pin function
 * @return none
 */
static inline void ACMP_SetOutputPinEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_OUTEN_Msk, ACMP_CR0_OUTEN_Pos, state);
}

/*!
 * @brief Get ACMP output pin enable flag.
 *
 * @param[in] base: acmp module base
 * @return whether output pin function is enabled
 */
static inline bool ACMP_GetOutputPinEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_OUTEN_Msk) >> ACMP_CR0_OUTEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP output signal selection.
 *
 * @param[in] base: acmp module base
 * @param[in] select: which signal to output
 *                  - ACMP_COUT
 *                  - ACMP_COUTA
 * @return none
 */
static inline void ACMP_SetOutputSelection(ACMP_Type *base, acmp_output_select_t select)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_COS_Msk, ACMP_CR0_COS_Pos, select);
}

/*!
 * @brief Get ACMP output signal selection.
 *
 * @param[in] base: acmp module base
 * @return which output signal is selected
 *                  - ACMP_COUT
 *                  - ACMP_COUTA
 */
static inline acmp_output_select_t ACMP_GetOutputSelection(const ACMP_Type *base)
{
    return (acmp_output_select_t)(READ_BIT32(base->CR0, ACMP_CR0_COS_Msk) >> ACMP_CR0_COS_Pos);
}

/*!
 * @brief Set ACMP analog module power enable flag.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable acmp analog module
 * @return none
 */
static inline void ACMP_SetEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_EN_Msk, ACMP_CR0_EN_Pos, state);
}

/*!
 * @brief Get ACMP analog module power enable flag.
 *
 * @param[in] base: acmp module base
 * @return whether acmp analog module is enabled
 */
static inline bool ACMP_GetEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_EN_Msk) >> ACMP_CR0_EN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP interrupt enable flag
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable interrupt function
 * @return none
 */
static inline void ACMP_SetInterruptEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_IE_Msk, ACMP_CR0_IE_Pos, state);
}

/*!
 * @brief Get ACMP interrupt enable flag
 *
 * @param[in] base: acmp module base
 * @return whether acmp interrupt function is enabled
 */
static inline bool ACMP_GetInterruptEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_IE_Msk) >> ACMP_CR0_IE_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP trigger mode.
 *
 * @param[in] base: acmp module base
 * @param[in] mode: trigger mode
 * @return none
 */
static inline void ACMP_SetTriggerMode(ACMP_Type *base, acmp_output_trigger_t mode)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_MOD_Msk, ACMP_CR0_MOD_Pos, mode);
}

/*!
 * @brief Get ACMP trigger mode.
 *
 * @param[in] base: acmp module base
 * @return acmp current trigger mode
 */
static inline acmp_output_trigger_t ACMP_GetTriggerMode(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_MOD_Msk) >> ACMP_CR0_MOD_Pos;
    if (2U == tmp)
    {
        tmp = (uint32_t)ACMP_FALLING_EDGE;
    }
    return (acmp_output_trigger_t)tmp;
}

/*!
 * @brief Set ACMP using LSI clock as function clock enable flag.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable ACMP using LSI clock as function clock
 * @return none
 */
static inline void ACMP_SetUsingLSIEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_LSICLK_EN_Msk, ACMP_CR0_LSICLK_EN_Pos, state);
}

/*!
 * @brief Get ACMP using LSI clock as function clock enable flag.
 *
 * @param[in] base: acmp module base
 * @return whether ACMP using LSI clock as function clock
 */
static inline bool ACMP_GetUsingLSIEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_LSICLK_EN_Msk) >> ACMP_CR0_LSICLK_EN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP positive input channel selection.
 *
 * @param[in] base: acmp module base
 * @param[in] channel: channel that need connect to the positive input
 * @return none
 */
static inline void ACMP_SetPositiveInputPin(ACMP_Type *base, acmp_ch_number_t channel)
{
    MODIFY_REG32(base->CR1, ACMP_CR1_PSEL_Msk, ACMP_CR1_PSEL_Pos, channel);
}

/*!
 * @brief Get ACMP positive input channel selection.
 *
 * @param[in] base: acmp module base
 * @return current channel that connect to the positive input.
 */
static inline acmp_ch_number_t ACMP_GetPositiveInputPin(const ACMP_Type *base)
{
    return (acmp_ch_number_t)(READ_BIT32(base->CR1, ACMP_CR1_PSEL_Msk) >> ACMP_CR1_PSEL_Pos);
}

/*!
 * @brief Set ACMP negative input channel selection.
 *
 * @param[in] base: acmp module base
 * @param[in] channel: channel that need connect to the negative input
 * @return none
 */
static inline void ACMP_SetNegativeInputPin(ACMP_Type *base, acmp_ch_number_t channel)
{
    MODIFY_REG32(base->CR1, ACMP_CR1_NSEL_Msk, ACMP_CR1_NSEL_Pos, channel);
}

/*!
 * @brief Get ACMP negative input channel selection.
 *
 * @param[in] base: acmp module base
 * @return current channel that connect to the negative input.
 */
static inline acmp_ch_number_t ACMP_GetNegativeInputPin(const ACMP_Type *base)
{
    return (acmp_ch_number_t)(READ_BIT32(base->CR1, ACMP_CR1_NSEL_Msk) >> ACMP_CR1_NSEL_Pos);
}

/*!
 * @brief Set ACMP hysteresis mode.
 *
 * @param[in] base: acmp module base
 * @param[in] mode: hysteresis mode
 * @return none
 */
static inline void ACMP_SetHysteresisMode(ACMP_Type *base, acmp_hysteresis_mode_t mode)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_HYSTMOD_Msk, ACMP_CR0_HYSTMOD_Pos, mode);
}

/*!
 * @brief Get ACMP hysteresis mode.
 *
 * @param[in] base: acmp module base
 * @return current hysteresis mode setting
 */
static inline acmp_hysteresis_mode_t ACMP_GetHysteresisMode(const ACMP_Type *base)
{
    return (acmp_hysteresis_mode_t)(READ_BIT32(base->CR0, ACMP_CR0_HYSTMOD_Msk) >> ACMP_CR0_HYSTMOD_Pos);
}

/*!
 * @brief Set ACMP hysteresis voltage type.
 *
 * @param[in] base: acmp module base
 * @param[in] mV: hysteresis voltage type
 * @return none
 */
static inline void ACMP_SetHysteresisVoltage(ACMP_Type *base, acmp_hysteresis_t mV)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_HYSTVOL_Msk, ACMP_CR0_HYSTVOL_Pos, mV);
}

/*!
 * @brief Get ACMP hysteresis voltage type.
 *
 * @param[in] base: acmp module base
 * @return current hysteresis voltage type
 */
static inline acmp_hysteresis_t ACMP_GetHysteresisVoltage(const ACMP_Type *base)
{
    return (acmp_hysteresis_t)(READ_BIT32(base->CR0, ACMP_CR0_HYSTVOL_Msk) >> ACMP_CR0_HYSTVOL_Pos);
}

/*!
 * @brief Set low pass filter bandwidth type.
 *
 * @param[in] base: acmp module base
 * @param[in] freq: low pass filter bandwidth
 * @return none
 */
static inline void ACMP_SetLowPassFilter(ACMP_Type *base, acmp_low_pass_filter_t freq)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_LPF_Msk, ACMP_CR0_LPF_Pos, freq);
}

/*!
 * @brief Get low pass filter bandwidth type.
 *
 * @param[in] base: acmp module base
 * @return low pass filter bandwidth
 */
static inline acmp_low_pass_filter_t ACMP_GetLowPassFilter(const ACMP_Type *base)
{
    return (acmp_low_pass_filter_t)(READ_BIT32(base->CR0, ACMP_CR0_LPF_Msk) >> ACMP_CR0_LPF_Pos);
}

/*!
 * @brief Set ACMP digital filter enable flag.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable digital filter
 * @return none
 */
static inline void ACMP_SetFilterEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_FILTEN_Msk, ACMP_CR0_FILTEN_Pos, state);
}

/*!
 * @brief Get ACMP digital filter enable flag.
 *
 * @param[in] base: acmp module base
 * @return whether digital filter is enabled
 */
static inline bool ACMP_GetFilterEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_FILTEN_Msk) >> ACMP_CR0_FILTEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set digital filter clock prescaler.
 *
 * @param[in] base: acmp module base
 * @param[in] psc: digital filter prescaler
 * @return none
 */
static inline void ACMP_SetFilterClockPrescaler(ACMP_Type *base, acmp_filter_divide_t psc)
{
    MODIFY_REG32(base->CLK, ACMP_CLK_PSC_Msk, ACMP_CLK_PSC_Pos, psc);
}

/*!
 * @brief Get digital filter clock prescaler.
 *
 * @param[in] base: acmp module base
 * @return current digital filter prescaler setting
 */
static inline acmp_filter_divide_t ACMP_GetFilterClockPrescaler(const ACMP_Type *base)
{
    return (acmp_filter_divide_t)(READ_BIT32(base->CLK, ACMP_CLK_PSC_Msk) >> ACMP_CLK_PSC_Pos);
}

/*!
 * @brief Set digital filter length.
 *
 * @param[in] base: acmp module base
 * @param[in] filterCnt: filter count setting
 * @return none
 */
static inline void ACMP_SetFilterLength(ACMP_Type *base, uint8_t filterCnt)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_FILTER_CNT_Msk, ACMP_CR0_FILTER_CNT_Pos, filterCnt);
}

/*!
 * @brief Get digital filter length.
 *
 * @param[in] base: acmp module base
 * @return current filter count setting
 */
static inline uint8_t ACMP_GetFilterLength(const ACMP_Type *base)
{
    return (uint8_t)(READ_BIT32(base->CR0, ACMP_CR0_FILTER_CNT_Msk) >> ACMP_CR0_FILTER_CNT_Pos);
}

/*!
 * @brief Set ACMP window mode enable setting.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable window mode
 * @return none
 */
static inline void ACMP_SetWindowMode(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_WINEN_Msk, ACMP_CR0_WINEN_Pos, state);
}

/*!
 * @brief Get ACMP window mode enable setting.
 *
 * @param[in] base: acmp module base
 * @return whether window mode is enabled
 */
static inline bool ACMP_GetWindowMode(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_WINEN_Msk) >> ACMP_CR0_WINEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP result invertion.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable result invertion
 * @return none
 */
static inline void ACMP_SetInvertMode(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_INVT_Msk, ACMP_CR0_INVT_Pos, state);
}

/*!
 * @brief Get ACMP result invertion.
 *
 * @param[in] base: acmp module base
 * @return whether result invertion is enabled
 */
static inline bool ACMP_GetInvertMode(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_INVT_Msk) >> ACMP_CR0_INVT_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP hall output enable flag.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable hall output enable flag
 * @return none
 */
static inline void ACMP_SetHallOutputEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR0, ACMP_CR0_OPE_Msk, ACMP_CR0_OPE_Pos, state);
}

/*!
 * @brief Get ACMP hall output enable flag.
 *
 * @param[in] base: acmp module base
 * @return whether hall output enable flag is enabled
 */
static inline bool ACMP_GetHallOutputEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR0, ACMP_CR0_OPE_Msk) >> ACMP_CR0_OPE_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP hall output A channel.
 *
 * @param[in] base: acmp module base
 * @param[in] hallCh: hall channel
 * @return none
 */
static inline void ACMP_SetHallOutputA(ACMP_Type *base, acmp_ch_number_t hallCh)
{
    MODIFY_REG32(base->OPA, ACMP_OPA_OPASEL_Msk, ACMP_OPA_OPASEL_Pos, hallCh);
}

/*!
 * @brief Get ACMP hall output A channel.
 *
 * @param[in] base: acmp module base
 * @return input channel of hall output
 */
static inline acmp_ch_number_t ACMP_GetHallOutputA(const ACMP_Type *base)
{
    return (acmp_ch_number_t)(READ_BIT32(base->OPA, ACMP_OPA_OPASEL_Msk) >> ACMP_OPA_OPASEL_Pos);
}

/*!
 * @brief Set ACMP hall output B channel.
 *
 * @param[in] base: acmp module base
 * @param[in] hallCh: hall channel
 * @return none
 */
static inline void ACMP_SetHallOutputB(ACMP_Type *base, acmp_ch_number_t hallCh)
{
    MODIFY_REG32(base->OPB, ACMP_OPB_OPBSEL_Msk, ACMP_OPB_OPBSEL_Pos, hallCh);
}

/*!
 * @brief Get ACMP hall output B channel.
 *
 * @param[in] base: acmp module base
 * @return input channel of hall output
 */
static inline acmp_ch_number_t ACMP_GetHallOutputB(const ACMP_Type *base)
{
    return (acmp_ch_number_t)(READ_BIT32(base->OPB, ACMP_OPB_OPBSEL_Msk) >> ACMP_OPB_OPBSEL_Pos);
}

/*!
 * @brief Set ACMP hall output C channel.
 *
 * @param[in] base: acmp module base
 * @param[in] hallCh: hall channel
 * @return none
 */
static inline void ACMP_SetHallOutputC(ACMP_Type *base, acmp_ch_number_t hallCh)
{
    MODIFY_REG32(base->OPC, ACMP_OPC_OPCSEL_Msk, ACMP_OPC_OPCSEL_Pos, hallCh);
}

/*!
 * @brief Get ACMP hall output C channel.
 *
 * @param[in] base: acmp module base
 * @return input channel of hall output
 */
static inline acmp_ch_number_t ACMP_GetHallOutputC(const ACMP_Type *base)
{
    return (acmp_ch_number_t)(READ_BIT32(base->OPC, ACMP_OPC_OPCSEL_Msk) >> ACMP_OPC_OPCSEL_Pos);
}

/*!
 * @brief Set ACMP DAC module enable flag.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable DAC module
 * @return none
 */
static inline void ACMP_SetDACEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR2, ACMP_CR2_DACEN_Msk, ACMP_CR2_DACEN_Pos, state);
}

/*!
 * @brief Get ACMP DAC module enable flag.
 *
 * @param[in] base: acmp module base
 * @return whether DAC module is enabled
 */
static inline bool ACMP_GetDACEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR2, ACMP_CR2_DACEN_Msk) >> ACMP_CR2_DACEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP DAC output code value.
 *
 * @param[in] base: acmp module base
 * @param[in] value: DAC output code value
 * @return none
 */
static inline void ACMP_SetDACOutput(ACMP_Type *base, uint8_t value)
{
    MODIFY_REG32(base->CR2, ACMP_CR2_DACVAL_Msk, ACMP_CR2_DACVAL_Pos, value);
}

/*!
 * @brief Get ACMP DAC output code value.
 *
 * @param[in] base: acmp module base
 * @return current DAC output code value
 */
static inline uint8_t ACMP_GetDACOutput(const ACMP_Type *base)
{
    return (uint8_t)(READ_BIT32(base->CR2, ACMP_CR2_DACVAL_Msk) >> ACMP_CR2_DACVAL_Pos);
}

/*!
 * @brief Set ACMP DAC reference source.
 *
 * @param[in] base: acmp module base
 * @param[in] ref: reference source
 * @return none
 */
static inline void ACMP_SetDACReference(ACMP_Type *base, acmp_voltage_reference_t ref)
{
    MODIFY_REG32(base->CR2, ACMP_CR2_DACREF_Msk, ACMP_CR2_DACREF_Pos, ref);
}

/*!
 * @brief Get ACMP DAC reference source.
 *
 * @param[in] base: acmp module base
 * @return DAC reference source
 */
static inline acmp_voltage_reference_t ACMP_GetDACReference(const ACMP_Type *base)
{
    return (acmp_voltage_reference_t)(READ_BIT32(base->CR2, ACMP_CR2_DACREF_Msk) >> ACMP_CR2_DACREF_Pos);
}

/*!
 * @brief Set ACMP positive polling enable flag. Should not enable positive polling and negative polling.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable positive polling
 * @return none
 */
static inline void ACMP_SetPositivePollingEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR3, ACMP_CR3_PSPLEN_Msk, ACMP_CR3_PSPLEN_Pos, state);
}

/*!
 * @brief Get ACMP positive polling enable flag.
 *
 * @param[in] base: acmp module base
 * @return whether positive polling is enabled
 */
static inline bool ACMP_GetPositivePollingEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR3, ACMP_CR3_PSPLEN_Msk) >> ACMP_CR3_PSPLEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP negative polling enable flag. Should not enable positive polling and negative polling.
 *
 * @param[in] base: acmp module base
 * @param[in] state: enable or disable negative polling
 * @return none
 */
static inline void ACMP_SetNegativePollingEnableFlag(ACMP_Type *base, bool state)
{
    MODIFY_REG32(base->CR3, ACMP_CR3_NSPLEN_Msk, ACMP_CR3_NSPLEN_Pos, state);
}

/*!
 * @brief Get ACMP negative polling enable flag.
 *
 * @param[in] base: acmp module base
 * @return whether negative polling is enabled
 */
static inline bool ACMP_GetNegativePollingEnableFlag(const ACMP_Type *base)
{
    uint32_t tmp = READ_BIT32(base->CR3, ACMP_CR3_NSPLEN_Msk) >> ACMP_CR3_NSPLEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ACMP polling sequence
 *
 * @param[in] base: acmp module base
 * @param[in] seq: channel sequence that need to poll, each bit corresponding to one channel
 *                  bit 0 represent channel 0 and other bit is likewise.
 * @return none
 */
static inline void ACMP_SetPollingSequence(ACMP_Type *base, uint16_t seq)
{
    MODIFY_REG32(base->CR4, ACMP_CR4_PLSEQ_Msk, ACMP_CR4_PLSEQ_Pos, seq);
}

/*!
 * @brief Get ACMP polling sequence
 *
 * @param[in] base: acmp module base
 * @return current polling channel sequence
 */
static inline uint16_t ACMP_GetPollingSequence(const ACMP_Type *base)
{
    return (uint16_t)(READ_BIT32(base->CR4, ACMP_CR4_PLSEQ_Msk) >> ACMP_CR4_PLSEQ_Pos);
}

/*!
 * @brief Set ACMP polling clock divide.
 *
 * @param[in] base: acmp module base
 * @param[in] div: polling clock divide
 * @return none
 */
static inline void ACMP_SetPollingFreqDiv(ACMP_Type *base, acmp_polling_clk_divide_t div)
{
    MODIFY_REG32(base->FD, ACMP_FD_PLFD_Msk, ACMP_FD_PLFD_Pos, div);
}

/*!
 * @brief Get ACMP polling clock divide.
 *
 * @param[in] base: acmp module base
 * @return current polling clock divide setting
 */
static inline acmp_polling_clk_divide_t ACMP_GetPollingFreqDiv(const ACMP_Type *base)
{
    return (acmp_polling_clk_divide_t)(READ_BIT32(base->FD, ACMP_FD_PLFD_Msk) >> ACMP_FD_PLFD_Pos);
}

/*!
 * @brief Get ACMP compare result in normal compare mode.
 *
 * @param[in] base: acmp module base
 * @return compare result
 */
static inline uint8_t ACMP_GetOutputData(const ACMP_Type *base)
{
    return (uint8_t)(READ_BIT32(base->DR, ACMP_DR_O_Msk) >> ACMP_DR_O_Pos);
}

/*!
 * @brief Get ACMP polling result.
 *
 * @param[in] base: acmp module base
 * @return polling channel result. Bits of Channels that not polling will be 0
 */
static inline uint16_t ACMP_GetPollingData(const ACMP_Type *base)
{
    return (uint16_t)READ_BIT32(base->DR, ACMP_DR_POLLING_O_Msk);
}

/*!
 * @brief Get ACMP normal comparator mode interrupt status.
 *
 * @param[in] base: acmp module base
 * @return normal mode interrupt status
 */
static inline uint8_t ACMP_GetOutputStatus(const ACMP_Type *base)
{
    return (uint8_t)(READ_BIT32(base->SR, ACMP_SR_F_Msk) >> ACMP_SR_F_Pos);
}

/*!
 * @brief Clear ACMP normal comparator mode interrupt status.
 *
 * @param[in] base: acmp module base
 * @return none
 */
static inline void ACMP_ClearOutputStatus(ACMP_Type *base)
{
    WRITE_REG32(base->SR, ACMP_SR_F_Msk);
}

/*!
 * @brief Get ACMP polling mode interrupt status.
 *
 * @param[in] base: acmp module base
 * @return polling mode interrupt status
 */
static inline uint16_t ACMP_GetPollingStatus(const ACMP_Type *base)
{
    return (uint16_t)READ_BIT32(base->SR, ACMP_SR_POLLING_F_Msk);
}

/*!
 * @brief Clear ACMP polling mode interrupt status.
 *
 * @param[in] base: acmp module base
 * @return none
 */
static inline void ACMP_ClearPollingStatus(ACMP_Type *base)
{
    WRITE_REG32(base->SR, ACMP_SR_POLLING_F_Msk);
}

/*!
 * @brief Config ACMP lowpower mode enable flag
 *
 * @param[in] state: enable or disable DAC module
 */
static inline void ACMP_SetLPEnableFlag(bool state)
{
    MODIFY_REG32(ANA->ACMPDAC_CFG, ANA_ACMPDAC_CFG_RG_MCU_ACMPDAC_LP_EN_Msk, ANA_ACMPDAC_CFG_RG_MCU_ACMPDAC_LP_EN_Pos, state);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ACMP_HW_H */

/* =============================================  EOF  ============================================== */
