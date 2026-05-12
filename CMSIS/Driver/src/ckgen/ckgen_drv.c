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
 * @file ckgen_drv.c
 *
 * @brief This file provides ckgen integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "ckgen_hw.h"
#include "../rcm/rcm_hw.h"
#include "../spm/spm_hw.h"

/* ============================================  Define  ============================================ */
/* Time to wait for HSI to stabilize (number of
   cycles when core runs at maximum speed - 120 MHz */
#define CKGEN_HSI_STABILIZATION_TIMEOUT    (100UL)

/* Time to wait for VHSI to stabilize (number of
   cycles when core runs at maximum speed - 120 MHz */
#define CKGEN_VHSI_STABILIZATION_TIMEOUT   (100UL)

/* Time to wait for HSE to stabilize (number of
   cycles when core runs at maximum speed - 120 MHz */
#define CKGEN_HSE_STABILIZATION_TIMEOUT    (100000UL)

/* Time to wait for SPLL to stabilize (number of
   cycles when core runs at maximum speed - 120 MHz */
#define CKGEN_SPLL_STABILIZATION_TIMEOUT   (10000UL)

/* Time to wait for System to stabilize (number of
   cycles when core runs at maximum speed - 120 MHz */
#define CKGEN_SYSTEM_STABILIZATION_TIMEOUT (100UL)

/* System PLL feedback divider value. */
#define CKGEN_SPLL_FBKDIV_MIN              (5UL)
#define CKGEN_SPLL_FBKDIV_MAX              (255UL)

/* System PLL predivider value */
#define CKGEN_SPLL_PREDIV_BASE             (1UL)
#define CKGEN_SPLL_PREDIV_MAX              (3UL)

/* System PLL posdivider value */
#define CKGEN_SPLL_POSDIV_MIN              (1UL)
#define CKGEN_SPLL_POSDIV_MAX              (31UL)

/* System PLL reference clock */
#define CKGEN_SPLL_VCO_REF_MIN             (500000000UL)
#define CKGEN_SPLL_VCO_REF_MAX             (1500000000UL)

/* LSI 128K fixed clock frequency. */
#define LSI_128K_FREQUENCY                 (128000UL)

/* LSI 32K fixed clock frequency. */
#define LSI_32K_FREQUENCY                  (32000UL)

/* LSI 1K fixed clock frequency. */
#define LSI_1K_FREQUENCY                   (1000UL)

#define CKGEN_SYS_CLK_SRC_NUM_RUN          (4UL)
#define CKGEN_SYS_CLK_SRC_NUM_VLPR         (1UL)

#define CLOCK_MAX_FREQUENCIES_VLPR_MODE                                                     \
{/* SYS_CLK    BUS_CLK */                                                                   \
{ 8000000UL, 8000000UL},               /* Maximum frequencies when system clock is HSI */   \
}

#define CLOCK_MAX_FREQUENCIES_RUN_MODE                                                      \
{/* SYS_CLK    BUS_CLK */                                                                   \
{48000000UL,  48000000UL},             /* Maximum frequencies when system clock is VHSI */  \
{30000000UL,  30000000UL},             /* Maximum frequencies when system clock is HSE */   \
{8000000UL,   8000000UL},              /* Maximum frequencies when system clock is HSI */   \
{120000000UL, 60000000UL},             /* Maximum frequencies when system clock is SPLL */  \
}

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief system clock type.
 */
typedef enum
{
    CKGEN_SYSTEM_CLOCK_CORE = 0U,  /*!< Core clock. */
    CKGEN_SYSTEM_CLOCK_BUS,        /*!< BUS clock.  */
    CKGEN_SYSTEM_CLOCK_MAX,        /*!< Max value.  */
} ckgen_system_clock_type_t;

/*!
 * @brief asynchronous clock type.
 */
typedef enum
{
    CKGEN_ASYNC_CLOCK_DIV1 = 0U,   /*!< Clock divider 1  */
    CKGEN_ASYNC_CLOCK_DIV2 = 1U,   /*!< Clock divider 2  */
    CKGEN_ASYNC_CLOCK_MAX  = 2U,   /*!< Max value.       */
} ckgen_async_clock_type_t;

/*!
 * @brief system clock modes.
 */
typedef enum
{
    CKGEN_SYSTEM_CLOCK_MODE_CURRENT = 0U,  /*!< Current mode.            */
    CKGEN_SYSTEM_CLOCK_MODE_RUN     = 1U,  /*!< Run mode.                */
    CKGEN_SYSTEM_CLOCK_MODE_VLPR    = 2U,  /*!< Very Low Power Run mode. */
    CKGEN_SYSTEM_CLOCK_MODE_NONE    = 3U,  /*!< MAX value.               */
} ckgen_system_clock_mode_t;

/* ==========================================  Variables  =========================================== */
static clock_manager_state_t s_clockState;       /* Clock manager state */

/* This frequency values should be set by different boards. */
uint32_t g_TClkFreq[NUMBER_OF_TCLK_INPUTS];      /* TCLKx clocks */

/* RTC */
uint32_t g_RtcClkInFreq;                         /* RTC CLKIN clock */

/* HSE */
uint32_t g_HseClkFreq = CKGEN_HSE_FREQ;          /* HSE clock */

#if CKGEN_AUTO_SEL_HSI
static uint8_t s_autoSelect = 0U;                /* Auto select HSI clock for SPLL when enable XOSC is fail */
#endif

/* ====================================  Functions declaration  ===================================== */
static void CKGEN_GetDefaultConfiguration(clock_manager_user_config_t *config);

static status_t CKGEN_GetClockFreq(clock_names_t clockName, uint32_t *frequency);

static status_t CKGEN_GetSimClockFreq(clock_names_t clockName, uint32_t *frequency);

static status_t CKGEN_GetPccClockFreq(clock_names_t clockName, uint32_t *frequency);

static ckgen_system_clock_mode_t CKGEN_GetCurrentRunMode(void);

static status_t CKGEN_TransitionSystemClock(const ckgen_system_clock_config_t *to_clk);

static uint32_t CKGEN_GetSimClkOutFreq(void);

static uint32_t CKGEN_GetMux1ClkOutFreq(void);

static uint32_t CKGEN_GetRtcClkFreq(void);

static status_t CKGEN_ConfigureTemporarySystemClock(void);

static status_t CKGEN_ConfigureModules(const ckgen_config_t *ckgenConfig);

static status_t CKGEN_ConfigureHSI(bool enable, const ckgen_hsi_config_t *hsiConfig);

static status_t CKGEN_ConfigureVHSI(bool enable, const ckgen_vhsi_config_t *vhsiConfig);

static status_t CKGEN_ConfigureHSE(bool enable, const ckgen_hse_config_t *hseConfig);

static status_t CKGEN_ConfigureSPLL(bool enable, const ckgen_spll_config_t *spllConfig);

static uint32_t CKGEN_GetSystemClockFreq(ckgen_system_clock_type_t type);

static status_t CKGEN_SetSystemClockConfig(ckgen_system_clock_mode_t mode, ckgen_system_clock_config_t const *config);

static uint32_t CKGEN_GetSysAsyncFreq(clock_names_t clockSource, ckgen_async_clock_type_t type);

static uint32_t CKGEN_GetHSIFreq(void);

static uint32_t CKGEN_GetVHSIFreq(void);

static uint32_t CKGEN_GetHSEFreq(void);

static uint32_t CKGEN_GetSPLLFreq(void);

static uint32_t CKGEN_GetLSIFreq(void);

static status_t CKGEN_TransitionToTmpSysClk(ckgen_system_clock_src_t currentSysClkSrc);

static void CKGEN_GetCurrentSysClkConfig(ckgen_system_clock_config_t *sysClockConfig);

static status_t CKGEN_SetConfiguration(const ckgen_config_t *ckgenConfig);

static void CKGEN_SetPccConfiguration(const pcc_config_t *peripheralClockConfig);

static void CKGEN_SetSimConfiguration(const sim_clock_config_t *simClockConfig);

static status_t CKGEN_SetHSIClockSource(bool clockSourceEnable,
                                        const clock_source_config_t *clkSrcConfig,
                                        ckgen_async_clock_div_t divider1,
                                        ckgen_async_clock_div_t divider2);

static status_t CKGEN_SetVHSIClockSource(bool clockSourceEnable,
                                        const clock_source_config_t *clkSrcConfig,
                                        ckgen_async_clock_div_t divider1,
                                        ckgen_async_clock_div_t divider2);

static status_t CKGEN_SetHSEClockSource(bool clockSourceEnable,
                                        const clock_source_config_t *clkSrcConfig,
                                        ckgen_async_clock_div_t divider1,
                                        ckgen_async_clock_div_t divider2);

static status_t CKGEN_SetSPLLClockSource(bool clockSourceEnable,
                                         const clock_source_config_t *clkSrcConfig,
                                         ckgen_async_clock_div_t divider1,
                                         ckgen_async_clock_div_t divider2);

static ckgen_hse_ext_ref_t CKGEN_GetExtRef(hse_ref_t refClk);

static ckgen_system_clock_mode_t CKGEN_GetSysClockMode(const pwr_modes_t mode);

static ckgen_system_clock_src_t CKGEN_GetSysClockSource(clock_names_t src);

static uint32_t CKGEN_GetSrcFreq(ckgen_system_clock_src_t src);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set the module bus clock.
 *
 * @param[in] module: The module which will be set bus clock
 * @param[in] enable: true:enable bus clock, false: disable bus clock
 * @return none
 */
void CKGEN_DRV_Enable(ckgen_clock_t module, bool enable)
{
    uint32_t moduleValue = (uint32_t)module;

    DEVICE_ASSERT(module < CLK_MODULE_NUM);
    if (moduleValue < 32U)
    {
        if (enable == true)
        {
            SET_BIT32(CKGEN->PERI_CLK_EN0, ((uint32_t)1U << moduleValue));
        }
        else
        {
            CLEAR_BIT32(CKGEN->PERI_CLK_EN0, ((uint32_t)1U << moduleValue));
        }
    }
    else if (moduleValue < 64U)
    {
        moduleValue -= 32U;
        if (enable)
        {
            SET_BIT32(CKGEN->PERI_CLK_EN1, ((uint32_t)1U << moduleValue));
        }
        else
        {
            CLEAR_BIT32(CKGEN->PERI_CLK_EN1, ((uint32_t)1U << moduleValue));
        }
    }
    else if (moduleValue < (uint32_t)CLK_MODULE_NUM)
    {
        moduleValue -= 64U;
        if (enable)
        {
            SET_BIT32(CKGEN->PERI_CLK_EN2, ((uint32_t)1U << moduleValue));
        }
        else
        {
            CLEAR_BIT32(CKGEN->PERI_CLK_EN2, ((uint32_t)1U << moduleValue));
        }
    }
    else
    {
        /* do nothing */
    }
}

/*!
 * @brief Soft reset for the module.
 *
 * @param[in] module: The module to be Soft Reset
 * @param[in] enable: true: active, false: inactive
 * @return  none
 */
void CKGEN_DRV_SoftReset(ckgen_softreset_t module, bool enable)
{
    uint32_t moduleValue = (uint32_t)module;

    DEVICE_ASSERT(module < SRST_MODULE_NUM);
    if (moduleValue < 32U)
    {
        if (enable)
        {
            SET_BIT32(CKGEN->PERI_SFT_RST0, ((uint32_t)1U << moduleValue));
        }
        else
        {
            CLEAR_BIT32(CKGEN->PERI_SFT_RST0, ((uint32_t)1U << moduleValue));
        }
    }
    else if (moduleValue < 64U)
    {
        moduleValue -= 32U;
        if (enable)
        {
            SET_BIT32(CKGEN->PERI_SFT_RST1, ((uint32_t)1U << moduleValue));
        }
        else
        {
            CLEAR_BIT32(CKGEN->PERI_SFT_RST1, ((uint32_t)1U << moduleValue));
        }
    }
    else if (moduleValue < (uint32_t)SRST_MODULE_NUM)
    {
        moduleValue -= 64U;
        if (enable)
        {
            SET_BIT32(CKGEN->PERI_SFT_RST2, ((uint32_t)1U << moduleValue));
        }
        else
        {
            CLEAR_BIT32(CKGEN->PERI_SFT_RST2, ((uint32_t)1U << moduleValue));
        }
    }
    else
    {
        /* do nothing */
    }
}

#if CKGEN_AUTO_TEST_CLK
/*!
 * @brief Ckgen auto test clock.
 *
 * @param[in] none
 * @return none
 */
void CKGEN_AutoTest(void)
{
    uint32_t clkMux, clkEn, clkDiv, i, timeout = 100U;
    uint32_t freq = CKGEN_GetSPLLFreq() / 2U;

    WRITE_MEM32(0x40085148U, 0x00000001U);

    clkDiv = CKGEN->CLK_DIV2;
    MODIFY_REG32(CKGEN->CLK_DIV2, 0x3FU, 0U, 0x01U);
    MODIFY_REG32(CKGEN->CLK_DIV2, 0x3F000U, 12U, 0x00U);
    clkMux = CKGEN->PERI_CLK_MUX2;
    MODIFY_REG32(CKGEN->PERI_CLK_MUX2, 0xFFU, 0U, 0x57U);
    clkEn = CKGEN->PERI_CLK_EN0;
    SET_BIT32(CKGEN->PERI_CLK_EN0, 0x00000003U);
    CLEAR_BIT32(CKGEN->PERI_SFT_RST0, 0x00000003U);
    SET_BIT32(CKGEN->PERI_SFT_RST0, 0x00000003U);
    __asm("nop");

    WRITE_MEM32(0x40018004U, freq / 100000U / 16U);
    WRITE_MEM32(0x40018044U, ((freq / 100000U) % 16U) * 2U);
    WRITE_MEM32(0x4001800CU, 0x00000001U);
    WRITE_MEM32(0x40018014U, 0x00000001U);
    WRITE_MEM32(0x40018010U, 0x00000003U);

    WRITE_MEM32(0x40018204U, 0x00000005U);
    WRITE_MEM32(0x4001820CU, 0x00000001U);
    WRITE_MEM32(0x40018214U, 0x00000001U);
    WRITE_MEM32(0x40018210U, 0x00000003U);
    __asm("nop");

    while ((0U == (READ_MEM32(0x40018220U) & 0x20U)) && (--timeout > 0U));
    WRITE_MEM32(0x40018200U, 0xAAU);
    timeout = 5000U;
    while ((0U == (READ_MEM32(0x40018020U) & 0x1U)) && (--timeout > 0U));
    if (0xAAU != (uint8_t)READ_MEM32(0x40018000U))
    {
        WRITE_MEM32(0xE000ED0CU, (READ_MEM32(0xE000ED0CU) & 0x700U) | 0x05FA0004U);
    }

    CLEAR_BIT32(CKGEN->PERI_SFT_RST0, 0x00000003U);
    SET_BIT32(CKGEN->PERI_SFT_RST0, 0x00000003U);
    WRITE_REG32(CKGEN->PERI_CLK_EN0, clkEn);
    WRITE_REG32(CKGEN->PERI_CLK_MUX2, clkMux);
    WRITE_REG32(CKGEN->CLK_DIV2, clkDiv);

    WRITE_MEM32(0x40085148U, 0x00000000U);

    MODIFY_REG32(CKGEN->CLK_DIV2, CKGEN_CLK_DIV2_SPLL_DIV2_Msk, CKGEN_CLK_DIV2_SPLL_DIV2_Pos, 0U);
    for (i = 0U; i < 100U; i++)
    {
        __asm("nop");
    }
    MODIFY_REG32(CKGEN->CLK_DIV2, CKGEN_CLK_DIV2_SPLL_DIV2_Msk, CKGEN_CLK_DIV2_SPLL_DIV2_Pos, 1U);
}
#endif

/*!
 * @brief This function sets the system to target configuration, it
 *        only sets the clock modules registers for clock mode change, but not send
 *        notifications to drivers.
 *
 * @param[in] config: User clock configuration pointer
 * @return STATUS_SUCCESS, STATUS_BUSY, STATUS_TIMEOUT
 */
status_t CKGEN_DRV_SetConfiguration(clock_manager_user_config_t const *config)
{
    status_t result;
    clock_manager_user_config_t config_default;
    clock_manager_user_config_t const *cfg = config;

    DEVICE_ASSERT(CKGEN_SYSTEM_CLOCK_MODE_RUN == CKGEN_GetCurrentRunMode());

    if (NULL == config)
    {
        /* Init structure configuration by default value */
        CKGEN_GetDefaultConfiguration(&config_default);
        cfg = &config_default;
    }

    /* Set settings. */
    result = CKGEN_SetConfiguration(&cfg->ckgenConfig);

    if (STATUS_SUCCESS == result)
    {
        #if CKGEN_AUTO_TEST_CLK
        if (SPM_GetSPLLStatus())
        {
            CKGEN_AutoTest();
        }
        #endif
        /* Set SIM settings. */
        CKGEN_SetSimConfiguration(&cfg->simConfig);

        /* Set PCC settings. */
        CKGEN_SetPccConfiguration(&cfg->pccConfig);
    }

    return result;
}

/*!
 * @brief This function configures the internal system clock blocks.
 *
 * @param[in] ckgenConfig: User clock configuration pointer
 * @return STATUS_SUCCESS, STATUS_BUSY, STATUS_TIMEOUT
 */
static status_t CKGEN_SetConfiguration(const ckgen_config_t *ckgenConfig)
{
    DEVICE_ASSERT(ckgenConfig != NULL);

    status_t status = STATUS_SUCCESS;

    if (ckgenConfig != NULL)
    {
        /* Configure a temporary system clock source: VHSI */
        status = CKGEN_ConfigureTemporarySystemClock();

        if (STATUS_SUCCESS == status)
        {
            /* Configure clock sources */
            status = CKGEN_ConfigureModules(ckgenConfig);
        }

        if (STATUS_SUCCESS == status)
        {
            g_RtcClkInFreq = 0U; /* reset the value RTC_clk frequency. */
            if (ckgenConfig->rtcConfig.initialize)
            {
                /* RTC Clock settings. */
                g_RtcClkInFreq = ckgenConfig->rtcConfig.rtcClkInFreq;
            }

            /* Configure clock modes. */
            if (ckgenConfig->clockModeConfig.initialize)
            {
                /* Configure clock modes */
                status = CKGEN_SetSystemClockConfig(CKGEN_SYSTEM_CLOCK_MODE_RUN, &(ckgenConfig->clockModeConfig.rccrConfig));
                if (STATUS_SUCCESS == status)
                {
                    status = CKGEN_SetSystemClockConfig(CKGEN_SYSTEM_CLOCK_MODE_VLPR, &(ckgenConfig->clockModeConfig.vccrConfig));
                }
            }
        }
    }

    return status;
}

/*!
 * @brief This function configures the peripheral blocks.
 *
 * @param[in] peripheralClockConfig: User peripheral clock configuration pointer
 * @return none
 */
static void CKGEN_SetPccConfiguration(const pcc_config_t *peripheralClockConfig)
{
    DEVICE_ASSERT(peripheralClockConfig != NULL);

    uint32_t i;
    module_clk_config_t moduleClkConfig;

    if ((peripheralClockConfig != NULL) && (peripheralClockConfig->peripheralClocks != NULL))
    {
        for (i = 0U; i < peripheralClockConfig->count; i++)
        {
            moduleClkConfig.source = peripheralClockConfig->peripheralClocks[i].clkSrc;
            moduleClkConfig.div = (uint8_t)peripheralClockConfig->peripheralClocks[i].divider + 1U;
            CKGEN_DRV_SetModuleClock(peripheralClockConfig->peripheralClocks[i].clockName, &moduleClkConfig);
        }
    }
}

/*!
 * @brief This function configures the SIM blocks.
 *
 * @param[in] simClockConfig: User SIM clock configuration pointer
 * @return none
 */
static void CKGEN_SetSimConfiguration(const sim_clock_config_t *simClockConfig)
{
    DEVICE_ASSERT(simClockConfig != NULL);
    uint8_t i;

    /* ClockOut settings. */
    if (simClockConfig->clockOutConfig.initialize)
    {
        CKGEN_SetClockoutSourceSel(CKGEN, (uint32_t)(simClockConfig->clockOutConfig.source));
        CKGEN_SetClockoutDivider(CKGEN, (uint32_t)simClockConfig->clockOutConfig.divider);
        CKGEN_SetClockout(CKGEN, simClockConfig->clockOutConfig.enable);
    }

    /* Low Power Clock settings from SIM. */
    if (simClockConfig->lpClockConfig.initialize)
    {
        CKGEN_SetRTCLSIClock(simClockConfig->lpClockConfig.sourceRtcClk, \
                             simClockConfig->lpClockConfig.sourceLsiClk);
    }

    /* TCLK Clock settings. */
    if (simClockConfig->tclkConfig.initialize)
    {
        for (i = 0U; i < NUMBER_OF_TCLK_INPUTS; i++)
        {
            g_TClkFreq[i] = simClockConfig->tclkConfig.tclkFreq[i];
        }
    }

    /* Debug trace Clock settings. */
    if (simClockConfig->traceClockConfig.initialize)
    {
        CKGEN_SetTPIUClkDiv(simClockConfig->traceClockConfig.divider);
    }
}

/*!
 * @brief Get the user clock default configuration.
 *
 * @param[out] config: User clock default configuration pointer
 * @return none
 */
static void CKGEN_GetDefaultConfiguration(clock_manager_user_config_t *config)
{
    static peripheral_clock_config_t peripheralClockConfig[13] = {
        {
            .clockName = I2C0_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = TIMER_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = SPI0_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = SPI1_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = SPI2_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = ADC0_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = ADC1_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = PCT_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = EIO_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = UART0_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = UART1_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = UART2_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
        {
            .clockName = UART3_CLK,
            .clkSrc    = HSIDIV2_CLK,
        },
    };

    /* HSI */
    config->ckgenConfig.hsiConfig.initialize               = true;                                /* Initialize */
    config->ckgenConfig.hsiConfig.enableInLowPower         = true;                                /* Enable in VLPS mode */
    config->ckgenConfig.hsiConfig.div1                     = CKGEN_ASYNC_CLOCK_DIV_BY_1;          /* HSIDIV1    */
    config->ckgenConfig.hsiConfig.div2                     = CKGEN_ASYNC_CLOCK_DIV_BY_1;          /* HSIDIV2    */

    /* VHSI */
    config->ckgenConfig.vhsiConfig.initialize               = true;                               /* Initialize */
    config->ckgenConfig.vhsiConfig.div1                     = CKGEN_ASYNC_CLOCK_DIV_BY_1;         /* VHSIDIV1   */
    config->ckgenConfig.vhsiConfig.div2                     = CKGEN_ASYNC_CLOCK_DIV_BY_1;         /* VHSIDIV2   */

    /* RTC */
    config->ckgenConfig.rtcConfig.initialize                = true;                               /* Initialize  */
    config->ckgenConfig.rtcConfig.rtcClkInFreq              = 0U;                                 /* RTC_CLKIN   */

    /* HSE */
    config->ckgenConfig.hseConfig.initialize               = true;                                /* Initialize */
    config->ckgenConfig.hseConfig.freq                     = 8000000U;                            /* Frequency  */
    config->ckgenConfig.hseConfig.extRef                   = CKGEN_HSE_REF_OSC;                   /* HSEREF     */
    config->ckgenConfig.hseConfig.div1                     = CKGEN_ASYNC_CLOCK_DIV_BY_1;          /* HSEDIV1    */
    config->ckgenConfig.hseConfig.div2                     = CKGEN_ASYNC_CLOCK_DIV_BY_1;          /* HSEDIV2    */

    /* SPLL */
    config->ckgenConfig.spllConfig.initialize               = true;                                      /* Initialize */
    config->ckgenConfig.spllConfig.prediv                   = (uint8_t)CKGEN_SPLL_CLOCK_PREDIV_BY_1;     /* PREDIV     */
    config->ckgenConfig.spllConfig.fbkdiv                   = 96U;                                       /* FBKDIV     */
    config->ckgenConfig.spllConfig.posdiv                   = (uint8_t)CKGEN_SPLL_CLOCK_POSDIV_BY_16;    /* POSDIV     */
    config->ckgenConfig.spllConfig.src                      = (uint8_t)CKGEN_SPLL_REF_HSI;               /* SOURCE     */

    config->ckgenConfig.spllConfig.div1                     = CKGEN_ASYNC_CLOCK_DIV_BY_2;         /* SPLLDIV1   */
    config->ckgenConfig.spllConfig.div2                     = CKGEN_ASYNC_CLOCK_DIV_BY_2;         /* SPLLDIV2   */

    /* Clock config for RUN */
    config->ckgenConfig.clockModeConfig.initialize          = true;                               /* Initialize */
    config->ckgenConfig.clockModeConfig.rccrConfig.src      = CKGEN_SYSTEM_CLOCK_SRC_VHSI;        /* SCS        */
    config->ckgenConfig.clockModeConfig.rccrConfig.divCore  = CKGEN_SYSTEM_CLOCK_DIV_BY_1;        /* DIVCORE    */
    config->ckgenConfig.clockModeConfig.rccrConfig.divBus   = CKGEN_SYSTEM_CLOCK_DIV_BY_2;        /* DIVBUS     */

    /* Clock config for VLPR */
    config->ckgenConfig.clockModeConfig.vccrConfig.src      = CKGEN_SYSTEM_CLOCK_SRC_HSI;         /* SCS        */
    config->ckgenConfig.clockModeConfig.vccrConfig.divCore  = CKGEN_SYSTEM_CLOCK_DIV_BY_1;        /* DIVCORE    */
    config->ckgenConfig.clockModeConfig.vccrConfig.divBus   = CKGEN_SYSTEM_CLOCK_DIV_BY_2;        /* DIVBUS     */

    /* PCC */
    config->pccConfig.peripheralClocks                    = peripheralClockConfig;              /* Peripheral clock control configurations */
    config->pccConfig.count                               = 13U;                                /* Number of the peripheral clock control configurations */

    /* SIM */
    /* Clock Out configuration.           */
    config->simConfig.clockOutConfig.initialize           = true;                               /* Initialize    */
    config->simConfig.clockOutConfig.enable               = false;                              /* CLKOUTEN      */
    config->simConfig.clockOutConfig.source               = CKGEN_CLKOUT_SEL_VHSI_CLK;          /* CLKOUTSEL     */
    config->simConfig.clockOutConfig.divider              = CKGEN_CLKOUT_DIV_BY_1;              /* CLKOUTDIV     */

    /* LSI or RTC */
    config->simConfig.lpClockConfig.initialize           = true;                                /* Initialize    */
    config->simConfig.lpClockConfig.sourceLsiClk         = LSI_CLK_SEL_128K;                    /* LSICLKSEL     */
    config->simConfig.lpClockConfig.sourceRtcClk         = RTC_CLK_SEL_LSI_32K;                 /* RTCCLKSEL     */

    /* PWM TCLK CLOCK configuration. */
    config->simConfig.tclkConfig.initialize               = true;                               /* Initialize    */
    config->simConfig.tclkConfig.tclkFreq[0]              = 0U;                                 /* TCLK0         */
    config->simConfig.tclkConfig.tclkFreq[1]              = 0U;                                 /* TCLK0         */
    config->simConfig.tclkConfig.tclkFreq[2]              = 0U;                                 /* TCLK0         */

    /* Debug trace Clock Configuration. */
    config->simConfig.traceClockConfig.initialize         = true;                               /* Initialize    */
    config->simConfig.traceClockConfig.divider            = 0U;                                 /* TRACEDIV      */
}

/*!
 * @brief Get the frequency of system clock source or internal other clock.
 *
 * @param[in] clockName: Module name
 * @param[out] frequency: Get module clock source frequency
 * @return STATUS_UNSUPPORTED or STATUS_SUCCESS
 */
static status_t CKGEN_GetClockFreq(clock_names_t clockName, uint32_t *frequency)
{
    status_t returnCode = STATUS_SUCCESS;
    uint32_t freq = 0U;

    switch (clockName)
    {
    /* Main clocks */
    case CORE_CLK:
        freq = CKGEN_GetSystemClockFreq(CKGEN_SYSTEM_CLOCK_CORE);
        break;

    case BUS_CLK:
        freq = CKGEN_GetSystemClockFreq(CKGEN_SYSTEM_CLOCK_BUS);
        break;

    case CLKOUT_CLK:
        freq = CKGEN_GetSimClkOutFreq();
        break;

    /* Other internal clocks used by peripherals. */
    case HSI_CLK:
        freq = CKGEN_GetHSIFreq();
        break;

    case VHSI_CLK:
        freq = CKGEN_GetVHSIFreq();
        break;

    case HSE_CLK:
        freq = CKGEN_GetHSEFreq();
        break;

    case SPLL_CLK:
        freq = CKGEN_GetSPLLFreq();
        break;

    case RTC_CLKIN_CLK:
        freq = g_RtcClkInFreq;
        break;

    case CKGEN_CLKOUT_CLK:
        freq = CKGEN_GetMux1ClkOutFreq();
        break;

    case HSIDIV1_CLK:
        freq = CKGEN_GetSysAsyncFreq(HSI_CLK, CKGEN_ASYNC_CLOCK_DIV1);
        break;

    case HSIDIV2_CLK:
        freq = CKGEN_GetSysAsyncFreq(HSI_CLK, CKGEN_ASYNC_CLOCK_DIV2);
        break;

    case VHSIDIV1_CLK:
        freq = CKGEN_GetSysAsyncFreq(VHSI_CLK, CKGEN_ASYNC_CLOCK_DIV1);
        break;

    case VHSIDIV2_CLK:
        freq = CKGEN_GetSysAsyncFreq(VHSI_CLK, CKGEN_ASYNC_CLOCK_DIV2);
        break;

    case HSEDIV1_CLK:
        freq = CKGEN_GetSysAsyncFreq(HSE_CLK, CKGEN_ASYNC_CLOCK_DIV1);
        break;

    case HSEDIV2_CLK:
        freq = CKGEN_GetSysAsyncFreq(HSE_CLK, CKGEN_ASYNC_CLOCK_DIV2);
        break;

    case SPLLDIV1_CLK:
        freq = CKGEN_GetSysAsyncFreq(SPLL_CLK, CKGEN_ASYNC_CLOCK_DIV1);
        break;

    case SPLLDIV2_CLK:
        freq = CKGEN_GetSysAsyncFreq(SPLL_CLK, CKGEN_ASYNC_CLOCK_DIV2);
        break;

    default:
        returnCode = STATUS_UNSUPPORTED;
        break;
    }

    if (frequency != NULL)
    {
        *frequency = freq;
    }

    return returnCode;
}

/*!
 * @brief This function returns the frequency of a given clock from SIM.
 *
 * @param[in] clockName: Module name
 * @param[out] frequency: Get module clock source frequency
 * @return STATUS_UNSUPPORTED or STATUS_SUCCESS
 */
static status_t CKGEN_GetSimClockFreq(clock_names_t clockName, uint32_t *frequency)
{
    status_t returnCode = STATUS_SUCCESS;
    uint32_t freq = 0U;
    uint8_t extSrcEnable = 0U, internalSrc = 0U, extSrc = 0U;

    switch (clockName)
    {
    case PWM0_EXT_CLK:
        CKGEN_GetPWMClock(0U, &extSrcEnable, &internalSrc, &extSrc);
        if (extSrc < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_TClkFreq[extSrc];
        }
        break;

    case PWM1_EXT_CLK:
        CKGEN_GetPWMClock(1U, &extSrcEnable, &internalSrc, &extSrc);
        if (extSrc < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_TClkFreq[extSrc];
        }
        break;

    case PWM2_EXT_CLK:
        CKGEN_GetPWMClock(2U, &extSrcEnable, &internalSrc, &extSrc);
        if (extSrc < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_TClkFreq[extSrc];
        }
        break;

    case PWM3_EXT_CLK:
        CKGEN_GetPWMClock(3U, &extSrcEnable, &internalSrc, &extSrc);
        if (extSrc < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_TClkFreq[extSrc];
        }
        break;

    case PWM4_EXT_CLK:
        CKGEN_GetPWMClock(4U, &extSrcEnable, &internalSrc, &extSrc);
        if (extSrc < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_TClkFreq[extSrc];
        }
        break;

    case PWM5_EXT_CLK:
        CKGEN_GetPWMClock(5U, &extSrcEnable, &internalSrc, &extSrc);
        if (extSrc < NUMBER_OF_TCLK_INPUTS)
        {
            freq = g_TClkFreq[extSrc];
        }
        break;

    case CLKOUTSEL:
        freq = CKGEN_GetSimClkOutFreq();
        break;

    case RTC_CLK:
        freq = CKGEN_GetRtcClkFreq();
        break;

    case LSI_CLK:
        freq = CKGEN_GetLSIFreq();
        break;

    case LSI_1K_CLK:
        freq = LSI_1K_FREQUENCY;

        break;
    case LSI_32K_CLK:
        freq = LSI_32K_FREQUENCY;
        break;

    case LSI_128K_CLK:
        freq = LSI_128K_FREQUENCY;
        break;

    default:
        returnCode = STATUS_UNSUPPORTED;
        break;
    }

    if (frequency != NULL)
    {
        *frequency = freq;
    }

    return returnCode;
}

/*!
 * @brief This function returns the clock frequency of peripheral functional clock.
 *
 * @param[in] clockName: Module name
 * @param[out] frequency: Get module clock source frequency
 * @return STATUS_UNSUPPORTED or STATUS_SUCCESS
 */
static status_t CKGEN_GetPccClockFreq(clock_names_t clockName, uint32_t *frequency)
{
    status_t returnCode = STATUS_SUCCESS;
    uint32_t freq = 0U, instance;
    clock_names_t source;
    can_clock_divider_t divider;
    uint8_t pwmExtClockEn = 0U, pwmInternalSrc = 0U, pwmExternalSrc = 0U;

    if (((uint32_t)clockName >= CKGEN_PERI_CLK_BASE) && (clockName < PWM0_CLK))
    {
        /* Get CAN module clock source */
        if ((clockName >= CAN0_CLK) && (clockName < PCT_CLK))
        {
            instance = (uint32_t)((uint32_t)clockName - (uint32_t)CAN0_CLK);
            source = CKGEN_GetCANClock((uint8_t)instance);
            returnCode = CKGEN_DRV_GetFreq(source, &freq);
            divider = CKGEN_GetCANClockDivider((uint8_t)instance);

            freq = freq / ((uint32_t)divider + 1U);
        }
        else /* Set other module clock source in clock mux0~2 */
        {
            source = (clock_names_t)((CKGEN_GetPeripheralClock(clockName) & 0x3U) + (uint8_t)HSEDIV2_CLK);
            returnCode = CKGEN_DRV_GetFreq(source, &freq);
        }
    }
    else if ((clockName >= PWM0_CLK) && (clockName < PCC_END_OF_CLOCKS)) /* Set PWM clock source */
    {
        instance = (uint32_t)((uint32_t)clockName - (uint32_t)(PWM0_CLK));
        CKGEN_GetPWMClock(instance, &pwmExtClockEn, &pwmInternalSrc, &pwmExternalSrc);

        if (pwmExtClockEn != 0U) /* PWM external clock source */
        {
            returnCode = CKGEN_DRV_GetFreq((clock_names_t)(instance + (uint32_t)PWM0_EXT_CLK), &freq);
        }
        else  /* PWM internal clock source */
        {
            returnCode = CKGEN_DRV_GetFreq((clock_names_t)(pwmInternalSrc + (uint32_t)HSEDIV1_CLK), &freq);
        }
    }
    else
    {
        returnCode = STATUS_UNSUPPORTED;
    }

    /* If frequency reference is provided, write this value */
    if (frequency != NULL)
    {
        *frequency = freq;
    }

    return returnCode;
}

/*!
 * @brief Get frequency which moudel is defined in clock_names_t.
 *
 * @param[in] clockName: Clock names defined in clock_names_t
 * @param[out] frequency: Returned clock frequency value in Hertz
 * @return status defined in status_t
 */
status_t CKGEN_DRV_GetFreq(clock_names_t clockName, uint32_t *frequency)
{
    status_t returnCode = STATUS_SUCCESS;

    /* Frequency of the clock name from CKGEN */
    if (clockName < CKGEN_END_OF_CLOCKS)
    {
        returnCode = CKGEN_GetClockFreq(clockName, frequency);
    }
    /* Frequency of the clock name from SIM */
    else if (clockName < SIM_END_OF_CLOCKS)
    {
        returnCode = CKGEN_GetSimClockFreq(clockName, frequency);
    }
    /* Frequency of the clock name from PCC */
    else if (clockName < PCC_END_OF_CLOCKS)
    {
        returnCode = CKGEN_GetPccClockFreq(clockName, frequency);
    }
    /* Invalid clock name */
    else
    {
        returnCode = STATUS_UNSUPPORTED;
    }

    return returnCode;
}

/*!
 * @brief Get current run mode.
 *
 * @param[in] none
 * @return Run mode.
 */
static ckgen_system_clock_mode_t CKGEN_GetCurrentRunMode(void)
{
    ckgen_system_clock_mode_t mode;

    /* Get the current running mode */
    switch (SPM_DRV_GetPowerMode())
    {
    /* Run mode */
    case POWER_MODE_RUN:
        mode = CKGEN_SYSTEM_CLOCK_MODE_RUN;
        break;

    /* Very low power run mode */
    case POWER_MODE_VLPR:
        mode = CKGEN_SYSTEM_CLOCK_MODE_VLPR;
        break;

    /* This should never happen - core has to be in some run mode to execute code */
    default:
        mode = CKGEN_SYSTEM_CLOCK_MODE_NONE;
        break;
    }

    return mode;
}

/*!
 * @brief Set system clock.
 *
 * @param[in] to_clk: The system clock configuration which will be set
 * @return STATUS_SUCCESS, STATUS_ERROR or STATUS_TIMEOUT
 */
static status_t CKGEN_TransitionSystemClock(const ckgen_system_clock_config_t *to_clk)
{
    ckgen_system_clock_mode_t run_mode;
    status_t retValue = STATUS_SUCCESS;
    uint32_t timeout;

    /* Check destination clock */
    DEVICE_ASSERT(to_clk != NULL);
    DEVICE_ASSERT(to_clk->src != CKGEN_SYSTEM_CLOCK_SRC_NONE);

    /* Get & Convert Run mode from SMC to CKGEN defines*/
    run_mode = CKGEN_GetCurrentRunMode();

    /* Check the current mode */
    DEVICE_ASSERT(run_mode != CKGEN_SYSTEM_CLOCK_MODE_NONE);

    /* Update run mode configuration */
    retValue = CKGEN_SetSystemClockConfig(run_mode, to_clk);

    if (STATUS_SUCCESS == retValue)
    {
        /* Wait for system clock to transition. */
        timeout = CKGEN_SYSTEM_STABILIZATION_TIMEOUT;
        do
        {
            timeout--;
        }
        while ((CKGEN_GetCurrentSystemClockSource(CKGEN) != ((uint32_t)(to_clk->src))) && (timeout > 0U));

        if (0U == timeout)
        {
            retValue = STATUS_TIMEOUT;
        }
    }

    return retValue;
}

/*!
 * @brief Configure the HSI.
 *
 * @param[in] enable: Configure HSI or not.
 * @param[in] hsiConfig: HSI configuration pointer
 * @return STATUS_BUSY, STATUS_TIMEOUT or STATUS_SUCCESS
 */
static status_t CKGEN_ConfigureHSI(bool enable, const ckgen_hsi_config_t *hsiConfig)
{
    status_t status = STATUS_SUCCESS;
    ckgen_hsi_config_t hsiDefaultConfig;
    const ckgen_hsi_config_t *hsiCfg;
    uint32_t timeout;

    if (NULL == hsiConfig)
    {
        hsiDefaultConfig.div1 = CKGEN_ASYNC_CLOCK_DIV_BY_1;
        hsiDefaultConfig.div2 = CKGEN_ASYNC_CLOCK_DIV_BY_1;
        hsiDefaultConfig.enableInLowPower = true;
        hsiCfg = &hsiDefaultConfig;
    }
    else
    {
        hsiCfg = hsiConfig;
    }

    /* If HSI clock is used by system, return error. */
    if ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSI == CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
        status = STATUS_BUSY;
    }
    else
    {
        /* Configure HSI. */
        if (enable)
        {
            /* Setup HSI dividers. */
            CKGEN_SetHSIAsyncConfig(CKGEN, (uint32_t)hsiCfg->div1, (uint32_t)hsiCfg->div2);

            /* Configure HSI clock. */
            SPM_EnableHSI(enable);
            SPM_EnableHSIInVLPS(hsiCfg->enableInLowPower);

            /* Wait for HSI to initialize */
            timeout = CKGEN_HSI_STABILIZATION_TIMEOUT;
            while ((0U == CKGEN_GetHSIFreq()) && (timeout > 0U))
            {
                timeout--;
            }
            if (0U == timeout)
            {
                status = STATUS_TIMEOUT;
            }
        }
    }

    return status;
}

/*!
 * @brief Configure the VHSI.
 *
 * @param[in] enable: Configure VHSI or not.
 * @param[in] vhsiConfig: VHSI configuration pointer
 * @return STATUS_SUCCESS, STATUS_BUSY or STATUS_TIMEOUT
 */
static status_t CKGEN_ConfigureVHSI(bool enable, const ckgen_vhsi_config_t *vhsiConfig)
{
    status_t status = STATUS_SUCCESS;
    ckgen_vhsi_config_t vhsiDefaultConfig;
    const ckgen_vhsi_config_t *vhsiCfg;
    uint32_t timeout;

    if (NULL == vhsiConfig)
    {
        vhsiDefaultConfig.div1 = CKGEN_ASYNC_CLOCK_DIV_BY_1;
        vhsiDefaultConfig.div2 = CKGEN_ASYNC_CLOCK_DIV_BY_1;
        vhsiCfg = &vhsiDefaultConfig;
    }
    else
    {
        vhsiCfg = vhsiConfig;
    }

    /* If VHSI clock is used by system, return error. */
    if ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_VHSI == CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
        status = STATUS_BUSY;
    }
    else
    {
        /* Configure VHSI. */
        if (enable)
        {
            /* Setup VHSI dividers. */
            CKGEN_SetVHSIAsyncConfig(CKGEN, (uint32_t)vhsiCfg->div1, (uint32_t)vhsiCfg->div2);

            /* Configure VHSI clock. */
            SPM_EnableVHSI(enable);

            /* Wait for VHSI to initialize */
            timeout = CKGEN_VHSI_STABILIZATION_TIMEOUT;
            while ((0U == CKGEN_GetVHSIFreq()) && (timeout > 0U))
            {
                timeout--;
            }
            if (0U == timeout)
            {
                status = STATUS_TIMEOUT;
            }
        }
        else
        {
            SPM_EnableVHSI(false);
        }
    }

    return status;
}

/*!
 * @brief Configure the HSE.
 *
 * @param[in] enable: Configure HSE or not
 * @param[in] hseConfig: HSE configuration pointer
 * @return STATUS_SUCCESS, STATUS_BUSY or STATUS_TIMEOUT
 */
static status_t CKGEN_ConfigureHSE(bool enable, const ckgen_hse_config_t *hseConfig)
{
    status_t status = STATUS_SUCCESS;
    ckgen_hse_config_t hseDefaultConfig;
    const ckgen_hse_config_t *hseCfg;
    uint32_t timeout;

    if (NULL == hseConfig)
    {
        hseDefaultConfig.div1 = CKGEN_ASYNC_CLOCK_DIV_BY_1;
        hseDefaultConfig.div2 = CKGEN_ASYNC_CLOCK_DIV_BY_1;
        hseDefaultConfig.extRef = CKGEN_HSE_REF_OSC;
        hseDefaultConfig.freq = 8000000U;

        hseCfg = &hseDefaultConfig;
    }
    else
    {
        hseCfg = hseConfig;
    }

    /* If HSE clock is used by system, return error. */
    if ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSE == CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
        status = STATUS_BUSY;
    }
    else
    {
        /* Configure HSE. */
        if (enable)
        {
            /* Disable lock bit field */
            CKGEN_CtrlRegisterLock(false);

            /* Setup HSE dividers. */
            CKGEN_SetHSEAsyncConfig(CKGEN, (uint32_t)hseCfg->div1, (uint32_t)hseCfg->div2);

            /* Configure HSE clock. */
            SPM_EnableXOSCBypassMode((bool)hseCfg->extRef);
            SPM_EnableXOSC(enable);

            g_HseClkFreq = hseCfg->freq;

            /* Wait for HSE to initialize */
            timeout = CKGEN_HSE_STABILIZATION_TIMEOUT;
            while ((0U == CKGEN_GetHSEFreq()) && (timeout > 0U))
            {
                timeout--;
            }
            if (0U == timeout)
            {
                status = STATUS_TIMEOUT;
            }
            else
            {
                /* Set XOSC monitor */
                switch (hseCfg->monitor)
                {
                case CKGEN_XOSC_MONITOR_DISABLE:
                    CKGEN_SetXOSCMonitor(CKGEN, false);
                    RCM_DRV_SetResetSource(RCM_RESET_SOURCE_XOSC_LOSS, false);
                    RCM_DRV_SetResetInterruptSource(RCM_RESET_SOURCE_XOSC_LOSS, false, (rcm_reset_delay_time_t)0U);
                    break;

                case CKGEN_XOSC_MONITOR_INT:
                    CKGEN_SetXOSCMonitor(CKGEN, true);
                    RCM_DRV_SetResetSource(RCM_RESET_SOURCE_XOSC_LOSS, false);
                    RCM_DRV_SetResetInterruptSource(RCM_RESET_SOURCE_XOSC_LOSS, true, (rcm_reset_delay_time_t)0U);
                    NVIC_EnableIRQ(RCM_IRQn);
                    break;

                case CKGEN_XOSC_MONITOR_RESET:
                    CKGEN_SetXOSCMonitor(CKGEN, true);
                    RCM_DRV_SetResetSource(RCM_RESET_SOURCE_XOSC_LOSS, true);
                    RCM_DRV_SetResetInterruptSource(RCM_RESET_SOURCE_XOSC_LOSS, false, (rcm_reset_delay_time_t)0U);
                    break;

                default:
                    break;
                }
            }
        }
        else
        {
            SPM_EnableXOSC(false);
            g_HseClkFreq = 0U;
        }
    }

    return status;
}

/*!
 * @brief Configure the SPLL.
 *
 * @param[in] enable: Configure SPLL or not
 * @param[in] spllConfig: SPLL configuration pointer
 * @return STATUS_SUCCESS, STATUS_BUSY or STATUS_TIMEOUT
 */
static status_t CKGEN_ConfigureSPLL(bool enable, const ckgen_spll_config_t *spllConfig)
{
    status_t status = STATUS_SUCCESS;
    ckgen_spll_config_t spllDefaultConfig;
    const ckgen_spll_config_t *spllCfg;
    uint32_t srcFreq, timeout;

    if (NULL == spllConfig)
    {
        spllDefaultConfig.div1   = CKGEN_ASYNC_CLOCK_DIV_BY_2;
        spllDefaultConfig.div2   = CKGEN_ASYNC_CLOCK_DIV_BY_2;

        spllDefaultConfig.prediv = 0U;
        spllDefaultConfig.fbkdiv = 96U;
        spllDefaultConfig.posdiv = 8U;
        spllDefaultConfig.src    = 0U;

        spllCfg = &spllDefaultConfig;
    }
    else
    {
        spllCfg = spllConfig;
    }

    /* If clock is used by system, return error. */
    if ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_SPLL == CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
        status = STATUS_BUSY;
    }
    else
    {
        /* Disable SPLL */
        SPM_EnableSPLL(false);
    }

    /* Configure SPLL. */
    if (enable && (STATUS_SUCCESS == status))
    {
        /* Disable lock bit field */
        CKGEN_CtrlRegisterLock(false);

        /* Get PLL clock source frequency */
        if (0U == spllCfg->src) /* PLL clock source is HSI */
        {
            srcFreq = CKGEN_GetHSIFreq();
        }
        else  /* PLL clock source is HSE */
        {
            srcFreq = CKGEN_GetHSEFreq();
            #if CKGEN_AUTO_SEL_HSI
            if ((0U == srcFreq) && (1U == s_autoSelect))
            {
                srcFreq = CKGEN_HSI_FREQ;
            }
            #endif
        }
        DEVICE_ASSERT(srcFreq != 0U);

        /* VCO checking. */
        srcFreq /= (((uint32_t)spllCfg->prediv) + CKGEN_SPLL_PREDIV_BASE);
        srcFreq *= ((uint32_t)spllCfg->fbkdiv);
        DEVICE_ASSERT((srcFreq >= CKGEN_SPLL_VCO_REF_MIN) && (srcFreq <= CKGEN_SPLL_VCO_REF_MAX));
        (void)srcFreq;

        /* Setup SPLL dividers. */
        CKGEN_SetSPLLAsyncConfig(CKGEN, (uint32_t)spllCfg->div1, 1U);

        /* Configure SPLL clock. */
        #if CKGEN_AUTO_SEL_HSI
        if (1U == s_autoSelect)
        {
            CKGEN_SetSourceSPLL(CKGEN, (uint8_t)CKGEN_SPLL_REF_HSI);
        }
        else
        #endif
        {
            CKGEN_SetSourceSPLL(CKGEN, spllCfg->src);
        }
        CKGEN_SetSPLLConfiguration(ANA, spllCfg->posdiv, spllCfg->fbkdiv, spllCfg->prediv);
        SPM_EnableSPLL(enable);

        /* Set SPLL lock detect */
        switch (spllCfg->lockDetect)
        {
        case CKGEN_SPLL_LOCK_DETECT_DISABLE:
            RCM_DRV_SetResetSource(RCM_RESET_SOURCE_PLL_UNLOCK, false);
            RCM_DRV_SetResetInterruptSource(RCM_RESET_SOURCE_PLL_UNLOCK, false, (rcm_reset_delay_time_t)0U);
            break;

        case CKGEN_SPLL_LOCK_DETECT_INT:
            RCM_DRV_SetResetSource(RCM_RESET_SOURCE_PLL_UNLOCK, false);
            RCM_DRV_SetResetInterruptSource(RCM_RESET_SOURCE_PLL_UNLOCK, true, (rcm_reset_delay_time_t)0U);
            NVIC_EnableIRQ(RCM_IRQn);
            break;

        case CKGEN_SPLL_LOCK_DETECT_RESET:
            RCM_DRV_SetResetSource(RCM_RESET_SOURCE_PLL_UNLOCK, true);
            RCM_DRV_SetResetInterruptSource(RCM_RESET_SOURCE_PLL_UNLOCK, false, (rcm_reset_delay_time_t)0U);
            break;

        default:
            break;
        }

        /* Wait for System PLL to initialize */
        timeout = CKGEN_SPLL_STABILIZATION_TIMEOUT;
        while ((0U == CKGEN_GetSPLLFreq()) && (timeout > 0U))
        {
            timeout--;
        }

        if (0U == timeout)
        {
            status = STATUS_TIMEOUT;
        }
    }

    return status;
}

/*!
 * @brief Configure the system clock to VHSI temporary.
 *
 * @param[in] none
 * @return STATUS_SUCCESS, STATUS_BUSY or STATUS_TIMEOUT
 */
static status_t CKGEN_ConfigureTemporarySystemClock(void)
{
    status_t status = STATUS_SUCCESS;
    ckgen_system_clock_config_t sysClockConfig;
    static const ckgen_system_clock_div_t tmpSysClk[CKGEN_SYS_CLK_SRC_MAX][CKGEN_SYS_DIV_SEL_MAX] = CKGEN_DEFAULT_SYSTEM_CLOCK_CONFIGS;

    /* If the current system clock source is not VHSI:
       1. Enable VHSI (if it's not enabled)
       2. Switch to VHSI. */
    if (CKGEN_GetCurrentSystemClockSource(CKGEN) != ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_VHSI))
    {
        /* If VHSI is not on, then VHSI is configured
           with the default configuration */
        if (0U == CKGEN_GetVHSIFreq())
        {
            status = CKGEN_ConfigureVHSI(true, NULL);
        }

        /* VHSI is enabled, transition the system clock source to VHSI. */
        if (STATUS_SUCCESS == status)
        {
            sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_VHSI;
            sysClockConfig.divCore = tmpSysClk[CKGEN_DEFAULT_VHSI_CLK][CKGEN_DEFAULT_SYS_DIV];
            sysClockConfig.divBus  = tmpSysClk[CKGEN_DEFAULT_VHSI_CLK][CKGEN_DEFAULT_BUS_DIV];
            status = CKGEN_TransitionSystemClock(&sysClockConfig);
        }
    }

    return status;
}

/*!
 * @brief Configures all modules from CKGEN (HSI, VHSI, HSE and SPLL).
 *
 * @param[in] ckgenConfig: CKGEN configuration pointer
 * @return none
 */
static status_t CKGEN_ConfigureModules(const ckgen_config_t *ckgenConfig)
{
    status_t status = STATUS_SUCCESS;
    ckgen_system_clock_config_t sysClockConfig;
    const ckgen_system_clock_config_t *nextSysClockConfig;
    static const ckgen_system_clock_div_t tmpSysClk[CKGEN_SYS_CLK_SRC_MAX][CKGEN_SYS_DIV_SEL_MAX] = CKGEN_DEFAULT_SYSTEM_CLOCK_CONFIGS;

    /* Configure all clock sources that are different from the
       current system clock source VHSI (HSI, HSE, SPLL). */
    status = CKGEN_ConfigureHSI(ckgenConfig->hsiConfig.initialize, &ckgenConfig->hsiConfig);
    if (STATUS_SUCCESS == status)
    {
        status = CKGEN_ConfigureHSE(ckgenConfig->hseConfig.initialize, &ckgenConfig->hseConfig);
        #if CKGEN_AUTO_SEL_HSI
        if ((STATUS_TIMEOUT == status) && (CKGEN_HSI_FREQ == ckgenConfig->hseConfig.freq) && ckgenConfig->hsiConfig.initialize)
        {
            s_autoSelect = 1U;
        }
        if (STATUS_BUSY != status)
        #else
        if (STATUS_SUCCESS == status)
        #endif
        {
            status = CKGEN_ConfigureSPLL(ckgenConfig->spllConfig.initialize, &ckgenConfig->spllConfig);
        }
    }

    /* Get the next system clock source */
    switch (CKGEN_GetCurrentRunMode())
    {
    case CKGEN_SYSTEM_CLOCK_MODE_RUN:
        nextSysClockConfig = &ckgenConfig->clockModeConfig.rccrConfig;
        break;

    case CKGEN_SYSTEM_CLOCK_MODE_VLPR:
        nextSysClockConfig = &ckgenConfig->clockModeConfig.vccrConfig;
        break;

    default:
        DEVICE_ASSERT(false);
        nextSysClockConfig = NULL;
        break;
    }

    if ((STATUS_SUCCESS == status) && (nextSysClockConfig != NULL))
    {
        /* The current system clock source is VHSI.
           Verify whether the next system clock source is VHSI. */
        if (CKGEN_SYSTEM_CLOCK_SRC_VHSI == (nextSysClockConfig->src))
        {
            /* If they are the same, search for a temporary system clock source
               (use one of the following sources: SPLL, HSE, HSI)
               Assume that a temporary clock is not found status = ERROR. */
            status = STATUS_ERROR;

            /* SPLL is enabled */
            if (ckgenConfig->spllConfig.initialize && (STATUS_ERROR == status))
            {
                sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_SPLL;
                sysClockConfig.divCore = tmpSysClk[CKGEN_DEFAULT_SPLL_CLK][CKGEN_DEFAULT_SYS_DIV];
                sysClockConfig.divBus  = tmpSysClk[CKGEN_DEFAULT_SPLL_CLK][CKGEN_DEFAULT_BUS_DIV];
                status = CKGEN_TransitionSystemClock(&sysClockConfig);
            }

            /* HSE is enabled and SPLL configuration for system clock source is not valid */
            if (ckgenConfig->hseConfig.initialize && (STATUS_ERROR == status))
            {
                sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_HSE;
                sysClockConfig.divCore = tmpSysClk[CKGEN_DEFAULT_HSE_CLK][CKGEN_DEFAULT_SYS_DIV];
                sysClockConfig.divBus  = tmpSysClk[CKGEN_DEFAULT_HSE_CLK][CKGEN_DEFAULT_BUS_DIV];
                status = CKGEN_TransitionSystemClock(&sysClockConfig);
            }

            /* HSI is enabled and HSE configuration for system clock source is not valid */
            if (ckgenConfig->hsiConfig.initialize && (STATUS_ERROR == status))
            {
                sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_HSI;
                sysClockConfig.divCore = tmpSysClk[CKGEN_DEFAULT_HSI_CLK][CKGEN_DEFAULT_SYS_DIV];
                sysClockConfig.divBus  = tmpSysClk[CKGEN_DEFAULT_HSI_CLK][CKGEN_DEFAULT_BUS_DIV];
                status = CKGEN_TransitionSystemClock(&sysClockConfig);
            }

            /* Transitioned to a temporary system clock source. */
            if (STATUS_SUCCESS == status)
            {
                /* Configure the remaining clock source (VHSI). */
                status = CKGEN_ConfigureVHSI(ckgenConfig->vhsiConfig.initialize, &ckgenConfig->vhsiConfig);

                if (STATUS_SUCCESS == status)
                {
                    /* Transition to the next system clock source. */
                    sysClockConfig.src     = nextSysClockConfig->src;
                    sysClockConfig.divCore = nextSysClockConfig->divCore;
                    sysClockConfig.divBus  = nextSysClockConfig->divBus;
                    status = CKGEN_TransitionSystemClock(&sysClockConfig);
                }
            }
        }
        else
        {   /* Transition to the next system clock source. */
            sysClockConfig.src     = nextSysClockConfig->src;
            sysClockConfig.divCore = nextSysClockConfig->divCore;
            sysClockConfig.divBus  = nextSysClockConfig->divBus;
            status = CKGEN_TransitionSystemClock(&sysClockConfig);

            if (STATUS_SUCCESS == status)
            {
                /* Configure the remaining clock source (VHSI) */
                status = CKGEN_ConfigureVHSI(ckgenConfig->vhsiConfig.initialize, &ckgenConfig->vhsiConfig);
            }
        }
    }

    return status;
}

/*!
 * @brief Get core or bus clock frequency
 *
 * @param[in] type: Select core or bus clock
 * @return the clock frequency
 */
static uint32_t CKGEN_GetSystemClockFreq(ckgen_system_clock_type_t type)
{
    uint32_t freq;

    DEVICE_ASSERT(type < CKGEN_SYSTEM_CLOCK_MAX);

    switch (CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
    case ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSE):
        freq = CKGEN_GetHSEFreq();
        break;

    case ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSI):
        freq = CKGEN_GetHSIFreq();
        break;

    case ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_VHSI):
        freq = CKGEN_GetVHSIFreq();
        break;

    case ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_SPLL):
        freq = CKGEN_GetSPLLFreq();
        break;

    default:
        freq = 0U;
        break;
    }

    freq /= (CKGEN_GetCurrentSysClockDividerRatio(CKGEN) + 1U);

    if (CKGEN_SYSTEM_CLOCK_BUS == type)
    {
        freq /= (CKGEN_GetCurrentBusClockDividerRatio(CKGEN) + 1U);
    }

    return freq;
}

/*!
 * @brief Get HSE, HSI, VHSI or SPLL clock source frequency.
 *
 * @param[in] src: Select HSE, HSI, VHSI
 * @return Clock source frequency
 */
static uint32_t CKGEN_GetSrcFreq(ckgen_system_clock_src_t src)
{
    uint32_t srcFreq;

    switch (src)
    {
    case CKGEN_SYSTEM_CLOCK_SRC_HSE:
        srcFreq = CKGEN_GetHSEFreq();
        break;

    case CKGEN_SYSTEM_CLOCK_SRC_HSI:
        srcFreq = CKGEN_GetHSIFreq();
        break;

    case CKGEN_SYSTEM_CLOCK_SRC_VHSI:
        srcFreq = CKGEN_GetVHSIFreq();
        break;

    case CKGEN_SYSTEM_CLOCK_SRC_SPLL:
        srcFreq = CKGEN_GetSPLLFreq();
        break;

    default:
        srcFreq = 0U;
        break;
    }

    return srcFreq;
}

/*!
 * @brief Set system clock, bus clock for RUN or VLPR mode.
 *
 * @param[in] mode: Select RUN or VLPR mode
 * @param[in] config: Clock configuration struct
 * @return STATUS_SUCCESS or STATUS_ERROR
 */
static status_t CKGEN_SetSystemClockConfig(ckgen_system_clock_mode_t mode, ckgen_system_clock_config_t const *config)
{
    status_t status = STATUS_SUCCESS;
    uint32_t srcFreq, sysFreq, busFreq;

    /* The maximum clock frequencies of system clocks in all power modes */
    static const uint32_t maxSysClksInRUN[CKGEN_SYS_CLK_SRC_NUM_RUN][SYS_CLK_MAX_NO] = CLOCK_MAX_FREQUENCIES_RUN_MODE;
    static const uint32_t maxSysClksInVLPR[CKGEN_SYS_CLK_SRC_NUM_VLPR][SYS_CLK_MAX_NO] = CLOCK_MAX_FREQUENCIES_VLPR_MODE;

    DEVICE_ASSERT(mode != CKGEN_SYSTEM_CLOCK_MODE_CURRENT);

    srcFreq = CKGEN_GetSrcFreq(config->src);
    sysFreq = srcFreq / (((uint32_t)config->divCore) + 1U);
    busFreq = sysFreq / (((uint32_t)config->divBus) + 1U);

    switch (mode)
    {
    case CKGEN_SYSTEM_CLOCK_MODE_RUN:       /* Run mode. */
        /* Check the frequencies of system and bus clocks. */
        if ((sysFreq > maxSysClksInRUN[(uint32_t)config->src][CORE_CLK_INDEX]) ||  /* Sys(core) clock */
            (busFreq > maxSysClksInRUN[(uint32_t)config->src][BUS_CLK_INDEX]))     /* Bus clock */
        {
            /* Configuration for the next system clock source is not valid. */
            status = STATUS_ERROR;
        }
        else
        {
            /* Disable lock bit field */
            CKGEN_CtrlRegisterLock(false);
            /* Unlock and set flash clock frequency */
            WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY1);
            WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY2);
            MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_CLKFREQ_Msk, FLASH_CNFG_CLKFREQ_Pos, (sysFreq / 1000000U));
            /* Configure system clock */
            CKGEN_SetRunClockControl(CKGEN, (uint32_t)config->src, (uint32_t)config->divCore, (uint32_t)config->divBus);
            /* Enable lock bit field */
            CKGEN_CtrlRegisterLock(true);
            WRITE_REG32(FLASH->KEYUNLK, ~FLASH_UNLOCK_KEY1);
            WRITE_REG32(FLASH->KEYUNLK, ~FLASH_UNLOCK_KEY2);
        }
        break;

    case CKGEN_SYSTEM_CLOCK_MODE_VLPR:      /* Very low power run mode. */
        DEVICE_ASSERT(CKGEN_SYSTEM_CLOCK_SRC_HSI == config->src);
        /* Check the frequencies of system and bus clocks. */
        if ((sysFreq > maxSysClksInVLPR[0][CORE_CLK_INDEX]) ||  /* Sys(core) clock */
            (busFreq > maxSysClksInVLPR[0][BUS_CLK_INDEX]) ||   /* Bus clock */
            (config->src != CKGEN_SYSTEM_CLOCK_SRC_HSI))        /* In VLPR mode, only support HSI as sysytem clock */
        {
            /* Configuration for the next system clock source is not valid. */
            status = STATUS_ERROR;
        }
        else
        {
            /* Disable lock bit field */
            CKGEN_CtrlRegisterLock(false);
            /* Configure system clock */
            CKGEN_SetVLPRClockControl(CKGEN, (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSI, (uint32_t)config->divCore, (uint32_t)config->divBus);
            /* Enable lock bit field */
            CKGEN_CtrlRegisterLock(true);
        }
        break;

    default:
        /* Invalid mode */
        DEVICE_ASSERT(false);
        break;
    }

    return status;
}

/*!
 * @brief Get DIV1 or DIV2 clock frequency from VHSI, HSI, HSE or SPLL
 *
 * @param[in] clockSource: Select VHSI, HSI, HSE or SPLL
 * @param[in] type: Select DIV1 or DIV2
 * @return the clock frequency
 */
static uint32_t CKGEN_GetSysAsyncFreq(clock_names_t clockSource, ckgen_async_clock_type_t type)
{
    uint32_t freq, div = 0U;

    switch (type)
    {
    case CKGEN_ASYNC_CLOCK_DIV1:
    {
        switch (clockSource)
        {
        case VHSI_CLK:
            freq = CKGEN_GetVHSIFreq();
            div = CKGEN_GetVHSIFirstAsyncDivider(CKGEN);
            break;

        case HSI_CLK:
            freq = CKGEN_GetHSIFreq();
            div = CKGEN_GetHSIFirstAsyncDivider(CKGEN);
            break;

        case HSE_CLK:
            freq = CKGEN_GetHSEFreq();
            div = CKGEN_GetHSEFirstAsyncDivider(CKGEN);
            break;

        case SPLL_CLK:
            freq = CKGEN_GetSPLLFreq();
            div = CKGEN_GetSPLLFirstAsyncDivider(CKGEN);
            break;

        default:
            /* Invalid clock source type */
            freq = 0U;
            DEVICE_ASSERT(false);
            break;
        }
    }
        break;

    case CKGEN_ASYNC_CLOCK_DIV2:
    {
        switch (clockSource)
        {
        case VHSI_CLK:
            freq = CKGEN_GetVHSIFreq();
            div = CKGEN_GetVHSISecondAsyncDivider(CKGEN);
            break;

        case HSI_CLK:
            freq = CKGEN_GetHSIFreq();
            div = CKGEN_GetHSISecondAsyncDivider(CKGEN);
            break;

        case HSE_CLK:
            freq = CKGEN_GetHSEFreq();
            div = CKGEN_GetHSESecondAsyncDivider(CKGEN);
            break;

        case SPLL_CLK:
            freq = CKGEN_GetSPLLFreq();
            div = CKGEN_GetSPLLSecondAsyncDivider(CKGEN);
            break;

        default:
            /* Invalid clock source type */
            freq = 0U;
            DEVICE_ASSERT(false);
            break;
        }
    }
        break;

    default:
        /* Invalid async clock source */
        freq = 0U;
        DEVICE_ASSERT(false);
        break;
    }

    freq /= (div + 1U);

    return freq;
}

/*!
 * @brief Get clock out frequecy.
 *
 * @param[in] none
 * @return  clock out frequecy.
 */
static uint32_t CKGEN_GetSimClkOutFreq(void)
{
    uint32_t frequency;

    switch (CKGEN_GetClockoutSourceSel(CKGEN))
    {
    case ((uint32_t)CKGEN_CLKOUT_SEL_HSE_CLK):
        frequency = CKGEN_GetHSEFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_HSI_CLK):
        frequency = CKGEN_GetHSIFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_VHSI_CLK):
        frequency = CKGEN_GetVHSIFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_SPLL_CLK):
        frequency = CKGEN_GetSPLLFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_RTC_CLK):
        frequency = CKGEN_GetRtcClkFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_LSI_CLK):
        frequency = CKGEN_GetLSIFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_LSI_128K_CLK):
        frequency = LSI_128K_FREQUENCY;
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_HSE_DIV2_CLK):
        frequency = CKGEN_GetSysAsyncFreq(HSE_CLK, CKGEN_ASYNC_CLOCK_DIV2);
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_VHSI_DIV2_CLK):
        frequency = CKGEN_GetSysAsyncFreq(VHSI_CLK, CKGEN_ASYNC_CLOCK_DIV2);
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_HSI_DIV2_CLK):
        frequency = CKGEN_GetSysAsyncFreq(HSI_CLK, CKGEN_ASYNC_CLOCK_DIV2);
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_SPLL_DIV2_CLK):
        frequency = CKGEN_GetSysAsyncFreq(SPLL_CLK, CKGEN_ASYNC_CLOCK_DIV2);
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_BUS_CLK):
        frequency = CKGEN_GetSystemClockFreq(CKGEN_SYSTEM_CLOCK_BUS);
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_FLASH_CLK):
        frequency = CKGEN_GetSystemClockFreq(CKGEN_SYSTEM_CLOCK_CORE);
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_SYSTEM_CLK):
        frequency = CKGEN_GetSystemClockFreq(CKGEN_SYSTEM_CLOCK_CORE);
        break;

    default:
        /* Invalid SIM CLKOUT selection.*/
        frequency = 0U;
        break;
    }

    /* Apply Divide Ratio */
    frequency /= (CKGEN_GetClockoutDivider(CKGEN) + 1U);

    return frequency;
}

/*!
 * @brief Get mux1 clock out frequecy.
 *
 * @param[in] none
 * @return clock out frequecy from mux1
 */
static uint32_t CKGEN_GetMux1ClkOutFreq(void)
{
    uint32_t frequency;

    switch (CKGEN_GetClockoutSourceFromMux1(CKGEN))
    {
    case ((uint32_t)CKGEN_CLKOUT_SEL_HSE_CLK):
        frequency = CKGEN_GetHSEFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_HSI_CLK):
        frequency = CKGEN_GetHSIFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_VHSI_CLK):
        frequency = CKGEN_GetVHSIFreq();
        break;

    case ((uint32_t)CKGEN_CLKOUT_SEL_SPLL_CLK):
        frequency = CKGEN_GetSPLLFreq();
        break;

    default:
        /* Invalid CLKOUT selection.*/
        frequency = 0U;
        break;
    }

    return frequency;
}

/*!
 * @brief Get RTC clock source frequency
 *
 * @param[in] none
 * @return RTC clock source frequency
 */
static uint32_t CKGEN_GetRtcClkFreq(void)
{
    uint32_t frequency;

    /* Check RTCCLK Select */
    switch (CKGEN_GetRTCClock(CKGEN))
    {
    case ((uint32_t)RTC_CLK_SEL_HSEDIV1):
        frequency = CKGEN_GetSysAsyncFreq(HSE_CLK, CKGEN_ASYNC_CLOCK_DIV1);
        break;

    case ((uint32_t)RTC_CLK_SEL_VHSIDIV1):
        frequency = CKGEN_GetSysAsyncFreq(VHSI_CLK, CKGEN_ASYNC_CLOCK_DIV1);
        break;

    case ((uint32_t)RTC_CLK_SEL_LSI_32K):
        frequency = LSI_32K_FREQUENCY;
        break;

    case ((uint32_t)RTC_CLK_SEL_CLKIN):
        frequency = g_RtcClkInFreq;
        break;

    default:
        /* Invalid RTCCLK selection.*/
        frequency = 0U;
        break;
    }

    return frequency;
}

/*!
 * @brief Get HSE frequency. If HSE is ready, it will return current frequency.
 *        Otherwise, it will return 0.
 *
 * @param[in] none
 * @return HSE frequency
 */
static uint32_t CKGEN_GetHSEFreq(void)
{
    uint32_t retValue = 0U;

    if (SPM_GetXOSCStatus()) /* XOSC clock is valid. */
    {
        retValue = g_HseClkFreq;
    }

    return retValue;
}

/*!
 * @brief Get HSI frequency. If HSI is ready, it will return current frequency.
 *        Otherwise, it will return 0.
 *
 * @param[in] none
 * @return HSI frequecy
 */
static uint32_t CKGEN_GetHSIFreq(void)
{
    uint32_t retValue = 0U;

    if (SPM_GetHSIStatus()) /* HSI 8M is valid. */
    {
        retValue = CKGEN_HSI_FREQ;
    }

    return retValue;
}

/*!
 * @brief Get VHSI frequency. If VHSI is ready, it will return current frequency.
 *        Otherwise, it will return 0.
 *
 * @param[in] none
 * @return VHSI frequecy
 */
static uint32_t CKGEN_GetVHSIFreq(void)
{
    uint32_t retValue = 0U;

    if (SPM_GetVHSIStatus()) /* VHSI is valid. */
    {
        retValue = CKGEN_VHSI_FREQ;
    }

    return retValue;
}

/*!
 * @brief Get SPLL frequency. If SPLL is ready, it will return current frequency.
 *        Otherwise, it will return 0.
 *
 * @param[in] none
 * @return SPLL frequecy
 */
static uint32_t CKGEN_GetSPLLFreq(void)
{
    uint32_t freq = 0U;
    uint8_t prediv = 0U, fbkdiv = 0U, posdiv = 0U;

    if (SPM_GetSPLLStatus()) /* System PLL is valid. */
    {
        /* Get PLL clock source */
        if (0U == CKGEN_GetSourcSPLL(CKGEN)) /* PLL clock source is HSI */
        {
            freq = CKGEN_GetHSIFreq();
        }
        else  /* PLL clock source is HSE */
        {
            freq = CKGEN_GetHSEFreq();
        }

        if (freq != 0U) /* If source is valid. */
        {
            CKGEN_GetSPLLConfiguration(ANA, &posdiv, &fbkdiv, &prediv);

            freq /= ((uint32_t)prediv + CKGEN_SPLL_PREDIV_BASE);  /* Pre-divider. */
            freq *= fbkdiv;                                       /* Fbk-divider. */
            freq /= ((uint32_t)posdiv * 2U);                      /* Pos-divider. */
        }
    }

    return freq;
}

/*!
 * @brief Get LSI frequency.
 *
 * @param[in] none
 * @return LSI frequecy
 */
static uint32_t CKGEN_GetLSIFreq(void)
{
    uint32_t freq = 0U;

    switch (CKGEN_GetLSIClock(CKGEN))
    {
    case LSI_CLK_SEL_128K:
        freq = LSI_128K_FREQUENCY;
        break;

    case LSI_CLK_SEL_32K:
        freq = LSI_32K_FREQUENCY;
        break;

    case LSI_CLK_SEL_1K:
        freq = LSI_1K_FREQUENCY;
        break;

    case LSI_CLK_SEL_RSV:
        freq = 0U;
        break;

    default:
        /* Invalid LSICLKSEL selection.*/
        DEVICE_ASSERT(false);
        break;
    }

    return freq;
}

/*!
 * @brief Transition to a temporary system clock
 *
 * @param[in] currentSysClkSrc: VHSI, HSE, HSI, SPLL
 * @return STATUS_SUCCESS, STATUS_ERROR or STATUS_TIMEOUT
 */
static status_t CKGEN_TransitionToTmpSysClk(ckgen_system_clock_src_t currentSysClkSrc)
{
    ckgen_system_clock_config_t sysClockConfig;
    static const ckgen_system_clock_div_t tmpSysClk[CKGEN_SYS_CLK_SRC_MAX][CKGEN_SYS_DIV_SEL_MAX] = CKGEN_DEFAULT_SYSTEM_CLOCK_CONFIGS;
    status_t status = STATUS_ERROR;

    /* SPLL is functional, and is not the current system clock source */
    if ((CKGEN_GetSPLLFreq() != 0U) && (currentSysClkSrc != CKGEN_SYSTEM_CLOCK_SRC_SPLL) && (STATUS_ERROR == status))
    {
        sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_SPLL;
        sysClockConfig.divCore = tmpSysClk[CKGEN_DEFAULT_SPLL_CLK][CKGEN_DEFAULT_SYS_DIV];
        sysClockConfig.divBus  = tmpSysClk[CKGEN_DEFAULT_SPLL_CLK][CKGEN_DEFAULT_BUS_DIV];
        status = CKGEN_TransitionSystemClock(&sysClockConfig);
    }

    /* VHSI is functional, it is not the current system clock source, no valid source has been found yet. */
    if ((CKGEN_GetVHSIFreq() != 0U) && (currentSysClkSrc != CKGEN_SYSTEM_CLOCK_SRC_VHSI) && (STATUS_SUCCESS != status))
    {
        sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_VHSI;
        sysClockConfig.divCore = tmpSysClk[CKGEN_DEFAULT_VHSI_CLK][CKGEN_DEFAULT_SYS_DIV];
        sysClockConfig.divBus  = tmpSysClk[CKGEN_DEFAULT_VHSI_CLK][CKGEN_DEFAULT_BUS_DIV];
        status = CKGEN_TransitionSystemClock(&sysClockConfig);
    }

    /* HSE is functional, it is not the current system clock source, no valid source has been found yet. */
    if ((CKGEN_GetHSEFreq() != 0U) && (currentSysClkSrc != CKGEN_SYSTEM_CLOCK_SRC_HSE) && (STATUS_SUCCESS != status))
    {
        sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_HSE;
        sysClockConfig.divCore = tmpSysClk[CKGEN_DEFAULT_HSE_CLK][CKGEN_DEFAULT_SYS_DIV];
        sysClockConfig.divBus  = tmpSysClk[CKGEN_DEFAULT_HSE_CLK][CKGEN_DEFAULT_BUS_DIV];
        status = CKGEN_TransitionSystemClock(&sysClockConfig);
    }

    /* HSI is functional, it is not the current system clock source, no valid source has been found yet. */
    if ((CKGEN_GetHSIFreq() != 0U) && (currentSysClkSrc != CKGEN_SYSTEM_CLOCK_SRC_HSI) && (STATUS_SUCCESS != status))
    {
        sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_HSI;
        sysClockConfig.divCore = tmpSysClk[CKGEN_DEFAULT_HSI_CLK][CKGEN_DEFAULT_SYS_DIV];
        sysClockConfig.divBus  = tmpSysClk[CKGEN_DEFAULT_HSI_CLK][CKGEN_DEFAULT_BUS_DIV];
        status = CKGEN_TransitionSystemClock(&sysClockConfig);
    }

    return status;
}

/*!
 * @brief Get current system clock configuration.
 *
 * @param[out] sysClockConfig: Store curent clock configuration pointer
 * @return none
 */
static void CKGEN_GetCurrentSysClkConfig(ckgen_system_clock_config_t *sysClockConfig)
{
    /* Save the current system clock source configuration */
    switch (CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
    case (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSE:
        sysClockConfig->src = CKGEN_SYSTEM_CLOCK_SRC_HSE;
        break;

    case (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSI:
        sysClockConfig->src = CKGEN_SYSTEM_CLOCK_SRC_HSI;
        break;

    case (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_VHSI:
        sysClockConfig->src = CKGEN_SYSTEM_CLOCK_SRC_VHSI;
        break;

    case (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_SPLL:
        sysClockConfig->src = CKGEN_SYSTEM_CLOCK_SRC_SPLL;
        break;

    default:
        /* Invalid system clock value */
        DEVICE_ASSERT(false);
        break;
    }

    sysClockConfig->divBus  = (ckgen_system_clock_div_t)CKGEN_GetCurrentBusClockDividerRatio(CKGEN);
    sysClockConfig->divCore = (ckgen_system_clock_div_t)CKGEN_GetCurrentSysClockDividerRatio(CKGEN);
}

/*!
 * @brief Configures module clock.
 *
 * @param[in] peripheralClock: Clock name of the configured module clock
 * @param[in] moduleClkConfig: Pointer to the configuration structure
 * @return none
 */
void CKGEN_DRV_SetModuleClock(clock_names_t peripheralClock, const module_clk_config_t *moduleClkConfig)
{
    uint32_t instance;

    DEVICE_ASSERT(moduleClkConfig != NULL);

    if (moduleClkConfig != NULL)
    {
        /* Set peripheral clock in periphral clock mux0 ~ mux2 */
        if (((uint32_t)peripheralClock >= CKGEN_PERI_CLK_BASE) && (peripheralClock < PWM0_CLK))
        {
            /* Set CAN module clock source */
            if ((peripheralClock >= CAN0_CLK) && (peripheralClock < PCT_CLK))
            {
                DEVICE_ASSERT((HSEDIV2_CLK == moduleClkConfig->source)      \
                               || (CORE_CLK == moduleClkConfig->source)     \
                               || (SPLLDIV2_CLK == moduleClkConfig->source) \
                               || (PERI_CLK_OFF == moduleClkConfig->source));

                instance = (uint32_t)((uint32_t)peripheralClock - (uint32_t)(CAN0_CLK));
                CKGEN_SetCANClock((uint8_t)instance, moduleClkConfig->source);
                CKGEN_SetCANClockDivider((uint8_t)instance, (can_clock_divider_t)(moduleClkConfig->div - 1U));
            }
            else /* Set other module clock source in clock mux0~2 */
            {
                DEVICE_ASSERT((HSEDIV2_CLK == moduleClkConfig->source)      \
                               || (HSIDIV2_CLK == moduleClkConfig->source)  \
                               || (VHSIDIV2_CLK == moduleClkConfig->source) \
                               || (SPLLDIV2_CLK == moduleClkConfig->source) \
                               || (PERI_CLK_OFF == moduleClkConfig->source));

                CKGEN_SetPeripheralClock(peripheralClock, moduleClkConfig->source);
            }
        }
        else if ((peripheralClock >= PWM0_CLK) && (peripheralClock < PCC_END_OF_CLOCKS)) /* Set PWM clock source */
        {
            DEVICE_ASSERT(((moduleClkConfig->source >= HSEDIV1_CLK) && (moduleClkConfig->source < SPLLDIV1_CLK)) \
                          || ((moduleClkConfig->source >= TCLK0_CLK) && (moduleClkConfig->source <= TCLK3_CLK)));

            instance = (uint32_t)((uint32_t)peripheralClock - (uint32_t)(PWM0_CLK));
            CKGEN_SetPWMClock(instance, moduleClkConfig->source);
        }
        else
        {
            /* module is not valid */
        }
    }
    else
    {
        /* moduleClkConfig is NULL */
    }
}

/*!
 * @brief Gets system clock mode.
 *
 * @param[in] mode: RUN or VLPR
 * @return CKGEN_SYSTEM_CLOCK_MODE_RUN, CKGEN_SYSTEM_CLOCK_MODE_VLPR or CKGEN_SYSTEM_CLOCK_MODE_NONE
 */
static ckgen_system_clock_mode_t CKGEN_GetSysClockMode(const pwr_modes_t mode)
{
    ckgen_system_clock_mode_t sysClockMode;

    switch (mode)
    {
    /* Run mode */
    case RUN_MODE:
        sysClockMode = CKGEN_SYSTEM_CLOCK_MODE_RUN;
        break;

    /* Very low power run mode */
    case VLPR_MODE:
        sysClockMode = CKGEN_SYSTEM_CLOCK_MODE_VLPR;
        break;

    /* This should never happen - input power mode is invalid */
    default:
        sysClockMode = CKGEN_SYSTEM_CLOCK_MODE_NONE;
        DEVICE_ASSERT(false);
        break;
    }

    return sysClockMode;
}

/*!
 * @brief Gets system clock source.
 *
 * @param[in] src: HSI_CLK, VHSI_CLK, HSE_CLK or SPLL_CLK
 * @return CKGEN_SYSTEM_CLOCK_SRC_HSI, CKGEN_SYSTEM_CLOCK_SRC_VHSI, CKGEN_SYSTEM_CLOCK_SRC_HSE or CKGEN_SYSTEM_CLOCK_SRC_NONE
 */
static ckgen_system_clock_src_t CKGEN_GetSysClockSource(clock_names_t src)
{
    ckgen_system_clock_src_t source;

    switch (src)
    {
        case HSI_CLK:
            source = CKGEN_SYSTEM_CLOCK_SRC_HSI;
            break;

        case VHSI_CLK:
            source = CKGEN_SYSTEM_CLOCK_SRC_VHSI;
            break;

        case HSE_CLK:
            source = CKGEN_SYSTEM_CLOCK_SRC_HSE;
            break;

        case SPLL_CLK:
            source = CKGEN_SYSTEM_CLOCK_SRC_SPLL;
            break;

        default:
            source = CKGEN_SYSTEM_CLOCK_SRC_NONE;
            DEVICE_ASSERT(false);
            break;
    }

    return source;
}

/*!
 * @brief Configures the system clocks.
 *        This function configures the system clocks (core, bus and flash clocks) in the specified power mode.
 *        If no power mode is specified (null parameter) then it is the current power mode.
 *
 * @param[in] mode:         Pointer to power mode for which the configured system clocks apply
 * @param[in] sysClkConfig: Pointer to the system clocks configuration structure
 * @return STATUS_SUCCESS, STATUS_ERROR or STATUS_TIMEOUT
 */
status_t CKGEN_DRV_SetSystemClock(const pwr_modes_t *mode, const sys_clk_config_t *sysClkConfig)
{
    status_t retCode = STATUS_SUCCESS;
    ckgen_system_clock_mode_t sysClockMode, currentSysClockMode = CKGEN_GetCurrentRunMode();
    static const ckgen_system_clock_div_t defSysClk[CKGEN_SYS_CLK_SRC_MAX][CKGEN_SYS_DIV_SEL_MAX] = CKGEN_DEFAULT_SYSTEM_CLOCK_CONFIGS;

    ckgen_system_clock_config_t sysClockConfig = {
        .src     = CKGEN_SYSTEM_CLOCK_SRC_HSI,
        .divCore = defSysClk[CKGEN_DEFAULT_HSI_CLK][CKGEN_DEFAULT_SYS_DIV],
        .divBus  = defSysClk[CKGEN_DEFAULT_HSI_CLK][CKGEN_DEFAULT_BUS_DIV],
    };

    /* The power mode is not provided, the current one will be used by default. */
    if (NULL == mode)
    {
        sysClockMode = currentSysClockMode;
    }
    else
    {
        sysClockMode = CKGEN_GetSysClockMode(*mode);
    }

    /* System clock configuration is not provided, a default one will be used. */
    if (NULL == sysClkConfig)
    {
        /* Find a valid clock source. */
        if (CKGEN_GetVHSIFreq() != 0U)
        {
            sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_VHSI;
            sysClockConfig.divCore = defSysClk[CKGEN_DEFAULT_VHSI_CLK][CKGEN_DEFAULT_SYS_DIV];
            sysClockConfig.divBus  = defSysClk[CKGEN_DEFAULT_VHSI_CLK][CKGEN_DEFAULT_BUS_DIV];
        }
        else if (CKGEN_GetSPLLFreq() != 0U)
        {
            sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_SPLL;
            sysClockConfig.divCore = defSysClk[CKGEN_DEFAULT_SPLL_CLK][CKGEN_DEFAULT_SYS_DIV];
            sysClockConfig.divBus  = defSysClk[CKGEN_DEFAULT_SPLL_CLK][CKGEN_DEFAULT_BUS_DIV];
        }
        else if (CKGEN_GetHSEFreq() != 0U)
        {
            sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_HSE;
            sysClockConfig.divCore = defSysClk[CKGEN_DEFAULT_HSE_CLK][CKGEN_DEFAULT_SYS_DIV];
            sysClockConfig.divBus  = defSysClk[CKGEN_DEFAULT_HSE_CLK][CKGEN_DEFAULT_BUS_DIV];
        }
        else if (CKGEN_GetHSIFreq() != 0U)
        {
            sysClockConfig.src     = CKGEN_SYSTEM_CLOCK_SRC_HSI;
            if (CKGEN_SYSTEM_CLOCK_MODE_VLPR == sysClockMode)
            {
                sysClockConfig.divCore = CKGEN_VLPR_SYS_CLK;
                sysClockConfig.divBus  = CKGEN_VLPR_BUS_CLK;
            }
            else
            {
                sysClockConfig.divCore = defSysClk[CKGEN_DEFAULT_HSI_CLK][CKGEN_DEFAULT_SYS_DIV];
                sysClockConfig.divBus  = defSysClk[CKGEN_DEFAULT_HSI_CLK][CKGEN_DEFAULT_BUS_DIV];
            }
        }
        else
        {
            DEVICE_ASSERT(false);
        }
    }
    else
    {
        /* The system clock source from input configuration structure must be valid. */
        DEVICE_ASSERT((CKGEN_GetHSIFreq() != 0U) || (sysClkConfig->src != HSI_CLK));
        DEVICE_ASSERT((CKGEN_GetVHSIFreq() != 0U) || (sysClkConfig->src != VHSI_CLK));
        DEVICE_ASSERT((CKGEN_GetHSEFreq() != 0U) || (sysClkConfig->src != HSE_CLK));
        DEVICE_ASSERT((CKGEN_GetSPLLFreq() != 0U) || (sysClkConfig->src != SPLL_CLK));

        sysClockConfig.src = CKGEN_GetSysClockSource(sysClkConfig->src);

        sysClockConfig.divCore = (ckgen_system_clock_div_t)(sysClkConfig->dividers[0] - 1U);
        sysClockConfig.divBus  = (ckgen_system_clock_div_t)(sysClkConfig->dividers[1] - 1U);
    }

    /* System clock is configured in the current mode,
     * set configuration and wait until the system clock is changed. */
    if (sysClockMode == currentSysClockMode)
    {
        retCode = CKGEN_TransitionSystemClock(&sysClockConfig);
    }
    /* System clock is not configured in the current mode, just set configuration. */
    else
    {
        retCode = CKGEN_SetSystemClockConfig(sysClockMode, &sysClockConfig);
    }

    return retCode;
}

/*!
 * @brief Get system clock source.
 *
 * @param[out] sysClkConfig: Store system clock configuration.
 * @return none
 */
void CKGEN_DRV_GetSystemClockSource(sys_clk_config_t *sysClkConfig)
{
    switch (CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
    case (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSE:
        sysClkConfig->src = HSE_CLK;
        break;

    case (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSI:
        sysClkConfig->src = HSI_CLK;
        break;

    case (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_VHSI:
        sysClkConfig->src = VHSI_CLK;
        break;

    case (uint32_t)CKGEN_SYSTEM_CLOCK_SRC_SPLL:
        sysClkConfig->src = SPLL_CLK;
        break;

    default:
        /* Invalid system clock source. */
        DEVICE_ASSERT(false);
        sysClkConfig->src = HSI_CLK;
        break;
    }

    /* Core divider */
    sysClkConfig->dividers[0U] = (uint8_t)((CKGEN_GetCurrentSysClockDividerRatio(CKGEN)) + 1U);

    /* Bus divider */
    sysClkConfig->dividers[1U] = (uint8_t)((CKGEN_GetCurrentBusClockDividerRatio(CKGEN)) + 1U);
}

/*!
 * @brief Sets HSI clock.
 *
 * @param[in] clockSourceEnable: Set clock source or not
 * @param[in] clkSrcConfig: Clock source configuration
 * @param[in] divider1: Set divider1
 * @param[in] divider2: Set divider2
 *
 * @return STATUS_SUCCESS, STATUS_ERROR, STATUS_BUSY or STATUS_TIMEOUT
 */
static status_t CKGEN_SetHSIClockSource(bool clockSourceEnable,
                                        const clock_source_config_t *clkSrcConfig,
                                        ckgen_async_clock_div_t divider1,
                                        ckgen_async_clock_div_t divider2)
{
    status_t retCode = STATUS_SUCCESS;
    ckgen_system_clock_config_t currentSysClockConfig = {
        .src     = CKGEN_SYSTEM_CLOCK_SRC_HSI,
        .divCore = CKGEN_SYSTEM_CLOCK_DIV_BY_1,
        .divBus  = CKGEN_SYSTEM_CLOCK_DIV_BY_2,
    };
    bool revertTmpSysClkTransition = false;
    ckgen_hsi_config_t hsiConfig;

    /* Check whether the current system clock source is HSI */
    if (((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSI) == CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
        if (false == clockSourceEnable)
        {
            /* Can't disable HSI, it's used as system clock source */
            retCode = STATUS_ERROR;
        }
        else
        {
            /* Save the current system clock source configuration */
            CKGEN_GetCurrentSysClkConfig(&currentSysClockConfig);

            /* Transition to a temporary system clock source */
            retCode = CKGEN_TransitionToTmpSysClk(CKGEN_SYSTEM_CLOCK_SRC_HSI);

            revertTmpSysClkTransition = (retCode == STATUS_SUCCESS) ? true : false;
        }
    }

    if (STATUS_SUCCESS == retCode)
    {
        if (NULL == clkSrcConfig)
        {
            retCode = CKGEN_ConfigureHSI(true, NULL);
        }
        else
        {
            /* Check VLP modes */
            hsiConfig.div1 = divider1;
            hsiConfig.div2 = divider2;

            retCode = CKGEN_ConfigureHSI(clockSourceEnable, &hsiConfig);
        }

        /* If system clock source was HSI and HSI has been set successfully,
           then transition back to HSI */
        if ((STATUS_SUCCESS == retCode) && (revertTmpSysClkTransition))
        {
            retCode = CKGEN_TransitionSystemClock(&currentSysClockConfig);
        }
    }

    return retCode;
}

/*!
 * @brief Sets VHSI clock.
 *
 * @param[in] clockSourceEnable: Set clock source or not
 * @param[in] clkSrcConfig: Clock source configuration
 * @param[in] divider1: Set divider1
 * @param[in] divider2: Set divider2
 *
 * @return STATUS_SUCCESS, STATUS_ERROR, STATUS_BUSY or STATUS_TIMEOUT
 */
static status_t CKGEN_SetVHSIClockSource(bool clockSourceEnable,
                                         const clock_source_config_t *clkSrcConfig,
                                         ckgen_async_clock_div_t divider1,
                                         ckgen_async_clock_div_t divider2)
{
    status_t retCode = STATUS_SUCCESS;
    ckgen_system_clock_config_t currentSysClockConfig = {
        .src     = CKGEN_SYSTEM_CLOCK_SRC_HSI,
        .divCore = CKGEN_SYSTEM_CLOCK_DIV_BY_1,
        .divBus  = CKGEN_SYSTEM_CLOCK_DIV_BY_2,
    };
    bool revertTmpSysClkTransition = false;
    ckgen_vhsi_config_t vhsiConfig;

    /* Check whether the current system clock source is VHSI */
    if (((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_VHSI) == CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
        if (false == clockSourceEnable)
        {
            /* Can't disable VHSI, it's used as system clock source. */
            retCode = STATUS_ERROR;
        }
        else
        {
            /* Save the current system clock source configuration. */
            CKGEN_GetCurrentSysClkConfig(&currentSysClockConfig);

            /* Transition to a temporary system clock source. */
            retCode = CKGEN_TransitionToTmpSysClk(CKGEN_SYSTEM_CLOCK_SRC_VHSI);

            revertTmpSysClkTransition = (STATUS_SUCCESS == retCode) ? true : false;
        }
    }

    if (STATUS_SUCCESS == retCode)
    {
        if (NULL == clkSrcConfig)
        {
            retCode = CKGEN_ConfigureVHSI(clockSourceEnable, NULL);
        }
        else
        {
            vhsiConfig.div1 = divider1;
            vhsiConfig.div2 = divider2;
            retCode = CKGEN_ConfigureVHSI(clockSourceEnable, &vhsiConfig);
        }

        /* If system clock source was VHSI and VHSI has been set successfully,
           then transition back to VHSI */
        if ((STATUS_SUCCESS == retCode) && (revertTmpSysClkTransition))
        {
            retCode = CKGEN_TransitionSystemClock(&currentSysClockConfig);
        }
    }

    return retCode;
}

/*!
 * @brief Gets external reference value for HSE clock.
 *
 * @param[in] refClk: HSE_EXT_REF or HSE_INT_OSC
 * @return CKGEN_HSE_REF_EXT or CKGEN_HSE_REF_OSC
 */
static ckgen_hse_ext_ref_t CKGEN_GetExtRef(hse_ref_t refClk)
{
    ckgen_hse_ext_ref_t extRef = CKGEN_HSE_REF_EXT;

    switch (refClk)
    {
    case HSE_EXT_REF:
        extRef = CKGEN_HSE_REF_EXT;
        break;

    case HSE_INT_OSC:
        extRef = CKGEN_HSE_REF_OSC;
        break;

    default:
        /* Invalid input reference value */
        DEVICE_ASSERT(false);
        break;
    }

    return extRef;
}

/*!
 * @brief Sets HSE clock.
 *
 * @param[in] clockSourceEnable: Set clock source or not
 * @param[in] clkSrcConfig: Clock source configuration
 * @param[in] divider1: Set divider1
 * @param[in] divider2: Set divider2
 *
 * @return STATUS_SUCCESS, STATUS_ERROR, STATUS_BUSY or STATUS_TIMEOUT
 */
static status_t CKGEN_SetHSEClockSource(bool clockSourceEnable,
                                        const clock_source_config_t *clkSrcConfig,
                                        ckgen_async_clock_div_t divider1,
                                        ckgen_async_clock_div_t divider2)
{
    status_t retCode = STATUS_SUCCESS;
    ckgen_system_clock_config_t currentSysClockConfig = {
        .src     = CKGEN_SYSTEM_CLOCK_SRC_HSI,
        .divCore = CKGEN_SYSTEM_CLOCK_DIV_BY_1,
        .divBus  = CKGEN_SYSTEM_CLOCK_DIV_BY_2,
    };
    bool revertTmpSysClkTransition = false;
    ckgen_hse_config_t hseConfig;

    /* Check whether the current system clock source is HSE */
    if (((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_HSE) == CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
        if (false == clockSourceEnable)
        {
            /* Can't disable HSE, it's used as system clock source. */
            retCode = STATUS_ERROR;
        }
        else
        {
            /* Save the current system clock source configuration. */
            CKGEN_GetCurrentSysClkConfig(&currentSysClockConfig);

            /* Transition to a temporary system clock source. */
            retCode = CKGEN_TransitionToTmpSysClk(CKGEN_SYSTEM_CLOCK_SRC_HSE);

            revertTmpSysClkTransition = (STATUS_SUCCESS == retCode) ? true : false;
        }
    }

    if (STATUS_SUCCESS == retCode)
    {
        if (NULL == clkSrcConfig)
        {
            retCode = CKGEN_ConfigureHSE(clockSourceEnable, NULL);
        }
        else
        {
            hseConfig.freq   = clkSrcConfig->refFreq;
            hseConfig.div1   = divider1;
            hseConfig.div2   = divider2;
            hseConfig.extRef = CKGEN_GetExtRef(clkSrcConfig->refClk);

            retCode = CKGEN_ConfigureHSE(clockSourceEnable, &hseConfig);
        }

        /* If system clock source was HSE and HSE has been set successfully,
         * then transition back to HSE */
        if ((STATUS_SUCCESS == retCode) && (revertTmpSysClkTransition))
        {
            retCode = CKGEN_TransitionSystemClock(&currentSysClockConfig);
        }
    }

    return retCode;
}

/*!
 * @brief Sets SPLL clock.
 *
 * @param[in] clockSourceEnable: Set clock source or not
 * @param[in] clkSrcConfig: Clock source configuration
 * @param[in] divider1: Set divider1
 * @param[in] divider2: Set divider2
 *
 * @return STATUS_SUCCESS, STATUS_ERROR, STATUS_BUSY or STATUS_TIMEOUT
 */
static status_t CKGEN_SetSPLLClockSource(bool clockSourceEnable,
                                         const clock_source_config_t *clkSrcConfig,
                                         ckgen_async_clock_div_t divider1,
                                         ckgen_async_clock_div_t divider2)
{
    status_t retCode = STATUS_SUCCESS;
    ckgen_system_clock_config_t currentSysClockConfig = {
        .src     = CKGEN_SYSTEM_CLOCK_SRC_HSI,
        .divCore = CKGEN_SYSTEM_CLOCK_DIV_BY_1,
        .divBus  = CKGEN_SYSTEM_CLOCK_DIV_BY_2,
    };
    bool revertTmpSysClkTransition = false;
    ckgen_spll_config_t spllConfig;

    /* Check whether the current system clock source is PLL */
    if (((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_SPLL) == CKGEN_GetCurrentSystemClockSource(CKGEN))
    {
        if (false == clockSourceEnable)
        {
            /* Can't disable PLL, it's used as system clock source. */
            retCode = STATUS_ERROR;
        }
        else
        {
            /* Save the current system clock source configuration. */
            CKGEN_GetCurrentSysClkConfig(&currentSysClockConfig);

            /* Transition to a temporary system clock source. */
            retCode = CKGEN_TransitionToTmpSysClk(CKGEN_SYSTEM_CLOCK_SRC_SPLL);

            revertTmpSysClkTransition = (STATUS_SUCCESS == retCode) ? true : false;
        }
    }

    if (STATUS_SUCCESS == retCode)
    {
        if (NULL == clkSrcConfig)
        {
            retCode = CKGEN_ConfigureSPLL(clockSourceEnable, NULL);
        }
        else
        {
            spllConfig.div1 = divider1;
            spllConfig.div2 = divider2;

            DEVICE_ASSERT(clkSrcConfig->prediv <= CKGEN_SPLL_PREDIV_MAX);
            spllConfig.prediv = (uint8_t)(clkSrcConfig->prediv);

            DEVICE_ASSERT((CKGEN_SPLL_FBKDIV_MIN <= clkSrcConfig->fbkdiv) && (clkSrcConfig->fbkdiv <= CKGEN_SPLL_FBKDIV_MAX));
            spllConfig.fbkdiv = (uint8_t)(clkSrcConfig->fbkdiv);

            DEVICE_ASSERT((CKGEN_SPLL_POSDIV_MIN <= clkSrcConfig->posdiv) && (clkSrcConfig->posdiv <= CKGEN_SPLL_POSDIV_MAX));
            spllConfig.posdiv = (uint8_t)(clkSrcConfig->posdiv);

            /* Set pll clock source */
            spllConfig.src = clkSrcConfig->src;

            retCode = CKGEN_ConfigureSPLL(clockSourceEnable, &spllConfig);
        }

        /* If system clock source was PLL and PLL has been set successfully,
           then transition back to PLL */
        if ((STATUS_SUCCESS == retCode) && revertTmpSysClkTransition)
        {
            retCode = CKGEN_TransitionSystemClock(&currentSysClockConfig);
        }
    }

    return retCode;
}

/*!
 * @brief This function configures a clock source.
 *        The clock source is configured based on the provided configuration.
 *        All values from the previous configuration of clock source are overwritten.
 *        If no configuration is provided, then a default one is used.
 *
 * @param[in] clockSource: Clock name of the configured clock source
 *                         (HSI_CLK, VHSI_CLK, HSE_CLK, SPLL_CLK, LSI_CLK)
 * @param[in] clkSrcConfig: Pointer to the configuration structure
 * @return Status of clock source initialization
 */
status_t CKGEN_DRV_SetClockSource(clock_names_t clockSource, const clock_source_config_t *clkSrcConfig)
{
    status_t retCode = STATUS_SUCCESS;
    bool clockSourceEnable = true;

    ckgen_async_clock_div_t divider1, divider2;

    if (NULL == clkSrcConfig)
    {
        divider1 = CKGEN_ASYNC_CLOCK_DIV_BY_1;
        divider2 = CKGEN_ASYNC_CLOCK_DIV_BY_1;
    }
    else
    {
        divider1 = (ckgen_async_clock_div_t)(clkSrcConfig->outputDiv1 - 1U);
        divider2 = (ckgen_async_clock_div_t)(clkSrcConfig->outputDiv2 - 1U);
    }

    /* Check whether the command enables and disables the clock source */
    if ((clkSrcConfig != NULL) && (false == clkSrcConfig->enable))
    {
        clockSourceEnable = false;
    }

    switch (clockSource)
    {
    case HSI_CLK:
        retCode = CKGEN_SetHSIClockSource(clockSourceEnable, clkSrcConfig, divider1, divider2);
        break;

    case VHSI_CLK:
        retCode = CKGEN_SetVHSIClockSource(clockSourceEnable, clkSrcConfig, divider1, divider2);
        break;

    case HSE_CLK:
        retCode = CKGEN_SetHSEClockSource(clockSourceEnable, clkSrcConfig, divider1, divider2);
        break;

    case SPLL_CLK:
        retCode = CKGEN_SetSPLLClockSource(clockSourceEnable, clkSrcConfig, divider1, divider2);
        break;

    case LSI_CLK:
        CKGEN_SetRTCLSIClock(RTC_CLK_SEL_HSEDIV1, LSI_CLK_SEL_128K);
        break;

    default:
        /* Invalid name of the clock source */
        DEVICE_ASSERT(false);
        break;
    }

    return retCode;
}

/*!
 * @brief Install pre-defined clock configurations.
 *        This function installs the pre-defined clock configuration table to the
 *        clock manager.
 *
 * @param[in] clockConfigsPtr: Pointer to the clock configuration table.
 * @param[in] configsNumber: Number of clock configurations in table.
 * @param[in] callbacksPtr: Pointer to the callback configuration table.
 * @param[in] callbacksNumber: Number of callback configurations in table.
 * @return Error code.
 */
status_t CKGEN_SYS_Init(clock_manager_user_config_t const **clockConfigsPtr,
                        uint8_t configsNumber,
                        clock_manager_callback_user_config_t **callbacksPtr,
                        uint8_t callbacksNumber)
{
    DEVICE_ASSERT(clockConfigsPtr != NULL);
    DEVICE_ASSERT(callbacksPtr != NULL);

    s_clockState.configTable     = clockConfigsPtr;
    s_clockState.clockConfigNum  = configsNumber;
    s_clockState.callbackConfig  = callbacksPtr;
    s_clockState.callbackNum     = callbacksNumber;

    /*
     * errorCallbackIndex is the index of the callback which returns error
     * during clock mode switch. If all callbacks return success, then the
     * errorCallbackIndex is callbacksNumber.
     */
    s_clockState.errorCallbackIndex = callbacksNumber;

    return STATUS_SUCCESS;
}

/*!
 * @brief Send notification and change system clock configuration.
 *        This function sets system to target clock configuration; before transition,
 *        clock manager will send notifications to all drivers registered to the
 *        callback table. When graceful policy is used, if some drivers are not ready
 *        to change, clock transition will not occur, all drivers still work in
 *        previous configuration and error is returned. When forceful policy is used,
 *        all drivers should stop work and system changes to new clock configuration.
 *        The function should be called only on run mode.
 *
 * @param[in] targetConfigIndex: Index of the clock configuration
 * @param[in] policy: Transaction policy, graceful or forceful
 * @return Error code
 */
status_t CKGEN_DRV_UpdateConfiguration(uint8_t targetConfigIndex, clock_manager_policy_t policy)
{
    uint8_t callbackIdx;
    bool successfulSetConfig;           /* SetConfiguraiton status */
    status_t ret = STATUS_SUCCESS;
    const clock_manager_callback_user_config_t *callbackConfig;
    clock_notify_struct_t notifyStruct;

    DEVICE_ASSERT(targetConfigIndex < s_clockState.clockConfigNum);       /* Clock configuration index is out of range. */

    notifyStruct.targetClockConfigIndex = targetConfigIndex;
    notifyStruct.policy                 = policy;

    /* Disable interrupts */
    DisableInterrupts;
    /* Set errorcallbackindex as callbackNum, which means no callback error now.*/
    s_clockState.errorCallbackIndex = s_clockState.callbackNum;

    /* First step: Send "BEFORE" notification. */
    notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_BEFORE;

    /* Send notification to all callback. */
    for (callbackIdx = 0U; callbackIdx < s_clockState.callbackNum; callbackIdx++)
    {
        callbackConfig = s_clockState.callbackConfig[callbackIdx];
        if ((callbackConfig != NULL) && (callbackConfig->callbackType != CLOCK_MANAGER_CALLBACK_AFTER))
        {
            if (STATUS_SUCCESS != (*callbackConfig->callback)(&notifyStruct, callbackConfig->callbackData))
            {
                s_clockState.errorCallbackIndex = callbackIdx;

                if (CLOCK_MANAGER_POLICY_AGREEMENT == policy)
                {
                    /* Save the error callback index. */
                    ret = STATUS_MCU_NOTIFY_BEFORE_ERROR;
                    break;
                }
            }
        }
    }

    /* If all callback success or forceful policy is used. */
    if ((STATUS_SUCCESS == ret) || (CLOCK_MANAGER_POLICY_FORCIBLE == policy))
    {
        /* clock mode switch. */
        ret = CKGEN_DRV_SetConfiguration(s_clockState.configTable[targetConfigIndex]);
        successfulSetConfig = (STATUS_SUCCESS == ret);

        s_clockState.curConfigIndex = targetConfigIndex;
    }
    else
    {
        /* Unsuccessful setConfiguration */
        successfulSetConfig = false;
    }

    if (successfulSetConfig)
    {
        notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_AFTER;

        for (callbackIdx = 0U; callbackIdx < s_clockState.callbackNum; callbackIdx++)
        {
            callbackConfig = s_clockState.callbackConfig[callbackIdx];
            if ((callbackConfig != NULL) && (callbackConfig->callbackType != CLOCK_MANAGER_CALLBACK_BEFORE))
            {
                if (STATUS_SUCCESS != (*callbackConfig->callback)(&notifyStruct, callbackConfig->callbackData))
                {
                    s_clockState.errorCallbackIndex = callbackIdx;

                    if (CLOCK_MANAGER_POLICY_AGREEMENT == policy)
                    {
                        /* Save the error callback index. */
                        ret = STATUS_MCU_NOTIFY_AFTER_ERROR;
                        break;
                    }
                }
            }
        }
    }
    else /* Error occurs, need to send "RECOVER" notification. */
    {
        notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_RECOVER;
        for (;;)
        {
            callbackConfig = s_clockState.callbackConfig[callbackIdx];
            if (callbackConfig != NULL)
            {
                (void)(*callbackConfig->callback)(&notifyStruct,
                        callbackConfig->callbackData);
            }
            if (0U == callbackIdx)
            {
                break;
            }
            callbackIdx--;
        }
    }

    /* Enable interrupts */
    EnableInterrupts;

    return ret;
}

/*!
 * @brief Get current clock configuration index.
 *
 * @param[in] none
 * @return Current clock configuration index
 */
uint8_t CKGEN_DRV_GetCurrentConfiguration(void)
{
    return s_clockState.curConfigIndex;
}

/*!
 * @brief Get the callback which returns error in last clock switch.
 *        When graceful policy is used, if some IP is not ready to change clock
 *        setting, the callback will return error and system stay in current
 *        configuration. Applications can use this function to check which
 *        IP callback returns error.
 *
 * @param[in] none
 * @return Pointer to the callback which returns error.
 */
clock_manager_callback_user_config_t *CKGEN_DRV_GetErrorCallback(void)
{
    clock_manager_callback_user_config_t *retValue;

    /* If all callbacks return success. */
    if (s_clockState.errorCallbackIndex >= s_clockState.clockConfigNum)
    {
        retValue = NULL;
    }
    else
    {
        retValue = s_clockState.callbackConfig[s_clockState.errorCallbackIndex];
    }

    return retValue;
}

#if CKGEN_AUTO_CHANGE_CLK
/*!
 * @brief RCM interrupt handler.
 *
 * @param[in] none
 * @return none
 */
void RCM_IRQHandler(void)
{
    /* Check reset interrupt flag */
    uint32_t resetIrqStatus = RCM_GetResetStatus();
    /* Clear reset int status */
    RCM_ClearResetInterruptStatus();

    /* Detect XOSC loss, then change the reference clock of SPLL to HSI */
    if (resetIrqStatus & CKGEN_RCM_STATUS_XOSC_LOSS_RST_INT_FLAG_Msk)
    {
        if (((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_SPLL == CKGEN_GetCurrentSystemClockSource(CKGEN)) && \
            ((uint8_t)CKGEN_SPLL_REF_HSE == CKGEN_GetSourcSPLL(CKGEN)))
        {
            CKGEN_CtrlRegisterLock(false);
            CKGEN_SetSourceSPLL(CKGEN, (uint8_t)CKGEN_SPLL_REF_HSI);
        }
    }
    /* Detect SPLL unlock, then change the system clock to VHSI */
    if (resetIrqStatus & CKGEN_RCM_STATUS_PLL_UNLOCK_RST_INT_FLAG_Msk)
    {
        if ((uint32_t)CKGEN_SYSTEM_CLOCK_SRC_SPLL == CKGEN_GetCurrentSystemClockSource(CKGEN))
        {
            CKGEN_CtrlRegisterLock(false);
            CKGEN_SetRunSysClkSrc(CKGEN, (uint8_t)CKGEN_SYSTEM_CLOCK_SRC_VHSI);
        }
    }
}
#endif

/* =============================================  EOF  ============================================== */
