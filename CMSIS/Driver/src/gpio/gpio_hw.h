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
 * @file gpio_hw.h
 *
 * @brief This file provides gpio hardware integration interface.
 *
 */

#ifndef _GPIO_HW_H
#define _GPIO_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "port_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get the pins directions configuration for a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return GPIO directions. Each bit represents one pin (LSB is pin 0, MSB is
 *         pin 31). For each bit:
 *         - 0: corresponding pin is set to input
 *         - 1: corresponding pin is set to output
 */
static inline gpio_channel_type_t GPIO_GetPinsDirection(const GPIO_Type * const base)
{
    return (gpio_channel_type_t)base->POER;
}

/*!
 * @brief Configure the IO direction for one pin.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pin: the pin number for which to configure the direction
 * @param[in] direction: the pin direction:
 *            - 0: corresponding pin is set to input
 *            - 1: corresponding pin is set to output
 * @return none
 */
static inline void GPIO_SetPinDirection(GPIO_Type * const base,
                                        gpio_channel_type_t pin,
                                        gpio_level_type_t direction)
{
    gpio_channel_type_t pinsDirections = (gpio_channel_type_t)base->POER;

    pinsDirections &= (gpio_channel_type_t)(~((gpio_channel_type_t)1U << pin));
    pinsDirections |= (gpio_channel_type_t)((gpio_channel_type_t)direction << pin);
    base->POER = pinsDirections;
}

/*!
 * @brief Configure the IO direction for a port.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: pin mask where each bit represents one pin (LSB
 *                 is pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin output disable, if input is enabled,
 *                 the pin is set to input. Otherwise, the pin will be high-Z
 *            - 1: corresponding pin output enable
 * @return none
 */
static inline void GPIO_SetPinsDirection(GPIO_Type * const base, gpio_channel_type_t pins)
{
    base->POER = pins;
}

/*!
 * @brief Configure the port input status.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB
 *                 is pin 0, MSB is pin 31). For each bit:
 *            - 1: corresponding pin input disable
 *            - 0: corresponding pin input enable
 * @return none
 */
static inline void GPIO_SetPortInputDisable(GPIO_Type * const base, gpio_channel_type_t pins)
{
    pins ^= 0xFFFFFFFF;
    base->PIER = pins;
}

/*!
 * @brief Get the port input status.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return Port input state: Each bit represents one pin (LSB
 *         is pin 0, MSB is pin 31). For each bit:
 *         - 1: corresponding pin input disable
 *         - 0: corresponding pin input enable
 */
static inline gpio_channel_type_t GPIO_GetPortInputDisable(const GPIO_Type * const base)
{
    return (gpio_channel_type_t)(base->PIER ^ 0xFFFFFFFF);
}

/*!
 * @brief Write the IO output level for one pin.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pin: The pin number to be written
 * @param[in] value: Pin value to be written
 *            - 0: corresponding pin is set to low
 *            - 1: corresponding pin is set to high
 * @return none
 */
static inline void GPIO_WritePin(GPIO_Type * const base, gpio_channel_type_t pin, gpio_level_type_t value)
{
    gpio_channel_type_t pinsValues = (gpio_channel_type_t)base->PODR;

    pinsValues &= (gpio_channel_type_t)(~((gpio_channel_type_t)1U << pin));
    pinsValues |= (gpio_channel_type_t)((gpio_channel_type_t)value << pin);
    base->PODR = pinsValues;
}

/*!
 * @brief Write the port output level.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB
 *                  is pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin is set to low
 *            - 1: corresponding pin is set to high
 * @return none
 */
static inline void GPIO_WritePins(GPIO_Type * const base, gpio_channel_type_t pins)
{
    base->PODR = pins;
}

/*!
 * @brief Get the port output level configuration.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return Port output level configuration: Pin mask where each bit represents
 *         one pin (LSB is pin 0, MSB is pin 31). For each bit:
 *         - 0: corresponding pin is set to low
 *         - 1: corresponding pin is set to high
 */
static inline gpio_channel_type_t GPIO_GetPinsOutput(const GPIO_Type * const base)
{
    gpio_channel_type_t returnValue = 0U;

    returnValue = (gpio_channel_type_t)(base->PODR);

    return returnValue;
}

/*!
 * @brief Write "1" to set the port output level.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB
 *                  is pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin output level no change
 *            - 1: corresponding pin is set to high
 * @return none
 */
static inline void GPIO_SetPins(GPIO_Type * const base, gpio_channel_type_t pins)
{
    base->PSOR = pins;
}

/*!
 * @brief Write "1" to reset the port output level.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB
 *                  is pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin output level no change
 *            - 1: corresponding pin is set to low
 * @return none
 */
static inline void GPIO_ClearPins(GPIO_Type * const base, gpio_channel_type_t pins)
{
    base->PROR = pins;
}

/*!
 * @brief Write "1" to toggle the port output level.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @param[in] pins: Pin mask where each bit represents one pin (LSB
 *                  is pin 0, MSB is pin 31). For each bit:
 *            - 0: corresponding pin output level no change
 *            - 1: corresponding pin is set to toggled
 * @return none
 */
static inline void GPIO_TogglePins(GPIO_Type * const base, gpio_channel_type_t pins)
{
    base->PIOR = pins;
}

/*!
 * @brief Get the port input level.
 *
 * @param[in] base: GPIO base pointer (GPIOA, GPIOB, GPIOC, etc.)
 * @return Port input level: Pin mask where each bit represents
 *         one pin (LSB is pin 0, MSB is pin 31). For each bit:
 *         - 0: corresponding pin is low
 *         - 1: corresponding pin is high
 */
static inline gpio_channel_type_t GPIO_ReadPins(const GPIO_Type * const base)
{
    gpio_channel_type_t returnValue = 0U;

    returnValue = (gpio_channel_type_t)(base->PIDR);

    return returnValue;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPIO_HW_H*/

/* =============================================  EOF  ============================================== */
