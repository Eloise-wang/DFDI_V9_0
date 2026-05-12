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
 * @file ctu_drv.c
 *
 * @brief This file provides ctu integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "ctu_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* CTU base address */
static CTU_Type *s_ctuBase[CTU_INSTANCE_MAX] = CTU_BASE_PTRS;

/* CTU ckgen clk info */
const ckgen_clock_t s_ctuCkgenClks[CTU_INSTANCE_MAX] = CTU_CKGEN_CLOCKS;
/* CTU soft reset info */
const ckgen_softreset_t s_ctuSoftReset[CTU_INSTANCE_MAX] = CTU_SOFT_RESETS;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initialize a trgmux instance for operation.
 *
 * This function first resets the source triggers of all TRGMUX target modules to their default values,
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
status_t TRGMUX_DRV_Init(const uint8_t instance, const trgmux_user_config_t * const trgmuxUserConfig)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);
    DEVICE_ASSERT(trgmuxUserConfig != NULL);

    status_t status;
    CTU_Type *base = s_ctuBase[instance];
    uint8_t i = 0U;

    status = TRGMUX_Init(base);

    if (STATUS_SUCCESS == status)
    {
       for (i = 0U; i < trgmuxUserConfig->numInOutMappingConfigs; i++)
       {
           TRGMUX_SetTrigSourceForTargetModule(base, trgmuxUserConfig->inOutMappingConfigs[i].triggerSource,
                                               trgmuxUserConfig->inOutMappingConfigs[i].targetModule);
       }

       for (i = 0U; i < trgmuxUserConfig->numInOutMappingConfigs; i++)
       {
           if (trgmuxUserConfig->inOutMappingConfigs[i].lockTargetModuleReg)
           {
               TRGMUX_SetLockForTargetModule(base, trgmuxUserConfig->inOutMappingConfigs[i].targetModule);
           }
       }
    }

    return (status);
}

/*!
 * @brief Reset to default values the source triggers corresponding to all target modules,
 *        if none of the target modules is locked.
 *
 * @param[in] instance: The trgmux instance number
 * @return status_t
 *         - STATUS_SUCCESS: all trgmux register not lock
 *         - STATUS_ERROR: at least one trgmux register is lock
 */
status_t TRGMUX_DRV_Deinit(const uint8_t instance)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];
    status_t status;

    status = TRGMUX_Init(base);

    return (status);
}

/*!
 * @brief Configure a trigger source for a selected target module.
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
                                                 const trgmux_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    bool lock;
    status_t status = STATUS_ERROR;
    CTU_Type *base = s_ctuBase[instance];

    lock = TRGMUX_GetLockForTargetModule(base, targetModule);

    if (false == lock)
    {
        TRGMUX_SetTrigSourceForTargetModule(base, triggerSource, targetModule);
        status = STATUS_SUCCESS;
    }

    return (status);
}

/*!
 * @brief Get ctu trigger source for target module.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: trgmux target module struct
 * @return trgmux_trigger_source_t: trigger source
 */
trgmux_trigger_source_t TRGMUX_DRV_GetTrigSourceForTargetModule(const uint8_t instance,
                                                                const trgmux_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return TRGMUX_GetTrigSourceForTargetModule(base, targetModule);
}

/*!
 * @brief Set ctu config lock for target module.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: trgmux target module
 * @return none
 */
void TRGMUX_DRV_SetLockForTargetModule(const uint8_t instance, const trgmux_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    TRGMUX_SetLockForTargetModule(base, targetModule);
}

/*!
 * @brief Get ctu lock state for target module.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: trgmux target module struct
 * @return trgmux lock state
 */
bool TRGMUX_DRV_GetLockForTargetModule(const uint8_t instance, const trgmux_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return TRGMUX_GetLockForTargetModule(base, targetModule);
}

/*!
 * @brief Set ctu rtc clock capture.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] triggerSource
              - TIMER_CH3 capture TIMER_CH3_TT
              - TIMER_CH3 capture RTC clock signal
 * @return none
 */
void CTU_DRV_SetRTCClockCapture(const uint8_t instance, uint8_t triggerSource)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetRTCClockCapture(base, triggerSource);
}

/*!
 * @brief Get rtc clock source capture.
 *
 * @param[in] instance: The trgmux instance number
 * @return rtc clock source capture state.
 */
uint8_t CTU_DRV_GetRTCClockCapture(const uint8_t instance)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return CTU_GetRTCClockCapture(base);
}

/*!
 * @brief Set pwm fault trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] triggerSource
              - PWM_FAULT_IN_PIN input signal connected to PWM
              - PWM_FAULT_TT input signal connected to PWM
 * @param[in] targetModule : pwm fault target module struct
 * @return none
 */
void CTU_DRV_SetPWMFaultTriggerSource(const uint8_t instance,
                                      uint8_t triggerSource,
                                      ctu_pwm_fault_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetPWMFaultTriggerSource(base, triggerSource, targetModule);
}

/*!
 * @brief Get pwm fault trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: pwm fault target module struct
 * @return pwm fault trigger source.
 */
uint8_t CTU_DRV_GetPWMFaultTriggerSource(const uint8_t instance, ctu_pwm_fault_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return CTU_GetPWMFaultTriggerSource(base, targetModule);
}

/*!
 * @brief Set pwm synchronization.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : pwm synchronization target module struct
 * @return none
 */
void CTU_DRV_SetPWMSync(const uint8_t instance, ctu_pwm_sync_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetPWMSync(base, targetModule);
}

/*!
 * @brief Clear pwm synchronization.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : pwm synchronization target module struct
 * @return none
 */
void CTU_DRV_ClearPWMSync(const uint8_t instance, ctu_pwm_sync_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_ClearPWMSync(base, targetModule);
}

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
                                   ctu_pwm_ch_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetPWMCHTriggerSource(base, triggerSource, targetModule);
}

/*!
 * @brief Get pwm CH trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule: pwm CH target module struct
 * @return pwm CH trigger source.
 */
uint8_t CTU_DRV_GetPWMCHTriggerSource(const uint8_t instance, ctu_pwm_ch_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return CTU_GetPWMCHTriggerSource(base, targetModule);
}

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
                                            ctu_uart_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetUARTModulationTriggerSource(base, triggerSource, targetModule);
}

/*!
 * @brief Get uart modulation trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : uart target module struct
 *
 * @return uart modulation trigger source
 */
uint8_t CTU_DRV_GetUARTModulationTriggerSource(const uint8_t instance, ctu_uart_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return CTU_GetUARTModulationTriggerSource(base, targetModule);
}

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
                                           ctu_uart_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetUARTRxDFilterTriggerSource(base, triggerSource, targetModule);
}

/*!
 * @brief Get uart rxd filter trigger source.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : uart target module struct
 * @return uart rx filter trigger source
 */
uint8_t CTU_DRV_GetUARTRxDFilterTriggerSource(const uint8_t instance, ctu_uart_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return CTU_GetUARTRxDFilterTriggerSource(base, targetModule);
}

/*!
 * @brief Generate sw trigger.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : sw trigger module struct
 * @return none
 */
void CTU_DRV_SetSWTrigger(const uint8_t instance, ctu_sw_trigger_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetSWTrigger(base, targetModule);
}

/*!
 * @brief Clear sw trigger.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] targetModule : sw trigger module struct
 * @return none
 */
void CTU_DRV_ClearSWTrigger(const uint8_t instance, ctu_sw_trigger_target_module_t targetModule)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_ClearSWTrigger(base, targetModule);
}

/*!
 * @brief Enable/Disable pwm0 modulation.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] channelEn: PWM0 all 8 channel modulation enable/disable mask
 * @return none
 */
void CTU_DRV_EnablePWM0Modulation(const uint8_t instance, uint8_t channelEn)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_EnablePWM0Modulation(base, channelEn);
}

/*!
 * @brief Enable/Disable pwm3 modulation.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] channelEn: PWM3 all 8 channel modulation enable/disable mask
 * @return none
 */
void CTU_DRV_EnablePWM3Modulation(const uint8_t instance, uint8_t channelEn)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_EnablePWM3Modulation(base, channelEn);
}

/*!
 * @brief Set adc regular simultaneous mode.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] enable: whether enable adc regular simultaneous mode
 * @return none
 */
void CTU_DRV_SetADCRegularSimultaneous(const uint8_t instance, bool enable)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetADCRegularSimultaneous(base, enable);
}

/**
 * @brief Get adc regular simultaneous mode config.
 *
 * @param[in] instance: The trgmux instance number
 * @return enable/disable: regular simultaneous mode
 */
bool CTU_DRV_GetADCRegularSimultaneous(const uint8_t instance)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return CTU_GetADCRegularSimultaneous(base);
}

/*!
 * @brief Set adc inject simultaneous mode.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] enable: whether enable regular simultaneous mode
 * @return none
 */
void CTU_DRV_SetADCInjectSimultaneous(const uint8_t instance, bool enable)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetADCInjectSimultaneous(base, enable);
}

/**
 * @brief Get adc inject simultaneous mode config.
 *
 * @param[in] instance: The trgmux instance number
 * @return inject simultaneous mode
 */
bool CTU_DRV_GetADCInjectSimultaneous(const uint8_t instance)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return CTU_GetADCInjectSimultaneous(base);
}

/**
 * @brief Set adc interleave input channel.
 *
 * @param[in] instance: The trgmux instance number
 * @param[in] mode : interleave mode
 * @return none
 */
void CTU_DRV_SetADCInterleave(const uint8_t instance, ctu_adc_interleave_mode_t mode)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    CTU_SetADCInterleave(base, mode);
}

/**
 * @brief Get adc interleave mode config.
 *
 * @param[in] instance: The trgmux instance number
 * @return ctu_adc_interleave_mode_t: interleave mode
 */
ctu_adc_interleave_mode_t CTU_DRV_GetADCInterleave(const uint8_t instance)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CTU_Type *base = s_ctuBase[instance];

    return CTU_GetADCInterleave(base);
}

/*!
 * @brief CTU Module De-initialize.
 *
 * @param[in] instance: The trgmux instance number
 * @return none
 */
void CTU_DRV_DeInit(const uint8_t instance)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CKGEN_DRV_SoftReset(s_ctuSoftReset[instance], false);
    CKGEN_DRV_Enable(s_ctuCkgenClks[instance], false);
}

/*!
 * @brief CTU ckgen disable & module reset.
 *
 * @param[in] instance: The trgmux instance number
 * @return none
 */
void CTU_DRV_Init(const uint8_t instance)
{
    DEVICE_ASSERT(instance < CTU_INSTANCE_MAX);

    CKGEN_DRV_SoftReset(s_ctuSoftReset[instance], true);
    CKGEN_DRV_Enable(s_ctuCkgenClks[instance], true);
}

/* =============================================  EOF  ============================================== */
