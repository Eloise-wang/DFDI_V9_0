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
 * @file pwm_output.h
 *
 * @brief This file provides pwm output hardware integration functions.
 *
 */

#ifndef _PWM_OUTPUT_H
#define _PWM_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "pwm_common.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief PWM center align duty type enumeration.
 */
typedef enum
{
    PWM_DUTY_MODE_0 = 0U,   /*!< CnV=Cn+1V=CNTIN,output 0% duty cycle;CnV=Cn+1V=MOD,output 100% duty cycle */
    PWM_DUTY_MODE_1         /*!< CnV=Cn+1V=CNTIN,output 100% duty cycle;CnV=Cn+1V=MOD,output 0% duty cycle */
} pwm_center_align_duty_type_t;

/*!
 * @brief PWM independent channel configuration structure.
 */
typedef struct
{
    pwm_channel_type_t channel;                  /*!< Independent channel number */
    uint16_t chValue;                            /*!< Channel value */
    uint8_t chDitherValue;                       /*!< Channel dither value for match value*/
    pwm_output_level_mode_t levelMode;           /*!< Output level mode */
    pwm_output_polarity_active_type_t polarity;  /*!< Polarity of the channel output */
    bool interruptEn;                            /*!< Enable/disable channel interrupt */
    pwm_output_state_t initLevel;                /*!< Channel init output level */
    bool matchTriggerEn;                         /*!< Enable/disable channel trigger */
    bool chEventDmaEn;                           /*!< Enable/disable DMA request for channel event */
} pwm_independent_ch_config_t;

/*!
 * @brief PWM combine channel configuration structure.
 */
typedef struct
{
    pwm_channel_type_t pairChannel;                   /*!< Pair of channel number (PWM_CH_0/2/4/6) */
    uint16_t ch1stValue;                              /*!< Channel n channel value */
    uint16_t ch2ndValue;                              /*!< Channel n+1 channel value */
    uint8_t ch1stDitherValue;                         /*!< Channel n channel dither value */
    uint8_t ch2ndDitherValue;                         /*!< Channel n+1 channel dither value */
    pwm_output_level_mode_t levelMode;                /*!< Output level mode */
    bool deadtimeEn;                                  /*!< Enable/disable dead-time insertion */
    bool complementEn;                                /*!< Enable/disable complementary output */
    bool symmetricEn;                                 /*!< Enable/disable symmetric */
    pwm_match_point_dir_type_t ch1stMatchDir;         /*!< Channel n Match point direction, It only active on the up-down counting mode */
    pwm_match_point_dir_type_t ch2ndMatchDir;         /*!< Channel n+1 Match point direction, It only active on the up-down counting mode */
    pwm_output_polarity_active_type_t ch1stPolarity;  /*!< Channel n the polarity of the channel output */
    pwm_output_polarity_active_type_t ch2ndPolarity;  /*!< Channel n+1 the polarity of the channel output */
    bool ch1stInterruptEn;                            /*!< Enable/disable channel n interrupt */
    bool ch2ndInterruptEn;                            /*!< Enable/disable channel n+1 interrupt */
    pwm_output_state_t ch1stInitLevel;                /*!< Channel n init output level */
    pwm_output_state_t ch2ndInitLevel;                /*!< Channel n+1 init output level */
    bool ch1stMatchTriggerEn;                         /*!< Enable/disable channel n trigger */
    bool ch2ndMatchTriggerEn;                         /*!< Enable/disable channel n+1 trigger */
    bool ch1stEventDmaEn;                             /*!< Enable/disable DMA request for channel n event */
    bool ch2ndEventDmaEn;                             /*!< Enable/disable DMA request for channel n+1 event */
    uint16_t deadtime;                                /*!< Dead time value  */
    pwm_dead_time_psc_type_t deadtimePsc;             /*!< Dead time prescaler value */
} pwm_combine_ch_config_t;

/*!
 * @brief PWM Modulation configuration structure.
 */
typedef struct
{
    pwm_count_mode_t countMode;                         /*!< Counting mode */
    uint8_t independentChannelNum;                      /*!< Number of independent channels (0~7) */
    uint8_t combineChannelNum;                          /*!< Number of combine channels (0~3) */
    uint8_t triggerRatio;                               /*!< Trigger ratio for init/max/match trigger */
    pwm_independent_ch_config_t *independentChConfig;   /*!< Configuration for independent channels */
    pwm_combine_ch_config_t *combineChConfig;           /*!< Configuration for combined channels */
    bool initChOutputEn;                                /*!< Enable/disable Initial channel output */
    bool initTriggerEn;                                 /*!< Enable/disable Initial trigger */
    bool maxTriggerEn;                                  /*!< Enable/disable max trigger */
    pwm_center_align_duty_type_t centerAlignDutyType;   /*!< Center-align duty type */
} pwm_modulation_config_t;

/*!
 * @brief PWM output compare mode enumeration.
 */
typedef enum
{
    PWM_NONE_OUTPUT = 0U,   /*!< No output */
    PWM_TOGGLE_OUTPUT,      /*!< Toggle output */
    PWM_CLEAR_OUTPUT,       /*!< Clear output */
    PWM_SET_OUTPUT          /*!< Set output */
} pwm_output_compare_mode_t;

/*!
 * @brief PWM output compare channel configuration structure.
 */
typedef struct
{
    pwm_channel_type_t channel;                  /*!< Channel number */
    uint16_t comparedValue;                      /*!< Compared value */
    pwm_output_compare_mode_t mode;              /*!< Output compare mode */
    bool interruptEn;                            /*!< Enable/disable channel interrupt */
    pwm_output_polarity_active_type_t polarity;  /*!< Polarity of the channel output */
    pwm_output_state_t initLevel;                /*!< Channel init output level */
    bool matchTriggerEn;                         /*!< Enable/disable channel trigger */
    bool chEventDmaEn;                           /*!< Enable/disable DMA request for channel event */
} pwm_output_compare_ch_config_t;

/*!
 * @brief PWM output compare configuration structure.
 */
typedef struct
{
    uint8_t channelNum;                             /*!< Number of output compare channels */
    uint8_t triggerRatio;                           /*!< Trigger ratio for init/max/match trigger */
    pwm_output_compare_ch_config_t *channelConfig;  /*!< Output compare channels configuration */
    bool initChOutputEn;                            /*!< Enable/disable Initial channel output */
    bool initTriggerEn;                             /*!< Enable/disable Initial trigger */
    bool maxTriggerEn;                              /*!< Enable/disable max trigger */
} pwm_output_compare_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Set pwm module work in pluse width modulation mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SetPWMMode(const uint8_t instance, const pwm_modulation_config_t *config);

/*!
 * @brief Set pwm module work in output compare mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SetOutputCompareMode(const uint8_t instance, const pwm_output_compare_config_t *config);

/*!
 * @brief Set channel output mask.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SetChannelOutputMask(const uint8_t instance, pwm_channel_type_t channel, bool state);

/*!
 * @brief Set all channel output mask.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] mask: channel mask
 *            - 0 ~ 0xff
 * @return none
 */
void PWM_DRV_SetAllChannelOutputMask(const uint8_t instance, uint32_t mask);

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
void PWM_DRV_SetChannelHizOutput(const uint8_t instance, pwm_channel_type_t channel, bool state);

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
                                pwm_output_polarity_active_type_t polarity);

/*!
 * @brief Set channel software control function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * * @return none
 */
void PWM_DRV_SetChannelSoftControl(const uint8_t instance, pwm_channel_type_t channel, bool state);

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
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SoftControlChannelLevel(const uint8_t instance, pwm_channel_type_t channel,
                                     pwm_output_state_t level, bool state);

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
void PWM_DRV_SoftControlAllChannelLevel(const uint8_t instance, uint8_t mask, uint8_t level);

/*!
 * @brief Set channel inverting control.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SetPairChannelInvert(const uint8_t instance, uint8_t pairChannel, bool state);

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
                         pwm_dead_time_psc_type_t psc, uint16_t value);

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
void PWM_DRV_SetChannelCounterDitherValue(const uint8_t instance, uint8_t ditherRegNum, uint32_t channelDitherValue);

/*!
 * @brief Set mod dither value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] maxCountDitherValue: mod dither value
 *            - 0 ~ 31
 * @return none
 */
void PWM_DRV_SetMaxCountDitherValue(const uint8_t instance, uint8_t maxCountDitherValue);

/*!
 * @brief Set the initialization trigger source.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SetInitTrigger(const uint8_t instance, bool state);

/*!
 * @brief Set the max trigger source.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SetMaxTrigger(const uint8_t instance, bool state);

/*!
 * @brief Set the match trigger source.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SetMatchTrigger(const uint8_t instance, pwm_channel_type_t channel, bool state);

/*!
 * @brief Set the max/match/init trigger ratio.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] triggerRatio: 0 ~ 7
 * @return none
 */
void PWM_DRV_SetTriggerRatio(const uint8_t instance, uint8_t triggerRatio);

/*!
 * @brief Set pair channel symmetric.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SetPairChannelSymmetric(const uint8_t instance, uint8_t pairChannel, bool state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PWM_OUTPUT_H */

/* =============================================  EOF  ============================================== */
