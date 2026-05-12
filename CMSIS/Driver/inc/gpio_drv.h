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
 * @file gpio_drv.h
 *
 * @brief This file provides gpio integration functions interface.
 *
 */

#ifndef _GPIO_DRV_H
#define _GPIO_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< Callback for GPIO module. */
typedef void (*gpio_callback_t)(uint32_t instance, uint32_t status, void *userData);

/*!
 * @brief GPIO callback struction
 */
typedef struct
{
    gpio_callback_t callback;              /*!< Callback for GPIO interrupt handler */
    void *callbackParam;                   /*!< GPIO interrupt callback parameter pointer.*/
} gpio_isr_t;

 /*!< Type of a gpio channel. */
typedef uint32_t gpio_channel_type_t;

/*!< Type of a port levels. */
typedef uint8_t gpio_level_type_t;

/*!
 * @brief Type of gpio data direction.
 */
typedef enum
{
    GPIO_INPUT_DIRECTION       = 0x0U,  /*!< General purpose input direction.       */
    GPIO_OUTPUT_DIRECTION      = 0x1U,  /*!< General purpose output direction.      */
    GPIO_UNSPECIFIED_DIRECTION = 0x2U   /*!< General purpose unspecified direction. */
} port_data_direction_t;

/*!
 * @brief Internal pull-up/down resistor.
 */
typedef enum
{
    PORT_INTERNAL_PULL_NOT_ENABLED   = 0U,  /*!< internal pull-down or pull-up resistor is not enabled.           */
    PORT_INTERNAL_PULL_DOWN_ENABLED  = 1U,  /*!< internal pull-down resistor is enabled. */
    PORT_INTERNAL_PULL_UP_ENABLED    = 2U   /*!< internal pull-up resistor is enabled. */
} port_pull_config_t;

/*!
 * @brief Configures the drive strength.
 */
typedef enum
{
    PORT_LOW_DRIVE_STRENGTH     = 0U,    /*!< low drive strength  */
    PORT_HIGH_DRIVE_STRENGTH    = 1U     /*!< high drive strength */
} port_drive_strength_t;

/*!
 * @brief Configures the pin mux.
 */
typedef enum
{
    PORT_PIN_DISABLED            = 0U,  /*!< gpio is disabled or as an analog funciton */
    PORT_MUX_AS_GPIO             = 1U,  /*!< gpio is configured as GPIO                */
    PORT_MUX_ALT2                = 2U,  /*!< chip-specific                             */
    PORT_MUX_ALT3                = 3U,  /*!< chip-specific                             */
    PORT_MUX_ALT4                = 4U,  /*!< chip-specific                             */
    PORT_MUX_ALT5                = 5U,  /*!< chip-specific                             */
    PORT_MUX_ALT6                = 6U,  /*!< chip-specific                             */
    PORT_MUX_ALT7                = 7U   /*!< chip-specific                             */
} port_mux_t;

/*!
 * @brief Configures the interrupt generation condition.
 */
typedef enum
{
    PORT_DMA_INT_DISABLED  = 0x0U,  /*!< Interrupt/DMA request is disabled. */
    PORT_DMA_RISING_EDGE   = 0x1U,  /*!< DMA request on rising edge.        */
    PORT_DMA_FALLING_EDGE  = 0x2U,  /*!< DMA request on falling edge.       */
    PORT_DMA_EITHER_EDGE   = 0x3U,  /*!< DMA request on either edge.        */
    PORT_INT_RISING_EDGE   = 0x9U,  /*!< Interrupt on rising edge.          */
    PORT_INT_FALLING_EDGE  = 0xAU,  /*!< Interrupt on falling edge.         */
    PORT_INT_EITHER_EDGE   = 0xBU   /*!< Interrupt on either edge.          */
} port_interrupt_config_t;

/*!
 * @brief Clock source for the digital input filters.
 */
typedef enum
{
    PORT_DIGITAL_FILTER_BUS_CLOCK  = 0U, /*!< Select bus clock */
    PORT_DIGITAL_FILTER_LPO_CLOCK  = 1U  /*!< Select LPO clock */
} port_digital_filter_clock_t;

/*!
 * @brief The digital filter configuration.
 */
typedef struct
{
    port_digital_filter_clock_t clock;  /*!< The digital filter clock for port */
    uint8_t                     width;  /*!< The digital filter width value */
} port_digital_filter_config_t;

/*!
 * @brief Defines the gpio configuration structure.
 */
typedef struct
{
    PORT_Type *base;                      /*!< Port base pointer.                        */
    uint32_t pinPortIdx;                  /*!< Pin number.                               */
    port_pull_config_t pullConfig;        /*!< Internal pull-up/down resistor selection. */
    port_drive_strength_t driveSelect;    /*!< Configures the drive strength.            */
    port_mux_t mux;                       /*!< Pin mux selection.                        */
    bool pinLock;                         /*!< Lock pin control register or not.         */
    port_interrupt_config_t intConfig;    /*!< Interrupt generation condition.           */
    bool clearIntFlag;                    /*!< Clears the interrupt status flag.         */
    bool digitalFilter;                   /*!< Enables digital filter.                   */
    GPIO_Type *gpioBase;                  /*!< GPIO base pointer.                        */
    port_data_direction_t direction;      /*!< Configures the port data direction.       */
    gpio_level_type_t initValue;          /*!< Initial value                             */
} gpio_settings_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initializes the gpio with the given configuration structure.
 *
 * @param[in] pinCount: The number of configured pins in structure
 * @param[in] config: The configuration structure
 * @return The status of the operation
 */
status_t GPIO_DRV_Init(uint32_t pinCount,
                       const gpio_settings_config_t config[]);

/*!
 * @brief Configures the internal pull-up/down resistor.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @param[in] pullConfig: The pull pull-up/down resistor configuration
 * @return none
 */
void GPIO_DRV_SetPullSel(PORT_Type * const base, uint32_t pin, port_pull_config_t pullConfig);

/*!
 * @brief Configures the pin mux.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @param[in] mux: Pin mux selection
 * @return none
 */
void GPIO_DRV_SetMuxModeSel(PORT_Type * const base, uint32_t pin, port_mux_t mux);

/*!
 * @brief Configures the port pin interrupt/DMA request.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @param[in] intConfig: Interrupt configuration
 * @return none
 */
void GPIO_DRV_SetPinIntSel(PORT_Type * const base, uint32_t pin, port_interrupt_config_t intConfig);

/*!
 * @brief Gets the current port pin interrupt/DMA request configuration.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @return Interrupt configuration
 */
port_interrupt_config_t GPIO_DRV_GetPinIntSel(const PORT_Type * const base, uint32_t pin);

/*!
 * @brief Clears the individual pin interrupt status flag.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @return none
 */
void GPIO_DRV_ClearPinIntFlagCmd(PORT_Type * const base, uint32_t pin);

/*!
 * @brief Enables digital filter for digital pin mux.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @return none
 */
void GPIO_DRV_EnableDigitalFilter(PORT_Type * const base, uint32_t pin);

/*!
 * @brief Disables digital filter for digital pin mux.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin: Port pin number
 * @return none
 */
void GPIO_DRV_DisableDigitalFilter(PORT_Type * const base, uint32_t pin);

/*!
 * @brief Configures digital filter for port with given configuration.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] config: The digital filter configuration struct
 * @return none
 */
void GPIO_DRV_ConfigDigitalFilter(PORT_Type * const base, const port_digital_filter_config_t * const config);

/*!
 * @brief Get the entire port interrupt status flag.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @return All 32 pins interrupt status flags
 */
uint32_t GPIO_DRV_GetPortIntFlag(const PORT_Type * const base);

/*!
 * @brief Clears the entire port interrupt status flag.
 *
 * @param[in] base: Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @return none
 */
void GPIO_DRV_ClearPortIntFlagCmd(PORT_Type * const base);

/*!
 * @brief Get the pins directions configuration for a port.
 *
 * @param[in] base: GPIO base pointer (PORTA, PORTB, PORTC, etc.)
 * @return GPIO directions: Each bit represents one pin (LSB is pin 0, MSB is
 *         pin 31). For each bit:
 *        - 0: corresponding pin is set to input
 *        - 1: corresponding pin is set to output
 */
gpio_channel_type_t GPIO_DRV_GetPinsDirection(const GPIO_Type * const base);

/*!
 * @brief Configure the direction for a certain pin from a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pin: The pin number for which to configure the direction
 * @param[in] direction: The pin direction:
 *        - 0: corresponding pin is set to input
 *        - 1: corresponding pin is set to output
 * @return none
 */
void GPIO_DRV_SetPinDirection(GPIO_Type * const base, gpio_channel_type_t pin, gpio_level_type_t direction);

/*!
 * @brief Set the pins directions configuration for a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB
 *            is pin 0, MSB is pin 31). For each bit:
 *        - 0: corresponding pin is set to input
 *        - 1: corresponding pin is set to output
 * @return none
 */
void GPIO_DRV_SetPinsDirection(GPIO_Type * const base, gpio_channel_type_t pins);

/*!
 * @brief Set the pins input disable state for a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB is pin 0, MSB is
 *            pin 31). For each bit:
 *        - 0: corresponding pin input will be enabled
 *        - 1: corresponding pin input will be disabled
 * @return none
 */
void GPIO_DRV_SetPortInputDisable(GPIO_Type * const base, gpio_channel_type_t pins);

/*!
 * @brief Get the pins input disable state for a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return GPIO input state: Each bit represents one pin (LSB is pin 0, MSB is
 *         pin 31). For each bit:
 *         - 0: corresponding pin input is enabled
 *         - 1: corresponding pin input is disabled
 */
gpio_channel_type_t GPIO_DRV_GetPortInputDisable(const GPIO_Type * const base);

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
void GPIO_DRV_WritePin(GPIO_Type * const base, gpio_channel_type_t pin, gpio_level_type_t value);

/*!
 * @brief Write all pins of a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask to be written
 *            - 0: corresponding pin is set to low
 *            - 1: corresponding pin is set to high
 * @return none
 */
void GPIO_DRV_WritePins(GPIO_Type * const base, gpio_channel_type_t pins);

/*!
 * @brief Get the current output level configuration from a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return GPIO outputs level configuration: Each bit represents one pin
 *         (LSB is pin 0, MSB is pin 31). For each bit:
 *         - 0: corresponding pin is set to low
 *         - 1: corresponding pin is set to high
 */
gpio_channel_type_t GPIO_DRV_GetPinsOutput(const GPIO_Type * const base);

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
void GPIO_DRV_SetPins(GPIO_Type * const base, gpio_channel_type_t pins);

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
void GPIO_DRV_ClearPins(GPIO_Type * const base, gpio_channel_type_t pins);

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
void GPIO_DRV_TogglePins(GPIO_Type * const base, gpio_channel_type_t pins);

/*!
 * @brief Read pins input level.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return GPIO inputs level: Each bit represents one pin (LSB is pin 0, MSB is pin
 *         31). For each bit:
 *         - 0: corresponding pin is read as low
 *         - 1: corresponding pin is read as high
 */
gpio_channel_type_t GPIO_DRV_ReadPins(const GPIO_Type * const base);

/*!
 * @brief Set GPIO as high-Z state.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pin: GPIO pin number 0~31
 * @param[in] enable: enable or disable high-z state
 * @return none
 */
void GPIO_DRV_SetHighZ(GPIO_Type *base, uint32_t pin, bool enable);

/*!
 * @brief Get PORT base poiner.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return port base pointer(PORTA, PORTB, PORTC, etc.)
 */
PORT_Type *GPIO_DRV_GetPort(GPIO_Type *base);

/*!
 * @brief Install GPIO interrupt callback function.
 *
 * @param[in] instance: GPIO instance
 * @param[in] function: The GPIO interrupt callback function to be installed
 * @param[in] callbackParam: The parameter of GPIO interrupt callback function
 * @return none
 */
void GPIO_DRV_InstallCallback(uint32_t instance, gpio_callback_t function, void *callbackParam);

/*!
 * @brief GPIO interrupt handler.
 *
 * @param[in] instance: GPIO instance
 * @return none
 */
void GPIO_DRV_IRQHandler(uint32_t instance);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _GPIO_DRV_H */

/* =============================================  EOF  ============================================== */
