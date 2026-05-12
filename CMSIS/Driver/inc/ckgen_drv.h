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
 * @file ckgen_drv.h
 *
 * @brief This file provides ckgen integration functions interface.
 *
 */

#ifndef _CKGEN_DRV_H
#define _CKGEN_DRV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
/*!< The maximum number of system clock dividers and system clock divider indexes. */
#define SYS_CLK_MAX_NO           (2UL)
#define CORE_CLK_INDEX           (0UL)
#define BUS_CLK_INDEX            (1UL)

#define CKGEN_DEFAULT_HSI_CLK    (0UL)
#define CKGEN_DEFAULT_VHSI_CLK   (1UL)
#define CKGEN_DEFAULT_HSE_CLK    (2UL)
#define CKGEN_DEFAULT_SPLL_CLK   (3UL)

/*!< system clock configuration in VLPR mode */
#define CKGEN_VLPR_SYS_CLK       (CKGEN_SYSTEM_CLOCK_DIV_BY_1)
#define CKGEN_VLPR_BUS_CLK       (CKGEN_SYSTEM_CLOCK_DIV_BY_2)

#define CKGEN_DEFAULT_SYS_DIV    (0UL)
#define CKGEN_DEFAULT_BUS_DIV    (1UL)

#define CKGEN_SYS_CLK_SRC_MAX    (4UL)
#define CKGEN_SYS_DIV_SEL_MAX    (2UL)

/*!< Default system clock configuration when HSI/VHSI/HSE/SPLL as system clock source */
#define CKGEN_DEFAULT_SYSTEM_CLOCK_CONFIGS                                           \
{  /*       SYS_CLK                    BUS_CLK                */                     \
{  CKGEN_SYSTEM_CLOCK_DIV_BY_1, CKGEN_SYSTEM_CLOCK_DIV_BY_1},  /*!< Dividers for HSI */  \
{  CKGEN_SYSTEM_CLOCK_DIV_BY_1, CKGEN_SYSTEM_CLOCK_DIV_BY_2},  /*!< Dividers for VHSI */ \
{  CKGEN_SYSTEM_CLOCK_DIV_BY_1, CKGEN_SYSTEM_CLOCK_DIV_BY_2},  /*!< Dividers for HSE */  \
{  CKGEN_SYSTEM_CLOCK_DIV_BY_2, CKGEN_SYSTEM_CLOCK_DIV_BY_2},  /*!< Dividers for SPLL */ \
}

#define NUMBER_OF_TCLK_INPUTS    (3UL)

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief Define the enum of the RTC clock source.
 */
typedef enum
{
    RTC_CLK_SEL_HSEDIV1 = 0U,
    RTC_CLK_SEL_VHSIDIV1,
    RTC_CLK_SEL_LSI_32K,
    RTC_CLK_SEL_CLKIN
} rtc_clock_select_t;

/*!
 * @brief Define the enum of the LSI clock source.
 */
typedef enum
{
    LSI_CLK_SEL_128K = 0U,
    LSI_CLK_SEL_32K,
    LSI_CLK_SEL_1K,
    LSI_CLK_SEL_RSV
} lsi_clock_select_t;

/*!
 * @brief Define the enum of the clock output.
 */
typedef enum
{
    CKGEN_CLKOUT_SEL_HSE_CLK        = 0U,                               /*!< HSE CLK       */
    CKGEN_CLKOUT_SEL_HSI_CLK        = 1U,                               /*!< HSI CLK       */
    CKGEN_CLKOUT_SEL_VHSI_CLK       = 2U,                               /*!< VHSI CLK      */
    CKGEN_CLKOUT_SEL_SPLL_CLK       = 3U,                               /*!< SPLL CLK      */
    CKGEN_CLKOUT_SEL_RTC_CLK        = CKGEN_CLK_OUT_MUX2_BASE + 1U,     /*!< RTC CLK       */
    CKGEN_CLKOUT_SEL_LSI_CLK        = CKGEN_CLK_OUT_MUX2_BASE + 2U,     /*!< LSI CLK       */
    CKGEN_CLKOUT_SEL_LSI_128K_CLK   = CKGEN_CLK_OUT_MUX2_BASE + 3U,     /*!< LSI 128K CLK  */
    CKGEN_CLKOUT_SEL_HSE_DIV2_CLK   = CKGEN_CLK_OUT_MUX2_BASE + 4U,     /*!< HSE DIV2 CLK  */
    CKGEN_CLKOUT_SEL_VHSI_DIV2_CLK  = CKGEN_CLK_OUT_MUX2_BASE + 5U,     /*!< VHSI DIV2 CLK */
    CKGEN_CLKOUT_SEL_HSI_DIV2_CLK   = CKGEN_CLK_OUT_MUX2_BASE + 6U,     /*!< HSI DIV2 CLK  */
    CKGEN_CLKOUT_SEL_SPLL_DIV2_CLK  = CKGEN_CLK_OUT_MUX2_BASE + 7U,     /*!< SPLL DIV2 CLK */
    CKGEN_CLKOUT_SEL_BUS_CLK        = CKGEN_CLK_OUT_MUX2_BASE + 8U,     /*!< BUS_CLK       */
    CKGEN_CLKOUT_SEL_FLASH_CLK      = CKGEN_CLK_OUT_MUX2_BASE + 9U,     /*!< FLASH_CLK     */
    CKGEN_CLKOUT_SEL_SYSTEM_CLK     = CKGEN_CLK_OUT_MUX2_BASE + 10U,    /*!< SYSTEM_CLK    */
} ckgen_clkout_src_t;

/*!
 * @brief Define the enum of clock output divider.
 */
typedef enum
{
    CKGEN_CLKOUT_DIV_BY_1 = 0U,        /*!< Divided by 1 */
    CKGEN_CLKOUT_DIV_BY_2 = 1U,        /*!< Divided by 2 */
    CKGEN_CLKOUT_DIV_BY_3 = 2U,        /*!< Divided by 3 */
    CKGEN_CLKOUT_DIV_BY_4 = 3U,        /*!< Divided by 4 */
    CKGEN_CLKOUT_DIV_BY_5 = 4U,        /*!< Divided by 5 */
    CKGEN_CLKOUT_DIV_BY_6 = 5U,        /*!< Divided by 6 */
    CKGEN_CLKOUT_DIV_BY_7 = 6U,        /*!< Divided by 7 */
    CKGEN_CLKOUT_DIV_BY_8 = 7U,        /*!< Divided by 8 */
} ckgen_clkout_div_t;

/*!
 * @brief Clock output configuration .
 */
typedef struct
{
    bool initialize;                   /*!< Initialize or not the ClockOut clock. */
    bool enable;                       /*!< ClockOut enable.                      */
    ckgen_clkout_src_t source;         /*!< ClockOut source select.               */
    ckgen_clkout_div_t divider;        /*!< ClockOut divide ratio.                */
} ckgen_clock_out_config_t;

/*!
 * @brief Low power clock configuration.
 */
typedef struct
{
    bool initialize;                   /*!< Initialize or not the LSI clock.     */
    rtc_clock_select_t sourceRtcClk;   /*!< RTC clock source select.             */
    lsi_clock_select_t sourceLsiClk;   /*!< LSI clock source select.             */
} ckgen_lp_clock_config_t;

/*!
 * @brief Define the enum of CAN clock source.
 */
typedef enum
{
    CAN_CLK_SRC_RESERVE = 0U,
    CAN_CLK_SRC_HSEDIV2,
    CAN_CLK_SRC_SYS_CLK,
    CAN_CLK_SRC_SPLLDIV2
} can_clock_source_t;

/*!
 * @brief Define the enum of CAN clock source drivers.
 */
typedef enum
{
    CAN_CLK_DIV_1 = 0U,
    CAN_CLK_DIV_2
} can_clock_divider_t;

/*!
 * @brief Define the enum of CAN time stamp external clock divider.
 */
typedef enum
{
    CAN_TIME_STAMP_CLK_DIV_1 = 0U,
    CAN_TIME_STAMP_CLK_DIV_2,
    CAN_TIME_STAMP_CLK_DIV_3,
    CAN_TIME_STAMP_CLK_DIV_4,
    CAN_TIME_STAMP_CLK_DIV_5,
    CAN_TIME_STAMP_CLK_DIV_6,
    CAN_TIME_STAMP_CLK_DIV_7,
    CAN_TIME_STAMP_CLK_DIV_8
} can_time_stamp_clk_div_t;

/*!
 * @brief Define the enum of PWM external clock source.
 */
typedef enum
{
    PWM_EXT_CLK_SRC_TCLK0 = 0U,
    PWM_EXT_CLK_SRC_TCLK1,
    PWM_EXT_CLK_SRC_TCLK2,
    PWM_EXT_CLK_SRC_RESERVE
} pwm_ext_clock_source_t;

/*!
 * @brief PWM Clock configuration.
 */
typedef struct
{
    bool initialize;                              /*!< Initialize or not the TCLK clock.  */
    uint32_t tclkFreq[NUMBER_OF_TCLK_INPUTS];     /*!< TCLKx frequency.                   */
} ckgen_tclk_config_t;

/*!
 * @brief Debug Trace clock configuration.
 */
typedef struct
{
    bool initialize;                              /*!< Initialize or not the Trace clock. */
    uint8_t divider;                              /*!< Trace clock divider divisor: 0~16. */
} ckgen_trace_clock_config_t;

/*!
 * @brief SIM clock configuration.
 */
typedef struct
{
    ckgen_clock_out_config_t    clockOutConfig;   /*!< Clock Out configuration.              */
    ckgen_lp_clock_config_t     lpClockConfig;    /*!< Low Power Clock configuration.        */
    ckgen_tclk_config_t         tclkConfig;       /*!< TCLK, FTM option Clock configuration. */
    ckgen_trace_clock_config_t  traceClockConfig; /*!< Trace clock configuration.            */
} sim_clock_config_t;

/*!
 * @brief Define the enum of system clock source.
 */
typedef enum
{
    CKGEN_SYSTEM_CLOCK_SRC_VHSI   = 0U,     /*!< VHSI.       */
    CKGEN_SYSTEM_CLOCK_SRC_HSE    = 1U,     /*!< HSE.        */
    CKGEN_SYSTEM_CLOCK_SRC_HSI    = 2U,     /*!< HSI.        */
    CKGEN_SYSTEM_CLOCK_SRC_SPLL   = 3U,     /*!< System PLL. */
    CKGEN_SYSTEM_CLOCK_SRC_NONE   = 255U    /*!< MAX value.  */
} ckgen_system_clock_src_t;

/*!
 * @brief Define the enum of system clock source divider.
 */
typedef enum
{
    CKGEN_SYSTEM_CLOCK_DIV_BY_1   = 0U,     /*!< Divided by 1.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_2   = 1U,     /*!< Divided by 2.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_3   = 2U,     /*!< Divided by 3.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_4   = 3U,     /*!< Divided by 4.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_5   = 4U,     /*!< Divided by 5.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_6   = 5U,     /*!< Divided by 6.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_7   = 6U,     /*!< Divided by 7.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_8   = 7U,     /*!< Divided by 8.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_9   = 8U,     /*!< Divided by 9.  */
    CKGEN_SYSTEM_CLOCK_DIV_BY_10  = 9U,     /*!< Divided by 10. */
    CKGEN_SYSTEM_CLOCK_DIV_BY_11  = 10U,    /*!< Divided by 11. */
    CKGEN_SYSTEM_CLOCK_DIV_BY_12  = 11U,    /*!< Divided by 12. */
    CKGEN_SYSTEM_CLOCK_DIV_BY_13  = 12U,    /*!< Divided by 13. */
    CKGEN_SYSTEM_CLOCK_DIV_BY_14  = 13U,    /*!< Divided by 14. */
    CKGEN_SYSTEM_CLOCK_DIV_BY_15  = 14U,    /*!< Divided by 15. */
    CKGEN_SYSTEM_CLOCK_DIV_BY_16  = 15U,    /*!< Divided by 16. */
} ckgen_system_clock_div_t;

/*!
 * @brief System clock configuration.
 */
typedef struct
{
    ckgen_system_clock_div_t divBus;        /*!< BUS clock divider.   */
    ckgen_system_clock_div_t divCore;       /*!< Core clock divider.  */
    ckgen_system_clock_src_t src;           /*!< System clock source. */
} ckgen_system_clock_config_t;

/*!
 * @brief Asynchronous clock divider enumeration.
 */
typedef enum
{
    CKGEN_ASYNC_CLOCK_DIV_BY_1  = 0U,       /*!< Divided by 1.  */
    CKGEN_ASYNC_CLOCK_DIV_BY_2  = 1U,       /*!< Divided by 2.  */
    CKGEN_ASYNC_CLOCK_DIV_BY_3  = 2U,       /*!< Divided by 3.  */
    CKGEN_ASYNC_CLOCK_DIV_BY_4  = 3U,       /*!< Divided by 4.  */
    CKGEN_ASYNC_CLOCK_DIV_BY_5  = 4U,       /*!< Divided by 5.  */
    CKGEN_ASYNC_CLOCK_DIV_BY_6  = 5U,       /*!< Divided by 6.  */
    CKGEN_ASYNC_CLOCK_DIV_BY_7  = 6U,       /*!< Divided by 7.  */
    CKGEN_ASYNC_CLOCK_DIV_BY_8  = 7U,       /*!< Divided by 8.  */
    CKGEN_ASYNC_CLOCK_DIV_BY_16 = 15U,      /*!< Divided by 16. */
    CKGEN_ASYNC_CLOCK_DIV_BY_32 = 31U,      /*!< Divided by 32. */
    CKGEN_ASYNC_CLOCK_DIV_BY_64 = 63U       /*!< Divided by 64. */
} ckgen_async_clock_div_t;

/*!
 * @brief XOSC monitor mode enumeration.
 */
typedef enum
{
    CKGEN_XOSC_MONITOR_DISABLE = 0U,  /*!< XOSC monitor disable.              */
    CKGEN_XOSC_MONITOR_INT,           /*!< Interrupt when XOSC loss detected. */
    CKGEN_XOSC_MONITOR_RESET          /*!< Reset when XOSC loss detected.     */
} ckgen_xosc_monitor_mode_t;

/*!
 * @brief HSE external reference clock select enumeration.
 */
typedef enum
{
    CKGEN_HSE_REF_OSC = 0U,           /*!< XOSC requested.                    */
    CKGEN_HSE_REF_EXT                 /*!< XOSC bypassed, external reference clock requested. */
} ckgen_hse_ext_ref_t;

/*!
 * @brief HSE clock configuration.
 */
typedef struct
{
    uint32_t freq;                     /*!< HSE frequency.                    */
    ckgen_xosc_monitor_mode_t monitor; /*!< XOSC monitor mode.                */
    ckgen_hse_ext_ref_t extRef;        /*!< HSE external reference select.    */
    ckgen_async_clock_div_t div1;      /*!< Asynchronous peripheral source.   */
    ckgen_async_clock_div_t div2;      /*!< Asynchronous peripheral source.   */
    bool initialize;                   /*!< Initialize or not the HSE module. */
} ckgen_hse_config_t;

/*!
 * @brief HSI clock configuration.
 */
typedef struct
{
    ckgen_async_clock_div_t div1;     /*!< Asynchronous peripheral source.    */
    ckgen_async_clock_div_t div2;     /*!< Asynchronous peripheral source.    */
    bool initialize;                  /*!< Initialize or not the HSI module.  */
    bool enableInLowPower;            /*!< HSI is enable or not in the VLPS module. */
} ckgen_hsi_config_t;

/*!
 * @brief VHSI clock configuration.
 */
typedef struct
{
    ckgen_async_clock_div_t div1;     /*!< Asynchronous peripheral source.    */
    ckgen_async_clock_div_t div2;     /*!< Asynchronous peripheral source.    */
    bool initialize;                  /*!< Initialize or not the VHSI module. */
} ckgen_vhsi_config_t;

/*!
 * @brief System PLL lock detect enumeration.
 */
typedef enum
{
    CKGEN_SPLL_LOCK_DETECT_DISABLE = 0U,  /*!< SPLL lock detect disable.                  */
    CKGEN_SPLL_LOCK_DETECT_INT,           /*!< Interrupt when system PLL unlock detected. */
    CKGEN_SPLL_LOCK_DETECT_RESET          /*!< Reset when system PLL unlock detected.     */
} ckgen_spll_lock_detect_t;

/*!
 * @brief System PLL predivider enumeration.
 */
typedef enum
{
    CKGEN_SPLL_CLOCK_PREDIV_BY_1 = 0U,
    CKGEN_SPLL_CLOCK_PREDIV_BY_2 = 1U,
    CKGEN_SPLL_CLOCK_PREDIV_BY_4 = 3U
} ckgen_spll_clock_prediv_t;

/*!
 * @brief System PLL Post divider enumeration.
 */
typedef enum
{
    CKGEN_SPLL_CLOCK_POSDIV_BY_2 = 1U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_4 = 2U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_6 = 3U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_8 = 4U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_10 = 5U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_12 = 6U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_14 = 7U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_16 = 8U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_18 = 9U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_20 = 10U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_22 = 11U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_24 = 12U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_26 = 13U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_28 = 14U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_30 = 15U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_32 = 16U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_34 = 17U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_36 = 18U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_38 = 19U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_40 = 20U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_42 = 21U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_44 = 22U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_46 = 23U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_48 = 24U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_50 = 25U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_52 = 26U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_54 = 27U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_56 = 28U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_58 = 29U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_60 = 30U,
    CKGEN_SPLL_CLOCK_POSDIV_BY_62 = 31U
} ckgen_spll_clock_posdiv_t;

/*!
 * @brief SPLL reference clock select enumeration.
 */
typedef enum
{
    CKGEN_SPLL_REF_HSI = 0U,           /*!< System PLL reference clock source HSI. */
    CKGEN_SPLL_REF_HSE                 /*!< System PLL reference clock source HSE. */
} ckgen_spll_ref_clock_t;

/*!
 * @brief System PLL configuration.
 * SPLL_CLK = (SRC_CLK / (prediv + 1)) * fbkdiv / (posdiv * 2)
 * SPLLDIV1 = SPLL_CLK / (div1 + 1)
 * SPLLDIV2 = SPLL_CLK / (div2 + 1)
 */
typedef struct
{
    uint8_t    prediv;                 /*!< System PLL prediv.                       */
    uint8_t    fbkdiv;                 /*!< System PLL fbkdiv.                       */
    uint8_t    posdiv;                 /*!< System PLL posdiv.                       */
    uint8_t    src;                    /*!< System PLL source, 0:HSI, 1:HSE          */

    ckgen_spll_lock_detect_t lockDetect; /*!< System PLL lock detect.                */
    ckgen_async_clock_div_t div1;      /*!< Asynchronous peripheral source.          */
    ckgen_async_clock_div_t div2;      /*!< Asynchronous peripheral source.          */

    bool initialize;                   /*!< Initialize or not the System PLL module. */
} ckgen_spll_config_t;

/*!
 * @brief RTC configuration.
 */
typedef struct
{
    uint32_t rtcClkInFreq;            /*!< RTC_CLKIN frequency.                     */
    bool initialize;                  /*!< Initialize or not the RTC.               */
} ckgen_rtc_config_t;

/*!
 * @brief Clock Mode Configuration structure.
 */
typedef struct
{
    ckgen_system_clock_config_t rccrConfig;      /*!< Run Clock Control configuration.                */
    ckgen_system_clock_config_t vccrConfig;      /*!< VLPR Clock Control configuration.               */
    bool                        initialize;      /*!< Initialize or not the Clock Mode Configuration. */
} ckgen_clock_mode_config_t;

/*!
 * @brief CKGEN configure structure.
 */
typedef struct
{
    ckgen_hsi_config_t          hsiConfig;       /*!< Slow internal reference clock configuration.    */
    ckgen_vhsi_config_t         vhsiConfig;      /*!< Fast internal reference clock configuration.    */
    ckgen_hse_config_t          hseConfig;       /*!< System oscillator configuration.                */
    ckgen_spll_config_t         spllConfig;      /*!< System Phase locked loop configuration.         */
    ckgen_rtc_config_t          rtcConfig;       /*!< Real Time Clock configuration.                  */
    ckgen_clock_mode_config_t   clockModeConfig; /*!< Clock Mode Configuration.                       */
} ckgen_config_t;

/*!
 * @brief PCC divider value select enumeration.
 */
typedef enum
{
    DIVIDE_BY_ONE   = 0U,                  /*!< Divide by 1 */
    DIVIDE_BY_TWO   = 1U,                  /*!< Divide by 2 */
    DIVIDE_BY_THREE = 2U,                  /*!< Divide by 3 */
    DIVIDE_BY_FOUR  = 3U,                  /*!< Divide by 4 */
    DIVIDE_BY_FIVE  = 4U,                  /*!< Divide by 5 */
    DIVIDE_BY_SIX   = 5U,                  /*!< Divide by 6 */
    DIVIDE_BY_SEVEN = 6U,                  /*!< Divide by 7 */
    DIVIDE_BY_EIGTH = 7U                   /*!< Divide by 8 */
} peripheral_clock_divider_t;

/*!
 * @brief PCC peripheral instance clock configuration.
 */
typedef struct
{
    clock_names_t clockName;
    clock_names_t clkSrc;                  /*!< Peripheral clock source.        */
    peripheral_clock_divider_t divider;    /*!< Peripheral clock divider value. */
} peripheral_clock_config_t;

/*!
 * @brief PCC configuration.
 */
typedef struct
{
    uint32_t count;                              /*!< Number of peripherals to be configured.               */
    peripheral_clock_config_t *peripheralClocks; /*!< Pointer to the peripheral clock configurations array. */
} pcc_config_t;

/*!
 * @brief Clock configuration structure.
 */
typedef struct
{
    ckgen_config_t              ckgenConfig;     /*!< CKGEN Clock configuration.    */
    sim_clock_config_t          simConfig;       /*!< SIM Clock configuration.      */
    pcc_config_t                pccConfig;       /*!< PCC Clock configuration.      */
} clock_manager_user_config_t;

/*!< User clock configuration. */
typedef clock_manager_user_config_t clock_user_config_t;

/*!
 * @brief Power mode enumeration.
 */
typedef enum {
    NO_MODE       = 0U,
    RUN_MODE      = (1U << 0U),
    VLPR_MODE     = (1U << 1U),
    STOP_MODE     = (1U << 3U),
    VLPS_MODE     = (1U << 4U),
    ALL_MODES     = 0xFF
} pwr_modes_t;

/*!
 * @brief HSE reference clock select enumeration.
 */
typedef enum
{
    HSE_EXT_REF = 0U,       /*!< XOSC is bypassed, external reference clock requested. */
    HSE_INT_OSC             /*!< XOSC requested. */
} hse_ref_t;

/*!
 * @brief Module clock configuration.
 */
typedef struct
{
    clock_names_t source;    /*!< Clock source input */
    uint8_t       div;       /*!< Divider 1-8 (some modules don't have divider) */
} module_clk_config_t;

/*!
 * @brief System clock configuration.
 */
typedef struct
{
    clock_names_t src;                   /*!< System clock source. */
    uint8_t dividers[SYS_CLK_MAX_NO];    /*!< System clock dividers 1-16. Value by which system clock is divided. */
} sys_clk_config_t;

/*!
 * @brief Clock source configuration.
 * SPLL_CLK = (refFreq / (prediv + 1)) * fbkdiv / (posdiv * 2)
 * SPLLDIV1 = SPLL_CLK / outputDiv1;
 * SPLLDIV2 = SPLL_CLK / outputDiv2;
 */
typedef struct
{
    uint32_t   refFreq;                  /*!< Frequency of the input reference clock. It applies to external oscillator clock sources. */
    bool       enable;                   /*!< Enable/disable clock source. */
    hse_ref_t  refClk;                   /*!< Bypass option. It applies to external oscillator clock sources. */
    uint8_t    src;                      /*!< Used for PLL ref clock source. */
    uint8_t    fbkdiv;                   /*!< feedback Divider. It applies to PLL clock sources. Valid range is 5~255. */
    uint8_t    prediv;                   /*!< pre Divider. It applies to PLL clock sources. Valid range is 0, 1, 3. */
    uint8_t    posdiv;                   /*!< pos Divider. It applies to PLL clock sources. Valid range is 1~31. */
    uint8_t    outputDiv1;               /*!< First output divider. It's used as protocol clock by modules. Valid range is 1~64. */
    uint8_t    outputDiv2;               /*!< Second output divider. It's used as protocol clock by modules. Valid range is 1~64. */
} clock_source_config_t;

/*!
 * @brief The clock notification type enumeration.
 */
typedef enum
{
    CLOCK_MANAGER_NOTIFY_RECOVER = 0U,   /*!< Notify IP to recover to previous work state.      */
    CLOCK_MANAGER_NOTIFY_BEFORE,         /*!< Notify IP that system will change clock setting.  */
    CLOCK_MANAGER_NOTIFY_AFTER           /*!< Notify IP that have changed to new clock setting. */
} clock_manager_notify_t;

/*!
 * @brief The callback type, indicates what kinds of notification this callback handles.
 */
typedef enum
{
    CLOCK_MANAGER_CALLBACK_BEFORE = 1U,  /*!< Callback handles BEFORE notification.           */
    CLOCK_MANAGER_CALLBACK_AFTER,        /*!< Callback handles AFTER notification.            */
    CLOCK_MANAGER_CALLBACK_BEFORE_AFTER  /*!< Callback handles BEFORE and AFTER notification. */
} clock_manager_callback_type_t;

/*!
 * @brief Clock transition policy enumeration.
 */
typedef enum
{
    CLOCK_MANAGER_POLICY_AGREEMENT,      /*!< Clock transfers gracefully. */
    CLOCK_MANAGER_POLICY_FORCIBLE        /*!< Clock transfers forcefully. */
} clock_manager_policy_t;

/*!
 * @brief Clock notification structure passed to clock callback function.
 */
typedef struct
{
    uint8_t targetClockConfigIndex;      /*!< Target clock configuration index. */
    clock_manager_policy_t policy;       /*!< Clock transition policy.          */
    clock_manager_notify_t notifyType;   /*!< Clock notification type.          */
} clock_notify_struct_t;

/*!
 * @brief Type of clock callback functions.
 */
typedef status_t (*clock_manager_callback_t)(clock_notify_struct_t *notify,
                                             void* callbackData);

/*!
 * @brief Structure for callback function and its parameter.
 */
typedef struct
{
    clock_manager_callback_t      callback;      /*!< Entry of callback function.     */
    clock_manager_callback_type_t callbackType;  /*!< Callback type.                  */
    void *callbackData;                          /*!< Parameter of callback function. */
} clock_manager_callback_user_config_t;

/*!
 * @brief Clock manager state structure.
 */
typedef struct
{
    clock_manager_user_config_t const **configTable; /*!< Pointer to clock configure table. */
    uint8_t clockConfigNum;                          /*!< Number of clock configurations.   */
    uint8_t curConfigIndex;                          /*!< Index of current configuration.   */
    clock_manager_callback_user_config_t **callbackConfig; /*!< Pointer to callback table.  */
    uint8_t callbackNum;                             /*!< Number of clock callbacks.        */
    uint8_t errorCallbackIndex;                      /*!< Index of callback returns error.  */
} clock_manager_state_t;

/* ==========================================  Variables  =========================================== */
/*!< TClk clock frequency. */
extern uint32_t g_TClkFreq[NUMBER_OF_TCLK_INPUTS];      /*!< TCLKx clocks */

/*!< HSE clock frequency. */
extern uint32_t g_HseClkFreq;

/*!< RTC_CLKIN clock frequency. */
extern uint32_t g_RtcClkInFreq;

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Set the module bus clock.
 *
 * @param[in] module: the module which will be set bus clock
 * @param[in] enable: true:enable bus clock, false: disable bus clock
 * @return none
 */
void CKGEN_DRV_Enable(ckgen_clock_t module, bool enable);

/*!
 * @brief Soft reset for the module.
 *
 * @param[in] module: the module to be Soft Reset
 * @param[in] enable: true: active, false: inactive
 * @return  none
 */
void CKGEN_DRV_SoftReset(ckgen_softreset_t module, bool enable);

/*!
 * @brief Configures module clock
 *
 * @param[in] peripheralClock: Clock name of the configured module clock
 * @param[in] moduleClkConfig: Pointer to the configuration structure.
 * @return  none
 */
void CKGEN_DRV_SetModuleClock(clock_names_t peripheralClock, const module_clk_config_t *moduleClkConfig);

/*!
 * @brief Configures the system clocks.
 *        This function configures the system clocks (core, bus and flash clocks) in the specified power mode.
 *        If no power mode is specified (null parameter) then it is the current power mode.
 *
 * @param[in] mode:         Pointer to power mode for which the configured system clocks apply
 * @param[in] sysClkConfig: Pointer to the system clocks configuration structure
 * @return STATUS_SUCCESS, STATUS_ERROR or STATUS_TIMEOUT
 */
status_t CKGEN_DRV_SetSystemClock(const pwr_modes_t *mode, const sys_clk_config_t *sysClkConfig);

/*!
 * @brief Get system clock source.
 *
 * @param[out] sysClkConfig: Store system clock configuration.
 * @return none
 */
void CKGEN_DRV_GetSystemClockSource(sys_clk_config_t *sysClkConfig);

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
status_t CKGEN_DRV_SetClockSource(clock_names_t clockSource, const clock_source_config_t *clkSrcConfig);

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
                        uint8_t callbacksNumber);

/*!
 * @brief Send notification and change system clock configuration.
 *        This function sets system to target clock configuration; before transition,
 *        clock manager will send notifications to all drivers registered to the
 *        callback table.  When graceful policy is used, if some drivers are not ready
 *        to change, clock transition will not occur, all drivers still work in
 *        previous configuration and error is returned. When forceful policy is used,
 *        all drivers should stop work and system changes to new clock configuration.
 *        The function should be called only on run mode.
 *
 * @param[in] targetConfigIndex: Index of the clock configuration
 * @param[in] policy: Transaction policy, graceful or forceful
 * @return Error code
 */
status_t CKGEN_DRV_UpdateConfiguration(uint8_t targetConfigIndex, clock_manager_policy_t policy);

/*!
 * @brief This function sets the system to target configuration, it
 *        only sets the clock modules registers for clock mode change, but not send
 *        notifications to drivers.
 *
 * @param[in] config: User clock configuration pointer
 * @return STATUS_SUCCESS, STATUS_BUSY, STATUS_TIMEOUT
 */
status_t CKGEN_DRV_SetConfiguration(clock_manager_user_config_t const *config);

/*!
 * @brief Get current clock configuration index.
 *
 * @param[in] none
 * @return Current clock configuration index
 */
uint8_t CKGEN_DRV_GetCurrentConfiguration(void);

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
clock_manager_callback_user_config_t *CKGEN_DRV_GetErrorCallback(void);

/*!
 * @brief Get frequency which moudel is defined in clock_names_t.
 *
 * @param[in] clockName: Clock names defined in clock_names_t
 * @param[out] frequency: Returned clock frequency value in Hertz
 * @return status defined in status_t
 */
status_t CKGEN_DRV_GetFreq(clock_names_t clockName, uint32_t *frequency);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CKGEN_DRV_H */

/* =============================================  EOF  ============================================== */
