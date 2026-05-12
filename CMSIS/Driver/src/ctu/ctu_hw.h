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
 * @file ctu_hw.h
 *
 * @brief This file provides ctu hardware integration functions.
 *
 */

#ifndef _CTU_HW_H
#define _CTU_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "ctu_drv.h"

/* ============================================  Define  ============================================ */
/* Number of SEL bitfields in one TRGMUX register */
#define TRGMUX_NUM_SEL_BITFIELDS_PER_REG     (4UL)
/* Get the index of TRGMUX register */
#define TRGMUX_IDX_REG(x)                    ((uint8_t)((uint8_t)(x) / TRGMUX_NUM_SEL_BITFIELDS_PER_REG))
/* Get the index of the SEL bitfield inside TRGMUX register */
#define TRGMUX_IDX_SEL_BITFIELD_REG(x)       ((uint8_t)((uint8_t)(x) % TRGMUX_NUM_SEL_BITFIELDS_PER_REG))

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initial all trgmux register with default values.
 *
 * @param[in] base: the ctu base address
 * @return lock status
 *         - STATUS_SUCCESS: not lock, all trgmux register be set with default value
 *         - STATUS_ERROR: at least one register is lock, trgmux register has not be set with default value
 */
status_t TRGMUX_Init(CTU_Type * const base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set trgmux trigger source for target module.
 *
 * @param[in] base: the ctu base address
 * @param[in] triggerSource: trgmux trigger source struct
 * @param[in] targetModule: trgmux target module struct
 * @return none
 */
static inline void TRGMUX_SetTrigSourceForTargetModule(CTU_Type * const base,
                                                       const trgmux_trigger_source_t triggerSource,
                                                       const trgmux_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->TRGMUX.TRGMUXn[TRGMUX_IDX_REG(targetModule)],
                 CTU_TRGMUX_SEL_Msk << (CTU_TRGMUX_SEL_SHIFT * TRGMUX_IDX_SEL_BITFIELD_REG(targetModule)),
                 CTU_TRGMUX_SEL_SHIFT * TRGMUX_IDX_SEL_BITFIELD_REG(targetModule), triggerSource);
}

/*!
 * @brief Get trgmux trigger source for target module.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule: trgmux target module struct
 * @return trgmux trigger source
 */
static inline trgmux_trigger_source_t TRGMUX_GetTrigSourceForTargetModule(CTU_Type * const base,
                                                                          const trgmux_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    uint8_t trigSource = 0U;

    trigSource = (READ_BIT32(base->TRGMUX.TRGMUXn[TRGMUX_IDX_REG(targetModule)], 0xFFFFFFFFU) \
                  >> (CTU_TRGMUX_SEL_SHIFT * TRGMUX_IDX_SEL_BITFIELD_REG(targetModule))) & CTU_TRGMUX_SEL_Msk;

    return (trgmux_trigger_source_t)(trigSource);
}

/*!
 * @brief Set trgmux config lock for target module.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule: trgmux target module
 * @return none
 */
static inline void TRGMUX_SetLockForTargetModule(CTU_Type * const base, const trgmux_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->TRGMUX.TRGMUXn[TRGMUX_IDX_REG(targetModule)],
                 CTU_TRGMUX_DMA_LK1_Msk, CTU_TRGMUX_DMA_LK1_Pos, true);
}

/*!
 * @brief Get trgmux lock state for target module.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule: trgmux target module struct
 * @return trgmux lock state
 */
static inline bool TRGMUX_GetLockForTargetModule(CTU_Type * const base, const trgmux_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    bool lockVal = false;
    uint8_t lockState = 0U;

    lockState = READ_BIT32(base->TRGMUX.TRGMUXn[TRGMUX_IDX_REG(targetModule)],
                           CTU_TRGMUX_DMA_LK1_Msk) >> CTU_TRGMUX_DMA_LK1_Pos;

    if (lockState)
    {
        lockVal = true;
    }

    return (lockVal);
}

/*!
 * @brief Set ctu rtc clock capture.
 *
 * @param[in] base: the ctu base address
 * @param[in] triggerSource
 *            - TIMER_CH3 capture TIMER_CH3_TT
 *            - TIMER_CH3 capture RTC clock signal
 * @return none
 */
static inline void CTU_SetRTCClockCapture(CTU_Type * const base, uint8_t triggerSource)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_CFG, CTU_CTU_CFG_RTC_CLK_CAP_Msk, CTU_CTU_CFG_RTC_CLK_CAP_Pos, triggerSource);
}

/*!
 * @brief Get rtc clock source capture.
 *
 * @param[in] base: the ctu base address
 * @return rtc clock source capture state.
 */
static inline uint8_t CTU_GetRTCClockCapture(CTU_Type * const base)
{
    DEVICE_ASSERT(base != NULL);

    uint8_t triggerSource = 0U;

    triggerSource = READ_BIT32(base->CTU_CFG, CTU_CTU_CFG_RTC_CLK_CAP_Msk) >> CTU_CTU_CFG_RTC_CLK_CAP_Pos;

    return (triggerSource);
}

/*!
 * @brief Set pwm fault trigger source.
 *
 * @param[in] base: the ctu base address
 * @param[in] triggerSource
 *            - PWM_FAULT_IN_PIN input signal connected to PWM
 *            - PWM_FAULT_TT input signal connected to PWM
 * @param[in] targetModule: pwm fault target module struct
 * @return none
 */
static inline void CTU_SetPWMFaultTriggerSource(CTU_Type * const base,
                                                uint8_t triggerSource,
                                                ctu_pwm_fault_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_CFG, CTU_CTU_CFG_SEL_PWM0_FAULT0_Msk << ((uint8_t)targetModule),
                 CTU_CTU_CFG_SEL_PWM0_FAULT0_Pos + ((uint8_t)targetModule), triggerSource);
}

/*!
 * @brief Get pwm fault trigger source.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule: pwm fault target module struct
 * @return pwm fault trigger source.
 */
static inline uint8_t CTU_GetPWMFaultTriggerSource(CTU_Type * const base, ctu_pwm_fault_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    uint8_t triggerSource = 0U;

    triggerSource = (READ_BIT32(base->CTU_CFG, (CTU_CTU_CFG_SEL_PWM0_FAULT0_Msk << ((uint8_t)targetModule))) \
                     >> ((uint8_t)targetModule + CTU_CTU_CFG_SEL_PWM0_FAULT0_Pos));

    return (triggerSource);
}

/*!
 * @brief Set pwm synchronization.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule: pwm synchronization target module struct
 * @return none
 */
static inline void CTU_SetPWMSync(CTU_Type * const base, ctu_pwm_sync_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_CFG, CTU_CTU_CFG_SEL_PWM0SYNC_Msk << ((uint8_t)targetModule),
                 CTU_CTU_CFG_SEL_PWM0SYNC_Pos + ((uint8_t)targetModule), true);
}

/*!
 * @brief Clear pwm synchronization.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule: pwm synchronization target module struct
 * @return none
 */
static inline void CTU_ClearPWMSync(CTU_Type * const base, ctu_pwm_sync_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_CFG, CTU_CTU_CFG_SEL_PWM0SYNC_Msk << ((uint8_t)targetModule),
                 CTU_CTU_CFG_SEL_PWM0SYNC_Pos + ((uint8_t)targetModule), false);
}

/*!
 * @brief Set pwm CH trigger source.
 *
 * @param[in] base: the ctu base address
 * @param[in] triggerSource
              - PWMn CHX_IN_PIN
              - ACMP0_OUT/ACMP_HALLA/ACMP_HALLB/ACMP_HALLC
 * @param[in] targetModule: pwm CH target module struct
 * @return none
 */
static inline void CTU_SetPWMCHTriggerSource(CTU_Type * const base,
                                             uint8_t triggerSource,
                                             ctu_pwm_ch_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_CFG, CTU_CTU_CFG_SEL_PWM1_CH0_Msk << ((uint8_t)targetModule),
                 CTU_CTU_CFG_SEL_PWM1_CH0_Pos + ((uint8_t)targetModule), triggerSource);
}

/*!
 * @brief Get pwm CH trigger source.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule: pwm CH target module struct
 * @return pwm CH trigger source.
 */
static inline uint8_t CTU_GetPWMCHTriggerSource(CTU_Type * const base, ctu_pwm_ch_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    uint8_t triggerSource = 0U;

    triggerSource = (READ_BIT32(base->CTU_CFG, (CTU_CTU_CFG_SEL_PWM1_CH0_Msk << ((uint8_t)targetModule))) \
                     >> ((uint8_t)targetModule + CTU_CTU_CFG_SEL_PWM1_CH0_Pos));

    return (triggerSource);
}

/*!
 * @brief Set uart modulation trigger source.
 *
 * @param[in] base: the ctu base address
 * @param[in] triggerSource
              - no modulation
              - modulation with uart triggger target
 * @param[in] targetModule: uart target module struct
 * @return none
 */
static inline void CTU_SetUARTModulationTriggerSource(CTU_Type * const base,
                                                      uint8_t triggerSource,
                                                      ctu_uart_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_CFG, CTU_CTU_CFG_SEL_UART0_TX_Msk << ((uint8_t)targetModule),
                 CTU_CTU_CFG_SEL_UART0_TX_Pos + ((uint8_t)targetModule), triggerSource);
}

/*!
 * @brief Get uart modulation trigger source.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule: uart target module struct
 *
 * @return uart modulation trigger source
 */
static inline uint8_t CTU_GetUARTModulationTriggerSource(CTU_Type * const base, ctu_uart_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    uint8_t triggerSource = 0U;

    triggerSource = (READ_BIT32(base->CTU_CFG, (CTU_CTU_CFG_SEL_UART0_TX_Msk << ((uint8_t)targetModule))) \
                     >> ((uint8_t)targetModule + CTU_CTU_CFG_SEL_UART0_TX_Pos));

    return (triggerSource);
}

/*!
 * @brief Set uart rxd filter trigger source.
 *
 * @param[in] base: the ctu base address
 * @param[in] triggerSource
 *            - rxd input signal is connected to uart module directly
 *            - rxd input signal is filtered by ACMP0, then injected to UART
 * @param[in] targetModule: uart target module struct
 * @return none
 */
static inline void CTU_SetUARTRxDFilterTriggerSource(CTU_Type * const base,
                                                     uint8_t triggerSource,
                                                     ctu_uart_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_CFG, CTU_CTU_CFG_SEL_UART0_Msk << ((uint8_t)targetModule),
                 CTU_CTU_CFG_SEL_UART0_Pos + ((uint8_t)targetModule), triggerSource);
}

/*!
 * @brief Get uart rxd filter trigger source.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule : uart target module struct
 * @return uart rx filter trigger source
 */
static inline uint8_t CTU_GetUARTRxDFilterTriggerSource(CTU_Type * const base, ctu_uart_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    uint8_t triggerSource = 0U;

    triggerSource = (READ_BIT32(base->CTU_CFG, (CTU_CTU_CFG_SEL_UART0_Msk << ((uint8_t)targetModule))) \
                     >> ((uint8_t)targetModule + CTU_CTU_CFG_SEL_UART0_Pos));

    return (triggerSource);
}

/*!
 * @brief Generate sw trigger.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule : sw trigger module struct
 * @return none
 */
static inline void CTU_SetSWTrigger(CTU_Type * const base, ctu_sw_trigger_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_SW, CTU_CTU_SW_SW_TRIG1_TS_Msk << ((uint8_t)targetModule),
                 CTU_CTU_SW_SW_TRIG1_TS_Pos + ((uint8_t)targetModule), true);
}

/*!
 * @brief Clear sw trigger.
 *
 * @param[in] base: the ctu base address
 * @param[in] targetModule : sw trigger module struct
 * @return none
 */
static inline void CTU_ClearSWTrigger(CTU_Type * const base, ctu_sw_trigger_target_module_t targetModule)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->CTU_SW, CTU_CTU_SW_SW_TRIG1_TS_Msk << ((uint8_t)targetModule),
                 CTU_CTU_SW_SW_TRIG1_TS_Pos + ((uint8_t)targetModule), false);
}

/*!
 * @brief Enable/Disable pwm0 modulation.
 *
 * @param[in] base: the ctu base address
 * @param[in] channelEn: PWM0 all 8 channel modulation enable/disable mask
 * @return none
 */
static inline void CTU_EnablePWM0Modulation(CTU_Type * const base, uint8_t channelEn)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->PWM_MODULATION, CTU_PWM_MODULATION_PWM0_OUTSEL_Msk,
                 CTU_PWM_MODULATION_PWM0_OUTSEL_Pos, channelEn);
}

/*!
 * @brief Enable/Disable pwm3 modulation.
 *
 * @param[in] base: the ctu base address
 * @param[in] channelEn: PWM3 all 8 channel modulation enable/disable mask
 * @return none
 */
static inline void CTU_EnablePWM3Modulation(CTU_Type * const base, uint8_t channelEn)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->PWM_MODULATION, CTU_PWM_MODULATION_PWM3_OUTSEL_Msk,
                 CTU_PWM_MODULATION_PWM3_OUTSEL_Pos, channelEn);
}

/**
 * @brief Set adc0&1 regular simultaneous mode.
 *
 * @param[in] base: the ctu base address
 * @param[in] en: enable or disable
 * @return : none
 */
static inline void CTU_SetADCRegularSimultaneous(CTU_Type * const base, bool en)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->ADC_SYNC, CTU_ADC_SYNC_SIMU_REG_Msk, CTU_ADC_SYNC_SIMU_REG_Pos, en);
}

/**
 * @brief Get adc regular simultaneous mode config.
 *
 * @param[in] base: the ctu base address
 * @return adc regular simultaneous mode
 */
static inline bool CTU_GetADCRegularSimultaneous(CTU_Type * const base)
{
    DEVICE_ASSERT(base != NULL);

    return ((READ_BIT32(base->ADC_SYNC, CTU_ADC_SYNC_SIMU_REG_Msk) >> CTU_ADC_SYNC_SIMU_REG_Pos) != 0U);
}

/**
 * @brief Set adc0&1 injection group simultaneous mode.
 *
 * @param[in] base: the ctu base address
 * @param[in] en: enable or disable
 * @return none
 */
static inline void CTU_SetADCInjectSimultaneous(CTU_Type * const base, bool en)
{
    DEVICE_ASSERT(base != NULL);

    MODIFY_REG32(base->ADC_SYNC, CTU_ADC_SYNC_SIMU_INJ_Msk, CTU_ADC_SYNC_SIMU_INJ_Pos, en);
}

/**
 * @brief Get adc injection group simultaneous mode config.
 *
 * @param[in] base: the ctu base address
 * @return en: enable or disable
 */
static inline bool CTU_GetADCInjectSimultaneous(CTU_Type * const base)
{
    DEVICE_ASSERT(base != NULL);

    return ((READ_BIT32(base->ADC_SYNC, CTU_ADC_SYNC_SIMU_INJ_Msk) >> CTU_ADC_SYNC_SIMU_INJ_Pos) != 0U);
}

/**
 * @brief Set adc interleave input channel.
 *
 * @param[in] base: the ctu base address
 * @param[in] mode : interleave mode
 * @return none
 */
static inline void CTU_SetADCInterleave(CTU_Type * const base, ctu_adc_interleave_mode_t mode)
{
    uint32_t reg0 = 0U, reg1 = 0U;

    DEVICE_ASSERT(base != NULL);

    switch (mode)
    {
    case CTU_ADC_INTERLEAVE_DISABLE:
        reg0 = 0U;
        reg1 = 0U;
        break;

    case CTU_ADC0_INTERLEAVE_VIN4:
        reg0 = 0U;
        reg1 = 1U;
        break;

    case CTU_ADC0_INTERLEAVE_VIN5:
        reg0 = 0U;
        reg1 = 3U;
        break;

    case CTU_ADC1_INTERLEAVE_VIN8:
        reg0 = 1U;
        reg1 = 0U;
        break;

    case CTU_ADC1_INTERLEAVE_VIN9:
        reg0 = 3U;
        reg1 = 0U;
        break;

    default:
        reg0 = 0U;
        reg1 = 0U;
        break;
    }

    MODIFY_REG32(base->ADC_SYNC, CTU_ADC_SYNC_ADC1_INTER_Msk | CTU_ADC_SYNC_ADC0_INTER_Msk,
                 CTU_ADC_SYNC_ADC0_INTER_Pos, (reg1 << 4U) | reg0);
}

/**
 * @brief Get adc interleave mode config.
 *
 * @param[in] base: the ctu base address
 * @return ctu_adc_interleave_mode_t: interleave mode
 */
static inline ctu_adc_interleave_mode_t CTU_GetADCInterleave(CTU_Type * const base)
{
    ctu_adc_interleave_mode_t mode = (ctu_adc_interleave_mode_t)0U;
    uint32_t reg = 0U;

    reg = (READ_BIT32(base->ADC_SYNC, CTU_ADC_SYNC_ADC1_INTER_Msk | CTU_ADC_SYNC_ADC0_INTER_Msk) \
           >> CTU_ADC_SYNC_ADC0_INTER_Pos);

    switch (reg)
    {
    case 0x0U:
        mode = CTU_ADC_INTERLEAVE_DISABLE;
        break;

    case 0x1U:
        mode = CTU_ADC1_INTERLEAVE_VIN8;
        break;

    case 0x3U:
        mode = CTU_ADC1_INTERLEAVE_VIN9;
        break;

    case 0x10U:
        mode = CTU_ADC0_INTERLEAVE_VIN4;
        break;

    case 0x30U:
        mode = CTU_ADC0_INTERLEAVE_VIN5;
        break;

    default:
        mode = CTU_ADC_INTERLEAVE_DISABLE;
        break;
    }

    return (mode);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CTU_HW_H */

/* =============================================  EOF  ============================================== */
