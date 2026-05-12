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
 * @file ac7840x_features.h
 *
 * @brief This file provides chip specific module features.
 *
 */

#ifndef _AC7840X_FEATURES_H
#define _AC7840X_FEATURES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* =========================================================================================================================== */
/* ================                                            MCM                                            ================ */
/* =========================================================================================================================== */

/* =========================================================================================================================== */
/* ================                                           CKGEN                                           ================ */
/* =========================================================================================================================== */
/*!< HSI clock frequency(8MHz) */
#define CKGEN_HSI_FREQ             (8000000UL)

/*!< VHSI clock frequency(48MHz) */
#define CKGEN_VHSI_FREQ            (48000000UL)

/*!< Auto select HSI clock for SPLL when enable XOSC is fail */
#define CKGEN_AUTO_SEL_HSI         (0UL)

/*!< Auto change SPLL reference clock to HSI clock when XOSC is detected loss(just for XOSC=8MHz);
     Auto change system clock to VHSI clock when SPLL is detected unlock */
#define CKGEN_AUTO_CHANGE_CLK      (0UL)

/*!< Peripheral clock base number */
#define CKGEN_PERI_CLK_BASE        (256UL)

/*!< Clock out mux2 base number */
#define CKGEN_CLK_OUT_MUX2_BASE    (8UL)

/*!
 * @brief Define the enum of the clock names for setting or getting clock frequency.
 */
typedef enum {
 	/* Main clocks */
    CORE_CLK                     = 0U,       /*!< Core/System clock              */
    BUS_CLK                      = 1U,       /*!< Bus clock                      */
    CLKOUT_CLK                   = 2U,       /*!< CLKOUT clock                   */

    /* Other internal clocks used by peripherals. */
    HSI_CLK                      = 3U,       /*!< HSI clock                      */
    VHSI_CLK                     = 4U,       /*!< VHSI clock                     */
    HSE_CLK                      = 5U,       /*!< HSE clock                      */
    SPLL_CLK                     = 6U,       /*!< SPLL clock                     */
    RTC_CLKIN_CLK                = 7U,       /*!< RTC_CLKIN clock                */
    CKGEN_CLKOUT_CLK             = 8U,       /*!< CLK_OUT clock from mux1        */

    HSEDIV1_CLK                  = 9U,       /*!< HSEDIV1 functional clock       */
    HSIDIV1_CLK                  = 10U,      /*!< HSIDIV1 functional clock       */
    VHSIDIV1_CLK                 = 11U,      /*!< VHSIDIV1 functional clock      */
    SPLLDIV1_CLK                 = 12U,      /*!< SPLLDIV1 functional clock      */

    HSEDIV2_CLK                  = 13U,      /*!< HSEDIV2 functional clock       */
    HSIDIV2_CLK                  = 14U,      /*!< HSIDIV2 functional clock       */
    VHSIDIV2_CLK                 = 15U,      /*!< VHSIDIV2 functional clock      */
    SPLLDIV2_CLK                 = 16U,      /*!< SPLLDIV2 functional clock      */

    CKGEN_END_OF_CLOCKS          = 17U,      /*!< End of ckgen clocks            */

    TCLK0_CLK                    = 18U,      /*!< PWM external clock: TCLK0      */
    TCLK1_CLK                    = 19U,      /*!< PWM external clock: TCLK1      */
    TCLK2_CLK                    = 20U,      /*!< PWM external clock: TCLK2      */
    TCLK3_CLK                    = 21U,      /*!< PWM external clock: TCLK3      */

    PWM0_EXT_CLK                 = 22U,      /*!< PWM0 external clock source     */
    PWM1_EXT_CLK                 = 23U,      /*!< PWM1 external clock source     */
    PWM2_EXT_CLK                 = 24U,      /*!< PWM2 external clock source     */
    PWM3_EXT_CLK                 = 25U,      /*!< PWM3 external clock source     */
    PWM4_EXT_CLK                 = 26U,      /*!< PWM4 external clock source     */
    PWM5_EXT_CLK                 = 27U,      /*!< PWM5 external clock source     */

    /* CLK OUT */
    CLKOUTSEL                    = 28U,      /*!< CLKOUT Select fo pin           */

    /* RTC and LSI */
    RTC_CLK                      = 29U,      /*!< RTCCLK clock                   */
    LSI_CLK                      = 30U,      /*!< LSI clock                      */
    LSI_1K_CLK                   = 31U,      /*!< LSI 1KHz clock                 */
    LSI_32K_CLK                  = 32U,      /*!< LSI 32KHz clock                */
    LSI_128K_CLK                 = 33U,      /*!< LSI 128KHz clock               */

    SIM_END_OF_CLOCKS            = 34U,      /*!< End of sim clocks              */

    /* Peripheral clock mux0 */
    I2C0_CLK                     = CKGEN_PERI_CLK_BASE + 0U,      /*!< I2C0 clock source     */
    TIMER_CLK                    = CKGEN_PERI_CLK_BASE + 2U,      /*!< TIMER clock source    */
    SPI0_CLK                     = CKGEN_PERI_CLK_BASE + 3U,      /*!< SPI0 clock source     */
    SPI1_CLK                     = CKGEN_PERI_CLK_BASE + 4U,      /*!< SPI1 clock source     */
    SPI2_CLK                     = CKGEN_PERI_CLK_BASE + 5U,      /*!< SPI2 clock source     */
    ADC0_CLK                     = CKGEN_PERI_CLK_BASE + 6U,      /*!< ADC0 clock source     */
    ADC1_CLK                     = CKGEN_PERI_CLK_BASE + 7U,      /*!< ADC1 clock source     */

    /* Peripheral clock mux1 */
    CAN0_CLK                     = CKGEN_PERI_CLK_BASE + 8U,      /*!< CAN0 clock source     */
    CAN1_CLK                     = CKGEN_PERI_CLK_BASE + 9U,      /*!< CAN1 clock source     */
    CAN2_CLK                     = CKGEN_PERI_CLK_BASE + 10U,     /*!< CAN2 clock source     */
    CAN3_CLK                     = CKGEN_PERI_CLK_BASE + 11U,     /*!< CAN3 clock source     */
    PCT_CLK                      = CKGEN_PERI_CLK_BASE + 14U,     /*!< PCT clock source      */
    EIO_CLK                      = CKGEN_PERI_CLK_BASE + 15U,     /*!< EIO  clock source     */

    /* Peripheral clock mux2 */
    UART0_CLK                    = CKGEN_PERI_CLK_BASE + 16U,     /*!< UART0 clock source    */
    UART1_CLK                    = CKGEN_PERI_CLK_BASE + 17U,     /*!< UART1 clock source    */
    UART2_CLK                    = CKGEN_PERI_CLK_BASE + 18U,     /*!< UART2 clock source    */
    UART3_CLK                    = CKGEN_PERI_CLK_BASE + 19U,     /*!< UART3 clock source    */

    /* Peripheral clock mux3 */
    PWM0_CLK                     = CKGEN_PERI_CLK_BASE + 24U,     /*!< PWM0 clock source     */
    PWM1_CLK                     = CKGEN_PERI_CLK_BASE + 25U,     /*!< PWM1 clock source     */
    PWM2_CLK                     = CKGEN_PERI_CLK_BASE + 26U,     /*!< PWM2 clock source     */
    PWM3_CLK                     = CKGEN_PERI_CLK_BASE + 27U,     /*!< PWM3 clock source     */
    PWM4_CLK                     = CKGEN_PERI_CLK_BASE + 28U,     /*!< PWM4 clock source     */
    PWM5_CLK                     = CKGEN_PERI_CLK_BASE + 29U,     /*!< PWM5 clock source     */

    PCC_END_OF_CLOCKS            = CKGEN_PERI_CLK_BASE + 30U,     /*!< End of pcc clocks     */
    PERI_CLK_OFF                 = CKGEN_PERI_CLK_BASE + 255U,    /*!< peripheral clock off  */

    CLOCK_NAME_COUNT             = CKGEN_PERI_CLK_BASE + 84U,     /*!< The total number of entries */
} clock_names_t;

/*!
 * @brief Define the enum of the module for setting module bus clock.
 */
typedef enum
{
    /* PERI_CLK_EN0 */
    CLK_UART0 = 0U,
    CLK_UART1,
    CLK_UART2,
    CLK_UART3,
    CLK_RESERVE4,
    CLK_RESERVE5,
    CLK_SPI0,
    CLK_SPI1,
    CLK_SPI2,
    CLK_I2C0,
    CLK_RESERVE10,
    CLK_PCT,
    CLK_RESERVE12,
    CLK_RESERVE13,
    CLK_RESERVE14,
    CLK_PWM0,
    CLK_PWM1,
    CLK_PWM2,
    CLK_PWM3,
    CLK_PWM4,
    CLK_PWM5,

    /* PERI_CLK_EN1 */
    CLK_RESERVE32 = 32U + 0U,
    CLK_RTC,
    CLK_DMA0,
    CLK_RESERVE35,
    CLK_RESERVE36,
    CLK_GPIO,
    CLK_WDG,
    CLK_EWDG,
    CLK_CRC,
    CLK_CAN0,
    CLK_CAN1,
    CLK_CAN2,
    CLK_CAN3,

    /* PERI_CLK_EN2 */
    CLK_RESERVE64 = 32U + 32U,
    CLK_CTU,
    CLK_RESERVE66,
    CLK_RESERVE67,
    CLK_RESERVE68,
    CLK_RESERVE69,
    CLK_RESERVE70,
    CLK_RESERVE71,
    CLK_ACMP0,
    CLK_PDT0,
    CLK_PDT1,
    CLK_ADC0,
    CLK_ADC1,
    CLK_TIMER,
    CLK_EIO,
    CLK_RESERVE79,
    CLK_SMU = 83U,
    CLK_MODULE_NUM
} ckgen_clock_t;

/*!
 * @brief Define the enum of the module for setting module at reset state or realse from reset state.
 */
typedef enum
{
    /* PERI_SFT_RST0 */
    SRST_UART0 = 0U,
    SRST_UART1,
    SRST_UART2,
    SRST_UART3,
    SRST_RESERVE4,
    SRST_RESERVE5,
    SRST_SPI0,
    SRST_SPI1,
    SRST_SPI2,
    SRST_I2C0,
    SRST_RESERVE10,
    SRST_PCT,
    SRST_RESERVE12,
    SRST_RESERVE13,
    SRST_RESERVE14,
    SRST_PWM0,
    SRST_PWM1,
    SRST_PWM2,
    SRST_PWM3,
    SRST_PWM4,
    SRST_PWM5,

    /* PERI_SFT_RST1 */
    SRST_RESERVE32 = 32U + 0U,
    SRST_RESERVE33,
    SRST_DMA0,
    SRST_RESERVE35,
    SRST_RESERVE36,
    SRST_GPIO,
    SRST_WDG,
    SRST_EWDG,
    SRST_CRC,
    SRST_CAN0,
    SRST_CAN1,
    SRST_CAN2,
    SRST_CAN3,

    /* PERI_SFT_RST2 */
    SRST_RESERVE64 = 32U + 32U,
    SRST_CTU,
    SRST_RESERVE66,
    SRST_RESERVE67,
    SRST_RESERVE68,
    SRST_RESERVE69,
    SRST_RESERVE70,
    SRST_RESERVE71,
    SRST_ACMP0,
    SRST_PDT0,
    SRST_PDT1,
    SRST_ADC0,
    SRST_ADC1,
    SRST_TIMER,
    SRST_EIO,
    SRST_RESERVE79,
    SRST_MODULE_NUM
} ckgen_softreset_t;

/* =========================================================================================================================== */
/* ================                                            MPU                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the MPU module */
#define MPU_INSTANCE_MAX           (1UL)

/*!< Has process identifier support. */
#define MPU_HAS_PROCESS_IDENTIFIER (1UL)

/*!< Specifies total number of slaves. */
#define MPU_SLAVE_COUNT            (3UL)

/*!< Specifies total number of masters. */
#define MPU_MASTER_COUNT           (3UL)

/*!< The MPU Logical Bus Master Number for core bus master. */
#define MPU_MASTER_CORE            (0UL)
/*!< The MPU Logical Bus Master Number for Debugger master. */
#define MPU_MASTER_DEBUGGER        (1UL)
/*!< The MPU Logical Bus Master Number for DMA master. */
#define MPU_MASTER_DMA             (2UL)

/*!< Specifies master number. */
#define MPU_MASTER                 \
{                                  \
    MPU_MASTER_CORE,               \
    MPU_MASTER_DEBUGGER,           \
    MPU_MASTER_DMA,                \
}

/*!< Array of mpu base addresses */
#define MPU_BASE_PTRS              {MPU}

/* =========================================================================================================================== */
/* ================                                            PBR                                            ================ */
/* =========================================================================================================================== */
/*!
 * @brief PBR master module.
 */
typedef enum
{
    PBR_MASTER_CORE     = 0U,
    PBR_MASTER_DEBUGGER = 1U,
    PBR_MASTER_DMA      = 2U,
    PBR_MASTER_MAX
} pbr_master_t;

/*!
 * @brief PBR peripheral module.
 */
typedef enum
{
    PBR_PWM0 = 0U,
    PBR_PWM1,
    PBR_PWM2,
    PBR_PWM3,
    PBR_PWM4,
    PBR_PWM5,
    PBR_RESERVE6,
    PBR_RESERVE7,

    PBR_CRC = 8U,
    PBR_GPIO,
    PBR_PDT0,
    PBR_PDT1,
    PBR_MPU,
    PBR_EIM,
    PBR_SMU,
    PBR_RESERVE15,

    PBR_CKGEN_RCM = 32U,
    PBR_RESERVE33,
    PBR_FLASH,
    PBR_ADC0,
    PBR_ADC1,
    PBR_ACMP0,
    PBR_CTU,
    PBR_CAN0,

    PBR_CAN1 = 40U,
    PBR_CAN2,
    PBR_CAN3,
    PBR_RESERVE43,
    PBR_RESERVE44,
    PBR_SPM,
    PBR_RTC,
    PBR_EIO,

    PBR_WDG = 48U,
    PBR_EWDG,
    PBR_SPI0,
    PBR_SPI1,
    PBR_SPI2,
    PBR_RESERVE53,
    PBR_I2C0,
    PBR_RESERVE55,

    PBR_TIMER = 64U,
    PBR_DMA,
    PBR_UART0,
    PBR_UART1,
    PBR_UART2,
    PBR_UART3,
    PBR_RESERVE70,
    PBR_RESERVE71,

    PBR_PCT = 72U,
    PBR_CMU,

    PBR_PERI_MAX
} pbr_peripheral_t;

/* =========================================================================================================================== */
/* ================                                            CMU                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the CMU module */
#define CMU_INSTANCE_MAX           (3UL)
/*!< Array of CMU base addresses */
#define CMU_BASE_PTRS              {CMU_VHSI, CMU_HSE, CMU_PLL}

/* =========================================================================================================================== */
/* ================                                            SPM                                            ================ */
/* =========================================================================================================================== */
/*!< Periph sleep ack status define */
#define  SPM_SLEEP_ACK_I2C0        (0x00000001UL)
#define  SPM_SLEEP_ACK_SPI0        (0x00000004UL)
#define  SPM_SLEEP_ACK_SPI1        (0x00000008UL)
#define  SPM_SLEEP_ACK_SPI2        (0x00000010UL)
#define  SPM_SLEEP_ACK_CAN0        (0x00000020UL)
#define  SPM_SLEEP_ACK_CAN1        (0x00000040UL)
#define  SPM_SLEEP_ACK_CAN2        (0x00000080UL)
#define  SPM_SLEEP_ACK_CAN3        (0x00000100UL)
#define  SPM_SLEEP_ACK_UART0       (0x00000800UL)
#define  SPM_SLEEP_ACK_UART1       (0x00001000UL)
#define  SPM_SLEEP_ACK_UART2       (0x00002000UL)
#define  SPM_SLEEP_ACK_UART3       (0x00004000UL)
#define  SPM_SLEEP_ACK_DMA0        (0x00020000UL)
#define  SPM_SLEEP_ACK_EIO         (0x00040000UL)
#define  SPM_SLEEP_ACK_FLASH       (0x00080000UL)

/* =========================================================================================================================== */
/* ================                                            SMU                                            ================ */
/* =========================================================================================================================== */

/* =========================================================================================================================== */
/* ================                                            SRAM                                           ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the SRAM module */
#define SRAM_INSTANCE_COUNT        (1UL)
/*!< Channel Number of SRAM module */
#define SRAM_CHANNEL_MAX           (2UL)
/*!< Array of SRAM IRQs */
#define SRAM_IRQS                  {ECC_SRAM_1BIT_ERROR_IRQn, ECC_SRAM_2BIT_ERROR_IRQn}

/* =========================================================================================================================== */
/* ================                                            EIM                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the EIM module */
#define EIM_INSTANCE_COUNT         (1UL)
/*!< Channel Number of EIM module */
#define EIM_CHANNEL_MAX            (2UL)
/*!< Array of EIM base addresses */
#define EIM_BASE_PTRS              {EIM_CHANNEL0, EIM_CHANNEL1}

/* =========================================================================================================================== */
/* ================                                          FLASH                                            ================ */
/* =========================================================================================================================== */
/*!< FLASH controler unlock key */
#define FLASH_UNLOCK_KEY1          (0xac7840UL)
#define FLASH_UNLOCK_KEY2          (0x01234567UL)
/*!< Based address of P-FLASH */
#define PFLASH_BASE_ADDRESS        (0x00000000UL)
/*!< Based address of D-FLASH */
#define DFLASH_BASE_ADDRESS        (0x01000000UL)
/*!< Based address of FlexRAM area */
#define FLEXRAM_BASE_ADDRESS       (0x14000000UL)
/*!< P-Flash size in byte */
#define PFLASH_BLOCK_SIZE          (1024*1024UL)
/*!< D-Flash size in byte */
#define DFLASH_BLOCK_SIZE          (128*1024UL)
/*!< FlexRAM size in byte */
#define FLEX_RAM_SIZE              (4*1024UL)
/*!< P-Flash page size in byte */
#define PFLASH_PAGE_SIZE           (0x00000800UL)
/*!< D-Flash page size in byte */
#define DFLASH_PAGE_SIZE           (0x00000800UL)
/*!< P-Flash program unit size */
#define PFLASH_WRITE_UNIT_SIZE     (8UL)
/*!< D-Flash program unit size */
#define DFLASH_WRITE_UNIT_SIZE     (8UL)

/* =========================================================================================================================== */
/* ================                                           GPIO                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the GPIO module */
#define GPIO_INSTANCE_MAX          (5UL)
/*!< Array of GPIO base addresses */
#define GPIO_BASE_PTRS             {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE}

/* =========================================================================================================================== */
/* ================                                           PORT                                            ================ */
/* =========================================================================================================================== */
/*!< Array of GPIO port base addresses */
#define GPIO_PORT_BASE_PTRS        {PORTA, PORTB, PORTC, PORTD, PORTE}

/* =========================================================================================================================== */
/* ================                                           CAN                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the CAN module */
#define CAN_INSTANCE_MAX           (4UL)
/*!< Array of CAN base addresses */
#define CAN_BASE_PTRS              {CAN0, CAN1, CAN2, CAN3}
/*!< Array of CAN IRQs */
#define CAN_IRQS                   {CAN0_IRQn, CAN1_IRQn, CAN2_IRQn, CAN3_IRQn}
/*!< Array of CAN wakeup IRQs */
#define CAN_WAKEUP_IRQS            {CAN0_WAKEUP_IRQn, CAN1_WAKEUP_IRQn, CAN2_WAKEUP_IRQn, CAN3_WAKEUP_IRQn}
/*!< Array of CAN ckgen interface clocks */
#define CAN_CKGEN_CLOCKS           {CLK_CAN0, CLK_CAN1, CLK_CAN2, CLK_CAN3}
/*!< Array of CAN soft resets */
#define CAN_SOFT_RESETS            {SRST_CAN0, SRST_CAN1, SRST_CAN2, SRST_CAN3}
/*!< Array of CAN clock names */
#define CAN_CLOCK_NAMES            {CAN0_CLK, CAN1_CLK, CAN2_CLK, CAN3_CLK}
/*!< CAN receive fifo count */
#define CAN_RECEIVE_FIFO_COUNT     (13UL)
/*!< CAN transmit secondary buffer count (6 STB) */
#define CAN_TRANSMIT_FIFO_COUNT    (6UL)
/*!< CAN max filter number */
#define CAN_FILTER_NUM_MAX         (60UL)

/* =========================================================================================================================== */
/* ================                                           UART                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the UART module */
#define UART_INSTANCE_MAX          (4UL)
/*!< Array of UART base addresses */
#define UART_BASE_PTRS             {UART0, UART1, UART2, UART3}
/*!< Array of UART IRQs */
#define UART_IRQS                  {UART0_IRQn, UART1_IRQn, UART2_IRQn, UART3_IRQn}
/*!< Array of UART ckgen interface clocks */
#define UART_CKGEN_CLOCKS          {CLK_UART0, CLK_UART1, CLK_UART2, CLK_UART3}
/*!< Array of UART soft resets */
#define UART_SOFT_RESETS           {SRST_UART0, SRST_UART1, SRST_UART2, SRST_UART3}
/*!< Array of UART clock names */
#define UART_CLOCK_NAMES           {UART0_CLK, UART1_CLK, UART2_CLK, UART3_CLK}

/* =========================================================================================================================== */
/* ================                                           I2C                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the I2C module */
#define I2C_INSTANCE_MAX           (1UL)
/*!< Array of I2C base addresses */
#define I2C_BASE_PTRS              {I2C0}
/*!< Array of I2C IRQs */
#define I2C_IRQS                   {I2C0_IRQn}
/*!< Array of I2C ckgen interface clocks */
#define I2C_CKGEN_CLOCKS           {CLK_I2C0}
/*!< Array of I2C soft resets */
#define I2C_SOFT_RESETS            {SRST_I2C0}
/*!< Array of I2C clock names */
#define I2C_CLOCK_NAMES            {I2C0_CLK}

/* =========================================================================================================================== */
/* ================                                           SPI                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the SPI module */
#define SPI_INSTANCE_MAX           (3UL)
/*!< Array of SPI base addresses */
#define SPI_BASE_PTRS              {SPI0, SPI1, SPI2}
/*!< Array of SPI IRQs */
#define SPI_IRQS                   {SPI0_IRQn, SPI1_IRQn, SPI2_IRQn}
/*!< Array of SPI ckgen interface clocks */
#define SPI_CKGEN_CLOCKS           {CLK_SPI0, CLK_SPI1, CLK_SPI2}
/*!< Array of SPI soft resets */
#define SPI_SOFT_RESETS            {SRST_SPI0, SRST_SPI1, SRST_SPI2}
/*!< Array of SPI clock names */
#define SPI_CLOCK_NAMES            {SPI0_CLK, SPI1_CLK, SPI2_CLK}

/* =========================================================================================================================== */
/* ================                                           EIO                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the EIO module */
#define EIO_INSTANCE_COUNT         (1UL)

/*!< Define the maximum number of shifters for any EIO instance. */
#define EIO_MAX_SHIFTER_COUNT      (4UL)

/*!< Array of EIO IRQs */
#define EIO_IRQS                   {EIO_IRQn}

/*!< Array of EIO clock names */
#define EIO_CLOCK_NAMES            {EIO_CLK}

/*!< Array of EIO base addresses */
#define EIO_BASE_PTRS              {EIO}

/* =========================================================================================================================== */
/* ================                                           CRC                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the CRC module */
#define CRC_INSTANCE_MAX           (1UL)
/*!< Array of CRC base addresses */
#define CRC_BASE_PTRS              {CRC}
/*!< Array of CRC ckgen interface clocks */
#define CRC_CKGEN_CLOCKS           {CLK_CRC}
/*!< Array of CRC soft resets */
#define CRC_SOFT_RESETS            {SRST_CRC}

/* =========================================================================================================================== */
/* ================                                           RTC                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the RTC module */
#define RTC_INSTANCE_MAX           (1UL)
/*!< Array of RTC base addresses */
#define RTC_BASE_PTRS              {RTC}
/*!< Array of RTC IRQs */
#define RTC_IRQS                   {RTC_IRQn}
/*!< Array of RTC ckgen interface clocks */
#define RTC_CKGEN_CLOCKS           {CLK_RTC}

/* =========================================================================================================================== */
/* ================                                           WDG                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the WDG module */
#define WDG_INSTANCE_MAX                          (1UL)
/*!< Array of WDG base addresses */
#define WDG_BASE_PTRS                             {WDG}
/*!< Array of WDG IRQs */
#define WDG_IRQS                                  {WDG_IRQn}
/*!< Array of WDG ckgen interface clocks */
#define WDG_CKGEN_CLOCKS                          {CLK_WDG}
/*!< Array of WDG soft resets */
#define WDG_SOFT_RESETS                           {SRST_WDG}
/* The reset value of the wdg window register */
#define WDG_WIN_RESET_DEFALUT_VALUE               (0x0UL)
/* The reset value of the wdg timeout register */
#define WDG_TIMEOUT_RESET_DEFAULT_VALUE           (0x5000UL)
/* The first 32-bit value used for unlocking the wdg */
#define WDG_UNLOCK_FIRST_VALUE                    (0xE064D987UL)
/* The second 32-bit value used for unlocking the wdg */
#define WDG_UNLOCK_SECOND_VALUE                   (0x868A8478UL)
/* The first 32-bit value used for feed the wdg */
#define WDG_FEED_FIRST_VALUE                      (0x7908AD15UL)
/* The second 32-bit value used for feed the wdg */
#define WDG_FEED_SECOND_VALUE                     (0x5AD5A879UL)
/* The default reset value of WDG CS0 register */
#define WDG_CS0_RESET_VALUE                       (0x20UL)
/* The default reset value of WDG CS1 register */
#define WDG_CS1_RESET_VALUE                       (0x0UL)
/* The default reset value of WDG TOVAL register */
#define WDG_TOVAL_RESET_VALUE                     (0x5000UL)
/* The default reset value of WDG WIN register */
#define WDG_WIN_RESET_VALUE                       (0x0UL)

/* =========================================================================================================================== */
/* ================                                           EWDG                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the EWDG module */
#define EWDG_INSTANCE_MAX          (1UL)
/*!< Array of EWDG IRQs */
#define EWDG_BASE_PTRS             {EWDG}
/*!< Array of EWDG IRQs */
#define EWDG_IRQS                  {EWDG_IRQn}
/*!< Array of EWDG ckgen interface clocks */
#define EWDG_CKGEN_CLOCKS          {CLK_EWDG}
/*!< Array of EWDG soft resets */
#define EWDG_SOFT_RESETS           {SRST_EWDG}
/*!< EWDG refresh key values */
#define EWDG_KEY_FIRST_BYTE        (0xB4UL)
#define EWDG_KEY_SECOND_BYTE       (0x2CUL)
/*!< EWDG CMPH CMPL limit values */
#define EWDG_CMPH_MAX_VALUE        (0xFEUL)
#define EWDG_CMPL_MIN_VALUE        (0x00UL)

/* =========================================================================================================================== */
/* ================                                            DMA                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the DMA module */
#define DMA_INSTANCE_MAX           (1UL)
/*!< Number of channel of the DMA module */
#define DMA_CH_MAX                 (16UL)
/*!< Number of virtual channel of all DMA module */
#define DMA_VIRTUAL_CH_MAX         (DMA_CH_MAX * DMA_INSTANCE_MAX)
/*!< Array of DMA channel base addresses */
#define DMA_VIRTUAL_CH_BASE_PTRS   {DMA0_CHANNEL0, DMA0_CHANNEL1, DMA0_CHANNEL2, DMA0_CHANNEL3, \
                                    DMA0_CHANNEL4, DMA0_CHANNEL5, DMA0_CHANNEL6, DMA0_CHANNEL7, \
                                    DMA0_CHANNEL8, DMA0_CHANNEL9, DMA0_CHANNEL10, DMA0_CHANNEL11, \
                                    DMA0_CHANNEL12, DMA0_CHANNEL13, DMA0_CHANNEL14, DMA0_CHANNEL15}
/*!< Array of DMA ckgen interface clocks */
#define DMA_CKGEN_CLOCKS           {CLK_DMA0}
/*!< Array of DMA soft resets */
#define DMA_SOFT_RESETS            {SRST_DMA0}

/*!
 * @brief DMA request for the DMA channel.
 */
typedef enum
{
    DMA_REQ_DISABLE = 0U,
    DMA_REQ_UART0_RX = 1U,
    DMA_REQ_UART0_TX = 2U,
    DMA_REQ_UART1_RX = 3U,
    DMA_REQ_UART1_TX = 4U,
    DMA_REQ_UART2_RX = 5U,
    DMA_REQ_UART2_TX = 6U,
    DMA_REQ_UART3_RX = 7U,
    DMA_REQ_UART3_TX = 8U,
    DMA_REQ_EIO_SHIFTER0 = 13U,
    DMA_REQ_EIO_SHIFTER1 = 14U,
    DMA_REQ_EIO_SHIFTER2 = 15U,
    DMA_REQ_EIO_SHIFTER3 = 16U,
    DMA_REQ_PWM1_CHANNEL_0 = 17U,
    DMA_REQ_PWM1_CHANNEL_1 = 18U,
    DMA_REQ_PWM1_CHANNEL_2 = 19U,
    DMA_REQ_PWM1_CHANNEL_3 = 20U,
    DMA_REQ_PWM1_CHANNEL_4 = 21U,
    DMA_REQ_PWM1_CHANNEL_5 = 22U,
    DMA_REQ_PWM1_CHANNEL_6 = 23U,
    DMA_REQ_PWM1_CHANNEL_7 = 24U,
    DMA_REQ_PWM1_UNDER_OR_OVER_FLOW = 25U,
    DMA_REQ_PWM2_CHANNEL_0 = 26U,
    DMA_REQ_PWM2_CHANNEL_1 = 27U,
    DMA_REQ_PWM2_CHANNEL_2 = 28U,
    DMA_REQ_PWM2_CHANNEL_3 = 29U,
    DMA_REQ_PWM2_CHANNEL_4 = 30U,
    DMA_REQ_PWM2_CHANNEL_5 = 31U,
    DMA_REQ_PWM2_CHANNEL_6 = 32U,
    DMA_REQ_PWM2_CHANNEL_7 = 33U,
    DMA_REQ_PWM2_UNDER_OR_OVER_FLOW = 34U,
    DMA_REQ_PWM0_OR_CH0_CH7 = 35U,
    DMA_REQ_PWM0_UNDER_OR_OVER_FLOW = 36U,
    DMA_REQ_PWM3_OR_CH0_CH7 = 37U,
    DMA_REQ_PWM3_UNDER_OR_OVER_FLOW = 38U,
    DMA_REQ_PWM4_OR_CH0_CH7 = 39U,
    DMA_REQ_PWM4_UNDER_OR_OVER_FLOW = 40U,
    DMA_REQ_PWM5_OR_CH0_CH7 = 41U,
    DMA_REQ_PWM5_UNDER_OR_OVER_FLOW = 42U,
    DMA_REQ_SPI0_RX = 47U,
    DMA_REQ_SPI0_TX = 48U,
    DMA_REQ_SPI1_RX = 49U,
    DMA_REQ_SPI1_TX = 50U,
    DMA_REQ_SPI2_RX = 51U,
    DMA_REQ_SPI2_TX = 52U,
    DMA_REQ_ADC0 = 53U,
    DMA_REQ_ADC1 = 54U,
    DMA_REQ_I2C0_RX = 55U,
    DMA_REQ_I2C0_TX = 56U,
    DMA_REQ_PORTA = 59U,
    DMA_REQ_PORTB = 60U,
    DMA_REQ_PORTC = 61U,
    DMA_REQ_PORTD = 62U,
    DMA_REQ_PORTE = 63U,
    DMA_REQ_CAN0_RX = 64U,
    DMA_REQ_CAN1_RX = 65U,
    DMA_REQ_CAN2_RX = 66U,
    DMA_REQ_CAN3_RX = 67U,
    DMA_REQ_ALWAYS_ENABLED = 72U
}dma_request_source_t;

/* =========================================================================================================================== */
/* ================                                           ADC                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the ADC module */
#define ADC_INSTANCE_MAX                (2UL)
/*!< Arrays of ADC base address */
#define ADC_BASE_PTRS                   {ADC0, ADC1}
/*!< Arrays of ADC ckgen interface clocks */
#define ADC_CKGEN_CLOCKS                {CLK_ADC0, CLK_ADC1}
/*!< Arrays of ADC soft resets */
#define ADC_SOFT_RESETS                 {SRST_ADC0, SRST_ADC1}
/*!< Array of ADC IRQs */
#define ADC_IRQS                        {ADC0_IRQn, ADC1_IRQn}
/*!< Array of ADC DMA requests */
#define ADC_DMA_REQEUSTS                {DMA_REQ_ADC0, DMA_REQ_ADC1}
/*!< Array of ADC clock names */
#define ADC_CLOCK_NAMES                 {ADC0_CLK, ADC1_CLK}
/*!< Max clock frequence of ADC function clock */
#define ADC_CLOCK_FREQ_MAX_RUNTIME      (30000000UL)

/* =========================================================================================================================== */
/* ================                                           ACMP                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the ACMP module */
#define ACMP_INSTANCE_MAX          (1UL)
/*!< Arrays of ACMP base address */
#define ACMP_BASE_PTRS             {ACMP0}
/*!< Arrays of ACMP ckgen interface clocks */
#define ACMP_CKGEN_CLOCKS          {CLK_ACMP0}
/*!< Arrays of ACMP soft resets */
#define ACMP_SOFT_RESETS           {SRST_ACMP0}
/*!< Array of ACMP IRQs */
#define ACMP_IRQS                  {ACMP0_IRQn}

/* =========================================================================================================================== */
/* ================                                           PWM                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the PWM module */
#define PWM_INSTANCE_MAX           (6UL)
/*!< Array of PWM base addresses */
#define PWM_BASE_PTRS              {PWM0, PWM1, PWM2, PWM3, PWM4, PWM5}
/*!< Array of PWM Overflow IRQs */
#define PWM_OVERFLOW_IRQS          {PWM0_OVERFLOW_IRQn, PWM1_OVERFLOW_IRQn, PWM2_OVERFLOW_IRQn, PWM3_OVERFLOW_IRQn, PWM4_OVERFLOW_IRQn, PWM5_OVERFLOW_IRQn}
/*!< Array of PWM Channel IRQs */
#define PWM_CHANNEL_IRQS           {PWM0_CHANNEL_IRQn, PWM1_CHANNEL_IRQn, PWM2_CHANNEL_IRQn, PWM3_CHANNEL_IRQn, PWM4_CHANNEL_IRQn, PWM5_CHANNEL_IRQn}
/*!< Array of PWM Fault IRQs */
#define PWM_FAULT_IRQS             {PWM0_FAULT_IRQn, PWM1_FAULT_IRQn, PWM2_FAULT_IRQn, PWM3_FAULT_IRQn, PWM4_FAULT_IRQn, PWM5_FAULT_IRQn}
/*!< Array of PWM ckgen interface clocks */
#define PWM_CKGEN_CLOCKS           {CLK_PWM0, CLK_PWM1, CLK_PWM2, CLK_PWM3, CLK_PWM4, CLK_PWM5}
/*!< Array of PWM soft resets */
#define PWM_SOFT_RESETS            {SRST_PWM0, SRST_PWM1, SRST_PWM2, SRST_PWM3, SRST_PWM4, SRST_PWM5}

/* =========================================================================================================================== */
/* ================                                           PDT                                             ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the PDT module */
#define PDT_INSTANCE_MAX           (2UL)
/*!< Array of PDT base addresses */
#define PDT_BASE_PTRS              {PDT0, PDT1}
/*!< Array of PDT IRQs */
#define PDT_IRQS                   {PDT0_IRQn, PDT1_IRQn}
/*!< Array of PDT ckgen interface clocks */
#define PDT_CKGEN_CLOCKS           {CLK_PDT0, CLK_PDT1}
/*!< Array of PDT soft resets */
#define PDT_SOFT_RESETS            {SRST_PDT0, SRST_PDT1}

/* =========================================================================================================================== */
/* ================                                           TIMER                                           ================ */
/* =========================================================================================================================== */
/*!< Channel Number of timer module */
#define TIMER_CHANNEL_MAX          (4UL)
/*!< Number of instances of the timer module */
#define TIMER_INSTANCE_COUNT       (1UL)
/*!< Array of timer base addresses */
#define TIMER_BASE_PTRS            {TIMER_CHANNEL0, TIMER_CHANNEL1, TIMER_CHANNEL2, TIMER_CHANNEL3}
/*!< Array of timer IRQs */
#define TIMER_IRQS                 {TIMER_CHANNEL0_IRQn, TIMER_CHANNEL1_IRQn, TIMER_CHANNEL2_IRQn, TIMER_CHANNEL3_IRQn}
/*!< Array of timer clock names */
#define TIMER_CLOCK_NAMES          {TIMER_CLK}

/* =========================================================================================================================== */
/* ================                                            PCT                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the PCT module */
#define PCT_INSTANCE_COUNT         (1UL)
/*!< Array of PCT base addresses */
#define PCT_BASE_PTRS              {PCT}
/*!< Array of PCT IRQs */
#define PCT_IRQS                   {PCT_IRQn}
/*!< Array of PCT clock names */
#define PCT_CLOCK_NAMES            {PCT_CLK}

/* =========================================================================================================================== */
/* ================                                            CTU                                            ================ */
/* =========================================================================================================================== */
/*!< Number of instances of the CTU module */
#define CTU_INSTANCE_MAX           (1UL)
/*!< Array of CTU base addresses */
#define CTU_BASE_PTRS              {CTU}
/*!< Array of CTU ckgen interface clocks */
#define CTU_CKGEN_CLOCKS           {CLK_CTU}
/*!< Array of CTU soft resets */
#define CTU_SOFT_RESETS            {SRST_CTU}

/*!
 * @brief Enumeration for trigger source of the trgmux.
 */
typedef enum
{
    TRGMUX_TRIG_SOURCE_DISABLE               = 0U,   /*!< Trigger source of disable */
    TRGMUX_TRIG_SOURCE_ENABLE                = 1U,   /*!< Trigger source of enable */
    TRGMUX_TRIG_SOURCE_EXT_IN0               = 2U,   /*!< Trigger source of ext_in0 */
    TRGMUX_TRIG_SOURCE_EXT_IN1               = 3U,   /*!< Trigger source of ext_in1 */
    TRGMUX_TRIG_SOURCE_EXT_IN2               = 4U,   /*!< Trigger source of ext_in2 */
    TRGMUX_TRIG_SOURCE_EXT_IN3               = 5U,   /*!< Trigger source of ext_in3 */
    TRGMUX_TRIG_SOURCE_EXT_IN4               = 6U,   /*!< Trigger source of ext_in4 */
    TRGMUX_TRIG_SOURCE_EXT_IN5               = 7U,   /*!< Trigger source of ext_in5 */
    TRGMUX_TRIG_SOURCE_EXT_IN6               = 8U,   /*!< Trigger source of ext_in6 */
    TRGMUX_TRIG_SOURCE_EXT_IN7               = 9U,   /*!< Trigger source of ext_in7 */
    TRGMUX_TRIG_SOURCE_EXT_IN8               = 10U,  /*!< Trigger source of ext_in8 */
    TRGMUX_TRIG_SOURCE_EXT_IN9               = 11U,  /*!< Trigger source of ext_in9 */
    TRGMUX_TRIG_SOURCE_EXT_IN10              = 12U,  /*!< Trigger source of ext_in10 */
    TRGMUX_TRIG_SOURCE_EXT_IN11              = 13U,  /*!< Trigger source of ext_in11 */
    TRGMUX_TRIG_SOURCE_ACMP0_OUT             = 14U,  /*!< Trigger source of acmp0_out */
    TRGMUX_TRIG_SOURCE_TIMER_CH0             = 16U,  /*!< Trigger source of timer_ch0 */
    TRGMUX_TRIG_SOURCE_TIMER_CH1             = 17U,  /*!< Trigger source of timer_Ch1 */
    TRGMUX_TRIG_SOURCE_TIMER_CH2             = 18U,  /*!< Trigger source of timer_ch2 */
    TRGMUX_TRIG_SOURCE_TIMER_CH3             = 19U,  /*!< Trigger source of timer_ch3 */
    TRGMUX_TRIG_SOURCE_PCT0_TRIG             = 20U,  /*!< Trigger source of pct0_trig */
    TRGMUX_TRIG_SOURCE_PWM0_INIT_TRIG        = 21U,  /*!< Trigger source of pwm0_init_trig */
    TRGMUX_TRIG_SOURCE_PWM0_MATCH_TRIG       = 22U,  /*!< Trigger source of pwm0_match_trig */
    TRGMUX_TRIG_SOURCE_PWM0_MAX_TRIG         = 23U,  /*!< Trigger source of pwm0_max_trig */
    TRGMUX_TRIG_SOURCE_PWM1_INIT_TRIG        = 24U,  /*!< Trigger source of pwm1_init_trig */
    TRGMUX_TRIG_SOURCE_PWM1_MATCH_TRIG       = 25U,  /*!< Trigger source of pwm1_match_trig */
    TRGMUX_TRIG_SOURCE_PWM1_MAX_TRIG         = 26U,  /*!< Trigger source of pwm1_max_trig */
    TRGMUX_TRIG_SOURCE_PWM2_INIT_TRIG        = 27U,  /*!< Trigger source of pwm2_init_trig */
    TRGMUX_TRIG_SOURCE_PWM2_MATCH_TRIG       = 28U,  /*!< Trigger source of pwm2_match_trig */
    TRGMUX_TRIG_SOURCE_PWM2_MAX_TRIG         = 29U,  /*!< Trigger source of pwm2_max_trig */
    TRGMUX_TRIG_SOURCE_PWM3_INIT_TRIG        = 30U,  /*!< Trigger source of pwm3_init_trig */
    TRGMUX_TRIG_SOURCE_PWM3_MATCH_TRIG       = 31U,  /*!< Trigger source of pwm3_match_trig */
    TRGMUX_TRIG_SOURCE_PWM3_MAX_TRIG         = 32U,  /*!< Trigger source of pwm3_max_trig */
    TRGMUX_TRIG_SOURCE_PWM4_INIT_TRIG        = 33U,  /*!< Trigger source of pwm4_init_trig */
    TRGMUX_TRIG_SOURCE_PWM4_MATCH_TRIG       = 34U,  /*!< Trigger source of pwm4_match_trig */
    TRGMUX_TRIG_SOURCE_PWM4_MAX_TRIG         = 35U,  /*!< Trigger source of pwm4_max_trig */
    TRGMUX_TRIG_SOURCE_PWM5_INIT_TRIG        = 36U,  /*!< Trigger source of pwm5_init_trig */
    TRGMUX_TRIG_SOURCE_PWM5_MATCH_TRIG       = 37U,  /*!< Trigger source of pwm5_match_trig */
    TRGMUX_TRIG_SOURCE_PWM5_MAX_TRIG         = 38U,  /*!< Trigger source of pwm5_max_trig */
    TRGMUX_TRIG_SOURCE_ADC0_EOC              = 45U,  /*!< Trigger source of adc0_eoc */
    TRGMUX_TRIG_SOURCE_ADC0_IEOC             = 46U,  /*!< Trigger source of adc0_ieoc */
    TRGMUX_TRIG_SOURCE_ADC0_AMO              = 47U,  /*!< Trigger source of adc0_amo */
    TRGMUX_TRIG_SOURCE_ADC1_EOC              = 48U,  /*!< Trigger source of adc1_eoc */
    TRGMUX_TRIG_SOURCE_ADC1_IEOC             = 49U,  /*!< Trigger source of adc1_ieoc */
    TRGMUX_TRIG_SOURCE_ADC1_AMO              = 50U,  /*!< Trigger source of adc1_amo */
    TRGMUX_TRIG_SOURCE_PDT0_TRIG             = 51U,  /*!< Trigger source of pdt0_trig */
    TRGMUX_TRIG_SOURCE_PDT0_PULSE_OUT        = 52U,  /*!< Trigger source of pdt0_pulse_out */
    TRGMUX_TRIG_SOURCE_PDT1_TRIG             = 53U,  /*!< Trigger source of pdt1_trig */
    TRGMUX_TRIG_SOURCE_PDT1_PULSE_OUT        = 54U,  /*!< Trigger source of pdt1_pulse_out */
    TRGMUX_TRIG_SOURCE_RTC_ALARM_TRIG        = 55U,  /*!< Trigger source of rtc_alarm_trig */
    TRGMUX_TRIG_SOURCE_RTC_PRESCALER_TRIG    = 56U,  /*!< Trigger source of rtc_prescaler_trig */
    TRGMUX_TRIG_SOURCE_EIO_TRIG0             = 57U,  /*!< Trigger source of eio_trig0 */
    TRGMUX_TRIG_SOURCE_EIO_TRIG1             = 58U,  /*!< Trigger source of eio_trig1 */
    TRGMUX_TRIG_SOURCE_EIO_TRIG2             = 59U,  /*!< Trigger source of eio_trig2 */
    TRGMUX_TRIG_SOURCE_EIO_TRIG3             = 60U,  /*!< Trigger source of eio_trig3 */
    TRGMUX_TRIG_SOURCE_SW_TRIG0              = 61U,  /*!< Trigger source of sw_trig0 */
    TRGMUX_TRIG_SOURCE_SW_TRIG1              = 62U,  /*!< Trigger source of sw_trig1 */
    TRGMUX_TRIG_SOURCE_SW_TRIG2              = 63U,  /*!< Trigger source of sw_trig2 */
    TRGMUX_TRIG_SOURCE_SW_TRIG3              = 64U   /*!< Trigger source of sw_trig3 */
} trgmux_trigger_source_t;

/*!
 * @brief Enumeration for target module of the trgmux.
 */
typedef enum
{
    TRGMUX_TARGET_MODULE_DMA_CH0            = 0U,   /*!< Target module of dma_ch0 */
    TRGMUX_TARGET_MODULE_DMA_CH1            = 1U,   /*!< Target module of dma_ch1 */
    TRGMUX_TARGET_MODULE_DMA_CH2            = 2U,   /*!< Target module of dma_ch2 */
    TRGMUX_TARGET_MODULE_DMA_CH3            = 3U,   /*!< Target module of dma_ch3 */
    TRGMUX_TARGET_MODULE_EXT_OUT0           = 4U,   /*!< Target module of ext_out0 */
    TRGMUX_TARGET_MODULE_EXT_OUT1           = 5U,   /*!< Target module of ext_out1 */
    TRGMUX_TARGET_MODULE_EXT_OUT2           = 6U,   /*!< Target module of ext_out2 */
    TRGMUX_TARGET_MODULE_EXT_OUT3           = 7U,   /*!< Target module of ext_out3 */
    TRGMUX_TARGET_MODULE_EXT_OUT4           = 8U,   /*!< Target module of ext_out4 */
    TRGMUX_TARGET_MODULE_EXT_OUT5           = 9U,   /*!< Target module of ext_out5 */
    TRGMUX_TARGET_MODULE_EXT_OUT6           = 10U,  /*!< Target module of ext_out6 */
    TRGMUX_TARGET_MODULE_EXT_OUT7           = 11U,  /*!< Target module of ext_out7 */
    TRGMUX_TARGET_MODULE_ADC0_REGULAR0      = 12U,  /*!< Target module of adc0_regular0 */
    TRGMUX_TARGET_MODULE_ADC0_REGULAR1      = 13U,  /*!< Target module of adc0_regular1 */
    TRGMUX_TARGET_MODULE_ADC0_REGULAR2      = 14U,  /*!< Target module of adc0_regular2 */
    TRGMUX_TARGET_MODULE_ADC0_REGULAR3      = 15U,  /*!< Target module of adc0_regular3 */
    TRGMUX_TARGET_MODULE_ADC0_INJECTION0    = 16U,  /*!< Target module of adc0_injection0 */
    TRGMUX_TARGET_MODULE_ADC0_INJECTION1    = 17U,  /*!< Target module of adc0_injection1 */
    TRGMUX_TARGET_MODULE_ADC0_INJECTION2    = 18U,  /*!< Target module of adc0_injection2 */
    TRGMUX_TARGET_MODULE_ADC0_INJECTION3    = 19U,  /*!< Target module of adc0_injection3 */
    TRGMUX_TARGET_MODULE_ADC1_REGULAR0      = 20U,  /*!< Target module of adc1_regular0 */
    TRGMUX_TARGET_MODULE_ADC1_REGULAR1      = 21U,  /*!< Target module of adc1_regular1 */
    TRGMUX_TARGET_MODULE_ADC1_REGULAR2      = 22U,  /*!< Target module of adc1_regular2 */
    TRGMUX_TARGET_MODULE_ADC1_REGULAR3      = 23U,  /*!< Target module of adc1_regular3 */
    TRGMUX_TARGET_MODULE_ADC1_INJECTION0    = 24U,  /*!< Target module of adc1_injection0 */
    TRGMUX_TARGET_MODULE_ADC1_INJECTION1    = 25U,  /*!< Target module of adc1_injection1 */
    TRGMUX_TARGET_MODULE_ADC1_INJECTION2    = 26U,  /*!< Target module of adc1_injection2 */
    TRGMUX_TARGET_MODULE_ADC1_INJECTION3    = 27U,  /*!< Target module of adc1_injection3 */
    TRGMUX_TARGET_MODULE_ACMP0_TR           = 28U,  /*!< Target module of acmp0_tr */
    TRGMUX_TARGET_MODULE_PWM0_TRIG0         = 32U,  /*!< Target module of pwm0_trig0 */
    TRGMUX_TARGET_MODULE_PWM0_FAULT0        = 33U,  /*!< Target module of pwm0_fault0 */
    TRGMUX_TARGET_MODULE_PWM0_FAULT1        = 34U,  /*!< Target module of pwm0_fault1 */
    TRGMUX_TARGET_MODULE_PWM0_FAULT2        = 35U,  /*!< Target module of pwm0_fault2 */
    TRGMUX_TARGET_MODULE_PWM1_TRIG0         = 36U,  /*!< Target module of pwm1_trig0 */
    TRGMUX_TARGET_MODULE_PWM1_FAULT0        = 37U,  /*!< Target module of pwm1_fault0 */
    TRGMUX_TARGET_MODULE_PWM1_FAULT1        = 38U,  /*!< Target module of pwm1_fault1 */
    TRGMUX_TARGET_MODULE_PWM1_FAULT2        = 39U,  /*!< Target module of pwm1_fault2 */
    TRGMUX_TARGET_MODULE_PWM2_TRIG0         = 40U,  /*!< Target module of pwm2_trig0 */
    TRGMUX_TARGET_MODULE_PWM2_FAULT0        = 41U,  /*!< Target module of pwm2_fault0 */
    TRGMUX_TARGET_MODULE_PWM2_FAULT1        = 42U,  /*!< Target module of pwm2_fault1 */
    TRGMUX_TARGET_MODULE_PWM2_FAULT2        = 43U,  /*!< Target module of pwm2_fault2 */
    TRGMUX_TARGET_MODULE_PWM3_TRIG0         = 44U,  /*!< Target module of pwm3_trig0 */
    TRGMUX_TARGET_MODULE_PWM3_FAULT0        = 45U,  /*!< Target module of pwm3_fault0 */
    TRGMUX_TARGET_MODULE_PWM3_FAULT1        = 46U,  /*!< Target module of pwm3_fault1 */
    TRGMUX_TARGET_MODULE_PWM3_FAULT2        = 47U,  /*!< Target module of pwm3_fault2 */
    TRGMUX_TARGET_MODULE_PWM4_TRIG0         = 48U,  /*!< Target module of pwm4_trig0 */
    TRGMUX_TARGET_MODULE_PWM5_TRIG0         = 52U,  /*!< Target module of pwm5_trig0 */
    TRGMUX_TARGET_MODULE_TIMER_CH0          = 64U,  /*!< Target module of timer_ch0 */
    TRGMUX_TARGET_MODULE_TIMER_CH1          = 65U,  /*!< Target module of timer_ch1 */
    TRGMUX_TARGET_MODULE_TIMER_CH2          = 66U,  /*!< Target module of timer_ch2 */
    TRGMUX_TARGET_MODULE_TIMER_CH3          = 67U,  /*!< Target module of timer_ch3 */
    TRGMUX_TARGET_MODULE_PCT0               = 68U,  /*!< Target module of pct0 */
    TRGMUX_TARGET_MODULE_UART0              = 72U,  /*!< Target module of uart0 */
    TRGMUX_TARGET_MODULE_UART1              = 76U,  /*!< Target module of uart1 */
    TRGMUX_TARGET_MODULE_PDT0               = 80U,  /*!< Target module of pdt0 */
    TRGMUX_TARGET_MODULE_PDT1               = 84U,  /*!< Target module of pdt1 */
    TRGMUX_TARGET_MODULE_EIO_TIMR0          = 88U,  /*!< Target module of eio_timer0 */
    TRGMUX_TARGET_MODULE_EIO_TIMR1          = 89U,  /*!< Target module of eio_timer1 */
    TRGMUX_TARGET_MODULE_EIO_TIMR2          = 90U,  /*!< Target module of eio_timer2 */
    TRGMUX_TARGET_MODULE_EIO_TIMR3          = 91U   /*!< Target module of eio_timer3 */
} trgmux_target_module_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AC7840X_FEATURES_H */

/* =============================================  EOF  ============================================== */
