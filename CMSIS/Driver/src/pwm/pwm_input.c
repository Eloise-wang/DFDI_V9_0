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
 * @file pwm_input.c
 *
 * @brief This file provides pwm input capture integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "pwm_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set pwm module work in input capture mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SetInputCaptureMode(const uint8_t instance, const pwm_input_capture_config_t *config)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);
    DEVICE_ASSERT(NULL != config);

    uint8_t i = 0U, pairChannel = 0U;
    pwm_channel_type_t channel;

    PWM_Type *base = g_pwmBase[instance];

    /* Set counting mode */
    PWM_SetCountMode(base, PWM_UP_COUNT);
    PWM_SetChannelInputFilterPsc(base, config->filterPsc);

    /* Configure input capture channels */
    DEVICE_ASSERT(PWM_CHANNEL_MAX > config->channelNum);

    for (i = 0U; i < config->channelNum; i++)
    {
        channel = config->channelConfig[i].channel;

        /* Single edge capture mode channel config */
        if (PWM_INPUTCAP_SINGLE_EDGE == config->channelConfig[i].mode)
        {
            PWM_SetChannelMSR(base, channel, 0U);
            PWM_SetChannelELSR(base, channel, (uint8_t)config->channelConfig[i].detectType);
            PWM_SetCaptureEventPrescaler(base, channel, config->channelConfig[i].eventPsc);
            PWM_SetChannelInterrupt(base, channel, config->channelConfig[i].interruptEn);
            PWM_SetChannelEventReset(base, channel, config->channelConfig[i].chCountRst);
            PWM_SetChannelDmaEnable(base, channel, config->channelConfig[i].chEventDmaEn);
        }
        else /* Dual edge capture mode channel config */
        {
            /* Check the channel is even number */
            DEVICE_ASSERT(0U == ((uint8_t)channel % 2U));

            pairChannel = (uint8_t)(channel) >> 1U;

            PWM_SetPairChannelDualEdgeCapture(base, pairChannel, true);
            PWM_SetPairChannelDecap(base, pairChannel, true);
            PWM_SetChannelMSR(base, channel, (uint8_t)config->channelConfig[i].onceMode);
            PWM_SetChannelMSR(base, (pwm_channel_type_t)(((uint8_t)channel + 1U)), (uint8_t)config->channelConfig[i].onceMode);
            PWM_SetPairMeasureMode(base, pairChannel, config->channelConfig[i].pulseWidthMeasureEn);

            /* Set measure type */
            switch (config->channelConfig[i].measureType)
            {
            case PWM_POSITIVE_PLUSE_WIDTH_MEASURE:
                PWM_SetChannelELSR(base, channel, 1U);
                PWM_SetChannelELSR(base, (pwm_channel_type_t)((uint8_t)channel + 1U), 2U);
                break;

            case PWM_NEGATIVE_PLUSE_WIDTH_MEASURE:
                PWM_SetChannelELSR(base, channel, 2U);
                PWM_SetChannelELSR(base, (pwm_channel_type_t)((uint8_t)channel + 1U), 1U);
                break;

            case PWM_RISING_EDGE_PERIOD_MEASURE:
                PWM_SetChannelELSR(base, channel, 1U);
                PWM_SetChannelELSR(base, (pwm_channel_type_t)((uint8_t)channel + 1U), 1U);
                break;

            case PWM_FALLING_EDGE_PERIOD_MEASURE:
                PWM_SetChannelELSR(base, channel, 2U);
                PWM_SetChannelELSR(base, (pwm_channel_type_t)((uint8_t)channel + 1U), 2U);
                break;

            default:
                break;
            }
            PWM_SetHallMode(base, config->hallEn);
            if (config->hallEn)
            {
                PWM_SetChannelInterrupt(base, (pwm_channel_type_t)((uint8_t)channel), config->channelConfig[i].interruptEn);
                PWM_SetPairChannelDualEdgeCapture(base, 1U, true);
                PWM_SetPairChannelDecap(base, 1U, true);
                PWM_SetChannelEventReset(base, (pwm_channel_type_t)((uint8_t)channel), config->channelConfig[i].chCountRst);
            }
            if (config->channelConfig[i].pulseWidthMeasureEn)
            {
                PWM_SetChannelEventReset(base, (pwm_channel_type_t)((uint8_t)channel), config->channelConfig[i].chCountRst);
            }
            /* Just set channel n+1 interrupt */
            PWM_SetChannelInterrupt(base, (pwm_channel_type_t)(((uint8_t)channel + 1U)), config->channelConfig[i].interruptEn);

            PWM_SetCaptureEventPrescaler(base, (pwm_channel_type_t)((uint8_t)channel + 1U), config->channelConfig[i].eventPsc);
            PWM_SetChannelEventReset(base, (pwm_channel_type_t)((uint8_t)channel + 1U), config->channelConfig[i].chCountRst);
            PWM_SetChannelDmaEnable(base, (pwm_channel_type_t)(((uint8_t)channel + 1U)), config->channelConfig[i].chEventDmaEn);
        }


        /* Enable filtering for input channels */
        DEVICE_ASSERT(PWM_CH_4 > channel);

        if (true == config->channelConfig[i].filterEn)
        {
            PWM_SetChannelInputFilterVal(base, channel, config->channelConfig[i].filterValue);
        }
        else
        {
            PWM_SetChannelInputFilterVal(base, channel, 0U);
        }
    }
}

/*!
 * @brief Set pwm input capture filter psc & filter value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel.
 * @param[in] psc: filter psc(0 ~ 111)
 * @param[in] value: filter value(0~15)
 * @return none
 */
void PWM_DRV_SetChannelInputFilter(const uint8_t instance, pwm_channel_type_t channel,
                                   pwm_channel_input_filter_psc_type_t psc, uint8_t value)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelInputFilterVal(base, channel, value);
    PWM_SetChannelInputFilterPsc(base, psc);
}


/*!
 * @brief Init fault control.
 *
 * Use only in PWM Modulation mode,Called after PWM_Init function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_InitFaultControl(const uint8_t instance, const pwm_fault_config_t *config)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);
    DEVICE_ASSERT(NULL != config);

    PWM_Type *base = g_pwmBase[instance];
    uint8_t i = 0U;

    pwm_fault_channel_type_t channel = PWM_FAULT_CH_0;

    PWM_SetFaultMode(base, config->mode);
    PWM_SetFaultInputFilterValue(base, config->filterValue);
    PWM_SetChannelInputFilterPsc(base, config->filterPsc);
    PWM_SetFaultInterrupt(base, config->interruptEn);
    if (config->interruptEn)
    {
        NVIC_EnableIRQ(s_pwmFaultIrq[instance]);
    }
    else
    {
        NVIC_DisableIRQ(s_pwmFaultIrq[instance]);
    }
    g_pwmFaultCallback[instance] = config->faultCallback;
    PWM_SetFaultHizOutput(base, config->HizEnable);

    for (i = 0U; i < PWM_PAIR_CHANNEL_NUM; i++)
    {
        PWM_SetPairChannelFaultControl(base, i, config->faultCtrlOutputEn[i]);
    }

    for (channel = PWM_FAULT_CH_0; channel < PWM_FAULT_CH_MAX; channel++)
    {
        PWM_SetFaultInputFilter(base, channel, config->channelConfig[channel].faultFilterEn);
        PWM_SetFaultInputPolarity(base, channel, config->channelConfig[channel].faultPolarity);
        PWM_SetFaultInput(base, channel, config->channelConfig[channel].faultInputEn);
    }
}

/*!
 * @brief Set fault input activity.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetFaultInput(const uint8_t instance, pwm_fault_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];
    PWM_SetFaultInput(base, channel ,state);
}

/*!
 * @brief Set fault input filter enable/disable.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetFaultInputFilter(const uint8_t instance, pwm_fault_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];
    PWM_SetFaultInputFilter(base, channel ,state);
}

/*!
 * @brief Set fault input polarity.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 * @param[in] polarity: output polarity
 *            - PWM_INPUT_POLARITY_ACTIVE_HIGH: fault input polarity is active high
 *            - PWM_INPUT_POLARITY_ACTIVE_LOW: fault input polarity is active low
 * @return none
 */
void PWM_DRV_SetFaultInputPolarity(const uint8_t instance, pwm_fault_channel_type_t channel,
                                   pwm_fault_input_polarity_active_type_t polarity)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetFaultInputPolarity(base, channel, polarity);
}

/*!
 * @brief Get the quadrature timer counting direction.
 *
 * @param[in] instance: The pwm instance number
 * @return quadrature counting direction
 */
uint32_t PWM_DRV_GetQuadCountDir(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetQuadCountDir(base);
}

/*!
 * @brief Get the quadrature timer overflow direction.
 *
 * @param[in] instance: The pwm instance number
 * @return quad time overflow direction
 */
uint32_t PWM_DRV_GetQuadOverflowDir(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetQuadOverflowDir(base);
}

/*!
 * @brief Get the phaseZ flag.
 *
 * @param[in] instance: The pwm instance number
 *
 * @return phaseZ Flag
 */
uint32_t PWM_DRV_GetPhaseZFlag(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetPhaseZFlag(base);
}

/*!
 * @brief Clear the phase Z flag.
 *
 * @param[in] instance: The pwm instance number
 *
 * @return none
 */
void PWM_DRV_ClearPhaseZFlag(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_ClearPhaseZFlag(base);
}

/*!
 * @brief Set pwm module work in quadrature decoder mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SetQuadDecodeMode(const uint8_t instance, const pwm_quad_decoder_config_t *config)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);
    DEVICE_ASSERT(NULL != config);

    PWM_Type *base = g_pwmBase[instance];

    /* Set quadrature encoding mode */
    PWM_SetQuadEncodeMode(base, config->mode);
    PWM_SetChannelInputFilterPsc(base, config->filterPsc);
    /* Set polarity for Phase A and Phase B */
    PWM_SetQuadPhaseAPolarity(base, config->phaseAConfig.polarity);
    PWM_SetQuadPhaseBPolarity(base, config->phaseBConfig.polarity);
    PWM_SetQuadPhaseZPolarity(base, config->phaseZConfig.polarity);
    PWM_SetQuadPhaseZResetCnt(base, config->phaseZResetEn);
    /* Set Phase filter */
    if (config->phaseAConfig.filterEn)
    {
        /* phaseA corresponding channel 0 */
        PWM_SetChannelInputFilterVal(base, PWM_CH_0, config->phaseAConfig.filterVal);
    }
    else
    {
        PWM_SetChannelInputFilterVal(base, PWM_CH_0, 0U);
    }

    if (config->phaseBConfig.filterEn)
    {
        /* phaseB corresponding channel 1 */
        PWM_SetChannelInputFilterVal(base, PWM_CH_1, config->phaseBConfig.filterVal);
    }
    else
    {
        PWM_SetChannelInputFilterVal(base, PWM_CH_1, 0U);
    }

    if (config->phaseZConfig.filterEn)
    {
        /* phaseZ corresponding channel 2 */
        PWM_SetChannelInputFilterVal(base, PWM_CH_2, config->phaseZConfig.filterVal);
    }
    else
    {
        PWM_SetChannelInputFilterVal(base, PWM_CH_2, 0U);
    }

    /* Set quadrature decoder mode */
    PWM_SetQuadEncoder(base, config->quadEn);
}

/*!
 * @brief Get fault channel detection flag.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 *            - PWM_FAULT_CH_3
 * @return fault channel detection flag
 */
uint32_t PWM_DRV_GetFaultChannelFlag(const uint8_t instance, pwm_fault_channel_type_t channel)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetFaultChannelFlag(base, (uint32_t)channel);
}

/*!
 * @brief Clear fault channel detection flag.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 *            - PWM_FAULT_CH_3
 * @return none
 */
void PWM_DRV_ClearFaultChannelFlag(const uint8_t instance, pwm_fault_channel_type_t channel)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_ClearFaultChannelFlag(base, (uint32_t)channel);
}

/*!
 * @brief Get the flag about the OR of all fault.
 *
 * @param[in] instance: The pwm instance number
 * @return the flag about the OR of all fault
 */
uint32_t PWM_DRV_GetFaultFlag(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetFaultFlag(base);
}

/*!
 * @brief Clear the flag about the OR of all fault.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_ClearFaultFlag(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_ClearFaultFlag(base);
}

/*!
 * @brief Get pwm hall status.
 *
 * @param[in] instance: The pwm instance number
 * @return hall status
 */
uint32_t PWM_DRV_GetHallStatus(const uint8_t instance)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    return PWM_GetHallStatus(base);
}

/*!
 * @brief Install pwm fault interrupt callback function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] func: The pwm callback function
 * @return none
 */
void PWM_DRV_InstallFaultCallback(const uint8_t instance, const pwm_callback_t func)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    g_pwmFaultCallback[instance] = func;
}

/*!
 * @brief PWM fault common interrupt service routine.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_Fault_IRQHandler(uint8_t instance)
{
    uint32_t lpara = 0U;
    uint32_t faultFlag;

    if (PWM_DRV_GetFaultChannelFlag(instance, PWM_FAULT_CH_0)) ///Check Interrupt flag
    {
        lpara |= (1U << (uint32_t)PWM_FAULT_CH_0);
    }

    if (PWM_DRV_GetFaultChannelFlag(instance, PWM_FAULT_CH_1)) ///Check Interrupt flag
    {
        lpara |= (1U << (uint32_t)PWM_FAULT_CH_1);
    }

    if (PWM_DRV_GetFaultChannelFlag(instance, PWM_FAULT_CH_2))
    {
        lpara |= (1U << (uint32_t)PWM_FAULT_CH_2);
    }

    if (PWM_DRV_GetFaultChannelFlag(instance, PWM_FAULT_CH_3))
    {
        lpara |= (1U << (uint32_t)PWM_FAULT_CH_3);
    }
    /* store channel status and clear */
    faultFlag = PWM_DRV_GetFaultFlag(instance);
    if (faultFlag)
    {
        /* clear fault flag will affect the output behavior, so it is oprated by the user*/
    }
    if (NULL != g_pwmFaultCallback[instance])
    {
        /* callback */
        g_pwmFaultCallback[instance](instance, faultFlag, &lpara);
    }
}

/* =============================================  EOF  ============================================== */
