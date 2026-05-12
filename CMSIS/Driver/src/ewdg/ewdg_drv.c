/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc.	and/or its licensors, any
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
 * @file ewdg_drv.c
 *
 * @brief This file provides ewdg integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "ewdg_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of base addresses for EWDG instances. */
static EWDG_Type * const s_ewdgBase[EWDG_INSTANCE_MAX] = EWDG_BASE_PTRS;

/* Table to save EWDG IRQ numbers. */
static const IRQn_Type s_ewdgIrqId[EWDG_INSTANCE_MAX] = EWDG_IRQS;

/* Table to save EWDG clock names as defined in clock manager. */
static const ckgen_clock_t s_ewdgClkNames[EWDG_INSTANCE_MAX] = EWDG_CKGEN_CLOCKS;

/* Table to save EWDG reset names as defined in clock manager. */
static const ckgen_softreset_t s_ewdgResetNames[EWDG_INSTANCE_MAX] = EWDG_SOFT_RESETS;

/* EWDG callback pointer */
static ewdg_callback_t s_ewdgCallback[EWDG_INSTANCE_MAX] = {(ewdg_callback_t)NULL};

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Init EWDG and Start
 *
 * @param[in] instance: EWDG instance number
 * @param[in] config: Pointer to the module configuration structure
 * @return the status of init
 *         - STATUS_SUCCESS: Init EWDG successfully
 *         - STATUS_ERROR:   Init EWDG fail
 */
status_t EWDG_DRV_Init(uint32_t instance, const ewdg_init_config_t *config)
{
    DEVICE_ASSERT(instance < EWDG_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    uint8_t tempValue = 0U;
    /* Return status variable */
    status_t statusCode = STATUS_SUCCESS;
    /* Flag to the module enabled bit */
    bool isModuleEnabled;
    /* Base pointer */
    EWDG_Type * const base = s_ewdgBase[instance];

    /* Enable clock and reset I2C module */
    CKGEN_DRV_Enable(s_ewdgClkNames[instance], true);
    CKGEN_DRV_SoftReset(s_ewdgResetNames[instance], true);

    /* Get the enable status of the module */
    isModuleEnabled = EWDG_IsEnabled(base);
    /* Check if the EWDG instance is already enabled or if the windows values are not correct */
    if ((isModuleEnabled) || (config->compareHigh <= config->compareLow) \
            || (config->compareHigh > EWDG_CMPH_MAX_VALUE))
    {
        /* If conditions are met change the status code to error */
        statusCode = STATUS_ERROR;
    }
    else
    {
        /* Set clock prescaler */
        EWDG_SetPrescaler(base, config->prescaler);
        /* Set compare high and low values */
        EWDG_SetCompareHigh(base, config->compareHigh);
        EWDG_SetCompareLow(base, config->compareLow);

        /* Configure the Control register and enable the instance.
         * Set the values that are not affected by the input pin
         */
        tempValue = (uint8_t)(EWDG_CTRL_EWDGEN_Msk | ((config->interruptEnable) ? EWDG_CTRL_INTEN_Msk : 0U));

        /* Depending how the input pin is configured set the values into the
         * temporary variable
         */
        switch (config->assertLogic)
        {
        case EWDG_IN_ASSERT_ON_LOGIC_ZERO:
            tempValue |= EWDG_CTRL_INEN_Msk;    /* Input pin enabled, Input asserted on logic 0 */
            break;

        case EWDG_IN_ASSERT_ON_LOGIC_ONE:
            tempValue |= (uint8_t)(EWDG_CTRL_INEN_Msk | EWDG_CTRL_ASSIN_Msk);    /* Input pin enabled, Input asserted on logic 1 */
            break;

        case EWDG_IN_ASSERT_DISABLED:

        default:
            /* Input pin disabled */
            break;
        }

        /* register callback function */
        s_ewdgCallback[instance] = config->callback;

        if (config->interruptEnable)
        {
            NVIC_EnableIRQ(s_ewdgIrqId[instance]);
        }

        /* Write the configuration into the Control register */
        EWDG_SetControl(base, tempValue);
    }

    /* Return the status code */
    return statusCode;
}

/*!
 * @brief De-initialize EWDG driver
 *
 * @param[in] instance: The EWDG instance number
 * @return The result of execution
 *         - STATUS_SUCCESS: operation was successful
 */
status_t EWDG_DRV_Deinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < EWDG_INSTANCE_MAX);

    DisableInterrupts;
    /* Disable timeout interrupt of EWDG */
    NVIC_DisableIRQ(s_ewdgIrqId[instance]);
    NVIC_ClearPendingIRQ(s_ewdgIrqId[instance]);

    /* Deinitialize EWDG */
    CKGEN_DRV_SoftReset(s_ewdgResetNames[instance], false);
    CKGEN_DRV_Enable(s_ewdgClkNames[instance], false);

    EnableInterrupts;

    return STATUS_SUCCESS;
}

/*!
 * @brief Get EWDG default config values.
 *
 * @param[out] config: Pointer to the default configuration structure
 * @return none
 */
void EWDG_DRV_GetDefaultConfig(ewdg_init_config_t *config)
{
    DEVICE_ASSERT(config != NULL);

    /* Disable interrupts */
    config->interruptEnable = false;
    /* Input pin enabled and configured to assert on logic 0 */
    config->assertLogic     = EWDG_IN_ASSERT_ON_LOGIC_ZERO;
    /* Maximum prescaler */
    config->prescaler       = 255U;
    /* Maximum service window */
    config->compareHigh     = EWDG_CMPH_MAX_VALUE;
    config->compareLow      = EWDG_CMPL_MIN_VALUE;
}

/*!
 * @brief Refresh EWDG.
 *
 * @param[in] instance: EWDG instance number
 * @return none
 */
void EWDG_DRV_Refresh(uint32_t instance)
{
    DEVICE_ASSERT(instance < EWDG_INSTANCE_MAX);

    /* Base pointer */
    EWDG_Type * const base = s_ewdgBase[instance];

    /* Disable global IRQ to avoid disturbing the refresh process */
    DisableInterrupts;
    /* Refresh EWDG */
    EWDG_Refresh(base);
    /* Enable global IRQ */
    EnableInterrupts;
}

/*!
 * @brief Get the Input pin assert logic.
 *
 * @param[in] instance: EWDG instance number
 * @return The input pin assert logic
 *         - EWDG_IN_ASSERT_DISABLED: EWDG input disabled
 *         - EWDG_INASSERT_ON_LOGIC_ZERO: EWDG is asserted when EWDG_in is logic 0
 *         - EWDG_INASSERT_ON_LOGIC_ONE: EWDG is asserted when EWDG_in is logic 1
 */
ewdg_in_assert_logic_t EWDG_DRV_GetInputPinAssertLogic(uint32_t instance)
{
    DEVICE_ASSERT(instance < EWDG_INSTANCE_MAX);

    /* Base pointer */
    const EWDG_Type * const base = s_ewdgBase[instance];
    /* Variable to save the input pin assert logic */
    ewdg_in_assert_logic_t returnValue;

    if ((EWDG_GetControl(base) & EWDG_CTRL_INEN_Msk) != 0U)
    {
        returnValue = ((EWDG_GetControl(base) & EWDG_CTRL_ASSIN_Msk) != 0U) ? \
                      EWDG_IN_ASSERT_ON_LOGIC_ONE : EWDG_IN_ASSERT_ON_LOGIC_ZERO;
    }
    else
    {
        returnValue = EWDG_IN_ASSERT_DISABLED;
    }

    return returnValue;
}

/*!
 * @brief EWDG IRQ Handler.
 *
 * @param[in] instance: EWDG instance number
 * @return none
 */
void EWDG_DRV_IRQHandler(uint32_t instance)
{
    DEVICE_ASSERT(instance < EWDG_INSTANCE_MAX);

    /* Base pointer */
    EWDG_Type * const base = s_ewdgBase[instance];

    /* Disable EWDG interrupt to avoid IRQ request */
    EWDG_SetControl(base, (EWDG_GetControl(base) & (~EWDG_CTRL_INTEN_Msk)));

    /* check if callback has been register */
    if (s_ewdgCallback[instance] != NULL)
    {
        s_ewdgCallback[instance]();
    }
}

/* =============================================  EOF  ============================================== */
