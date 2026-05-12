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
 * AutoChips Inc. (C) 2020. All rights reserved.
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
 * @file port_hw.h
 *
 * @brief This file provides port hardware integration functions interface.
 *
 */

#ifndef _PORT_HW_H
#define _PORT_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "gpio_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes the pins.
 *
 * @param[in] config: the configuration structure
 * @return none
 */
void PORT_Init(const gpio_settings_config_t * config);

/*!
 * @brief Configures the pin muxing.
 *
 * @param[in] base: port base pointer
 * @param[in] pin: port pin number
 * @param[in] mux: pin muxing slot selection
 *            - PORT_PIN_DISABLED: Pin disabled.
 *            - PORT_MUX_AS_GPIO : Set as GPIO.
 *            - others           : chip-specific.
 * @return none
 */
void PORT_SetMuxModeSel(PORT_Type * const base, uint32_t pin, port_mux_t mux);

/*!
 * @brief Get the pin muxing.
 *
 * @param[in] base: port base pointer
 * @param[in] pin: port pin number
 * @return pinmux
 */
port_mux_t PORT_GetMuxModeSel(PORT_Type * const base, uint32_t pin);

/*!
 * @brief Configures the port pin interrupt/DMA request.
 *
 * @param[in] base: port base pointer.
 * @param[in] pin: port pin number
 * @param[in] intConfig:  interrupt configuration
 *            - PORT_INT_DISABLED     : Interrupt/DMA request disabled.
 *            - PORT_DMA_RISING_EDGE  : DMA request on rising edge.
 *            - PORT_DMA_FALLING_EDGE : DMA request on falling edge.
 *            - PORT_DMA_EITHER_EDGE  : DMA request on either edge.
 *            - PORT_INT_RISING_EDGE  : Interrupt on rising edge.
 *            - PORT_INT_FALLING_EDGE : Interrupt on falling edge.
 *            - PORT_INT_EITHER_EDGE  : Interrupt on either edge.
 * @return none
 */
void PORT_SetPinIntSel(PORT_Type * const base, uint32_t pin, port_interrupt_config_t intConfig);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Configures the internal pull-up/down resistor.
 *
 * @param[in] base: port base pointer.
 * @param[in] pin: port pin number
 * @param[in] pullConfig:  internal pull-up/down resistor configuration
 *            - PORT_PULL_NOT_ENABLED: internal pull-down or pull-up resistor is not enabled.
 *            - PORT_PULL_DOWN_ENABLED: internal pull-down resistor is enabled.
 *            - PORT_PULL_UP_ENABLED: internal pull-up resistor is enabled.
 * @return none
 */
static inline void PORT_SetPullSel(PORT_Type * const base, uint32_t pin, port_pull_config_t pullConfig)
{
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    uint32_t regValue = base->PCR[pin];

    switch (pullConfig)
    {
    case PORT_INTERNAL_PULL_NOT_ENABLED:
        regValue &= ~(PORT_PCR_PU_Msk | PORT_PCR_PD_Msk);
        base->PCR[pin] = regValue;
        break;

    case PORT_INTERNAL_PULL_DOWN_ENABLED:
        regValue &= ~PORT_PCR_PU_Msk;
        regValue |= PORT_PCR_PD_Msk;
        base->PCR[pin] = regValue;
        break;

    case PORT_INTERNAL_PULL_UP_ENABLED:
        regValue &= ~PORT_PCR_PD_Msk;
        regValue |= PORT_PCR_PU_Msk;
        base->PCR[pin] = regValue;
        break;

    default:
        /* invalid command */
        DEVICE_ASSERT(false);
        break;
    }
}

/*!
 * @brief Gets the current port pin interrupt/DMA request configuration.
 *
 * @param[in] base: port base pointer
 * @param[in] pin: port pin number
 * @return  interrupt configuration:
 *        - PORT_INT_DISABLED     : Interrupt/DMA request disabled.
 *        - PORT_DMA_RISING_EDGE  : DMA request on rising edge.
 *        - PORT_DMA_FALLING_EDGE : DMA request on falling edge.
 *        - PORT_DMA_EITHER_EDGE  : DMA request on either edge.
 *        - PORT_INT_RISING_EDGE  : Interrupt on rising edge.
 *        - PORT_INT_FALLING_EDGE : Interrupt on falling edge.
 *        - PORT_INT_EITHER_EDGE  : Interrupt on either edge.
 */
static inline port_interrupt_config_t PORT_GetPinIntSel(const PORT_Type * const base, uint32_t pin)
{
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    uint32_t regValue = base->PCR[pin];

    regValue = (regValue & PORT_PCR_IRQC_Msk) >> PORT_PCR_IRQC_Pos;

    return (port_interrupt_config_t)regValue;
}

/*!
 * @brief Clears the individual pin interrupt status flag.
 *
 * @param[in] base: port base pointer
 * @param[in] pin: port pin number
 * @return none
 */
static inline void PORT_ClearPinIntFlagCmd(PORT_Type * const base, uint32_t pin)
{
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    uint32_t regValue = base->PCR[pin];

    regValue &= ~(PORT_PCR_ISF_Msk);
    regValue |= (uint32_t)1U << PORT_PCR_ISF_Pos;
    base->PCR[pin] = regValue;
}

/*!
 * @brief Enables digital filter for digital pin muxing.
 *
 * @param[in] base: port base pointer
 * @param[in] pin: port pin number
 * @return none
 */
static inline void PORT_EnableDigitalFilter(PORT_Type * const base, uint32_t pin)
{
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    base->DFER |= (uint32_t)1U << pin;
}

/*!
 * @brief Disables digital filter for digital pin muxing.
 *
 * @param[in] base: port base pointer
 * @param[in] pin: port pin number
 * @return none
 */
static inline void PORT_DisableDigitalFilter(PORT_Type * const base, uint32_t pin)
{
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    base->DFER &= ~((uint32_t)1U << pin);
}

/*!
 * @brief Set drive strength.
 *
 * @param[in] base: port base pointer
 * @param[in] pin: port pin number
 * @param[in] enable: Low or high drive strength
 * @return none
 */
static inline void PORT_SetDriveStrength(PORT_Type * const base, uint32_t pin, bool enable)
{
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    MODIFY_REG32(base->PCR[pin], PORT_PCR_DSE_Msk, PORT_PCR_DSE_Pos, enable);
}

/*!
 * @brief Configures digital filter clock for port with given configuration.
 *
 * @param[in] base: port base pointer
 * @param[in] config: configuration struct
 * @return none
 */
static inline void PORT_ConfigDigitalFilter(PORT_Type * const base,
                                            const port_digital_filter_config_t * const config)
{
    DEVICE_ASSERT(config->width <= PORT_DFWR_FILT_Msk);

    base->DFCR = (uint32_t)config->clock;
    base->DFWR = (config->width & PORT_DFWR_FILT_Msk);
}

/*!
 * @brief Get the port interrupt status flag.
 *
 * @param[in] base:  port base pointer
 * @return all 32 pin interrupt status flags. For each bit:
 *         - 0: interrupt is not detected.
 *         - 1: interrupt is detected.
 */
static inline uint32_t PORT_GetPortIntFlag(const PORT_Type * const base)
{
    uint32_t regValue = base->ISFR;

    return regValue;
}

/*!
 * @brief Clears the entire port interrupt status flag.
 *
 * @param[in] base:  port base pointer
 * @return none
 */
static inline void PORT_ClearPortIntFlagCmd(PORT_Type * const base)
{
    base->ISFR = 0xFFFFFFFF;
}

/*!
 * @brief Set GPIO lock bit.
 *
 * @param[in] base: PORTA, B, C, D, E
 * @param[in] pin: pin index, value can be 0 to 31
 * @param[in] enable: GPIO configuration lock function. 1:lock    0:not lock
 * @return none
 */
static inline void PORT_SetLock(PORT_Type * const base, uint32_t pin, bool enable)
{
    MODIFY_REG32((base)->PCR[pin], PORT_PCR_LK_Msk,  PORT_PCR_LK_Pos, (enable));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PORT_HW_H */

/* =============================================  EOF  ============================================== */
