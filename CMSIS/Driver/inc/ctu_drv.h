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
 * @file ctu_drv.h
 *
 * @brief This file provides ctu integration functions interface.
 *
 */

#ifndef _CTU_DRV_H
#define _CTU_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Constant array storing the value of all trgmux output(target module) identifiers.
 */
#define FEATURE_TRGMUX_TARGET_MODULE      \
{                                         \
    TRGMUX_TARGET_MODULE_DMA_CH0,         \
    TRGMUX_TARGET_MODULE_DMA_CH1,         \
    TRGMUX_TARGET_MODULE_DMA_CH2,         \
    TRGMUX_TARGET_MODULE_DMA_CH3,         \
    TRGMUX_TARGET_MODULE_EXT_OUT0,        \
    TRGMUX_TARGET_MODULE_EXT_OUT1,        \
    TRGMUX_TARGET_MODULE_EXT_OUT2,        \
    TRGMUX_TARGET_MODULE_EXT_OUT3,        \
    TRGMUX_TARGET_MODULE_EXT_OUT4,        \
    TRGMUX_TARGET_MODULE_EXT_OUT5,        \
    TRGMUX_TARGET_MODULE_EXT_OUT6,        \
    TRGMUX_TARGET_MODULE_EXT_OUT7,        \
    TRGMUX_TARGET_MODULE_ADC0_REGULAR0,   \
    TRGMUX_TARGET_MODULE_ADC0_REGULAR1,   \
    TRGMUX_TARGET_MODULE_ADC0_REGULAR2,   \
    TRGMUX_TARGET_MODULE_ADC0_REGULAR3,   \
    TRGMUX_TARGET_MODULE_ADC0_INJECTION0, \
    TRGMUX_TARGET_MODULE_ADC0_INJECTION1, \
    TRGMUX_TARGET_MODULE_ADC0_INJECTION2, \
    TRGMUX_TARGET_MODULE_ADC0_INJECTION3, \
    TRGMUX_TARGET_MODULE_ADC1_REGULAR0,   \
    TRGMUX_TARGET_MODULE_ADC1_REGULAR1,   \
    TRGMUX_TARGET_MODULE_ADC1_REGULAR2,   \
    TRGMUX_TARGET_MODULE_ADC1_REGULAR3,   \
    TRGMUX_TARGET_MODULE_ADC1_INJECTION0, \
    TRGMUX_TARGET_MODULE_ADC1_INJECTION1, \
    TRGMUX_TARGET_MODULE_ADC1_INJECTION2, \
    TRGMUX_TARGET_MODULE_ADC1_INJECTION3, \
    TRGMUX_TARGET_MODULE_ACMP0_TR,        \
    TRGMUX_TARGET_MODULE_PWM0_TRIG0,      \
    TRGMUX_TARGET_MODULE_PWM0_FAULT0,     \
    TRGMUX_TARGET_MODULE_PWM0_FAULT1,     \
    TRGMUX_TARGET_MODULE_PWM0_FAULT2,     \
    TRGMUX_TARGET_MODULE_PWM1_TRIG0,      \
    TRGMUX_TARGET_MODULE_PWM1_FAULT0,     \
    TRGMUX_TARGET_MODULE_PWM1_FAULT1,     \
    TRGMUX_TARGET_MODULE_PWM1_FAULT2,     \
    TRGMUX_TARGET_MODULE_PWM2_TRIG0,      \
    TRGMUX_TARGET_MODULE_PWM2_FAULT0,     \
    TRGMUX_TARGET_MODULE_PWM2_FAULT1,     \
    TRGMUX_TARGET_MODULE_PWM2_FAULT2,     \
    TRGMUX_TARGET_MODULE_PWM3_TRIG0,      \
    TRGMUX_TARGET_MODULE_PWM3_FAULT0,     \
    TRGMUX_TARGET_MODULE_PWM3_FAULT1,     \
    TRGMUX_TARGET_MODULE_PWM3_FAULT2,     \
    TRGMUX_TARGET_MODULE_PWM4_TRIG0,      \
    TRGMUX_TARGET_MODULE_PWM5_TRIG0,      \
    TRGMUX_TARGET_MODULE_TIMER_CH0,       \
    TRGMUX_TARGET_MODULE_TIMER_CH1,       \
    TRGMUX_TARGET_MODULE_TIMER_CH2,       \
    TRGMUX_TARGET_MODULE_TIMER_CH3,       \
    TRGMUX_TARGET_MODULE_PCT0,            \
    TRGMUX_TARGET_MODULE_UART0,           \
    TRGMUX_TARGET_MODULE_UART1,           \
    TRGMUX_TARGET_MODULE_PDT0,            \
    TRGMUX_TARGET_MODULE_PDT1,            \
    TRGMUX_TARGET_MODULE_EIO_TIMR0,       \
    TRGMUX_TARGET_MODULE_EIO_TIMR1,       \
    TRGMUX_TARGET_MODULE_EIO_TIMR2,       \
    TRGMUX_TARGET_MODULE_EIO_TIMR3        \
};

/*!
 * @brief Enumeration for target module about pwm_fault.
 */
typedef enum
{
    CTU_CFG_PWM0_FAULT0 = 0U,   /*!< Target module of pwm0_fault0 */
    CTU_CFG_PWM0_FAULT1 = 1U,   /*!< Target module of pwm0_fault1 */
    CTU_CFG_PWM0_FAULT2 = 2U,   /*!< Target module of pwm0_fault2 */
    CTU_CFG_PWM1_FAULT0 = 4U,   /*!< Target module of pwm1_fault0 */
    CTU_CFG_PWM1_FAULT1 = 5U,   /*!< Target module of pwm1_fault1 */
    CTU_CFG_PWM1_FAULT2 = 6U,   /*!< Target module of pwm1_fault2 */
    CTU_CFG_PWM2_FAULT0 = 8U,   /*!< Target module of pwm2_fault0 */
    CTU_CFG_PWM2_FAULT1 = 9U,   /*!< Target module of pwm2_fault1 */
    CTU_CFG_PWM2_FAULT2 = 10U,  /*!< Target module of pwm2_fault2 */
    CTU_CFG_PWM3_FAULT0 = 12U,  /*!< Target module of pwm3_fault0 */
    CTU_CFG_PWM3_FAULT1 = 13U,  /*!< Target module of pwm3_fault1 */
    CTU_CFG_PWM3_FAULT2 = 14U   /*!< Target module of pwm3_fault2 */
} ctu_pwm_fault_target_module_t;

/*!
 * @brief Enumeration for target module about pwm_sync.
 */
typedef enum
{
    CTU_CFG_PWM0_SYNC = 0U,  /*!< Target module of pwm0_sync */
    CTU_CFG_PWM1_SYNC = 1U,  /*!< Target module of pwm1_sync */
    CTU_CFG_PWM2_SYNC = 2U,  /*!< Target module of pwm2_sync */
    CTU_CFG_PWM3_SYNC = 3U,  /*!< Target module of pwm3_sync */
    CTU_CFG_PWM4_SYNC = 4U,  /*!< Target module of pwm4_sync */
    CTU_CFG_PWM5_SYNC = 5U   /*!< Target module of pwm5_sync */
} ctu_pwm_sync_target_module_t;

/*!
 * @brief Enumeration for target module about pwm_ch.
 */
typedef enum
{
    CTU_CFG_PWM1_CH0 = 0U,  /*!< Target module of pwm1_ch0 in CFG register */
    CTU_CFG_PWM3_CH0 = 1U,  /*!< Target module of pwm3_ch0 in CFG register */
    CTU_CFG_PWM2_CHx = 2U   /*!< Target module of pwm2_chx in CFG register */
} ctu_pwm_ch_target_module_t;

/*!
 * @brief Enumeration for target module about uart.
 */
typedef enum
{
    CTU_CFG_UART0 = 0U,     /*!< Target module of uart0 in CFG register */
    CTU_CFG_UART1 = 1U      /*!< Target module of uart1 in CFG register */
}ctu_uart_target_module_t;

/*!
 * @brief Enumeration for target module about sw_trig.
 */
typedef enum
{
    CTU_CFG_SW_TRIG0 = 0U,  /*!< Target module of sw_trig0 */
    CTU_CFG_SW_TRIG1 = 1U,  /*!< Target module of sw_trig1 */
    CTU_CFG_SW_TRIG2 = 2U,  /*!< Target module of sw_trig2 */
    CTU_CFG_SW_TRIG3 = 3U   /*!< Target module of sw_trig3 */
} ctu_sw_trigger_target_module_t;

/*!
 * @brief Enumeration for adc_interleace.
 */
typedef enum
{
    CTU_ADC_INTERLEAVE_DISABLE = 0U,   /*!< Adc interleave disable */
    CTU_ADC0_INTERLEAVE_VIN4 = 1U,     /*!< Select adc0 interleave vin4 */
    CTU_ADC0_INTERLEAVE_VIN5 = 2U,     /*!< Select adc0 interleave vin5 */
    CTU_ADC1_INTERLEAVE_VIN8 = 3U,     /*!< Select adc1 interleave vin8 */
    CTU_ADC1_INTERLEAVE_VIN9 = 4U      /*!< Select adc1 interleave vin9 */
} ctu_adc_interleave_mode_t;

/*!
 * @brief Configuration structure for pairing source triggers with target modules.
 */
typedef struct
{
    trgmux_trigger_source_t triggerSource;  /*!< selects one of the trgmux trigger sources */
    trgmux_target_module_t targetModule;    /*!< selects one of the trgmux target modules */
    bool lockTargetModuleReg;               /*!< if true, the LOCK bit of the target module register will be set
                                                 by TRGMUX_DRV_INIT(), after the current mapping if configured*/
} trgmux_inout_mapping_config_t;

/*!
 * @brief User configuration structure for the trgmux driver.
 */
typedef struct
{
    uint8_t numInOutMappingConfigs;                            /*!< Number of in-out mappings defined in trgmux configuration */
    const trgmux_inout_mapping_config_t *inOutMappingConfigs;  /*!< Pointer to array of in-out mapping structures */
} trgmux_user_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize a trgmux instance for operation.
 *
 * This function first resets the source triggers of all trgmux target modules to their default values,
 * then configures the trgmux with all the user defined in-out mappings.
 * If at least one of the target modules is locked, the function will not change any of the trgmux target modules
 * and return error code.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] trgmuxUserConfig: Pointer to the user configuration structure.
 * @return status_t
 *         - STATUS_SUCCESS: all trgmux register not lock
 *         - STATUS_ERROR: at least one trgmux register is lock
 */
status_t TRGMUX_DRV_Init(const uint8_t instance, const trgmux_user_config_t * const trgmuxUserConfig);

/*!
 * @brief Reset to default values the source triggers corresponding to all target modules,
 *        if none of the target modules is locked.
 *
 * @param[in] instance: The trgmux instance number
 * @return status_t
 *         - STATUS_SUCCESS: all trgmux register not lock
 *         - STATUS_ERROR: at least one trgmux register is lock
 */
status_t TRGMUX_DRV_Deinit(const uint8_t instance);

/*!
 * @brief Configure a trigger source for a selected target module
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] triggerSource: trgmux trigger source struct
 * @param[in] targetModule: trgmux target module struct
 * @return status_t
 *         - STATUS_SUCCESS: trgmux register about target module is not lock
 *         - STATUS_ERROR: trgmux register about target module is lock
 */
status_t TRGMUX_DRV_SetTrigSourceForTargetModule(const uint8_t instance,
                                                 const trgmux_trigger_source_t triggerSource,
                                                 const trgmux_target_module_t targetModule);

/*!
 * @brief Get ctu trigger source for target module.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: trgmux target module struct
 * @return trgmux_trigger_source_t: trigger source
 */
trgmux_trigger_source_t TRGMUX_DRV_GetTrigSourceForTargetModule(const uint8_t instance,
                                                                const trgmux_target_module_t targetModule);

/*!
 * @brief Set ctu config lock for target module.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: trgmux target module
 * @return none
 */
void TRGMUX_DRV_SetLockForTargetModule(const uint8_t instance, const trgmux_target_module_t targetModule);

/*!
 * @brief Get ctu lock state for target module.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: trgmux target module struct
 * @return trgmux lock state
 */
bool TRGMUX_DRV_GetLockForTargetModule(const uint8_t instance, const trgmux_target_module_t targetModule);

/*!
 * @brief Set ctu rtc clock capture.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] triggerSource
              - TIMER_CH3 capture TIMER_CH3_TT
              - TIMER_CH3 capture RTC clock signal
 * @return none
 */
void CTU_DRV_SetRTCClockCapture(const uint8_t instance, uint8_t triggerSource);

/*!
 * @brief Get rtc clock source capture.
 *
 * @param[in] instance: The trgmux instance number
 * @return rtc clock source capture state
 */
uint8_t CTU_DRV_GetRTCClockCapture(const uint8_t instance);

/*!
 * @brief Set pwm fault trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] triggerSource
              - PWM_FAULT_IN_PIN input signal connected to PWM
              - PWM_FAULT_TT input signal connected to PWM
 * @param[in] targetModule: pwm fault target module struct
 * @return none
 */
void CTU_DRV_SetPWMFaultTriggerSource(const uint8_t instance,
                                      uint8_t triggerSource,
                                      ctu_pwm_fault_target_module_t targetModule);

/*!
 * @brief Get pwm fault trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: pwm fault target module struct
 * @return pwm fault trigger source
 */
uint8_t CTU_DRV_GetPWMFaultTriggerSource(const uint8_t instance, ctu_pwm_fault_target_module_t targetModule);


/*!
 * @brief Set pwm synchronization.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : pwm synchronization target module struct
 * @return none
 */
void CTU_DRV_SetPWMSync(const uint8_t instance, ctu_pwm_sync_target_module_t targetModule);

/*!
 * @brief Clear pwm synchronization.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : pwm synchronization target module struct
 * @return none
 */
void CTU_DRV_ClearPWMSync(const uint8_t instance, ctu_pwm_sync_target_module_t targetModule);

/*!
 * @brief Set pwm CH trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] triggerSource
              - PWMn CHX_IN_PIN
              - ACMP0_OUT/ACMP_HALLA/ACMP_HALLB/ACMP_HALLC
 * @param[in] targetModule : pwm CH target module struct
 * @return none
 */
void CTU_DRV_SetPWMCHTriggerSource(const uint8_t instance,
                                   uint8_t triggerSource,
                                   ctu_pwm_ch_target_module_t targetModule);

/*!
 * @brief Get pwm CH trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: pwm CH target module struct
 * @return pwm CH trigger source
 */
uint8_t CTU_DRV_GetPWMCHTriggerSource(const uint8_t instance, ctu_pwm_ch_target_module_t targetModule);

/*!
 * @brief Set uart modulation trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] triggerSource
              - no modulation
              - modulation with uart triggger target
 * @param[in] targetModule : uart target module struct
 * @return none
 */
void CTU_DRV_SetUARTModulationTriggerSource(const uint8_t instance,
                                            uint8_t triggerSource,
                                            ctu_uart_target_module_t targetModule);

/*!
 * @brief Get uart modulation trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : uart target module struct
 *
 * @return uart modulation trigger source
 */
uint8_t CTU_DRV_GetUARTModulationTriggerSource(const uint8_t instance, ctu_uart_target_module_t targetModule);

/*!
 * @brief Set uart rxd filter trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] triggerSource
              - rxd input signal is connected to uart module directly
              - rxd input signal is filtered by ACMP0, then injected to UART
 * @param[in] targetModule : uart target module struct
 * @return none
 */
void CTU_DRV_SetUARTRxDFilterTriggerSource(const uint8_t instance,
                                           uint8_t triggerSource,
                                           ctu_uart_target_module_t targetModule);

/*!
 * @brief Get uart rxd filter trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : uart target module struct
 * @return uart rx filter trigger source
 */
uint8_t CTU_DRV_GetUARTRxDFilterTriggerSource(const uint8_t instance, ctu_uart_target_module_t targetModule);

/*!
 * @brief Generate sw trigger.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : sw trigger module struct
 * @return none
 */
void CTU_DRV_SetSWTrigger(const uint8_t instance, ctu_sw_trigger_target_module_t targetModule);

/*!
 * @brief Clear sw trigger.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : sw trigger module struct
 * @return none
 */
void CTU_DRV_ClearSWTrigger(const uint8_t instance, ctu_sw_trigger_target_module_t targetModule);

/*!
 * @brief Enable/Disable pwm0 modulation.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] channelEn: PWM0 all 8 channel modulation enable/disable mask
 * @return none
 */
void CTU_DRV_EnablePWM0Modulation(const uint8_t instance, uint8_t channelEn);

/*!
 * @brief Enable/Disable pwm3 modulation.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] channelEn: PWM3 all 8 channel modulation enable/disable mask
 * @return none
 */
void CTU_DRV_EnablePWM3Modulation(const uint8_t instance, uint8_t channelEn);

/*!
 * @brief Set adc regular simultaneous mode.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] enable: whether enable adc regular simultaneous mode
 * @return none
 */
void CTU_DRV_SetADCRegularSimultaneous(const uint8_t instance, bool enable);

/**
 * @brief Get adc regular simultaneous mode config.
 *
 * @param[in] instance: The trgmux instance number
 * @return enable/disable: regular simultaneous mode
 */
bool CTU_DRV_GetADCRegularSimultaneous(const uint8_t instance);

/*!
 * @brief Set adc inject simultaneous mode.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] enable: whether enable regular simultaneous mode
 * @return none
 */
void CTU_DRV_SetADCInjectSimultaneous(const uint8_t instance, bool enable);

/**
 * @brief Get adc inject simultaneous mode config
 *
 * @param[in] instance: The trgmux instance number
 * @return inject simultaneous mode
 */
bool CTU_DRV_GetADCInjectSimultaneous(const uint8_t instance);

/**
 * @brief Set adc interleave input channel.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] mode : interleave mode
 * @return none
 */
void CTU_DRV_SetADCInterleave(const uint8_t instance, ctu_adc_interleave_mode_t mode);

/**
 * @brief Get adc interleave mode config.
 *
 * @param[in] instance: The trgmux instance number
 * @return ctu_adc_interleave_mode_t: interleave mode
 */
ctu_adc_interleave_mode_t CTU_DRV_GetADCInterleave(const uint8_t instance);

/*!
 * @brief CTU Module De-initialize.
 *
 * @param[in] instance: The trgmux instance number
 * @return none
 */
void CTU_DRV_DeInit(const uint8_t instance);

/*!
 * @brief CTU ckgen disable & module reset.
 *
 * @param[in] instance: The trgmux instance number
 * @return none
 */
void CTU_DRV_Init(const uint8_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CTU_DRV_H */

/* =============================================  EOF  ============================================== */
