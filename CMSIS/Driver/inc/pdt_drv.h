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
 * @file pdt_drv.h
 *
 * @brief This file provides PDT integration functions interface.
 *
 */

#ifndef _PDT_DRV_H
#define _PDT_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
/*!< Get PDT Index form base address */
#define PDT_INDEX(base)    ((uint8_t)(((uint32_t)(base) - PDT0_BASE) >> 11))

/* ===========================================  Typedef  ============================================ */
/*!< Callback for PDT module */
typedef void (*pdt_callback_t)(uint32_t instance, uint32_t status, void *userData);

/*!
 * @brief PDT multiplication factor enumeration.
 */
typedef enum
{
    PDT_CLK_PREMULT_FACT_AS_1 = 0U,    /*!< Multiplication factor is 1 */
    PDT_CLK_PREMULT_FACT_AS_10,        /*!< Multiplication factor is 10 */
    PDT_CLK_PREMULT_FACT_AS_20,        /*!< Multiplication factor is 20 */
    PDT_CLK_PREMULT_FACT_AS_40         /*!< Multiplication factor is 40 */
} pdt_clk_prescaler_mult_factor_t;

/*!
 * @brief PDT prescaler divided enumeration.
 */
typedef enum
{
    PDT_CLK_PREDIV_BY_1 = 0U,   /*!< Clk divided by MULT */
    PDT_CLK_PREDIV_BY_2,        /*!< Clk divided by 2 * MULT */
    PDT_CLK_PREDIV_BY_4,        /*!< Clk divided by 4 * MULT */
    PDT_CLK_PREDIV_BY_8,        /*!< Clk divided by 8 * MULT */
    PDT_CLK_PREDIV_BY_16,       /*!< Clk divided by 16 * MULT */
    PDT_CLK_PREDIV_BY_32,       /*!< Clk divided by 32 * MULT */
    PDT_CLK_PREDIV_BY_64,       /*!< Clk divided by 64 * MULT */
    PDT_CLK_PREDIV_BY_128       /*!< Clk divided by 128 * MULT */
} pdt_clock_prescaler_div_t;

/*!
 * @brief PDT load mode select.
 */
typedef enum
{
    PDT_LOAD_VAL_IMMEDIATELY = 0U,                       /*!< Load immediately after 1 is written to LDOK */
    PDT_LOAD_VAL_AT_MODULO_COUNTER,                      /*!< Load when CNT = MODE + 1, after 1 is written to LDOK */
    PDT_LOAD_VAL_AT_NEXT_TRIGGER,                        /*!< Load when a trigger input event is detected, after 1 is written to LDOK */
    PDT_LOAD_VAL_AT_MODULO_COUNTER_OR_NEXT_TRIGGER       /*!< Load when either the CNT = MODE + 1, or a trigger input event is detected, after 1 is written to LDOK */
} pdt_load_value_mode_t;

/*!
 * @brief PDT trigger input source select.
 */
typedef enum
{
    PDT_HARDWARE_TRIGGER = 0U,               /*!< Select hardware trigger */
    PDT_SOFTWARE_TRIGGER = 1U                /*!< Select software trigger */
} pdt_trigger_src_t;

/*!
 * @brief PDT pulse-out config structure.
 */
typedef struct
{
    uint16_t poly1;                   /*!< Delay value for goes high */
    uint16_t poly2;                   /*!< Delay value for goes low */
    bool pulseOutEnable;              /*!< PDT pulse-out enable */
} pdt_pulse_out_config_type_t;

/*!
 * @brief PDT dly channel enumeration.
 */
typedef enum
{
    PDT_DLY_0 = 0U,   /*!< PDT dly0 */
    PDT_DLY_1,        /*!< PDT dly1 */
    PDT_DLY_2,        /*!< PDT dly2 */
    PDT_DLY_3,        /*!< PDT dly3 */
    PDT_DLY_4,        /*!< PDT dly4 */
    PDT_DLY_5,        /*!< PDT dly5 */
    PDT_DLY_6,        /*!< PDT dly6 */
    PDT_DLY_7         /*!< PDT dly7 */
} pdt_dly_channel_type_t;

/*!
 * @brief PDT timer configuration structure.
 */
typedef struct
{
    pdt_load_value_mode_t loadValueMode;                    /*!< Load mode select */
    pdt_clock_prescaler_div_t clkPreDiv;                    /*!< Clock prescaler divided */
    pdt_clk_prescaler_mult_factor_t clkPreMultFactor;       /*!< Multiplication factor select for prescaler */
    pdt_trigger_src_t triggerInput;                         /*!< Trigger input source select */
    bool continuousModeEnable;                              /*!< Continuous mode enable */
    bool intEnable;                                         /*!< Enable the interrupt for timer */
    pdt_callback_t callback;                                /*!< PDT callback pointer */
} pdt_timer_config_t;

/*!
 * @brief PDT Trigger delay config structure.
 */
typedef struct
{
    bool triggerDelayBypassEn;                  /*!< Bypass enable */
    uint8_t delayEnable;                        /*!< Delay enable */
    uint16_t dly[PDT_DLY_MAX];                  /*!< PDT delay value */
} pdt_trigger_delay_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief PDT module reset.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_ModuleReset(const uint8_t instance);

/*!
 * @brief PDT module reset.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] userConfig: Pointer to the user configuration structure
 * @return none
 */
void PDT_DRV_Init(const uint8_t instance, const pdt_timer_config_t * const userConfig);

/*!
 * @brief Get PDT default config.
 *
 * @param[in] config: pointer to PDT default config
 * @return none
 */
void PDT_DRV_GetDefaultConfig(pdt_timer_config_t * const config);

/*!
 * @brief PDT enable.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_Enable(const uint8_t instance);

/*!
 * @brief PDT disable.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_Disable(const uint8_t instance);

/*!
 * @brief Set PDT software trigger cmd.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_SoftTriggerCmd(const uint8_t instance);

/*!
 * @brief Set PDT load values cmd.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_LoadValuesCmd(const uint8_t instance);

/*!
 * @brief PDT ckgen/irq disable & module reset.
 *
 * @param[in] instance: The PDT instance number.
 * @return none
 */
void PDT_DRV_Deinit(const uint8_t instance);

/*!
 * @brief Get PDT timer value.
 *
 * @param[in] instance: The PDT instance number
 * @return timer counting value
 */
uint32_t PDT_DRV_GetTimerValue(const uint8_t instance);

/*!
 * @brief Get PDT timer interrupt flag.
 *
 * @param[in] instance: The PDT instance number
 * @return timer interrupt flag
 */
bool PDT_DRV_GetTimerIntFlag(const uint8_t instance);

/*!
 * @brief Clear PDT timer interrupt flag.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_ClearTimerIntFlag(const uint8_t instance);

/*!
 * @brief Set PDT timer modulus value.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] value: PDT modulus value
 * @return none
 */
void PDT_DRV_SetTimerModulusValue(const uint8_t instance, const uint16_t value);

/*!
 * @brief Set PDT timer interrupt value.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] value: PDT interrupt value
 * @return none
 */
void PDT_DRV_SetValueForTimerInterrupt(const uint8_t instance, const uint16_t value);

/*!
 * @brief Set PDT pulse out enable.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] enable
 *            - ENABLE: pulse output enable
 *            - DISABLE: pulse output disable
 * @return none
 */
void PDT_DRV_SetCmpPulseOutEnable(const uint8_t instance, bool enable);

/*!
 * @brief set PDT pulse out delay value for high
 *
 * @param[in] instance: The PDT instance number
 * @param[in] value: pulse out delay value for high
 * @return none
 */
void PDT_DRV_SetCmpPulseOutDelayForHigh(const uint8_t instance, uint16_t value);

/*!
 * @brief Set PDT pulse out delay value for low.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] value: pulse out delay value for low
 * @return none
 */
void PDT_DRV_SetCmpPulseOutDelayForLow(const uint8_t instance, uint16_t value);

/*!
 * @brief Set PDT trigger Delay Value.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] chn: delay channel(0~7)
 * @param[in] value: trigger delay value
 * @return none
 */
void PDT_DRV_SetTriggerDelayValue(const uint8_t instance, const uint8_t chn, const uint16_t value);

/*!
 * @brief Config PDT delay value and output enable.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] delayConfig: Pointer to the PDT delay configuration structure
 * @return none
 */
void PDT_DRV_ConfigTriggerDelay(const uint8_t instance, const pdt_trigger_delay_config_t *delayConfig);

/*!
 * @brief Config PDT pulse out width and output enable.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] config: pointer to pulse out configuration structure
 * @return none
 */
void PDT_DRV_ConfigPulseOut(const uint8_t instance, const pdt_pulse_out_config_type_t *config);

/*!
 * @brief Set PDT callback function.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] func: callback function
 * @return none
 */
void PDT_DRV_InstallCallback(uint8_t instance, const pdt_callback_t func);

/*!
* @brief PDT common interrupt service routine.
*
* @param[in] instance: The PDT instance number
* @return none
*/
void PDT_DRV_IRQHandler(uint8_t instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PDT_DRV_H */

/* =============================================  EOF  ============================================== */
