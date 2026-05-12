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
 * @file gpio_drv.c
 *
 * @brief This file provides gpio integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "gpio_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of GPIO port base address */
static PORT_Type *const s_gpioPortBase[GPIO_INSTANCE_MAX] = GPIO_PORT_BASE_PTRS;

/* Array storing references to GPIO IRQ handlers */
static gpio_isr_t s_gpioIsr[GPIO_INSTANCE_MAX] = {NULL};

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initializes the gpio with the given configuration structure.
 *
 * @param[in] pinCount: The number of configured pins in structure
 * @param[in] config: The configuration structure
 * @return The status of the operation
 */
status_t GPIO_DRV_Init(uint32_t pinCount, const gpio_settings_config_t config[])
{
    uint32_t i;

    for (i = 0U; i < pinCount; i++)
    {
        PORT_Init(&config[i]);
    }

    NVIC_EnableIRQ(PORTA_IRQn);
    NVIC_EnableIRQ(PORTB_IRQn);
    NVIC_EnableIRQ(PORTC_IRQn);
    NVIC_EnableIRQ(PORTD_IRQn);
    NVIC_EnableIRQ(PORTE_IRQn);

    return STATUS_SUCCESS;
}

/*!
 * @brief Configures the internal pull-up/down resistor.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @param[in] pullConfig: The pull pull-up/down resistor configuration
 * @return none
 */
void GPIO_DRV_SetPullSel(PORT_Type * const base, uint32_t pin, port_pull_config_t pullConfig)
{
    PORT_SetPullSel(base, pin, pullConfig);
}

/*!
 * @brief Configures the pin mux.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @param[in] mux: Pin mux selection
 * @return none
 */
void GPIO_DRV_SetMuxModeSel(PORT_Type * const base, uint32_t pin, port_mux_t mux)
{
    PORT_SetMuxModeSel(base, pin, mux);
}

/*!
 * @brief Configures the port pin interrupt/DMA request.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @param[in] intConfig: Interrupt configuration
 * @return none
 */
void GPIO_DRV_SetPinIntSel(PORT_Type * const base, uint32_t pin, port_interrupt_config_t intConfig)
{
    PORT_SetPinIntSel(base, pin, intConfig);
}

/*!
 * @brief Gets the current port pin interrupt/DMA request configuration.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @return Interrupt configuration
 */
port_interrupt_config_t GPIO_DRV_GetPinIntSel(const PORT_Type * const base, uint32_t pin)
{
    return PORT_GetPinIntSel(base, pin);
}

/*!
 * @brief Clears the individual pin interrupt status flag.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @return none
 */
void GPIO_DRV_ClearPinIntFlagCmd(PORT_Type * const base, uint32_t pin)
{
    PORT_ClearPinIntFlagCmd(base, pin);
}

/*!
 * @brief Enables digital filter for digital pin mux.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @return none
 */
void GPIO_DRV_EnableDigitalFilter(PORT_Type * const base, uint32_t pin)
{
    PORT_EnableDigitalFilter(base, pin);
}

/*!
 * @brief Disables digital filter for digital pin mux.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @return none
 */
void GPIO_DRV_DisableDigitalFilter(PORT_Type * const base, uint32_t pin)
{
    PORT_DisableDigitalFilter(base, pin);
}

/*!
 * @brief Configures digital filter for port with given configuration.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] config: The digital filter configuration struct
 * @return none
 */
void GPIO_DRV_ConfigDigitalFilter(PORT_Type * const base, const port_digital_filter_config_t * const config)
{
    PORT_ConfigDigitalFilter(base, config);
}

/*!
 * @brief Get the entire port interrupt status flag.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @return All 32 pins interrupt status flags
 */
uint32_t GPIO_DRV_GetPortIntFlag(const PORT_Type * const base)
{
    return PORT_GetPortIntFlag(base);
}

/*!
 * @brief Clears the entire port interrupt status flag.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @return none
 */
void GPIO_DRV_ClearPortIntFlagCmd(PORT_Type * const base)
{
    PORT_ClearPortIntFlagCmd(base);
}

/*!
 * @brief Get the pins directions configuration for a port.
 *
 * @param[in] base: GPIO base pointer (PORTA, PORTB, PORTC, etc.)
 * @return GPIO directions: Each bit represents one pin (LSB is pin 0, MSB is
 *         pin 31). For each bit:
 *         - 0: corresponding pin is set to input
 *         - 1: corresponding pin is set to output
 */
gpio_channel_type_t GPIO_DRV_GetPinsDirection(const GPIO_Type * const base)
{
    return GPIO_GetPinsDirection(base);
}

/*!
 * @brief Configure the direction for a certain pin from a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pin: The pin number for which to configure the direction
 * @param[in] direction: The pin direction:
 *            - 0: corresponding pin is set to input
 *            - 1: corresponding pin is set to output
 * @return none
 */
void GPIO_DRV_SetPinDirection(GPIO_Type * const base, gpio_channel_type_t pin, gpio_level_type_t direction)
{
    GPIO_SetPinDirection(base, pin, direction);
}

/*!
 * @brief Set the pins directions configuration for a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB
 *            is pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin is set to input
 *            - 1: corresponding pin is set to output
 * @return none
 */
void GPIO_DRV_SetPinsDirection(GPIO_Type * const base, gpio_channel_type_t pins)
{
    GPIO_SetPinsDirection(base, pins);
}

/*!
 * @brief Set the pins input disable state for a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB is pin 0, MSB is
 *            pin 31). For each bit:
 *            - 0: corresponding pin input will be enabled
 *            - 1: corresponding pin input will be disabled
 * @return none
 */
void GPIO_DRV_SetPortInputDisable(GPIO_Type * const base, gpio_channel_type_t pins)
{
    GPIO_SetPortInputDisable(base, pins);
}

/*!
 * @brief Get the pins input disable state for a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return GPIO input state: Each bit represents one pin (LSB is pin 0, MSB is
 *         pin 31). For each bit:
 *         - 0: corresponding pin input is enabled
 *         - 1: corresponding pin input is disabled
 */
gpio_channel_type_t GPIO_DRV_GetPortInputDisable(const GPIO_Type * const base)
{
    return GPIO_GetPortInputDisable(base);
}

/*!
 * @brief Write a pin of a port with a given value.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pin: Pin number to be written
 * @param[in] value: Pin value to be written
 *            - 0: corresponding pin is set to low
 *            - 1: corresponding pin is set to high
 * @return none
 */
void GPIO_DRV_WritePin(GPIO_Type * const base, gpio_channel_type_t pin, gpio_level_type_t value)
{
    GPIO_WritePin(base, pin, value);
}

/*!
 * @brief Write all pins of a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask to be written
 *            - 0: corresponding pin is set to low
 *            - 1: corresponding pin is set to high
 * @return none
 */
void GPIO_DRV_WritePins(GPIO_Type * const base, gpio_channel_type_t pins)
{
    GPIO_WritePins(base, pins);
}

/*!
 * @brief Get the current output level configuration from a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return GPIO outputs level configuration: Each bit represents one pin
 *         (LSB is pin 0, MSB is pin 31). For each bit:
 *         - 0: corresponding pin is set to low
 *         - 1: corresponding pin is set to high
 */
gpio_channel_type_t GPIO_DRV_GetPinsOutput(const GPIO_Type * const base)
{
    return GPIO_GetPinsOutput(base);
}

/*!
 * @brief Write "1" to set pins value.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask of bits to be set. Each bit represents one pin (LSB is
 *            pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin level is unaffected
 *            - 1: corresponding pin level is set to high
 * @return none
 */
void GPIO_DRV_SetPins(GPIO_Type * const base, gpio_channel_type_t pins)
{
    GPIO_SetPins(base, pins);
}

/*!
 * @brief Write "0" to reset pins value.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask of bits to be cleared. Each bit represents one pin (LSB
 *            is pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin is unaffected
 *            - 1: corresponding pin is set to low
 * @return none
 */
void GPIO_DRV_ClearPins(GPIO_Type * const base, gpio_channel_type_t pins)
{
    GPIO_ClearPins(base, pins);
}

/*!
 * @brief Toggle pins output level.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask of bits to be toggled.  Each bit represents one pin (LSB
 *            is pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin is unaffected
 *            - 1: corresponding pin is toggled
 * @return none
 */
void GPIO_DRV_TogglePins(GPIO_Type * const base, gpio_channel_type_t pins)
{
    GPIO_TogglePins(base, pins);
}

/*!
 * @brief Read pins input level.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return GPIO inputs level: Each bit represents one pin (LSB is pin 0, MSB is pin
 *         31). For each bit:
 *         - 0: corresponding pin is read as low
 *         - 1: corresponding pin is read as high
 */
gpio_channel_type_t GPIO_DRV_ReadPins(const GPIO_Type * const base)
{
    return GPIO_ReadPins(base);
}

/*!
 * @brief Set GPIO as high-Z state or float input state.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pin: GPIO pin number 0~31
 * @param[in] enable: enable or disable high-z state. If disable high-z, IO will be set to floating input.
 * @return none
 */
void GPIO_DRV_SetHighZ(GPIO_Type *base, uint32_t pin, bool enable)
{
    PORT_Type *PORTx = PORTA;
    uint32_t regVal = 0U;

    /* Set pinmux to GPIO */
    PORTx = GPIO_DRV_GetPort(base);
    PORT_SetMuxModeSel(PORTx, pin, PORT_MUX_AS_GPIO);

    /* Disable pull-up and pull-down */
    PORT_SetPullSel(PORTx, pin, PORT_INTERNAL_PULL_NOT_ENABLED);

    /* Disable output */
    GPIO_SetPinDirection(base, pin, 0u);

    /* Disable or enable input */
    regVal = GPIO_GetPortInputDisable(base);
    if (enable)
    {
        regVal |= (1U << pin);
    }
    else
    {
        regVal &= ~(1U << pin);
    }
    GPIO_SetPortInputDisable(base, regVal);
}

/*!
 * @brief Get PORT base poiner.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return port base pointer(PORTA, PORTB, PORTC, etc.)
 */
PORT_Type *GPIO_DRV_GetPort(GPIO_Type *base)
{
    PORT_Type *PORTx = PORTA;

    switch ((uint32_t)base)
    {
    case GPIOA_BASE:
        PORTx = PORTA;
        break;

    case GPIOB_BASE:
        PORTx = PORTB;
        break;

    case GPIOC_BASE:
        PORTx = PORTC;
        break;

    case GPIOD_BASE:
        PORTx = PORTD;
        break;

    case GPIOE_BASE:
        PORTx = PORTE;
        break;

    default:
        break;
    }

    return PORTx;
}

/*!
 * @brief Install GPIO interrupt callback function.
 *
 * @param[in] instance: GPIO instance
 * @param[in] function: The GPIO interrupt callback function to be installed
 * @param[in] callbackParam: The parameter of GPIO interrupt callback function
 * @return none
 */
void GPIO_DRV_InstallCallback(uint32_t instance, gpio_callback_t function, void *callbackParam)
{
    DEVICE_ASSERT(instance < GPIO_INSTANCE_MAX);

    gpio_isr_t *gpioIsr = &s_gpioIsr[instance];

    gpioIsr->callback = function;
    gpioIsr->callbackParam = callbackParam;
}

/*!
 * @brief GPIO interrupt handler.
 *
 * @param[in] instance: GPIO instance
 */
void GPIO_DRV_IRQHandler(uint32_t instance)
{
    DEVICE_ASSERT(instance < GPIO_INSTANCE_MAX);

    PORT_Type *PORTx = s_gpioPortBase[instance];
    gpio_isr_t *gpioIsr = &s_gpioIsr[instance];
    uint32_t interruptStatus = PORT_GetPortIntFlag(PORTx);

    /* Clear interrupt status */
    PORT_ClearPortIntFlagCmd(PORTx);

    /* Call user interrupt callback function */
    if (gpioIsr->callback != NULL)
    {
        gpioIsr->callback(instance, interruptStatus, gpioIsr->callbackParam);
    }
}

/* =============================================  EOF  ============================================== */
