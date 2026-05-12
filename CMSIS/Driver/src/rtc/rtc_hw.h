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
 * @file rtc_hw.h
 *
 * @brief RTC access register inline function definition.
 *
 */

#ifndef _RTC_HW_H
#define _RTC_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "rtc_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Enable RTC counter.
 *
 * @param[in] base: The RTC base pointer
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: the counter is already enabled
 */
status_t RTC_Enable(RTC_Type * const base);

/*!
 * @brief Disable RTC counter.
 *
 * @param[in] base: The RTC base pointer
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if the counter is not disabled
 */
status_t RTC_Disable(RTC_Type * const base);

/*!
 * @brief Set RTC time counter value.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] timeCnt: 32-bit RTC time counter value
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if setting value error because timer is enabled
 */
status_t RTC_SetTimeCntRegister(RTC_Type * const base, uint32_t timeCnt);

/*!
 * @brief Set RTC prescaler value.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] psrValue: 32-bit RTC prescaler value
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if setting value error because timer is enabled
 */
status_t RTC_SetPrescaler(RTC_Type * const base, uint32_t psrValue);

/*!
 * @brief Configure RTC lock register.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] registerType: RTC lock register type
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if lock register is locked, and lock setting cannot be modified.
 */
status_t RTC_ConfigureLockRegister(RTC_Type * const base, rtc_lock_register_select_t registerType);

/*!
 * @brief Get RTC lock register status.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] reg: RTC lock register type
 * @return True if the register is locked, false if not
 */
bool RTC_IsRegisterLocked(const RTC_Type * const base, rtc_lock_register_select_t reg);

/*!
 * @brief Configure the RTC clkout pin.
 *
 * @param[in] base: The RTC base pointer
 * @param[in] config: RTC clkout pin configuration
 * @return the result of operation
 *         - STATUS_SUCCESS: operation was successful
 *         - STATUS_ERROR: if control register is locked, and cannot enable RTC clkout function
 */
status_t RTC_ConfigureClockOut(RTC_Type * const base, rtc_clk_out_config_t config);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get RTC control register.
 *
 * @param[in] base: RTC base pointer
 * @return control register value
 */
static inline uint32_t RTC_GetCtrlReg(const RTC_Type * const base)
{
    return (base->CTRL);
}

/*!
 * @brief Set RTC Time alarm value.
 *
 * @param[in] base: RTC base pointer
 * @return None
 */
static inline void RTC_SetTimeAlarmReg(RTC_Type * const base, uint32_t alarmCnt)
{
    base->TAR = alarmCnt;
}

/*!
 * @brief Get RTC prescaler value.
 *
 * @param[in] base: RTC base pointer
 * @return RTC prescaler value
 */
static inline uint32_t RTC_GetPrescaler(const RTC_Type * const base)
{
    return (base->PSR);
}

/*!
 * @brief Get RTC time counter value.
 *
 * @param[in] base: RTC base pointer
 * @return RTC time counter value
 */
static inline uint32_t RTC_GetTimeCntReg(const RTC_Type * const base)
{
    return (base->TC);
}

/*!
 * @brief Get RTC prescaler value.
 *
 * @param[in] base: RTC base pointer
 * @return RTC cnt value
 */
static inline uint32_t RTC_GetPrescalerCnt(const RTC_Type * const base)
{
    return (base->PSC);
}

/*!
 * @brief Get RTC Time alarm value.
 *
 * @param[in] base: RTC base pointer
 * @return RTC Time alarm value
 */
static inline uint32_t RTC_GetTimeAlarmReg(const RTC_Type * const base)
{
    return (base->TAR);
}

/*!
 * @brief Enable/disable RTC invalid interrupt.
 *
 * @param[in] base: RTC base pointer
 * @param[in] enable: Enable/disable RTC invalid interrupt
 * @return None
 */
static inline void RTC_SetTIIE(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->CTRL;

    tmp &= ~RTC_CTRL_TIIE_Msk;
    if (true == enable)
    {
        tmp |= RTC_CTRL_TIIE_Msk;
    }
    base->CTRL = tmp;
}

/*!
 * @brief Enable/disable RTC prescaler counter overflow interrupt.
 *
 * @param[in] base: RTC base pointer
 * @param[in] enable: Enable/disable RTC prescaler counter overflow interrupt
 * @return None
 */
static inline void RTC_SetPOIE(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->CTRL;

    tmp &= ~RTC_CTRL_POIE_Msk;
    if (true == enable)
    {
        tmp |= RTC_CTRL_POIE_Msk;
    }
    base->CTRL = tmp;
}

/*!
 * @brief Enable/disable RTC time counter overflow interrupt.
 *
 * @param[in] base: RTC base pointer
 * @param[in] enable: Enable/disable RTC time counter overflow interrupt
 * @return None
 */
static inline void RTC_SetTOIE(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->CTRL;

    tmp &= ~RTC_CTRL_TOIE_Msk;
    if (true == enable)
    {
        tmp |= RTC_CTRL_TOIE_Msk;
    }
    base->CTRL = tmp;
}

/*!
 * @brief Enable/disable RTC time alarm interrupt.
 *
 * @param[in] base: RTC base pointer
 * @param[in] enable: Enable/disable RTC time alarm interrupt
 * @return None
 */
static inline void RTC_SetTAIE(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->CTRL;

    tmp &= ~RTC_CTRL_TAIE_Msk;
    if (true == enable)
    {
        tmp |= RTC_CTRL_TAIE_Msk;
    }
    base->CTRL = tmp;
}

/*!
 * @brief Enable/disable RTC output.
 *
 * @param[in] base: RTC base pointer
 * @param[in] enable: Enable/disable RTC output
 * @return None
 */
static inline void RTC_SetRTCOutput(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->CTRL;

    tmp &= ~RTC_CTRL_COE_Msk;
    if (true == enable)
    {
        tmp |= RTC_CTRL_COE_Msk;
    }
    base->CTRL = tmp;
}

/*!
 * @brief Set RTC update mode of the register when locked.
 *
 * @param[in] base: RTC base pointer
 * @param[in] enable: Enable/disable RTC update
 * @return None
 */
static inline void RTC_SetUpdateMode(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->CTRL;

    tmp &= ~RTC_CTRL_UPM_Msk;
    if (true == enable)
    {
        tmp |= RTC_CTRL_UPM_Msk;
    }
    base->CTRL = tmp;
}

/*!
 * @brief Get RTC update mode of the register when locked.
 *
 * @param[in] base: RTC base pointer
 * @return update mode value
 */
static inline uint32_t RTC_GetUpdateMode(const RTC_Type * const base)
{
    return (base->CTRL & RTC_CTRL_UPM_Msk);
}

/*!
 * @brief Trigger RTC a software reset.
 *
 * @param[in] base: RTC base pointer
 * @return None
 */
static inline void RTC_SoftwareReset(RTC_Type * const base)
{
    base->CTRL |= RTC_CTRL_SWR_Msk;
    base->CTRL &= ~RTC_CTRL_SWR_Msk;
}

/*!
 * @brief Get RTC status register value.
 *
 * @param[in] base: RTC base pointer
 * @return RTC status register value
 */
static inline uint32_t RTC_GetStatusReg(const RTC_Type * const base)
{
    return (base->SR);
}

/*!
 * @brief Enable or disable RTC counter.
 *
 * @param[in] base: RTC base pointer
 * @param[in] enable: Enable/disable rtc counter
 * @return None
 */
static inline void RTC_SetTimeCounterEnable(RTC_Type * const base, bool enable)
{
    uint32_t tmp = base->SR & ~(RTC_SR_POF_Msk);

    tmp &= ~RTC_SR_TCE_Msk;
    if (true == enable)
    {
        tmp |= RTC_SR_TCE_Msk;
    }
    base->SR = tmp;
}

/*!
 * @brief Get RTC counter enable value.
 *
 * @param[in] base: RTC base pointer
 * @return State of the counter enable bit
 *         - true if the counter is enabled
 *         - false if the counter is disabled
 */
static inline bool RTC_GetTimeCounterEnable(const RTC_Type * const base)
{
    return (((base->SR & RTC_SR_TCE_Msk) >> RTC_SR_TCE_Pos) != 0U);
}

/*!
 * @brief Get rtc time alarm flag.
 *
 * @param[in] base: RTC base pointer
 * @return rtc time alarm flag
 */
static inline bool RTC_GetTAF(const RTC_Type * const base)
{
    return (((base->SR & RTC_SR_TAF_Msk) >> RTC_SR_TAF_Pos) != 0U);
}

/*!
 * @brief Get rtc time counter overflow flag.
 *
 * @param[in] base: RTC base pointer
 * @return rtc time counter overflow flag
 */
static inline bool RTC_GetTOF(const RTC_Type * const base)
{
    return (((base->SR & RTC_SR_TOF_Msk) >> RTC_SR_TOF_Pos) != 0U);
}

/*!
 * @brief Get rtc prescaler time counter overflow flag.
 *
 * @param[in] base: RTC base pointer
 * @return rtc time prescaler counter overflow flag
 */
static inline bool RTC_GetPOF(const RTC_Type * const base)
{
    return (((base->SR & RTC_SR_POF_Msk) >> RTC_SR_POF_Pos) != 0U);
}

/*!
 * @brief Clear rtc time prescaler counter overflow flag.
 *
 * @param[in] base: RTC base pointer
 * @return None
 */
static inline void RTC_ClearPOF(RTC_Type * const base)
{
    uint32_t tmp = base->SR;

    tmp |= RTC_SR_POF_Msk;
    base->SR = tmp;
}

/*!
 * @brief Get rtc time invalid flag.
 *
 * @param[in] base: RTC base pointer
 * @return rtc time invalid flag
 */
static inline bool RTC_GetTIF(const RTC_Type * const base)
{
    return (((base->SR & RTC_SR_TIF_Msk) >> RTC_SR_TIF_Pos) != 0U);
}

/*!
 * @brief Lock the lock register.
 *
 * @param[in] base: RTC base pointer
 * @return None
 */
static inline void RTC_LockLR(RTC_Type * const base)
{
    uint32_t tmp = base->LR;

    tmp |= RTC_LR_LRL_Msk;
    base->LR = tmp;
}

/*!
 * @brief Get the lock register lock state.
 *
 * @param[in] base: RTC base pointer
 * @return The status of lock state
 *         - true: register is locked
 *         - false: register is not locked
 */
static inline bool RTC_GetLRL(const RTC_Type * const base)
{
    return (((base->LR & RTC_LR_LRL_Msk) >> RTC_LR_LRL_Pos) != 0U);
}

/*!
 * @brief Lock the status register.
 *
 * @param[in] base: RTC base pointer
 * @return None
 */
static inline void RTC_LockSR(RTC_Type * const base)
{
    uint32_t tmp = base->LR;

    tmp |= RTC_LR_SRL_Msk;
    base->LR = tmp;
}

/*!
 * @brief Get the status register lock state.
 *
 * @param[in] base: RTC base pointer
 * @return The status of lock state
 *         - true: register is locked
 *         - false: register is not locked
 */
static inline bool RTC_GetSRL(const RTC_Type * const base)
{
    return (((base->LR & RTC_LR_SRL_Msk) >> RTC_LR_SRL_Pos) != 0U);
}

/*!
 * @brief Lock the control register.
 *
 * @param[in] base: RTC base pointer
 * @return None
 */
static inline void RTC_LockCTRL(RTC_Type * const base)
{
    uint32_t tmp = base->LR;

    tmp |= RTC_LR_CRL_Msk;
    base->LR = tmp;
}

/*!
 * @brief Get the control register lock state.
 *
 * @param[in] base: RTC base pointer
 * @return The status of lock state
 *         - true: register is locked
 *         - false: register is not locked
 */
static inline bool RTC_GetCRL(const RTC_Type * const base)
{
    return (((base->LR & RTC_LR_CRL_Msk) >> RTC_LR_CRL_Pos) != 0U);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTC_HW_H */

/* =============================================  EOF  ============================================== */
