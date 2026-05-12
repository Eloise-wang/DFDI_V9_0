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
 * @file pwm_common.h
 *
 * @brief This file provides pwm common integration functions.
 *
 */

#ifndef _PWM_COMMON_H
#define _PWM_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */
/*!< Number of pair channels */
#define PWM_PAIR_CHANNEL_NUM            (4UL)

/*!< The maximum of counter */
#define PWM_MAX_COUNTER_VALUE           (0xFFFFUL)

/* ===========================================  Typedef  ============================================ */
/*!< Callback for pwm module */
typedef void (*pwm_callback_t)(uint8_t instance, uint32_t status, void *userData);

/*!
 * @brief PWM clock source enumeration.
 */
typedef enum
{
    PWM_CLK_SOURCE_NONE = 0U,             /*!< No clock selected, in effect disables the counter */
    PWM_CLK_SOURCE_SYSTEM,                /*!< System clock */
    PWM_CLK_SOURCE_FIXED_FREQUENCY,       /*!< Fixed frequency clock */
    PWM_CLK_SOURCE_EXTERNAL               /*!< External clock */
} pwm_clk_source_t;

/*!
 * @brief PWM channel enumeration.
 */
typedef enum
{
    PWM_CH_0 = 0U,      /*!< PWM channel 0 */
    PWM_CH_1,           /*!< PWM channel 1 */
    PWM_CH_2,           /*!< PWM channel 2 */
    PWM_CH_3,           /*!< PWM channel 3 */
    PWM_CH_4,           /*!< PWM channel 4 */
    PWM_CH_5,           /*!< PWM channel 5 */
    PWM_CH_6,           /*!< PWM channel 6 */
    PWM_CH_7,           /*!< PWM channel 7 */
    PWM_CH_MAX          /*!< Invalid channel */
} pwm_channel_type_t;

/*!
* @brief PWM operation mode enumeration.
*/
typedef enum
{
    PWM_MODE_MODULATION = 0U,       /*!< Pulse width modulation module mode */
    PWM_MODE_INPUT_CAPTURE,         /*!< Input capture mode */
    PWM_MODE_OUTPUT_COMPARE,        /*!< Output compare mode */
    PWM_MODE_QUADRATURE_DECODER,    /*!< Quadrature decoder mode */
    PWM_MODE_GENERAL_TIMER          /*!< General timer mode */
} pwm_operate_mode_t;

/*!
 * @brief PWM channel combine enumeration.
 */
typedef enum
{
    PWM_INDEPENDENT_MODE = 0U,  /*!< Independent channel mode */
    PWM_COMBINE_MODE            /*!< Combine channel mode */
} pwm_channel_combine_type_t;

/*!
 * @brief PWM counting mode enumeration.
 */
typedef enum
{
    PWM_UP_COUNT = 0U,         /*!< Up counting mode */
    PWM_UP_DOWN_COUNT          /*!< Up-down counting mode */
} pwm_count_mode_t;

/*!
* @brief PWM prescaler factor for the dead-time insertion enumeration.
*/
typedef enum
{
    PWM_DEADTIME_DIVID_1 = 1U,  /*!< Divide by 1 */
    PWM_DEADTIME_DIVID_4,       /*!< Divide by 4 */
    PWM_DEADTIME_DIVID_16       /*!< Divide by 16 */
} pwm_dead_time_psc_type_t;

/*!
 * @brief PWM output level enumeration.
 */
typedef enum
{
    PWM_LOW_LEVEL = 0U,     /*!< LOW LEVEL */
    PWM_HIGH_LEVEL          /*!< HIGH LEVEL */
} pwm_output_state_t;

/*!
 * @brief PWM output level enumeration.
 */
typedef enum
{
    PWM_HIGH_TRUE = 2U,    /*!< High true */
    PWM_LOW_TRUE           /*!< Low true */
} pwm_output_level_mode_t;

/*!
 * @brief PWM channel match point counting direction in combine mode enumeration.
 */
typedef enum
{
    PWM_MATCH_DIR_DOWN = 0U,    /*!< Channel match point in down counting direction */
    PWM_MATCH_DIR_UP            /*!< Channel match point in up counting direction */
} pwm_match_point_dir_type_t;

/*!
* @brief PWM channel output polarity active enumeration.
*/
typedef enum
{
    PWM_OUTPUT_POLARITY_ACTIVE_HIGH = 0U,  /*!< The channel output polarity is active high */
    PWM_OUTPUT_POLARITY_ACTIVE_LOW         /*!< The channel output polarity is active low */
} pwm_output_polarity_active_type_t;

/*!
 * @brief PWM DRV Init configuration structure.
 */
typedef struct
{
    pwm_operate_mode_t mode;                  /*!< Select pwm operation mode */
    void *initModeStruct;                     /*!< Pointer to the corresponding initialization structure for different modes */
    pwm_clk_source_t clkSource;               /*!< Select clock source */
    uint16_t clkPsc;                          /*!< Clock prescaler */
    uint16_t initValue;                       /*!< Initial counter value */
    uint16_t maxValue;                        /*!< Maximum counter value */
    uint8_t modDitherValue;                   /*!< Dither value for MOD */
    bool overflowEventEn;                     /*!< Pwm overflow event enable */
    bool underflowEventEn;                    /*!< Pwm underflow event enable */
    bool overflowInterrupEn;                  /*!< Enable/disable CNTOF interrupt */
    bool overflowDmaEn;                       /*!< Enable/disable dma for underflow/overflow event */
    bool dmaLghEn;                            /*!< Enable/disable dma transfer length */
    uint8_t dmaLgh;                           /*!< Dma transfer length for dma reg clearing */
    uint8_t cntOverflowFreq;                  /*!< CNTOF frequency */
    bool interruptEn;                         /*!< Enable/disable pwm interrupt  */
    pwm_callback_t overflowCallback;          /*!< Overflow Callback pointer */
    pwm_callback_t channelCallback;           /*!< Channel Callback pointer */
} pwm_config_t;

/*!
 * @brief PWM simply configuration structure.
 */
typedef struct
{
    pwm_channel_combine_type_t allChCombineMode;     /*!< All Channel combine mode */
    pwm_count_mode_t countMode;                      /*!< Counting mode */
    pwm_output_level_mode_t levelMode;               /*!< Output level mode */
    pwm_clk_source_t clkSource;                      /*!< Select clock source */
    uint16_t clkPsc;                                 /*!< Clock prescaler */
    uint16_t initValue;                              /*!< Initial counter value */
    uint16_t maxValue;                               /*!< Maximum counter value */
    uint8_t modDitherValue;                          /*!< Dither value for MOD */
    uint16_t chValue[PWM_CHANNEL_MAX];               /*!< Channel value */
    uint8_t chDitherValue[PWM_CHANNEL_MAX];          /*!< Channel dither value */
    bool complementEn;                               /*!< Enable/disable complementary output */
    pwm_output_polarity_active_type_t oddPolarity;   /*!< Odd channel the polarity of the channel output */
    pwm_output_polarity_active_type_t evenPolarity;  /*!< Even channel the polarity of the channel output */
    pwm_output_state_t oddInitLevel;                 /*!< Odd channel init output level */
    pwm_output_state_t evenInitLevel;                /*!< Even channel init output level */
    bool initChOutputEn;                             /*!< Enable/disable Initial channel output */
    uint16_t deadtime;                               /*!< Dead time value  */
    pwm_dead_time_psc_type_t deadtimePsc;            /*!< Dead time prescaler value */
    bool interruptEn;                                /*!< Enable/disable pwm interrupt  */
    bool overflowEventEn;                            /*!< Pwm overflow event enable */
    bool underflowEventEn;                           /*!< Pwm underflow event enable */
    bool overflowInterrupEn;                         /*!< Enable/disable CNTOF interrupt */
    pwm_callback_t overflowCallback;                 /*!< Overflow Callback pointer */
    pwm_callback_t channelCallback;                  /*!< Channel Callback pointer */
} pwm_simply_config_t;

/*!
 * @brief PWM sync trigger mode enumeration.
 */
typedef enum
{
    PWM_SYNC_TRIGGER_SOFTWARE = 0U, /*!< Software trigger synchronization */
    PWM_SYNC_TRIGGER_HARDWARE       /*!< Hardware trigger synchronization */
} pwm_sync_trigger_mode_t;

/*!
 * @brief PWM sync mode enumeration.
 */
typedef enum
{
    PWM_SYNC_MODE_LEGACY = 0U,  /*!< Legacy synchronization */
    PWM_SYNC_MODE_ENHANCED      /*!< Enhanced synchronization */
} pwm_sync_mode_t;

/*!
 * @brief PWM sync configuration structure.
 * Please don't use software and hardware trigger simultaneously
 */
typedef struct
{
    pwm_sync_trigger_mode_t syncTriggerMode; /*!< Synchronization trigger mode */
    bool hardwareSync0En;                    /*!< Enable/disable hardware sync trigger source 0 */
    bool hardwareSync1En;                    /*!< Enable/disable hardware sync trigger source 1 */
    bool hardwareSync2En;                    /*!< Enable/disable hardware sync trigger source 2 */
    bool autoClearHWTriggerEn;               /*!< Available only for hardware trigger */
    bool counterInitSyncEn;                  /*!< Enable/disable CNTIN sync */
    bool outCtrlSyncEn;                      /*!< Enable/disable CHOSWCR sync */
    bool inverterSyncEn;                     /*!< Enable/disable INVCR sync */
    bool outmaskSyncEn;                      /*!< Enable/disable OMCR sync */
    bool polaritySyncEn;                     /*!< Enable/disable CHOPOLCR sync */
    bool chValueSyncEn[PWM_PAIR_CHANNEL_NUM];/*!< Enable/disable dual channel CHV sync */
    bool maxLoadingPointEn;                  /*!< Enable/disable maximum loading point */
    bool minLoadingPointEn;                  /*!< Enable/disable minimum loading point */
    bool syncBypassEn;                       /*!< Enable/disable Synchronization bypass */
    bool pwmSyncEnable;                      /*!< PWM Enable/disable */
} pwm_sync_config_t;

/*!
 * @brief PWM sync mode enumeration.
 */
typedef enum
{
    PWM_DEBUG_MODE_NO_EFFECT = 0U,                      /*!< No effect for counter, channel output, CNTIN/MOD/CnV/Cn+1V buffer and registers */
    PWM_DEBUG_MODE_COUNTER_STOPPED_OUTPUT_PREVIOUS,     /*!< Counter stopped, channel output remains previous value, write CNTIN/MOD/CnV/Cn+1V buffer and immediately update to registersn */
    PWM_DEBUG_MODE_COUNTER_STOPPED_OUTPUT_HIGH          /*!< Counter stopped, channel output remains high resistance value, write CNTIN/MOD/CnV/Cn+1V buffer and immediately update to registersn */
} pwm_debug_mode_t;

/*!
 * @brief PWM fault input channel enumeration.
 */
typedef enum
{
    PWM_FAULT_CH_0 = 0U,      /*!< PWM fault input channel 0 */
    PWM_FAULT_CH_1,           /*!< PWM fault input channel 1 */
    PWM_FAULT_CH_2,           /*!< PWM fault input channel 2 */
    PWM_FAULT_CH_3,           /*!< PWM fault input channel 3 */
    PWM_FAULT_CH_MAX          /*!< Invalid fault input channel */
} pwm_fault_channel_type_t;

/* ==========================================  Variables  =========================================== */
/*!< PWM base address */
extern PWM_Type * const g_pwmBase[PWM_INSTANCE_MAX];

/*!< PWM overflow interrupt request info */
extern const IRQn_Type s_pwmOverflowIrq[PWM_INSTANCE_MAX];
/*!< PWM channel interrupt request info */
extern const IRQn_Type s_pwmChannelIrq[PWM_INSTANCE_MAX];
/*!< PWM fault interrupt request info */
extern const IRQn_Type s_pwmFaultIrq[PWM_INSTANCE_MAX];

/*!< PWM ckgen clk info */
extern const ckgen_clock_t s_pwmCkgenClks[PWM_INSTANCE_MAX];
/*!< PWM soft reset info */
extern const ckgen_softreset_t s_pwmSoftReset[PWM_INSTANCE_MAX];

/*!< PWM overflow callback pointer */
extern pwm_callback_t g_pwmOverflowCallback[PWM_INSTANCE_MAX];
/*!< PWM channel callback pointer */
extern pwm_callback_t g_pwmChannelCallback[PWM_INSTANCE_MAX];
/*!< PWM fault callback pointer */
extern pwm_callback_t g_pwmFaultCallback[PWM_INSTANCE_MAX];

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief PWM initialize.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_Init(const uint8_t instance, const pwm_config_t *config);

/*!
 * @brief PWM ckgen/irq disable & module reset.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_Deinit(const uint8_t instance);

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
void PWM_DRV_SetClockSource(const uint8_t instance, pwm_clk_source_t source);

/*!
 * @brief PWM simply init.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_SimplyInit(const uint8_t instance, const pwm_simply_config_t *config);

/*!
 * @brief Set CnV/CNTIN/MOD sync bypass.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetSyncBYPEN(const uint8_t instance, bool state);

/*!
 * @brief PWM synchronization Control init.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] config: pointer to configuration structure
 * @return none
 */
void PWM_DRV_InitSyncControl(const uint8_t instance, const pwm_sync_config_t *config);

/*!
 * @brief Enable/Disable global timer base.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: true/false
 * @return none
 */
void PWM_DRV_SetGlobalTimeBase(const uint8_t instance, bool state);

/*!
 * @brief Global timer base output enable/disable.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: true/false
 * @return none
 */
void PWM_DRV_SetGlobalTimeBaseOutput(const uint8_t instance, bool state);

/*!
 * @brief Set pwm channel counter value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] channel: pwm channel num.
 * @param[in] value: channel counter value
 * @return none
 */
void PWM_DRV_SetChannelCountValue(const uint8_t instance, pwm_channel_type_t channel, uint16_t value);

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
uint32_t PWM_DRV_GetChannelCountValue(const uint8_t instance, pwm_channel_type_t channel);

/*!
 * @brief Set pwm counter value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] value: counter value
 * @return none
 */
void PWM_DRV_SetCounter(const uint8_t instance, uint16_t value);

/*!
 * @brief Get pwm counter value.
 *
 * @param[in] instance: The pwm instance number
 * @return counter value
 */
uint32_t PWM_DRV_GetCounter(const uint8_t instance);

/*!
 * @brief Set pwm modulus value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] value: modulus value
 * @return none
 */
void PWM_DRV_SetMaxCountValue(const uint8_t instance, uint16_t value);

/*!
 * @brief Get pwm modulus value.
 *
 * @param[in] instance: The pwm instance number
 * @return modulus value
 */
uint32_t PWM_DRV_GetMaxCountValue(const uint8_t instance);

/*!
 * @brief Set pwm counter initial value.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] value: counter initial value
 * @return none
 */
void PWM_DRV_SetCounterInitValue(const uint8_t instance, uint16_t value);

/*!
 * @brief Get pwm counter initial value.
 *
 * @param[in] instance: The pwm instance number
 * @return counter initial value
 */
uint32_t PWM_DRV_GetCounterInitValue(const uint8_t instance);

/*!
 * @brief Get all channel event flag.
 *
 * @param[in] instance: The pwm instance number
 * @return all channel Flag
 */
uint32_t PWM_DRV_GetAllChannelEventFlag(const uint8_t instance);

/*!
 * @brief Clear all channel event flag.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] mask: channel mask.
 * @return none
 */
void PWM_DRV_ClearAllChannelEventFlag(const uint8_t instance, uint32_t mask);

/*!
 * @brief Get all channel input status.
 *
 * @param[in] instance: The pwm instance number
 * @return all channel input status
 */
uint32_t PWM_DRV_GetAllChannelInputStatus(const uint8_t instance);

/*!
 * @brief Get pwm overflow & underflow flag.
 *
 * @param[in] instance: The pwm instance number
 * @return pwm overflow & underflow flag
 */
uint32_t PWM_DRV_GetOverflowFlag(const uint8_t instance);

/*!
 * @brief Check pwm counter is overflow or underflow.
 *
 * @param[in] instance: The pwm instance number
 * @return pwm overflow & underflow flag
 */
uint32_t PWM_DRV_GetOverflowDir(const uint8_t instance);

/*!
 * @brief Clear overflow & underflow flag.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_ClearOverflowFlag(const uint8_t instance);

/*!
 * @brief Set software trigger synchronization.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetSoftwareTriggerSync(const uint8_t instance, bool state);

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
void PWM_DRV_SetChannelEventReset(const uint8_t instance, pwm_channel_type_t channel, bool state);

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
void PWM_DRV_SetChannelInterrupt(const uint8_t instance, pwm_channel_type_t channel, bool state);

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
void PWM_DRV_SetChannelDmaEnable(const uint8_t instance, pwm_channel_type_t channel, bool state);

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
void PWM_DRV_SetDebugMode(const uint8_t instance, pwm_debug_mode_t mode);

/*!
 * @brief Set the write protection.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
void PWM_DRV_SetWriteProtection(const uint8_t instance, bool state);

/*!
 * @brief Install pwm overflow & underflow interrupt callback function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] func: The pwm callback function.
 * @return none
 */
void PWM_DRV_InstallOverflowCallback(const uint8_t instance, const pwm_callback_t func);

/*!
 * @brief Install pwm channel interrupt callback function.
 *
 * @param[in] instance: The pwm instance number
 * @param[in] func: The pwm callback function
 * @return none
 */
void PWM_DRV_InstallChannelCallback(const uint8_t instance, const pwm_callback_t func);

/*!
 * @brief PWM overflow common interrupt service routine.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_Overflow_IRQHandler(uint8_t instance);

/*!
 * @brief PWM channel common interrupt service routine.
 *
 * @param[in] instance: The pwm instance number
 * @return none
 */
void PWM_DRV_Channel_IRQHandler(uint8_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PWM_COMMON_H */

/* =============================================  EOF  ============================================== */
