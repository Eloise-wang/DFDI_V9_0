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
 * @file adc_hw.h
 *
 * @brief Analog to digital converter module access register inline function definition.
 *
 */

#ifndef _ADC_HW_H
#define _ADC_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "adc_drv.h"

/* ============================================  Define  ============================================ */
#define ADC_EACH_REG_SPT_NUM    (10UL)      /*!< ADC sampel timer setting number in each SPT register */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Set GE OE calibration voltage input signal enable flag.
 *
 * @param[in] instance: adc instance number
 * @param[in] state: enable or disable calibration voltage input
 * @return none
 */
void ADC_SetGEOEVINEnableFlag(const uint32_t instance, const bool state);

/*!
 * @brief Get GE OE calibration voltage input signal enable flag.
 *
 * @param[in] instance: adc instance number
 * @return whether calibration voltage input is enabled
 */
bool ADC_GetGEOEVINEnableFlag(const uint32_t instance);

/*!
 * @brief Set GE OE calibration input signal.
 *
 * @param[in] instance: adc instance number
 * @param[in] state: calibration input signal
 * @return none
 */
void ADC_SetGEOEVIN(const uint32_t instance, const ana_geoecal_vin_t vin);

/*!
 * @brief Get GE OE calibration input signal.
 *
 * @param[in] instance: adc instance number
 * @return calibration input signal
 */
ana_geoecal_vin_t ADC_GetGEOEVIN(const uint32_t instance);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set adc clock prescaler.
 *
 * @param[in] base: adc module
 *                  - 0: ADC0
 *                  - 1: ADC1
 * @param[in] psc: prescaler value
 *                  - ADC_CLK_DIVIDE_1
 *                  - ADC_CLK_DIVIDE_2
 *                  - ADC_CLK_DIVIDE_3
 *                  - ADC_CLK_DIVIDE_4
 *                  - ADC_CLK_DIVIDE_5
 *                  - ADC_CLK_DIVIDE_6
 *                  - ADC_CLK_DIVIDE_7
 *                  - ADC_CLK_DIVIDE_8
 *                  - ADC_CLK_DIVIDE_9
 *                  - ADC_CLK_DIVIDE_10
 *                  - ADC_CLK_DIVIDE_11
 *                  - ADC_CLK_DIVIDE_12
 *                  - ADC_CLK_DIVIDE_13
 *                  - ADC_CLK_DIVIDE_14
 *                  - ADC_CLK_DIVIDE_15
 *                  - ADC_CLK_DIVIDE_16
 * @return none
 */
static inline void ADC_SetClockPrescaler(ADC_Type * const base, const adc_clk_divide_t psc)
{
    MODIFY_REG32(base->CTRL1, ADC_CTRL1_PSC_Msk, ADC_CTRL1_PSC_Pos, psc);
}

/*!
 * @brief Get ADC clock prescaler setting.
 *
 * @param[in] base: adc module
 *                  - 0: ADC0
 *                  - 1: ADC1
 * @return prescaler value:
 *                  - ADC_CLK_DIVIDE_1
 *                  - ADC_CLK_DIVIDE_2
 *                  - ADC_CLK_DIVIDE_3
 *                  - ADC_CLK_DIVIDE_4
 *                  - ADC_CLK_DIVIDE_5
 *                  - ADC_CLK_DIVIDE_6
 *                  - ADC_CLK_DIVIDE_7
 *                  - ADC_CLK_DIVIDE_8
 *                  - ADC_CLK_DIVIDE_9
 *                  - ADC_CLK_DIVIDE_10
 *                  - ADC_CLK_DIVIDE_11
 *                  - ADC_CLK_DIVIDE_12
 *                  - ADC_CLK_DIVIDE_13
 *                  - ADC_CLK_DIVIDE_14
 *                  - ADC_CLK_DIVIDE_15
 *                  - ADC_CLK_DIVIDE_16
 */
static inline adc_clk_divide_t ADC_GetClockPrescaler(const ADC_Type * const base)
{
    return (adc_clk_divide_t)(READ_BIT32(base->CTRL1, ADC_CTRL1_PSC_Msk) >> ADC_CTRL1_PSC_Pos);
}

/*!
 * @brief Set adc resolution.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] resolution: resolution setting
 *                  - ADC_RESOLUTION_12BIT
 *                  - ADC_RESOLUTION_10BIT
 *                  - ADC_RESOLUTION_8BIT
 * @return none
 */
static inline void ADC_SetResolution(ADC_Type * const base, const adc_resolution_t resolution)
{
    MODIFY_REG32(base->CTRL1, ADC_CTRL1_RESL_Msk, ADC_CTRL1_RESL_Pos, resolution);
}

/*!
 * @brief Get adc resution setting.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return adc resution setting
 */
static inline adc_resolution_t ADC_GetResolution(const ADC_Type * const base)
{
    return (adc_resolution_t)(READ_BIT32(base->CTRL1, ADC_CTRL1_RESL_Msk) >> ADC_CTRL1_RESL_Pos);
}

/*!
 * @brief Set adc voltage reference source.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] voltageRef: voltage reference source
 *                  - ADC_VOLTAGEREF_VREF
 *                  - ADC_VOLTAGEREF_VDDA
 * @return none
 */
static inline void ADC_SetVoltageReference(ADC_Type * const base, const adc_voltage_reference_t voltageRef)
{
    MODIFY_REG32(base->SQL, ADC_SQL_REFSEL_Msk, ADC_SQL_REFSEL_Pos, voltageRef);
}

/*!
 * @brief Get adc voltage reference source setting.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return  voltage reference source setting
 *                  - ADC_VOLTAGEREF_VREF
 *                  - ADC_VOLTAGEREF_VDDA
 */
static inline adc_voltage_reference_t ADC_GetVoltageReference(const ADC_Type * const base)
{
    return (adc_voltage_reference_t)(READ_BIT32(base->SQL, ADC_SQL_REFSEL_Msk) >> ADC_SQL_REFSEL_Pos);
}

/*!
 * @brief Set adc power mode.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] mode: enable or disable ADC power
 * @return none
 */
static inline void ADC_SetPowerEnableFlag(ADC_Type * const base, const bool mode)
{
    MODIFY_REG32(base->CTRL1, ADC_CTRL1_ADON_Msk, ADC_CTRL1_ADON_Pos,
            mode ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get ADC power setting.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether adc power is enabled
 */
static inline bool ADC_GetPowerEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL1, ADC_CTRL1_ADON_Msk) >> ADC_CTRL1_ADON_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set scan convert mode enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable scan convert mode
 * @return none
 */
static inline void ADC_SetScanEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_SCAN_Msk, ADC_CTRL0_SCAN_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get scan convert mode setting.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether scan convert mode is enabled
 */
static inline bool ADC_GetScanEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_SCAN_Msk) >> ADC_CTRL0_SCAN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set ADC interval mode.
 *
 * Can only be configured in mode3/5, other modes do not work.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable scan convert mode
 * @return none
 */
static inline void ADC_SetIntervalEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_INTERVAL_Msk, ADC_CTRL0_INTERVAL_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get ADC interval mode.
 *
 * Interval mode can only be configured in mode3/5, other modes do not work.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether interval convert mode is enabled
 */
static inline bool ADC_GetIntervalEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_INTERVAL_Msk) >> ADC_CTRL0_INTERVAL_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set continuous convert mode.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable continuous convert mode
 * @return none
 */
static inline void ADC_SetContinuousEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_CONT_Msk, ADC_CTRL0_CONT_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get continuous convert mode enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether continuous convert mode is enabled.
 */
static inline bool ADC_GetContinuousEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_CONT_Msk) >> ADC_CTRL0_CONT_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set discontinuous mode enable flag of regular group.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable discontinuous convert mode
 * @return none
 */
static inline void ADC_SetRegularDiscontinuousEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_DISCEN_Msk, ADC_CTRL0_DISCEN_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get discontinuous mode enable flag of regular group.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether continuous convert mode is enabled.
 */
static inline bool ADC_GetRegularDiscontinuousEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_DISCEN_Msk) >> ADC_CTRL0_DISCEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set discontinuous mode number for regular group.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] num: discontinuous convert number for regular group
 * @return none
 */
static inline void ADC_SetRegularDiscontinuousNum(ADC_Type * const base, const uint8_t num)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_DISCNUM_Msk, ADC_CTRL0_DISCNUM_Pos, num);
}

/*!
 * @brief Get discontinuous mode number of regular group.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return discontinuous convert number for regular group
 */
static inline uint8_t ADC_GetRegularDiscontinuousNum(const ADC_Type * const base)
{
    return READ_BIT32(base->CTRL0, ADC_CTRL0_DISCNUM_Msk) >> ADC_CTRL0_DISCNUM_Pos;
}

/*!
 * @brief Set ADC automatic inject injection group convert after regular group
 *          function enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable automatic inject convert mode
 * @return none
 */
static inline void ADC_SetInjectAutoEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_IAUTO_Msk, ADC_CTRL0_IAUTO_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get ADC automatic injected group convert enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether automatic inject convert mode is enabled.
 */
static inline bool ADC_GetInjectAutoEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_IAUTO_Msk) >> ADC_CTRL0_IAUTO_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set discontinuous mode for injection group.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable injecttion group discontinuous convert mode
 * @return none
 */
static inline void ADC_SetInjectDiscontinuousEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_IDISCEN_Msk, ADC_CTRL0_IDISCEN_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get discontinuous mode for injection group.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether injection group discontinuous convert mode is enabled.
 */
static inline bool ADC_GetInjectDiscontinuousEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_IDISCEN_Msk) >> ADC_CTRL0_IDISCEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set regular group trigger source.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] source: trigger source type
 *                  - ADC_TRIGGER_INTERNAL
 *                  - ADC_TRIGGER_EXTERNAL
 * @return none
 */
static inline void ADC_SetRegularTriggerSource(ADC_Type * const base, const adc_trigger_t source)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_EXTTRIG_Msk, ADC_CTRL0_EXTTRIG_Pos, source);
}

/*!
 * @brief Get regular group trigger source.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return regular group trigger source type
 *                  - ADC_TRIGGER_INTERNAL
 *                  - ADC_TRIGGER_EXTERNAL
 */
static inline adc_trigger_t ADC_GetRegularTriggerSource(const ADC_Type * const base)
{
    return (adc_trigger_t)(READ_BIT32(base->CTRL0, ADC_CTRL0_EXTTRIG_Msk) >> ADC_CTRL0_EXTTRIG_Pos);
}

/*!
 * @brief Start software trigger for regular group convert.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return none
 */
static inline void ADC_SoftwareStartRegularConvert(ADC_Type * const base)
{
    SET_BIT32(base->CTRL0, ADC_CTRL0_SWSTART_Msk);
}

/*!
 * @brief Set injected group trigger source.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] source: trigger source type
 *                  - ADC_TRIGGER_INTERNAL
 *                  - ADC_TRIGGER_EXTERNAL
 * @return none
 */
static inline void ADC_SetInjectTriggerSource(ADC_Type * const base, const adc_trigger_t source)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_IEXTTRIG_Msk, ADC_CTRL0_IEXTTRIG_Pos, source);
}

/*!
 * @brief Get injection group trigger source.
 *
 * @param[in] base:adc module
 *                  - ADC0
 *                  - ADC1
 * @return injection group trigger source
 *                  - ADC_TRIGGER_INTERNAL
 *                  - ADC_TRIGGER_EXTERNAL
 */
static inline adc_trigger_t ADC_GetInjectTriggerSource(const ADC_Type * const base)
{
    return (adc_trigger_t)(READ_BIT32(base->CTRL0, ADC_CTRL0_IEXTTRIG_Msk) >> ADC_CTRL0_IEXTTRIG_Pos);
}

/*!
 * @brief Start software trigger for injected group convert.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return none
 */
static inline void ADC_SoftwareStartInjectConvert(ADC_Type * const base)
{
    SET_BIT32(base->CTRL0, ADC_CTRL0_ISWSTART_Msk);
}

/*!
 * @brief Set injection group offset value.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] num: injection group index, should be less than ADC_INJECT_SEQUENCE_LEGNTH
 * @param[in] offset: offset value, which will be subtracted after injection
 *                  group index convertion complete.
 * @return none
 */
static inline void ADC_SetInjectOffset(ADC_Type * const base, const uint8_t num, const uint16_t offset)
{
    MODIFY_REG32(base->IOFR[num], ADC_IOFR_IOFR_Msk, ADC_IOFR_IOFR_Pos, offset);
}

/*!
 * @brief Get injection group offset value.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] num: injection group index, should be less than ADC_INJECT_SEQUENCE_LEGNTH
 * @return offset value
 */
static inline uint16_t ADC_GetInjectOffset(const ADC_Type * const base, const uint8_t num)
{
    return (uint16_t)(READ_BIT32(base->IOFR[num], ADC_IOFR_IOFR_Msk) >> ADC_IOFR_IOFR_Pos);
}

/*!
 * @brief Set regualr group convertion length.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] length: regular group length
 * @return none
 */
static inline void ADC_SetRegularLength(ADC_Type * const base, const uint8_t length)
{
    MODIFY_REG32(base->SQL, ADC_SQL_RSQL_Msk, ADC_SQL_RSQL_Pos, length > 0U ? length - 1U : 0U);
}

/*!
 * @brief Get regualr group convertion length.
 *
 * @param[in] base:adc module
 *                  - ADC0
 *                  - ADC1
 * @return regualr gropu length setting
 */
static inline uint8_t ADC_GetRegularLength(const ADC_Type * const base)
{
    return (uint8_t)((READ_BIT32(base->SQL, ADC_SQL_RSQL_Msk) >> ADC_SQL_RSQL_Pos) + 1U);
}

/*!
 * @brief Set injection group convertion length.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] length: injection group length
 * @return none
 */
static inline void ADC_SetInjectLength(ADC_Type * const base, const uint8_t length)
{
    MODIFY_REG32(base->SQL, ADC_SQL_ISQL_Msk, ADC_SQL_ISQL_Pos, length > 0U ? length - 1U : 0U);
}

/*!
 * @brief Get injection group convertion length.
 *
 * @param[in] base:adc module
 *                  - ADC0
 *                  - ADC1
 * @return injection group length setting
 */
static inline uint8_t ADC_GetInjectLength(const ADC_Type * const base)
{
    return (uint8_t)((READ_BIT32(base->SQL, ADC_SQL_ISQL_Msk) >> ADC_SQL_ISQL_Pos) + 1U);
}

/*!
 * @brief Set channel sample time.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] channel: adc channel
 * @param[in] time: sample time
 *                  - ADC_SPT_CLK_5
 *                  - ADC_SPT_CLK_10
 *                  - ADC_SPT_CLK_15
 *                  - ADC_SPT_CLK_23
 *                  - ADC_SPT_CLK_35
 *                  - ADC_SPT_CLK_45
 *                  - ADC_SPT_CLK_85
 *                  - ADC_SPT_CLK_185
 * @return none
 */
static inline void ADC_SetChannelSampleTime(ADC_Type * const base,
        const adc_inputchannel_t channel, const adc_sample_time_t time)
{
    uint8_t index = 0U, pos = 0U;

    DEVICE_ASSERT(ADC_CH_MAX > channel);

    index = (uint8_t)channel / ADC_EACH_REG_SPT_NUM;
    pos = (uint8_t)channel % ADC_EACH_REG_SPT_NUM * ADC_SPT_Width;
    MODIFY_REG32((base->SPT[index]), ADC_SPT_Msk << pos, pos, time);
}

/*!
 * @brief Get channel sample time.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] channel: adc channel
 * @return sample time
 */
static inline adc_sample_time_t ADC_GetChannelSampleTime(const ADC_Type * const base, const adc_inputchannel_t channel)
{
    DEVICE_ASSERT(ADC_CH_MAX > channel);

    uint8_t index = 0U, pos = 0U;

    index = (uint8_t)channel / ADC_EACH_REG_SPT_NUM;
    pos = (uint8_t)channel % ADC_EACH_REG_SPT_NUM * ADC_SPT_Width;

    return (adc_sample_time_t)(READ_BIT32((base->SPT[index]), ADC_SPT_Msk << pos) >> pos);
}

/*!
 * @brief Set internal supply channel.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] channel: adc channel
 * @return none
 */
static inline void ADC_SetSupplyChannel(ADC_Type * const base, const adc_supply_channel_t channel)
{
    MODIFY_REG32(base->SQL, ADC_SQL_SUPPLY_Msk, ADC_SQL_SUPPLY_Pos, channel);
}

/*!
 * @brief Get internal supply channel setting.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return current supply channel setting
 */
static inline adc_supply_channel_t ADC_GetSupplyChannel(const ADC_Type * const base)
{
    return (adc_supply_channel_t)(READ_BIT32(base->SQL, ADC_SQL_SUPPLY_Msk) >> ADC_SQL_SUPPLY_Pos);
}

/*!
 * @brief Set ADC DMA enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disbale DMA function in ADC
 * @return none
 */
static inline void ADC_SetDMAEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_DMAEN_Msk, ADC_CTRL0_DMAEN_Pos, state);
}

/*!
 * @brief Get ADC DMA enable flag status.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether DMA support in ADC is enabled
 */
static inline bool ADC_GetDMAEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_DMAEN_Msk) >> ADC_CTRL0_DMAEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set regular group sequence.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @param[in] channel: adc channel
 * @return none
 */
static inline void ADC_SetRegularConversionChannel(ADC_Type * const base,
        const adc_sequence_t seq, adc_inputchannel_t channel)
{
    MODIFY_REG32(base->RSQR[seq], ADC_SQR_SEQ_Msk, ADC_SQR_SEQ_Pos, channel);
}

/*!
 * @brief Get the channel of particular regular group sequence.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @return adc channel that particular sequence is using
 */
static inline adc_inputchannel_t ADC_GetRegularConversionChannel(const ADC_Type * const base, const adc_sequence_t seq)
{
    return (adc_inputchannel_t)(READ_BIT32(base->RSQR[seq], ADC_SQR_SEQ_Msk) >> ADC_SQR_SEQ_Pos);
}

/*!
 * @brief Set regular group end of convertion(EOC) interrupt enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @param[in] state: enable or disbale EOC interrupt
 * @return none
 */
static inline void ADC_SetRegularEOCInterruptEnableFlag(ADC_Type * const base, const adc_sequence_t seq, const bool state)
{
    MODIFY_REG32(base->RSQR[seq], ADC_SQR_IE_Msk, ADC_SQR_IE_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get regular group EOC interrupt enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @return whether EOC interrupt of particular sequence is enabled
 */
static inline bool ADC_GetRegularEOCInterruptEnableFlag(const ADC_Type * const base, const adc_sequence_t seq)
{
    uint32_t tmp = READ_BIT32(base->RSQR[seq], ADC_SQR_IE_Msk) >> ADC_SQR_IE_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Get regular group EOC flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @return EOC flag of particular sequence
 */
static inline bool ADC_GetRegularEOCFlag(const ADC_Type * const base, const adc_sequence_t seq)
{
    uint32_t tmp = READ_BIT32(base->RSQR[seq], ADC_SQR_EOC_Msk) >> ADC_SQR_EOC_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Clear regular group EOC flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @return none
 */
static inline void ADC_ClearRegularEOCFlag(ADC_Type * const base, const adc_sequence_t seq)
{
    SET_BIT32(base->RSQR[seq], ADC_SQR_EOC_Msk);
}

/*!
 * @brief Set injection group sequence.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @param[in] channel: adc channel
 * @return none
 */
static inline void ADC_SetInjectConversionChannel(ADC_Type * const base,
        const adc_sequence_t seq, adc_inputchannel_t channel)
{
    MODIFY_REG32(base->ISQR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_SQR_SEQ_Msk, ADC_SQR_SEQ_Pos, channel);
}

/*!
 * @brief Get the channel of particular injection group sequence.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @return adc channel that particular sequence is using
 */
static inline adc_inputchannel_t ADC_GetInjectConversionChannel(const ADC_Type * const base, const adc_sequence_t seq)
{
    return (adc_inputchannel_t)(READ_BIT32(base->ISQR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_SQR_SEQ_Msk) >> ADC_SQR_SEQ_Pos);
}

/*!
 * @brief Set injection group end of convertion(EOC) interrupt enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @param[in] state: enable or disbale EOC interrupt
 * @return none
 */
static inline void ADC_SetInjectEOCInterruptEnableFlag(ADC_Type * const base, const adc_sequence_t seq, const bool state)
{
    MODIFY_REG32(base->ISQR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_SQR_IE_Msk, ADC_SQR_IE_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get injection group EOC interrupt enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @return whether EOC interrupt of particular sequence is enabled
 */
static inline bool ADC_GetInjectEOCInterruptEnableFlag(const ADC_Type * const base, const adc_sequence_t seq)
{
    uint32_t tmp = READ_BIT32(base->ISQR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_SQR_IE_Msk) >> ADC_SQR_IE_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Get injection group EOC flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @return EOC flag of particular sequence
 */
static inline bool ADC_GetInjectEOCFlag(const ADC_Type * const base, const adc_sequence_t seq)
{
    uint32_t tmp = READ_BIT32(base->ISQR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_SQR_EOC_Msk) >> ADC_SQR_EOC_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Clear injection group EOC flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc sequence
 * @return none
 */
static inline void ADC_ClearInjectEOCFlag(ADC_Type * const base, const adc_sequence_t seq)
{
    SET_BIT32(base->ISQR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_SQR_EOC_Msk);
}

/*!
 * @brief Set ADC data alignment.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] align: data alignment setting
 *                  - ADC_DATA_ALIGN_RIGHT
 *                  - ADC_DATA_ALIGN_LEFT
 * @return none
 */
static inline void ADC_SetDataAlign(ADC_Type * const base, const adc_align_t align)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_ALIGN_Msk, ADC_CTRL0_ALIGN_Pos, align);
}

/*!
 * @brief Get adc data alignment setting.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return alignment setting
 *                  - ADC_DATA_ALIGN_RIGHT
 *                  - ADC_DATA_ALIGN_LEFT
 */
static inline adc_align_t ADC_GetDataAlign(const ADC_Type * const base)
{
    return (adc_align_t)(READ_BIT32(base->CTRL0, ADC_CTRL0_ALIGN_Msk) >> ADC_CTRL0_ALIGN_Pos);
}

/*!
 * @brief Get ADC regular group data.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc seqeunce
 * @return ADC data of particular sequence
 */
static inline uint16_t ADC_GetRegularData(const ADC_Type * const base, const adc_sequence_t seq)
{
    return (uint16_t)(READ_BIT32(base->RDR[seq], ADC_DR_DATA_Msk) >> ADC_DR_DATA_Pos);
}

/*!
 * @brief Get ADC regular group data even parity.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc seqeunce
 * @return ADC data even parity value of particular sequence
 */
static inline uint8_t ADC_GetRegularParity(const ADC_Type * const base, const adc_sequence_t seq)
{
    return (uint8_t)(READ_BIT32(base->RDR[seq], ADC_DR_EVEN_PARITY_Msk) >> ADC_DR_EVEN_PARITY_Pos);
}

/*!
 * @brief Get ADC regular group sequence channel number.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc seqeunce
 * @return ADC channel of particular sequence
 */
static inline uint8_t ADC_GetRegularSeqChannel(const ADC_Type * const base, const adc_sequence_t seq)
{
    return (uint8_t)(READ_BIT32(base->RDR[seq], ADC_DR_CH_Msk) >> ADC_DR_CH_Pos);
}

/*!
 * @brief Get ADC injection data.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc seqeunce
 * @return ADC data of particular sequence
 */
static inline uint16_t ADC_GetInjectData(const ADC_Type * const base, const adc_sequence_t seq)
{
    return (uint16_t)(READ_BIT32(base->IDR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_DR_DATA_Msk) >> ADC_DR_DATA_Pos);
}

/*!
 * @brief Get ADC data even parity.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc seqeunce
 * @return ADC data even parity value of particular sequence
 */
static inline uint8_t ADC_GetInjectParity(const ADC_Type * const base, const adc_sequence_t seq)
{
    return (uint8_t)(READ_BIT32(base->IDR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_DR_EVEN_PARITY_Msk) >> ADC_DR_EVEN_PARITY_Pos);
}

/*!
 * @brief Get ADC group sequence channel number.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] seq: adc seqeunce
 * @return ADC channel of particular sequence
 */
static inline uint8_t ADC_GetInjectSeqChannel(const ADC_Type * const base, const adc_sequence_t seq)
{
    return (uint8_t)(READ_BIT32(base->IDR[(uint32_t)seq - (uint32_t)ADC_ISEQ_0], ADC_DR_CH_Msk) >> ADC_DR_CH_Pos);
}

/*!
 * @brief Set hardware average enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disbale hardware average function
 * @return none
 */
static inline void ADC_SetAverageEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL1, ADC_CTRL1_AVGE_Msk, ADC_CTRL1_AVGE_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get hardware average enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether hardware average function is enabled
 */
static inline bool ADC_GetAverageEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL1, ADC_CTRL1_AVGE_Msk) >> ADC_CTRL1_AVGE_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set hardware average mode.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] averageMode: average mode
 *                  - ADC_AVERAGE_4
 *                  - ADC_AVERAGE_8
 *                  - ADC_AVERAGE_16
 *                  - ADC_AVERAGE_32
 * @return none
 */
static inline void ADC_SetAverageMode(ADC_Type * const base, const adc_average_t averageMode)
{
    MODIFY_REG32(base->CTRL1, ADC_CTRL1_AVGS_Msk, ADC_CTRL1_AVGS_Pos, averageMode);
}

/*!
 * @brief Get hardware average mode.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return  average mode
 *                  - ADC_AVERAGE_4
 *                  - ADC_AVERAGE_8
 *                  - ADC_AVERAGE_16
 *                  - ADC_AVERAGE_32
 */
static inline adc_average_t ADC_GetAverageMode(const ADC_Type * const base)
{
    return (adc_average_t)(READ_BIT32(base->CTRL1, ADC_CTRL1_AVGS_Msk) >> ADC_CTRL1_AVGS_Pos);
}

/*!
 * @brief Set Calibration function enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disbale calibration function
 * @return none
 */
static inline void ADC_SetCalibrationEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL1, ADC_CTRL1_CALEN_Msk, ADC_CTRL1_CALEN_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get calibration function enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether calibration function is enabled
 */
static inline bool ADC_GetCalibrationEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL1_CALEN_Msk) >> ADC_CTRL1_CALEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set gain error and offset error value0 for external channel.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] gain: gain error in LSB. Can only be in [-4096, 4095]
 * @param[in] offset: offset error in LSB. Can only be in [-1024, 1023]
 * @return none
 */
static inline void ADC_SetGainOffset0Value(ADC_Type * const base, const int16_t gain, const int16_t offset)
{
    uint32_t gainValue = gain, offsetValue = offset;
    MODIFY_REG32(base->CALI0, ADC_CALI0_GE0_Msk, ADC_CALI0_GE0_Pos, gainValue);
    MODIFY_REG32(base->CALI1, ADC_CALI1_OE0_Msk, ADC_CALI1_OE0_Pos, offsetValue);
}

/*!
 * @brief Get gain error and offset error value0 for external channel.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[out] gain: gain error in LSB.
 * @param[out] offset: offset error in LSB.
 * @return none
 */
static inline void ADC_GetGainOffset0Value(const ADC_Type * const base, int16_t * const gain, int16_t * const offset)
{
    *gain = (int16_t)((uint16_t)(READ_BIT32(base->CALI0, ADC_CALI0_GE0_Msk) >> ADC_CALI0_GE0_Pos) << 4U) >> 4U;
    *offset = (int16_t)((uint16_t)(READ_BIT32(base->CALI1, ADC_CALI1_OE0_Msk) >> ADC_CALI1_OE0_Pos) << 5U) >> 5U;
}

/*!
 * @brief Set gain error and offset error value0 for internal channel.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] gain: gain error in LSB. Can only be in [-4096, 4095]
 * @param[in] offset: offset error in LSB. Can only be in [-1024, 1023]
 * @return none
 */
static inline void ADC_SetGainOffset1Value(ADC_Type * const base, const int16_t gain, const int16_t offset)
{
    uint32_t gainValue = gain, offsetValue = offset;
    MODIFY_REG32(base->CALI0, ADC_CALI0_GE1_Msk, ADC_CALI0_GE1_Pos, gainValue);
    MODIFY_REG32(base->CALI1, ADC_CALI1_OE1_Msk, ADC_CALI1_OE1_Pos, offsetValue);
}

/*!
 * @brief Get gain error and offset error value0 for internal channel.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] gain: gain error in LSB.
 * @param[in] offset: offset error in LSB.
 * @return none
 */
static inline void ADC_GetGainOffset1Value(const ADC_Type * const base, int16_t * const gain, int16_t * const offset)
{
    *gain = (int16_t)((uint16_t)(READ_BIT32(base->CALI0, ADC_CALI0_GE1_Msk) >> ADC_CALI0_GE1_Pos) << 4U) >> 4U;
    *offset = (int16_t)((uint16_t)(READ_BIT32(base->CALI1, ADC_CALI1_OE1_Msk) >> ADC_CALI1_OE1_Pos) << 5U) >> 5U;
}

/*!
 * @brief Set analog monitor interrupt enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable analog monitor interrupt function.
 * @return none
 */
static inline void ADC_SetAMOInterruptEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_AMOIE_Msk, ADC_CTRL0_AMOIE_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get analog monitor interrupt enable flag state.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether analog monitor interrupt is enabled.
 */
static inline bool ADC_GetAMOInterruptEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_AMOIE_Msk) >> ADC_CTRL0_AMOIE_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set analog monitor single channel mode enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable analog monitor single channel mode
 * @return none
 */
static inline void ADC_SetAMOSingleChannelEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_AMOSGL_Msk, ADC_CTRL0_AMOSGL_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get analog monitor single channel mode enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether analog monitor single channel mode is enabled
 */
static inline bool ADC_GetAMOSingleChannelEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_AMOSGL_Msk) >> ADC_CTRL0_AMOSGL_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set analog monitor single channel.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] channel: channel number that need to be monitored
 * @return none
 */
static inline void ADC_SetAMOSingleChannel(ADC_Type * const base, const adc_inputchannel_t channel)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_AMOCH_Msk, ADC_CTRL0_AMOCH_Pos, channel);
}

/*!
 * @brief Get the channel number that analog monitor is monitoring in single channel monitor mode.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return the channel number that analog monitor is monitoring.
 */
static inline adc_inputchannel_t ADC_GetAMOSingleChannel(const ADC_Type * const base)
{
    return (adc_inputchannel_t)(READ_BIT32(base->CTRL0, ADC_CTRL0_AMOCH_Msk) >> ADC_CTRL0_AMOCH_Pos);
}

/*!
 * @brief Set regular group analog monitor mode enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable regualr group analog monitor function.
 * @return none
 */
static inline void ADC_SetAMORegularEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_AMOEN_Msk, ADC_CTRL0_AMOEN_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get regular group analog monitor mode enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether regualr group analog monitor function is enabled.
 */
static inline bool ADC_GetAMORegularEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_AMOEN_Msk) >> ADC_CTRL0_AMOEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set injected group analog monitor mode.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] state: enable or disable injection group analog monitor function.
 * @return none
 */
static inline void ADC_SetAMOInjectEnableFlag(ADC_Type * const base, const bool state)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_IAMOEN_Msk, ADC_CTRL0_IAMOEN_Pos,
            state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get injection group analog monitor mode enable flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether regualr group analog monitor function is enabled.
 */
static inline bool ADC_GetAMOInjectEnableFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->CTRL0, ADC_CTRL0_IAMOEN_Msk) >> ADC_CTRL0_IAMOEN_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Set analog monitor trigger type.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] mode: analog monitor trigger mode
 *                  - ADC_AMO_TRIGGER_LEVEL
 *                  - ADC_AMO_TRIGGER_EDGE
 * @return none
 */
static inline void ADC_SetAMOTriggerMode(ADC_Type * const base, const adc_amo_trigger_mode_t mode)
{
    MODIFY_REG32(base->CTRL0, ADC_CTRL0_AMOMODE_Msk, ADC_CTRL0_AMOMODE_Pos, mode);
}

/*!
 * @brief Get analog monitor trigger mode setting.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return analog monitor trigger mode setting
 *                  - ADC_AMO_TRIGGER_LEVEL
 *                  - ADC_AMO_TRIGGER_EDGE
 */
static inline adc_amo_trigger_mode_t ADC_GetAMOTriggerMode(const ADC_Type * const base)
{
    return (adc_amo_trigger_mode_t)(READ_BIT32(base->CTRL0, ADC_CTRL0_AMOMODE_Msk) >> ADC_CTRL0_AMOMODE_Pos);
}

/*!
 * @brief Set analog monitor upper threshold.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] value: upper threshold, a 12-bit value
 * @return none
 */
static inline void ADC_SetAMOUpThreshold(ADC_Type * const base,
        const uint16_t value)
{
    MODIFY_REG32(base->AMOHR, ADC_AMOHR_AMOHT_Msk, ADC_AMOHR_AMOHT_Pos, value);
}

/*!
 * @brief Get analog monitor upper threshold.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return analog monitor upper threshold
 */
static inline uint16_t ADC_GetAMOUpThreshold(const ADC_Type * const base)
{
    return (uint16_t)(READ_BIT32(base->AMOHR, ADC_AMOHR_AMOHT_Msk) >> ADC_AMOHR_AMOHT_Pos);
}

/*!
 * @brief Set analog monitor lower threshold.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] value: lower threshold, a 12-bit value.
 * @return none
 */
static inline void ADC_SetAMOLowThreshold(ADC_Type * const base,
        const uint16_t value)
{
    MODIFY_REG32(base->AMOLR, ADC_AMOLR_AMOLT_Msk, ADC_AMOLR_AMOLT_Pos, value);
}

/*!
 * @brief Get analog monitor lower threshold.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return analog monitor lower threshold
 */
static inline uint16_t ADC_GetAMOLowThreshold(const ADC_Type * const base)
{
    return (uint16_t)(READ_BIT32(base->AMOLR, ADC_AMOLR_AMOLT_Msk) >> ADC_AMOLR_AMOLT_Pos);
}

/*!
 * @brief Set analog monitor upper offset.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] value: upper offset, a 12-bit value.
 * @return none
 */
static inline void ADC_SetAMOUpOffset(ADC_Type * const base, const uint16_t value)
{
    MODIFY_REG32(base->AMOHR, ADC_AMOHR_AMOHO_Msk, ADC_AMOHR_AMOHO_Pos, value);
}

/*!
 * @brief Get analog monitor upper offset.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return analog monitor upper offset setting.
 */
static inline uint16_t ADC_GetAMOUpOffset(const ADC_Type * const base)
{
    return (uint16_t)(READ_BIT32(base->AMOHR, ADC_AMOHR_AMOHO_Msk) >> ADC_AMOHR_AMOHO_Pos);
}

/*!
 * @brief Set analog monitor lower offset.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @param[in] lowOffset: lower offset, a 12-bit value.
 * @return none
 */
static inline void ADC_SetAMOLowOffset(ADC_Type * const base, const uint16_t lowOffset)
{
    MODIFY_REG32(base->AMOLR, ADC_AMOLR_AMOLO_Msk, ADC_AMOLR_AMOLO_Pos, lowOffset);
}

/*!
 * @brief Get analog monitor lower offset.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return analog monitor lower offset setting.
 */
static inline uint16_t ADC_GetAMOLowOffset(const ADC_Type * const base)
{
    return (uint16_t)(READ_BIT32(base->AMOLR, ADC_AMOLR_AMOLO_Msk) >> ADC_AMOLR_AMOLO_Pos);
}

/*!
 * @brief Get analog monitor flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return analog monitor flag state
 */
static inline bool ADC_GetAMOFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->STR, ADC_STR_AMO_Msk) >> ADC_STR_AMO_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Get idle flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return idle flag state.
 */
static inline bool ADC_GetIdleFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->STR, ADC_STR_IDLE_Msk) >> ADC_STR_IDLE_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Clear analog monitor flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return none
 */
static inline void ADC_ClearAMOFlag(ADC_Type * const base)
{
    CLEAR_BIT32(base->STR, ADC_STR_AMO_Msk);
}

/*!
 * @brief Get normal analog monitor flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return analog monitor normal flag state
 */
static inline bool ADC_GetNAMOFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->STR, ADC_STR_NAMO_Msk) >> ADC_STR_NAMO_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Clear normal analog monitor flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return none
 */
static inline void ADC_ClearNAMOFlag(ADC_Type * const base)
{
    CLEAR_BIT32(base->STR, ADC_STR_NAMO_Msk);
}

/*!
 * @brief Get abnormal analog monitor flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return analog monitor abnormal flag state
 */
static inline bool ADC_GetAAMOFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->STR, ADC_STR_AAMO_Msk) >> ADC_STR_AAMO_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Clear abnormal analog monitor flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return none
 */
static inline void ADC_ClearAAMOFlag(ADC_Type * const base)
{
    CLEAR_BIT32(base->STR, ADC_STR_AAMO_Msk);
}

/*!
 * @brief Get convertion conflict flag state.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return whether convertion conflict happened.
 */
static inline bool ADC_GetConvertConflictFlag(const ADC_Type * const base)
{
    uint32_t tmp = READ_BIT32(base->STR, ADC_STR_COVCFT_Msk) >> ADC_STR_COVCFT_Pos;
    return (tmp != 0U);
}

/*!
 * @brief Clear convertion conflict flag.
 *
 * @param[in] base: adc module
 *                  - ADC0
 *                  - ADC1
 * @return none
 */
static inline void ADC_ClearConvertConflictFlag(ADC_Type * const base)
{
    CLEAR_BIT32(base->STR, ADC_STR_COVCFT_Msk);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ADC_HW_H */

/* =============================================  EOF  ============================================== */
