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
 * @file pwm_common.c
 *
 * @brief This file provides pwm common integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "pwm_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* PWM overflow interrupt request info */
const IRQn_Type s_pwmOverflowIrq[PWM_INSTANCE_MAX] = PWM_OVERFLOW_IRQS;

/* PWM channel interrupt request info */
const IRQn_Type s_pwmChannelIrq[PWM_INSTANCE_MAX] = PWM_CHANNEL_IRQS;

/* PWM fault interrupt request info */
const IRQn_Type s_pwmFaultIrq[PWM_INSTANCE_MAX] = PWM_FAULT_IRQS;

/* PWM ckgen clk info */
const ckgen_clock_t s_pwmCkgenClks[PWM_INSTANCE_MAX] = PWM_CKGEN_CLOCKS;
/* PWM soft reset info */
const ckgen_softreset_t s_pwmSoftReset[PWM_INSTANCE_MAX] = PWM_SOFT_RESETS;

/* PWM overflow callback pointer */
pwm_callback_t g_pwmOverflowCallback[PWM_INSTANCE_MAX] = {NULL};
/* PWM channel callback pointer */
pwm_callback_t g_pwmChannelCallback[PWM_INSTANCE_MAX] = {NULL};
/* PWM fault callback pointer */
pwm_callback_t g_pwmFaultCallback[PWM_INSTANCE_MAX] = {NULL};

/* PWM base address */
PWM_Type * const g_pwmBase[PWM_INSTANCE_MAX] = PWM_BASE_PTRS;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief PWM initialize.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_Init(const uint8_t instance, const pwm_config_t *config)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);
    DEVICE_ASSERT(NULL != config);

    PWM_Type *base = g_pwmBase[instance];

    /* Enable clock */
    CKGEN_DRV_Enable(s_pwmCkgenClks[instance], true);
    CKGEN_DRV_SoftReset(s_pwmSoftReset[instance], true);

    if (instance % 2U)
    {
        CKGEN_DRV_Enable(s_pwmCkgenClks[instance - 1U], true);
        CKGEN_DRV_SoftReset(s_pwmSoftReset[instance - 1U], true);
    }

    /* Set period value */
    PWM_SetClockPrescaler(base, config->clkPsc);
    PWM_SetCounterInitValue(base, config->initValue);
    PWM_SetCounter(base, config->initValue);
    PWM_SetMaxCountValue(base, config->maxValue);
    PWM_SetMaxCountDitherValue(base, config->modDitherValue);

    /* Register callback function */
    g_pwmOverflowCallback[instance] = config->overflowCallback;
    g_pwmChannelCallback[instance] = config->channelCallback;
    PWM_SetCNTOFFrequence(base, config->cntOverflowFreq);

    /* PWM DMA Setting */
    PWM_SetOverflowDmaEnable(base, config->overflowDmaEn);
    PWM_SetDmaLenEn(base, config->dmaLghEn);
    PWM_SetDmaTransferLen(base, config->dmaLgh);

    /* Set interrupt */
    PWM_SetOverflowInterrupt(base, config->overflowInterrupEn);
    PWM_SetOverflowEventEnable(base, config->overflowEventEn);
    PWM_SetUnderflowEventEnable(base, config->underflowEventEn);
    if (true == config->interruptEn)
    {
        NVIC_EnableIRQ(s_pwmOverflowIrq[instance]);
        NVIC_EnableIRQ(s_pwmChannelIrq[instance]);
    }
    else
    {
        NVIC_DisableIRQ(s_pwmOverflowIrq[instance]);
        NVIC_EnableIRQ(s_pwmChannelIrq[instance]);
    }

    /* Initialize according to different mode */
    switch (config->mode)
    {
    case PWM_MODE_MODULATION:
        PWM_DRV_SetPWMMode(instance, config->initModeStruct);
        break;

    case PWM_MODE_INPUT_CAPTURE:
        PWM_DRV_SetInputCaptureMode(instance, config->initModeStruct);
        break;

    case PWM_MODE_OUTPUT_COMPARE:
        PWM_DRV_SetOutputCompareMode(instance, config->initModeStruct);
        break;

    case PWM_MODE_QUADRATURE_DECODER:
        PWM_DRV_SetQuadDecodeMode(instance, config->initModeStruct);
        break;

    case PWM_MODE_GENERAL_TIMER:
        /* Do nothing */
        break;

    default:
        break;
    }

    /* Set clock source causes the counter to start working,If you don't want to start immediately after initialization
    you can disable it by passing PWM_CLK_SOURCE_NONE parameters */
    PWM_SetClockSource(base, config->clkSource);
}

/*!
 * @brief PWM ckgen/irq disable & module reset.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_Deinit(const uint8_t instance)
{
    CKGEN_DRV_Enable(s_pwmCkgenClks[instance], false);
    CKGEN_DRV_SoftReset(s_pwmSoftReset[instance], false);

    NVIC_DisableIRQ(s_pwmOverflowIrq[instance]);
    NVIC_DisableIRQ(s_pwmChannelIrq[instance]);
    NVIC_DisableIRQ(s_pwmFaultIrq[instance]);
}

/*!
 * @brief Set pwm clock source.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] source: clock source
 *            - PWM_CLK_SOURCE_NONE: No clock selected,in effect disables the PWM counter
 *            - PWM_CLK_SOURCE_SYSTEM: system clock
 *            - PWM_CLK_SOURCE_FIXED_FREQUENCY: fixed frequency clock
 *            - PWM_CLK_SOURCE_EXTERNAL: external clock
 * @return none
 */
void PWM_DRV_SetClockSource(const uint8_t instance, pwm_clk_source_t source)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetClockSource(base, source);
}

/*!
 * @brief PWM simply init.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SimplyInit(const uint8_t instance, const pwm_simply_config_t *config)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);
    DEVICE_ASSERT(NULL != config);

    PWM_Type *base = g_pwmBase[instance];
    uint8_t i = 0U;
    uint16_t channelDitherValue[2U] = {0U};

    /* Enable clock */
    CKGEN_DRV_Enable(s_pwmCkgenClks[instance], true);
    CKGEN_DRV_SoftReset(s_pwmSoftReset[instance], true);

    if (instance % 2U)
    {
        CKGEN_DRV_Enable(s_pwmCkgenClks[instance - 1U], true);
        CKGEN_DRV_SoftReset(s_pwmSoftReset[instance - 1U], true);
    }

    for (i = 0U; i < PWM_CHANNEL_MAX; i++)
    {
        if (i % 2U)
        {
            /* odd channel */
            PWM_SetChannelOutputInitLevel(base, (pwm_channel_type_t)(i), config->oddInitLevel);
        }
        else
        {
            /* even channel */
            PWM_SetChannelOutputInitLevel(base, (pwm_channel_type_t)(i), config->evenInitLevel);
        }
    }

    /* Set init output , the initialization feature must be used only with disabled counter*/
    PWM_InitChannelOutputEn(base, config->initChOutputEn);

    /* Set period value */
    PWM_SetClockPrescaler(base, config->clkPsc);
    PWM_SetCounterInitValue(base, config->initValue);
    PWM_SetCounter(base, config->initValue);
    PWM_SetMaxCountValue(base, config->maxValue);
    PWM_SetMaxCountDitherValue(base, config->modDitherValue);

    /* Set counting mode */
    PWM_SetCountMode(base, config->countMode);

    /* Set channel  */
    for (i = 0U; i < PWM_CHANNEL_MAX / 2; i++)
    {
        if (PWM_COMBINE_MODE == config->allChCombineMode)
        {
            /* Combine channel mode */
            PWM_SetPairChannelCombine(base, i, true);
            PWM_SetPairChannelDeadtime(base, i, true);
        }
        else
        {
            /* Independent channel mode */
            PWM_SetPairChannelCombine(base, i, false);
            PWM_SetPairChannelDeadtime(base, i, false);
        }

        PWM_SetPairChannelComplement(base, i, config->complementEn);

        /* Set Deadtime*/
        PWM_SetDeadtime(base, i, config->deadtimePsc, config->deadtime);
    }

    for (i = 0U; i < PWM_CHANNEL_MAX; i++)
    {
        PWM_SetChannelELSR(base, (pwm_channel_type_t)(i), (uint8_t)config->levelMode);
        PWM_SetChannelMSR(base, (pwm_channel_type_t)(i), 2U);
        PWM_SetChannelCountValue(base, (pwm_channel_type_t)(i), config->chValue[i]);
        channelDitherValue[i / 4U] |= ((config->chDitherValue[i] & PWM_DITHER0_C0DHR_Msk) << ((i % 4U) * PWM_DITHER0_C1DHR_Pos));
        PWM_SetChannelInterrupt(base, (pwm_channel_type_t)(i), config->interruptEn);
        if (i % 2U)
        {
            /* odd channel */
            PWM_SetChannelPolarity(base, (pwm_channel_type_t)(i), config->oddPolarity);
        }
        else
        {
            /* even channel */
            PWM_SetChannelPolarity(base, (pwm_channel_type_t)(i), config->evenPolarity);
        }
    }
    PWM_SetChannelCounterDitherValue(base, 0U, channelDitherValue[0U]);
    PWM_SetChannelCounterDitherValue(base, 1U, channelDitherValue[1U]);

    /* Register callback function */
    g_pwmOverflowCallback[instance] = config->overflowCallback;
    g_pwmChannelCallback[instance] = config->channelCallback;

    /* Set interrupt */
    PWM_SetOverflowInterrupt(base, config->overflowInterrupEn);
    PWM_SetOverflowEventEnable(base, config->overflowEventEn);
    PWM_SetUnderflowEventEnable(base, config->underflowEventEn);
    /* Set interrupt */
    if (true == config->interruptEn)
    {
        NVIC_EnableIRQ(s_pwmOverflowIrq[instance]);
        NVIC_EnableIRQ(s_pwmChannelIrq[instance]);
    }
    else
    {
        NVIC_DisableIRQ(s_pwmOverflowIrq[instance]);
        NVIC_DisableIRQ(s_pwmChannelIrq[instance]);
    }

    /* Set clock source causes the counter to start working,If you don't want to start immediately after initialization
    you can disable it by passing PWM_CLK_SOURCE_NONE parameters */
    PWM_SetClockSource(base, config->clkSource);
}

/*!
 * @brief Set CnV/CNTIN/MOD sync bypass.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetSyncBYPEN(const uint8_t instance, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetSyncBYPEN(base, state);
}

/*!
 * @brief PWM synchronization control init.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_InitSyncControl(const uint8_t instance, const pwm_sync_config_t *config)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);
    DEVICE_ASSERT(NULL != config);

    PWM_Type *base = g_pwmBase[instance];
    uint8_t i = 0U;

    /* Set synchronization trigger mode */
    if (PWM_SYNC_TRIGGER_SOFTWARE == config->syncTriggerMode)
    {
        PWM_SetMCVRSyncTriggerMode(base, PWM_SYNC_TRIGGER_SOFTWARE, true);
        PWM_SetOMCRSyncTriggerMode(base, PWM_SYNC_TRIGGER_SOFTWARE, true);
        PWM_SetINVCRSyncTriggerMode(base, PWM_SYNC_TRIGGER_SOFTWARE, true);
        PWM_SetCHOSWCRSyncTriggerMode(base, PWM_SYNC_TRIGGER_SOFTWARE, true);
        PWM_SetPOLTriggerMode(base, PWM_SYNC_TRIGGER_SOFTWARE, true);
    }
    else /* hardware trigger mode*/
    {
        PWM_SetMCVRSyncTriggerMode(base, PWM_SYNC_TRIGGER_HARDWARE, true);
        PWM_SetOMCRSyncTriggerMode(base, PWM_SYNC_TRIGGER_HARDWARE, true);
        PWM_SetINVCRSyncTriggerMode(base, PWM_SYNC_TRIGGER_HARDWARE, true);
        PWM_SetCHOSWCRSyncTriggerMode(base, PWM_SYNC_TRIGGER_HARDWARE, true);
        PWM_SetPOLTriggerMode(base, PWM_SYNC_TRIGGER_HARDWARE, true);
    }

    PWM_SetCNTINSync(base, config->counterInitSyncEn);
    PWM_SetCHOSWCRSync(base, config->outCtrlSyncEn);
    PWM_SetINVCRSync(base, config->inverterSyncEn);
    PWM_SetPOLSync(base, config->polaritySyncEn);
    PWM_EnableOutputMaskSync(base, config->outmaskSyncEn);

    for (i = 0U; i < PWM_PAIR_CHANNEL_NUM; i++)
    {
        PWM_SetPairChannelCounterSync(base, i, config->chValueSyncEn[i]);
    }

    if ((false == config->maxLoadingPointEn) && (false == config->minLoadingPointEn))
    {
        /* update immediately when no load point is set */
        PWM_SetCNTSyncTriggerMode(base, config->syncTriggerMode, true);
        PWM_SetMaxLoadingPoint(base, false);
        PWM_SetMinLoadingPoint(base, false);
    }
    else
    {
        PWM_SetCNTSyncTriggerMode(base, config->syncTriggerMode, false);
        PWM_SetMaxLoadingPoint(base, config->maxLoadingPointEn);
        PWM_SetMinLoadingPoint(base, config->minLoadingPointEn);
    }

    PWM_SetHardwareTriggerSyncSrc(base, 0U, config->hardwareSync0En);
    PWM_SetHardwareTriggerSyncSrc(base, 1U, config->hardwareSync1En);
    PWM_SetHardwareTriggerSyncSrc(base, 2U, config->hardwareSync2En);
    PWM_SetHwTriggerSyncAutoClear(base, config->autoClearHWTriggerEn);

    PWM_SetSyncType(base, 0U);
    PWM_SyncEnable(base, config->pwmSyncEnable);
    PWM_SetSyncBYPEN(base, config->syncBypassEn);

    PWM_SetSyncMode(base, PWM_SYNC_MODE_ENHANCED);
}

/*!
 * @brief Enable/Disable global timer base.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: true/false
 * @return none
 */
void PWM_DRV_SetGlobalTimeBase(const uint8_t instance, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetGlobalTimeBase(base, state);
}

/*!
 * @brief Global timer base output enable/disable.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: true/false
 * @return none
 */
void PWM_DRV_SetGlobalTimeBaseOutput(const uint8_t instance, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetGlobalTimeBaseOutput(base, state);
}

/*!
 * @brief Set pwm channel counter value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel num.
 * @param[in] value: channel counter value
 * @return none
 */
void PWM_DRV_SetChannelCountValue(const uint8_t instance, pwm_channel_type_t channel, uint16_t value)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelCountValue(base, channel, value);
}

/*!
 * @brief Get channel counting value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @return channel counting value
 */
uint32_t PWM_DRV_GetChannelCountValue(const uint8_t instance, pwm_channel_type_t channel)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetChannelCountValue(base, channel);
}


/*!
 * @brief Set pwm counter value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] value: counter value
 * @return none
 */
void PWM_DRV_SetCounter(const uint8_t instance, uint16_t value)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetCounter(base, value);
}

/*!
 * @brief Get pwm counter value.
 *
 * @param[in] instance: The pwm instance number
 * @return counter value
 */
uint32_t PWM_DRV_GetCounter(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetCounter(base);
}

/*!
 * @brief Set pwm modulus value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] value: modulus value
 * @return none
 */
void PWM_DRV_SetMaxCountValue(const uint8_t instance, uint16_t value)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetMaxCountValue(base, value);
}

/*!
 * @brief Get pwm modulus value.
 *
 * @param[in] instance: The pwm instance number
 * @return modulus value
 */
uint32_t PWM_DRV_GetMaxCountValue(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetMaxCountValue(base);
}

/*!
 * @brief Set pwm counter initial value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] value: counter initial value
 * @return none
 */
void PWM_DRV_SetCounterInitValue(const uint8_t instance, uint16_t value)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetCounterInitValue(base, value);
}

/*!
 * @brief Get pwm counter initial value.
 *
 * @param[in] instance: The pwm instance number
 * @return counter initial value
 */
uint32_t PWM_DRV_GetCounterInitValue(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetCounterInitValue(base);
}

/*!
 * @brief Get all channel event flag.
 *
 * @param[in] instance: The pwm instance number
 * @return all channel Flag
 */
uint32_t PWM_DRV_GetAllChannelEventFlag(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetAllChannelEventFlag(base);
}

/*!
 * @brief Clear all channel event flag.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] mask: channel mask.
 * @return none
 */
void PWM_DRV_ClearAllChannelEventFlag(const uint8_t instance, uint32_t mask)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_ClearAllChannelEventFlag(base, mask);
}

/*!
 * @brief Get all channel input status.
 *
 * @param[in] instance: The pwm instance number
 * @return all channel input status
 */
uint32_t PWM_DRV_GetAllChannelInputStatus(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetAllChannelInputStatus(base);
}

/*!
 * @brief Get pwm overflow & underflow flag.
 *
 * @param[in] instance: The pwm instance number
 * @return pwm overflow & underflow flag
 */
uint32_t PWM_DRV_GetOverflowFlag(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetOverflowFlag(base);
}

/*!
 * @brief Check pwm counter is overflow or underflow.
 *
 * @param[in] instance: The pwm instance number
 * @return pwm overflow & underflow flag
 */
uint32_t PWM_DRV_GetOverflowDir(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetOverflowDir(base);
}

/*!
 * @brief Clear overflow & underflow flag.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_ClearOverflowFlag(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_ClearOverflowFlag(base);
}

/*!
 * @brief Set software trigger synchronization.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetSoftwareTriggerSync(const uint8_t instance, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetSoftwareTriggerSync(base, state);
}

/*!
 * @brief Set the channel event reset.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetChannelEventReset(const uint8_t instance, pwm_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelEventReset(base, channel, state);
}

/*!
 * @brief Set the channel interrupt.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetChannelInterrupt(const uint8_t instance, pwm_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelInterrupt(base, channel, state);
}

/*!
 * @brief Set the channel dma enable.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetChannelDmaEnable(const uint8_t instance, pwm_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelDmaEnable(base, channel, state);
}

/*!
 * @brief Set pwm enter debug mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] mode: pwm debug mode
 *            - PWM_DEBUG_MODE_NO_EFFECT
 *            - PWM_DEBUG_MODE_COUNTER_STOPPED_OUTPUT_PREVIOUS
 *            - PWM_DEBUG_MODE_COUNTER_STOPPED_OUTPUT_HIGH
 * @return none
 */
void PWM_DRV_SetDebugMode(const uint8_t instance, pwm_debug_mode_t mode)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetDebugMode(base, mode);
}

/*!
 * @brief Set the write protection.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetWriteProtection(const uint8_t instance, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetWriteProtection(base, state);
}
/*!
 * @brief Install pwm overflow & underflow interrupt callback function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] func: The pwm callback function.
 * @return none
 */
void PWM_DRV_InstallOverflowCallback(const uint8_t instance, const pwm_callback_t func)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    g_pwmOverflowCallback[instance] = func;
}

/*!
 * @brief Install pwm channel interrupt callback function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] func: The pwm callback function.
 * @return none
 */
void PWM_DRV_InstallChannelCallback(const uint8_t instance, const pwm_callback_t func)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    g_pwmChannelCallback[instance] = func;
}

/*!
 * @brief PWM overflow common interrupt service routine.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_Overflow_IRQHandler(uint8_t instance)
{
    uint32_t wpara = 0U, lpara = 0U;

    /* store overflow status */
    wpara = PWM_DRV_GetOverflowFlag(instance);
    if (wpara)
    {
        /* clear overflow status */
        PWM_DRV_ClearOverflowFlag(instance);
    }
    lpara = PWM_DRV_GetOverflowDir(instance);
    if (NULL != g_pwmOverflowCallback[instance])
    {
        /* callback */
        g_pwmOverflowCallback[instance](instance, wpara, &lpara);
    }
}

/*!
 * @brief PWM channel common interrupt service routine.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_Channel_IRQHandler(uint8_t instance)
{
    uint32_t wpara = 0U, lpara = 0U;

    /* store channel status and clear */
    wpara = PWM_DRV_GetAllChannelEventFlag(instance);
    if (wpara & PWM_STR_CH_EVENT_Msk)
    {
        PWM_DRV_ClearAllChannelEventFlag(instance, wpara);
    }
    lpara = PWM_DRV_GetAllChannelInputStatus(instance);
    if (NULL != g_pwmChannelCallback[instance])
    {
        /* callback */
        g_pwmChannelCallback[instance](instance, wpara, &lpara);
    }
}

/* =============================================  EOF  ============================================== */
