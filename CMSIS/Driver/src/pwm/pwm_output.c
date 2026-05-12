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
 * @file pwm_output.c
 *
 * @brief This file provides pwm output integration functions.
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
 * @brief Set pwm module work in pluse width modulation mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SetPWMMode(const uint8_t instance, const pwm_modulation_config_t *config)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);
    DEVICE_ASSERT(NULL != config);

    uint8_t i = 0U, pairChannel = 0U;
    pwm_channel_type_t channel;
    PWM_Type *base = g_pwmBase[instance];
    uint32_t channelDitherValue[2U] = {0U, 0U};

    /* Initialize vaiables */
    for (i = 0U; i < 2U; i++)
    {
        channelDitherValue[i] = 0U;
    }

    /* Set counting mode */
    PWM_SetCountMode(base, config->countMode);

    /* Configure independent channels */
    DEVICE_ASSERT(PWM_CHANNEL_MAX > config->independentChannelNum);

    for (i = 0U; i < config->independentChannelNum; i++)
    {
        channel = config->independentChConfig[i].channel;
        PWM_SetChannelOutputInitLevel(base, channel, config->independentChConfig[i].initLevel);
    }

    DEVICE_ASSERT(PWM_PAIR_CHANNEL_NUM > config->combineChannelNum);

    for (i = 0U; i < config->combineChannelNum; i++)
    {
        channel = config->combineChConfig[i].pairChannel;
        PWM_SetChannelOutputInitLevel(base, channel, config->combineChConfig[i].ch1stInitLevel);
        PWM_SetChannelOutputInitLevel(base, (pwm_channel_type_t)((uint8_t)channel + 1U),
                                      config->combineChConfig[i].ch2ndInitLevel);
    }
    PWM_InitChannelOutputEn(base, config->initChOutputEn);

    for (i = 0U; i < config->independentChannelNum; i++)
    {
        channel = config->independentChConfig[i].channel;
        PWM_SetChannelMSR(base, channel, 2U);
        PWM_SetChannelELSR(base, channel, (uint8_t)config->independentChConfig[i].levelMode);
        PWM_SetChannelCountValue(base, channel, config->independentChConfig[i].chValue);
        channelDitherValue[(uint8_t)channel / 4U] |= ((config->independentChConfig[i].chDitherValue \
                                                      & PWM_DITHER0_C0DHR_Msk) \
                                                      << (((uint8_t)channel % 4U) * PWM_DITHER0_C1DHR_Pos));
        PWM_SetChannelPolarity(base, channel, config->independentChConfig[i].polarity);
        PWM_SetChannelInterrupt(base, channel, config->independentChConfig[i].interruptEn);
        PWM_SetChannelMatchTrigger(base, channel, config->independentChConfig[i].matchTriggerEn);
        PWM_SetChannelDmaEnable(base, channel, config->independentChConfig[i].chEventDmaEn);
    }

    /* Configure combined channels */
    DEVICE_ASSERT(PWM_PAIR_CHANNEL_NUM > config->combineChannelNum);

    for (i = 0U; i < config->combineChannelNum; i++)
    {
        channel = config->combineChConfig[i].pairChannel;

        /* Check the channel is even number */
        DEVICE_ASSERT(0U == ((uint8_t)channel % 2U));

        pairChannel = (uint8_t)(channel) >> 1U;

        PWM_SetPairChannelCombine(base, pairChannel, true);
        PWM_SetPairChannelDeadtime(base, pairChannel, config->combineChConfig[i].deadtimeEn);
        PWM_SetPairChannelComplement(base, pairChannel, config->combineChConfig[i].complementEn);
        PWM_SetPairChannelSymmetric(base, pairChannel, config->combineChConfig[i].symmetricEn);

        PWM_SetDeadtime(base, pairChannel, config->combineChConfig[i].deadtimePsc,
                        config->combineChConfig[i].deadtime);

        PWM_SetChannelELSR(base, channel, (uint8_t)config->combineChConfig[i].levelMode);

        PWM_SetChannelCountValue(base, channel, config->combineChConfig[i].ch1stValue);
        PWM_SetChannelCountValue(base, (pwm_channel_type_t)((uint8_t)channel + 1U),
                                 config->combineChConfig[i].ch2ndValue);

        channelDitherValue[(uint8_t)channel / 4U] |= ((config->combineChConfig[i].ch1stDitherValue \
                                                      & PWM_DITHER0_C0DHR_Msk) \
                                                      << (((uint8_t)channel % 4U) * PWM_DITHER0_C1DHR_Pos));
        channelDitherValue[(uint8_t)channel / 4U] |= ((config->combineChConfig[i].ch2ndDitherValue \
                                                      & PWM_DITHER0_C0DHR_Msk) \
                                                      << ((((uint8_t)channel % 4U) + 1U) * PWM_DITHER0_C1DHR_Pos));

        PWM_SetChannelMatchDir(base, channel, config->combineChConfig[i].ch1stMatchDir);
        PWM_SetChannelMatchDir(base, (pwm_channel_type_t)((uint8_t)channel + 1U),
                               config->combineChConfig[i].ch2ndMatchDir);

        PWM_SetChannelPolarity(base, channel, config->combineChConfig[i].ch1stPolarity);
        PWM_SetChannelPolarity(base, (pwm_channel_type_t)((uint8_t)channel + 1U),
                               config->combineChConfig[i].ch2ndPolarity);

        PWM_SetChannelInterrupt(base, channel, config->combineChConfig[i].ch1stInterruptEn);
        PWM_SetChannelInterrupt(base, (pwm_channel_type_t)((uint8_t)channel + 1U),
                                config->combineChConfig[i].ch2ndInterruptEn);

        PWM_SetChannelMatchTrigger(base, channel, config->combineChConfig[i].ch1stMatchTriggerEn);
        PWM_SetChannelMatchTrigger(base, (pwm_channel_type_t)((uint8_t)channel + 1U),
                                   config->combineChConfig[i].ch2ndMatchTriggerEn);

        PWM_SetChannelDmaEnable(base, channel, config->combineChConfig[i].ch1stEventDmaEn);
        PWM_SetChannelDmaEnable(base, (pwm_channel_type_t)((uint8_t)channel + 1U),
                                config->combineChConfig[i].ch2ndEventDmaEn);
    }
    PWM_SetChannelCounterDitherValue(base, 0U, channelDitherValue[0U]);
    PWM_SetChannelCounterDitherValue(base, 1U, channelDitherValue[1U]);
    /* Set init output , the initialization feature must be used only with disabled counter*/
    PWM_SetTriggerRatio(base, config->triggerRatio);
    PWM_SetMaxTrigger(base, config->maxTriggerEn);
    PWM_SetInitTrigger(base, config->initTriggerEn);
    PWM_SetCenterAlignDutyType(base, config->centerAlignDutyType);
}

/*!
 * @brief Set pwm module work in output compare mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SetOutputCompareMode(const uint8_t instance, const pwm_output_compare_config_t *config)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);
    DEVICE_ASSERT(NULL != config);

    uint8_t i = 0U;
    pwm_channel_type_t channel;
    PWM_Type *base = g_pwmBase[instance];

    /* Set counting mode */
    PWM_SetCountMode(base, PWM_UP_COUNT);

    /* Configure output compare channels */
    DEVICE_ASSERT(PWM_CHANNEL_MAX > config->channelNum);
    for (i = 0U; i < config->channelNum; i++)
    {
        channel = config->channelConfig[i].channel;
        PWM_SetChannelOutputInitLevel(base, channel, config->channelConfig[i].initLevel);
    }
    PWM_InitChannelOutputEn(base, config->initChOutputEn);

    for (i = 0U; i < config->channelNum; i++)
    {
        channel = config->channelConfig[i].channel;

        PWM_SetChannelMSR(base, channel, 1U);
        PWM_SetChannelELSR(base, channel, (uint8_t)config->channelConfig[i].mode);
        PWM_SetChannelCountValue(base, channel, config->channelConfig[i].comparedValue);
        PWM_SetChannelInterrupt(base, channel, config->channelConfig[i].interruptEn);
        PWM_SetChannelPolarity(base, channel, config->channelConfig[i].polarity);
        PWM_SetChannelMatchTrigger(base, channel, config->channelConfig[i].matchTriggerEn);
        PWM_SetChannelDmaEnable(base, channel, config->channelConfig[i].chEventDmaEn);
    }
    PWM_SetTriggerRatio(base, config->triggerRatio);
    PWM_SetMaxTrigger(base, config->maxTriggerEn);
    PWM_SetInitTrigger(base, config->initTriggerEn);
}

/*!
 * @brief Set channel output mask.
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
void PWM_DRV_SetChannelOutputMask(const uint8_t instance, pwm_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelOutputMask(base, (uint32_t)channel, state);
}

/*!
 * @brief Set all channel output mask.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] mask: channel mask
 *            - 0 ~ 0xff
 * @return none
 */
void PWM_DRV_SetAllChannelOutputMask(const uint8_t instance, uint32_t mask)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetAllChannelOutputMask(base, mask);
}

/*!
 * @brief Set the channel Hiz Output.
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
void PWM_DRV_SetChannelHizOutput(const uint8_t instance, pwm_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelHizOutput(base, channel, state);
}

/*!
 * @brief Set channel polarity.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] polarity: output polarity
 *            - PWM_OUTPUT_POLARITY_ACTIVE_HIGH: active low
 *            - PWM_OUTPUT_POLARITY_ACTIVE_LOW: active high
 * @return none
 */
void PWM_DRV_SetChannelPolarity(const uint8_t instance, pwm_channel_type_t channel,
                                pwm_output_polarity_active_type_t polarity)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelPolarity(base, channel, polarity);
}

/*!
 * @brief Set channel software control function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * * @return none
 */
void PWM_DRV_SetChannelSoftControl(const uint8_t instance, pwm_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelSoftControl(base, (uint32_t)channel, state);
}

/*!
 * @brief Software control channel output level.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] level: output level
 *            - PWM_LOW_LEVEL: output low level
 *            - PWM_HIGH_LEVEL: output high level
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SoftControlChannelLevel(const uint8_t instance, pwm_channel_type_t channel,
                                     pwm_output_state_t level, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SoftControlChannelLevel(base, channel, level, state);
}

/*!
 * @brief Software control all channel output level.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] mask: channel mask
 *            - 0 ~ 0xff
 * @param[in] level: all 8 channel output state
 *            - 0 ~ 0xff
 * @return none
 */
void PWM_DRV_SoftControlAllChannelLevel(const uint8_t instance, uint8_t mask, uint8_t level)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SoftControlAllChannelLevel(base, mask, level);
}
/*!
 * @brief Set channel inverting control.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetPairChannelInvert(const uint8_t instance, uint8_t pairChannel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetPairChannelInvert(base, pairChannel, state);
}

/*!
 * @brief Set deadtime psc & value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] pairChannel: pwm pair channel(0~3)
 * @param[in] psc: prescaler divider
 *            - PWM_DEADTIME_DIVID_1
 *            - PWM_DEADTIME_DIVID_4
 *            - PWM_DEADTIME_DIVID_16
 * @param[in] value: inserted value
 *            - 0 ~ 63
 * @return none
 */
void PWM_DRV_SetDeadtime(const uint8_t instance, uint8_t pairChannel,
                         pwm_dead_time_psc_type_t psc, uint16_t value)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetDeadtime(base, pairChannel, psc, value);
}

/*!
 * @brief Set channel0 ~ channel3 dither value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] ditherRegNum
 *            - 0: set dither0 regiter
 *            - 1: set dither1 regiter
 * @param[in] channelDitherValue: value for all 4 channel dither value
 * @return none
 */
void PWM_DRV_SetChannelCounterDitherValue(const uint8_t instance, uint8_t ditherRegNum, uint32_t channelDitherValue)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelCounterDitherValue(base, ditherRegNum, channelDitherValue);
}

/*!
 * @brief Set mod dither value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] maxCountDitherValue: mod dither value
 *            - 0 ~ 31
 * @return none
 */
void PWM_DRV_SetMaxCountDitherValue(const uint8_t instance, uint8_t maxCountDitherValue)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetMaxCountDitherValue(base, maxCountDitherValue);
}

/*!
 * @brief Set the initialization trigger source.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetInitTrigger(const uint8_t instance, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];
    PWM_SetInitTrigger(base, state);
}

/*!
 * @brief Set the max trigger source.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetMaxTrigger(const uint8_t instance, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];
    PWM_SetMaxTrigger(base, state);
}

/*!
 * @brief Set the match trigger source.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetMatchTrigger(const uint8_t instance, pwm_channel_type_t channel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetChannelMatchTrigger(base, channel, state);
}

/*!
 * @brief Set the max/match/init trigger ratio.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] triggerRatio: 0 ~ 7
 * @return none
 */
void PWM_DRV_SetTriggerRatio(const uint8_t instance, uint8_t triggerRatio)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetTriggerRatio(base, triggerRatio);
}

/*!
 * @brief Set pair channel symmetric.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetPairChannelSymmetric(const uint8_t instance, uint8_t pairChannel, bool state)
{
    DEVICE_ASSERT(PWM_INSTANCE_MAX > instance);

    PWM_Type *base = g_pwmBase[instance];

    PWM_SetPairChannelSymmetric(base, pairChannel, state);
}

/* =============================================  EOF  ============================================== */
