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
 * @file adc_drv.h
 *
 * @brief This file provides analog to digital converter module integration functions interfaces.
 *
 */

#ifndef _ADC_DRV_H
#define _ADC_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "ctu_drv.h"

/* ============================================  Define  ============================================ */
/*!< ADC regular group sequence length. */
#define ADC_REGULAR_SEQUENCE_LENGTH         (24UL)

/*!< ADC injected group sequence length. */
#define ADC_INJECT_SEQUENCE_LENGTH          (4UL)

/*!< get ADC average count number, x should be adc_average_t */
#define ADC_GET_AVERAGE_NUM(x)              (4UL << (x))

/*!< ADC regular group trigger source count number in CTU */
#define ADC_CTU_REGULAR_SOURCE_COUNT        (4UL)
/*!< ADC injection group trigger source count number in CTU */
#define ADC_CTU_INJECT_SOURCE_COUNT         (4UL)

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Clock Divider selection
 */
typedef enum
{
    ADC_CLK_DIVIDE_1 = 0U,      /*!< Input clock divided by 1. */
    ADC_CLK_DIVIDE_2,           /*!< Input clock divided by 2. */
    ADC_CLK_DIVIDE_3,           /*!< Input clock divided by 3. */
    ADC_CLK_DIVIDE_4,           /*!< Input clock divided by 4. */
    ADC_CLK_DIVIDE_5,           /*!< Input clock divided by 5. */
    ADC_CLK_DIVIDE_6,           /*!< Input clock divided by 6. */
    ADC_CLK_DIVIDE_7,           /*!< Input clock divided by 7. */
    ADC_CLK_DIVIDE_8,           /*!< Input clock divided by 8. */
    ADC_CLK_DIVIDE_9,           /*!< Input clock divided by 9. */
    ADC_CLK_DIVIDE_10,          /*!< Input clock divided by 10.*/
    ADC_CLK_DIVIDE_11,          /*!< Input clock divided by 11.*/
    ADC_CLK_DIVIDE_12,          /*!< Input clock divided by 12.*/
    ADC_CLK_DIVIDE_13,          /*!< Input clock divided by 13.*/
    ADC_CLK_DIVIDE_14,          /*!< Input clock divided by 14.*/
    ADC_CLK_DIVIDE_15,          /*!< Input clock divided by 15.*/
    ADC_CLK_DIVIDE_16           /*!< Input clock divided by 16.*/
} adc_clk_divide_t;

/*!
 * @brief Conversion resolution selection
 */
typedef enum
{
    ADC_RESOLUTION_12BIT = 0U,      /*!< 12-bit resolution mode */
    ADC_RESOLUTION_10BIT,           /*!< 10-bit resolution mode */
    ADC_RESOLUTION_8BIT,            /*!< 8-bit resolution mode */
    ADC_RESOLUTION_MAX = 3U
} adc_resolution_t;

/*!
 * @brief Voltage reference selection
 */
typedef enum
{
    ADC_VOLTAGEREF_VREF = 0U,       /*!< VrefH and VrefL as Voltage reference. */
    ADC_VOLTAGEREF_VDDA             /*!< VDDA as Voltage reference. */
} adc_voltage_reference_t;

/*!
 * @brief ADC sample time enumeration.
 */
typedef enum
{
    ADC_SPT_CLK_5 = 0U,         /*!< ADC Sample time 5 Clock Cycle */
    ADC_SPT_CLK_10,             /*!< ADC Sample time 10 Clock Cycle */
    ADC_SPT_CLK_15,             /*!< ADC Sample time 15 Clock Cycle */
    ADC_SPT_CLK_23,             /*!< ADC Sample time 23 Clock Cycle */
    ADC_SPT_CLK_35,             /*!< ADC Sample time 35 Clock Cycle */
    ADC_SPT_CLK_45,             /*!< ADC Sample time 45 Clock Cycle */
    ADC_SPT_CLK_85,             /*!< ADC Sample time 85 Clock Cycle */
    ADC_SPT_CLK_185 = 7U,       /*!< ADC Sample time 185 Clock Cycle */
    ADC_SPT_CLK_MAX = 8U        /*!< Invalid ADC Sample time */
} adc_sample_time_t;

/*!
 * @brief ADC conversion group type enumeration.
 */
typedef enum
{
    ADC_REGULAR_GROUP = 0U,     /*!< ADC regular group */
    ADC_INJECT_GROUP,           /*!< ADC inject group */
    ADC_GROUP_MAX               /*!< ADC group max value, should be less than this value */
} adc_group_t;

/*!
 * @brief ADC sequence type enumeration.
 */
typedef enum
{
    ADC_RSEQ_0 = 0U,            /*!< ADC rseq 0 */
    ADC_RSEQ_1,                 /*!< ADC rseq 1 */
    ADC_RSEQ_2,                 /*!< ADC rseq 2 */
    ADC_RSEQ_3,                 /*!< ADC rseq 3 */
    ADC_RSEQ_4,                 /*!< ADC rseq 4 */
    ADC_RSEQ_5,                 /*!< ADC rseq 5 */
    ADC_RSEQ_6,                 /*!< ADC rseq 6 */
    ADC_RSEQ_7,                 /*!< ADC rseq 7 */
    ADC_RSEQ_8,                 /*!< ADC rseq 8 */
    ADC_RSEQ_9,                 /*!< ADC rseq 9 */
    ADC_RSEQ_10,                /*!< ADC rseq 10 */
    ADC_RSEQ_11,                /*!< ADC rseq 11 */
    ADC_RSEQ_12,                /*!< ADC rseq 12 */
    ADC_RSEQ_13,                /*!< ADC rseq 13 */
    ADC_RSEQ_14,                /*!< ADC rseq 14 */
    ADC_RSEQ_15,                /*!< ADC rseq 15 */
    ADC_RSEQ_16,                /*!< ADC rseq 16 */
    ADC_RSEQ_17,                /*!< ADC rseq 17 */
    ADC_RSEQ_18,                /*!< ADC rseq 18 */
    ADC_RSEQ_19,                /*!< ADC rseq 19 */
    ADC_RSEQ_20,                /*!< ADC rseq 20 */
    ADC_RSEQ_21,                /*!< ADC rseq 21 */
    ADC_RSEQ_22,                /*!< ADC rseq 22 */
    ADC_RSEQ_23,                /*!< ADC rseq 23 */
    ADC_RSEQ_24,                /*!< ADC rseq 24 */
    ADC_RSEQ_25,                /*!< ADC rseq 25 */
    ADC_RSEQ_26,                /*!< ADC rseq 26 */
    ADC_RSEQ_27,                /*!< ADC rseq 27 */
    ADC_RSEQ_28,                /*!< ADC rseq 28 */
    ADC_RSEQ_29,                /*!< ADC rseq 29 */
    ADC_RSEQ_30,                /*!< ADC rseq 30 */
    ADC_RSEQ_31,                /*!< ADC rseq 31 */
    ADC_ISEQ_0 = 32U,           /*!< ADC iseq 0 */
    ADC_ISEQ_1,                 /*!< ADC iseq 1 */
    ADC_ISEQ_2,                 /*!< ADC iseq 2 */
    ADC_ISEQ_3,                 /*!< ADC iseq 3 */
    ADC_SEQ_MAX = 36U           /*!< Invalid seq */
} adc_sequence_t;

/*!
 * @brief ADC channel type enumeration.
 */
typedef enum
{
    ADC_CH_0 = 0U,          /*!< ADC channel 0 */
    ADC_CH_1,               /*!< ADC channel 1 */
    ADC_CH_2,               /*!< ADC channel 2 */
    ADC_CH_3,               /*!< ADC channel 3 */
    ADC_CH_4,               /*!< ADC channel 4 */
    ADC_CH_5,               /*!< ADC channel 5 */
    ADC_CH_6,               /*!< ADC channel 6 */
    ADC_CH_7,               /*!< ADC channel 7 */
    ADC_CH_8,               /*!< ADC channel 8 */
    ADC_CH_9,               /*!< ADC channel 9 */
    ADC_CH_10,              /*!< ADC channel 10 */
    ADC_CH_11,              /*!< ADC channel 11 */
    ADC_CH_12,              /*!< ADC channel 12 */
    ADC_CH_13,              /*!< ADC channel 13 */
    ADC_CH_14,              /*!< ADC channel 14 */
    ADC_CH_15,              /*!< ADC channel 15 */
    ADC_CH_16,              /*!< ADC channel 16 */
    ADC_CH_17,              /*!< ADC channel 17 */
    ADC_CH_18,              /*!< ADC channel 18 */
    ADC_CH_19,              /*!< ADC channel 19 */
    ADC_CH_20,              /*!< ADC channel 20 */
    ADC_CH_21,              /*!< ADC channel 21 */
    ADC_CH_22,              /*!< ADC channel 22 */
    ADC_CH_23,              /*!< ADC channel 23 */
    ADC_CH_24,              /*!< ADC channel 24 */
    ADC_CH_25,              /*!< ADC channel 25 */
    ADC_CH_26,              /*!< ADC channel 26 */
    ADC_CH_27,              /*!< ADC channel 27 */
    ADC_CH_28,              /*!< ADC channel 28 */
    ADC_CH_29,              /*!< ADC channel 29 */
    ADC_CH_30,              /*!< ADC channel 30 */
    ADC_CH_31,              /*!< ADC channel 31 */
    ADC_CH_BANDGAP = 33U,   /*!< ADC internal channel Bandgap */
    ADC_CH_TSENSOR = 34U,   /*!< ADC internal channel T-Sensor */
    ADC_CH_GEOE_CAL = 35U,  /*!< ADC GE OE calibration channel */
    ADC_CH_SUPPLY = 37U,    /*!< ADC internal channel supply */
    ADC_CH_MAX,             /*!< Invalid channel */
    ADC_CH_DISABLE = 0x3FU  /*!< Disabled channel */
} adc_inputchannel_t;

/*!
 * @brief ADC internal supply channel type enumeration.
 */
typedef enum
{
    ADC_CH_SUPPLY_VDD = 0U,         /*!< ADC internal channel VDD supply */
    ADC_CH_SUPPLY_VDDA,             /*!< ADC internal channel VDDA supply */
    ADC_CH_SUPPLY_REFH,             /*!< ADC reference positive input */
    ADC_CH_SUPPLY_OSC_LDO,          /*!< ADC OSC LDO supply */
    ADC_CH_SUPPLY_FLASH_LDO,        /*!< ADC flash LDO supply */
    ADC_CH_SUPPLY_DIGITAL_LDO,      /*!< ADC digital LDO supply */
    ADC_CH_SUPPLY_MAX = 6U          /*!< ADC supply max value */
} adc_supply_channel_t;

/*!
 * @brief Hardware average selection
 */
typedef enum
{
    ADC_AVERAGE_4 = 0U,         /*!< Hardware average of 4 samples. */
    ADC_AVERAGE_8 = 1U,         /*!< Hardware average of 8 samples. */
    ADC_AVERAGE_16 = 2U,        /*!< Hardware average of 16 samples. */
    ADC_AVERAGE_32 = 3U         /*!< Hardware average of 32 samples. */
} adc_average_t;

/*!
 * @brief ADC data alignment enumeration.
 */
typedef enum
{
    ADC_DATA_ALIGN_RIGHT = 0U,  /*!< Data right alignment */
    ADC_DATA_ALIGN_LEFT         /*!< Data left alignment */
} adc_align_t;

/*!
 * @brief ADC analog monitor trigger mode enumeration.
 */
typedef enum
{
    ADC_AMO_TRIGGER_LEVEL = 0U, /*!< AMO Level trigger */
    ADC_AMO_TRIGGER_EDGE        /*!< AMO Edge trigger */
} adc_amo_trigger_mode_t;

/*!
 * @brief ADC trigger convert mode enumeration.
 */
typedef enum
{
    ADC_TRIGGER_INTERNAL = 0U,      /*!< Internal software trigger */
    ADC_TRIGGER_EXTERNAL            /*!< External signal trigger */
} adc_trigger_t;

/*!
 * @brief Events which can trigger ADC callback
 *
 * These evnets can be combined in one uint32_t.
 */
typedef enum
{
    ADC_EVENT_AMO       = ADC_STR_AMO_Msk,      /*!< Abnormal event in AMO level mode */
    ADC_EVENT_AAMO      = ADC_STR_AAMO_Msk,     /*!< Abnormal event in AMO edge mode */
    ADC_EVENT_NAMO      = ADC_STR_NAMO_Msk,     /*!< Normal event in AMO edge mode */
    ADC_EVENT_EOC       = (1U << 10U),          /*!< End of convertion evnet */
} adc_event_t;

/*!
 * @brief ADC interrupt information structure
 */
typedef struct {
    uint32_t instance;
    uint32_t event;     /* adc event bit mask, combination of adc_event_t */
    adc_sequence_t sequence;
} adc_interrupt_info_t;

/*!
 * @brief ADC interrupt handler type
 */
typedef void (*adc_callback_t)(adc_interrupt_info_t *info, void *parameter);

/*!
 * @brief ADC genernal convertion config
 *
 * This structure is used to config ADC generanl convertion config,
 * like clock divide, trigger source, convertion mode and ADC analog module power.
 */
typedef struct
{
    adc_clk_divide_t clockDivide;       /*!< ADC clock divide */
    adc_resolution_t resolution;        /*!< ADC resolution setting */
    adc_align_t alignment;              /*!< ADC result alignment setting */

    adc_trigger_t regularTrigger;       /*!< regular group trigger source type */
    adc_trigger_t injectTrigger;        /*!< injection group trigger source type */

    bool dmaEnable;                     /*!< Enable/disable ADC DMA support function */

    adc_voltage_reference_t voltageRef; /*!< ADC reference source selection */

    bool scanModeEn;                    /*!< Enable/disable scan mode for regular and injected group */
    bool continuousModeEn;              /*!< Enable/disable continuous mode */
    bool regularDiscontinuousModeEn;    /*!< Enable/disable discontinuous mode for regular group */
    bool injectDiscontinuousModeEn;     /*!< Enable/disable discontinuous mode for injected group */
    bool injectAutoModeEn;              /*!< Enable/disable antomatic mode for injected group */
    bool intervalModeEn;                /*!< Enable/disable interval mode for mode3/5 */

    uint8_t regularDiscontinuousNum;    /*!< Regular discontinuous mode number */

    uint8_t regularSequenceLength;      /*!< Regular sequence length */
    uint8_t injectSequenceLength;       /*!< Injected sequence length */

    adc_callback_t callback;            /*!< ADC interrupt callback function pointer for EOC & AMO interrupt */
    void *parameter;                    /*!< Interrupt callback parameter from user */

    bool powerEn;                       /*!< Enable/disbale ADC analog module power supply */
} adc_converter_config_t;

/*!
 * @brief Defines the control channel configuration
 *
 * This structure is used to configure a control channel
 * of the ADC
 */
typedef struct
{
    adc_inputchannel_t channel;     /*!< Selection of input channel for measurement */
    adc_sample_time_t spt;          /*!< sample time */
    bool interruptEn;               /*!< Enable interrupts for this channel. Should not enable EOC using DMA */
    /*!< Selection of supply source for supply channel, only one source of supply can be
     * used in group convertion, since there is only one supply source selection register */
    adc_supply_channel_t supply;
} adc_chan_config_t;

/*!
 * @brief ADC analog monitor(AMO) config structure
 *
 * This structure is used to config AMO function of ADC.
 */
typedef struct
{
    adc_amo_trigger_mode_t amoTriggerMode;  /*!< 0:level trigger; 1:edge trigger */
    bool amoInterruptEn;                    /*!< Enable/disable AMO interrupt */
    bool amoRegularEn;                      /*!< Enable/disable analog monitor regular group mode */
    bool amoInjectEn;                       /*!< Enable/disable analog monitor injected group mode */
    bool amoSingleModeEn;                   /*!< Enable/disable analog monitor single channel mode */
    adc_inputchannel_t amoSingleChannel;    /*!< Select analog monitor single channel */
    uint16_t amoUpThreshold;                /*!< Set analog monitor upper Threshold */
    uint16_t amoLowThreshold;               /*!< Set analog monitor lower Threshold */
    uint16_t amoUpOffset;                   /*!< Set analog monitor upper Offset */
    uint16_t amoLowOffset;                  /*!< Set analog monitor lower Offset */
} adc_amo_config_t;

/*!
 * @brief ADC hardware average structure
 */
typedef struct
{
    bool hwAvgEnable;           /*!< Enable hardware averaging */
    adc_average_t hwAverage;    /*!< Selection for number of samples used for hardware averaging */
} adc_average_config_t;

/*!
 * @brief ADC calibration structure
 */
typedef struct
{
    int16_t userGain;
    int16_t userOffset;
} adc_calibration_t;

/*!
 * @brief Analog voltage input common mode voltage selection
 */
typedef enum
{
    ANA_VINVCM_DC = 0U,
    ANA_VINVCM_AC_285K = 1U,
    ANA_VINVCM_AC_317K = 2U,
    ANA_VINVCM_AC_349K = 5U,
    ANA_VINVCM_AC_381K = 7U,
} ana_vinvcm_t;

/*!
 * @brief Analog ADC channel number
 */
typedef enum
{
    ANA_CHANNEL_VIN0 = 0U,
    ANA_CHANNEL_VIN1,
    ANA_CHANNEL_VIN2,
    ANA_CHANNEL_VIN3,
    ANA_CHANNEL_VIN4,
    ANA_CHANNEL_VIN5,
    ANA_CHANNEL_VIN6,
    ANA_CHANNEL_VIN7,
    ANA_CHANNEL_VIN8,
    ANA_CHANNEL_VIN9,
    ANA_CHANNEL_VIN10,
    ANA_CHANNEL_VIN11,
    ANA_CHANNEL_VIN12,
    ANA_CHANNEL_VIN13,
    ANA_CHANNEL_VIN14,
    ANA_CHANNEL_VIN15,
    ANA_CHANNEL_VIN16,
    ANA_CHANNEL_VIN17,
    ANA_CHANNEL_VIN18,
    ANA_CHANNEL_VIN19,
    ANA_CHANNEL_VIN20,
    ANA_CHANNEL_VIN21,
    ANA_CHANNEL_VIN22,
    ANA_CHANNEL_VIN23,
    ANA_CHANNEL_AMUX = 32U,
    ANA_CHANNEL_VBG,
    ANA_CHANNEL_VTS,
    ANA_CHANNEL_GEOE_CAL,
    ANA_CHANNEL_VREFP,
    ANA_CHANNEL_VSUPPLY,
    ANA_CHANNEL_FLOATING = 0x3FU
} ana_channel_t;

/*!
 * @brief Analog ADC GE OE calibration internal input source.
 */
typedef enum
{
    ANA_GEOECAL_FLOATING = 0U,
    ANA_GEOECAL_REF_16_4,
    ANA_GEOECAL_REF_16_5,
    ANA_GEOECAL_REF_16_6,
    ANA_GEOECAL_REF_16_8,
    ANA_GEOECAL_REF_16_10,
    ANA_GEOECAL_REF_16_11,
    ANA_GEOECAL_REF_16_12,
} ana_geoecal_vin_t;
/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize ADC module, which is enable its clock and deassert its reset signal.
 *
 * Before calling any other ADC driver API, this function should be called.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_Init(const uint32_t instance);

/*!
 * @brief Deinitialize ADC module, which is disable its clock and assert its reset signal.
 *
 * After calling this function, any other ADC driver API should not be called other than
 * ADC_DRV_Init.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_Deinit(const uint32_t instance);

/*!
 * @brief reset the ADC register.
 *
 * @param [in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_Reset(const uint32_t instance);

/*!
 * @brief Auto calibration funciton for ADC. This function should be called every time after
 * ADC reset or power on.
 *
 * @param [in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_AutoCalibration(const uint32_t instance);

/*!
 * @brief get interrupt number for ADC instance.
 *
 * @param [in] instance: ADC instance number
 * @return ADC interrupt number for instance
 */
IRQn_Type ADC_DRV_GetInterruptNumber(const uint32_t instance);

/*!
 * @brief Initialization convert structure.
 *
 * @param [out] config: convert config structure pointer
 * @return none
 */
void ADC_DRV_InitConverterStruct(adc_converter_config_t * const config);

/*!
 * @brief config ADC convert setting.
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: convert config that need to config
 * @return none
 */
void ADC_DRV_ConfigConverter(const uint32_t instance,
        const adc_converter_config_t * const config);

/*!
 * @brief Get ADC converter config from register.
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: config pointer that need to fill with current register config
 * @return none
 */
void ADC_DRV_GetConverterConfig(const uint32_t instance,
        adc_converter_config_t * const config);

/*!
 * @brief initialize Analog Monitor config structure.
 *
 * @param [out] config: pointer to the config that need to be initialized
 * @return none
 */
void ADC_DRV_InitAMOStruct(adc_amo_config_t * const config);

/*!
 * @brief config Analog Monitor function.
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: pointer to configuration that need to apply to the register
 * @return none
 */
void ADC_DRV_ConfigAMO(const uint32_t instance, const adc_amo_config_t * const config);

/*!
 * @brief Get the configuration of AMO function.
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: pointer that need to be filled with AMO register configuration
 * @return none
 */
void ADC_DRV_GetAMOConfig(const uint32_t instance, adc_amo_config_t * const config);

/*!
 * @brief Initialization average config structure.
 *
 * @param [out] config: average configuration structure that need to be initialized
 * @return none
 */
void ADC_DRV_InitHwAverageStruct(adc_average_config_t * const config);

/*!
 * @brief Apply the average function configuration to the hardware.
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: configuration that need to apply to average function
 * @return none
 */
void ADC_DRV_ConfigHwAverage(const uint32_t instance, const adc_average_config_t * const config);

/*!
 * @brief Get average function configuration form register.
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: pointer that need to be filled with average register
 *                      configuration
 * @return none
 */
void ADC_DRV_GetHwAverageConfig(const uint32_t instance, adc_average_config_t * const config);

/*!
 * @brief Initialization channel config structure.
 *
 * @param [out] config: channel configuration structure that need to be initialized
 * @return none
 */
void ADC_DRV_InitChanStruct(adc_chan_config_t * const config);

/*!
 * @brief Apply the channel configuration to the hardware.
 *
 * @param [in] instance: ADC instance number
 * @param [in] seqIndex ADC group seqeunce index
 * @param [in] config: configuration that need to apply to the group sequence function
 * @return none
 */
void ADC_DRV_ConfigChan(const uint32_t instance,
        const adc_sequence_t seqIndex,
        const adc_chan_config_t * const config);

/*!
 * @brief Get group sequence channel configuration form register.
 *
 * @param [in] instance: ADC instance number
 * @param [in] seqIndex ADC group seqeunce index
 * @param [out] config: pointer that need to be filled with group sequence channel
 *                      register
 * @return none
 */
void ADC_DRV_GetChanConfig(const uint32_t instance,
        const adc_sequence_t seqIndex,
        adc_chan_config_t * const config);

/*!
 * @brief Enable ADC calibration caculartion function.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_EnableCalibration(const uint32_t instance);

/*!
 * @brief Disable ADC calibration caculartion function.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_DisableCalibration(const uint32_t instance);

/*!
 * @brief Initialization calibartion config structure.
 *
 * @param [out] config: calibartion configuration structure that need to be initialized
 * @return none
 */
void ADC_DRV_InitCalibrationStruct(adc_calibration_t * const config);

/*!
 * @brief Apply the calibration configuration to the external channel.
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: configuration that need to apply to external channel calibartion function
 * @return none
 */
void ADC_DRV_ConfigExternalCalibration(const uint32_t instance,
        const adc_calibration_t * const config);

/*!
 * @brief Get external channel calibration configuration form register.
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: pointer that need to be filled with external channel calibration register
 * @return none
 */
void ADC_DRV_GetExternalCalibration(const uint32_t instance,
        adc_calibration_t * const config);

/*!
 * @brief Apply the calibration configuration to the internal channel.
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: configuration that need to apply to internal channel calibartion function
 * @return none
 */
void ADC_DRV_ConfigInternalCalibration(const uint32_t instance,
        const adc_calibration_t * const config);

/*!
 * @brief Get internal channel calibration configuration form register.
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: pointer that need to be filled with internal channel calibration register
 * @return none
 */
void ADC_DRV_GetInternalCalibration(const uint32_t instance,
        adc_calibration_t * const config);

/*!
 * @brief Start software trigger regular group convertion.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_SoftwareStartRegularConvert(uint32_t instance);

/*!
 * @brief Start software trigger inject group convertion.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_SoftwareStartInjectConvert(uint32_t instance);

/*!
 * @brief Get ADC Idle flag.
 *
 * @param[in] instance: ADC instance number
 * @return whether ADC is idle
 *          - true: ADC is not converting
 *          - false: ADC is busy converting
 */
bool ADC_DRV_GetIdleFlag(const uint32_t instance);

/*!
 * @brief Wait for the ADC begin to convert.
 *
 * After software trigger regualr or injection conversion,
 * the idle flag may still be 1 for a few microsecond, use this API to wait before ADC start conversion.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_WaitConvStart(const uint32_t instance);

/*!
 * @brief Wait for the ADC conversion to finish.
 *
 * Shoule not be used if DMA is used for ADC data transfer.
 *
 * @param [in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_WaitConvDone(const uint32_t instance);

/*!
 * @brief Get convertion complete flag form register.
 *
 * @param [in] instance: ADC instance number
 * @param [in] seqIndex ADC group seqeunce index
 * @return the convertion complete flag of the group sequence
 */
bool ADC_DRV_GetConvCompleteFlag(const uint32_t instance,
        const adc_sequence_t seqIndex);

/*!
 * @brief Clear convertion complete flag in register.
 *
 * @param[in] instance: ADC instance number
 * @param[in] seqIndex: ADC group seqeunce index
 */
void ADC_DRV_ClearConvCompleteFlag(const uint32_t instance,
        const adc_sequence_t seqIndex);

/*!
 * @brief Get the ADC convertion result for the group sequence.
 *
 * @param [in] instance: ADC instance number
 * @param [in] seqIndex: ADC group seqeunce index
 * @param [out] result: ADC convertion result
 * @return none
 */
void ADC_DRV_GetSeqResult(const uint32_t instance,
        const adc_sequence_t seqIndex,
        uint16_t * const result);


/*!
 * @brief Get the trigger error flags.
 *
 * @param [in] instance: ADC instance number
 * @return
 */
uint32_t ADC_DRV_GetTriggerErrorFlags(const uint32_t instance);

/*!
 * @brief Clear the trigger error flag.
 *
 * @param [in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_ClearTriggerErrors(const uint32_t instance);

/*!
 * @brief Convert ADC value of T-Sensor to Temperature.
 *
 * @param[in] value: ADC conversion result of internal T-Sensor
 * @param[in] refVoltage: reference voltage of ADC, in mV
 * @return temperature value in celsius scale
 */
float ADC_DRV_ConvertToTemperature(const uint16_t value, const float refVoltage);

/*!
 * @brief Config ADC0 & ADC1 interleave function.
 *
 * @param[in] interleave: ADC0/1 interleave mode
 * @return none
 */
void ADC_DRV_SetInterleave(ctu_adc_interleave_mode_t interleave);

/*!
 * @brief Set adc callback function.
 *
 * @param[in] instance: ADC instance number
 * @param[in] callback: callback function from user
 * @param[in] parameter: parameter pointer which user want to use. Set to NULL if not used.
 * @return previous callback function
 */
adc_callback_t ADC_DRV_InstallCallback(const uint32_t instance, const adc_callback_t callback, void * parameter);

/*!
 * @brief ADC common interrupt service routine.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_CommonISR(const uint32_t instance);

/* ====================================  Functions definition  ======================================= */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ADC_DRV_H */

/* =============================================  EOF  ============================================== */
