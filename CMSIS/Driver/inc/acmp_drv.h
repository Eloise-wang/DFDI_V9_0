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
 * @file acmp_drv.h
 *
 * @brief This file provides analog comparator module integration functions interfaces.
 *
 */

#ifndef _ACMP_DRV_H
#define _ACMP_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief ACMP input channel number.
 */
typedef enum
{
    ACMP_EXTERNAL_CH0 = 0U,     /*!< External channel 0 */
    ACMP_EXTERNAL_CH1,          /*!< External channel 1 */
    ACMP_EXTERNAL_CH2,          /*!< External channel 2 */
    ACMP_EXTERNAL_CH3,          /*!< External channel 3 */
    ACMP_EXTERNAL_CH4,          /*!< External channel 4 */
    ACMP_EXTERNAL_CH5,          /*!< External channel 5 */
    ACMP_EXTERNAL_CH6,          /*!< External channel 6 */
    ACMP_EXTERNAL_CH7,          /*!< External channel 7 */
    ACMP_DAC_OUTPUT,            /*!< DAC output channel 8 */
    ACMP_CHANNEL_MAX            /*!< Invalid channel */
} acmp_ch_number_t;

/*!
 * @brief ACMP output interrupt trigger type.
 */
typedef enum
{
    ACMP_FALLING_EDGE        = 0U,          /*!< Trigger interrupt on falling edge */
    ACMP_RISING_EDGE         = 1U,          /*!< Trigger interrupt on rising edge */
    ACMP_BOTH_EDGES          = 3U           /*!< Trigger interrupt on falling/rising edge */
} acmp_output_trigger_t;

/*!
 * @brief ACMP result output signal select
 */
typedef enum
{
    ACMP_COUT  = 0U,     /*!< Select filtered result COUT as comparator output signal. */
    ACMP_COUTA = 1U      /*!< Select unfiltered result COUTA as comparator output signal. */
} acmp_output_select_t;

/*!
 * @brief ACMP hysteresis mode type.
 */
typedef enum
{
    ACMP_HYS_FALLING_EDGE = 0U,     /*!< hysteresis direction on falling edge */
    ACMP_HYS_BOTH_EDGE              /*!< hysteresis direction on both edge */
} acmp_hysteresis_mode_t;

/*!
 * @brief ACMP hysteresis level type.
 */
typedef enum
{
    ACMP_LEVEL_HYS_0MV = 0U,    /*!< 0mv  LEVEL_HYS voltage */
    ACMP_LEVEL_HYS_10MV,        /*!< 10mv LEVEL_HYS voltage */
    ACMP_LEVEL_HYS_20MV,        /*!< 20mv LEVEL_HYS voltage */
    ACMP_LEVEL_HYS_40MV         /*!< 40mv LEVEL_HYS voltage */
} acmp_hysteresis_t;

/*!
 * @brief ACMP digital filter clock divide type.
 */
typedef enum
{
    ACMP_FLT_DIVIDE_1 = 0U,     /*!< Input clock divided by 1. */
    ACMP_FLT_DIVIDE_2,          /*!< Input clock divided by 2. */
    ACMP_FLT_DIVIDE_3,          /*!< Input clock divided by 3. */
    ACMP_FLT_DIVIDE_4,          /*!< Input clock divided by 4. */
    ACMP_FLT_DIVIDE_5,          /*!< Input clock divided by 5. */
    ACMP_FLT_DIVIDE_6,          /*!< Input clock divided by 6. */
    ACMP_FLT_DIVIDE_7,          /*!< Input clock divided by 7. */
    ACMP_FLT_DIVIDE_8           /*!< Input clock divided by 8. */
} acmp_filter_divide_t;

/*!
 * @brief ACMP low pass filter bandwidth type.
 */
typedef enum
{
    ACMP_LPF_500KHZ = 0U,       /*!< 500KHz filter frequence */
    ACMP_LPF_1MHZ,              /*!< 1MHz filter frequence */
    ACMP_LPF_2MHZ,              /*!< 2MHz filter frequence */
    ACMP_LPF_BYPASS             /*!< Bypass filter frequence */
} acmp_low_pass_filter_t;

/*!
 * @brief ACMP DAC voltage reference source type.
 */
typedef enum
{
    ACMP_DAC_BANDGAP = 0U,      /*!< Bandgap as reference source */
    ACMP_DAC_VDD                /*!< Vdd as reference source */
} acmp_voltage_reference_t;

/*!
 * @brief ACMP input channel polling mode type.
 */
typedef enum
{
    ACMP_NONE_POLLING = 0U,     /*!< Not using polling mode, which is normal comparator mode */
    ACMP_POSITIVE_POLLING,      /*!< Positive polling */
    ACMP_NEGATIVE_POLLING       /*!< Negative polling */
} acmp_input_polling_t;

/*!
 * @brief ACMP polling clock divide type.
 */
typedef enum
{
    ACMP_CLK_DIVIDE_256 = 0U,       /*!< Clock/256 */
    ACMP_CLK_DIVIDE_101,            /*!< Clock/100 */
    ACMP_CLK_DIVIDE_71,             /*!< Clock/70 */
    ACMP_CLK_DIVIDE_51,             /*!< Clock/50 */
    ACMP_CLK_DIVIDE_MAX             /*!< Invalid divid ratio */
} acmp_polling_clk_divide_t;

/*!
 * @brief ACMP interrupt callback function type
 */
typedef void (*acmp_callback_t)(const uint32_t instance, uint32_t sr);

/*!
 * @brief ACMP comparator configuration type.
 */
typedef struct
{
    bool interruptEn;                       /*!< Interrupt enable flag */
    acmp_output_trigger_t outputTrigger;    /*!< Trigger mode */
    acmp_output_select_t outputSelect;      /*!< Output signal selection */
    bool filterEnable;                      /*!< Digital filter enable flag */
    uint8_t filterSampleCount;              /*!< Digital filter sample count */
    acmp_filter_divide_t clockDivide;       /*!< Digital filter clock divide */
    acmp_low_pass_filter_t lpfBandwidth;    /*!< Low pass filter bandwidth */
    bool inverterEnable;                    /*!< ACMP result invertion enable flag */
    bool pinState;                          /*!< ACMP output pin enable flag */
    acmp_hysteresis_mode_t hysteresisMode;  /*!< Hysteresis mode */
    acmp_hysteresis_t hysteresisLevel;      /*!< Hysteresis level selection */
    bool usingLSIEnable;                    /*!< Using LSI as function clock enable flag */
    bool windowModeEnable;                  /*!< Window mode enable flag */
    bool powerEnable;                       /*!< ACMP analog module power enable flag */
    acmp_callback_t callback;               /*!< ACMP interrupt callback function pointer */
} acmp_comparator_t;

/*!
 * @brief ACMP analog mux input configuration type.
 */
typedef struct
{
    acmp_ch_number_t positiveInputMux;      /*!< Positive input channel selection */
    acmp_ch_number_t negativeInputMux;      /*!< Negative input channel selection */
} acmp_anmux_t;

/*!
 * @brief ACMP DAC configuration type.
 */
typedef struct
{
    acmp_voltage_reference_t voltageReferenceSource;    /*!< DAC voltage reference source selection */
    uint8_t voltage;            /*!< DAC output code */
    bool state;                 /*!< DAC power enable flag */
} acmp_dac_t;

/*!
 * @brief ACMP polling mode configuration.
 */
typedef struct
{
    acmp_polling_clk_divide_t pollingClockDivide;   /*!< Polling clock divide */
    acmp_input_polling_t mode;          /*!< Polling mode selection */
    uint16_t pollingSequence;           /*!< Polling sequence, using bit0:8, each bit for each channel,
                                            bit 0 represent channel 0, and other bits likewise */

    bool hallOutputEnable;              /*!< Hall output enable flag */
    acmp_ch_number_t hallAOutputCh;     /*!< Hall A output channel selection */
    acmp_ch_number_t hallBOutputCh;     /*!< Hall B output channel selection */
    acmp_ch_number_t hallCOutputCh;     /*!< Hall C output channel selection */
} acmp_polling_mode_t;

/*!
 * @brief ACMP module configuration type
 */
typedef struct
{
    acmp_comparator_t comparator;       /*!< comparator basic configuration */
    acmp_anmux_t mux;                   /*!< Input mux configuration */
    acmp_dac_t dac;                     /*!< DAC configuration */
    acmp_polling_mode_t pollingMode;    /*!< Polling mode configuration */
} acmp_module_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Reset all ACMP registers. Need to be called after ACMP clock is applied and reset is deasserted.
 *
 * @param[in] instance: ACMP instance number
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_Reset(const uint32_t instance);

/*!
 * @brief Get reset configuration of all registers in configuration structure.
 *
 * @param[out] config: ACMP configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitConfigAll(acmp_module_t *config);

/*!
 * @brief Get default configuration structure.
 *
 * @param[out] config: ACMP configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetDefaultConfig(acmp_module_t* const config);

/*!
 * @brief Configure all comparator function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: ACMP configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_Init(const uint32_t instance, const acmp_module_t *const config);

/*!
 * @brief Reset ACMP module and close the clock source.
 *
 * @param[in] instance: ACMP instance number
 * @return none
 */
void ACMP_DRV_Deinit(const uint32_t instance);

/*!
 * @brief Get the current ACMP configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: ACMP configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetConfigAll(const uint32_t instance, acmp_module_t * const config);

/*!
 * @brief Get reset configuration of registers related to ACMP comparator.
 *
 * @param[out] config: ACMP comparator configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitConfigComparator(acmp_comparator_t *config);

/*!
 * @brief Configure all ACMP comparator function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: ACMP comparator configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ConfigComparator(const uint32_t instance, const acmp_comparator_t *config);

/*!
 * @brief Get the current ACMP comparator configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: ACMP comparator configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetComparatorConfig(const uint32_t instance, acmp_comparator_t *config);

/*!
 * @brief Get reset configuration of registers related to DAC.
 *
 * @param[out] config: DAC configuration struce
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitConfigDAC(acmp_dac_t *config);

/*!
 * @brief Configure all DAC function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: DAC configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ConfigDAC(const uint32_t instance, const acmp_dac_t *config);

/*!
 * @brief Get the current DAC configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: DAC configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetDACConfig(const uint32_t instance, acmp_dac_t * const config);

/*!
 * @brief Get reset configuration of registers related to input mux.
 *
 * @param[out] config: ACMP intpu mux configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitConfigMUX(acmp_anmux_t *config);

/*!
 * @brief Configure all input mux function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: ACMP input mux configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ConfigMUX(const uint32_t instance, const acmp_anmux_t *config);

/*!
 * @brief Get the current intput mux configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: ACMP input mux configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetMUXConfig(const uint32_t instance, acmp_anmux_t * const config);

/*!
 * @brief Get reset configuration of registers related to polling mode.
 *
 * @param[out] config: ACMP polling mode configuration structure
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInitPollingMode(acmp_polling_mode_t *config);

/*!
 * @brief Configure all polling function with the given configuration structure.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] config: ACMP polling mode configuration structure that need to apply
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ConfigPollingMode(const uint32_t instance, const acmp_polling_mode_t *config);

/*!
 * @brief Get the current polling mode configuration.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] config: ACMP polling mode configuration structure that need to fill with current configuration
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetPollingModeConfig(const uint32_t instance, acmp_polling_mode_t * const config);

/*!
 * @brief Get ACMP normal mode output data.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] flags: output data
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetOutputData(const uint32_t instance, uint8_t *flags);

/*!
 * @brief Get ACMP output status flags.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] flags: output status flags
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetOutputFlags(const uint32_t instance, uint8_t *flags);

/*!
 * @brief Clear ACMP output flags status.
 *
 * @param[in] instance: ACMP instance number
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ClearOutputFlags(const uint32_t instance);

/*!
 * @brief Get polling mode compare data.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] flags: polling mode input channel compare data
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInputData(const uint32_t instance, uint16_t *flags);

/*!
 * @brief Get polling mode status flags.
 *
 * @param[in] instance: ACMP instance number
 * @param[out] flags: polling mode input channel status
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_GetInputFlags(const uint32_t instance, uint16_t *flags);

/*!
 * @brief Clear polling mode status flags.
 *
 * @param[in] instance: ACMP instance number
 * @return execution status
 *          - STATUS_SUCCESS : Completed successfully.
 *          - STATUS_ERROR : Error occurred.
 */
status_t ACMP_DRV_ClearInputFlags(const uint32_t instance);

/*!
 * @brief Set ACMP interrutp callback handler.
 *
 * @param[in] instance: ACMP instance number
 * @param[in] func: callback handler function pointer.
 * @return none
 */
void ACMP_DRV_SetCallback(const uint32_t instance, const acmp_callback_t func);

/*!
 * @brief ACMP interrupt service routine. ACMP interrupt status flags will be cleared here.
 *
 * @param[in] instance: ACMP instance number
 * @return none
 */
void ACMP_CommonISR(uint32_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ACMP_DRV_H */

/* =============================================  EOF  ============================================== */
