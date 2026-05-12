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
 * @file system_ac7840x.h
 *
 * @brief This file provides system clock config integration functions interfaces.
 *
 */

#ifndef _SYSTEM_AC7840X_H
#define _SYSTEM_AC7840X_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include <stdint.h>

/* ============================================  Define  ============================================ */
/*!< Sram ECC read enable */
#define SRAM_ECC_READ_ENABLE     (1UL)

/*!< Sram ECC 2bit error reset enable */
#define SRAM_ECC_ERR_RST_ENABLE  (0UL)

/*!< Watchdog disable */
#ifndef WDG_DISABLE
  #define WDG_DISABLE            (1UL)
#endif

/*!< Don't initialize sram after reset except power on, lvr and standby wakeup */
#define NOT_INIT_SRAM_AFTER_RESET (1UL)

/*!< Default system clock */
#define DEFAULT_SYSTEM_CLOCK     (48000000UL)

/*!< Default HSE(XOSC) frequency or user define */
#define CKGEN_HSE_FREQ           (8000000UL)

/*!< System clock uses ckgen driver */
#define SYSTEM_USE_CKGEN         (0UL)

/*!< Global interrupt control */
extern volatile uint8_t s_disIrqReqTimes;
#define EnableInterrupts                                \
        if (s_disIrqReqTimes > 0U) s_disIrqReqTimes--;  \
        if (s_disIrqReqTimes == 0U) __asm("CPSIE i");

#define DisableInterrupts                               \
        if (s_disIrqReqTimes == 0U) __asm("CPSID i");   \
        s_disIrqReqTimes++;

/*!< Global fault control */
extern volatile uint8_t s_disIrqFltTimes;
#define EnableFatules                                   \
        if (s_disIrqFltTimes > 0U) s_disIrqFltTimes--;  \
        if (s_disIrqFltTimes == 0U) __asm("CPSIE F");
#define DisableFatules                                  \
        if (s_disIrqFltTimes == 0U) __asm("CPSID F");   \
        s_disIrqFltTimes++;

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Initialize SPLL then set system clock to the SPLL clock
 *        (just support SPLL refer clock 4/8/12/16/30MHz, others need to modify).
 *
 * @param[in] refClk: 0: HSI clock, 1: HSE clock
 * @param[in] freq: SPLL out frequency (16 - 120)
 * @return status 0: success others: error
 */
uint32_t SetSysClkToSPLL(uint8_t refClk, uint8_t freq);

/*!
 * @brief Initialize VHSI then set system clock to the VHSI clock.
 *
 * @param[in] none
 * @return status 0: success others: error
 */
uint32_t SetSysClkToVHSI(void);

/*!
 * @brief Initialize HSE then set system clock to the HSE clock.
 *
 * @param[in] bypass: 0: disable, 1: enable
 * @return status 0: success others: error
 */
uint32_t SetSysClkToHSE(uint8_t bypass);

/*!
 * @brief Initialize HSI then set system clock to the HSI clock.
 *
 * @param[in] none
 * @return status 0: success others: error
 */
uint32_t SetSysClkToHSI(void);

/*!
 * @brief Get UUID from devie
 *
 * @param[out] uuidBuffer: UUID buffer
 * @return none
 */
void GetUUID(uint32_t *uuidBuffer);

/*!
 * @brief Setup the microcontroller system. Initialize the System.
 *
 * @param[in] none
 * @return none
 */
void SystemInit(void);

/*!
 * @brief Initialize the SRAM for ECC.
 *
 * @param[in] none
 * @return none
 */
void InitSram(void);

/*!
 * @brief Update system clock frequence.
 *
 * @param[in] none
 * @return none
 */
void SystemCoreClockUpdate(void);

/*!
 * @brief Enable NMI, after set pinmux.
 *
 * @param[in] enable: enable state
 * @return none
 */
void EnableNMI(uint8_t enable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SYSTEM_AC7840X_H */

/* =============================================  EOF  ============================================== */
