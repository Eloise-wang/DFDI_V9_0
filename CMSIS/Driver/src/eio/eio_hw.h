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
 * @file eio_hw.h
 *
 * @brief This file provides eio hardware integration functions interface.
 *
 */

#ifndef _EIO_HW_H
#define _EIO_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
/* CTRL Bit Fields */
#define EIO_CTRL_FLEXEN(x)                    (((uint32_t)(((uint32_t)(x))<<EIO_CTRL_ENHANCED_EN_Pos))&EIO_CTRL_ENHANCED_EN_Msk)
#define EIO_CTRL_SWRST(x)                     (((uint32_t)(((uint32_t)(x))<<EIO_CTRL_SWRST_Pos))&EIO_CTRL_SWRST_Msk)
#define EIO_CTRL_DBGE(x)                      (((uint32_t)(((uint32_t)(x))<<EIO_CTRL_DBGE_Pos))&EIO_CTRL_DBGE_Msk)
/* PIN Bit Fields */
#define EIO_PIN_PDI(x)                        (((uint32_t)(((uint32_t)(x))<<EIO_PIN_PDI_Pos))&EIO_PIN_PDI_Msk)
/* PosSTAT Bit Fields */
#define EIO_SHIFTSTAT_SSF(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTSTAT_SSF_Pos))&EIO_SHIFTSTAT_SSF_Msk)
/* PosERR Bit Fields */
#define EIO_SHIFTERR_SEF(x)                   (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTERR_SEF_Pos))&EIO_SHIFTERR_SEF_Msk)
/* TIMSTAT Bit Fields */
#define EIO_TIMSTAT_TSF(x)                    (((uint32_t)(((uint32_t)(x))<<EIO_TIMSTAT_TSF_Pos))&EIO_TIMSTAT_TSF_Msk)
/* PosSIEN Bit Fields */
#define EIO_SHIFTSIEN_SSIE(x)                 (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTSIEN_SSIE_Pos))&EIO_SHIFTSIEN_SSIE_Msk)
/* PosEIEN Bit Fields */
#define EIO_SHIFTEIEN_SEIE(x)                 (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTEIEN_SEIE_Pos))&EIO_SHIFTEIEN_SEIE_Msk)
/* TIMIEN Bit Fields */
#define EIO_TIMIEN_TEIE(x)                    (((uint32_t)(((uint32_t)(x))<<EIO_TIMIEN_TEIE_Pos))&EIO_TIMIEN_TEIE_Msk)
/* PosSDEN Bit Fields */
#define EIO_SHIFTSDEN_SSDE(x)                 (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTSDEN_SSDE_Pos))&EIO_SHIFTSDEN_SSDE_Msk)
/* PosCTL Bit Fields */
#define EIO_SHIFTCTL_SMOD(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCTL0_SMOD_Pos))&EIO_SHIFTCTL0_SMOD_Msk)
#define EIO_SHIFTCTL_PINPOL(x)                (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCTL0_PINPOL_Pos))&EIO_SHIFTCTL0_PINPOL_Msk)
#define EIO_SHIFTCTL_PINSEL(x)                (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCTL0_PINSEL_Pos))&EIO_SHIFTCTL0_PINSEL_Msk)
#define EIO_SHIFTCTL_PINCFG(x)                (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCTL0_PINCFG_Pos))&EIO_SHIFTCTL0_PINCFG_Msk)
#define EIO_SHIFTCTL_TIMPOL(x)                (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCTL0_TIMPOL_Pos))&EIO_SHIFTCTL0_TIMPOL_Msk)
#define EIO_SHIFTCTL_TIMSEL(x)                (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCTL0_TIMSEL_Pos))&EIO_SHIFTCTL0_TIMSEL_Msk)
/* PosCFG Bit Fields */
#define EIO_SHIFTCFG_SSTART(x)                (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCFG0_SSTART_Pos))&EIO_SHIFTCFG0_SSTART_Msk)
#define EIO_SHIFTCFG_SSTOP(x)                 (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCFG0_SSTOP_Pos))&EIO_SHIFTCFG0_SSTOP_Msk)
#define EIO_SHIFTCFG_INSRC(x)                 (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTCFG0_INSRC_Pos))&EIO_SHIFTCFG0_INSRC_Msk)
/* PosBUF Bit Fields */
#define EIO_SHIFTBUF_SHIFBUF(x)               (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTBUF0_SHIFTBUF_Pos))&EIO_SHIFTBUF0_SHIFTBUF_Msk)
/* PosBUFBIS Bit Fields */
#define EIO_SHIFTBUFBIS_SHIFBUFBIS(x)         (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTBUFBIS0_SHIFTBUFBIS_Pos))&EIO_SHIFTBUFBIS0_SHIFTBUFBIS_Msk)
/* PosBUFBYS Bit Fields */
#define EIO_SHIFTBUFBYS_SHIFBUFBYS(x)         (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTBUFBYS0_SHIFTBUFBYS_Pos))&EIO_SHIFTBUFBYS0_SHIFTBUFBYS_Msk)
/* PosBUFBBS Bit Fields */
#define EIO_SHIFTBUFBBS_SHIFBUFBBS(x)         (((uint32_t)(((uint32_t)(x))<<EIO_SHIFTBUFBBS0_SHIFTBUFBBS_Pos))&EIO_SHIFTBUFBBS0_SHIFTBUFBBS_Msk)
/* TIMCTL Bit Fields */
#define EIO_TIMCTL_TIMOD(x)                   (((uint32_t)(((uint32_t)(x))<<EIO_TIMCTL0_TIMOD_Pos))&EIO_TIMCTL0_TIMOD_Msk)
#define EIO_TIMCTL_PINPOL(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCTL0_PINPOL_Pos))&EIO_TIMCTL0_PINPOL_Msk)
#define EIO_TIMCTL_PINSEL(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCTL0_PINSEL_Pos))&EIO_TIMCTL0_PINSEL_Msk)
#define EIO_TIMCTL_PINCFG(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCTL0_PINCFG_Pos))&EIO_TIMCTL0_PINCFG_Msk)
#define EIO_TIMCTL_TRGSRC(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCTL0_TRGSRC_Pos))&EIO_TIMCTL0_TRGSRC_Msk)
#define EIO_TIMCTL_TRGPOL(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCTL0_TRGPOL_Pos))&EIO_TIMCTL0_TRGPOL_Msk)
#define EIO_TIMCTL_TRGSEL(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCTL0_TRGSEL_Pos))&EIO_TIMCTL0_TRGSEL_Msk)
/* TIMCFG Bit Fields */
#define EIO_TIMCFG_TSTART(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCFG0_TSTART_Pos))&EIO_TIMCFG0_TSTART_Msk)
#define EIO_TIMCFG_TSTOP(x)                   (((uint32_t)(((uint32_t)(x))<<EIO_TIMCFG0_TSTOP_Pos))&EIO_TIMCFG0_TSTOP_Msk)
#define EIO_TIMCFG_TIMENA(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCFG0_TIMENA_Pos))&EIO_TIMCFG0_TIMENA_Msk)
#define EIO_TIMCFG_TIMDIS(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCFG0_TIMDIS_Pos))&EIO_TIMCFG0_TIMDIS_Msk)
#define EIO_TIMCFG_TIMRST(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCFG0_TIMRST_Pos))&EIO_TIMCFG0_TIMRST_Msk)
#define EIO_TIMCFG_TIMDEC(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCFG0_TIMDEC_Pos))&EIO_TIMCFG0_TIMDEC_Msk)
#define EIO_TIMCFG_TIMOUT(x)                  (((uint32_t)(((uint32_t)(x))<<EIO_TIMCFG0_TIMOUT_Pos))&EIO_TIMCFG0_TIMOUT_Msk)
/* TIMCMP Bit Fields */
#define EIO_TIMCMP_CMP(x)                     (((uint32_t)(((uint32_t)(x))<<EIO_TIMCMP0_CMP_Pos))&EIO_TIMCMP0_CMP_Msk)

/* ===========================================  Typedef  ============================================ */
/* Shift clock polarity options */
typedef enum
{
    EIO_TIMER_POLARITY_POSEDGE            = 0x00U,  /* Shift on positive edge of Shift clock */
    EIO_TIMER_POLARITY_NEGEDGE            = 0x01U   /* Shift on negative edge of Shift clock */
} eio_timer_polarity_t;

/* Pin polarity options */
typedef enum
{
    EIO_PIN_POLARITY_HIGH                  = 0x00U,  /* Pin is active high */
    EIO_PIN_POLARITY_LOW                   = 0x01U   /* Pin is active low  */
} eio_pin_polarity_t;

/* Pin configuration options */
typedef enum
{
    EIO_PIN_CONFIG_DISABLED                = 0x00U,  /* Shifter pin output disabled */
    EIO_PIN_CONFIG_OPEN_DRAIN              = 0x01U,  /* Shifter pin open drain or bidirectional output enable */
    EIO_PIN_CONFIG_BIDIR_OUTPUT            = 0x02U,  /* Shifter pin bidirectional output data */
    EIO_PIN_CONFIG_OUTPUT                  = 0x03U   /* Shifter pin output */
} eio_pin_config_t;

/* Shifter mode options */
typedef enum
{
    EIO_SHIFTER_MODE_DISABLED               = 0x00U,  /* Shifter disabled */
    EIO_SHIFTER_MODE_RECEIVE                = 0x01U,  /* Receive mode */
    EIO_SHIFTER_MODE_TRANSMIT               = 0x02U,  /* Transmit mode */
    EIO_SHIFTER_MODE_MATCH_STORE            = 0x04U   /* Match Store mode */
} eio_shifter_mode_t;

/* Shifter input source options */
typedef enum
{
    EIO_SHIFTER_SOURCE_PIN                  = 0x00U,  /* Input source is selected pin */
    EIO_SHIFTER_SOURCE_SHIFTER              = 0x01U   /* Input source is shifter N+1 output */
} eio_shifter_source_t;

/* Read/Write mode for shifter buffer */
typedef enum
{
    EIO_SHIFTER_RW_MODE_NORMAL              = 0x00U,  /* Normal shifter buffer read/write */
    EIO_SHIFTER_RW_MODE_BIT_SWAP            = 0x01U   /* Data is bit-swapped */
} eio_shifter_buffer_mode_t;

/* Trigger polarity */
typedef enum
{
    EIO_TRIGGER_POLARITY_HIGH               = 0x00U,  /* Trigger is active high */
    EIO_TRIGGER_POLARITY_LOW                = 0x01U   /* Trigger is active low  */
} eio_trigger_polarity_t;

/* Trigger sources */
typedef enum
{
    EIO_TRIGGER_SOURCE_EXTERNAL             = 0x00U,  /* External trigger selected */
    EIO_TRIGGER_SOURCE_INTERNAL             = 0x01U   /* Internal trigger selected */
} eio_trigger_source_t;

/* Timer mode options */
typedef enum
{
    EIO_TIMER_MODE_DISABLED                 = 0x00U,  /* Timer Disabled. */
    EIO_TIMER_MODE_8BIT_BAUD                = 0x01U,  /* Dual 8-bit counters baud/bit mode. */
    EIO_TIMER_MODE_8BIT_PWM                 = 0x02U,  /* Dual 8-bit counters PWM mode. */
    EIO_TIMER_MODE_16BIT                    = 0x03U   /* Single 16-bit counter mode. */
} eio_timer_mode_t;

/* Timer initial output options */
typedef enum
{
    EIO_TIMER_INITOUT_ONE                    = 0x00U,  /* Timer output is logic one when enabled, unaffected by timer reset. */
    EIO_TIMER_INITOUT_ZERO                   = 0x01U,  /* Timer output is logic zero when enabled, unaffected by timer reset. */
    EIO_TIMER_INITOUT_ONE_RESET              = 0x02U,  /* Timer output is logic one when enabled and on timer reset. */
    EIO_TIMER_INITOUT_ZERO_RESET             = 0x03U   /* Timer output is logic zero when enabled and on timer reset. */
} eio_timer_output_t;

/* Timer decrement options */
typedef enum
{
    EIO_TIMER_DECREMENT_CLK_SHIFT_TMR        = 0x00U,  /* Decrement counter on EIO clock, Shift clock equals Timer output. */
    EIO_TIMER_DECREMENT_TRG_SHIFT_TMR        = 0x01U,  /* Decrement counter on Trigger input (both edges), Shift clock equals Timer output. */
    EIO_TIMER_DECREMENT_PIN_SHIFT_PIN        = 0x02U,  /* Decrement counter on Pin input (both edges), Shift clock equals Pin input. */
    EIO_TIMER_DECREMENT_TRIG_SHIFT_TRIG      = 0x03U   /* Decrement counter on Trigger input (both edges), Shift clock equals Trigger input. */
} eio_timer_decrement_t;

/* Timer reset options */
typedef enum
{
    EIO_TIMER_RESET_NEVER                    = 0x00U,  /* Timer never reset. */
    EIO_TIMER_RESET_PIN_OUT                  = 0x02U,  /* Timer reset on Timer Pin equal to Timer Output. */
    EIO_TIMER_RESET_TRG_OUT                  = 0x03U,  /* Timer reset on Timer Trigger equal to Timer Output. */
    EIO_TIMER_RESET_PIN_RISING               = 0x04U,  /* Timer reset on Timer Pin rising edge. */
    EIO_TIMER_RESET_TRG_RISING               = 0x06U,  /* Timer reset on Trigger rising edge. */
    EIO_TIMER_RESET_TRG_BOTH                 = 0x07U   /* Timer reset on Trigger rising or falling edge. */
} eio_timer_reset_t;

/* Timer disable options */
typedef enum
{
    EIO_TIMER_DISABLE_NEVER                  = 0x00U,  /* Timer never disabled. */
    EIO_TIMER_DISABLE_TIM_DISABLE            = 0x01U,  /* Timer disabled on Timer N-1 disable. */
    EIO_TIMER_DISABLE_TIM_CMP                = 0x02U,  /* Timer disabled on Timer compare. */
    EIO_TIMER_DISABLE_TIM_CMP_TRG_LOW        = 0x03U,  /* Timer disabled on Timer compare and Trigger Low. */
    EIO_TIMER_DISABLE_PIN                    = 0x04U,  /* Timer disabled on Pin rising or falling edge. */
    EIO_TIMER_DISABLE_PIN_TRG_HIGH           = 0x05U,  /* Timer disabled on Pin rising or falling edge provided Trigger is high. */
    EIO_TIMER_DISABLE_TRG                    = 0x06U   /* Timer disabled on Trigger falling edge. */
} eio_timer_disable_t;

/* Timer disable options */
typedef enum
{
    EIO_TIMER_ENABLE_ALWAYS                  = 0x00U,  /* Timer always enabled. */
    EIO_TIMER_ENABLE_TIM_ENABLE              = 0x01U,  /* Timer enabled on Timer N-1 enable. */
    EIO_TIMER_ENABLE_TRG_HIGH                = 0x02U,  /* Timer enabled on Trigger high. */
    EIO_TIMER_ENABLE_TRG_PIN_HIGH            = 0x03U,  /* Timer enabled on Trigger high and Pin high. */
    EIO_TIMER_ENABLE_PIN_POSEDGE             = 0x04U,  /* Timer enabled on Pin rising edge. */
    EIO_TIMER_ENABLE_PIN_POSEDGE_TRG_HIGH    = 0x05U,  /* Timer enabled on Pin rising edge and Trigger high. */
    EIO_TIMER_ENABLE_TRG_POSEDGE             = 0x06U,  /* Timer enabled on Trigger rising edge. */
    EIO_TIMER_ENABLE_TRG_EDGE                = 0x07U   /* Timer enabled on Trigger rising or falling edge. */
} eio_timer_enable_t;

/* Timer stop bit options */
typedef enum
{
    EIO_TIMER_STOP_BIT_DISABLED              = 0x00U,  /* Stop bit disabled. */
    EIO_TIMER_STOP_BIT_TIM_CMP               = 0x01U,  /* Stop bit is enabled on timer compare. */
    EIO_TIMER_STOP_BIT_TIM_DIS               = 0x02U,  /* Stop bit is enabled on timer disable. */
    EIO_TIMER_STOP_BIT_TIM_CMP_DIS           = 0x03U   /* Stop bit is enabled on timer compare and disable. */
} eio_timer_stop_t;

/* Timer stop bit options - for Transmit, Receive or Match Store modes only */
typedef enum
{
    EIO_SHIFTER_STOP_BIT_DISABLED            = 0x00U,  /* Stop bit disabled. */
    EIO_SHIFTER_STOP_BIT_0                   = 0x02U,  /* Transmit/expect stop bit value 0 */
    EIO_SHIFTER_STOP_BIT_1                   = 0x03U   /* Transmit/expect stop bit value 1 */
} eio_shifter_stop_t;

/* Timer start bit options - for Transmit, Receive or Match Store modes only */
typedef enum
{
    EIO_SHIFTER_START_BIT_DISABLED           = 0x00U,  /* Start bit disabled, transmitter loads data on enable */
    EIO_SHIFTER_START_BIT_DISABLED_SH        = 0x01U,  /* Start bit disabled, transmitter loads data on first shift */
    EIO_SHIFTER_START_BIT_0                  = 0x02U,  /* Transmit/expect start bit value 0 */
    EIO_SHIFTER_START_BIT_1                  = 0x03U   /* Transmit/expect start bit value 1 */
} eio_shifter_start_t;

/* Timer start bit options */
typedef enum
{
    EIO_TIMER_START_BIT_DISABLED             = 0x00U,  /* Start bit disabled. */
    EIO_TIMER_START_BIT_ENABLED              = 0x01U   /* Start bit enabled. */
} eio_timer_start_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief This function enables of disables EIO functionality in debug mode.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] enable:enabled or disabled
 * @return none
 */
static inline void EIO_SetDebugMode(EIO_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t)baseAddr->CTRL;
    regValue &= (uint32_t)(~(EIO_CTRL_DBGE_Msk));
    regValue |= EIO_CTRL_DBGE(enable);
    baseAddr->CTRL = (uint32_t)regValue;
}

/*!
 * @brief Calling this function with enable param[in]eter set to true resets all internal
 *        master logic and registers, except the EIO Control Register. The reset state
 *        persists until this function is called with enable param[in]eter set to false.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] enable: enabled or disabled
 * @return none
 */
static inline void EIO_SetSoftwareReset(EIO_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t)baseAddr->CTRL;
    regValue &= (uint32_t)(~(EIO_CTRL_SWRST_Msk));
    regValue |= EIO_CTRL_SWRST(enable);
    baseAddr->CTRL = (uint32_t)regValue;
}

/*!
 * @brief This function enables or disables the EIO module.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] enable:enabled or disabled
 * @return none
 */
static inline void EIO_SetEnable(EIO_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t)baseAddr->CTRL;
    regValue &= (uint32_t)(~(EIO_CTRL_ENHANCED_EN_Msk));
    regValue |= EIO_CTRL_FLEXEN(enable);
    baseAddr->CTRL = (uint32_t)regValue;
}

/*!
 * @brief This function returns the data read from all the EIO pins. Only the lower
 *        n bits are valid, where n is the number of pins returned by EIO_GetPinNum().
 *
 * @param[in] baseAddr: EIO baseAddr
 * @return the data read from all the EIO pins
 */
static inline uint8_t EIO_GetPinData(const EIO_Type *baseAddr)
{
    uint32_t tmp = baseAddr->PIN;
    tmp = (tmp & EIO_PIN_PDI_Msk) >> EIO_PIN_PDI_Pos;
    return (uint8_t)tmp;
}

/*!
 * @brief This function returns the value of the status flag for the specified shifter.
 *        The meaning of the status flag depends on the current mode.
 *        - Transmit mode: shifter buffer is empty and ready to accept more data
 *        - Receive mode: shifter buffer is full and received data can be read from it
 *        - Match Store mode: match occurred between shifter buffer and shifter
 *        - Match Continuous mode: current match result between shifter buffer and shifter
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: value of shifter
 * @return the value of the status flag for the specified shifter
 */
static inline bool EIO_GetShifterStatus(const EIO_Type *baseAddr, uint8_t shifter)
{
    uint32_t regValue = (uint32_t)baseAddr->SHIFTSTAT;
    regValue = (regValue >> shifter) & 1U;
    return (bool)regValue;
}

/*!
 * @brief This function returns the value of the status flags for all shifters. Each bit in
 *        the returned value specifies the status for one shifter, starting with
 *        shifter 0 from least significant bit.
 *        The meaning of the status flag depends on the current mode.
 *        - Transmit mode: shifter buffer is empty and ready to accept more data
 *        - Receive mode: shifter buffer is full and received data can be read from it
 *        - Match Store mode: match occurred between shifter buffer and shifter
 *        - Match Continuous mode: current match result between shifter buffer and shifter
 *
 * @param[in] baseAddr: EIO baseAddr
 * @return the value of the status flags for all shifters
 */
static inline uint32_t EIO_GetAllShifterStatus(const EIO_Type *baseAddr)
{
    return baseAddr->SHIFTSTAT;
}

/*!
 * @brief This function clears the status flag for the specified shifter. This is possible in
 *        all modes except Match Continuous mode.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: value of shifter
 * @return none
 */
static inline void EIO_ClearShifterStatus(EIO_Type *baseAddr, uint8_t shifter)
{
    baseAddr->SHIFTSTAT = 1UL << shifter;
}

/*!
 * @brief This function returns the value of the error status flag for the specified shifter.
 *        The meaning of the error status flag depends on the current mode.
 *        - Transmit mode: shifter buffer was not written before it was transferred in the shifter (buffer overrun)
 *        - Receive mode: shifter buffer was not read before new data was transferred from the shifter (buffer underrun)
 *        - Match Store mode: match event occurred before the previous match data was read from shifter buffer (buffer overrun)
 *        - Match Continuous mode: match occurred between shifter buffer and shifter
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: value of shifter
 * @return the value of the error status flag for the specified shifter
 */
static inline bool EIO_GetShifterErrorStatus(const EIO_Type *baseAddr, uint8_t shifter)
{
    uint32_t regValue = (uint32_t)baseAddr->SHIFTERR;
    regValue = (regValue >> shifter) & 1U;
    return (bool)regValue;
}

/*!
 * @brief This function returns the value of the error status flags for all shifters. Each bit in
 *        the returned value specifies the error status for one shifter, starting with
 *        shifter 0 from least significant bit.
 *        The meaning of the error status flag depends on the current mode.
 *        - Transmit mode: shifter buffer was not written before it was transferred in the shifter (buffer overrun)
 *        - Receive mode: shifter buffer was not read before new data was transferred from the shifter (buffer underrun)
 *        - Match Store mode: match event occurred before the previous match data was read from shifter buffer (buffer overrun)
 *        - Match Continuous mode: match occurred between shifter buffer and shifter
 *
 * @param[in] baseAddr: EIO baseAddr
 * @return the value of the error status flags for all shifters
 */
static inline uint32_t EIO_GetAllShifterErrorStatus(const EIO_Type *baseAddr)
{
    return baseAddr->SHIFTERR;
}

/*!
 * @brief This function clears the error status flag for the specified shifter.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: value of shifter
 * @return none
 */
static inline void EIO_ClearShifterErrorStatus(EIO_Type *baseAddr, uint8_t shifter)
{
    baseAddr->SHIFTERR = 1UL << shifter;
}

/*!
 * @brief This function returns the value of the status flag for the specified timer.
 *        The meaning of the status flag depends on the current mode.
 *        - 8-bit counter mode: the timer status flag is set when the upper 8-bit counter equals zero
 *        and decrements. This also causes the counter to reload with the value in the compare register.
 *        - 8-bit PWM mode: the upper 8-bit counter equals zero and decrements. This also causes the
 *        counter to reload with the value in the compare register.
 *        - 16-bit counter mode: the 16-bit counter equals zero and decrements. This also causes the
 *        counter to reload with the value in the compare register.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: value of timer
 * @return the value of the status flag for the specified timer
 */
static inline bool EIO_GetTimerStatus(const EIO_Type *baseAddr, uint8_t timer)
{
    uint32_t regValue = (uint32_t)baseAddr->TIMSTAT;
    regValue = (regValue >> timer) & 1U;
    return (bool)regValue;
}

/*!
 * @brief This function returns the value of the status flags for all timers. Each bit in
 *        the returned value specifies the status for one timer, starting with
 *        timer 0 from least significant bit.
 *        The meaning of the status flag depends on the current mode.
 *        - 8-bit counter mode: the timer status flag is set when the upper 8-bit counter equals zero
 *        and decrements. This also causes the counter to reload with the value in the compare register.
 *        - 8-bit PWM mode: the upper 8-bit counter equals zero and decrements. This also causes the
 *        counter to reload with the value in the compare register.
 *        - 16-bit counter mode: the 16-bit counter equals zero and decrements. This also causes the
 *        counter to reload with the value in the compare register.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @return the value of the status flags for all timers
 */
static inline uint32_t EIO_GetAllTimerStatus(const EIO_Type *baseAddr)
{
    return baseAddr->TIMSTAT;
}

/*!
 * @brief This function clears the status flag for the specified timer.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: value of timer
 * @return none
 */
static inline void EIO_ClearTimerStatus(EIO_Type *baseAddr, uint8_t timer)
{
    baseAddr->TIMSTAT = 1UL << timer;
}

/*!
 * @brief Returns the state of the interrupt for all shifters. Each bit in
 *        the returned value specifies the interrupt state for one shifter, starting with
 *        shifter 0 from least significant bit.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @return the state of the interrupt for all shifters
 */
static inline uint32_t EIO_GetAllShifterInterrupt(const EIO_Type *baseAddr)
{
    return baseAddr->SHIFTSIEN;
}

/*!
 * @brief Enable or disable specified shifter interrupts. The interrupt mask must contain a
 *        bit of 1 for each shifter who's interrupt must be enabled or disabled.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] interruptMask: value of interruptMask
 * @param[in] enable:enabled or disabled
 * @return none
 */
static inline void EIO_SetShifterInterrupt(EIO_Type *baseAddr, uint8_t interruptMask, bool enable)
{
    uint32_t tmp;

    tmp = baseAddr->SHIFTSIEN;

    if (enable == true)
    {
        tmp |= interruptMask;
    }
    else
    {
        tmp &= ~(uint32_t)interruptMask;
    }

    baseAddr->SHIFTSIEN = tmp;
}

/*!
 * @brief Returns the state of the error interrupt for all shifters. Each bit in
 *        the returned value specifies the interrupt state for one shifter, starting with
 *        shifter 0 from least significant bit.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @return the state of the error interrupt for all shifters
 */
static inline uint32_t EIO_GetAllShifterErrorInterrupt(const EIO_Type *baseAddr)
{
    return baseAddr->SHIFTEIEN;
}

/*!
 * @brief Enable or disable specified shifter error interrupts. The interrupt mask must contain a
 *        bit of 1 for each shifter who's error interrupt must be enabled or disabled.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] interruptMask: value of interruptMask
 * @param[in] enable:enabled or disabled
 * @return none
 */
static inline void EIO_SetShifterErrorInterrupt(EIO_Type *baseAddr, uint8_t interruptMask, bool enable)
{
    uint32_t tmp;

    tmp = baseAddr->SHIFTEIEN;

    if (enable == true)
    {
        tmp |= interruptMask;
    }
    else
    {
        tmp &= ~(uint32_t)interruptMask;
    }
    baseAddr->SHIFTEIEN = tmp;
}

/*!
 * @brief Returns the state of the interrupt for all timers. Each bit in
 *        the returned value specifies the interrupt state for one timer, starting with
 *        timer 0 from least significant bit.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @return the state of the interrupt for all timers
 */
static inline uint32_t EIO_GetAllTimerInterrupt(const EIO_Type *baseAddr)
{
    return baseAddr->TIMIEN;
}

/*!
 * @brief Enable or disable specified timer interrupts. The interrupt mask must contain a
 *        bit of 1 for each timer who's interrupt must be enabled or disabled.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] interruptMask: value of interruptMask
 * @param[in] enable:enabled or disabled
 * @return none
 */
static inline void EIO_SetTimerInterrupt(EIO_Type *baseAddr, uint8_t interruptMask, bool enable)
{
    uint32_t tmp;

    tmp = baseAddr->TIMIEN;

    if (enable == true)
    {
        tmp |= interruptMask;
    }
    else
    {
        tmp &= ~(uint32_t)interruptMask;
    }
    baseAddr->TIMIEN = tmp;
}

/*!
 * @brief Enable or disable specified shifter DMA requests. The request mask must contain a
 *        bit of 1 for each shifter who's DMA requests must be enabled or disabled.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] requestMask: value of requestMask
 * @param[in] enable:enabled or disabled
 * @return none
 */
static inline void EIO_SetShifterDMARequest(EIO_Type *baseAddr, uint8_t requestMask, bool enable)
{
    uint32_t tmp;

    tmp = baseAddr->SHIFTSDEN;

    if (enable == true)
    {
        tmp |= requestMask;
    }
    else
    {
        tmp &= ~(uint32_t)requestMask;
    }
    baseAddr->SHIFTSDEN = tmp;
}

/*!
 * @brief This function assigns a timer to control the specified shifter, and also configures its polarity.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] timer: Number of timer
 * @param[in] Polarity
 *            - 0: EIO_TIMER_POLARITY_POSEDGE
 *            - 1: EIO_TIMER_POLARITY_NEGEDGE
 * @return none
 */
static inline void EIO_SetShifterTimer(EIO_Type *baseAddr,
                                         uint8_t shifter,
                                         uint8_t timer,
                                         eio_timer_polarity_t polarity)
{
    uint32_t tmp = baseAddr->SHIFTCTL[shifter];
    tmp &= ~(EIO_SHIFTCTL0_TIMSEL_Msk);
    tmp |= EIO_SHIFTCTL_TIMSEL(timer);
    tmp &= ~(EIO_SHIFTCTL0_TIMPOL_Msk);
    tmp |= EIO_SHIFTCTL_TIMPOL(polarity);
    baseAddr->SHIFTCTL[shifter] = tmp;
}

/*!
 * @brief This function assigns the specified pin to the specified shifter, and also sets
 *        its polarity and configuration.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] pin: Number of pin
 * @param[in] pinPolarity
 *            - 0: EIO_PIN_POLARITY_HIGH
 *            - 1: EIO_PIN_POLARITY_LOW
 * @param[in] Config
 *            - 0: EIO_PIN_CONFIG_DISABLED
 *            - 1: EIO_PIN_CONFIG_OPEN_DRAIN
 *            - 2: EIO_PIN_CONFIG_BIDIR_OUTPUT
 *            - 3: EIO_PIN_CONFIG_OUTPUT
 * @return none
 */
static inline void EIO_SetShifterPin(EIO_Type *baseAddr,
                                       uint8_t shifter,
                                       uint8_t pin,
                                       eio_pin_polarity_t polarity,
                                       eio_pin_config_t config)
{
    uint32_t tmp = baseAddr->SHIFTCTL[shifter];
    tmp &= ~(EIO_SHIFTCTL0_PINSEL_Msk);
    tmp |= EIO_SHIFTCTL_PINSEL(pin);
    tmp &= ~(EIO_SHIFTCTL0_PINPOL_Msk);
    tmp |= EIO_SHIFTCTL_PINPOL(polarity);
    tmp &= ~(EIO_SHIFTCTL0_PINCFG_Msk);
    tmp |= EIO_SHIFTCTL_PINCFG(config);
    baseAddr->SHIFTCTL[shifter] = tmp;
}

/*!
 * @brief This function configures the pin assigned to the specified specified shifter.
 *        It does not change the other pin-related settings.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] config
 *            - 0: EIO_PIN_CONFIG_DISABLED
 *            - 1: EIO_PIN_CONFIG_OPEN_DRAIN
 *            - 2: EIO_PIN_CONFIG_BIDIR_OUTPUT
 *            - 3: EIO_PIN_CONFIG_OUTPUT
 * @return none
 */
static inline void EIO_SetShifterPinConfig(EIO_Type *baseAddr,
                                             uint8_t shifter,
                                             eio_pin_config_t config)
{
    uint32_t tmp = baseAddr->SHIFTCTL[shifter];
    tmp &= ~(EIO_SHIFTCTL0_PINCFG_Msk);
    tmp |= EIO_SHIFTCTL_PINCFG(config);
    baseAddr->SHIFTCTL[shifter] = tmp;
}

/*!
 * @brief This function configures the mode for the specified shifter.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] mode
 *            - 0: EIO_SHIFTER_MODE_DISABLED
 *            - 1: EIO_SHIFTER_MODE_RECEIVE
 *            - 2: EIO_SHIFTER_MODE_TRANSMIT
 *            - 4: EIO_SHIFTER_MODE_MATCH_STORE
 * @return none
 */
static inline void EIO_SetShifterMode(EIO_Type *baseAddr,
                                        uint8_t shifter,
                                        eio_shifter_mode_t mode)
{
    uint32_t tmp = baseAddr->SHIFTCTL[shifter];
    tmp &= ~(EIO_SHIFTCTL0_SMOD_Msk);
    tmp |= EIO_SHIFTCTL_SMOD(mode);
    baseAddr->SHIFTCTL[shifter] = tmp;
}

/*!
 * @brief This gey the mode for the specified shifter.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] mode
 *            - 0: EIO_SHIFTER_MODE_DISABLED
 *            - 1: EIO_SHIFTER_MODE_RECEIVE
 *            - 2: EIO_SHIFTER_MODE_TRANSMIT
 *            - 4: EIO_SHIFTER_MODE_MATCH_STORE
 * @return none
 */
static inline eio_shifter_mode_t EIO_GetShifterMode(const EIO_Type *baseAddr, uint8_t shifter)
{
    uint32_t tmp = baseAddr->SHIFTCTL[shifter];
    return (eio_shifter_mode_t)EIO_SHIFTCTL_SMOD(tmp);
}

/*!
 * @brief This function configures the control settings for the specified shifter: mode settings,
 *        pin settings and timer settings.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] mode
 *            - 0: EIO_SHIFTER_MODE_DISABLED
 *            - 1: EIO_SHIFTER_MODE_RECEIVE
 *            - 2: EIO_SHIFTER_MODE_TRANSMIT
 *            - 4: EIO_SHIFTER_MODE_MATCH_STORE
 * @param[in] pin: Number of pin
 * @param[in] pinPolarity
 *            - 0: EIO_PIN_POLARITY_HIGH
 *            - 1: EIO_PIN_POLARITY_LOW
 * @param[in] pinConfig
 *            - 0: EIO_PIN_CONFIG_DISABLED
 *            - 1: EIO_PIN_CONFIG_OPEN_DRAIN
 *            - 2: EIO_PIN_CONFIG_BIDIR_OUTPUT
 *            - 3: EIO_PIN_CONFIG_OUTPUT
 * @param[in] timer: Number of timer
 * @param[in] timerPolarity
 *            - 0: EIO_TIMER_POLARITY_POSEDGE
 *            - 1: EIO_TIMER_POLARITY_NEGEDGE
 * @return none
 */
static inline void EIO_SetShifterControl(EIO_Type *baseAddr,
                                           uint8_t shifter,
                                           eio_shifter_mode_t mode,
                                           uint8_t pin,
                                           eio_pin_polarity_t pinPolarity,
                                           eio_pin_config_t pinConfig,
                                           uint8_t timer,
                                           eio_timer_polarity_t timerPolarity)
{
    baseAddr->SHIFTCTL[shifter] = EIO_SHIFTCTL_SMOD(mode)
                                | EIO_SHIFTCTL_PINPOL(pinPolarity)
                                | EIO_SHIFTCTL_PINSEL(pin)
                                | EIO_SHIFTCTL_PINCFG(pinConfig)
                                | EIO_SHIFTCTL_TIMPOL(timerPolarity)
                                | EIO_SHIFTCTL_TIMSEL(timer);
}

/*!
 * @brief This function configures the input source of the specified shifter.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] source
 *            - 0: EIO_SHIFTER_SOURCE_PIN
 *            - 1: EIO_SHIFTER_SOURCE_SHIFTER
 * @return none
 */
static inline void EIO_SetShifterInputSource(EIO_Type *baseAddr,
                                               uint8_t shifter,
                                               eio_shifter_source_t source)
{
    uint32_t tmp;

    tmp = baseAddr->SHIFTCFG[shifter];
    tmp &= ~(EIO_SHIFTCFG0_INSRC_Msk);
    tmp |= EIO_SHIFTCFG_INSRC(source);

    baseAddr->SHIFTCFG[shifter] = tmp;
}

/*!
 * @brief This function configures the sending or receiving of
 *        a stop bit in Transmit, Receive or Match Store modes.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] stop
 *            - 0: EIO_SHIFTER_STOP_BIT_DISABLED
 *            - 2: EIO_SHIFTER_STOP_BIT_0
 *            - 3: EIO_SHIFTER_STOP_BIT_1
 * @return none
 */
static inline void EIO_SetShifterStopBit(EIO_Type *baseAddr, uint8_t shifter, eio_shifter_stop_t stop)
{
    uint32_t tmp;

    tmp = baseAddr->SHIFTCFG[shifter];
    tmp &= ~(EIO_SHIFTCFG0_SSTOP_Msk);
    tmp |= EIO_SHIFTCFG_SSTOP(stop);

    baseAddr->SHIFTCFG[shifter] = tmp;
}

/*!
 * @brief This function configures the sending or receiving of
 *        a start bit in Transmit, Receive or Match Store modes.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] start
 *            - 0: EIO_SHIFTER_START_BIT_DISABLED
 *            - 1: EIO_SHIFTER_START_BIT_DISABLED_SH
 *            - 2: EIO_SHIFTER_START_BIT_0
 *            - 3: EIO_SHIFTER_START_BIT_1
 * @return none
 */
static inline void EIO_SetShifterStartBit(EIO_Type *baseAddr,
                                            uint8_t shifter,
                                            eio_shifter_start_t start)
{
    uint32_t tmp;

    tmp = baseAddr->SHIFTCFG[shifter];
    tmp &= ~(EIO_SHIFTCFG0_SSTART_Msk);
    tmp |= EIO_SHIFTCFG_SSTART(start);

    baseAddr->SHIFTCFG[shifter] = tmp;
}

/*!
 * @brief This function sets the following configurations for the specified shifter:
 *        start bit, stop bit, input source
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] start
 *            - 0: EIO_SHIFTER_START_BIT_DISABLED
 *            - 1: EIO_SHIFTER_START_BIT_DISABLED_SH
 *            - 2: EIO_SHIFTER_START_BIT_0
 *            - 3: EIO_SHIFTER_START_BIT_1
 * @param[in] stop
 *            - 0: EIO_SHIFTER_STOP_BIT_DISABLED
 *            - 2: EIO_SHIFTER_STOP_BIT_0
 *            - 3: EIO_SHIFTER_STOP_BIT_1
 * @param[in] source
 *            - 0: EIO_SHIFTER_SOURCE_PIN
 *            - 1: EIO_SHIFTER_SOURCE_SHIFTER
 * @return none
 */
static inline void EIO_SetShifterConfig(EIO_Type *baseAddr,
                                          uint8_t shifter,
                                          eio_shifter_start_t start,
                                          eio_shifter_stop_t stop,
                                          eio_shifter_source_t source)
{
    baseAddr->SHIFTCFG[shifter] = EIO_SHIFTCFG_SSTART(start)
                                | EIO_SHIFTCFG_SSTOP(stop)
                                | EIO_SHIFTCFG_INSRC(source);
}

/*!
 * @brief This function reads data from the specified shifter buffer. The data can be
 *        read in any of the four ways allowed by the hardware - see description of type
 *        eio_shifter_buffer_mode_t for details.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] mode
 *            - 0: EIO_SHIFTER_RW_MODE_NORMAL
 *            - 1: EIO_SHIFTER_RW_MODE_BIT_SWAP
 * @return Read data value
 */
static inline uint32_t EIO_ReadShifterBuffer(const EIO_Type *baseAddr,
                                               uint8_t shifter,
                                               eio_shifter_buffer_mode_t mode)
{
    uint32_t data;

    switch (mode)
    {
        case EIO_SHIFTER_RW_MODE_NORMAL:
            data = baseAddr->SHIFTBUF[shifter];
            break;

        case EIO_SHIFTER_RW_MODE_BIT_SWAP:
            data = baseAddr->SHIFTBUFBIS[shifter];
            break;

        default:
            /* bad read mode */
            data = 0U;
        break;
    }
    return data;
}

/*!
 * @brief This function writes data in the specified shifter buffer. The data can be
 *        written in any of the four ways allowed by the hardware - see description of type
 *        eio_shifter_buffer_mode_t for details.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] shifter: Number of shifter
 * @param[in] value: Write data value
 * @param[in] mode
 *            - 0: EIO_SHIFTER_RW_MODE_NORMAL
 *            - 1: EIO_SHIFTER_RW_MODE_BIT_SWAP
 * @return none
 */
static inline void EIO_WriteShifterBuffer(EIO_Type *baseAddr,
                                            uint8_t shifter,
                                            uint32_t value,
                                            eio_shifter_buffer_mode_t mode)
{
    switch (mode)
    {
        case EIO_SHIFTER_RW_MODE_NORMAL:
            baseAddr->SHIFTBUF[shifter] = value;
            break;

        case EIO_SHIFTER_RW_MODE_BIT_SWAP:
            baseAddr->SHIFTBUFBIS[shifter] = value;
            break;

        default:
            /* bad write mode */
            break;
    }
}

/*!
 * @brief This function configures the trigger for the specified timer, and also its
 *        source (internal or external) and polarity settings.
 *        For internal triggers, the selection is as follows:
 *        - 4*N - Pin 2*N input
 *        - 4*N+1 - Shifter N status flag
 *        - 4*N+2 - Pin 2*N+1 input
 *        - 4*N+3 - Timer N trigger output
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] trigger: Number of trigger
 * @param[in] triggerPolarity
 *            - 0: EIO_TRIGGER_POLARITY_HIGH
 *            - 1: EIO_TRIGGER_POLARITY_LOW
 * @param[in] triggerSource
 *            - 0: EIO_TRIGGER_SOURCE_EXTERNAL
 *            - 1: EIO_TRIGGER_SOURCE_INTERNAL
 * @return none
 */
static inline void EIO_SetTimerTrigger(EIO_Type *baseAddr,
                                         uint8_t timer,
                                         uint8_t trigger,
                                         eio_trigger_polarity_t polarity,
                                         eio_trigger_source_t source)
{
    uint32_t tmp = baseAddr->TIMCTL[timer];

    tmp &= ~(EIO_TIMCTL0_TRGSEL_Msk);
    tmp |= EIO_TIMCTL_TRGSEL(trigger);
    tmp &= ~(EIO_TIMCTL0_TRGPOL_Msk);
    tmp |= EIO_TIMCTL_TRGPOL(polarity);
    tmp &= ~(EIO_TIMCTL0_TRGSRC_Msk);
    tmp |= EIO_TIMCTL_TRGSRC(source);

    baseAddr->TIMCTL[timer] = tmp;
}

/*!
 * @brief This function assigns the specified pin to the specified timer, and also sets
 *        its polarity and configuration.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] pin: Number of pin
 * @param[in] pinPolarity:
 *            - 0: EIO_PIN_POLARITY_HIGH
 *            - 1: EIO_PIN_POLARITY_LOW
 * @param[in] pinConfig:
 *            - 0: EIO_PIN_CONFIG_DISABLED
 *            - 1: EIO_PIN_CONFIG_OPEN_DRAIN
 *            - 2: EIO_PIN_CONFIG_BIDIR_OUTPUT
 *            - 3: EIO_PIN_CONFIG_OUTPUT
 * @return none
 */
static inline void EIO_SetTimerPin(EIO_Type *baseAddr,
                                     uint8_t timer,
                                     uint8_t pin,
                                     eio_pin_polarity_t polarity,
                                     eio_pin_config_t config)
{
    uint32_t tmp = baseAddr->TIMCTL[timer];

    tmp &= ~(EIO_TIMCTL0_PINSEL_Msk);
    tmp |= EIO_TIMCTL_PINSEL(pin);
    tmp &= ~(EIO_TIMCTL0_PINPOL_Msk);
    tmp |= EIO_TIMCTL_PINPOL(polarity);
    tmp &= ~(EIO_TIMCTL0_PINCFG_Msk);
    tmp |= EIO_TIMCTL_PINCFG(config);

    baseAddr->TIMCTL[timer] = tmp;
}

/*!
 * @brief This function configures the mode for the specified timer.
 *        In 8-bit counter mode, the lower 8-bits of the counter and compare register are used to
 *        configure the baud rate of the timer shift clock and the upper 8-bits are used to configure
 *        the shifter bit count.
 *        In 8-bit PWM mode, the lower 8-bits of the counter and compare
 *        register are used to configure the high period of the timer shift clock and the upper
 *        8-bits are used to configure the low period of the timer shift clock. The shifter bit count
 *        is configured using another timer or external signal.
 *        In 16-bit counter mode, the full 16-bits of the counter and compare register are used to
 *        configure either the baud rate of the shift clock or the shifter bit count.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] mode
 *            - 0: EIO_TIMER_MODE_DISABLED
 *            - 1: EIO_TIMER_MODE_8BIT_BAUD
 *            - 2: EIO_TIMER_MODE_8BIT_PWM
 *            - 3: EIO_TIMER_MODE_16BIT
 * @return none
 */
static inline void EIO_SetTimerMode(EIO_Type *baseAddr,
                                      uint8_t timer,
                                      eio_timer_mode_t mode)
{
    uint32_t tmp = baseAddr->TIMCTL[timer];

    tmp &= ~(EIO_TIMCTL0_TIMOD_Msk);
    tmp |= EIO_TIMCTL_TIMOD(mode);

    baseAddr->TIMCTL[timer] = tmp;
}

/*!
 * @brief This function configures the control settings for the specified timer: mode settings,
 *        pin settings and trigger settings.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] trigger: Number of trigger
 * @param[in] triggerPolarity
 *            - 0: EIO_TRIGGER_POLARITY_HIGH
 *            - 1: EIO_TRIGGER_POLARITY_LOW
 * @param[in] triggerSource
 *            - 0: EIO_TRIGGER_SOURCE_EXTERNAL
 *            - 1: EIO_TRIGGER_SOURCE_INTERNAL
 * @param[in] pin: Number of pin
 * @param[in] pinPolarity
 *            - 0: EIO_PIN_POLARITY_HIGH
 *            - 1: EIO_PIN_POLARITY_LOW
 * @param[in] pinConfig
 *            - 0: EIO_PIN_CONFIG_DISABLED
 *            - 1: EIO_PIN_CONFIG_OPEN_DRAIN
 *            - 2: EIO_PIN_CONFIG_BIDIR_OUTPUT
 *            - 3: EIO_PIN_CONFIG_OUTPUT
 * @param[in] mode
 *            - 0: EIO_TIMER_MODE_DISABLED
 *            - 1: EIO_TIMER_MODE_8BIT_BAUD
 *            - 2: EIO_TIMER_MODE_8BIT_PWM
 *            - 3: EIO_TIMER_MODE_16BIT
 * @return none
 */
static inline void EIO_SetTimerControl(EIO_Type *baseAddr,
                                         uint8_t timer,
                                         uint8_t trigger,
                                         eio_trigger_polarity_t triggerPolarity,
                                         eio_trigger_source_t triggerSource,
                                         uint8_t pin,
                                         eio_pin_polarity_t pinPolarity,
                                         eio_pin_config_t pinConfig,
                                         eio_timer_mode_t mode)
{
    baseAddr->TIMCTL[timer] = EIO_TIMCTL_TIMOD(mode)
                            | EIO_TIMCTL_PINSEL(pin)
                            | EIO_TIMCTL_PINPOL(pinPolarity)
                            | EIO_TIMCTL_PINCFG(pinConfig)
                            | EIO_TIMCTL_TRGSRC(triggerSource)
                            | EIO_TIMCTL_TRGPOL(triggerPolarity)
                            | EIO_TIMCTL_TRGSEL(trigger);
}

/*!
 * @brief This function configures the initial output of the specified timer and whether it is
 *        affected by the Timer reset.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] output
 *            - 0: EIO_TIMER_INITOUT_ONE
 *            - 1: EIO_TIMER_INITOUT_ZERO
 *            - 2: EIO_TIMER_INITOUT_ONE_RESET
 *            - 3: EIO_TIMER_INITOUT_ZERO_RESET
 * @return none
 */
static inline void EIO_SetTimerInitialOutput(EIO_Type *baseAddr,
                                               uint8_t timer,
                                               eio_timer_output_t output)
{
    uint32_t tmp = baseAddr->TIMCFG[timer];

    tmp &= ~(EIO_TIMCFG0_TIMOUT_Msk);
    tmp |= EIO_TIMCFG_TIMOUT(output);

    baseAddr->TIMCFG[timer] = tmp;
}

/*!
 * @brief This function configures the decrement condition for the specified timer and the source
 *        of the shift clock.
 *        See description of type eio_timer_decrement_t for a list of options.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] decrement
 *            - 0: EIO_TIMER_DECREMENT_CLK_SHIFT_TMR
 *            - 1: EIO_TIMER_DECREMENT_TRG_SHIFT_TMR
 *            - 2: EIO_TIMER_DECREMENT_PIN_SHIFT_PIN
 *            - 3: EIO_TIMER_DECREMENT_TRG_SHIFT_TRG
 * @return none
 */
static inline void EIO_SetTimerDecrement(EIO_Type *baseAddr,
                                           uint8_t timer,
                                           eio_timer_decrement_t decrement)
{
    uint32_t tmp = baseAddr->TIMCFG[timer];

    tmp &= ~(EIO_TIMCFG0_TIMDEC_Msk);
    tmp |= EIO_TIMCFG_TIMDEC(decrement);

    baseAddr->TIMCFG[timer] = tmp;
}

/*!
 * @brief This function configures the conditions that cause the timer counter (and optionally
 *        output) to be reset. In 8-bit counter mode, the timer reset will only reset the
 *        lower 8-bits that configure the baud rate. In all other modes, the timer reset
 *        will reset the full 16-bits of the counter.
 *        See description of type eio_timer_decrement_t for a list of options.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] reset
 *            - 0: EIO_TIMER_RESET_NEVER
 *            - 2: EIO_TIMER_RESET_PIN_OUT
 *            - 3: EIO_TIMER_RESET_TRG_OUT
 *            - 4: EIO_TIMER_RESET_PIN_RISING
 *            - 6: EIO_TIMER_RESET_TRG_RISING
 *            - 7: EIO_TIMER_RESET_TRG_BOTH
 * @return none
 */
static inline void EIO_SetTimerReset(EIO_Type *baseAddr,
                                       uint8_t timer,
                                       eio_timer_reset_t reset)
{
    uint32_t tmp = baseAddr->TIMCFG[timer];

    tmp &= ~(EIO_TIMCFG0_TIMRST_Msk);
    tmp |= EIO_TIMCFG_TIMRST(reset);

    baseAddr->TIMCFG[timer] = tmp;
}

/*!
 * @brief This function configures the condition that cause the specified timer to be disabled.
 *        See description of type eio_timer_disable_t for a list of options.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] disable
 *            - 0: EIO_TIMER_DISABLE_NEVER
 *            - 1: EIO_TIMER_DISABLE_TIM_DISABLE
 *            - 2: EIO_TIMER_DISABLE_TIM_CMP
 *            - 3: EIO_TIMER_DISABLE_TIM_CMP_TRG_LOW
 *            - 4: EIO_TIMER_DISABLE_PIN
 *            - 5: EIO_TIMER_DISABLE_PIN_TRG_HIGH
 *            - 6: EIO_TIMER_DISABLE_TRG
 * @return none
 */
static inline void EIO_SetTimerDisable(EIO_Type *baseAddr,
                                         uint8_t timer,
                                         eio_timer_disable_t disable)
{
    uint32_t tmp = baseAddr->TIMCFG[timer];

    tmp &= ~(EIO_TIMCFG0_TIMDIS_Msk);
    tmp |= EIO_TIMCFG_TIMDIS(disable);

    baseAddr->TIMCFG[timer] = tmp;
}

/*!
 * @brief This function configures the condition that cause the specified timer to be enabled
 *        and start decrementing.
 *        See description of type eio_timer_disable_t for a list of options.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] enable
 *            - 0: EIO_TIMER_ENABLE_ALWAYS
 *            - 1: EIO_TIMER_ENABLE_TIM_ENABLE
 *            - 2: EIO_TIMER_ENABLE_TRG_HIGH
 *            - 3: EIO_TIMER_ENABLE_TRG_PIN_HIGH
 *            - 4: EIO_TIMER_ENABLE_PIN_POSEDGE
 *            - 5: EIO_TIMER_ENABLE_PIN_POSEDGE_TRG_HIGH
 *            - 6: EIO_TIMER_ENABLE_TRG_POSEDGE
 *            - 7: EIO_TIMER_ENABLE_TRG_EDGE
 * @return none
 */
static inline void EIO_SetTimerEnable(EIO_Type *baseAddr,
                                        uint8_t timer,
                                        eio_timer_enable_t enable)
{
    uint32_t tmp = baseAddr->TIMCFG[timer];

    tmp &= ~(EIO_TIMCFG0_TIMENA_Msk);
    tmp |= EIO_TIMCFG_TIMENA(enable);

    baseAddr->TIMCFG[timer] = tmp;
}

/*!
 * @brief This function configures stop bit insertion for the specified timer.
 *        The stop bit can be added on a timer compare (between each word) or on a timer disable.
 *        When stop bit is enabled, configured shifters will output the contents of the stop bit
 *        when the timer is disabled. When stop bit is enabled on timer disable, the timer remains
 *        disabled until the next rising edge of the shift clock. If configured for both timer
 *        compare and timer disable, only one stop bit is inserted on timer disable.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] stop
 *            - 0: EIO_TIMER_STOP_BIT_DISABLED
 *            - 1: EIO_TIMER_STOP_BIT_TIM_CMP
 *            - 2: EIO_TIMER_STOP_BIT_TIM_DIS
 *            - 3: EIO_TIMER_STOP_BIT_TIM_CMP_DIS
 * @return none
 */
static inline void EIO_SetTimerStop(EIO_Type *baseAddr,
                                      uint8_t timer,
                                      eio_timer_stop_t stop)
{
    uint32_t tmp = baseAddr->TIMCFG[timer];

    tmp &= ~(EIO_TIMCFG0_TSTOP_Msk);
    tmp |= EIO_TIMCFG_TSTOP(stop);

    baseAddr->TIMCFG[timer] = tmp;
}

/*!
 * @brief This function configures start bit insertion for the specified timer.
 *        When start bit is enabled, configured shifters will output the contents of the start
 *        bit when the timer is enabled and the timer counter will reload from the compare
 *        register on the first rising edge of the shift clock.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] start
 *            - 0: EIO_TIMER_START_BIT_DISABLED
 *            - 1: EIO_TIMER_START_BIT_ENABLED
 * @return none
 */
static inline void EIO_SetTimerStart(EIO_Type *baseAddr,
                                       uint8_t timer,
                                       eio_timer_start_t start)
{
    uint32_t regValue = (uint32_t)baseAddr->TIMCFG[timer];

    regValue &= (uint32_t)(~(EIO_TIMCFG0_TSTART_Msk));
    regValue |= EIO_TIMCFG_TSTART(start);

    baseAddr->TIMCFG[timer] = (uint32_t)regValue;
}

/*!
 * @brief This function sets the following configurations for the specified timer:
 *        start bit, stop bit, enable condition, disable condition, reset condition,
 *        decrement condition, initial output
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] start
 *            - 0: EIO_TIMER_START_BIT_DISABLED
 *            - 1: EIO_TIMER_START_BIT_ENABLED
 * @param[in] stop:
 *            - 0: EIO_TIMER_STOP_BIT_DISABLED
 *            - 1: EIO_TIMER_STOP_BIT_TIM_CMP
 *            - 2: EIO_TIMER_STOP_BIT_TIM_DIS
 *            - 3: EIO_TIMER_STOP_BIT_TIM_CMP_DIS
 * @param[in] enable
 *            - 0: EIO_TIMER_ENABLE_ALWAYS
 *            - 1: EIO_TIMER_ENABLE_TIM_ENABLE
 *            - 2: EIO_TIMER_ENABLE_TRG_HIGH
 *            - 3: EIO_TIMER_ENABLE_TRG_PIN_HIGH
 *            - 4: EIO_TIMER_ENABLE_PIN_POSEDGE
 *            - 5: EIO_TIMER_ENABLE_PIN_POSEDGE_TRG_HIGH
 *            - 6: EIO_TIMER_ENABLE_TRG_POSEDGE
 *            - 7: EIO_TIMER_ENABLE_TRG_EDGE
 * @param[in] disable
 *            - 0: EIO_TIMER_DISABLE_NEVER
 *            - 1: EIO_TIMER_DISABLE_TIM_DISABLE
 *            - 2: EIO_TIMER_DISABLE_TIM_CMP
 *            - 3: EIO_TIMER_DISABLE_TIM_CMP_TRG_LOW
 *            - 4: EIO_TIMER_DISABLE_PIN
 *            - 5: EIO_TIMER_DISABLE_PIN_TRG_HIGH
 *            - 6: EIO_TIMER_DISABLE_TRG
 * @param[in] reset
 *            - 0: EIO_TIMER_RESET_NEVER
 *            - 2: EIO_TIMER_RESET_PIN_OUT
 *            - 3: EIO_TIMER_RESET_TRG_OUT
 *            - 4: EIO_TIMER_RESET_PIN_RISING
 *            - 6: EIO_TIMER_RESET_TRG_RISING
 *            - 7: EIO_TIMER_RESET_TRG_BOTH
 * @param[in] decrement
 *            - 0: EIO_TIMER_DECREMENT_CLK_SHIFT_TMR
 *            - 1: EIO_TIMER_DECREMENT_TRG_SHIFT_TMR
 *            - 2: EIO_TIMER_DECREMENT_PIN_SHIFT_PIN
 *            - 3: EIO_TIMER_DECREMENT_TRG_SHIFT_TRG
 * @param[in] output
 *            - 0: EIO_TIMER_INITOUT_ONE
 *            - 1: EIO_TIMER_INITOUT_ZERO
 *            - 2: EIO_TIMER_INITOUT_ONE_RESET
 *            - 3: EIO_TIMER_INITOUT_ZERO_RESET
 * @return none
 */
static inline void EIO_SetTimerConfig(EIO_Type *baseAddr,
                                        uint8_t timer,
                                        eio_timer_start_t start,
                                        eio_timer_stop_t stop,
                                        eio_timer_enable_t enable,
                                        eio_timer_disable_t disable,
                                        eio_timer_reset_t reset,
                                        eio_timer_decrement_t decrement,
                                        eio_timer_output_t output)
{

    baseAddr->TIMCFG[timer] = EIO_TIMCFG_TSTART(start)
                            | EIO_TIMCFG_TSTOP(stop)
                            | EIO_TIMCFG_TIMENA(enable)
                            | EIO_TIMCFG_TIMDIS(disable)
                            | EIO_TIMCFG_TIMRST(reset)
                            | EIO_TIMCFG_TIMDEC(decrement)
                            | EIO_TIMCFG_TIMOUT(output);
}

/*!
 * @brief This function returns the compare value currently set for the specified timer.
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @return Timer compare value
 */
static inline uint16_t EIO_GetTimerCompare(const EIO_Type *baseAddr, uint8_t timer)
{
    return (uint16_t)(baseAddr->TIMCMP[timer]);
}

/*!
 * @brief This function configures the compare value for the specified timer.
 *        The timer compare value is loaded into the timer counter when the timer is first enabled,
 *        when the timer is reset and when the timer decrements down to zero.
 *        In dual 8-bit counters baud/bit mode, the lower 8-bits configure the baud rate divider
 *        and the upper 8-bits configure the number of bits in each word.
 *        In dual 8-bit counters PWM mode, the lower 8-bits configure the high period of the output
 *        and the upper 8-bits configure the low period.
 *        In 16-bit counter mode, the compare value can be used to generate the baud rate divider
 *        (if shift clock source is timer output) or the number of bits in each word (when the shift
 *        clock source is a pin or trigger input).
 *
 * @param[in] baseAddr: EIO baseAddr
 * @param[in] timer: Number of timer
 * @param[in] value: Timer compare value
 * @return none
 */
static inline void EIO_SetTimerCompare(EIO_Type *baseAddr,
                                         uint8_t timer,
                                         uint16_t value)
{
    baseAddr->TIMCMP[timer] = (uint32_t)value;
}

/*!
 * @brief This function initializes all the registers of the EIO module to their reset value.
 *
 * @param[in] none
 * @return none
 */
static inline void EIO_Init(EIO_Type *baseAddr)
{
    /* Use software reset bit to reset the module */
    EIO_SetSoftwareReset(baseAddr, true);
    /* Control register is not affected by software reset */
    baseAddr->CTRL = 0x0U;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EIO_HW_H */

/* =============================================  EOF  ============================================== */
