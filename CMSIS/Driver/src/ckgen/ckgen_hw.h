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
 * @file ckgen_hw.h
 *
 * @brief ckgen access register inline function definition.
 *
 */

#ifndef _CKGEN_HW_H
#define _CKGEN_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief  set output clock source by the clock_out_pin
 *
 * @param[in] base: Register base address for the CKGEN
 * @param[in] source: clock_out_source_t value can be
 *           - CLOCK_OUT_SOURCE_HSE
 *           - CLOCK_OUT_SOURCE_HSI
 *           - CLOCK_OUT_SOURCE_VHSI
 *           - CLOCK_OUT_SOURCE_SPLL
 *           - CLOCK_OUT_SOURCE_RTC
 *           - CLOCK_OUT_SOURCE_LSI
 *           - CLOCK_OUT_SOURCE_LSI128K
 *           - CLOCK_OUT_SOURCE_HSEDIV2
 *           - CLOCK_OUT_SOURCE_VHSIDIV2
 *           - CLOCK_OUT_SOURCE_HSIDIV2
 *           - CLOCK_OUT_SOURCE_SPLLDIV2
 *           - CLOCK_OUT_SOURCE_BUSCLK
 *           - CLOCK_OUT_SOURCE_FLASH
 *           - CLOCK_OUT_SOURCE_SYSCLK
 * @return none
 */
void CKGEN_SetClockoutSourceSel(CKGEN_Type *base, uint32_t source);

/*!
 * @brief Get clock out source.
 *
 * @param[in] base: Register base address for the CKGEN
 * @return clock out source, refer to clock_out_source_t
 */
uint32_t CKGEN_GetClockoutSourceSel(const CKGEN_Type *base);

/*!
 * @brief Get clock out source from mux1.
 *
 * @param[in] base: Register base address for the CKGEN
 * @return clock out source, refer to clock_out_source_t
 */
uint32_t CKGEN_GetClockoutSourceFromMux1(const CKGEN_Type *base);

/*!
 * @brief Gets the current system clock source.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return Current system clock source
 */
uint32_t CKGEN_GetCurrentSystemClockSource(const CKGEN_Type *base);

/*!
 * @brief Set CAN function clock source.
 *
 * @param[in] instance: 0:CAN0 1:CAN1 2:CAN2 3:CAN3
 * @param[in] source: CAN clock source sourceect, value can be
 *                - PERI_CLK_OFF (no clock)
 *                - HSEDIV2_CLK
 *                - CORE_CLK
 *                - SPLLDIV2_CLK
 * @return none
 */
void CKGEN_SetCANClock(uint8_t instance, clock_names_t source);

/*!
 * @brief Get CAN function clock source.
 *
 * @param[in] instance: 0:CAN0 1:CAN1 2:CAN2 3:CAN3
 * @return CAN clock source source, value can be
 *                - PERI_CLK_OFF (no clock)
 *                - HSEDIV2_CLK
 *                - CORE_CLK
 *                - SPLLDIV2_CLK
 */
clock_names_t CKGEN_GetCANClock(uint8_t instance);

/*!
 * @brief Set CAN function clock divider.
 *
 * @param[in] instance: 0:CAN0 1:CAN1 2:CAN2 3:CAN3
 * @param[in] divider: CAN clock divider, value can be
 *                - CAN_CLK_DIV_1
 *                - CAN_CLK_DIV_2
 * @return none
 */
void CKGEN_SetCANClockDivider(uint8_t instance, can_clock_divider_t divider);

/*!
 * @brief Get CAN function clock divider.
 *
 * @param[in] instance: 0:CAN0 1:CAN1 2:CAN2 3:CAN3
 * @return CAN clock divider, value can be
 *                - CAN_CLK_DIV_1
 *                - CAN_CLK_DIV_2
 */
can_clock_divider_t CKGEN_GetCANClockDivider(uint8_t instance);

/*!
 * @brief Set the CAN time stamp external clock divider.
 *
 * @param[in] instance: 0:CAN0 1:CAN1 2:CAN2 3:CAN3
 * @param[in] divider: CAN time stamp clock divider, value can be
 *                - CAN_TIME_STAMP_CLK_DIV_1
 *                - CAN_TIME_STAMP_CLK_DIV_2
 *                - ...
 *                - CAN_TIME_STAMP_CLK_DIV_8
 * @return none
 */
void CKGEN_SetCANTimeStampClkDiv(uint8_t instance, can_time_stamp_clk_div_t divider);

/*!
 * @brief Set RTC function clock.
 *
 * @param[in] rtcSel: RTC clock source select, value can be
 *                - RTC_CLK_SEL_HSEDIV1
 *                - RTC_CLK_SEL_VHSIDIV1
 *                - RTC_CLK_SEL_LSI_32K
 *                - RTC_CLK_SEL_CLKIN
 * @param[in] lsisSel: LSI clock source select, value can be
 *                - LSI_CLK_SEL_128K
 *                - LSI_CLK_SEL_32K
 *                - LSI_CLK_SEL_1K
 *                - LSI_CLK_SEL_RSV
 * @return none
 */
void CKGEN_SetRTCLSIClock(rtc_clock_select_t rtcSel, lsi_clock_select_t lsiSel);

/*!
 * @brief Get RTC function clock.
 *
 * @param[in] base: CKGEN module base address pointer
 * @return RTC clock source
 */
uint32_t CKGEN_GetRTCClock(const CKGEN_Type *base);

/*!
 * @brief Get LSI function clock.
 *
 * @param[in] base: CKGEN module base address pointer
 * @return none
 */
uint32_t CKGEN_GetLSIClock(const CKGEN_Type *base);

/*!
 * @brief set PWM function clock.
 *
 * @param[in] instance: PWM instance, value can be
 *                - 0: PWM0
 *                - 1: PWM1
 *                - 2: PWM2
 *                - 3: PWM3
 *                - 4: PWM4
 *                - 5: PWM5
 * @param[in] source: PWM module source select
 *                - HSEDIV1_CLK
 *                - HSIDIV1_CLK
 *                - VHSIDIV1_CLK
 *                - TCLK0_CLK
 *                - TCLK1_CLK
 *                - TCLK2_CLK
 *                - TCLK3_CLK
 * @return none
 */
void CKGEN_SetPWMClock(uint32_t instance, clock_names_t source);

/*!
 * @brief Get PWM function clock.
 *
 * @param[in] instance: PWM instance, value can be
 *                - 0: PWM0
 *                - 1: PWM1
 *                - 2: PWM2
 *                - 3: PWM3
 *                - 4: PWM4
 *                - 5: PWM5
 * @param[out] extSrcEnable: PWM module clock source is from external or internal
 *                - 0: internal clock source
 *                - 1: external clock source
 * @param[out] internalSrc: PWM module internal clock source
 * @param[out] extSrc: PWM module external clock source
 * @return none
 */
void CKGEN_GetPWMClock(uint32_t instance, uint8_t *extSrcEnable, uint8_t *internalSrc, uint8_t *extSrc);

/*!
 * @brief set TPIU clock divider.
 *
 * @param[in] instance: PWM instance, value can be
 * @param[in] div: 0~15, means divider is 1~16
 * @return none
 */
void CKGEN_SetTPIUClkDiv(uint32_t div);

/*!
 * @brief set peripheral(except can、pwm、rtc) function clock.
 *
 * @param[in] peripheralName: value can be one of the list value
 *                - I2C0_CLK
 *                - TIMER_CLK
 *                - SPI0_CLK
 *                - SPI1_CLK
 *                - SPI2_CLK
 *                - ADC0_CLK
 *                - ADC1_CLK
 *                - PCT_CLK
 *                - EIO_CLK
 *                - UART0_CLK
 *                - UART1_CLK
 *                - UART2_CLK
 *                - UART3_CLK
 * @param[in] source: peripheral clock source select, value can be
 *                - HSEDIV2_CLK
 *                - HSIDIV2_CLK
 *                - VHSIDIV2_CLK
 *                - SPLLDIV2_CLK
 * @return none
 */
void CKGEN_SetPeripheralClock(clock_names_t peripheralName, clock_names_t source);

/*!
 * @brief Get peripheral(except can、pwm、rtc) function clock.
 *
 * @param[in] peripheralName: value can be one of the list value
 *                - I2C0_CLK
 *                - TIMER_CLK
 *                - SPI0_CLK
 *                - SPI1_CLK
 *                - SPI2_CLK
 *                - ADC0_CLK
 *                - ADC1_CLK
 *                - PCT_CLK
 *                - EIO_CLK
 *                - UART0_CLK
 *                - UART1_CLK
 *                - UART2_CLK
 *                - UART3_CLK
 * @return peripheral clock source
 */
uint8_t CKGEN_GetPeripheralClock(clock_names_t peripheralName);

/*!
 * @brief Gets Current bus clock divide ratio
 *
 * @param[in] base Register base address for the CKGEN instance
 * @return Current  bus clock divide ratio
 */
uint32_t CKGEN_GetCurrentBusClockDividerRatio(const CKGEN_Type *base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Sets system clock source for run mode.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @param[in] source: System clock source
 * @return none
 */
static inline void CKGEN_SetRunSysClkSrc(CKGEN_Type *base, uint32_t source)
{
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos, (uint32_t)source);
}

/*!
 * @brief Sets system clock source and system clock, bus clock dividers for run mode.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @param[in] source: System clock source
 * @param[in] sysDivider: System clock divider
 * @param[in] busDivider: Bus clock divider
 * @return none
 */
static inline void CKGEN_SetRunClockControl(CKGEN_Type *base, uint32_t source, uint32_t sysDivider, uint32_t busDivider)
{
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos, (uint32_t)source);
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_SYSCLK_DIV_Msk, CKGEN_CTRL_SYSCLK_DIV_Pos, (uint32_t)sysDivider);
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_BUS_CLK_DIV_RUN_Msk, CKGEN_CTRL_BUS_CLK_DIV_RUN_Pos, (uint32_t)busDivider);
}

/*!
 * @brief Sets system clock source and system clock, bus clock dividers for vlpr mode.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @param[in] source: System clock source(Only support HSI as system clock in vlpr mode)
 * @param[in] sysDivider: System clock divider
 * @param[in] busDivider: Bus clock divider
 * @return none
 */
static inline void CKGEN_SetVLPRClockControl(CKGEN_Type *base, uint32_t source, uint32_t sysDivider, uint32_t busDivider)
{
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_SYSCLK_SRC_SEL_VLPR_Msk, CKGEN_CTRL_SYSCLK_SRC_SEL_VLPR_Pos, (uint32_t)source);
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_SYSCLK_DIV_VLPR_Msk, CKGEN_CTRL_SYSCLK_DIV_VLPR_Pos, (uint32_t)sysDivider);
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_BUS_CLK_DIV_VLPR_Msk, CKGEN_CTRL_BUS_CLK_DIV_VLPR_Pos, (uint32_t)busDivider);
}

/*!
 * @brief Gets the current core clock divide ratio.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return Current Ccore clock divide ratio
 */
static inline uint32_t CKGEN_GetCurrentSysClockDividerRatio(const CKGEN_Type *base)
{
    return ((base->CTRL & CKGEN_CTRL_SYSCLK_DIV_Msk) >> CKGEN_CTRL_SYSCLK_DIV_Pos);
}

/*!
 * @brief Sets asynchronous dividers for VHSI.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @param[in] div1: Asynchronous first divider
 * @param[in] div2: Asynchronous second divider
 * @return none
 */
static inline void CKGEN_SetVHSIAsyncConfig(CKGEN_Type *base, uint32_t div1, uint32_t div2)
{
    MODIFY_REG32(base->CLK_DIV1, CKGEN_CLK_DIV1_VHSI_DIV1_Msk, CKGEN_CLK_DIV1_VHSI_DIV1_Pos, div1);
    MODIFY_REG32(base->CLK_DIV2, CKGEN_CLK_DIV2_VHSI_DIV2_Msk, CKGEN_CLK_DIV2_VHSI_DIV2_Pos, div2);
}

/*!
 * @brief Gets the first asynchronous divider for VHSI.
 *
 * @param[in] base: Register base address for the CKGEN instance.
 * @return VHSI first asynchronous divider
 */
static inline uint32_t CKGEN_GetVHSIFirstAsyncDivider(const CKGEN_Type *base)
{
    return ((base->CLK_DIV1 & CKGEN_CLK_DIV1_VHSI_DIV1_Msk) >> CKGEN_CLK_DIV1_VHSI_DIV1_Pos);
}

/*!
 * @brief Gets the second asynchronous divider for VHSI.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return VHSI second asynchronous divider
 */
static inline uint32_t CKGEN_GetVHSISecondAsyncDivider(const CKGEN_Type *base)
{
    return ((base->CLK_DIV2 & CKGEN_CLK_DIV2_VHSI_DIV2_Msk) >> CKGEN_CLK_DIV2_VHSI_DIV2_Pos);
}

/*!
 * @brief Sets asynchronous dividers for HSI.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @param[in] div1: Asynchronous first divider
 * @param[in] div2: Asynchronous second divider
 * @return none
 */
static inline void CKGEN_SetHSIAsyncConfig(CKGEN_Type *base, uint32_t div1, uint32_t div2)
{
    MODIFY_REG32(base->CLK_DIV1, CKGEN_CLK_DIV1_HSI_DIV1_Msk, CKGEN_CLK_DIV1_HSI_DIV1_Pos, div1);
    MODIFY_REG32(base->CLK_DIV2, CKGEN_CLK_DIV2_HSI_DIV2_Msk, CKGEN_CLK_DIV2_HSI_DIV2_Pos, div2);
}

/*!
 * @brief Gets the first asynchronous divider for HSI.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return HSI first asynchronous divider
 */
static inline uint32_t CKGEN_GetHSIFirstAsyncDivider(const CKGEN_Type *base)
{
    return ((base->CLK_DIV1 & CKGEN_CLK_DIV1_HSI_DIV1_Msk) >> CKGEN_CLK_DIV1_HSI_DIV1_Pos);
}

/*!
 * @brief Gets the second asynchronous divider for HSI.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return HSI second asynchronous divider
 */
static inline uint32_t CKGEN_GetHSISecondAsyncDivider(const CKGEN_Type *base)
{
    return ((base->CLK_DIV2 & CKGEN_CLK_DIV2_HSI_DIV2_Msk) >> CKGEN_CLK_DIV2_HSI_DIV2_Pos);
}

/*!
 * @brief Enable or disable XOSC monitor.
 *
 * @param[in] base: Register base address for the CKGEN instance.
 * @param[in] enable: Enable or disable
 * @return none
 */
static inline void CKGEN_SetXOSCMonitor(CKGEN_Type *base, bool enable)
{
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_XOSC_MON_EN_Msk, CKGEN_CTRL_XOSC_MON_EN_Pos, enable);
}

/*!
 * @brief Sets asynchronous dividers for HSE.
 *
 * @param[in] base: Register base address for the CKGEN instance.
 * @param[in] div1: Asynchronous first divider
 * @param[in] div2: Asynchronous second divider
 * @return none
 */
static inline void CKGEN_SetHSEAsyncConfig(CKGEN_Type *base, uint32_t div1, uint32_t div2)
{
    MODIFY_REG32(base->CLK_DIV1, CKGEN_CLK_DIV1_HSE_DIV1_Msk, CKGEN_CLK_DIV1_HSE_DIV1_Pos, div1);
    MODIFY_REG32(base->CLK_DIV2, CKGEN_CLK_DIV2_HSE_DIV2_Msk, CKGEN_CLK_DIV2_HSE_DIV2_Pos, div2);
}

/*!
 * @brief Gets the first asynchronous divider for HSE.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return HSE first asynchronous divider
 */
static inline uint32_t CKGEN_GetHSEFirstAsyncDivider(const CKGEN_Type *base)
{
    return ((base->CLK_DIV1 & CKGEN_CLK_DIV1_HSE_DIV1_Msk) >> CKGEN_CLK_DIV1_HSE_DIV1_Pos);
}

/*!
 * @brief Gets the second asynchronous divider for HSE.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return HSE second asynchronous divider
 */
static inline uint32_t CKGEN_GetHSESecondAsyncDivider(const CKGEN_Type *base)
{
    return ((base->CLK_DIV2 & CKGEN_CLK_DIV2_HSE_DIV2_Msk) >> CKGEN_CLK_DIV2_HSE_DIV2_Pos);
}

/*!
 * @brief Sets asynchronous dividers for SPLL.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @param[in] div1: Asynchronous first divider
 * @param[in] div2: Asynchronous second divider
 * @return none
 */
static inline void CKGEN_SetSPLLAsyncConfig(CKGEN_Type *base, uint32_t div1, uint32_t div2)
{
    MODIFY_REG32(base->CLK_DIV1, CKGEN_CLK_DIV1_SPLL_DIV1_Msk, CKGEN_CLK_DIV1_SPLL_DIV1_Pos, div1);
    MODIFY_REG32(base->CLK_DIV2, CKGEN_CLK_DIV2_SPLL_DIV2_Msk, CKGEN_CLK_DIV2_SPLL_DIV2_Pos, div2);
}

/*!
 * @brief Gets the first asynchronous divider for SPLL.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return SPLL first asynchronous divider
 */
static inline uint32_t CKGEN_GetSPLLFirstAsyncDivider(const CKGEN_Type *base)
{
    return ((base->CLK_DIV1 & CKGEN_CLK_DIV1_SPLL_DIV1_Msk) >> CKGEN_CLK_DIV1_SPLL_DIV1_Pos);
}

/*!
 * @brief Gets the second asynchronous divider for SPLL.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return SPLL second asynchronous divider
 */
static inline uint32_t CKGEN_GetSPLLSecondAsyncDivider(const CKGEN_Type *base)
{
    return ((base->CLK_DIV2 & CKGEN_CLK_DIV2_SPLL_DIV2_Msk) >> CKGEN_CLK_DIV2_SPLL_DIV2_Pos);
}

/*!
 * @brief Sets clock source for SPLL.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @param[in] source: Select clock source for SPLL
 *                  -1:HSE
 *                  -0:HSI
 */
static inline void CKGEN_SetSourceSPLL(CKGEN_Type *base, uint32_t source)
{
    MODIFY_REG32(base->CTRL, CKGEN_CTRL_PLL_REF_SEL_Msk, CKGEN_CTRL_PLL_REF_SEL_Pos, source);
}

/*!
 * @brief Gets the clock source for SPLL.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return SPLL source
 */
static inline uint32_t CKGEN_GetSourcSPLL(const CKGEN_Type *base)
{
    return ((base->CTRL & CKGEN_CTRL_PLL_REF_SEL_Msk) >> CKGEN_CTRL_PLL_REF_SEL_Pos);
}

/*!
 * @brief Set CKGEN CTRL register lock.
 *
 * @param[in] enable: clock CTRL register lock set, value can be
 *                - true: register lock
 *                - false: register unlock
 * @return none
 */
static inline void CKGEN_CtrlRegisterLock(bool enable)
{
    MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_LOCK_Msk, CKGEN_CTRL_LOCK_Pos, (uint32_t)enable);
}

/*!
 * @brief Sets SPLL configuration register.
 *
 * @param[in] base: ANA base pointer
 * @param[in] pllPosdiv: SPLL posdivider value
 * @param[in] pllFbkdiv: SPLL fbkdivider value
 * @param[in] pllPrediv: SPLL predivider value
 * @return none
 */
static inline void CKGEN_SetSPLLConfiguration(ANA_Type *base, uint8_t pllPosdiv, uint8_t pllFbkdiv, uint8_t pllPrediv)
{
    MODIFY_REG32(base->SPLL_CFG0, ANA_SPLL_CFG0_POSDIV_Msk, ANA_SPLL_CFG0_POSDIV_Pos, pllPosdiv);
    MODIFY_REG32(base->SPLL_CFG0, ANA_SPLL_CFG0_FBKDIV_Msk, ANA_SPLL_CFG0_FBKDIV_Pos, pllFbkdiv);
    MODIFY_REG32(base->SPLL_CFG0, ANA_SPLL_CFG0_PREDIV_Msk, ANA_SPLL_CFG0_PREDIV_Pos, pllPrediv);
    SET_BIT32(ANA->SPLL_CFG1, ANA_SPLL_CFG1_LD_EN_Msk | ANA_SPLL_CFG1_LD_SAMP0_EN_Msk | \
        ANA_SPLL_CFG1_LD_SAMP1_EN_Msk);  /* Just for ES1 */
    MODIFY_REG32(ANA->SPLL_CFG1, ANA_SPLL_CFG1_LD_DLY_SEL_Msk, ANA_SPLL_CFG1_LD_DLY_SEL_Pos, 3U);
}

/*!
 * @brief Gets SPLL configuration register.
 *
 * @param[out] base: ANA base pointer
 * @param[out] pllPosdiv: SPLL posdivider value
 * @param[out] pllFbkdiv: SPLL fbkdivider value
 * @param[out] pllPrediv: SPLL predivider value
 * @return none
 */
static inline void CKGEN_GetSPLLConfiguration(ANA_Type *base, uint8_t *pllPosdiv, uint8_t *pllFbkdiv, uint8_t *pllPrediv)
{
    *(uint8_t *)pllPosdiv = (uint8_t)(READ_BIT32(base->SPLL_CFG0, ANA_SPLL_CFG0_POSDIV_Msk) >> ANA_SPLL_CFG0_POSDIV_Pos);
    *(uint8_t *)pllFbkdiv = (uint8_t)(READ_BIT32(base->SPLL_CFG0, ANA_SPLL_CFG0_FBKDIV_Msk) >> ANA_SPLL_CFG0_FBKDIV_Pos);
    *(uint8_t *)pllPrediv = (uint8_t)(READ_BIT32(base->SPLL_CFG0, ANA_SPLL_CFG0_PREDIV_Msk) >> ANA_SPLL_CFG0_PREDIV_Pos);
}

/*!
 * @brief  Set output clock divider
 *
 * @param[in] base: CKGEN base pointer
 * @param[in] div: 0~7 means divider is 1~8
 * @return none
 */
static inline void CKGEN_SetClockoutDivider(CKGEN_Type *base, uint32_t div)
{
    MODIFY_REG32(base->CLK_OUT_CFG, CKGEN_CLK_OUT_CFG_DIV_Msk, CKGEN_CLK_OUT_CFG_DIV_Pos, div);
}

/*!
 * @brief  Get clock out divider
 *
 * @param[in] base: CKGEN base pointer
 * @return clock out divider: 0~7 means divider is 1~8
 */
static inline uint32_t CKGEN_GetClockoutDivider(CKGEN_Type *base)
{
    return (uint32_t)((base->CLK_OUT_CFG & CKGEN_CLK_OUT_CFG_DIV_Msk) >> CKGEN_CLK_OUT_CFG_DIV_Pos);
}

/*!
 * @brief  Enable or disable output clock.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @param[in] enable: Enable or disable output clock
 * @return none
 */
static inline void CKGEN_SetClockout(CKGEN_Type *base, bool enable)
{
    MODIFY_REG32(base->CLK_OUT_CFG, CKGEN_CLK_OUT_CFG_ENABLE_Msk, CKGEN_CLK_OUT_CFG_ENABLE_Pos, enable);
}

#if defined(__cplusplus)
}
#endif /* __cplusplus*/


#endif /* _CKGEN_HW_H */

/* =============================================  EOF  ============================================== */
