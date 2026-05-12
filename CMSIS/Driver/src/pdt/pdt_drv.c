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
 * @file pdt_drv.c
 *
 * @brief This file provides PDT integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "pdt_hw.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* PDT base address */
static PDT_Type *s_pdtBase[PDT_INSTANCE_MAX] = PDT_BASE_PTRS;

/* PDT callback pointer */
static pdt_callback_t s_pdtCallback[PDT_INSTANCE_MAX] = {NULL};

/* PDT interrupt request info */
const IRQn_Type s_pdtIrq[PDT_INSTANCE_MAX] = PDT_IRQS;

/* PDT ckgen clk info */
const ckgen_clock_t s_pdtCkgenClks[PDT_INSTANCE_MAX] = PDT_CKGEN_CLOCKS;
/* PDT soft reset info */
const ckgen_softreset_t s_pdtSoftReset[PDT_INSTANCE_MAX] = PDT_SOFT_RESETS;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief PDT module reset.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_ModuleReset(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    CKGEN_DRV_SoftReset(s_pdtSoftReset[instance], false);
    CKGEN_DRV_SoftReset(s_pdtSoftReset[instance], true);
}

/*!
 * @brief PDT module init.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] userConfig: Pointer to the user configuration structure
 * @return none
 */
void PDT_DRV_Init(const uint8_t instance, const pdt_timer_config_t * const userConfig)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);
    DEVICE_ASSERT(userConfig != NULL);

    PDT_Type *base = s_pdtBase[instance];

    CKGEN_DRV_Enable(s_pdtCkgenClks[instance], true);
    CKGEN_DRV_SoftReset(s_pdtSoftReset[instance], true);

    if (instance % 2U)
    {
        CKGEN_DRV_Enable(s_pdtCkgenClks[instance - 1U], true);
        CKGEN_DRV_SoftReset(s_pdtSoftReset[instance - 1U], true);
    }

    PDT_ConfigTimer(base, userConfig);

    /* Register callback function */
    s_pdtCallback[instance] = userConfig->callback;

    if (userConfig->intEnable)
    {
        NVIC_EnableIRQ(s_pdtIrq[instance]);
    }
    else
    {
        NVIC_DisableIRQ(s_pdtIrq[instance]);
    }
}

/*!
 * @brief Get PDT default config.
 *
 * @param[in] config: pointer to PDT default config
 * @return none
 */
void PDT_DRV_GetDefaultConfig(pdt_timer_config_t * const config)
{
    DEVICE_ASSERT(config != NULL);

    config->loadValueMode = PDT_LOAD_VAL_IMMEDIATELY;
    config->clkPreDiv = PDT_CLK_PREDIV_BY_1;
    config->clkPreMultFactor = PDT_CLK_PREMULT_FACT_AS_1;
    config->triggerInput = PDT_HARDWARE_TRIGGER;
    config->continuousModeEnable = false;
    config->intEnable = false;
}

/*!
 * @brief PDT enable.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_Enable(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_Enable(base);
}

/*!
 * @brief PDT disable.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_Disable(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_Disable(base);
}

/*!
 * @brief Set PDT load values cmd.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_LoadValuesCmd(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_LoadValuesCmd(base);
}

/*!
 * @brief Set PDT software trigger cmd.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_SoftTriggerCmd(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_SoftTriggerCmd(base);
}

/*!
 * @brief PDT ckgen/irq disable & module reset.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_Deinit(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    NVIC_DisableIRQ(s_pdtIrq[instance]);
    CKGEN_DRV_Enable(s_pdtCkgenClks[instance], false);
    CKGEN_DRV_SoftReset(s_pdtSoftReset[instance], false);
}

/*!
 * @brief Get PDT timer value.
 *
 * @param[in] instance: The PDT instance number
 * @return timer counting value
 */
uint32_t PDT_DRV_GetTimerValue(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    return PDT_GetTimerValue(base);
}

/*!
 * @brief Get PDT timer interrupt flag.
 *
 * @param[in] instance: The PDT instance number
 * @return timer interrupt flag
 */
bool PDT_DRV_GetTimerIntFlag(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    return (PDT_GetTimerIntFlag(base) != 0U);
}

/*!
 * @brief Clear PDT timer interrupt flag.
 *
 * @param[in] instance: The PDT instance number
 * @return none
 */
void PDT_DRV_ClearTimerIntFlag(const uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_ClearTimerIntFlag(base);
}

/*!
 * @brief Set PDT timer modulus value.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] value: PDT modulus value
 * @return none
 */
void PDT_DRV_SetTimerModulusValue(const uint8_t instance, const uint16_t value)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_SetTimerModulusValue(base, value);
}

/*!
 * @brief Set PDT timer interrupt value.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] value: PDT interrupt value
 * @return none
 */
void PDT_DRV_SetValueForTimerInterrupt(const uint8_t instance, const uint16_t value)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_SetValueForTimerInterrupt(base, value);
}

/*!
 * @brief Set PDT pulse out enable.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] enable
 *            - ENABLE: pulse output enable
 *            - DISABLE: pulse output disable
 * @return none
 */
void PDT_DRV_SetCmpPulseOutEnable(const uint8_t instance, bool enable)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_SetCmpPulseOutEnable(base, enable);
}

/*!
 * @brief Set PDT pulse out delay value for high.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] value: pulse out delay value for high
 * @return none
 */
void PDT_DRV_SetCmpPulseOutDelayForHigh(const uint8_t instance, uint16_t value)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_SetCmpPulseOutDelayForHigh(base, value);
}

/*!
 * @brief Set PDT pulse out delay value for low.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] value: pulse out delay value for low
 * @return none
 */
void PDT_DRV_SetCmpPulseOutDelayForLow(const uint8_t instance, uint16_t value)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_SetCmpPulseOutDelayForLow(base, value);
}

/*!
 * @brief Set PDT trigger Delay Value.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] chn: delay channel(0~7)
 * @param[in] value: trigger delay value
 * @return none
 */
void PDT_DRV_SetTriggerDelayValue(const uint8_t instance, const uint8_t chn, const uint16_t value)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    PDT_Type *base = s_pdtBase[instance];

    PDT_SetTriggerDelayValue(base, chn, value);
}

/*!
 * @brief Config PDT delay value and output enable.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] delayConfig: Pointer to the PDT delay configuration structure.
 * @return none
 */
void PDT_DRV_ConfigTriggerDelay(const uint8_t instance, const pdt_trigger_delay_config_t *delayConfig)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);
    DEVICE_ASSERT(delayConfig != NULL);

    PDT_Type *base = s_pdtBase[instance];
    uint8_t i = 0U;

    for (i = 0U; i < 8U; i++)
    {
        PDT_SetTriggerDelayValue(base, i, delayConfig->dly[i]);
    }
    PDT_SetTriggerDelayEnable(base, delayConfig->delayEnable);
    PDT_SetTriggerDelayBypass(base, delayConfig->triggerDelayBypassEn);
}

/*!
 * @brief Config PDT pulse out width and output enable.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] config: pointer to pulse out configuration structure
 * @return none
 */
void PDT_DRV_ConfigPulseOut(const uint8_t instance, const pdt_pulse_out_config_type_t *config)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);
    DEVICE_ASSERT(config != NULL);

    PDT_Type *base = s_pdtBase[instance];

    PDT_ConfigPulseOut(base, config);
}

/*!
 * @brief Set PDT callback function.
 *
 * @param[in] instance: The PDT instance number
 * @param[in] func: callback function
 * @return none
 */
void PDT_DRV_InstallCallback(uint8_t instance, const pdt_callback_t func)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    s_pdtCallback[instance] = func;
}

/*!
* @brief PDT common interrupt service routine.
*
* @param[in] instance: The PDT instance number
* @return none
*/
void PDT_DRV_IRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < PDT_INSTANCE_MAX);

    bool wpara = true;

    /* store device status */
    wpara = PDT_DRV_GetTimerIntFlag(instance);

    if (wpara)
    {
        /* clear device status*/
        PDT_DRV_ClearTimerIntFlag(instance);
        if (NULL != s_pdtCallback[instance])
        {
            s_pdtCallback[instance](instance, wpara, NULL);
        }
    }

}

/* =============================================  EOF  ============================================== */
