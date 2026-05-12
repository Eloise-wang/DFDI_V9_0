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
 * @file ckgen_hw.c
 *
 * @brief This file provides ckgen module hardware integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "ckgen_hw.h"
#include "../spm/spm_hw.h"

/* ============================================  Define  ============================================ */
#define CKGEN_PERI_CLK_MUX_WIDTH           (4UL)
#define CKGEN_PERI_CLK_MUX_MASK            (0xFU)
#define CKGEN_PERI_CLK_MUX_MODULE_NUM      (8U)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set output clock source by the clock out pin.
 *
 * @param[in] base: Register base address for the CKGEN
 * @param[in] source: clock_out_source_t value can be
 *           - CKGEN_CLKOUT_SEL_HSE_CLK
 *           - CKGEN_CLKOUT_SEL_HSI_CLK
 *           - CKGEN_CLKOUT_SEL_VHSI_CLK
 *           - CKGEN_CLKOUT_SEL_SPLL_CLK
 *           - CKGEN_CLKOUT_SEL_RTC_CLK
 *           - CKGEN_CLKOUT_SEL_LSI_CLK
 *           - CKGEN_CLKOUT_SEL_LSI_128K_CLK
 *           - CKGEN_CLKOUT_SEL_HSE_DIV2_CLK
 *           - CKGEN_CLKOUT_SEL_VHSI_DIV2_CLK
 *           - CKGEN_CLKOUT_SEL_HSI_DIV2_CLK
 *           - CKGEN_CLKOUT_SEL_SPLL_DIV2_CLK
 *           - CKGEN_CLKOUT_SEL_BUS_CLK
 *           - CKGEN_CLKOUT_SEL_FLASH_CLK
 *           - CKGEN_CLKOUT_SEL_SYSTEM_CLK
 * @return none
 */
void CKGEN_SetClockoutSourceSel(CKGEN_Type *base, uint32_t source)
{
    /* before change clock out source, user should disable output function first. */
    MODIFY_REG32(CKGEN->CLK_OUT_CFG, CKGEN_CLK_OUT_CFG_ENABLE_Msk, CKGEN_CLK_OUT_CFG_ENABLE_Pos, 0U);

    if (source < CKGEN_CLK_OUT_MUX2_BASE) /* Select mux1 as output clock */
    {
        MODIFY_REG32(CKGEN->CLK_OUT_CFG, CKGEN_CLK_OUT_CFG_MUX1_Msk, CKGEN_CLK_OUT_CFG_MUX1_Pos, (uint32_t)source);
        /* select clock out mux1 output */
        MODIFY_REG32(CKGEN->CLK_OUT_CFG, CKGEN_CLK_OUT_CFG_MUX2_Msk, CKGEN_CLK_OUT_CFG_MUX2_Pos, 0U);
    }
    else  /* Select mux2 as output clock */
    {
        MODIFY_REG32(CKGEN->CLK_OUT_CFG, CKGEN_CLK_OUT_CFG_MUX2_Msk, CKGEN_CLK_OUT_CFG_MUX2_Pos, (uint32_t)(source - CKGEN_CLK_OUT_MUX2_BASE));
    }
}

/*!
 * @brief Get clock out source.
 *
 * @param[in] base: Register base address for the CKGEN
 * @return clock out source, refer to clock_out_source_t
 */
uint32_t CKGEN_GetClockoutSourceSel(const CKGEN_Type *base)
{
    uint32_t clockSource = 0U;

    /* check clock out mux1 or mux2 */
    if (CKGEN->CLK_OUT_CFG & CKGEN_CLK_OUT_CFG_MUX2_Msk)  /* Clock out from MUX2 */
    {
        clockSource = ((CKGEN->CLK_OUT_CFG & CKGEN_CLK_OUT_CFG_MUX2_Msk) >> CKGEN_CLK_OUT_CFG_MUX2_Pos) + CKGEN_CLK_OUT_MUX2_BASE;
    }
    else  /* Clock out from MUX1 */
    {
        clockSource = (CKGEN->CLK_OUT_CFG & CKGEN_CLK_OUT_CFG_MUX1_Msk) >> CKGEN_CLK_OUT_CFG_MUX1_Pos;
    }

    return clockSource;
}

/*!
 * @brief Get clock out source from mux1.
 *
 * @param[in] base: Register base address for the CKGEN
 * @return clock out source, refer to clock_out_source_t
 */
uint32_t CKGEN_GetClockoutSourceFromMux1(const CKGEN_Type *base)
{
    uint32_t clockSource = 0U;

    /* Clock out from MUX1 */
    clockSource = ((CKGEN->CLK_OUT_CFG & CKGEN_CLK_OUT_CFG_MUX2_Msk) >> CKGEN_CLK_OUT_CFG_MUX2_Pos) + CKGEN_CLK_OUT_MUX2_BASE;

    return clockSource;
}

/*!
 * @brief Gets current system clock source.
 *
 * This function gets the current system clock source.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return Current system clock source.
 */
uint32_t CKGEN_GetCurrentSystemClockSource(const CKGEN_Type *base)
{
    uint32_t currentClockSource = 0U;

    if (POWER_MODE_RUN == SPM_GetPowerMode())
    {
        currentClockSource = ((CKGEN->CTRL & CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk) >> CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos);
    }
    else
    {
        currentClockSource = ((CKGEN->CTRL & CKGEN_CTRL_SYSCLK_SRC_SEL_VLPR_Msk) >> CKGEN_CTRL_SYSCLK_SRC_SEL_VLPR_Pos);
    }

    return currentClockSource;
}

/*!
 * @brief Gets current Bus Clock Divide Ratio.
 *
 * This function gets the Current Bus Clock Divide Ratio.
 *
 * @param[in] base: Register base address for the CKGEN instance
 * @return Current Bus Clock Divide Ratio.
 */
uint32_t CKGEN_GetCurrentBusClockDividerRatio(const CKGEN_Type *base)
{
    uint32_t currentBusDivider;

    if (POWER_MODE_RUN == SPM_GetPowerMode())
    {
        currentBusDivider = (CKGEN->CTRL & CKGEN_CTRL_BUS_CLK_DIV_RUN_Msk) >> CKGEN_CTRL_BUS_CLK_DIV_RUN_Pos;
    }
    else
    {
       currentBusDivider = (CKGEN->CTRL & CKGEN_CTRL_BUS_CLK_DIV_VLPR_Msk) >> CKGEN_CTRL_BUS_CLK_DIV_VLPR_Pos;
    }

    return currentBusDivider;
}

/*!
 * @brief Set CAN function clock source.
 *
 * @param[in] instance: CAN module instance
 * @param[in] source: CAN clock source source, value can be
 *                - PERI_CLK_OFF (no clock)
 *                - HSEDIV2_CLK
 *                - CORE_CLK
 *                - SPLLDIV2_CLK
 * @return none
 */
void CKGEN_SetCANClock(uint8_t instance, clock_names_t source)
{
    uint32_t bitFileVal = 0U;
    uint32_t mask = CKGEN_PERI_CLK_MUX1_CAN0_MUX_Msk, pos = CKGEN_PERI_CLK_MUX1_CAN0_MUX_Pos;

    /* Get CAN clock source register bit filed value */
    switch (source)
    {
    case HSEDIV2_CLK:
        bitFileVal = 1U;
        break;

    case CORE_CLK:
        bitFileVal = 2U;
        break;

    case SPLLDIV2_CLK:
        bitFileVal = 3U;
        break;

    default:
        bitFileVal = 0U;
        break;
    }

    /* Set CAN function clock source */
    pos = instance * 2U;
    mask = mask << pos;
    MODIFY_REG32(CKGEN->PERI_CLK_MUX1, mask, pos, bitFileVal);
}

/*!
 * @brief Get CAN function clock source.
 *
 * @param[in] instance: CAN module instance
 * @return CAN clock source source, value can be
 *                - PERI_CLK_OFF (no clock)
 *                - HSEDIV2_CLK
 *                - CORE_CLK
 *                - SPLLDIV2_CLK
 */
clock_names_t CKGEN_GetCANClock(uint8_t instance)
{
    uint32_t bitFileVal = 0U;
    clock_names_t source = PERI_CLK_OFF;

    bitFileVal = (CKGEN->PERI_CLK_MUX1 >> (instance * 2U)) & CKGEN_PERI_CLK_MUX1_CAN0_MUX_Msk;

    /* Get CAN clock source */
    switch (bitFileVal)
    {
    case CAN_CLK_SRC_HSEDIV2:
        source = HSEDIV2_CLK;
        break;

    case CAN_CLK_SRC_SYS_CLK:
        source = CORE_CLK;
        break;

    case CAN_CLK_SRC_SPLLDIV2:
        source = SPLLDIV2_CLK;
        break;

    default:
        source = PERI_CLK_OFF;
        break;
    }

    return source;
}

/*!
 * @brief Set CAN function clock divider.
 *
 * @param[in] instance: CAN module instance
 * @param[in] divider: CAN clock divider, value can be
 *                - CAN_CLK_DIV_1
 *                - CAN_CLK_DIV_2
 * @return none
 */
void CKGEN_SetCANClockDivider(uint8_t instance, can_clock_divider_t divider)
{
    switch (instance)
    {
    case 0U:
        MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_CAN0_DIV_Msk, CKGEN_PERI_CLK_DIV_CAN0_DIV_Pos, (uint32_t)divider);
        break;

    case 1U:
        MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_CAN1_DIV_Msk, CKGEN_PERI_CLK_DIV_CAN1_DIV_Pos, (uint32_t)divider);
        break;

    case 2U:
        MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_CAN2_DIV_Msk, CKGEN_PERI_CLK_DIV_CAN2_DIV_Pos, (uint32_t)divider);
        break;

    case 3U:
        MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_CAN3_DIV_Msk, CKGEN_PERI_CLK_DIV_CAN3_DIV_Pos, (uint32_t)divider);
        break;

    default:
        break;
    }
}

/*!
 * @brief Get CAN function clock divider.
 *
 * @param[in] instance: CAN module instance
 * @return CAN clock divider, value can be
 *                - CAN_CLK_DIV_1
 *                - CAN_CLK_DIV_2
 */
can_clock_divider_t CKGEN_GetCANClockDivider(uint8_t instance)
{
    uint32_t regVal = CKGEN->PERI_CLK_DIV;
    can_clock_divider_t divider = CAN_CLK_DIV_1;

    switch (instance)
    {
    case 0U:
        divider = (can_clock_divider_t)((regVal & CKGEN_PERI_CLK_DIV_CAN0_DIV_Msk) >> CKGEN_PERI_CLK_DIV_CAN0_DIV_Pos);
        break;

    case 1U:
        divider = (can_clock_divider_t)((regVal & CKGEN_PERI_CLK_DIV_CAN1_DIV_Msk) >> CKGEN_PERI_CLK_DIV_CAN1_DIV_Pos);
        break;

    case 2U:
        divider = (can_clock_divider_t)((regVal & CKGEN_PERI_CLK_DIV_CAN2_DIV_Msk) >> CKGEN_PERI_CLK_DIV_CAN2_DIV_Pos);
        break;

    case 3U:
        divider = (can_clock_divider_t)((regVal & CKGEN_PERI_CLK_DIV_CAN3_DIV_Msk) >> CKGEN_PERI_CLK_DIV_CAN3_DIV_Pos);
        break;

    default:
        break;
    }

    return divider;
}

/*!
 * @brief Set the CAN time stamp external clock divider.
 *
 * @param[in] instance: CAN module instance
 * @param[in] divider: CAN time stamp clock divider, value can be
 *                - CAN_TIME_STAMP_CLK_DIV_1
 *                - CAN_TIME_STAMP_CLK_DIV_2
 *                - ...
 *                - CAN_TIME_STAMP_CLK_DIV_8
 * @return none
 */
void CKGEN_SetCANTimeStampClkDiv(uint8_t instance, can_time_stamp_clk_div_t divider)
{
    switch (instance)
    {
    case 0U:
        MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_CAN0_TS_DIV_Msk, CKGEN_PERI_CLK_DIV_CAN0_TS_DIV_Pos, (uint32_t)divider);
        break;

    case 1U:
        MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_CAN1_TS_DIV_Msk, CKGEN_PERI_CLK_DIV_CAN1_TS_DIV_Pos, (uint32_t)divider);
        break;

    case 2U:
        MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_CAN2_TS_DIV_Msk, CKGEN_PERI_CLK_DIV_CAN2_TS_DIV_Pos, (uint32_t)divider);
        break;

    case 3U:
        MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_CAN3_TS_DIV_Msk, CKGEN_PERI_CLK_DIV_CAN3_TS_DIV_Pos, (uint32_t)divider);
        break;

    default:
        break;
    }
}

/*!
 * @brief Set low power function clock.
 *
 * @param[in] rtcSel: RTC clock source select, value can be
 *                - RTC_CLK_SEL_HSEDIV1
 *                - RTC_CLK_SEL_VHSIDIV1
 *                - RTC_CLK_SEL_LSI_32K
 *                - RTC_CLK_SEL_CLKIN
 * @param[in] lsiSel: LSI clock source select, value can be
 *                - LSI_CLK_SEL_128K
 *                - LSI_CLK_SEL_32K
 *                - LSI_CLK_SEL_1K
 *                - LSI_CLK_SEL_RSV
 * @return none
 */
void CKGEN_SetRTCLSIClock(rtc_clock_select_t rtcSel, lsi_clock_select_t lsiSel)
{
    DEVICE_ASSERT(rtcSel <= RTC_CLK_SEL_CLKIN);
    DEVICE_ASSERT(lsiSel <= LSI_CLK_SEL_RSV);

    /* because lsiclk register is write-once, so rtc&lsi clock select need to be configured at the same time */
    MODIFY_REG32(CKGEN->LP_CLK_MUX, \
                 CKGEN_LP_CLK_MUX_LSI_CLK_MUX_Msk | CKGEN_LP_CLK_MUX_RTC_CLK_MUX_Msk, \
                 CKGEN_LP_CLK_MUX_LSI_CLK_MUX_Pos, \
                 (uint32_t)(((uint32_t)rtcSel << CKGEN_LP_CLK_MUX_RTC_CLK_MUX_Pos) | (uint32_t)lsiSel));
}

/*!
 * @brief Get RTC function clock.
 *
 * @param[in] base: CKGEN module base address pointer
 * @return RTC clock source
 */
uint32_t CKGEN_GetRTCClock(const CKGEN_Type *base)
{
    return (uint32_t)(READ_BIT32(base->LP_CLK_MUX, CKGEN_LP_CLK_MUX_RTC_CLK_MUX_Msk) >> CKGEN_LP_CLK_MUX_RTC_CLK_MUX_Pos);
}

/*!
 * @brief Get LSI function clock.
 *
 * @param[in] base: CKGEN module base address pointer
 * @return none
 */
uint32_t CKGEN_GetLSIClock(const CKGEN_Type *base)
{
    return (uint32_t)(READ_BIT32(base->LP_CLK_MUX, CKGEN_LP_CLK_MUX_LSI_CLK_MUX_Msk) >> CKGEN_LP_CLK_MUX_LSI_CLK_MUX_Pos);
}

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
void CKGEN_SetPWMClock(uint32_t instance, clock_names_t source)
{
    uint8_t extSrcEn = 0U;
    uint32_t src = 0U, bitFiledVal = 0U;
    uint32_t regVal = CKGEN->PERI_CLK_MUX3; /* Read register value */
    uint32_t bitFiledMask = CKGEN_PERI_CLK_MUX3_PWM0_INT_Msk \
                              | CKGEN_PERI_CLK_MUX3_PWM0_EXT_IN_Msk \
                              | CKGEN_PERI_CLK_MUX3_PWM0_EXT_Msk;
    uint32_t bitFiledWidth = 5U;

    if ((source >= HSEDIV1_CLK) && (source < SPLLDIV1_CLK)) /* internal source */
    {
        extSrcEn = 0U;
        src = (uint32_t)((uint32_t)source - (uint32_t)HSEDIV1_CLK);
        bitFiledVal = (extSrcEn << CKGEN_PERI_CLK_MUX3_PWM0_EXT_IN_Pos) \
                       | (src << CKGEN_PERI_CLK_MUX3_PWM0_INT_Pos);
    }
    else if ((source >= TCLK0_CLK) && (source <= TCLK3_CLK)) /* external source */
    {
        extSrcEn = 1U;
        src = (uint32_t)((uint32_t)source - (uint32_t)TCLK0_CLK);
        bitFiledVal = (extSrcEn << CKGEN_PERI_CLK_MUX3_PWM0_EXT_IN_Pos) \
                       | (src << CKGEN_PERI_CLK_MUX3_PWM0_EXT_Pos);
    }
    else
    {
        extSrcEn = 0U;  /* default use internal HSE_DIV1 CLK */
        src = 0U;
        bitFiledVal = (extSrcEn << CKGEN_PERI_CLK_MUX3_PWM0_EXT_IN_Pos) \
               | (src << CKGEN_PERI_CLK_MUX3_PWM0_INT_Pos);
    }

    bitFiledVal <<= (bitFiledWidth * instance);
    regVal &= ~(bitFiledMask << (bitFiledWidth * instance));
    regVal |= bitFiledVal;

    /* Write register value */
    CKGEN->PERI_CLK_MUX3 = regVal;
}

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
void CKGEN_GetPWMClock(uint32_t instance, uint8_t *extSrcEnable, uint8_t *internalSrc, uint8_t *extSrc)
{
    *extSrcEnable = (CKGEN->PERI_CLK_MUX3 >> (instance * 5U + CKGEN_PERI_CLK_MUX3_PWM0_EXT_Pos)) & CKGEN_PERI_CLK_MUX3_PWM0_EXT_Msk;
    *extSrc = (CKGEN->PERI_CLK_MUX3 >> (instance * 5U + CKGEN_PERI_CLK_MUX3_PWM0_EXT_Pos)) & CKGEN_PERI_CLK_MUX3_PWM0_EXT_Msk;
    *internalSrc = (CKGEN->PERI_CLK_MUX3 >> (instance * 5U + CKGEN_PERI_CLK_MUX3_PWM0_INT_Pos)) & CKGEN_PERI_CLK_MUX3_PWM0_INT_Msk;
}

/*!
 * @brief set TPIU clock divider.
 *
 * @param[in] div: 0~15, means divider is 1~16
 * @return none
 */
void CKGEN_SetTPIUClkDiv(uint32_t div)
{
    MODIFY_REG32(CKGEN->PERI_CLK_DIV, CKGEN_PERI_CLK_DIV_TPIU_DIV_Msk, CKGEN_PERI_CLK_DIV_TPIU_DIV_Pos, div);
}

/*!
 * @brief set peripheral(except can,pwm,rtc) function clock.
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
 *                - PERI_CLK_OFF
 *                - HSEDIV2_CLK
 *                - HSIDIV2_CLK
 *                - VHSIDIV2_CLK
 *                - SPLLDIV2_CLK
 * @return none
 */
void CKGEN_SetPeripheralClock(clock_names_t peripheralName, clock_names_t source)
{
    uint32_t periphName = (uint32_t)((uint32_t)peripheralName - (uint32_t)CKGEN_PERI_CLK_BASE);
    uint32_t bitFileVal = 0U;
    uint32_t periphPos = 0U;

    /* Ger clock source bit filed value */
    if (PERI_CLK_OFF == source)
    {
        bitFileVal = 0U;
    }
    else
    {
        bitFileVal = (uint32_t)((uint32_t)source - (uint32_t)HSEDIV2_CLK + 4U);
    }

    /* Set peripheral function clock */
    if (periphName < 8U)  /* MUX0: I2C, TIMER, SPI and ADC clock source */
    {
        periphPos = (uint32_t)((periphName % CKGEN_PERI_CLK_MUX_MODULE_NUM) * CKGEN_PERI_CLK_MUX_WIDTH);
        MODIFY_REG32(CKGEN->PERI_CLK_MUX0, (CKGEN_PERI_CLK_MUX_MASK << periphPos), periphPos, bitFileVal);
    }
    else if (periphName < 16U) /* MUX1: PCT and EIO clock source */
    {
        periphPos = (uint32_t)(((periphName - 3U) % CKGEN_PERI_CLK_MUX_MODULE_NUM) * CKGEN_PERI_CLK_MUX_WIDTH);
        MODIFY_REG32(CKGEN->PERI_CLK_MUX1, CKGEN_PERI_CLK_MUX_MASK << periphPos, periphPos, bitFileVal);
    }
    else if (periphName < 32U)  /* MUX2: UART clock source */
    {
        periphPos = (uint32_t)((periphName % CKGEN_PERI_CLK_MUX_MODULE_NUM) * CKGEN_PERI_CLK_MUX_WIDTH);
        MODIFY_REG32(CKGEN->PERI_CLK_MUX2, CKGEN_PERI_CLK_MUX_MASK << periphPos, periphPos, bitFileVal);
    }
    else
    {
        /* do nothing */
    }
}

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
uint8_t CKGEN_GetPeripheralClock(clock_names_t peripheralName)
{
    uint32_t periphName = (uint32_t)((uint32_t)peripheralName - (uint32_t)CKGEN_PERI_CLK_BASE);
    uint32_t periphPos = 0U;
    uint8_t clkSrc = 0U;

    if (periphName < 8U)
    {
        periphPos = ((periphName % CKGEN_PERI_CLK_MUX_MODULE_NUM) * CKGEN_PERI_CLK_MUX_WIDTH);
        clkSrc = (CKGEN->PERI_CLK_MUX0 >> periphPos) & CKGEN_PERI_CLK_MUX_MASK;
    }
    else if (periphName < 16U)
    {
        periphPos = (((periphName - 3U) % CKGEN_PERI_CLK_MUX_MODULE_NUM) * CKGEN_PERI_CLK_MUX_WIDTH);
        clkSrc = (CKGEN->PERI_CLK_MUX1 >> periphPos) & CKGEN_PERI_CLK_MUX_MASK;
    }
    else if (periphName < 32U)
    {
        periphPos = ((periphName % CKGEN_PERI_CLK_MUX_MODULE_NUM) * CKGEN_PERI_CLK_MUX_WIDTH);
        clkSrc = (CKGEN->PERI_CLK_MUX2 >> periphPos) & CKGEN_PERI_CLK_MUX_MASK;
    }
    else
    {
        /* do nothing */
    }

    return clkSrc;
}

/* =============================================  EOF  ============================================== */
