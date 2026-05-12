/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written psramission of AutoChips inc. and/or its licensors, any
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
 * @file pct_drv.c
 *
 * @brief This file provides pct integration functions interface.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "pct_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */
#define PCT_PSR_PRESCALE_WIDTH                 (4UL)
#define PCT_MAX_CMR_NTICKS                     (PCT_CMR_CMR_Msk + 1UL)
#define PCT_MAX_PRESCALER                      (1UL << PCT_PSR_PRESCALE_WIDTH)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of base addresses for PCT instances */
static PCT_Type* const s_pctBase[PCT_INSTANCE_COUNT] = PCT_BASE_PTRS;
static const IRQn_Type s_pctIRQ = PCT_IRQS;
/* pct callback pointer */
static pct_callback_t s_pctCallback = NULL;

/* ====================================  Functions declaration  ===================================== */
/*
 * @brief Gets the prescaler coefficient value.
 *
 * @param[in] prescValue: the prescaler value configured in the PS_VAL register field
 * @param[in] bypass: enable or disable prescaler bypass
 * @return the value of prescaler to be used in the calculation process
 */
static inline uint8_t PCT_CfgToPre(const pct_prescaler_t prescValue, const bool bypass);

/*
 * @brief Gets the PCT counts number before the prescaler is considered.
 *
 * @param[in] clkFreq: the clock source value
 * @param[in] us: the microseconds value
 * @return the PCT counts number without prescaler
 */
static inline uint64_t PCT_UsToCounts(const uint32_t clkFreq, const uint32_t us);

/*
 * @brief Gets the ticks number after the prescaler is considered.
 *
 * @param[in] countsNum: the ticks counts number without prescaler
 * @param[in] prescCalcu: the prescaler value used in calculation
 * @return the ticks number with prescaler
 */
static inline uint64_t PCT_ComputeNticks(uint64_t countsNum, uint8_t prescCalcu);

/*
 * @brief Gets the ticks value to be written to CMR register.
 *
 * @param[in] nticks: the ticks value got by calculation
 * @param[out] ticks: the ticks value to be written to CMR register
 * @return the operation status
 */
static inline bool PCT_NticksToCompareTicks(uint64_t nticks, uint16_t *ticks);

/*
 * @brief Gets the clock frequency of PCT.
 *
 * @param[in] clkSrc: the selected clock source
 * @param[in] instance: instance number of PCT
 * @return the clock frequency of PCT
 */
static uint32_t PCT_GetClkFreq(const pct_clocksource_t clkSrc, const uint8_t instance);

/*
 * @brief Gets the microseconds value.
 *
 * @param[in] clkFreq: the selected clock source
 * @param[in] prescValue: the prescaler value
 * @param[in] bypass: enable or disable prescaler bypass
 * @param[in] ticks: the ticks value to be converted
 * @param[out] us: the microseconds to get
 * @return the operation status
 */
static bool PCT_TicksToUs(const uint32_t clkFreq, const pct_prescaler_t prescValue, const bool bypass,
                          const uint16_t ticks, uint32_t* const us);

/*
 * @brief Calculates the ticks value.
 *
 * @param[in] clkFreq: the selected clock source
 * @param[in] prescVal: the prescaler value
 * @param[in] bypass: enable or disable prescaler bypass
 * @param[in] us: the microseconds value to be converted
 * @param[out] ticks: the ticks value to get
 * @return the operation status
 */
static bool PCT_UsToTicks(const uint32_t clkFreq, const pct_prescaler_t prescVal, const bool bypass,
                          const uint32_t us, uint16_t* const ticks);

/*
 * @brief Gets the ticks value and the prescaler configuration.
 *
 * @param[in] clkFreq: the clock frequency
 * @param[in] us: the microseconds value
 * @param[out] prescVal: the prescaler value to get
 * @param[out] bypass: enable or disable state of prescaler bypass
 * @param[out] ticks: the ticks value to get
 * @return the operation status
 */
static bool PCT_ChooseClkConfig(const uint32_t clkFreq, const uint32_t us, pct_prescaler_t* const prescVal,
                                bool* const bypass, uint16_t* const ticks);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Gets the prescaler coefficient value.
 *
 * @param[in] prescValue: the prescaler value configured in the PS_VAL register field
 * @param[in] bypass: enable or disable prescaler bypass
 * @return the value of prescaler to be used in the calculation process
 */
static inline uint8_t PCT_CfgToPre(const pct_prescaler_t prescValue, const bool bypass)
{
    uint8_t prescCalcu = 0U;

    if (!bypass)
    {
        prescCalcu = (uint8_t)(((uint8_t)prescValue) + 1U);
    }

    return prescCalcu;
}

/*!
 * @brief Gets the PCT counts number before the prescaler is considered.
 *
 * @param[in] clkFreq: the clock source value
 * @param[in] us: the microseconds value
 * @return the PCT counts number without prescaler
 */
static inline uint64_t PCT_UsToCounts(const uint32_t clkFreq, const uint32_t us)
{
    uint64_t countsNum = (uint64_t)((uint64_t)us * (uint64_t)clkFreq);
    /* Rounding the countsNum value for better accuracy */
    countsNum = (countsNum + 500000U) / 1000000U;

    return countsNum;
}

/*!
 * @brief Gets the ticks number after the prescaler is considered.
 *
 * @param[in] countsNum: the ticks counts number without prescaler
 * @param[in] prescCalcu: the prescaler value used in calculation
 * @return the ticks number with prescaler
 */
static inline uint64_t PCT_ComputeNticks(uint64_t countsNum, uint8_t prescCalcu)
{
    uint64_t prescCoeff = (uint64_t)1U << prescCalcu;
    DEVICE_ASSERT(prescCoeff != 0U);
    uint64_t nticks = 0U;

    /* Rounding the nticks value for better accuracy */
    nticks = ((countsNum + (prescCoeff >> 1U)) / prescCoeff);

    return nticks;
}

/*!
 * @brief Gets the ticks value to be written to CMR register.
 *
 * @param[in] nticks: the ticks value got by calculation
 * @param[out] ticks: the ticks value to be written to CMR register
 * @return the operation status
 */
static inline bool PCT_NticksToCompareTicks(uint64_t nticks, uint16_t *ticks)
{
    bool successStatus = true;

    if (nticks <= PCT_MAX_CMR_NTICKS)
    {
        if (0U == nticks)
        {
            *ticks = 0U;
            successStatus = false;
        }
        else
        {
            *ticks = (uint16_t)(nticks - 1U);
        }
    }
    else
    {
        *ticks = PCT_CMR_CMR_Msk;
        successStatus = false;
    }

    return successStatus;
}

/*!
 * @brief Gets the clock frequency of PCT.
 *
 * @param[in] clkSrc: the selected clock source
 * @param[in] instance: instance number of PCT
 * @return the clock frequency of PCT
 */
static uint32_t PCT_GetClkFreq(const pct_clocksource_t clkSrc, const uint8_t instance)
{
    static const clock_names_t pctPccClockName[PCT_INSTANCE_COUNT] = {PCT_CLK};
    clock_names_t inputClockName = HSI_CLK;
    uint32_t      clkFreq;
    status_t      clkStatus;

    switch (clkSrc)
    {
    case PCT_CLOCKSOURCE_HSIDIV2:
        inputClockName = HSI_CLK;
        break;

    case PCT_CLOCKSOURCE_LSI1K:
        inputClockName = LSI_1K_CLK;
        break;

    case PCT_CLOCKSOURCE_RTC:
        inputClockName = RTC_CLK;
        break;

    case PCT_CLOCKSOURCE_PCC:
        inputClockName = pctPccClockName[instance];
        break;

    default:
        DEVICE_ASSERT(false);
        break;
    }

    if (HSI_CLK == inputClockName)
    {
        clkStatus = CKGEN_DRV_GetFreq(HSIDIV2_CLK, &clkFreq);
        DEVICE_ASSERT(STATUS_SUCCESS == clkStatus);
        (void) clkStatus;
        DEVICE_ASSERT(clkFreq != 0U);
    }
    else
    {
        clkStatus = CKGEN_DRV_GetFreq(inputClockName, &clkFreq);
        DEVICE_ASSERT(STATUS_SUCCESS == clkStatus);
        (void) clkStatus;
        DEVICE_ASSERT(clkFreq != 0U);
    }

    return clkFreq;
}

/*!
 * @brief Gets the microseconds value.
 *
 * @param[in] clkFreq: the selected clock source
 * @param[in] prescValue: the prescaler value
 * @param[in] bypass: enable or disable prescaler bypass
 * @param[in] ticks: the ticks value to be converted
 * @param[out] us: the microseconds to get
 * @return the operation status
 */
static bool PCT_TicksToUs(const uint32_t clkFreq, const pct_prescaler_t prescValue, const bool bypass,
                          const uint16_t ticks, uint32_t* const us)
{
    DEVICE_ASSERT(clkFreq != 0U);

    bool successStatus = true;
    uint8_t prescCalcu = PCT_CfgToPre(prescValue, bypass);
    uint64_t counts = ((uint64_t)ticks + 1U) << prescCalcu;
    uint64_t usReal = (counts * 1000000U) / (clkFreq);
    uint32_t usLocal;

    if (usReal <= (0xFFFFFFFFU))
    {
        usLocal = (uint32_t)usReal;
    }
    else
    {
        usLocal = 0xFFFFFFFFU;
        successStatus = false;
    }

    *us = usLocal;

    return successStatus;
}

/*!
 * @brief Calculates the ticks value.
 *
 * @param[in] clkFreq: the selected clock source
 * @param[in] prescVal: the prescaler value
 * @param[in] bypass: enable or disable prescaler bypass
 * @param[in] us: the microseconds value to be converted
 * @param[out] ticks: the ticks value to get
 * @return the operation status
 */
static bool PCT_UsToTicks(const uint32_t clkFreq, const pct_prescaler_t prescVal, const bool bypass,
                          const uint32_t us, uint16_t* const ticks)
{
    bool successStatus = true;
    uint8_t prescCalcu = 0U;
    uint64_t countsNum = 0U;
    uint64_t nticks = 0U;

    prescCalcu = PCT_CfgToPre(prescVal, bypass);
    countsNum = PCT_UsToCounts(clkFreq, us);
    nticks = PCT_ComputeNticks(countsNum, prescCalcu);
    successStatus = PCT_NticksToCompareTicks(nticks, ticks);

    return successStatus;
}

/*!
 * @brief Gets the ticks value and the prescaler configuration based on choosed clock.
 *
 * @param[in] clkFreq: the clock frequency
 * @param[in] us: the microseconds value
 * @param[out] prescVal: the prescaler value to get
 * @param[out] bypass: enable or disable state of prescaler bypass
 * @param[out] ticks: the ticks value to get
 * @return the operation status
 */
static bool PCT_ChooseClkConfig(const uint32_t clkFreq, const uint32_t us, pct_prescaler_t* const prescVal,
                                bool* const bypass, uint16_t* const ticks)
{
    uint8_t prescCalcu;
    uint64_t nticks = 0U;
    bool successStatus;

    uint64_t countsNum = PCT_UsToCounts(clkFreq, us);

    for (prescCalcu = 0U; prescCalcu <= PCT_MAX_PRESCALER; prescCalcu++)
    {
        nticks = PCT_ComputeNticks(countsNum, prescCalcu);

        if (nticks <= PCT_MAX_CMR_NTICKS)
        {
            break;
        }
    }

    successStatus = PCT_NticksToCompareTicks(nticks, ticks);

    if (0U == prescCalcu)
    {
        *bypass = true;
        *prescVal = PCT_PRESCALE_2;
    }
    else
    {
        *bypass = false;
        prescCalcu--;
        *prescVal = (pct_prescaler_t)prescCalcu;
    }

    return successStatus;
}

/*!
 * @brief Initializes configuration structure to default values.
 *
 * @param[out] config: the configuration structure to be initialized
 * @return none
 */
void PCT_DRV_InitConfigStruct(pct_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    config->interruptEnable = false;
    config->freeRun         = false;
    config->workMode        = PCT_WORKMODE_TIMER;
    config->clockSelect     = PCT_CLOCKSOURCE_HSIDIV2;
    config->prescaler       = PCT_PRESCALE_2;
    config->bypassPrescaler = false;
    config->compareValue    = 0U;
    config->counterUnits    = PCT_COUNTER_UNITS_TICKS;
    config->pinSelect       = PCT_PINSELECT_TRGMUX;
    config->pinPolarity     = PCT_PINPOLARITY_RISING;
}

/*!
 * @brief Initializes PCT.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] config: the configuration structure
 * @param[in] startCounter: starting counting control
 * @return none
 */
void PCT_DRV_Init(const uint8_t instance, const pct_config_t * const config, const bool startCounter)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);
    DEVICE_ASSERT(config != NULL);

    PCT_Type* const base = s_pctBase[instance];

    /* Enable PCT clock */
    CKGEN_DRV_Enable(CLK_PCT, true);
    CKGEN_DRV_SoftReset(SRST_PCT, true);

    PCT_DRV_SetConfig(instance, config);

    if (startCounter)
    {
        PCT_Enable(base);
    }
}

/*!
 * @brief Configure PCT.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] config: the configuration structure
 * @return none
 */
void PCT_DRV_SetConfig(const uint8_t instance, const pct_config_t * const config)
{
    uint32_t clkFreq;
    bool chooseClkConfigStatus;
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);
    DEVICE_ASSERT(config != NULL);

    PCT_Type* const base    = s_pctBase[instance];
    uint32_t configCmpValue = config->compareValue;
    pct_workmode_t configWorkMode = config->workMode;
    uint16_t cmpValueTicks   = 0U;
    pct_prescaler_t prescVal = config->prescaler;
    bool prescBypass = config->bypassPrescaler;
    pct_counter_units_t configCounterUnits = config->counterUnits;

    if (PCT_WORKMODE_TIMER == configWorkMode)
    {
        clkFreq = PCT_GetClkFreq(config->clockSelect, instance);
        DEVICE_ASSERT(clkFreq != 0U);
        PCT_SetCvalReadSel(base, config->cvalreadsel);

        if (PCT_COUNTER_UNITS_MICROSECONDS == configCounterUnits)
        {
            chooseClkConfigStatus = PCT_ChooseClkConfig(clkFreq, configCmpValue, &prescVal, &prescBypass, &cmpValueTicks);
            DEVICE_ASSERT(true == chooseClkConfigStatus);
            (void) chooseClkConfigStatus;
        }
        else
        {
            DEVICE_ASSERT(PCT_COUNTER_UNITS_TICKS == configCounterUnits);
            DEVICE_ASSERT(configCmpValue <= PCT_CMR_CMR_Msk);

            cmpValueTicks = (uint16_t)(configCmpValue & PCT_CMR_CMR_Msk);
        }
    }
    else
    {
        DEVICE_ASSERT(PCT_WORKMODE_PULSECOUNTER == configWorkMode);
        DEVICE_ASSERT(PCT_COUNTER_UNITS_TICKS == config->counterUnits);
        DEVICE_ASSERT((PCT_GetClkFreq(config->clockSelect, instance) != 0U) || prescBypass);
        DEVICE_ASSERT(prescBypass || (prescVal != PCT_PRESCALE_2));
        DEVICE_ASSERT(configCmpValue <= PCT_CMR_CMR_Msk);

        cmpValueTicks = (uint16_t)(configCmpValue & PCT_CMR_CMR_Msk);
        PCT_SetCvalReadSel(base, PCT_BCLK_FCLK_4);
    }

    PCT_Init(base);

    PCT_SetInterrupt(base, config->interruptEnable);
    if (config->callback != NULL)
    {
        PCT_DRV_InstallCallback(config->callback);
    }
    if (config->interruptEnable)
    {
        NVIC_EnableIRQ(s_pctIRQ);
    }
    else
    {
        NVIC_DisableIRQ(s_pctIRQ);
    }
    PCT_SetFreeRunning(base, config->freeRun);
    PCT_SetWorkMode(base, configWorkMode);
    PCT_SetPrescaler(base, prescVal);
    PCT_SetBypass(base, prescBypass);
    PCT_SetClockSelect(base, config->clockSelect);
    PCT_SetCompareValue(base, cmpValueTicks);
    PCT_SetPinSelect(base, config->pinSelect);
    PCT_SetPinPolarity(base, config->pinPolarity);
}

/*!
 * @brief Gets the configuration of PCT.
 *
 * @param[in] instance: instance number of PCT
 * @param[out] config: the configuration structure got
 * @return none
 */
void PCT_DRV_GetConfig(const uint8_t instance, pct_config_t * const config)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);
    DEVICE_ASSERT(config != NULL);
    const PCT_Type* const base = s_pctBase[instance];

    config->interruptEnable = PCT_GetInterruptEnable(base);
    config->freeRun         = PCT_GetFreeRunning(base);
    config->workMode        = PCT_GetWorkMode(base);
    config->prescaler       = PCT_GetPrescaler(base);
    config->bypassPrescaler = PCT_GetBypass(base);
    config->clockSelect     = PCT_GetClockSelect(base);
    config->compareValue    = PCT_GetCompareValue(base);
    config->counterUnits    = PCT_COUNTER_UNITS_TICKS;
    config->pinSelect       = PCT_GetPinSelect(base);
    config->pinPolarity     = PCT_GetPinPolarity(base);
}

/*!
 * @brief De-initialize PCT.
 *
 * @param[in] instance: instance number of PCT
 * @return none
 */
void PCT_DRV_Deinit(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    PCT_Type* const base = s_pctBase[instance];
    PCT_Disable(base);
    PCT_Init(base);
    /* Disbale PCT clock */
    CKGEN_DRV_SoftReset(SRST_PCT, false);
    CKGEN_DRV_Enable(CLK_PCT, false);
}

/*!
 * @brief Sets the compare value in counter unit.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] compareValueByCount: The compare value in counter ticks
 * @return Operation status
 */
status_t PCT_DRV_SetCompareValueByCount(const uint8_t instance, const uint16_t compareValueByCount)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    PCT_Type* const base = s_pctBase[instance];
    status_t statusCode = STATUS_SUCCESS;
    uint16_t counterVal;
    bool bypass = PCT_GetBypass(base);
    pct_workmode_t workMode = PCT_GetWorkMode(base);
    (void) bypass;
    (void) workMode;

    DEVICE_ASSERT((PCT_GetClkFreq(PCT_GetClockSelect(base), instance) != 0U) || \
                  (bypass && (PCT_WORKMODE_PULSECOUNTER == workMode)));

    PCT_SetCompareValue(base, compareValueByCount);
    counterVal = PCT_GetCounterValue(base);
    if (counterVal >= compareValueByCount)
    {
        statusCode = STATUS_TIMEOUT;
    }

    return statusCode;
}

/*!
 * @brief Gets the compare value in counter tick units.
 *
 * @param[in] instance: instance number of PCT
 * @param[out] compareValueByCount: current compare value in counter ticks
 * @return none
 */
void PCT_DRV_GetCompareValueByCount(const uint8_t instance, uint16_t * const compareValueByCount)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    const PCT_Type* const base = s_pctBase[instance];

    *compareValueByCount = PCT_GetCompareValue(base);
}

/*!
 * @brief Sets the compare value in microseconds unit.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] compareValueUs: Compare value in microseconds unit
 * @return Operation status
 */
status_t PCT_DRV_SetCompareValueByUs(const uint8_t instance, const uint32_t compareValueUs)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    status_t returnCode   = STATUS_SUCCESS;
    PCT_Type* const base  = s_pctBase[instance];
    pct_clocksource_t clkSrc;
    uint32_t clkFreq;
    uint16_t cmpValTicks, currentCounterVal;
    pct_prescaler_t prescVal;
    bool prescBypass, conversionStatus;

    DEVICE_ASSERT(PCT_WORKMODE_TIMER == PCT_GetWorkMode(base));

    clkSrc  = PCT_GetClockSelect(base);
    clkFreq = PCT_GetClkFreq(clkSrc, instance);
    DEVICE_ASSERT(clkFreq != 0U);

    prescVal    = PCT_GetPrescaler(base);
    prescBypass = PCT_GetBypass(base);
    conversionStatus = PCT_UsToTicks(clkFreq, prescVal, prescBypass, compareValueUs, &cmpValTicks);
    DEVICE_ASSERT(true == conversionStatus);
    (void) conversionStatus;

    PCT_SetCompareValue(base, cmpValTicks);
    currentCounterVal = PCT_GetCounterValue(base);

    if (currentCounterVal >= cmpValTicks)
    {
        returnCode = STATUS_TIMEOUT;
    }

    return returnCode;
}

/*!
 * @brief Gets the compare value in microseconds.
 *
 * @param[in] instance: instance number of PCT
 * @param[out] compareValueUs: current compare value in microseconds
 * @return none
 */
void PCT_DRV_GetCompareValueByUs(const uint8_t instance, uint32_t * const compareValueUs)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);
    DEVICE_ASSERT(compareValueUs != NULL);

    const PCT_Type* const base = s_pctBase[instance];
    pct_clocksource_t clkSrc;
    uint32_t clkFreq;
    uint16_t cmpValTicks;
    pct_prescaler_t prescVal;
    bool prescBypass, conversionStatus;

    DEVICE_ASSERT(PCT_WORKMODE_TIMER == PCT_GetWorkMode(base));

    clkSrc  = PCT_GetClockSelect(base);
    clkFreq = PCT_GetClkFreq(clkSrc, instance);
    DEVICE_ASSERT(clkFreq != 0U);

    prescVal    = PCT_GetPrescaler(base);
    prescBypass = PCT_GetBypass(base);
    cmpValTicks = PCT_GetCompareValue(base);

    conversionStatus = PCT_TicksToUs(clkFreq, prescVal, prescBypass, cmpValTicks, compareValueUs);
    DEVICE_ASSERT(true == conversionStatus);
    (void) conversionStatus;
}

/*!
 * @brief Gets the compare flag of PCT.
 *
 * @param[in] instance: instance number of PCT
 * @return the compare flag
 */
bool PCT_DRV_GetCompareFlag(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    const PCT_Type* const base = s_pctBase[instance];
    bool compareFlag = PCT_GetCompareFlag(base);

    return compareFlag;
}

/*!
 * @brief Clears the compare flag of PCT .
 *
 * @param[in] instance: instance number of PCT
 * @return none
 */
void PCT_DRV_ClearCompareFlag(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    PCT_Type* const base = s_pctBase[instance];

    PCT_ClearCompareFlag(base);
}

/*!
 * @brief Gets if PCT is running.
 *
 * @param[in] instance: instance number of PCT
 * @return The running state of PCT
 */
bool PCT_DRV_IsRunning(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    const PCT_Type* const base = s_pctBase[instance];
    bool runningState = PCT_GetEnable(base);

    return runningState;
}

/*!
 * @brief Enable/disable PCT interrupt.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] enableInterrupt: The PCT interrupt enable flag
 * @return none
 */
void PCT_DRV_SetInterrupt(const uint8_t instance, const bool enableInterrupt)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    PCT_Type* const base = s_pctBase[instance];

    PCT_SetInterrupt(base, enableInterrupt);
}

/*!
 * @brief Gets the current counter value in counter units.
 *
 * @param[in] instance: instance number of PCT
 * @return The current counter value
 */
uint16_t PCT_DRV_GetCounterValueByCount(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    PCT_Type* const base = s_pctBase[instance];
    uint16_t counterVal = PCT_GetCounterValue(base);

    return counterVal;
}

/*!
 * @brief Starts the counter.
 *
 * @param[in] instance: instance number of PCT
 * @return none
 */
void PCT_DRV_StartCounter(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    PCT_Type* const base = s_pctBase[instance];

    bool bypass = PCT_GetBypass(base);
    pct_workmode_t workMode = PCT_GetWorkMode(base);
    (void) bypass;
    (void) workMode;

    DEVICE_ASSERT((PCT_GetClkFreq(PCT_GetClockSelect(base), instance) != 0U) \
                  || (bypass && (PCT_WORKMODE_PULSECOUNTER == workMode)));

    PCT_Enable(base);
}

/*!
 * @brief Stops the counter.
 *
 * @param[in] instance: instance number of PCT
 * @return none
 */
void PCT_DRV_StopCounter(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    PCT_Type* const base = s_pctBase[instance];

    PCT_Disable(base);
}

/*!
 * @brief Sets the Input Pin configuration for Pulse Count mode.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] pinSelect: PCT pin selection
 * @param[in] pinPolarity: Polarity of counting pin(at rising/falling edge)
 * @return none
 */
void PCT_DRV_SetPinConfiguration(const uint8_t instance, const pct_pinselect_t pinSelect,
                                 const pct_pinpolarity_t pinPolarity)
{
    DEVICE_ASSERT(instance < PCT_INSTANCE_COUNT);

    PCT_Type* const base = s_pctBase[instance];

    PCT_SetPinSelect(base, pinSelect);
    PCT_SetPinPolarity(base, pinPolarity);
}

/*!
 * @brief Installs pct callback function.
 *
 * @param[in] func: PCT interrupt callback
 * @return none
 */
void PCT_DRV_InstallCallback(const pct_callback_t func)
{
    s_pctCallback = func;
}

/*!
 * @brief PCT common interrupt service routine.
 *
 * @return none
 */
void PCT_DRV_IRQHandler(void)
{
    uint32_t wpara = 0U;

    /* get timeout status */
    wpara = PCT_DRV_GetCompareFlag(0U);

    if (wpara)
    {
        /* clear timeout status */
        PCT_DRV_ClearCompareFlag(0U);
        s_pctCallback(PCT, wpara, 0U);
    }
}

/* =============================================  EOF  ============================================== */
