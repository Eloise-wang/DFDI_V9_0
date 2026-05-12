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
 * @file port_hw.c
 *
 * @brief This file provides port hardware integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "gpio_hw.h"

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Initializes the pins.
 *
 * @param[in] config: the configuration structure
 * @return none
 */
void PORT_Init(const gpio_settings_config_t *config)
{
    DEVICE_ASSERT(config->base != NULL);
    DEVICE_ASSERT((PORT_MUX_AS_GPIO != config->mux) || (config->gpioBase != NULL));
    DEVICE_ASSERT(config->pinPortIdx < PORT_PCR_COUNT);

    uint32_t regValue = config->base->PCR[config->pinPortIdx];
    uint32_t directions;
    uint32_t digitalFilters;

    /* Configure PCR register */
    switch (config->pullConfig)
    {
    case PORT_INTERNAL_PULL_NOT_ENABLED:
        regValue &= ~(PORT_PCR_PU_Msk | PORT_PCR_PD_Msk);
        break;

    case PORT_INTERNAL_PULL_DOWN_ENABLED:
        regValue &= ~(PORT_PCR_PU_Msk);
        regValue |= PORT_PCR_PD_Msk;
        break;

    case PORT_INTERNAL_PULL_UP_ENABLED:
        regValue &= ~(PORT_PCR_PD_Msk);
        regValue |= PORT_PCR_PU_Msk;
        break;

    default:
        /* invalid command */
        DEVICE_ASSERT(false);
        break;
    }

    regValue &= ~(PORT_PCR_DSE_Msk);
    regValue |= (uint32_t)(config->driveSelect) << PORT_PCR_DSE_Pos;

    regValue &= ~(PORT_PCR_MUX_Msk);
    regValue |= (uint32_t)(config->mux) << PORT_PCR_MUX_Pos;

    regValue &= ~(PORT_PCR_LK_Msk);
    regValue |= config->pinLock << PORT_PCR_LK_Pos;

    regValue &= ~(PORT_PCR_IRQC_Msk);
    regValue |= (uint32_t)(config->intConfig) << PORT_PCR_IRQC_Pos;
    if (config->clearIntFlag)
    {
        regValue &= ~(PORT_PCR_ISF_Msk);
        regValue |= (1UL << PORT_PCR_ISF_Pos);
    }

    config->base->PCR[config->pinPortIdx] = regValue;

    /* Configure digital filter */
    digitalFilters = (uint32_t)(config->base->DFER);
    digitalFilters &= ~(1UL << (config->pinPortIdx));
    digitalFilters |= (((uint32_t)(config->digitalFilter)) << (config->pinPortIdx));

    config->base->DFER = digitalFilters;

    /* Set the IO direction */
    if (PORT_MUX_AS_GPIO == config->mux)
    {
        /* Read direction */
        directions = (uint32_t)(config->gpioBase->POER);
        switch (config->direction)
        {
        case GPIO_INPUT_DIRECTION:
            directions &= ~(1UL << config->pinPortIdx);
            break;

        case GPIO_OUTPUT_DIRECTION:
            directions |= (1UL << config->pinPortIdx);
            break;

        case GPIO_UNSPECIFIED_DIRECTION:
            /* nothing to configure */
            DEVICE_ASSERT(false);
            break;

        default:
            /* nothing to configure */
            DEVICE_ASSERT(false);
            break;
        }

        /* Configure initial value if direction is set to output */
        if (config->direction == GPIO_OUTPUT_DIRECTION)
        {
            GPIO_WritePin(config->gpioBase, config->pinPortIdx, config->initValue);
        }

        /* Configure direction */
        config->gpioBase->POER = directions;
    }
}

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
void PORT_SetMuxModeSel(PORT_Type * const base, uint32_t pin, port_mux_t mux)
{
    DEVICE_ASSERT(base != NULL);
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    uint32_t regValue = base->PCR[pin];
    port_mux_t muxing = mux;

    regValue &= ~(PORT_PCR_MUX_Msk);
    regValue |= (uint32_t)muxing << PORT_PCR_MUX_Pos;
    base->PCR[pin] = regValue;
}

/*!
 * @brief Get the pin muxing.
 *
 * @param[in] base: port base pointer
 * @param[in] pin: port pin number
 * @return pinmux
 */
port_mux_t PORT_GetMuxModeSel(PORT_Type * const base, uint32_t pin)
{
    DEVICE_ASSERT(base != NULL);
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    return ((port_mux_t)((base->PCR[pin] & PORT_PCR_MUX_Msk) >> PORT_PCR_MUX_Pos));
}

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
void PORT_SetPinIntSel(PORT_Type * const base, uint32_t pin, port_interrupt_config_t intConfig)
{
    DEVICE_ASSERT(base != NULL);
    DEVICE_ASSERT(pin < PORT_PCR_COUNT);

    uint32_t regValue = base->PCR[pin];

    regValue &= ~(PORT_PCR_IRQC_Msk);
    regValue |= (uint32_t)(intConfig) << PORT_PCR_IRQC_Pos;
    base->PCR[pin] = regValue;

    if ((intConfig == PORT_INT_RISING_EDGE) \
        || (intConfig == PORT_INT_FALLING_EDGE) \
        || (intConfig == PORT_INT_EITHER_EDGE))
    {
        switch ((uint32_t)base)
        {
        case PORTA_BASE:
            NVIC_EnableIRQ(PORTA_IRQn);
            break;

        case PORTB_BASE:
            NVIC_EnableIRQ(PORTB_IRQn);
            break;

        case PORTC_BASE:
            NVIC_EnableIRQ(PORTC_IRQn);
            break;

        case PORTD_BASE:
            NVIC_EnableIRQ(PORTD_IRQn);
            break;

        case PORTE_BASE:
            NVIC_EnableIRQ(PORTE_IRQn);
            break;

        default:
            break;
        }
    }
    else
    {
        /* Do nothing */
    }
}

/* =============================================  EOF  ============================================== */
