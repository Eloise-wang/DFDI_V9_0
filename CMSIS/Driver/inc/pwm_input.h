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
 * @file pwm_input.h
 *
 * @brief This file provides pwm input integration functions.
 *
 */

#ifndef _PWM_INPUT_H
#define _PWM_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "pwm_common.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief PWM input capture edge mode enumeration.
 */
typedef enum
{
    PWM_INPUTCAP_SINGLE_EDGE = 0U,      /*!< Signle edge capture mode */
    PWM_INPUTCAP_DUAL_EDGE              /*!< Dual edge capture mode */
} pwm_input_capture_edge_mode_t;

/*!
 * @brief PWM input capture single edge detect enumeration.
 */
typedef enum
{
    PWM_RISING_EDGE_DETECT = 1U,    /*!< Rising edge detect */
    PWM_FALLING_EDGE_DETECT,        /*!< Falling Edge detect */
    PWM_BOTH_EDGE_DETECT            /*!< Rising or falling edge detect */
} pwm_input_single_edge_detect_type_t;

/*!
 * @brief PWM input capture dual edge measure enumeration.
 */
typedef enum
{
    PWM_POSITIVE_PLUSE_WIDTH_MEASURE = 0U,      /*!< Positive Pluse width measurement */
    PWM_NEGATIVE_PLUSE_WIDTH_MEASURE,           /*!< Negative Pluse width measurement */
    PWM_RISING_EDGE_PERIOD_MEASURE,             /*!< Period measurement between two consecutive rising edges */
    PWM_FALLING_EDGE_PERIOD_MEASURE             /*!< Period measurement between two consecutive falling edges */
} pwm_input_dual_edge_measure_type_t;

/*!
 * @brief PWM input capture mode enumeration.
 */
typedef enum
{
    PWM_INPUTCAP_ONESHOT = 0U,          /*!< Dual edge capture with one shot mode */
    PWM_INPUTCAP_CONTINUOUS             /*!< Dual edge capture with continuous mode */
} pwm_dual_once_mode_type_t;

/*!
 * @brief PWM input capture event prescaler enumeration.
 */
typedef enum
{
    PWM_EVENT_PSC_1 = 0U,   /*!< 1 event trigger input capture */
    PWM_EVENT_PSC_2,        /*!< 2 event trigger input capture */
    PWM_EVENT_PSC_4,        /*!< 4 event trigger input capture */
    PWM_EVENT_PSC_8         /*!< 8 event trigger input capture */
} pwm_input_event_psc_type_t;

/*!
 * @brief PWM input capture event prescaler enumeration.
 */
typedef enum
{
    PWM_CHANNEL_INPUT_FILTER_PS_1 = 0U,     /*!< Capture filter psc is 1 */
    PWM_CHANNEL_INPUT_FILTER_PS_2,          /*!< Capture filter psc is 2 */
    PWM_CHANNEL_INPUT_FILTER_PS_4,          /*!< Capture filter psc is 4 */
    PWM_CHANNEL_INPUT_FILTER_PS_8,          /*!< Capture filter psc is 8 */
    PWM_CHANNEL_INPUT_FILTER_PS_16,         /*!< Capture filter psc is 16 */
    PWM_CHANNEL_INPUT_FILTER_PS_32,         /*!< Capture filter psc is 32 */
    PWM_CHANNEL_INPUT_FILTER_PS_64,         /*!< Capture filter psc is 64 */
    PWM_CHANNEL_INPUT_FILTER_PS_128,        /*!< Capture filter psc is 128 */
    PWM_CHANNEL_INPUT_FILTER_PS_256,        /*!< Capture filter psc is 256 */
    PWM_CHANNEL_INPUT_FILTER_PS_1024,       /*!< Capture filter psc is 1024 */
    PWM_CHANNEL_INPUT_FILTER_PS_2048,       /*!< Capture filter psc is 2048 */
    PWM_CHANNEL_INPUT_FILTER_PS_4096        /*!< Capture filter psc is 4096 */
} pwm_channel_input_filter_psc_type_t;

/*!
 * @brief PWM fault channel input polarity active enumeration.
 */
typedef enum
{
    PWM_INPUT_POLARITY_ACTIVE_HIGH = 0U,    /*!< The fault channel polarity is active high */
    PWM_INPUT_POLARITY_ACTIVE_LOW           /*!< The fault channel polarity is active low */
} pwm_fault_input_polarity_active_type_t;

/*!
 * @brief PWM Input capture channel configuration structure.
 */
typedef struct
{
    pwm_channel_type_t channel;                       /*!< Channel number */
    pwm_input_capture_edge_mode_t mode;               /*!< Input channel operation mode */
    /* single mode parameters */
    pwm_input_single_edge_detect_type_t detectType;   /*!< In single mode, channel edge detect type */
    pwm_input_event_psc_type_t eventPsc;              /*!< Event prescaler */
    /* dual mode parameters */
    pwm_input_dual_edge_measure_type_t measureType;   /*!< In dual mode, channel measure type */
    pwm_dual_once_mode_type_t onceMode;               /*!< Enable/disable Continuous mode */
    /* General parameters */
    bool filterEn;                                    /*!< Enable/disable Input capture filter */
    uint8_t filterValue;                              /*!< Filter value */
    bool interruptEn;                                 /*!< Enable/disable channel interrupt */
    bool pulseWidthMeasureEn;                         /*!< Enable/disable pulseWidth measurement */
    bool chEventDmaEn;                                /*!< Enable/disable channel event dma request*/
    bool chCountRst;                                  /*!< Enable/disable channel counter reset when input capture*/
} pwm_input_ch_config_type_t;

/*!
 * @brief PWM input capture mode configuration structure.
 */
typedef struct
{
    uint8_t channelNum;                             /*!< Number of output compare channels */
    pwm_channel_input_filter_psc_type_t filterPsc;  /*!< Filter prescaler */
    bool hallEn;                                    /*!< Hall enable */
    pwm_input_ch_config_type_t *channelConfig;      /*!< Input capture channels configuration */
} pwm_input_capture_config_t;

/*!
 * @brief PWM channel fault control mode enumeration.
 */
typedef enum
{
    PWM_FAULT_CTRL_NONE  = 0U,      /*!< No Fault control */
    PWM_FAULT_CTRL_MANUAL_EVEN,     /*!< Fault control is enabled for even channels and manual fault clearing */
    PWM_FAULT_CTRL_MANUAL_ALL,      /*!< Fault control is enabled for all channels and manual fault clearing */
    PWM_FAULT_CTRL_AUTO             /*!< Fault control is enabled for all channels and automatic fault clearing */
} pwm_fault_ctrl_mode_t;

/*!
 * @brief PWM Fault channel configuration structure.
 */
typedef struct
{
    bool faultInputEn;                                      /*!< Fault input channel state */
    bool faultFilterEn;                                     /*!< Fault channel filter state */
    pwm_fault_input_polarity_active_type_t faultPolarity;   /*!< Fault channel input polarity active */
} pwm_fault_ch_config_t;

/*!
 * @brief PWM Fault configuration structure.
 */
typedef struct
{
    pwm_fault_ctrl_mode_t mode;                                  /*!< Fault mode */
    pwm_fault_ch_config_t channelConfig[PWM_FAULT_CH_MAX];       /*!< Fault input channels configuration */
    uint8_t filterValue;                                         /*!< Fault filter value */
    pwm_channel_input_filter_psc_type_t filterPsc;               /*!< Fault filter psc */
    bool faultCtrlOutputEn[PWM_PAIR_CHANNEL_NUM];                /*!< Fault control channel output state */
    bool interruptEn;                                            /*!< Enable PWM fault interrupt */
    bool HizEnable;                                              /*!< Enable PWM fault Hiz Output */
    pwm_callback_t faultCallback;
} pwm_fault_config_t;

/*!
 * @brief PWM quadrature decode mode enumeration.
 */
typedef enum
{
    PWM_QUAD_PHASE_ENCODE = 0U, /*!< Phase encoding mode */
    PWM_QUAD_COUNT_DIR          /*!< Counter and direction encoding mode */
} pwm_quad_decoder_mode_t;

/*!
 * @brief PWM quadrature phase polarity enumeration.
 */
typedef enum
{
    PWM_QUAD_PHASE_NORMAL = 0U,   /*!< Phase input signal is not inverted */
    PWM_QUAD_PHASE_INVERT         /*!< Phase input signal is inverted */
} pwm_quad_phase_polarity_type_t;

/*!
 * @brief PWM quadrature decoder phase input configuration structure.
 *
 */
typedef struct
{
    pwm_quad_phase_polarity_type_t polarity;     /*!< PhaseA Polarity */
    bool filterEn;                               /*!< Filter Enable */
    uint8_t filterVal;                           /*!< Filter value */
} pwm_quad_phase_config_t;

/*!
 * @brief PWM quadrature configuration structure.
 */
typedef struct
{
    pwm_quad_decoder_mode_t mode;                    /*!< PWM quadrature decode mode */
    pwm_quad_phase_config_t phaseAConfig;            /*!< PhaseA config */
    pwm_quad_phase_config_t phaseBConfig;            /*!< PhaseB config */
    pwm_quad_phase_config_t phaseZConfig;            /*!< PhaseZ config */
    bool quadEn;                                     /*!< Enable quadrature decode mode */
    pwm_channel_input_filter_psc_type_t filterPsc;   /*!< Input Capture Filter psc */
    bool phaseZResetEn;
} pwm_quad_decoder_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Set pwm module work in input capture mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SetInputCaptureMode(const uint8_t instance, const pwm_input_capture_config_t *config);

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
                                   pwm_channel_input_filter_psc_type_t psc, uint8_t value);

/*!
 * @brief Init fault control.
 *
 * Use only in PWM Modulation mode,Called after PWM_Init function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_InitFaultControl(const uint8_t instance, const pwm_fault_config_t *config);

/*!
 * @brief Set fault input activity.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SetFaultInput(const uint8_t instance, pwm_fault_channel_type_t channel, bool state);

/*!
 * @brief Set fault input filter enable/disable.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 * @param[in] state: enabling state
 *            - ENABLE
 *            - DISABLE
 * @return none
 */
void PWM_DRV_SetFaultInputFilter(const uint8_t instance, pwm_fault_channel_type_t channel, bool state);

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
                                   pwm_fault_input_polarity_active_type_t polarity);

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
uint32_t PWM_DRV_GetFaultChannelFlag(const uint8_t instance, pwm_fault_channel_type_t channel);

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
void PWM_DRV_ClearFaultChannelFlag(const uint8_t instance, pwm_fault_channel_type_t channel);

/*!
 * @brief Get the flag about the OR of all fault.
 *
 * @param[in] instance: The pwm instance number
 * @return the flag about the OR of all fault
 */
uint32_t PWM_DRV_GetFaultFlag(const uint8_t instance);

/*!
 * @brief Clear the flag about the OR of all fault.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_ClearFaultFlag(const uint8_t instance);

/*!
 * @brief Get the quadrature timer counting direction.
 *
 * @param[in] instance: The pwm instance number
 * @return quadrature counting direction
 */
uint32_t PWM_DRV_GetQuadCountDir(const uint8_t instance);

/*!
 * @brief Get the quadrature timer overflow direction.
 *
 * @param[in] instance: The pwm instance number
 * @return quad time overflow direction flag
 */
uint32_t PWM_DRV_GetQuadOverflowDir(const uint8_t instance);

/*!
 * @brief Get the phaseZ flag.
 *
 * @param[in] instance: The pwm instance number
 *
 * @return phaseZ Flag
 */
uint32_t PWM_DRV_GetPhaseZFlag(const uint8_t instance);

/*!
 * @brief Clear the phase Z flag.
 *
 * @param[in] instance: The pwm instance number
 *
 * @return none
 */
void PWM_DRV_ClearPhaseZFlag(const uint8_t instance);

/*!
 * @brief Set pwm module work in quadrature decoder mode.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SetQuadDecodeMode(const uint8_t instance, const pwm_quad_decoder_config_t *config);

/*!
 * @brief Get pwm hall status.
 *
 * @param[in] instance: The pwm instance number
 * @return hall status
 */
uint32_t PWM_DRV_GetHallStatus(const uint8_t instance);

/*!
 * @brief Install pwm fault interrupt callback function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] func: The pwm callback function
 * @return none
 */
void PWM_DRV_InstallFaultCallback(const uint8_t instance, const pwm_callback_t func);

/*!
 * @brief PWM fault common interrupt service routine.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_Fault_IRQHandler(uint8_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PWM_INPUT_H */

/* =============================================  EOF  ============================================== */
