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
 * @file adc_drv.c
 *
 * @brief This file provides analog to digital converter module integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include <math.h>
#include "ckgen_drv.h"
#include "adc_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief ADC interrupt struct
 */
typedef struct {
    adc_callback_t callback;    /*!< callback funciton pointer */
    void *parameter;            /*!< customize common parameter pointer */
} adc_interrupt_t;

/* ==========================================  Variables  =========================================== */
/* Table of ADC base address */
static ADC_Type * const s_adcBase[] = ADC_BASE_PTRS;

/* Table of ADC IRQs */
static const IRQn_Type s_adcIRQ[] = ADC_IRQS;

/* Table of ADC ckgen interface clocks */
static const ckgen_clock_t s_adcCkgenClk[] = ADC_CKGEN_CLOCKS;

/* Table of ADC function clock names for configuration */
static const clock_names_t s_adcClkNames[] = ADC_CLOCK_NAMES;

/* Table of ADC soft resets */
static const ckgen_softreset_t s_adcSoftReset[] = ADC_SOFT_RESETS;

/*!< Table of adc interrtup handler information */
static adc_interrupt_t s_adcISR[ADC_INSTANCE_MAX];

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initialize ADC module, which is enable its clock and deassert its reset signal
 *
 * Before calling any other ADC driver API, this function should be called.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_Init(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    CKGEN_DRV_SoftReset(s_adcSoftReset[instance], true);
    CKGEN_DRV_Enable(s_adcCkgenClk[instance], true);
}

/*!
 * @brief Deinitialize ADC module, which is disable its clock and assert its reset signal
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_Deinit(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    CKGEN_DRV_Enable(s_adcCkgenClk[instance], false);
    CKGEN_DRV_SoftReset(s_adcSoftReset[instance], false);
}

/*!
 * @brief reset the ADC register.
 *
 * @param [in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_Reset(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    IRQn_Type irq = ADC_DRV_GetInterruptNumber(instance);

    /* Disable ADC interrupt */
    NVIC_DisableIRQ(irq);
    NVIC_ClearPendingIRQ(irq);

    CKGEN_DRV_SoftReset(s_adcSoftReset[instance], false);
    CKGEN_DRV_SoftReset(s_adcSoftReset[instance], true);
}

/*!
 * @brief ADC initialization delay.
 *
 * @return none
 */
static int ADC_InitDelay(void)
{
    int i = 0U;
    const int count = 1000U;

    for (i = 0; i < count; i++)
    {
        __asm("nop");
    }

    return 0;
}

/*!
 * @brief Get average value of ADC convert result from assigned sequence.
 *
 * @param[in] instance: ADC instance number
 * @param[in] seq: ADC seqeunce
 * @param[in] count: average count
 * @return averaged ADC result
 */
static float ADC_GetAverageValue(const uint32_t instance, const adc_sequence_t seq, const uint32_t count)
{
    DEVICE_ASSERT(count > 0U);
    DEVICE_ASSERT(seq < ADC_SEQ_MAX);

    float sum = 0;
    uint32_t i = 0U;
    uint16_t value = 0U;

    for (i = 0; i < count; i++)
    {
        ADC_DRV_SoftwareStartRegularConvert(instance);

        /* Wait for convertion complete */
        while (!ADC_DRV_GetConvCompleteFlag(instance, seq))
        {
            ;
        }

        ADC_DRV_GetSeqResult(instance, seq, &value);
        sum += value;
    }

    return (sum / count);
}

/*!
 * @brief ADC self calibration function, canbe used after calibration setting is configured.
 *
 * @param[in] instance: ADC instance number
 * @param[out] config: calibration config result
 * @param[in] seq: ADC seqeunce
 * @return none
 */
static void ADC_SelfCalibration(const uint32_t instance, adc_calibration_t * const config,
            const adc_sequence_t seq)
{
    DEVICE_ASSERT(NULL == config);
    DEVICE_ASSERT(seq < ADC_SEQ_MAX);

    float y1 = 0, y2 = 0, x1 = 0, x2 = 0, ge = 0, oe = 0;
    const int avgCount = 16U;

    /* Get calibration value */
    ADC_SetGEOEVIN(instance, ANA_GEOECAL_REF_16_4);
    x1 = (float)4 / 16;    /* x1 from GEOEVIN value */

    y1 = ADC_GetAverageValue(instance, seq, avgCount);

    ADC_SetGEOEVIN(instance, ANA_GEOECAL_REF_16_12);
    x2 = (float)12 / 16;    /* x2 from GEOEVIN value */

    y2 = ADC_GetAverageValue(instance, seq, avgCount);

    /* Caculate calibration GE OE value */
    ge = (4095 * (x2 - x1) / (y2 - y1) - 1) * 4096;
    oe = (y2 * x1 - y1 * x2) * 4096 / (y2 - y1);
    DEVICE_ASSERT(ge <= 4095 && ge >= -4096);
    DEVICE_ASSERT(oe <= 1023 && oe >= -1024);

    config->userGain = (int16_t)round((double)ge);
    config->userOffset = (int16_t)round((double)oe);
}

/*!
 * @brief Auto calibration function for ADC.  This function should be called every time after
 * ADC reset or power on.
 *
 * @param [in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_AutoCalibration(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    adc_converter_config_t config, caliConvConfig;
    adc_chan_config_t chConfig, caliChConfig;
    adc_sequence_t seq = ADC_RSEQ_0;
    /* x1 & x2 are calibration voltage value, and y1 & y2 are the corresponding adc code */
    adc_calibration_t caliConfig;

    ADC_DRV_InitCalibrationStruct(&caliConfig);

    /* save convert config setting */
    ADC_DRV_GetConverterConfig(instance, &config);

    /* Save channel setting of regular sequence 0 */
    ADC_DRV_GetChanConfig(instance, seq, &chConfig);

    /* Config ADC analog register to generate ge oe calibration signal */
    ADC_SetGEOEVINEnableFlag(instance, true);

    /* config ADC convertion mode to mode 1 */
    ADC_DRV_InitConverterStruct(&caliConvConfig);

    caliConvConfig.clockDivide                = ADC_CLK_DIVIDE_16;
    caliConvConfig.alignment                  = ADC_DATA_ALIGN_RIGHT;
    caliConvConfig.dmaEnable                  = false;
    caliConvConfig.scanModeEn                 = false;
    caliConvConfig.continuousModeEn           = false;
    caliConvConfig.regularDiscontinuousModeEn = false;
    caliConvConfig.injectDiscontinuousModeEn  = false;
    caliConvConfig.intervalModeEn             = false;
    caliConvConfig.regularTrigger             = ADC_TRIGGER_INTERNAL;
    caliConvConfig.powerEn                    = true;
    caliConvConfig.voltageRef                 = config.voltageRef;

    ADC_DRV_ConfigConverter(instance, &caliConvConfig);

    /* Disable calibration caculation logic */
    ADC_DRV_DisableCalibration(instance);

    /* Config convertion channel */
    ADC_DRV_InitChanStruct(&caliChConfig);

    caliChConfig.channel = ADC_CH_GEOE_CAL;
    caliChConfig.interruptEn = false;
    caliChConfig.spt = ADC_SPT_CLK_185;

    ADC_DRV_ConfigChan(instance, seq, &caliChConfig);

    /* wait for ADC internal status to be stable */
    (void)ADC_InitDelay();

    /* calibrate external channel */
    ADC_SelfCalibration(instance, &caliConfig, seq);

    ADC_DRV_ConfigExternalCalibration(instance, &caliConfig);

    /* using external calibration parameter for internal channel */
    ADC_DRV_ConfigInternalCalibration(instance, &caliConfig);

    /* Restore ADC ANA setting */
    ADC_SetGEOEVINEnableFlag(instance, false);

    /* Restore ADC setting */
    ADC_DRV_ConfigConverter(instance, &config);
    ADC_DRV_ConfigChan(instance, seq, &chConfig);

    /* Enable calibration caculation after GE OE is ready */
    ADC_DRV_EnableCalibration(instance);
}

/*!
 * @brief get interrupt number for ADC instance
 *
 * @param [in] instance: ADC instance number
 * @return ADC interrupt number for instance
 */
IRQn_Type ADC_DRV_GetInterruptNumber(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    return s_adcIRQ[instance];
}

/*!
 * @brief Initialization convert structure
 *
 * @param [out] config: convert config structure pointer
 * @return none
 */
void ADC_DRV_InitConverterStruct(adc_converter_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    config->clockDivide                = ADC_CLK_DIVIDE_16;
    config->resolution                 = ADC_RESOLUTION_12BIT;
    config->alignment                  = ADC_DATA_ALIGN_RIGHT;

    config->regularTrigger             = ADC_TRIGGER_INTERNAL;
    config->injectTrigger              = ADC_TRIGGER_INTERNAL;

    config->dmaEnable                  = false;

    config->voltageRef                 = ADC_VOLTAGEREF_VREF;

    /* Conversion mode */
    config->scanModeEn                 = false;
    config->continuousModeEn           = false;
    config->regularDiscontinuousModeEn = false;
    config->injectDiscontinuousModeEn  = false;
    config->injectAutoModeEn           = false;
    config->intervalModeEn             = false;
    config->regularDiscontinuousNum    = 0U;

    config->regularSequenceLength      = 0U;
    config->injectSequenceLength       = 0U;

    config->callback                   = NULL;
    config->parameter                  = NULL;

    config->powerEn                    = false;
}

/*!
 * @brief config ADC convert setting
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: convert config that need to config
 * @return none
 */
void ADC_DRV_ConfigConverter(const uint32_t instance,
        const adc_converter_config_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);
    DEVICE_ASSERT(ADC_REGULAR_SEQUENCE_LENGTH >= config->regularSequenceLength);
    DEVICE_ASSERT(ADC_INJECT_SEQUENCE_LENGTH >= config->injectSequenceLength);

    ADC_Type * const base = s_adcBase[instance];
    uint32_t frequence = 0;

    (void)CKGEN_DRV_GetFreq(s_adcClkNames[instance], &frequence);
    frequence = frequence / ((uint32_t)config->clockDivide + 1);
    DEVICE_ASSERT(frequence < ADC_CLOCK_FREQ_MAX_RUNTIME);
    (void)frequence;

    /* Set clock divide */
    ADC_SetClockPrescaler(base, config->clockDivide);

    /* Set DMA enable flag */
    ADC_SetDMAEnableFlag(base, config->dmaEnable);

    /* Set resolution */
    ADC_SetResolution(base, config->resolution);

    /* Set alignment */
    ADC_SetDataAlign(base, config->alignment);

    /* Set voltage reference; */
    ADC_SetVoltageReference(base, config->voltageRef);

    /* Set adc mode */
    ADC_SetScanEnableFlag(base, config->scanModeEn);
    ADC_SetContinuousEnableFlag(base, config->continuousModeEn);
    ADC_SetRegularDiscontinuousEnableFlag(base, config->regularDiscontinuousModeEn);
    ADC_SetInjectDiscontinuousEnableFlag(base, config->injectDiscontinuousModeEn);
    ADC_SetInjectAutoEnableFlag(base, config->injectAutoModeEn);
    ADC_SetIntervalEnableFlag(base, config->intervalModeEn);
    ADC_SetRegularDiscontinuousNum(base, config->regularDiscontinuousNum);

    /* Set Regular/Inject group trigger source */
    ADC_SetRegularTriggerSource(base, config->regularTrigger);
    ADC_SetInjectTriggerSource(base, config->injectTrigger);

    /* Set adc sequence length */
    ADC_SetRegularLength(base, config->regularSequenceLength);
    ADC_SetInjectLength(base, config->injectSequenceLength);

    /* Enable ADC interrupt */
    NVIC_EnableIRQ(ADC_DRV_GetInterruptNumber(instance));

    (void)ADC_DRV_InstallCallback(instance, config->callback, config->parameter);

    /* Set power */
    ADC_SetPowerEnableFlag(base, config->powerEn);

    (void)ADC_InitDelay();
}

/*!
 * @brief Get ADC converter config from register
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: config pointer that need to fill with current register config
 * @return none
 */
void ADC_DRV_GetConverterConfig(const uint32_t instance,
        adc_converter_config_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    const ADC_Type * const base = s_adcBase[instance];

    config->clockDivide                = ADC_GetClockPrescaler(base);
    config->resolution                 = ADC_GetResolution(base);
    config->alignment                  = ADC_GetDataAlign(base);

    config->regularTrigger             = ADC_GetRegularTriggerSource(base);
    config->injectTrigger              = ADC_GetInjectTriggerSource(base);

    config->dmaEnable                  = ADC_GetDMAEnableFlag(base);

    config->voltageRef                 = ADC_GetVoltageReference(base);

    config->scanModeEn                 = ADC_GetScanEnableFlag(base);
    config->continuousModeEn           = ADC_GetContinuousEnableFlag(base);
    config->regularDiscontinuousModeEn = ADC_GetRegularDiscontinuousEnableFlag(base);
    config->injectDiscontinuousModeEn  = ADC_GetInjectDiscontinuousEnableFlag(base);
    config->injectAutoModeEn           = ADC_GetInjectAutoEnableFlag(base);
    config->intervalModeEn             = ADC_GetIntervalEnableFlag(base);
    config->regularDiscontinuousNum    = ADC_GetRegularDiscontinuousNum(base);

    config->regularSequenceLength      = ADC_GetRegularLength(base);
    config->injectSequenceLength       = ADC_GetInjectLength(base);

    config->callback                   = s_adcISR[instance].callback;
    config->parameter                  = s_adcISR[instance].parameter;

    config->powerEn                    = ADC_GetPowerEnableFlag(base);
}

/*!
 * @brief initialize Analog Monitor config structure
 *
 * @param [out] config: pointer to the config that need to be initialized
 * @return none
 */
void ADC_DRV_InitAMOStruct(adc_amo_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    config->amoTriggerMode = ADC_AMO_TRIGGER_LEVEL;

    config->amoInterruptEn = false;

    config->amoRegularEn = false;
    config->amoInjectEn = false;
    config->amoSingleModeEn = false;
    config->amoSingleChannel = ADC_CH_0;

    config->amoUpThreshold = 0U;
    config->amoLowThreshold = 0U;
    config->amoUpOffset = 0U;
    config->amoLowOffset = 0U;
}

/*!
 * @brief config Analog Monitor function
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: pointer to configuration that need to apply to the register
 * @return none
 */
void ADC_DRV_ConfigAMO(const uint32_t instance,
        const adc_amo_config_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);
    DEVICE_ASSERT(config->amoUpThreshold > config->amoLowThreshold);
    DEVICE_ASSERT(config->amoUpOffset > config->amoLowOffset);

    ADC_Type * const base = s_adcBase[instance];

    ADC_SetAMOTriggerMode(base, config->amoTriggerMode);
    ADC_SetAMORegularEnableFlag(base, config->amoRegularEn);
    ADC_SetAMOInjectEnableFlag(base, config->amoInjectEn);
    ADC_SetAMOSingleChannelEnableFlag(base, config->amoSingleModeEn);
    ADC_SetAMOSingleChannel(base, config->amoSingleChannel);

    ADC_SetAMOUpThreshold(base, config->amoUpThreshold);
    ADC_SetAMOLowThreshold(base, config->amoLowThreshold);
    ADC_SetAMOUpOffset(base, config->amoUpOffset);
    ADC_SetAMOLowOffset(base, config->amoLowOffset);

    ADC_SetAMOInterruptEnableFlag(base, config->amoInterruptEn);
}

/*!
 * @brief Get the configuration of AMO function
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: pointer that need to be filled with AMO register configuration
 * @return none
 */
void ADC_DRV_GetAMOConfig(const uint32_t instance,
        adc_amo_config_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    const ADC_Type * const base = s_adcBase[instance];

    config->amoTriggerMode = ADC_GetAMOTriggerMode(base);
    config->amoSingleModeEn = ADC_GetAMOSingleChannelEnableFlag(base);
    config->amoSingleChannel = ADC_GetAMOSingleChannel(base);
    config->amoRegularEn = ADC_GetAMORegularEnableFlag(base);
    config->amoInjectEn = ADC_GetAMOInjectEnableFlag(base);

    config->amoUpThreshold = ADC_GetAMOUpThreshold(base);
    config->amoLowThreshold = ADC_GetAMOLowThreshold(base);

    config->amoUpOffset = ADC_GetAMOUpOffset(base);
    config->amoLowOffset = ADC_GetAMOLowOffset(base);

    config->amoInterruptEn = ADC_GetAMOInterruptEnableFlag(base);
}

/*!
 * @brief Initialization average config structure
 *
 * @param [out] config: average configuration structure that need to be initialized
 * @return none
 */
void ADC_DRV_InitHwAverageStruct(adc_average_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    config->hwAvgEnable = false;
    config->hwAverage = ADC_AVERAGE_4;
}

/*!
 * @brief Apply the average function configuration to the hardware
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: configuration that need to apply to average function
 * @return none
 */
void ADC_DRV_ConfigHwAverage(const uint32_t instance,
        const adc_average_config_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    ADC_Type * const base = s_adcBase[instance];

    ADC_SetAverageEnableFlag(base, config->hwAvgEnable);
    ADC_SetAverageMode(base, config->hwAverage);
}

/*!
 * @brief Get average function configuration form register
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: pointer that need to be filled with average register
 *                      configuration
 * @return none
 */
void ADC_DRV_GetHwAverageConfig(const uint32_t instance,
        adc_average_config_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    const ADC_Type * const base = s_adcBase[instance];

    config->hwAvgEnable = ADC_GetAverageEnableFlag(base);
    config->hwAverage = ADC_GetAverageMode(base);
}

/*!
 * @brief Initialization channel config structure
 *
 * @param [out] config: channel configuration structure that need to be initialized
 * @return none
 */
void ADC_DRV_InitChanStruct(adc_chan_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    config->spt = ADC_SPT_CLK_5;
    config->channel = ADC_CH_DISABLE;
    config->supply = ADC_CH_SUPPLY_REFH;
    config->interruptEn = false;
}

/*!
 * @brief Apply the channel configuration to the hardware
 *
 * @param [in] instance: ADC instance number
 * @param [in] seqIndex ADC group seqeunce index
 * @param [in] config: configuration that need to apply to the group sequence function
 * @return none
 */
void ADC_DRV_ConfigChan(const uint32_t instance,
        const adc_sequence_t seqIndex,
        const adc_chan_config_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(seqIndex < ADC_SEQ_MAX);
    DEVICE_ASSERT(config != NULL);

    ADC_Type * const base = s_adcBase[instance];

    ADC_SetChannelSampleTime(base, config->channel, config->spt);

    if ((uint32_t)seqIndex < ADC_REGULAR_SEQ_NUM)
    {
        ADC_SetRegularConversionChannel(base, seqIndex, config->channel);
        ADC_SetRegularEOCInterruptEnableFlag(base, seqIndex, config->interruptEn);
    }
    else if (seqIndex >= ADC_ISEQ_0 && seqIndex < ADC_SEQ_MAX)
    {
        /* injection group */
        ADC_SetInjectConversionChannel(base, seqIndex, config->channel);
        ADC_SetInjectEOCInterruptEnableFlag(base, seqIndex, config->interruptEn);
    }
    else
    {
    }

    if (ADC_CH_SUPPLY == config->channel)
    {
        ADC_SetSupplyChannel(base, config->supply);
    }
}

/*!
 * @brief Get group sequence channel configuration form register
 *
 * @param [in] instance: ADC instance number
 * @param [in] seqIndex ADC group seqeunce index
 * @param [out] config: pointer that need to be filled with group sequence channel
 *                      register
 * @return none
 */
void ADC_DRV_GetChanConfig(const uint32_t instance,
        const adc_sequence_t seqIndex,
        adc_chan_config_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(seqIndex < ADC_SEQ_MAX);
    DEVICE_ASSERT(config != NULL);

    const ADC_Type * const base = s_adcBase[instance];
    if ((uint32_t)seqIndex < ADC_REGULAR_SEQ_NUM)
    {
        config->interruptEn = ADC_GetRegularEOCInterruptEnableFlag(base, seqIndex);
        config->channel = ADC_GetRegularConversionChannel(base, seqIndex);
    }
    else if (seqIndex >= ADC_ISEQ_0 && seqIndex < ADC_SEQ_MAX)
    {
        config->interruptEn = ADC_GetInjectEOCInterruptEnableFlag(base, seqIndex);
        config->channel = ADC_GetInjectConversionChannel(base, seqIndex);
    }
    else
    {
    }
    config->spt = ADC_GetChannelSampleTime(base, config->channel);
    if (ADC_CH_SUPPLY == config->channel)
    {
        config->supply = ADC_GetSupplyChannel(base);
    }
}

/*!
 * @brief Enable ADC calibration caculartion function
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_EnableCalibration(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    ADC_Type * const base = s_adcBase[instance];

    ADC_SetCalibrationEnableFlag(base, true);
}

/*!
 * @brief Disable ADC calibration caculartion function
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_DisableCalibration(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    ADC_Type * const base = s_adcBase[instance];

    ADC_SetCalibrationEnableFlag(base, false);
}

/*!
 * @brief Initialization calibartion config structure
 *
 * @param [out] config: calibartion configuration structure that need to be initialized
 * @return none
 */
void ADC_DRV_InitCalibrationStruct(adc_calibration_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    config->userGain = 0U;
    config->userOffset = 0U;
}

/*!
 * @brief Apply the calibration configuration to the external channel
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: configuration that need to apply to external channel calibartion function
 * @return none
 */
void ADC_DRV_ConfigExternalCalibration(const uint32_t instance,
        const adc_calibration_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    ADC_Type * const base = s_adcBase[instance];

    ADC_SetGainOffset0Value(base, config->userGain, config->userOffset);
}

/*!
 * @brief Get external channel calibration configuration form register
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: pointer that need to be filled with external channel calibration register
 * @return none
 */
void ADC_DRV_GetExternalCalibration(const uint32_t instance,
        adc_calibration_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    const ADC_Type * const base = s_adcBase[instance];

    ADC_GetGainOffset0Value(base, &config->userGain, &config->userOffset);
}

/*!
 * @brief Apply the calibration configuration to the internal channel
 *
 * @param [in] instance: ADC instance number
 * @param [in] config: configuration that need to apply to internal channel calibartion function
 * @return none
 */
void ADC_DRV_ConfigInternalCalibration(const uint32_t instance,
        const adc_calibration_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    ADC_Type * const base = s_adcBase[instance];

    ADC_SetGainOffset1Value(base, config->userGain, config->userOffset);
}

/*!
 * @brief Get internal channel calibration configuration form register
 *
 * @param [in] instance: ADC instance number
 * @param [out] config: pointer that need to be filled with internal channel calibration register
 * @return none
 */
void ADC_DRV_GetInternalCalibration(const uint32_t instance,
        adc_calibration_t * const config)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    const ADC_Type * const base = s_adcBase[instance];

    ADC_GetGainOffset1Value(base, &config->userGain, &config->userOffset);
}

/*!
 * @brief Start software trigger regular group convertion
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_SoftwareStartRegularConvert(uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    ADC_Type *const base = s_adcBase[instance];

    ADC_SoftwareStartRegularConvert(base);
}

/*!
 * @brief Start software trigger inject group convertion
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_SoftwareStartInjectConvert(uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    ADC_Type * const base = s_adcBase[instance];

    ADC_SoftwareStartInjectConvert(base);
}

/*!
 * @brief Get ADC Idle flag.
 *
 * @param[in] instance: ADC instance number
 * @return whether ADC is idle
 *          - true: ADC is not converting
 *          - false: ADC is busy converting
 */
bool ADC_DRV_GetIdleFlag(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    const ADC_Type * const base = s_adcBase[instance];

    return ADC_GetIdleFlag(base);
}

/*!
 * @brief Wait for the ADC begin to convert.
 *
 * After software trigger regualr or injection conversion,
 * the idle flag may still be 1 for a few microsecond, use this API to wait before ADC start conversion.
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_WaitConvStart(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    const ADC_Type * const base = s_adcBase[instance];

    while (ADC_GetIdleFlag(base))
    {
        /* Wait */
    }
}

/*!
 * @brief Wait for the ADC conversion to finish.
 *
 * @param [in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_WaitConvDone(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    const ADC_Type * const base = s_adcBase[instance];

    while (!ADC_GetIdleFlag(base))
    {
        /* Wait */
    }
}

/*!
 * @brief Get convertion complete flag form register.
 *
 * @param [in] instance: ADC instance number
 * @param [in] seqIndex ADC group seqeunce index
 * @return the convertion complete flag of the group sequence
 */
bool ADC_DRV_GetConvCompleteFlag(const uint32_t instance, const adc_sequence_t seqIndex)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(seqIndex < ADC_SEQ_MAX);

    const ADC_Type * const base = s_adcBase[instance];
    bool status = false;

    if ((uint32_t)seqIndex < ADC_REGULAR_SEQ_NUM)
    {
        status = ADC_GetRegularEOCFlag(base, seqIndex);
    }
    else if (seqIndex >= ADC_ISEQ_0 && seqIndex < ADC_SEQ_MAX)
    {
        status = ADC_GetInjectEOCFlag(base, seqIndex);
    }
    else
    {
    }

    return status;
}

/*!
 * @brief Clear convertion complete flag in register.
 *
 * @param[in] instance: ADC instance number
 * @param[in] seqIndex: ADC group seqeunce index
 */
void ADC_DRV_ClearConvCompleteFlag(const uint32_t instance,
        const adc_sequence_t seqIndex)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(seqIndex < ADC_SEQ_MAX);

    ADC_Type * const base = s_adcBase[instance];

    if ((uint32_t)seqIndex < ADC_REGULAR_SEQ_NUM)
    {
        ADC_ClearRegularEOCFlag(base, seqIndex);
    }
    else if (seqIndex >= ADC_ISEQ_0 && seqIndex < ADC_SEQ_MAX)
    {
        ADC_ClearInjectEOCFlag(base, seqIndex);
    }
    else
    {
    }
}

/*!
 * @brief Get the ADC convertion result for the group sequence
 *
 * @param [in] instance: ADC instance number
 * @param [in] seqIndex ADC group seqeunce index
 * @param [out] result ADC convertion result
 * @return none
 */
void ADC_DRV_GetSeqResult(const uint32_t instance,
        const adc_sequence_t seqIndex,
        uint16_t * const result)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    DEVICE_ASSERT(seqIndex < ADC_SEQ_MAX);

    const ADC_Type * const base = s_adcBase[instance];

    if ((uint32_t)seqIndex < ADC_REGULAR_SEQ_NUM)
    {
        *result = ADC_GetRegularData(base, seqIndex);
    }
    else if (seqIndex >= ADC_ISEQ_0 && seqIndex < ADC_SEQ_MAX)
    {
        *result = ADC_GetInjectData(base, seqIndex);
    }
    else
    {
    }
}

/*!
 * @brief Get the trigger error flags
 *
 * @param [in] instance: ADC instance number
 * @return trigger error flags
 */
uint32_t ADC_DRV_GetTriggerErrorFlags(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    const ADC_Type * const base = s_adcBase[instance];
    return (READ_BIT32(base->STR, ADC_STR_COVCFT_Msk) >> ADC_STR_COVCFT_Pos);
}

/*!
 * @brief Clear the trigger error flag
 *
 * @param [in] instance: ADC instance number
 * @return none
 */
void ADC_DRV_ClearTriggerErrors(const uint32_t instance)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);
    ADC_Type * const base = s_adcBase[instance];
    ADC_ClearConvertConflictFlag(base);
}

/*!
 * @brief Convert ADC value of T-Sensor to Temperature.
 *
 * @param[in] value: ADC conversion result of internal T-Sensor
 * @param[in] refVoltage: reference voltage of ADC, in mV
 * @return temperature value in celsius scale
 */
float ADC_DRV_ConvertToTemperature(const uint16_t value, const float refVoltage)
{
    float temperature = 0;
    const float slop = 1.862, volTempBase = 673, tempBase = 25;

    temperature = (volTempBase - (float)value / 4096 * refVoltage) / slop + tempBase;

    return temperature;
}

/*!
 * @brief Config ADC0 & ADC1 interleave function.
 *
 * @param[in] interleave: ADC0/1 interleave mode
 * @return none
 */
void ADC_DRV_SetInterleave(ctu_adc_interleave_mode_t interleave)
{
    const uint32_t ctuInstance = 0;

    CTU_DRV_SetADCInterleave(ctuInstance, interleave);
    switch (interleave)
    {
    case CTU_ADC_INTERLEAVE_DISABLE:
        /* turn off RG_MODE */
        MODIFY_MEM32(&ANA->AUXADC_CFG0, ANA_AUXADC_CFG0_RG_ADC0_RGMODE_EN_Msk, ANA_AUXADC_CFG0_RG_ADC0_RGMODE_EN_Pos, 0);
        MODIFY_MEM32(&ANA->AUXADC_CFG4, ANA_AUXADC_CFG4_RG_ADC1_RGMODE_EN_Msk, ANA_AUXADC_CFG4_RG_ADC1_RGMODE_EN_Pos, 0);
        break;

    case CTU_ADC0_INTERLEAVE_VIN4:
        /* fall throught */
    case CTU_ADC0_INTERLEAVE_VIN5:
        MODIFY_MEM32(&ANA->AUXADC_CFG0, ANA_AUXADC_CFG0_RG_ADC0_LDO_EN_Msk, ANA_AUXADC_CFG0_RG_ADC0_LDO_EN_Pos, 1);
        MODIFY_MEM32(&ANA->AUXADC_CFG0, ANA_AUXADC_CFG0_RG_ADC0_VREFGEN_EN_Msk, ANA_AUXADC_CFG0_RG_ADC0_VREFGEN_EN_Pos, 1);
        MODIFY_MEM32(&ANA->AUXADC_CFG0, ANA_AUXADC_CFG0_RG_ADC0_RGMODE_EN_Msk, ANA_AUXADC_CFG0_RG_ADC0_RGMODE_EN_Pos, 1);
        MODIFY_MEM32(&ANA->AUXADC_CFG0, ANA_AUXADC_CFG0_RG_ADC0_EN_Msk, ANA_AUXADC_CFG0_RG_ADC0_EN_Pos, 1);
        if (CTU_ADC0_INTERLEAVE_VIN4 == interleave)
        {
            MODIFY_MEM32(&ANA->AUXADC_CFG0, ANA_AUXADC_CFG0_RG_ADC0_CHANNEL_SEL_Msk, ANA_AUXADC_CFG0_RG_ADC0_CHANNEL_SEL_Pos, 4);

            ADC_SetRegularConversionChannel(ADC0, ADC_RSEQ_0, ADC_CH_4);
            ADC_SetRegularConversionChannel(ADC1, ADC_RSEQ_0, ADC_CH_14);
            ADC_SetChannelSampleTime(ADC0, ADC_CH_4, ADC_SPT_CLK_15);
            ADC_SetChannelSampleTime(ADC1, ADC_CH_14, ADC_SPT_CLK_15);
        }
        else if (CTU_ADC0_INTERLEAVE_VIN5 == interleave)
        {
            MODIFY_MEM32(&ANA->AUXADC_CFG0, ANA_AUXADC_CFG0_RG_ADC0_CHANNEL_SEL_Msk, ANA_AUXADC_CFG0_RG_ADC0_CHANNEL_SEL_Pos, 5);

            ADC_SetRegularConversionChannel(ADC0, ADC_RSEQ_0, ADC_CH_5);
            ADC_SetRegularConversionChannel(ADC1, ADC_RSEQ_0, ADC_CH_15);
            ADC_SetChannelSampleTime(ADC0, ADC_CH_5, ADC_SPT_CLK_15);
            ADC_SetChannelSampleTime(ADC1, ADC_CH_15, ADC_SPT_CLK_15);
        }
        break;

    case CTU_ADC1_INTERLEAVE_VIN8:
        /* fall throught */
    case CTU_ADC1_INTERLEAVE_VIN9:
        /* enable ADC1 RG_MODE for ADC1 interleave */
        MODIFY_MEM32(&ANA->AUXADC_CFG4, ANA_AUXADC_CFG4_RG_ADC1_LDO_EN_Msk, ANA_AUXADC_CFG4_RG_ADC1_LDO_EN_Pos, 1);
        MODIFY_MEM32(&ANA->AUXADC_CFG4, ANA_AUXADC_CFG4_RG_ADC1_VREFGEN_EN_Msk, ANA_AUXADC_CFG4_RG_ADC1_VREFGEN_EN_Pos, 1);
        MODIFY_MEM32(&ANA->AUXADC_CFG4, ANA_AUXADC_CFG4_RG_ADC1_RGMODE_EN_Msk, ANA_AUXADC_CFG4_RG_ADC1_RGMODE_EN_Pos, 1);
        MODIFY_MEM32(&ANA->AUXADC_CFG4, ANA_AUXADC_CFG4_RG_ADC1_EN_Msk, ANA_AUXADC_CFG4_RG_ADC1_EN_Pos, 1);
        if (CTU_ADC1_INTERLEAVE_VIN8 == interleave)
        {
            MODIFY_MEM32(&ANA->AUXADC_CFG4, ANA_AUXADC_CFG4_RG_ADC1_CHANNEL_SEL_Msk, ANA_AUXADC_CFG4_RG_ADC1_CHANNEL_SEL_Pos, 8);

            ADC_SetRegularConversionChannel(ADC0, ADC_RSEQ_0, ADC_CH_14);
            ADC_SetRegularConversionChannel(ADC1, ADC_RSEQ_0, ADC_CH_8);
        }
        else if (CTU_ADC1_INTERLEAVE_VIN9 == interleave)
        {
            MODIFY_MEM32(&ANA->AUXADC_CFG4, ANA_AUXADC_CFG4_RG_ADC1_CHANNEL_SEL_Msk, ANA_AUXADC_CFG4_RG_ADC1_CHANNEL_SEL_Pos, 9);

            ADC_SetRegularConversionChannel(ADC0, ADC_RSEQ_0, ADC_CH_15);
            ADC_SetRegularConversionChannel(ADC1, ADC_RSEQ_0, ADC_CH_9);
        }
        break;
    }
}

/*!
 * @brief Set adc callback function
 *
 * @param[in] instance: ADC instance number
 * @param[in] callback: callback function from user
 * @param[in] parameter: parameter pointer which user want to use. Set to NULL if not used.
 * @return previous callback function
 */
adc_callback_t ADC_DRV_InstallCallback(const uint32_t instance, const adc_callback_t callback, void * parameter)
{
    DEVICE_ASSERT(instance < ADC_INSTANCE_MAX);

    adc_callback_t currentCallback = s_adcISR[instance].callback;

    s_adcISR[instance].callback = callback;
    s_adcISR[instance].parameter = parameter;

    return currentCallback;
}

/*!
 * @brief ADC common interrupt service routine
 *
 * @param[in] instance: ADC instance number
 * @return none
 */
void ADC_CommonISR(const uint32_t instance)
{
    ADC_Type * const base = s_adcBase[instance];
    adc_interrupt_info_t info = {0U};
    uint32_t event = 0U;
    const uint32_t amoMask = ADC_STR_AAMO_Msk | ADC_STR_NAMO_Msk | ADC_STR_AMO_Msk;
    /* take snapshot of current regular & injection group length */
    uint16_t regularLen = ADC_GetRegularLength(base);
    uint16_t injectLen = ADC_GetInjectLength(base);
    int i = 0U;
    adc_sequence_t seq = ADC_RSEQ_0;

    /* Restrict regularLen form above ADC_REGULAR_SEQ_NUM. No need for injection group for now */
    if (regularLen > ADC_REGULAR_SEQ_NUM)
    {
        regularLen = ADC_REGULAR_SEQ_NUM;
    }

    /* store device status */
    event = base->STR;
    event &= amoMask;

    /* clear device status, and clear AMO state only */
    base->STR = ~amoMask;

    info.instance = instance;

    /* call AMO interrupt handler */
    if (event > 0U
        && NULL != s_adcISR[instance].callback)
    {
        info.event = event;
        /* callback */
        s_adcISR[instance].callback(&info, s_adcISR[instance].parameter);
    }

    /* Process EOC interrupt */
    event = 0U;

    for (i = 0U; i < injectLen; i++)
    {
        seq = (adc_sequence_t)((uint32_t)ADC_ISEQ_0 + i);
        if (ADC_GetInjectEOCInterruptEnableFlag(base, seq)
            && ADC_GetInjectEOCFlag(base, seq))
        {
            /* clear EOC status */
            ADC_ClearInjectEOCFlag(base, seq);
            /* save event information */
            info.sequence = seq;
            event = (uint32_t)ADC_EVENT_EOC;
            break;
        }
    }

    /* call EOC interrupt handler */
    if ((uint32_t)ADC_EVENT_EOC == event
        && NULL != s_adcISR[instance].callback)
    {
        info.event = event;
        s_adcISR[instance].callback(&info, s_adcISR[instance].parameter);
    }

    event = 0;

    for (i = 0U; i < regularLen; i++)
    {
        seq = (adc_sequence_t)i;
        if (ADC_GetRegularEOCInterruptEnableFlag(base, seq)
            && ADC_GetRegularEOCFlag(base, seq))
        {
            /* clear EOC status */
            ADC_ClearRegularEOCFlag(base, seq);
            /* save event information */
            info.sequence = seq;
            event = (uint32_t)ADC_EVENT_EOC;
            break;
        }
    }

    /* call EOC interrupt handler */
    if ((uint32_t)ADC_EVENT_EOC == event
        && NULL != s_adcISR[instance].callback)
    {
        info.event = event;
        s_adcISR[instance].callback(&info, s_adcISR[instance].parameter);
    }
}

/* =============================================  EOF  ============================================== */
