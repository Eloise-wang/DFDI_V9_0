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
 * @file system_ac7840x.c
 *
 * @brief This file provides system clock config integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#if SYSTEM_USE_CKGEN
#include "ckgen_drv.h"
#endif

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;  /* System/core clock */

/* ====================================  Functions declaration  ===================================== */

/* Externals declaration */
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
extern uint32_t __Vectors;
#endif

/* ======================================  Functions define  ======================================== */
/*!
 * @brief Initialize SPLL then set system clock to the SPLL clock
 *        (just support SPLL refer clock 4/8/12/16/30MHz, others need to modify).
 *
 * @param[in] refClk: 0: HSI clock, 1: HSE clock
 * @param[in] freq: SPLL out frequency (16 - 120)
 * @return status 0: success others: error
 */
uint32_t SetSysClkToSPLL(uint8_t refClk, uint8_t freq)
{
    uint32_t ret = 0U, status = 0U, outFreq = freq;
    uint32_t timeout = 100000U, posdiv, fbkdiv, prediv;

    if (outFreq > 120U)
    {
        outFreq = 120U;
    }
    if (outFreq < 16U)
    {
        outFreq = 16U;
    }

    if (refClk)
    {
        MODIFY_REG32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_HSEBYP_Msk, SPM_PWR_MGR_CFG1_XOSC_HSEBYP_Pos, 0U);
        /* XOSC enable */
        SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_HSEEN_Msk);
        do
        {
            status = READ_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_RDY_Msk);
        } while ((!status) && (--timeout != 0U));
        if (0U == timeout)
        {
            ret = 1U;
        }

        /* SPLL configuration outFreq = (srcFreq / (prediv + 1)) * fbkdiv / (posdiv * 2) */
        #if (CKGEN_HSE_FREQ == 30000000U)  /* For HSE frequency 30MHz */
        if (outFreq == 120U)
        {
            posdiv = 5U;
            fbkdiv = 160U;
        }
        else if (outFreq == 96U)
        {
            posdiv = 5U;
            fbkdiv = 128U;
        }
        else if (outFreq == 80U)
        {
            posdiv = 6U;
            fbkdiv = 128U;
        }
        else  /* Output frequency 48MHz */
        {
            posdiv = 10U;
            fbkdiv = 128U;
            outFreq = 48U;
        }
        prediv = 3U;
        #elif (CKGEN_HSE_FREQ == 16000000U)  /* For HSE frequency 16MHz */
        if (outFreq > 64U)
        {
            posdiv = 4U;
            fbkdiv = outFreq;
        }
        else if (outFreq > 32U)
        {
            posdiv = 8U;
            fbkdiv = outFreq * 2U;
        }
        else
        {
            posdiv = 16U;
            fbkdiv = outFreq * 4U;
        }
        prediv = 1U;
        #elif (CKGEN_HSE_FREQ == 12000000U)  /* For HSE frequency 12MHz */
        if (outFreq > 62U)
        {
            posdiv = 6U;
            fbkdiv = outFreq;
        }
        else if (outFreq > 31U)
        {
            posdiv = 12U;
            fbkdiv = outFreq * 2U;
        }
        else
        {
            posdiv = 24U;
            fbkdiv = outFreq * 4U;
        }
        prediv = 0U;
        #elif (CKGEN_HSE_FREQ == 4000000U)  /* For HSE frequency 4MHz */
        if (outFreq > 64U)
        {
            posdiv = 4U;
            fbkdiv = outFreq * 2;
        }
        else if (outFreq > 32U)
        {
            posdiv = 8U;
            fbkdiv = outFreq * 4U;
        }
        else
        {
            posdiv = 16U;
            fbkdiv = outFreq * 8U;
        }
        prediv = 0U;
        #else  /* For default HSE 8MHz */
        if (outFreq > 64U)
        {
            posdiv = 4U;
            fbkdiv = outFreq;
        }
        else if (outFreq > 32U)
        {
            posdiv = 8U;
            fbkdiv = outFreq * 2U;
        }
        else
        {
            posdiv = 16U;
            fbkdiv = outFreq * 4U;
        }
        prediv = 0U;
        #endif
    }
    else
    {
        /* HSI enable */
        SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_HSI_EN_NORMAL_Msk);
        timeout = 1000U;
        do
        {
            status = READ_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_HSI_RDY_Msk);
        } while ((!status) && (--timeout != 0U));
        if (0U == timeout)
        {
            ret = 2U;
        }

        /* For HSI 8MHz */
        if (outFreq > 64U)
        {
            posdiv = 4U;
            fbkdiv = outFreq;
        }
        else if (outFreq > 32U)
        {
            posdiv = 8U;
            fbkdiv = outFreq * 2U;
        }
        else
        {
            posdiv = 16U;
            fbkdiv = outFreq * 4U;
        }
        prediv = 0U;
    }

    if (0U == ret)
    {
        CLEAR_BIT32(CKGEN->CTRL, CKGEN_CTRL_LOCK_Msk);
        /* Configure SPLL */
        if (refClk)
        {
            SET_BIT32(CKGEN->CTRL, CKGEN_CTRL_PLL_REF_SEL_Msk);
        }
        else
        {
            CLEAR_BIT32(CKGEN->CTRL, CKGEN_CTRL_PLL_REF_SEL_Msk);
        }
        MODIFY_REG32(ANA->SPLL_CFG0, ANA_SPLL_CFG0_POSDIV_Msk, ANA_SPLL_CFG0_POSDIV_Pos, posdiv);
        MODIFY_REG32(ANA->SPLL_CFG0, ANA_SPLL_CFG0_FBKDIV_Msk, ANA_SPLL_CFG0_FBKDIV_Pos, fbkdiv);
        MODIFY_REG32(ANA->SPLL_CFG0, ANA_SPLL_CFG0_PREDIV_Msk, ANA_SPLL_CFG0_PREDIV_Pos, prediv);
        MODIFY_REG32(ANA->SPLL_CFG1, ANA_SPLL_CFG1_LD_DLY_SEL_Msk, ANA_SPLL_CFG1_LD_DLY_SEL_Pos, 3U);
        /* SPLL enable */
        SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_SPLL_EN_Msk);
        timeout = 10000U;
        do
        {
            status = READ_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_SPLL_RDY_Msk);
        } while ((!status) && (--timeout != 0U));
        if (0U == timeout)
        {
            ret = 3U;
        }
    }

    if (0U == ret)
    {
        /* Unlock and set flash clock frequency */
        WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY1);
        WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY2);
        MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_CLKFREQ_Msk, FLASH_CNFG_CLKFREQ_Pos, outFreq);
        /* Switch system clock to spll */
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos, 3U);
        /* Set divider, default sysdiv is 1 divider and busdiv is 2 divider */
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_SYSCLK_DIV_Msk, CKGEN_CTRL_SYSCLK_DIV_Pos, 0U);
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_BUS_CLK_DIV_RUN_Msk, CKGEN_CTRL_BUS_CLK_DIV_RUN_Pos, 1U);
        /* Check current system clock */
        if (3U != ((CKGEN->CTRL & CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk) >> CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos))
        {
            ret = 4U;
        }
    }

    return ret;
}

/*!
 * @brief Initialize VHSI then set system clock to the VHSI clock.
 *
 * @param[in] none
 * @return status 0: success others: error
 */
uint32_t SetSysClkToVHSI(void)
{
    uint32_t ret = 0U, status = 0U, timeout = 100U;

    /* VHSI enable */
    SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_VHSI_EN_Msk);
    do
    {
        status = READ_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_VHSI_RDY_Msk);
    } while ((!status) && (--timeout != 0U));
    if (0U == timeout)
    {
        ret = 1U;
    }

    if (0U == ret)
    {
        CLEAR_BIT32(CKGEN->CTRL, CKGEN_CTRL_LOCK_Msk);
        /* Unlock and set flash clock frequency */
        WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY1);
        WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY2);
        MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_CLKFREQ_Msk, FLASH_CNFG_CLKFREQ_Pos, 48U);
        /* Switch system clock to vhsi */
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos, 0U);
        /* Set divider, default sysdiv is 1 divider and busdiv is 2 divider */
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_SYSCLK_DIV_Msk, CKGEN_CTRL_SYSCLK_DIV_Pos, 0U);
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_BUS_CLK_DIV_RUN_Msk, CKGEN_CTRL_BUS_CLK_DIV_RUN_Pos, 1U);
        /* Check current system clock */
        if (0U != ((CKGEN->CTRL & CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk) >> CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos))
        {
            ret = 2U;
        }
    }

    return ret;
}

/*!
 * @brief Initialize HSE then set system clock to the HSE clock.
 *
 * @param[in] bypass: 0: disable, 1: enable
 * @return status 0: success others: error
 */
uint32_t SetSysClkToHSE(uint8_t bypass)
{
    uint32_t ret = 0U, status = 0U, timeout = 100000U;

    if (bypass)
    {
        SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_HSEBYP_Msk);
    }
    else
    {
        CLEAR_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_HSEBYP_Msk);
    }
    /* XOSC enable */
    SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_HSEEN_Msk);
    do
    {
        status = READ_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_XOSC_RDY_Msk);
    } while ((!status) && (--timeout != 0U));
    if (0U == timeout)
    {
        ret = 1U;
    }

    if (0U == ret)
    {
        CLEAR_BIT32(CKGEN->CTRL, CKGEN_CTRL_LOCK_Msk);
        /* Unlock and set flash clock frequency */
        WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY1);
        WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY2);
        MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_CLKFREQ_Msk, FLASH_CNFG_CLKFREQ_Pos, CKGEN_HSE_FREQ / 1000000U);
        /* Switch system clock to hse */
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos, 1U);
        /* Set divider, default sysdiv is 1 divider and busdiv is 2 divider */
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_SYSCLK_DIV_Msk, CKGEN_CTRL_SYSCLK_DIV_Pos, 0U);
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_BUS_CLK_DIV_RUN_Msk, CKGEN_CTRL_BUS_CLK_DIV_RUN_Pos, 1U);
        /* Check current system clock */
        if (1U != ((CKGEN->CTRL & CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk) >> CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos))
        {
            ret = 2U;
        }
    }

    return ret;
}

/*!
 * @brief Initialize HSI then set system clock to the HSI clock.
 *
 * @param[in] none
 * @return status 0: success others: error
 */
uint32_t SetSysClkToHSI(void)
{
    uint32_t ret = 0U, status = 0U, timeout = 100U;

    /* HSI enable */
    SET_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_HSI_EN_NORMAL_Msk);
    do
    {
        status = READ_BIT32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_HSI_RDY_Msk);
    } while ((!status) && (--timeout != 0U));
    if (0U == timeout)
    {
        ret = 1U;
    }

    if (0U == ret)
    {
        CLEAR_BIT32(CKGEN->CTRL, CKGEN_CTRL_LOCK_Msk);
        /* Unlock and set flash clock frequency */
        WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY1);
        WRITE_REG32(FLASH->KEYUNLK, FLASH_UNLOCK_KEY2);
        MODIFY_REG32(FLASH->CNFG, FLASH_CNFG_CLKFREQ_Msk, FLASH_CNFG_CLKFREQ_Pos, 8U);
        /* Switch system clock to hsi */
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk, CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos, 2U);
        /* Set divider, default sysdiv is 1 divider and busdiv is 2 divider */
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_SYSCLK_DIV_Msk, CKGEN_CTRL_SYSCLK_DIV_Pos, 0U);
        MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_BUS_CLK_DIV_RUN_Msk, CKGEN_CTRL_BUS_CLK_DIV_RUN_Pos, 1U);
        /* Check current system clock */
        if (2U != ((CKGEN->CTRL & CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk) >> CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos))
        {
            ret = 2U;
        }
    }

    return ret;
}

/*!
 * @brief Get MCU CPUID.
 *
 * @param[in] none
 * @return MCU CPUID
 */
uint32_t GetCPUID(void)
{
    return SCB->CPUID;
}

/*!
 * @brief Get UUID from device.
 *
 * @param[out] uuidBuffer: UUID buffer
 * @return none
 */
void GetUUID(uint32_t *uuidBuffer)
{
#define UUID_BASE_ADDRESS  0x00201800U

    uint8_t i = 0U;

    if (uuidBuffer != NULL)
    {
        for (i = 0U; i < 4U; i++)
        {
            uuidBuffer[i] = (*(__IO uint32_t *)(UUID_BASE_ADDRESS  + i * 4U));
        }
    }
}

/*!
 * @brief Setup the microcontroller system. Initialize the System.
 *
 * @param[in] none
 * @return none
 */
void SystemInit(void)
{
    /* Sram L&U ECC read enable */
#if (SRAM_ECC_READ_ENABLE)
    MCM->MLMDR0 |= MCM_MLMDR0_LREEN_Msk | MCM_MLMDR0_UREEN_Msk;
    /* Sram ECC 2bit error reset disable */
#if !(SRAM_ECC_ERR_RST_ENABLE)
    CKGEN->RCM_EN &= ~CKGEN_RCM_EN_ECC2_ERR_RST_EN_Msk;
#endif
#endif

    /* Set CP10 and CP11 Full Access */
#if (__FPU_PRESENT == 1U) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3U << (10U * 2U)) | (3U << (11U * 2U)));
#endif

    /* Disable watchdog */
#if (WDG_DISABLE)
#define WDG_UNLOCK_VALUE1      0xE064D987U
#define WDG_UNLOCK_VALUE2      0x868A8478U
    WDG->CNT = (uint32_t)WDG_UNLOCK_VALUE1;
    WDG->CNT = (uint32_t)WDG_UNLOCK_VALUE2;
    WDG->CS0 &= ~WDG_CS0_EN_Msk;
#endif

    /* Relocate vector table */
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t) &__Vectors;
#endif
}

/*!
 * @brief Update system clock frequence.
 *
 * @param[in] none
 * @return none
 */
void SystemCoreClockUpdate(void)
{
#if SYSTEM_USE_CKGEN
    (void)CKGEN_DRV_GetFreq(CORE_CLK, &SystemCoreClock);
#else
    uint8_t runMode = 0U, clkSrc = 0U, sysDiv = 0U, prediv, fbkdiv, postdiv;
    uint32_t srcFreq = 0U;

    runMode = SPM->STATUS & SPM_STATUS_CURR_POWER_MODE_Msk;
    if (1U == runMode)  /* VLPR mode */
    {
        sysDiv = (CKGEN->CTRL & CKGEN_CTRL_SYSCLK_DIV_VLPR_Msk) >> CKGEN_CTRL_SYSCLK_DIV_VLPR_Pos;
        srcFreq = CKGEN_HSI_FREQ;
    }
    else  /* RUN mode */
    {
        sysDiv = (CKGEN->CTRL & CKGEN_CTRL_SYSCLK_DIV_Msk) >> CKGEN_CTRL_SYSCLK_DIV_Pos;
        clkSrc = (CKGEN->CTRL & CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Msk) >> CKGEN_CTRL_SYSCLK_SRC_SEL_RUN_Pos;
        switch (clkSrc)
        {
        case 1U:  /* HSE clock, user define */
            srcFreq = CKGEN_HSE_FREQ;
            break;

        case 2U:  /* HSI clock */
            srcFreq = CKGEN_HSI_FREQ;
            break;

        case 3U:  /* SPLL clock */
            prediv = (ANA->SPLL_CFG0 & ANA_SPLL_CFG0_PREDIV_Msk) >> ANA_SPLL_CFG0_PREDIV_Pos;
            fbkdiv = (ANA->SPLL_CFG0 & ANA_SPLL_CFG0_FBKDIV_Msk) >> ANA_SPLL_CFG0_FBKDIV_Pos;
            postdiv = (ANA->SPLL_CFG0 & ANA_SPLL_CFG0_POSDIV_Msk) >> ANA_SPLL_CFG0_POSDIV_Pos;
            /* Get reference clock */
            if (0U == ((CKGEN->CTRL & CKGEN_CTRL_PLL_REF_SEL_Msk) >> CKGEN_CTRL_PLL_REF_SEL_Pos))
            {
                srcFreq = CKGEN_HSI_FREQ;
            }
            else
            {
                srcFreq = CKGEN_HSE_FREQ;
            }
            /* Calculate SPLL frequency */
            srcFreq = (srcFreq / (prediv + 1U)) * fbkdiv / (postdiv * 2U);
            break;

        default:  /* VHSI clock */
            srcFreq = CKGEN_VHSI_FREQ;
            break;
        }
    }

    SystemCoreClock = srcFreq / (sysDiv + 1U);
#endif
}

/*!
 * @brief Enable NMI, after set pinmux.
 *
 * @param[in] enable: enable state
 * @return none
 */
void EnableNMI(uint8_t enable)
{
    if (enable)
    {
        SET_BIT32(MCM->MNCR, MCM_MNCR_NMI_EN_Msk);
    }
    else
    {
        CLEAR_BIT32(MCM->MNCR, MCM_MNCR_NMI_EN_Msk);
    }
}

/* =============================================  EOF  ============================================== */
