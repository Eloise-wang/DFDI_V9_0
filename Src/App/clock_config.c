/*!
* @file clock_config.c
*
* @brief 此文件提供时钟配置集成功能
*
*/

/* ===========================================  Includes  =========================================== */
#include "clock_config.h"
#include "debugout_ac7840x.h"

/* *************************************************************************
 * Configuration structure for peripheral clock configuration 0
 * ************************************************************************* */
/*! @brief 外设时钟配置 0 */
peripheral_clock_config_t peripheralClockConfig0[NUM_OF_PERIPHERAL_CLOCKS_0] = {
    {
        .clockName        = I2C0_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = TIMER_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = SPI0_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = SPI1_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = SPI2_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = ADC0_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = ADC1_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = CAN0_CLK,
        .clkSrc           = CORE_CLK,
        .divider          = DIVIDE_BY_TWO,
    },
    {
        .clockName        = CAN1_CLK,
        .clkSrc           = CORE_CLK,
        .divider          = DIVIDE_BY_TWO,
    },
    {
        .clockName        = CAN2_CLK,
        .clkSrc           = CORE_CLK,
        .divider          = DIVIDE_BY_TWO,
    },
    {
        .clockName        = UART0_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = UART1_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = CAN3_CLK,
        .clkSrc           = CORE_CLK,
        .divider          = DIVIDE_BY_TWO,
    },
    {
        .clockName        = PCT_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = EIO_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = UART0_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = UART1_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = UART2_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = UART3_CLK,
        .clkSrc           = HSEDIV2_CLK,
    },
    {
        .clockName        = PWM0_CLK,
        .clkSrc           = HSIDIV1_CLK,
    },
    {
        .clockName        = PWM1_CLK,
        .clkSrc           = HSIDIV1_CLK,
    },
    {
        .clockName        = PWM2_CLK,
        .clkSrc           = HSIDIV1_CLK,
    },
    {
        .clockName        = PWM3_CLK,
        .clkSrc           = HSIDIV1_CLK,
    },
    {
        .clockName        = PWM4_CLK,
        .clkSrc           = HSIDIV1_CLK,
    },
    {
        .clockName        = PWM5_CLK,
        .clkSrc           = HSIDIV1_CLK,
    },
};

/* *************************************************************************
 * Configuration structure for Clock Configuration 0
 * ************************************************************************* */
/*! @brief 用户配置结构体 clockMan1_InitConfig0 */
clock_manager_user_config_t clockMan1_InitConfig0 = {
    /*! @brief SIRC配置 */
    .ckgenConfig =
    {
        .hsiConfig =
        {
            .initialize       = true,                                          /*!< Initialize */
            .enableInLowPower = true,                                          /*!< Enable in VLPS mode */
            /* HSIDIV */
            .div1             = CKGEN_ASYNC_CLOCK_DIV_BY_1,                    /*!< SIRCDIV1  */
            .div2             = CKGEN_ASYNC_CLOCK_DIV_BY_1,                    /*!< SIRCDIV2  */
        },
        .vhsiConfig =
        {
            .initialize       = true,                                          /*!< Initialize */
            /* VHSIDIV */
            .div1             = CKGEN_ASYNC_CLOCK_DIV_BY_1,                    /*!< FIRCDIV1   */
            .div2             = CKGEN_ASYNC_CLOCK_DIV_BY_1,                    /*!< FIRCDIV2   */
        },
        .rtcConfig =
        {
            .initialize       = true,                                          /*!< Initialize  */
            .rtcClkInFreq     = 0U,                                            /*!< RTC_CLKIN   */
        },
        .hseConfig =
        {
            .initialize       = true,                                          /*!< Initialize */
            .monitor          = CKGEN_XOSC_MONITOR_RESET,                      /*!< XOSC monitor mode. */
            .freq             = CKGEN_HSE_FREQ,                                /*!< Frequency  */
            /* HSECFG */
            .extRef           = CKGEN_HSE_REF_OSC,                             /*!< EREFS       */
            /* HSEDIV */
            .div1             = CKGEN_ASYNC_CLOCK_DIV_BY_1,                    /*!< SOSCDIV1    */
            .div2             = CKGEN_ASYNC_CLOCK_DIV_BY_1,                    /*!< SOSCDIV2    */
        },
        .spllConfig =
        {
            .initialize       = true,                                          /*!< Initialize */
            .lockDetect       = CKGEN_SPLL_LOCK_DETECT_RESET,                  /*!< System PLL lock detect. */
            /* SPLLCFG */
            .prediv           = (uint8_t)CKGEN_SPLL_CLOCK_PREDIV_BY_1,         /*!< PREDIV     */
            .fbkdiv           = (uint8_t)120U,                                 /*!< FBKDIV     */
            .posdiv           = (uint8_t)CKGEN_SPLL_CLOCK_POSDIV_BY_8,
            .src              = (uint8_t)CKGEN_SPLL_REF_HSE,
            /* SPLLDIV */
            .div1             = CKGEN_ASYNC_CLOCK_DIV_BY_2,                    /*!< SPLLDIV1   */
            .div2             = CKGEN_ASYNC_CLOCK_DIV_BY_2,                    /*!< SPLLDIV2   */
        },
        .clockModeConfig =
        {
            .initialize       = true,                                          /*!< Initialize */
            .rccrConfig =              /*!< RCCR - Run Clock Control Register          */
            {
                .src          = CKGEN_SYSTEM_CLOCK_SRC_SPLL,                   /*!< SCS        */
                .divCore      = CKGEN_SYSTEM_CLOCK_DIV_BY_1,                   /*!< DIVCORE    */
                .divBus       = CKGEN_SYSTEM_CLOCK_DIV_BY_2,                   /*!< DIVBUS     */
            },
            .vccrConfig =              /*!< VCCR - VLPR Clock Control Register         */
            {
                .src          = CKGEN_SYSTEM_CLOCK_SRC_HSI,                    /*!< SCS        */
                .divCore      = CKGEN_SYSTEM_CLOCK_DIV_BY_1,                   /*!< DIVCORE    */
                .divBus       = CKGEN_SYSTEM_CLOCK_DIV_BY_2,                   /*!< DIVBUS     */
            },
        },
    },
    .pccConfig =
    {
        .peripheralClocks = peripheralClockConfig0,                            /*!< Peripheral clock control configurations  */
        .count = NUM_OF_PERIPHERAL_CLOCKS_0,                                   /*!< Number of the peripheral clock control configurations  */
    },
    .simConfig =
    {
        .clockOutConfig =              /*!< Clock Out configuration.           */
        {
            .initialize       = true,                                          /*!< Initialize    */
            .enable           = false,
            .source           = CKGEN_CLKOUT_SEL_HSI_CLK,                      /*!< CLKOUTSEL     */
            .divider          = CKGEN_CLKOUT_DIV_BY_2,                         /*!< CLKOUTDIV     */
        },
        .lpClockConfig =              /*!< Low Power Clock configuration.     */
        {
            .initialize       = false,                                          /*!< Initialize    */
            .sourceLsiClk     = LSI_CLK_SEL_128K,                              /*!< LPOCLKSEL     */
            .sourceRtcClk     = RTC_CLK_SEL_VHSIDIV1,                          /*!< RTCCLKSEL     */
        },
        .tclkConfig =                  /*!< TCLK CLOCK configuration. */
        {
            .initialize       = true,                                          /*!< Initialize    */
            .tclkFreq[0U]     = 0UL,                                           /*!< TCLK0         */
            .tclkFreq[1U]     = 0UL,                                           /*!< TCLK1         */
            .tclkFreq[2U]     = 0UL,                                           /*!< TCLK2         */
        },
        .traceClockConfig =            /*!< Debug trace Clock Configuration. */
        {
            .initialize       = true,                                          /*!< Initialize    */
            .divider          = 0U,                                            /*!< TRACEDIV      */
        },
    },
};

/*! @brief 用户配置结构体指针数组 */
clock_manager_user_config_t const * g_clockManConfigsArr[] = {
    &clockMan1_InitConfig0
};
/*! @brief 用户定义回调配置结构体指针数组 */
clock_manager_callback_user_config_t * g_clockManCallbacksArr[] = {(void*)0};

/* ====================================  Functions declaration  ===================================== */


/* ======================================  Functions define  ======================================== */

/*!
* @brief 时钟配置初始化
*
* @param[in] none
* @return none
*/
void ClockConfig_Init(void)
{
    /* Clock system initialization */
    CKGEN_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CKGEN_DRV_UpdateConfiguration(0, CLOCK_MANAGER_POLICY_AGREEMENT);
    
    /* Basic delay for system stabilization */
    uint32_t i = 0;
    for (i = 0; i < 1000000; i++)
    {
        __NOP();
    }
}

/* =============================================  EOF  ============================================== */

