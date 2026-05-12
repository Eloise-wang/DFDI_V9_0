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
 * @file pct_drv.h
 *
 * @brief This file provides pct functions interface.
 *
 */

#ifndef _PCT_DRV_H
#define _PCT_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< Callback for PCT module */
typedef void (*pct_callback_t)(void *device, uint32_t wpara, uint32_t lpara);

/*!
 * @brief Input Pulse selection
 */
typedef enum
{
    PCT_PINSELECT_TRGMUX = 0x00U, /*!<  pulse from TRGMUX trigger */
    PCT_PINSELECT_ALT1 = 0x01U,   /*!<  pulse from pin 1          */
    PCT_PINSELECT_ALT2 = 0x02U,   /*!<  pulse from pin 2          */
    PCT_PINSELECT_ALT3 = 0x03U    /*!<  pulse from pin 3          */
}pct_pinselect_t;

/*!
 * @brief Pulse count polarity
 */
typedef enum
{
    PCT_PINPOLARITY_RISING = 0U,  /*!< Count pulse on rising edge  */
    PCT_PINPOLARITY_FALLING = 1U  /*!< Count pulse on falling edge */
} pct_pinpolarity_t;

/*!
 * @brief PCT count Mode
 */
typedef enum
{
    PCT_WORKMODE_TIMER = 0U,       /*!< Time counter  */
    PCT_WORKMODE_PULSECOUNTER = 1U /*!< Pulse counter */
} pct_workmode_t;

/*!
 * @brief Prescaler & filter Selection
 */
typedef enum
{
    PCT_PRESCALE_2 = 0x00U,                        /*!< Time mode: prescaler 2, Filter mode: invalid          */
    PCT_PRESCALE_4_GLITCHFILTER_2 = 0x01U,         /*!< Time mode: prescaler 4, Filter mode: 2 clocks         */
    PCT_PRESCALE_8_GLITCHFILTER_4 = 0x02U,         /*!< Time mode: prescaler 8, Filter mode: 4 clocks         */
    PCT_PRESCALE_16_GLITCHFILTER_8 = 0x03U,        /*!< Time mode: prescaler 16, Filter mode: 8 clocks        */
    PCT_PRESCALE_32_GLITCHFILTER_16 = 0x04U,       /*!< Time mode: prescaler 32, Filter mode: 16 clocks       */
    PCT_PRESCALE_64_GLITCHFILTER_32 = 0x05U,       /*!< Time mode: prescaler 64, Filter mode: 32 clocks       */
    PCT_PRESCALE_128_GLITCHFILTER_64 = 0x06U,      /*!< Time mode: prescaler 128, Filter mode: 64 clocks      */
    PCT_PRESCALE_256_GLITCHFILTER_128 = 0x07U,     /*!< Time mode: prescaler 256, Filter mode: 128 clocks     */
    PCT_PRESCALE_512_GLITCHFILTER_256 = 0x08U,     /*!< Time mode: prescaler 512, Filter mode: 256 clocks     */
    PCT_PRESCALE_1024_GLITCHFILTER_512 = 0x09U,    /*!< Time mode: prescaler 1024, Filter mode: 512 clocks    */
    PCT_PRESCALE_2048_GLITCHFILTER_1024 = 0x0AU,   /*!< Time mode: prescaler 2048, Filter mode: 1024 clocks   */
    PCT_PRESCALE_4096_GLITCHFILTER_2048 = 0x0BU,   /*!< Time mode: prescaler 4096, Filter mode: 2048 clocks   */
    PCT_PRESCALE_8192_GLITCHFILTER_4096 = 0x0CU,   /*!< Time mode: prescaler 8192, Filter mode: 4096 clocks   */
    PCT_PRESCALE_16384_GLITCHFILTER_8192 = 0x0DU,  /*!< Time mode: prescaler 16384, Filter mode: 8192 clocks  */
    PCT_PRESCALE_32768_GLITCHFILTER_16384 = 0x0EU, /*!< Time mode: prescaler 32768, Filter mode: 16384 clocks */
    PCT_PRESCALE_65536_GLITCHFILTER_32768 = 0x0FU  /*!< Time mode: prescaler 65536, Filter mode: 32768 clocks */
} pct_prescaler_t;

/*!
 * @brief pct cval read presicion sel
 */
typedef enum
{
    PCT_BCLK_FCLK_4 = 0x00U,      /*!< BUSCLK/FUNCTION CLK >= 4        */
    PCT_BCLK_FCLK_3 = 0x01U,      /*!< BUSCLK/FUNCTION CLK >= 3        */
    PCT_BCLK_FCLK_1 = 0x02U,      /*!< BUSCLK/FUNCTION CLK >= 1        */
} pct_sel_t;

/*!
 * @brief Clock source selection
 */
typedef enum
{
    PCT_CLOCKSOURCE_HSIDIV2 = 0x00U,  /*!< HSIDIV2 clock        */
    PCT_CLOCKSOURCE_LSI1K = 0x01U,    /*!< LSI1K clock          */
    PCT_CLOCKSOURCE_RTC = 0x02U,      /*!< RTC clock            */
    PCT_CLOCKSOURCE_PCC = 0x03U       /*!< PCC configured clock */
} pct_clocksource_t;

/*!
 * @brief PCT counter units
 */
typedef enum
{
    PCT_COUNTER_UNITS_TICKS = 0x00U,
    PCT_COUNTER_UNITS_MICROSECONDS = 0x01U
} pct_counter_units_t;

/*!
 * @brief Configuration structure for PCT
 */
typedef struct
{
    bool interruptEnable;             /*!< Enable/Disable Interrupt                 */
    bool freeRun;                     /*!< Enable/Disable Free Running Mode         */
    pct_workmode_t workMode;          /*!< Time/Pulse Counter Mode                  */
    pct_clocksource_t clockSelect;    /*!< Clock source selection                   */
    pct_prescaler_t prescaler;        /*!< Prescaler & filter Selection             */
    pct_sel_t cvalreadsel;            /*!< Read Cval presicison sel                 */
    bool bypassPrescaler;             /*!< Enable/Disable prescaler & fliter bypass */
    uint32_t compareValue;            /*!< Compare value                            */
    pct_counter_units_t counterUnits; /*!< Compare value units                      */
    pct_pinselect_t pinSelect;        /*!< Pin selection for Pulse-Count mode       */
    pct_pinpolarity_t pinPolarity;    /*!< Pin Polarity for Pulse-Count mode        */
    pct_callback_t callback;          /*!< call back function                       */
} pct_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes configuration structure to default values.
 *
 * @param[out] config: the configuration structure to be initialized
 * @return none
 */
void PCT_DRV_InitConfigStruct(pct_config_t *const config);

/*!
 * @brief Initializes PCT.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] config: the configuration structure
 * @param[in] startCounter: starting counting control
 * @return none
 */
void PCT_DRV_Init(const uint8_t instance, const pct_config_t *const config, const bool startCounter);

/*!
 * @brief Configure PCT.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] config: the configuration structure
 * @return none
 */
void PCT_DRV_SetConfig(const uint8_t instance, const pct_config_t *const config);

/*!
 * @brief Gets the configuration of PCT.
 *
 * @param[in] instance: instance number of PCT
 * @param[out] config: the configuration structure to get
 * @return none
 */
void PCT_DRV_GetConfig(const uint8_t instance, pct_config_t *const config);

/*!
 * @brief De-initialize PCT.
 *
 * @param[in] instance: instance number of PCT
 * @return none
 */
void PCT_DRV_Deinit(const uint8_t instance);

/*!
 * @brief Sets the compare value in counter unit.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] compareValueByCount: The compare value in counter ticks
 * @return Operation status
 */
status_t PCT_DRV_SetCompareValueByCount(const uint8_t instance, const uint16_t compareValueByCount);

/*!
 * @brief Gets the compare value in counter tick units.
 *
 * @param[in] instance: instance number of PCT
 * @param[out] compareValueByCount: current compare value in counter ticks
 * @return none
 */
void PCT_DRV_GetCompareValueByCount(const uint8_t instance, uint16_t *const compareValueByCount);

/*!
 * @brief Sets the compare value in microseconds unit.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] compareValueUs: Compare value in microseconds unit
 * @return Operation status
 */
status_t PCT_DRV_SetCompareValueByUs(const uint8_t instance, const uint32_t compareValueUs);

/*!
 * @brief Gets the compare value in microseconds.
 *
 * @param[in] instance: instance number of PCT
 * @param[out] compareValueUs: current compare value in microseconds
 * @return none
 */
void PCT_DRV_GetCompareValueByUs(const uint8_t instance, uint32_t *const compareValueUs);

/*!
 * @brief Gets the compare flag of PCT.
 *
 * @param[in] instance: instance number of PCT
 * @return the compare flag
 */
bool PCT_DRV_GetCompareFlag(const uint8_t instance);

/*!
 * @brief Clears the compare flag of PCT.
 *
 * @param[in] instance: instance number of PCT
 * @return none
 */
void PCT_DRV_ClearCompareFlag(const uint8_t instance);

/*!
 * @brief Gets if PCT is running.
 *
 * @param[in] instance: instance number of PCT
 * @return The running state of PCT
 */
bool PCT_DRV_IsRunning(const uint8_t instance);

/*!
 * @brief Enable/disable PCT interrupt.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] enableInterrupt: The PCT interrupt enable flag
 * @return none
 */
void PCT_DRV_SetInterrupt(const uint8_t instance, const bool enableInterrupt);

/*!
 * @brief Gets the current counter value in counter units.
 *
 * @param[in] instance: instance number of PCT
 * @return The current counter value
 */
uint16_t PCT_DRV_GetCounterValueByCount(const uint8_t instance);

/*!
 * @brief Starts the counter.
 *
 * @param[in] instance: instance number of PCT
 * @return none
 */
void PCT_DRV_StartCounter(const uint8_t instance);

/*!
 * @brief Stops the counter.
 *
 * @param[in] instance: instance number of PCT
 * @return none
 */
void PCT_DRV_StopCounter(const uint8_t instance);

/*!
 * @brief Sets the Input Pin configuration for Pulse Count mode.
 *
 * @param[in] instance: instance number of PCT
 * @param[in] pinSelect: PCT pin selection
 * @param[in] pinPolarity: Polarity of counting pin(at rising/falling edge)
 * @return none
 */
void PCT_DRV_SetPinConfiguration(const uint8_t instance, const pct_pinselect_t pinSelect,
                                 const pct_pinpolarity_t pinPolarity);

/*!
 * @brief Installs PCT callback function.
 *
 * @param[in] func: PCT interrupt callback
 * @return none
 */
void PCT_DRV_InstallCallback(const pct_callback_t func);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PCT_DRV_H */

/* =============================================  EOF  ============================================== */
