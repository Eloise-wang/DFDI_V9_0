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
 * @file acmp_drv.c
 *
 * @brief This file provides analog comparator module integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "ctu_drv.h"
#include "ckgen_drv.h"
#include "acmp_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of ADC base address */
static ACMP_Type * const s_acmpBase[] = ACMP_BASE_PTRS;

/* Table of ADC IRQs */
static const IRQn_Type s_acmpIRQ[] = ACMP_IRQS;

/* Table of ADC ckgen interface clocks */
static const ckgen_clock_t s_acmpCkgenClk[] = ACMP_CKGEN_CLOCKS;

/* Table of ACMP soft resets */
static const ckgen_softreset_t s_acmpSoftReset[] = ACMP_SOFT_RESETS;

/* ACMP interrupt callback array */
static acmp_callback_t s_acmpCallback[ACMP_INSTANCE_MAX];

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Reset all ACMP registers. Need to be called after ACMP clock is applied and reset is deasserted.
 *
 * @param[in] instance: ACMP instance number
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_Reset(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);

    status_t status = STATUS_SUCCESS;
    IRQn_Type irq = s_acmpIRQ[instance];

    /*!< Disable ACMP interrupt */
    NVIC_DisableIRQ(irq);
    NVIC_ClearPendingIRQ(irq);

    CKGEN_DRV_SoftReset(s_acmpSoftReset[instance], false);
    CKGEN_DRV_SoftReset(s_acmpSoftReset[instance], true);

    return status;
}

/*!
 * @brief Get reset configuration of all registers in configuration structure.
 *
 * @param[out] config: ACMP configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitConfigAll(acmp_module_t *config)
{
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;

    (void)ACMP_DRV_GetInitConfigComparator(&config->comparator);

    (void)ACMP_DRV_GetInitConfigMUX(&config->mux);

    (void)ACMP_DRV_GetInitConfigDAC(&config->dac);

    (void)ACMP_DRV_GetInitPollingMode(&config->pollingMode);

    return status;
}

/*!
 * @brief Get default configuration structure.
 *
 * @param[out] config: ACMP configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetDefaultConfig(acmp_module_t* const config)
{
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;

    config->comparator.interruptEn       = false;
    config->comparator.outputTrigger     = ACMP_BOTH_EDGES;
    config->comparator.outputSelect      = ACMP_COUT;
    config->comparator.filterEnable      = false;
    config->comparator.filterSampleCount = 0U;
    config->comparator.lpfBandwidth      = ACMP_LPF_500KHZ;
    config->comparator.inverterEnable    = false;
    config->comparator.pinState          = false;
    config->comparator.hysteresisMode    = ACMP_HYS_FALLING_EDGE;
    config->comparator.hysteresisLevel   = ACMP_LEVEL_HYS_10MV;
    config->comparator.usingLSIEnable    = false;
    config->comparator.windowModeEnable  = false;
    config->comparator.clockDivide       = ACMP_FLT_DIVIDE_1;
    config->comparator.callback          = NULL;
    config->comparator.powerEnable       = true;

    config->mux.positiveInputMux = ACMP_EXTERNAL_CH1;
    config->mux.negativeInputMux = ACMP_DAC_OUTPUT;

    config->dac.voltageReferenceSource = ACMP_DAC_VDD;
    config->dac.voltage                = 127U;
    config->dac.state                  = true;

    config->pollingMode.mode               = ACMP_NONE_POLLING;
    config->pollingMode.pollingClockDivide = ACMP_CLK_DIVIDE_256;
    config->pollingMode.pollingSequence    = 0U;
    config->pollingMode.hallOutputEnable   = 0U;
    config->pollingMode.hallAOutputCh      = ACMP_EXTERNAL_CH0;
    config->pollingMode.hallBOutputCh      = ACMP_EXTERNAL_CH0;
    config->pollingMode.hallCOutputCh      = ACMP_EXTERNAL_CH0;

    return status;
}

/*!
 * @brief Configure all comparator function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: ACMP configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_Init(const uint32_t instance, const acmp_module_t *const config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;

    CKGEN_DRV_SoftReset(s_acmpSoftReset[instance], true);
    CKGEN_DRV_Enable(s_acmpCkgenClk[instance], true);

    (void)ACMP_DRV_ConfigComparator(instance, &config->comparator);
    (void)ACMP_DRV_ConfigDAC(instance, &config->dac);
    (void)ACMP_DRV_ConfigMUX(instance, &config->mux);
    (void)ACMP_DRV_ConfigPollingMode(instance, &config->pollingMode);

    return status;
}

/*!
 * @brief Reset ACMP module and close the clock source.
 *
 * @param[in] instance: ACMP instance number
 * @return none
 */
void ACMP_DRV_Deinit(const uint32_t instance)
{
    IRQn_Type irq = s_acmpIRQ[instance];

    /*!< Disable ACMP interrupt */
    NVIC_DisableIRQ(irq);
    NVIC_ClearPendingIRQ(irq);

    CKGEN_DRV_SoftReset(s_acmpSoftReset[instance], false);
    CKGEN_DRV_Enable(s_acmpCkgenClk[instance], false);
}

/*!
 * @brief Get the current ACMP configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: ACMP configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetConfigAll(const uint32_t instance, acmp_module_t * const config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;

    (void)ACMP_DRV_GetComparatorConfig(instance, &config->comparator);
    (void)ACMP_DRV_GetDACConfig(instance, &config->dac);
    (void)ACMP_DRV_GetMUXConfig(instance, &config->mux);
    (void)ACMP_DRV_GetPollingModeConfig(instance, &config->pollingMode);

    return status;
}

/*!
 * @brief Get reset configuration of registers related to ACMP comparator.
 *
 * @param[out] config: ACMP comparator configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitConfigComparator(acmp_comparator_t *config)
{
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;

    config->interruptEn       = false;
    config->outputTrigger     = ACMP_FALLING_EDGE;
    config->outputSelect      = ACMP_COUT;
    config->filterEnable      = false;
    config->filterSampleCount = 0U;
    config->lpfBandwidth      = ACMP_LPF_500KHZ;
    config->inverterEnable    = false;
    config->pinState          = false;
    config->hysteresisMode    = ACMP_HYS_FALLING_EDGE;
    config->hysteresisLevel   = ACMP_LEVEL_HYS_10MV;
    config->usingLSIEnable    = false;
    config->windowModeEnable  = false;
    config->clockDivide       = ACMP_FLT_DIVIDE_1;
    config->callback          = NULL;
    config->powerEnable       = false;

    return status;
}

/*!
 * @brief Configure all ACMP comparator function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: ACMP comparator configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ConfigComparator(const uint32_t instance, const acmp_comparator_t *config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;
    ACMP_Type *base = s_acmpBase[instance];
    IRQn_Type irq = s_acmpIRQ[instance];

    ACMP_SetInterruptEnableFlag(base, config->interruptEn);
    ACMP_SetTriggerMode(base, config->outputTrigger);
    ACMP_SetOutputSelection(base, config->outputSelect);
    ACMP_SetFilterEnableFlag(base, config->filterEnable);
    ACMP_SetFilterLength(base, config->filterSampleCount);
    ACMP_SetFilterClockPrescaler(base, config->clockDivide);
    ACMP_SetLowPassFilter(base, config->lpfBandwidth);
    ACMP_SetInvertMode(base, config->inverterEnable);
    ACMP_SetOutputPinEnableFlag(base, config->pinState);
    ACMP_SetHysteresisMode(base, config->hysteresisMode);
    ACMP_SetHysteresisVoltage(base, config->hysteresisLevel);
    /* Before changing clock source, should disable the ACMP power first */
    ACMP_SetUsingLSIEnableFlag(base, config->usingLSIEnable);
    ACMP_SetLPEnableFlag(config->usingLSIEnable);
    ACMP_SetWindowMode(base, config->windowModeEnable);
    ACMP_DRV_SetCallback(instance, config->callback);
    ACMP_SetEnableFlag(base, config->powerEnable);

    /* Config ACMP interrupt */
    if (config->interruptEn)
    {
        NVIC_EnableIRQ(irq);
    }
    else
    {
        NVIC_DisableIRQ(irq);
        NVIC_ClearPendingIRQ(irq);
    }

    return status;
}

/*!
 * @brief Get the current ACMP comparator configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: ACMP comparator configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetComparatorConfig(const uint32_t instance, acmp_comparator_t *config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;
    const ACMP_Type *base = s_acmpBase[instance];

    config->interruptEn = ACMP_GetInterruptEnableFlag(base);
    config->outputTrigger = ACMP_GetTriggerMode(base);
    config->outputSelect = ACMP_GetOutputSelection(base);
    config->filterEnable = ACMP_GetFilterEnableFlag(base);
    config->filterSampleCount = ACMP_GetFilterLength(base);
    config->clockDivide = ACMP_GetFilterClockPrescaler(base);
    config->lpfBandwidth = ACMP_GetLowPassFilter(base);
    config->inverterEnable = ACMP_GetInvertMode(base);
    config->pinState = ACMP_GetOutputPinEnableFlag(base);
    config->hysteresisMode = ACMP_GetHysteresisMode(base);
    config->hysteresisLevel = ACMP_GetHysteresisVoltage(base);
    config->usingLSIEnable = ACMP_GetUsingLSIEnableFlag(base);
    config->windowModeEnable = ACMP_GetWindowMode(base);
    config->callback = s_acmpCallback[instance];
    config->powerEnable = ACMP_GetEnableFlag(base);

    return status;
}

/*!
 * @brief Get reset configuration of registers related to DAC.
 *
 * @param[out] config: DAC configuration struce
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitConfigDAC(acmp_dac_t *config)
{
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;

    config->voltageReferenceSource = ACMP_DAC_BANDGAP;
    config->voltage                = 0U;
    config->state                  = false;

    return status;
}

/*!
 * @brief Configure all DAC function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: DAC configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ConfigDAC(const uint32_t instance, const acmp_dac_t *config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;
    ACMP_Type *base = s_acmpBase[instance];

    ACMP_SetDACReference(base, config->voltageReferenceSource);
    ACMP_SetDACOutput(base, config->voltage);
    ACMP_SetDACEnableFlag(base, config->state);

    return status;
}

/*!
 * @brief Get the current DAC configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: DAC configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetDACConfig(const uint32_t instance, acmp_dac_t * const config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;
    const ACMP_Type *base = s_acmpBase[instance];

    config->voltageReferenceSource = ACMP_GetDACReference(base);
    config->voltage                = ACMP_GetDACOutput(base);
    config->state                  = ACMP_GetDACEnableFlag(base);

    return status;
}

/*!
 * @brief Get reset configuration of registers related to input mux.
 *
 * @param[out] config: ACMP intpu mux configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitConfigMUX(acmp_anmux_t *config)
{
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;

    config->positiveInputMux = ACMP_EXTERNAL_CH0;
    config->negativeInputMux = ACMP_EXTERNAL_CH0;

    return status;
}

/*!
 * @brief Configure all input mux function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: ACMP input mux configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ConfigMUX(const uint32_t instance, const acmp_anmux_t *config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;
    ACMP_Type *base = s_acmpBase[instance];

    ACMP_SetPositiveInputPin(base, config->positiveInputMux);
    ACMP_SetNegativeInputPin(base, config->negativeInputMux);

    return status;
}

/*!
 * @brief Get the current intput mux configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: ACMP input mux configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetMUXConfig(const uint32_t instance, acmp_anmux_t * const config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;
    const ACMP_Type *base = s_acmpBase[instance];

    config->positiveInputMux = ACMP_GetPositiveInputPin(base);
    config->negativeInputMux = ACMP_GetNegativeInputPin(base);

    return status;
}

/*!
 * @brief Get reset configuration of registers related to polling mode.
 *
 * @param[out] config: ACMP polling mode configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitPollingMode(acmp_polling_mode_t *config)
{
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;

    config->pollingClockDivide = ACMP_CLK_DIVIDE_256;
    config->mode               = ACMP_NONE_POLLING;
    config->pollingSequence    = 0U;
    config->hallOutputEnable   = false;
    config->hallAOutputCh      = ACMP_EXTERNAL_CH0;
    config->hallBOutputCh      = ACMP_EXTERNAL_CH0;
    config->hallCOutputCh      = ACMP_EXTERNAL_CH0;

    return status;
}

/*!
 * @brief Configure all polling function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: ACMP polling mode configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ConfigPollingMode(const uint32_t instance, const acmp_polling_mode_t *config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;
    ACMP_Type *base = s_acmpBase[instance];

    ACMP_SetPollingFreqDiv(base, config->pollingClockDivide);
    ACMP_SetPollingSequence(base, config->pollingSequence);
    ACMP_SetPollingMode(base, config->mode);
    ACMP_SetHallOutputEnableFlag(base, config->hallOutputEnable);
    ACMP_SetHallOutputA(base, config->hallAOutputCh);
    ACMP_SetHallOutputB(base, config->hallBOutputCh);
    ACMP_SetHallOutputC(base, config->hallCOutputCh);

    return status;
}

/*!
 * @brief Get the current polling mode configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: ACMP polling mode configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetPollingModeConfig(const uint32_t instance, acmp_polling_mode_t * const config)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    status_t status = STATUS_SUCCESS;
    const ACMP_Type *base = s_acmpBase[instance];

    config->pollingClockDivide = ACMP_GetPollingFreqDiv(base);
    config->mode               = ACMP_GetPollingMode(base);
    config->pollingSequence    = ACMP_GetPollingSequence(base);
    config->hallOutputEnable   = ACMP_GetHallOutputEnableFlag(base);
    config->hallAOutputCh      = ACMP_GetHallOutputA(base);
    config->hallBOutputCh      = ACMP_GetHallOutputB(base);
    config->hallCOutputCh      = ACMP_GetHallOutputC(base);

    return status;
}

/*!
 * @brief Get ACMP normal mode output data.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] flags: output data
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetOutputData(const uint32_t instance, uint8_t *flags)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(flags != NULL);

    status_t status = STATUS_SUCCESS;
    const ACMP_Type *base = s_acmpBase[instance];

    *flags = ACMP_GetOutputData(base);

    return status;
}

/*!
 * @brief Get ACMP output status flags.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] flags: output status flags
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetOutputFlags(const uint32_t instance, uint8_t *flags)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(flags != NULL);

    status_t status = STATUS_SUCCESS;
    const ACMP_Type *base = s_acmpBase[instance];

    *flags = ACMP_GetOutputStatus(base);

    return status;
}

/*!
 * @brief Clear ACMP output flags status.
 *
 * @param[in] instance: ACMP instance number
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ClearOutputFlags(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);

    status_t status = STATUS_SUCCESS;
    ACMP_Type *base = s_acmpBase[instance];

    ACMP_ClearOutputStatus(base);

    return status;
}

/*!
 * @brief Get polling mode compare data.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] flags: polling mode input channel compare data
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInputData(const uint32_t instance, uint16_t *flags)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(flags != NULL);

    status_t status = STATUS_SUCCESS;
    const ACMP_Type *base = s_acmpBase[instance];

    *flags = ACMP_GetPollingData(base);

    return status;
}

/*!
 * @brief Get polling mode status flags.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] flags: polling mode input channel status
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInputFlags(const uint32_t instance, uint16_t *flags)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(flags != NULL);

    status_t status = STATUS_SUCCESS;
    const ACMP_Type *base = s_acmpBase[instance];

    *flags = ACMP_GetPollingStatus(base);

    return status;
}

/*!
 * @brief Clear polling mode status flags.
 *
 * @param[in] instance: ACMP instance number
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ClearInputFlags(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);

    status_t status = STATUS_SUCCESS;
    ACMP_Type *base = s_acmpBase[instance];

    ACMP_ClearPollingStatus(base);

    return status;
}

/*!
 * @brief Set ACMP interrutp callback handler.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] func: callback handler function pointer
 * @return none
 */
void ACMP_DRV_SetCallback(const uint32_t instance, const acmp_callback_t func)
{
    DEVICE_ASSERT(instance < ACMP_INSTANCE_MAX);
    DEVICE_ASSERT(func != NULL);

    s_acmpCallback[instance] = func;
}

/*!
 * @brief ACMP interrupt service routine. ACMP interrupt status flags will be cleared here.
 *
 * @param[in] instance: ACMP instance number
 * @return none
 */
void ACMP_CommonISR(uint32_t instance)
{
    ACMP_Type *base = s_acmpBase[instance];
    uint32_t state = 0U;

    /* Get all interrupt status */
    state = base->SR;

    /* Clear all interrtup status */
    base->SR = state;

    /* Call the customized interrtup handler function */
    if (NULL != s_acmpCallback[instance])
    {
        s_acmpCallback[instance](instance, state);
    }
}

/* =============================================  EOF  ============================================== */
