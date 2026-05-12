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
 * @file pwm_hw.h
 *
 * @brief This file provides pwm hardware integration functions.
 *
 */

#ifndef _PWM_HW_H
#define _PWM_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "pwm_input.h"
#include "pwm_output.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Set pwm pair channel dead time.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] psc: prescaler divider
 *            - PWM_DEADTIME_DIVID_1
 *            - PWM_DEADTIME_DIVID_4
 *            - PWM_DEADTIME_DIVID_16
 * @param[in] value: inserted value
 *            - 0 ~ 1023
 * @return none
 */
void PWM_SetDeadtime(PWM_Type *base, uint8_t pairChannel, pwm_dead_time_psc_type_t psc, uint16_t value);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set pwm overflow event enable.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetOverflowEventEnable(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->INIT, PWM_INIT_COFE_Msk, PWM_INIT_COFE_Pos, state);
}

/*!
 * @brief Set pwm underflow event enable.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetUnderflowEventEnable(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->INIT, PWM_INIT_CUVE_Msk, PWM_INIT_CUVE_Pos, state);
}

/*!
 * @brief Set pwm clock source.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] source: clock source
 *            - PWM_CLK_SOURCE_NONE: No clock selected,in effect disables the PWM counter
 *            - PWM_CLK_SOURCE_SYSTEM: system clock
 *            - PWM_CLK_SOURCE_FIXED_FREQUENCY: fixed frequency clock
 *            - PWM_CLK_SOURCE_EXTERNAL: external clock
 * @return none
 */
static inline void PWM_SetClockSource(PWM_Type *base, pwm_clk_source_t source)
{
    MODIFY_REG32(base->INIT, PWM_INIT_CLKSRC_Msk, PWM_INIT_CLKSRC_Pos, source);
}

/*!
 * @brief Set pwm clock prescaler.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] psc: clock prescaler
 *            - 0 ~ 65535
 * @return none
 */
static inline void PWM_SetClockPrescaler(PWM_Type *base, uint16_t psc)
{
    MODIFY_REG32(base->INIT, PWM_INIT_CLKPSC_Msk, PWM_INIT_CLKPSC_Pos, psc);
}

/*!
 * @brief Set counting mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: counting mode
 *            - PWM_UP_COUNT: Up counting mode
 *            - PWM_UP_DOWN_COUNT: Up-down counting mode
 * @return none
 */
static inline void PWM_SetCountMode(PWM_Type *base, pwm_count_mode_t mode)
{
    MODIFY_REG32(base->INIT, PWM_INIT_CNTMODE_Msk, PWM_INIT_CNTMODE_Pos, mode);
}

/*!
 * @brief Set overflow & underflow interrupt.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetOverflowInterrupt(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->INIT, PWM_INIT_CNTOIE_Msk, PWM_INIT_CNTOIE_Pos, state);
}

/*!
 * @brief Get overflow & underflow flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return none
 */
#define PWM_GetOverflowFlag(base)           READ_BIT32((base)->INIT, PWM_INIT_CNTOF_Msk)

/*!
 * @brief Get overflow & underflow direction.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return counter overflow & underflow direction
 */
#define PWM_GetOverflowDir(base)            READ_BIT32((base)->INIT, PWM_INIT_OFDIR_Msk)

/*!
 * @brief Clear overflow & underflow flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return none
 */
#define PWM_ClearOverflowFlag(base)         CLEAR_BIT32((base)->INIT, PWM_INIT_CNTOF_Msk)

/*!
 * @brief Set overflow & underflow dma enable.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetOverflowDmaEnable(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->INIT, PWM_INIT_OFUFDMAEN_Msk, PWM_INIT_OFUFDMAEN_Pos, state);
}

/*!
 * @brief Set counter value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] value: counter value
 *            - 0 ~ 0xffff
 * @return none
 */
#define PWM_SetCounter(base, value)             WRITE_REG32((base)->CNT, value)

/*!
 * @brief Get counter value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return counter value
 */
#define PWM_GetCounter(base)                    READ_BIT32((base)->CNT, PWM_CNT_COUNT_Msk)

/*!
 * @brief Set max counting value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] value: max counting value
 *            - 0 ~ 0xffff
 * @return none
 */
#define PWM_SetMaxCountValue(base, value)       WRITE_REG32((base)->MCVR, value)

/*!
 * @brief Get max counting value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return max counting value
 */
#define PWM_GetMaxCountValue(base)              READ_BIT32((base)->MCVR, PWM_MCVR_MCVR_Msk)

/*!
 * @brief Set counter initial value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] value: initial counting value
 *            - 0 ~ 0xffff
 * @return none
 */
#define PWM_SetCounterInitValue(base, value)    WRITE_REG32((base)->CNTIN, value)

/*!
 * @brief Get counter initial value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return counter initial value
 */
#define PWM_GetCounterInitValue(base)           READ_BIT32((base)->CNTIN, PWM_CNTIN_CNTINIT_Msk)

/*!
 * @brief Set the channel event reset.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetChannelEventReset(PWM_Type *base, pwm_channel_type_t channel, bool state)
{
    MODIFY_REG32(base->CHANNELS[channel].CHnSCR, PWM_CH0SCR_CHRSTEN_Msk, PWM_CH0SCR_CHRSTEN_Pos, state);
}

/*!
 * @brief Set channel edge level.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] edgeLevel: ELSR1:ELSR0
 *            - 00, 01, 10, 11
 * @return none
 */
static inline void PWM_SetChannelELSR(PWM_Type *base, pwm_channel_type_t channel, uint8_t edgeLevel)
{
    MODIFY_REG32(base->CHANNELS[channel].CHnSCR, PWM_CH0SCR_ELSR0_Msk | PWM_CH0SCR_ELSR1_Msk,
                 PWM_CH0SCR_ELSR0_Pos, (edgeLevel & 0x3U));  /* edgeLevel only need 2bit.so must mask 0x3U */
}

/*!
 * @brief Set channel mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] mode: MSR1:MSR0
 *            - 00, 01, 10, 11
 * @return none
 */
static inline void PWM_SetChannelMSR(PWM_Type *base, pwm_channel_type_t channel, uint8_t mode)
{
    MODIFY_REG32(base->CHANNELS[channel].CHnSCR, PWM_CH0SCR_MSR0_Msk | PWM_CH0SCR_MSR1_Msk,
                 PWM_CH0SCR_MSR0_Pos, (mode & 0x3U));   /* mode only need 2bit.so must mask 0x3U */
}

/*!
 * @brief Set channel match point direction.
 *
 * only works in combine mode & up-down counting mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] dir: match point direction
 *            - PWM_MATCH_DIR_UP: up counting direction
 *            - PWM_MATCH_DIR_DOWN: down counting direction
 * @return none
 */
static inline void PWM_SetChannelMatchDir(PWM_Type *base, pwm_channel_type_t channel, pwm_match_point_dir_type_t dir)
{
    MODIFY_REG32(base->CHANNELS[channel].CHnSCR, PWM_CH0SCR_DIR_Msk, PWM_CH0SCR_DIR_Pos, dir);
}

/*!
 * @brief Set the channel interrupt.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetChannelInterrupt(PWM_Type *base, pwm_channel_type_t channel, bool state)
{
    MODIFY_REG32(base->CHANNELS[channel].CHnSCR, PWM_CH0SCR_CHIE_Msk, PWM_CH0SCR_CHIE_Pos, state);
}

/*!
 * @brief Get the channel interrupt flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @return channel Flag
 */
#define PWM_GetChannelFlag(base, channel)           READ_BIT32((base)->CHANNELS[channel].CHnSCR, PWM_CH0SCR_CHIF_Msk)

/*!
 * @brief Clear the channel interrupt flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @return none
 */
#define PWM_ClearChannelFlag(base, channel)         CLEAR_BIT32((base)->CHANNELS[channel].CHnSCR, PWM_CH0SCR_CHIF_Msk)

/*!
 * @brief Set the channel dma enable.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetChannelDmaEnable(PWM_Type *base, pwm_channel_type_t channel, bool state)
{
    MODIFY_REG32(base->CHANNELS[channel].CHnSCR, PWM_CH0SCR_DMAEN_Msk, PWM_CH0SCR_DMAEN_Pos, state);
}

/*!
 * @brief Set channel counting value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] value: counting value
 *            - 0 ~ 0xffff
 * @return none
 */
#define PWM_SetChannelCountValue(base, channel, value)   WRITE_REG32((base)->CHANNELS[channel].CHnV, value)

/*!
 * @brief Get channel counting value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @return channel counting value
 */
#define PWM_GetChannelCountValue(base, channel)          READ_BIT32((base)->CHANNELS[channel].CHnV, PWM_CH0V_CHCVAL_Msk)

/*!
 * @brief Get all channel event flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return all channel Flag
 */
#define PWM_GetAllChannelEventFlag(base)                 READ_BIT32((base)->STR, PWM_STR_CH_EVENT_Msk)

/*!
 * @brief Get all channel input status.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return all channel Flag
 */
#define PWM_GetAllChannelInputStatus(base)               READ_BIT32((base)->STR, PWM_STR_CH_INPUT_Msk)

/*!
 * @brief Clear all channel flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mask: mask bit
 *            - 0 ~ 0xff
 * @return none
 */
#define PWM_ClearAllChannelEventFlag(base, mask)         CLEAR_BIT32((base)->STR, mask)

/*!
 * @brief Get all channel flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return all channel Flag
 */
#define PWM_GetHallStatus(base)                          (READ_BIT32((base)->STR, PWM_STR_HALL_STATUS_Msk) >> PWM_STR_HALL_STATUS_Pos)

/*!
 * @brief PWM_SyncEnable.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SyncEnable(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_PWMSYNCEN_Msk, PWM_FUNCSEL_PWMSYNCEN_Pos, state);
}

/*!
 * @brief Set channel initial output function.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_InitChannelOutputEn(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_INIT_Msk, PWM_FUNCSEL_INIT_Pos, state);
}

/*!
 * @brief Set the write protection.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetWriteProtection(PWM_Type *base, bool state)
{
    if (true == state)
    {
        MODIFY_REG32(base->FDSR, PWM_FDSR_WPEN_Msk, PWM_FDSR_WPEN_Pos, true);
    }
    else
    {
        MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_WPDIS_Msk, PWM_FUNCSEL_WPDIS_Pos, true);
    }
}

/*!
 * @brief Set synchronizing type.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] type: sycn type
 *            - 0: No restrictions. Software and hardware triggers can be used by MCVR, CHnV, OMSR, and
 *                 PWM counter synchronization
 *            - 1: Software trigger can only be used by MCVR and CHnV synchronization, and hardware triggers
 *                 can only be used by OMSR and PWM counter synchronization
 * @return none
 */
static inline void PWM_SetSyncType(PWM_Type *base, uint8_t type)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_PWMSYNC_Msk, PWM_FUNCSEL_PWMSYNC_Pos, type);
}

/*!
 * @brief Set fault control mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: fault mode
 *            - PWM_FAULT_CTRL_NONE
 *            - PWM_FAULT_CTRL_MANUAL_EVEN
 *            - PWM_FAULT_CTRL_MANUAL_ALL
 *            - PWM_FAULT_CTRL_AUTO
 * @return none
 */
static inline void PWM_SetFaultMode(PWM_Type *base, pwm_fault_ctrl_mode_t mode)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_FAULTMODE_Msk, PWM_FUNCSEL_FAULTMODE_Pos, mode);
}

/*!
 * @brief Set fault interrupt.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetFaultInterrupt(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_FAULTIE_Msk, PWM_FUNCSEL_FAULTIE_Pos, state);
}

/*!
 * @brief Set the fault HIZ output.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetFaultHizOutput(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_FAULTHIZEN_Msk, PWM_FUNCSEL_FAULTHIZEN_Pos, state);
}

/*!
 * @brief Set the channel Hiz Output.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetChannelHizOutput(PWM_Type *base, pwm_channel_type_t channel, bool state)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_CH0HIZEN_Msk << ((uint8_t)channel),
                 PWM_FUNCSEL_CH0HIZEN_Pos + ((uint8_t)channel), state);
}

/*!
 * @brief Set channel center align duty type when Cnv == Cn+1V.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] dutyType: pwm channel number
 *            - PWM_DUTY_MODE_0
 *            - PWM_DUTY_MODE_1
 * @return none
 */
static inline void PWM_SetCenterAlignDutyType(PWM_Type *base, pwm_center_align_duty_type_t dutyType)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_PDYTPEN_Msk, PWM_FUNCSEL_PDYTPEN_Pos, dutyType);
}

/*!
 * @brief Set hall mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetHallMode(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_HALLEN_Msk, PWM_FUNCSEL_HALLEN_Pos, state);
}

/*!
 * @brief Set measure mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairMeasureMode(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->FUNCSEL, PWM_FUNCSEL_CHP0PNWEN_Msk << pairChannel, PWM_FUNCSEL_CHP0PNWEN_Pos + pairChannel, state);
}

/*!
 * @brief Set maximum loading point mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetMaxLoadingPoint(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNC, PWM_SYNC_MAXSYNCP_Msk, PWM_SYNC_MAXSYNCP_Pos, state);
}

/*!
 * @brief Set minimum loading point mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetMinLoadingPoint(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNC, PWM_SYNC_MINSYNCP_Msk, PWM_SYNC_MINSYNCP_Pos, state);
}

/*!
 * @brief Set the OMCR register updated time.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_EnableOutputMaskSync(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNC, PWM_SYNC_OMSYNCP_Msk, PWM_SYNC_OMSYNCP_Pos, state);
}

/*!
 * @brief Set the POL register updated time.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPOLSync(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNC, PWM_SYNC_PWM_SYNCPOL_Msk, PWM_SYNC_PWM_SYNCPOL_Pos, state);
}

/*!
 * @brief Set CnV/CNTIN/MOD sync bypass.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetSyncBYPEN(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNC, PWM_SYNC_PWM_SYNC_BYPEN_Msk, PWM_SYNC_PWM_SYNC_BYPEN_Pos, state);
}

/*!
 * @brief sel CnV/CNTIN/MOD Buffer or Register when read.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetBufferAndRegisterChoose(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNC, PWM_SYNC_PWM_BFRGCHSEN_Msk, PWM_SYNC_PWM_BFRGCHSEN_Pos, state);
}

/*!
 * @brief Set hardware synchronization trigger source.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] num: hardware synchronization trigger num
 *            - 0 ~ 2
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetHardwareTriggerSyncSrc(PWM_Type *base, uint8_t num, bool state)
{
    DEVICE_ASSERT(3U > num);

    MODIFY_REG32(base->SYNC, PWM_SYNC_TRIG0_Msk << num, PWM_SYNC_TRIG0_Pos + num, state);
}

/*!
 * @brief Set software trigger synchronization.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetSoftwareTriggerSync(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNC, PWM_SYNC_SWSYNC_Msk, PWM_SYNC_SWSYNC_Pos, state);
}

/*!
 * @brief Set channel output init level.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] level: initialization level
 *            - PWM_LOW_LEVEL
 *            - PWM_HIGH_LEVEL
 * @return none
 */
static inline void PWM_SetChannelOutputInitLevel(PWM_Type *base, pwm_channel_type_t channel, pwm_output_state_t level)
{
    MODIFY_REG32(base->OUTINIT, PWM_OUTINIT_CH0OIV_Msk << ((uint8_t)channel), ((uint8_t)channel), level);
}

/*!
 * @brief Set channel output mask.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
#define PWM_SetChannelOutputMask(base, channel, state)      MODIFY_REG32((base)->OMCR, PWM_OUTINIT_CH0OIV_Msk << (channel), channel, state)

/*!
 * @brief Set all channel output mask.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mask: channel mask
 *            - 0 ~ 0xff
 * @return none
 */
#define PWM_SetAllChannelOutputMask(base, mask)             MODIFY_REG32((base)->OMCR, PWM_OMCR_CH_Msk, PWM_OMCR_CH0OMEN_Pos, mask);

/*!
 * @brief Set channel pair output combine.
 *
 * @param[in] base: pwm module
 *            - PWM0
 *            - PWM1
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelCombine(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->MODESEL, PWM_MODESEL_PAIR0COMBINEN_Msk << (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH),
                 (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH), state);
}

/*!
 * @brief Set channel pair output complement.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelComplement(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->MODESEL, PWM_MODESEL_PAIR0COMPEN_Msk << (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH),
                 PWM_MODESEL_PAIR0COMPEN_Pos + (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH), state);
}

/*!
 * @brief Set channel pair dual edge capture mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelDualEdgeCapture(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->MODESEL, PWM_MODESEL_PAIR0DECAPEN_Msk << (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH),
                 PWM_MODESEL_PAIR0DECAPEN_Pos + (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH), state);
}

/*!
 * @brief Set pair channel dual edge capture.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelDecap(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->MODESEL, PWM_MODESEL_PAIR0DECAP_Msk << (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH),
                 PWM_MODESEL_PAIR0DECAP_Pos + (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH), state);
}

/*!
 * @brief Set pair channel deadtime insertion.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelDeadtime(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->MODESEL, PWM_MODESEL_PAIR0DTEN_Msk << (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH),
                 PWM_MODESEL_PAIR0DTEN_Pos + (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH), state);
}

/*!
 * @brief Set pair channel counter synchronization.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelCounterSync(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->MODESEL, PWM_MODESEL_PAIR0SYNCEN_Msk << (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH),
                 PWM_MODESEL_PAIR0SYNCEN_Pos + (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH), state);
}

/*!
 * @brief Set pair channel fault control.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelFaultControl(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->MODESEL, PWM_MODESEL_PAIR0FAULTEN_Msk << (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH),
                 PWM_MODESEL_PAIR0FAULTEN_Pos + (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH), state);
}

/*!
 * @brief Set pair channel symmetric.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelSymmetric(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->MODESEL, PWM_MODESEL_PAIR0SYMEN_Msk << (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH),
                 PWM_MODESEL_PAIR0SYMEN_Pos + (pairChannel * PWM_COMBINE_PAIR_CONFIG_WIDTH), state);
}

/*!
 * @brief Set pair channel inverting control.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] pairChannel: pair channel number
 *            - 0 ~ 3
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPairChannelInvert(PWM_Type *base, uint8_t pairChannel, bool state)
{
    MODIFY_REG32(base->INVCR, PWM_INVCR_PAIR0INVEN_Msk << pairChannel, pairChannel, state);
}

/*!
 * @brief Set the channel match trigger source.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetChannelMatchTrigger(PWM_Type *base, pwm_channel_type_t channel, bool state)
{
    MODIFY_REG32(base->EXTTRIG, PWM_EXTTRIG_CH0TRIG_Msk << ((uint8_t)channel), ((uint8_t)channel), state);
}

/*!
 * @brief Set the initialization trigger source.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetInitTrigger(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->EXTTRIG, PWM_EXTTRIG_INITTRIGEN_Msk, PWM_EXTTRIG_INITTRIGEN_Pos, state);
}

/*!
 * @brief Set the max trigger source.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetMaxTrigger(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->EXTTRIG, PWM_EXTTRIG_MAXTRIGEN_Msk, PWM_EXTTRIG_MAXTRIGEN_Pos, state);
}

/*!
 * @brief Set trigger ratio.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] triggerRatio: 0 ~ 7
 * @return none
 */
static inline void PWM_SetTriggerRatio(PWM_Type *base, uint8_t triggerRatio)
{
    MODIFY_REG32(base->EXTTRIG, PWM_EXTTRIG_TRIGRATIO_Msk, PWM_EXTTRIG_TRIGRATIO_Pos, triggerRatio);
}

/*!
 * @brief Get the channel trigger flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return external trigger flag
 */
static inline uint32_t PWM_GetChannelTriggerFlag(PWM_Type *base)
{
    return READ_BIT32(base->EXTTRIG, PWM_EXTTRIG_TRIGF_Msk);
}

/*!
 * @brief Clear channel trigger flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return none
 */
static inline void PWM_ClearChannelTriggerFlag(PWM_Type *base)
{
    CLEAR_BIT32(base->EXTTRIG, PWM_EXTTRIG_TRIGF_Msk);
}

/*!
 * @brief Set channel polarity.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] polarity: output polarity
 *            - PWM_OUTPUT_POLARITY_ACTIVE_HIGH: active low
 *            - PWM_OUTPUT_POLARITY_ACTIVE_LOW: active high
 * @return none
 */
static inline void PWM_SetChannelPolarity(PWM_Type *base, pwm_channel_type_t channel,
                                          pwm_output_polarity_active_type_t polarity)
{
    MODIFY_REG32(base->CHOPOLCR, PWM_CHOPOLCR_CH0POL_Msk << ((uint8_t)channel), ((uint8_t)channel), polarity);
}

/*!
 * @brief Get the flag about the OR of all fault.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return the flag about the OR of all fault
 */
#define PWM_GetFaultFlag(base)                      READ_BIT32((base)->FDSR, PWM_FDSR_FAULTDF_Msk)

/*!
 * @brief Clear the flag about the OR of all fault.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return none
 */
#define PWM_ClearFaultFlag(base)                    CLEAR_BIT32((base)->FDSR, PWM_FDSR_FAULTDF_Msk);

/*!
 * @brief Get fault channel detection flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 *            - PWM_FAULT_CH_3
 * @return fault channel detection flag
 */
#define PWM_GetFaultChannelFlag(base, channel)      READ_BIT32((base)->FDSR, (PWM_FDSR_FAULTDF0_Msk << (channel)))

/*!
 * @brief Clear fault channel detection flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 *            - PWM_FAULT_CH_3
 * @return none
 */
#define PWM_ClearFaultChannelFlag(base, channel)    CLEAR_BIT32((base)->FDSR, (PWM_FDSR_FAULTDF0_Msk << (channel)))

/*!
 * @brief Get fault input status flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return fault input status flag
 */
#define PWM_GetFaultInputStatusFlag(base)           READ_BIT32((base)->FDSR, PWM_FDSR_FAULTIN_Msk)

/*!
 * @brief Set channel input filter value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - PWM_CH_1
 *            - ...
 *            - PWM_CH_3
 * @param[in] value: filter value
 *            - 0 ~ 15
 * @return none
 */
static inline void PWM_SetChannelInputFilterVal(PWM_Type *base, pwm_channel_type_t channel, uint8_t value)
{
    DEVICE_ASSERT(PWM_CH_4 > channel);

    MODIFY_REG32(base->CAPFILTER, PWM_CAPFILTER_CH0CAPFVAL_Msk << (((uint8_t)channel) * PWM_CAPFILTER_WIDTH),
                (((uint8_t)channel) * PWM_CAPFILTER_WIDTH), value);
}

/*!
 * @brief Set channel input filter psc.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] psc: pwm channel number
 *            - PWM_INPUT_CAPTURE_FILTER_PS_1
 *            - PWM_INPUT_CAPTURE_FILTER_PS_2
 *            - ...
 *            - PWM_INPUT_CAPTURE_FILTER_PS_4096
 * @return none
 */
static inline void PWM_SetChannelInputFilterPsc(PWM_Type *base, pwm_channel_input_filter_psc_type_t psc)
{
    MODIFY_REG32(base->CAPFILTER, PWM_CAPFILTER_CAPFPSC_Msk, PWM_CAPFILTER_CAPFPSC_Pos, psc);
}

/*!
 * @brief Set fault input activity.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetFaultInput(PWM_Type *base, pwm_fault_channel_type_t channel, bool state)
{
    MODIFY_REG32(base->FFAFER, PWM_FFAFER_FER0EN_Msk << ((uint8_t)channel), ((uint8_t)channel), state);
}

/*!
 * @brief Set fault input filter.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetFaultInputFilter(PWM_Type *base, pwm_fault_channel_type_t channel, bool state)
{
    MODIFY_REG32(base->FFAFER, PWM_FFAFER_FF0EN_Msk << ((uint8_t)channel),
                 PWM_FFAFER_FF0EN_Pos + ((uint8_t)channel), state);
}

/*!
 * @brief Set fault input filter value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] value: fault filter value
 *            - 0 ~ 15
 * @return none
 */
static inline void PWM_SetFaultInputFilterValue(PWM_Type *base, uint8_t value)
{
    MODIFY_REG32(base->FFAFER, PWM_FFAFER_FFVAL_Msk, PWM_FFAFER_FFVAL_Pos, value);
}

/*!
 * @brief Set fault input polarity.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: fault input channel
 *            - PWM_FAULT_CH_0
 *            - PWM_FAULT_CH_1
 *            - PWM_FAULT_CH_2
 * @param[in] polarity: output polarity
 *            - PWM_INPUT_POLARITY_ACTIVE_HIGH: fault input polarity is active high
 *            - PWM_INPUT_POLARITY_ACTIVE_LOW: fault input polarity is active low
 * @return none
 */
static inline void PWM_SetFaultInputPolarity(PWM_Type *base, pwm_fault_channel_type_t channel,
                                             pwm_fault_input_polarity_active_type_t polarity)
{
    MODIFY_REG32(base->FLTPOL, PWM_FLTPOL_FLT0POL_Msk << ((uint8_t)channel), ((uint8_t)channel), polarity);
}

/*!
 * @brief Set quadrature decoder enable/disable.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetQuadEncoder(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->QDI, PWM_QDI_QDIEN_Msk, PWM_QDI_QDIEN_Pos, state);
}

/*!
 * @brief Get the quadrature timer overflow direction.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return quad time overflow direction flag
 */
#define PWM_GetQuadOverflowDir(base)            READ_BIT32((base)->QDI, PWM_QDI_CNTOFDIR_Msk)

/*!
 * @brief Get the quadrature timer counting direction.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return quadrature counting direction
 */
#define PWM_GetQuadCountDir(base)               READ_BIT32((base)->QDI, PWM_QDI_QUADIR_Msk)

/*!
 * @brief Set quadrature encoding mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: quadrature encoding mode
 *            - PWM_QUAD_PHASE_ENCODE: Phase A and Phase B encoding mode
 *            - PWM_QUAD_COUNT_DIR: Count and direction encoding mode
 * @return none
 */
static inline void PWM_SetQuadEncodeMode(PWM_Type *base, pwm_quad_decoder_mode_t mode)
{
    MODIFY_REG32(base->QDI, PWM_QDI_QUADMODE_Msk, PWM_QDI_QUADMODE_Pos, mode);
}

/*!
 * @brief Set quadrature phase A polarity.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] polarity: quadrature input polarity
 *            - PWM_QUAD_PHASE_NORMAL: Normal polarity
 *            - PWM_QUAD_PHASE_INVERT: Inverted polarity
 * @return none
 */
static inline void PWM_SetQuadPhaseAPolarity(PWM_Type *base, pwm_quad_phase_polarity_type_t polarity)
{
    MODIFY_REG32(base->QDI, PWM_QDI_PHAPOL_Msk, PWM_QDI_PHAPOL_Pos, polarity);
}

/*!
 * @brief Set quadrature phase B polarity.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] polarity: quadrature input polarity
 *            - PWM_QUAD_PHASE_NORMAL: Normal polarity
 *            - PWM_QUAD_PHASE_INVERT: Inverted polarity
 * @return none
 */
static inline void PWM_SetQuadPhaseBPolarity(PWM_Type *base, pwm_quad_phase_polarity_type_t polarity)
{
    MODIFY_REG32(base->QDI, PWM_QDI_PHBPOL_Msk, PWM_QDI_PHBPOL_Pos, polarity);
}

/*!
 * @brief Set quadrature phase Z polarity.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] polarity: quadrature input polarity
 *            - PWM_QUAD_PHASE_NORMAL: Normal polarity
 *            - PWM_QUAD_PHASE_INVERT: Inverted polarity
 * @return none
 */
static inline void PWM_SetQuadPhaseZPolarity(PWM_Type *base, pwm_quad_phase_polarity_type_t polarity)
{
    MODIFY_REG32(base->QDI, PWM_QDI_PHZPOL_Msk, PWM_QDI_PHZPOL_Pos, polarity);
}

/*!
 * @brief Set quadrature phase Z reset cnt enable/disable.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetQuadPhaseZResetCnt(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->QDI, PWM_QDI_PHZRSTEN_Msk, PWM_QDI_PHZRSTEN_Pos, state);
}

/*!
 * @brief Get the phaseZ flag.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return phaseZ Flag
 */
#define PWM_GetPhaseZFlag(base)           READ_BIT32((base)->QDI, PWM_QDI_PHZSTS_Msk)

/*!
 * @brief Clear the phase Z flag.
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @return none
 */
#define PWM_ClearPhaseZFlag(base)         CLEAR_BIT32((base)->QDI, PWM_QDI_PHZSTS_Msk)

/*!
 * @brief Set CNTOF frequency.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] freq: CNTOF overflow frequency
 *            - 0 ~ 127
 * @return none
 */
static inline void PWM_SetCNTOFFrequence(PWM_Type *base, uint8_t freq)
{
    MODIFY_REG32(base->CONF, PWM_CONF_CNTOFNUM_Msk, PWM_CONF_CNTOFNUM_Pos, freq);
}

/**
 * @brief Set the capture event prescale.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] psc: capture event prescale
 *            - PWM_EVENT_PSC_1
 *            - PWM_EVENT_PSC_2
 *            - PWM_EVENT_PSC_4
 *            - PWM_EVENT_PSC_8
 * @return none
 */
static inline void PWM_SetCaptureEventPrescaler(PWM_Type *base, pwm_channel_type_t channel,
                                                pwm_input_event_psc_type_t psc)
{
    MODIFY_REG32(base->CONF, PWM_CONF_EVENT0_PSC_Msk << (((uint8_t)channel) * PWM_CONF_EVENTPSC_WIDTH),
                 PWM_CONF_EVENT0_PSC_Pos + (((uint8_t)channel) * PWM_CONF_EVENTPSC_WIDTH), psc);
}

/*!
 * @brief Set global time base enable/disable.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetGlobalTimeBase(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->CONF, PWM_CONF_GTBEEN_Msk, PWM_CONF_GTBEEN_Pos, state);
}

/*!
 * @brief Set global time base output.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetGlobalTimeBaseOutput(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->CONF, PWM_CONF_GTBEOUT_Msk, PWM_CONF_GTBEOUT_Pos, state);
}

/*!
 * @brief Set hardware trigger auto clear.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetHwTriggerSyncAutoClear(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_HWTRIGMODE_Msk, PWM_SYNCONF_HWTRIGMODE_Pos, !state);
}

/*!
 * @brief Set synchronization mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: synchronization selection
 *            - PWM_SYNC_MODE_LEGACY: Legacy synchronization
 *            - PWM_SYNC_MODE_ENHANCED: Enhanced synchronization
 * @return none
 */
static inline void PWM_SetSyncMode(PWM_Type *base, pwm_sync_mode_t mode)
{
    MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_SYNCMODE_Msk, PWM_SYNCONF_SYNCMODE_Pos, mode);
}

/*!
 * @brief Set the CNTIN register updated time.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetCNTINSync(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_CNTINC_Msk, PWM_SYNCONF_CNTINC_Pos, state);
}

/*!
 * @brief Set the INVCR register updated time.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetINVCRSync(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_INVC_Msk, PWM_SYNCONF_INVC_Pos, state);
}

/*!
 * @brief Set the CHOSWCR register updated time.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetCHOSWCRSync(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_SWOC_Msk, PWM_SYNCONF_SWOC_Pos, state);
}

/*!
 * @brief Set the CHOPOLCR register updated time.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetPOLTriggerMode(PWM_Type *base, pwm_sync_trigger_mode_t mode, bool state)
{
    if (PWM_SYNC_TRIGGER_SOFTWARE == mode)
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_SWPOL_Msk, PWM_SYNCONF_SWPOL_Pos, state);
    }
    else
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_HWPOL_Msk, PWM_SYNCONF_HWPOL_Pos, state);
    }
}

/*!
 * @brief Set CNT register tirgger mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: tirgger mode
 *            - PWM_SYNC_TRIGGER_SOFTWARE: software trigger
 *            - PWM_SYNC_TRIGGER_HARDWARE: hardware trigger
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetCNTSyncTriggerMode(PWM_Type *base, pwm_sync_trigger_mode_t mode, bool state)
{
    if (PWM_SYNC_TRIGGER_SOFTWARE == mode)
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_CNTVSWSYNC_Msk, PWM_SYNCONF_CNTVSWSYNC_Pos, state);
    }
    else
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_CNTVHWSYNC_Msk, PWM_SYNCONF_CNTVHWSYNC_Pos, state);
    }
}

/*!
 * @brief Set MCVR/CNTIN/CHV register tirgger mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: tirgger mode
 *            - PWM_SYNC_TRIGGER_SOFTWARE: software trigger
 *            - PWM_SYNC_TRIGGER_HARDWARE: hardware trigger
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetMCVRSyncTriggerMode(PWM_Type *base, pwm_sync_trigger_mode_t mode, bool state)
{
    if (PWM_SYNC_TRIGGER_SOFTWARE == mode)
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_PWMSVSWSYNC_Msk, PWM_SYNCONF_PWMSVSWSYNC_Pos, state);
    }
    else
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_PWMSVHWSYNC_Msk, PWM_SYNCONF_PWMSVHWSYNC_Pos, state);
    }
}

/*!
 * @brief Set OMCR register tirgger mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: tirgger mode
 *            - PWM_SYNC_TRIGGER_SOFTWARE: software trigger
 *            - PWM_SYNC_TRIGGER_HARDWARE: hardware trigger
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetOMCRSyncTriggerMode(PWM_Type *base, pwm_sync_trigger_mode_t mode, bool state)
{
    if (PWM_SYNC_TRIGGER_SOFTWARE == mode)
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_OMVSWSYNC_Msk, PWM_SYNCONF_OMVSWSYNC_Pos, state);
    }
    else
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_OMVHWSYNC_Msk, PWM_SYNCONF_OMVHWSYNC_Pos, state);
    }
}

/*!
 * @brief Set INVCR register tirgger mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: tirgger mode
 *            - PWM_SYNC_TRIGGER_SOFTWARE: software trigger
 *            - PWM_SYNC_TRIGGER_HARDWARE: hardware trigger
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetINVCRSyncTriggerMode(PWM_Type *base, pwm_sync_trigger_mode_t mode, bool state)
{
    if (PWM_SYNC_TRIGGER_SOFTWARE == mode)
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_INVSWSYNC_Msk, PWM_SYNCONF_INVSWSYNC_Pos, state);
    }
    else
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_INVHWSYNC_Msk, PWM_SYNCONF_INVHWSYNC_Pos, state);
    }
}

/*!
 * @brief Set CHOSWCR register tirgger mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: tirgger mode
 *            - PWM_SYNC_TRIGGER_SOFTWARE: software trigger
 *            - PWM_SYNC_TRIGGER_HARDWARE: hardware trigger
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetCHOSWCRSyncTriggerMode(PWM_Type *base, pwm_sync_trigger_mode_t mode, bool state)
{
    if (PWM_SYNC_TRIGGER_SOFTWARE == mode)
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_SWVSWSYNC_Msk, PWM_SYNCONF_SWVSWSYNC_Pos, state);
    }
    else
    {
        MODIFY_REG32(base->SYNCONF, PWM_SYNCONF_SWVHWSYNC_Msk, PWM_SYNCONF_SWVHWSYNC_Pos, state);
    }
}

/*!
 * @brief Set channel software control function.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] state: enabling state
 *            - true
 *            - false
 * * @return none
 */
#define PWM_SetChannelSoftControl(base, channel, state)     MODIFY_REG32((base)->CHOSWCR, (PWM_CHOSWCR_CH0SWEN_Msk << (channel)), \
                                                                         channel, state)

/*!
 * @brief Set all channel software control function.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mask: channel software control mask
 *            - 0 ~ 0xff
 * @return none
 */
#define PWM_SetAllChannelSoftControl(base, mask)            MODIFY_REG32((base)->CHOSWCR, PWM_CHOSWCR_CHNSWEN_MASK, \
                                                                         PWM_CHOSWCR_CH0SWEN_Pos, mask)

/*!
 * @brief Software control channel output level.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] channel: pwm channel number
 *            - PWM_CH_0
 *            - ...
 *            - PWM_CH_7
 * @param[in] level: output level
 *            - PWM_LOW_LEVEL: output low level
 *            - PWM_HIGH_LEVEL: output high level
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SoftControlChannelLevel(PWM_Type *base, pwm_channel_type_t channel,
                                               pwm_output_state_t level, bool state)
{
    MODIFY_REG32(base->CHOSWCR, PWM_CHOSWCR_CH0SWCV_Msk << ((uint8_t)channel),
                 PWM_CHOSWCR_CH0SWCV_Pos + ((uint8_t)channel), level);
    PWM_SetChannelSoftControl(base, ((uint8_t)channel), state);
}

/*!
 * @brief Software control all channel output level.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mask: channel mask
 *            - 0 ~ 0xff
 * @param[in] level: all 8 channel output state
 *            - 0 ~ 0xff
 * @return none
 */
static inline void PWM_SoftControlAllChannelLevel(PWM_Type *base, uint8_t mask, uint8_t level)
{
    MODIFY_REG32(base->CHOSWCR, PWM_CHOSWCR_CHNSWCV_MASK, PWM_CHOSWCR_CH0SWCV_Pos, level);
    PWM_SetAllChannelSoftControl(base, mask);
}

/*!
 * @brief Set pwm channel0 ~ channel7 counter dither value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] ditherRegNum
 *            - 0: set dither0 regiter
 *            - 1: set dither1 regiter
 * @param[in] channelDitherValue: value for all 4 channel dither value
 * @return none
 */
static inline void PWM_SetChannelCounterDitherValue(PWM_Type *base, uint8_t ditherRegNum, uint32_t channelDitherValue)
{
    if (0U == ditherRegNum)
    {
        WRITE_REG32(base->DITHER0, channelDitherValue);
    }
    else
    {
        WRITE_REG32(base->DITHER1, channelDitherValue);
    }
}

/*!
 * @brief Set mod dither value.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] maxCountDitherValue: mod dither value
 *            - 0 ~ 31
 * @return none
 */
static inline void PWM_SetMaxCountDitherValue(PWM_Type *base, uint8_t maxCountDitherValue)
{
    MODIFY_REG32(base->DITHER2, PWM_DITHER2_PDHR_Msk, PWM_DITHER2_PDHR_Pos, maxCountDitherValue);
}

/*!
 * @brief Set pwm enter debug mode.
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] mode: pwm debug mode
 *            - PWM_DEBUG_MODE_NO_EFFECT
 *            - PWM_DEBUG_MODE_COUNTER_STOPPED_OUTPUT_PREVIOUS
 *            - PWM_DEBUG_MODE_COUNTER_STOPPED_OUTPUT_HIGH
 * @return none
 */
static inline void PWM_SetDebugMode(PWM_Type *base, pwm_debug_mode_t mode)
{
    MODIFY_REG32(base->DITHER2, PWM_DITHER2_DEBUGCTRL_Msk, PWM_DITHER2_DEBUGCTRL_Pos, mode);
}

/*!
 * @brief Set pwm transfer length for dma req clearing
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] len: PWM Dma transfer length
 * @return none
 */
static inline void PWM_SetDmaTransferLen(PWM_Type *base, uint8_t len)
{
    MODIFY_REG32(base->DMACTRL, PWM_DMACTRL_TRANS_LEN_Msk, PWM_DMACTRL_TRANS_LEN_Pos, len);
}

/*!
 * @brief Set pwm dma transfer len enable
 *
 * @param[in] base: the pwm base address
 *            - PWM0
 *            - PWM1
 *            - ...
 *            - PWM5
 * @param[in] state: enabling state
 *            - true
 *            - false
 * @return none
 */
static inline void PWM_SetDmaLenEn(PWM_Type *base, bool state)
{
    MODIFY_REG32(base->DMACTRL, PWM_DMACTRL_TRANS_EN_Msk, PWM_DMACTRL_TRANS_EN_Pos, state);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PWM_HW_H */

/* =============================================  EOF  ============================================== */
