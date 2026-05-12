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
 * @file ewdg_drv.h
 */

#ifndef _EWDG_DRV_H
#define _EWDG_DRV_H

#if defined(__cplusplus)
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!< Callback typedef for ewdg */
typedef void (*ewdg_callback_t)(void);

/*!
 * @brief EWDG input pin assert logic.
 */
typedef enum
{
    EWDG_IN_ASSERT_DISABLED      = 0x00U,    /*!< Input pin disabled                    */
    EWDG_IN_ASSERT_ON_LOGIC_ZERO = 0x01U,    /*!< Input pin asserts EWDG when on logic 0 */
    EWDG_IN_ASSERT_ON_LOGIC_ONE  = 0x02U     /*!< Input pin asserts EWDG when on logic 1 */
} ewdg_in_assert_logic_t;

/*!
 * @brief EWDG configuration structure.
 *
 * This structure holds the configuration settings for the EWDG
 */
typedef struct
{
    ewdg_in_assert_logic_t assertLogic;  /*!< Assert logic for EWDG input pin */
    bool      interruptEnable;           /*!< Enable EWDG interrupt           */
    uint8_t   prescaler;                 /*!< EWDG clock prescaler            */
    uint8_t   compareLow;                /*!< Compare low value               */
    uint8_t   compareHigh;               /*!< Compare high value              */
    ewdg_callback_t callback;            /*!< Callback function               */
} ewdg_init_config_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Init EWDG and Start
 *
 * @param[in] instance: EWDG instance number
 * @param[in] config: Pointer to the module configuration structure
 * @return the status of init
 *         - STATUS_SUCCESS: Init EWDG successfully
 *         - STATUS_ERROR:   Init EWDG fail
 */
status_t EWDG_DRV_Init(uint32_t instance, const ewdg_init_config_t *config);

/*!
 * @brief De-initialize EWDG driver
 *
 * @param[in] instance: The EWDG instance number
 * @return The result of execution
 *         - STATUS_SUCCESS: operation was successful
 */
status_t EWDG_DRV_Deinit(uint8_t instance);

/*!
 * @brief Get EWDG default config values.
 *
 * @param[out] config: Pointer to the default configuration structure
 * @return none
 */
void EWDG_DRV_GetDefaultConfig(ewdg_init_config_t * config);

/*!
 * @brief Refresh EWDG.
 *
 * @param[in] instance: EWDG instance number
 * @return none
 */
void EWDG_DRV_Refresh(uint32_t instance);

/*!
 * @brief Get the Input pin assert logic.
 *
 * @param[in] instance: EWDG instance number
 * @return The input pin assert logic
 *         - EWDG_IN_ASSERT_DISABLED: EWDG input disabled
 *         - EWDG_INASSERT_ON_LOGIC_ZERO: EWDG is asserted when EWDG_in is logic 0
 *         - EWDG_INASSERT_ON_LOGIC_ONE: EWDG is asserted when EWDG_in is logic 1
 */
ewdg_in_assert_logic_t EWDG_DRV_GetInputPinAssertLogic(uint32_t instance);

/*!
 * @brief EWDG IRQ Handler.
 *
 * @param[in] instance: EWDG instance number
 * @return none
 */
void EWDG_DRV_IRQHandler(uint32_t instance);

#if defined(__cplusplus)
}
#endif

#endif /* _EWDG_DRV_H */

/* =============================================  EOF  ============================================== */
