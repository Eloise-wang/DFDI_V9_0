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
 * @file uart_hw.h
 *
 * @brief This file provides uart hardware integration functions.
 *
 */

#ifndef _UART_HW_H
#define _UART_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "uart_drv.h"
#include "lin_drv.h"

/* ============================================  Define  ============================================ */
#define UART_SHIFT              (16UL)
#define UART_LSR0_REG_ID        (1UL)
#define UART_LSR1_REG_ID        (2UL)
#define UART_IER_REG_ID         (3UL)
#define UART_MATCHCR_REG_ID     (4UL)
#define UART_LINCR_REG_ID       (5UL)

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief UART status flag structure.
 */
typedef enum
{
    UART_RX_DATA_READY              = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_DR_Pos),
    UART_RX_OVERRUN                 = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_OE_Pos),
    UART_PARITY_ERR                 = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_PE_Pos),
    UART_FRAME_ERR                  = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_FE_Pos),
    UART_BREAK_ERR                  = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_BI_Pos),
    UART_TX_DATA_NOT_FULL           = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_TXNF_Pos),
    UART_TX_COMPLETE                = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_TC_Pos),
    UART_NOISE_ERR                  = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_NE_Pos),
    UART_TX_DATA_FULL               = (((uint32_t)UART_LSR0_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR0_TXDF_Pos),
    UART_IDLE_LINE                  = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_IDLE_Pos),
    UART_LIN_SYNC_ERR               = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_SYNERR_Pos),
    UART_LIN_BREAK                  = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_FBRK_Pos),
    UART_CTS_CHANGE                 = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_DCTS_Pos),
    UART_LIN_WAKEUP                 = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_LINWAK_Pos),
    UART_IDLE                       = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_UART_IDLE_Pos),
    UART_CTS                        = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_CTS_Pos),
    UART_RTS                        = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_RTS_Pos),
    UART_DATA_MATCH                 = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_DTMTCH_Pos),
    UART_ADDR_FILTER_MATCH          = (((uint32_t)UART_LSR1_REG_ID << (uint32_t)UART_SHIFT) \
                                        | (uint32_t)UART_LSR1_ADDMTCH_Pos)
} uart_status_flag_t;

/*!
 * @brief UART interrupt configuration structure.
 */
typedef enum
{
    UART_INT_RX_NOT_EMPTY      = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_ERXNE_Pos),
    UART_INT_TX_NOT_FULL       = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_ETXNF_Pos),
    UART_INT_TX_COMPLETE       = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_ETC_Pos),
    UART_INT_PARITY_ERR        = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_EPE_Pos),
    UART_INT_FRAME_ERR         = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_EFE_Pos),
    UART_INT_NOISE_ERR         = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_ENE_Pos),
    UART_INT_RX_OVERRUN        = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_EOEBI_Pos),
    UART_INT_CTS_CHANGE        = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_EDCTS_Pos),
    UART_INT_TX_FULL           = (((uint32_t)UART_IER_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_IER_ETXDF_Pos),
    UART_INT_DATA_MATCH        = (((uint32_t)UART_MATCHCR_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_MTCHCR_MTCHIE_Pos),
    UART_INT_LIN_WAKEUP        = (((uint32_t)UART_LINCR_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_LINCR_LINWAKIE_Pos),
    UART_INT_LIN_SYNC_ERR      = (((uint32_t)UART_LINCR_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_LINCR_SYNERRIE_Pos),
    UART_INT_LIN_BREAK         = (((uint32_t)UART_LINCR_REG_ID << (uint32_t)UART_SHIFT) \
                                   | (uint32_t)UART_LINCR_LBRKIE_Pos),
} uart_interrupt_t;

/*!
 * @brief UART sample counter enumeration.
 */
typedef enum {
    UART_SMP_CNT16 = 0U,   /*!< SAM_CNT0: based on 16*baud_pulse, baud_rate = APB CLOCK/16/{DIV_H, DIV_L} */
    UART_SMP_CNT8,         /*!< SAM_CNT1: based on 8*baud_pulse, baud_rate = APB CLOCK/8/{DIV_H, DIV_L} */
    UART_SMP_CNT4,         /*!< SAM_CNT2: based on 4*baud_pulse, baud_rate = APB CLOCK/4/{DIV_H, DIV_L} */
}uart_sample_cnt_t;

/*!
 * @brief UART TX PIN direction enumeration.
 */
typedef enum {
    UART_TX_PIN_DIR_INPUT = 0U,   /*!< TX PIN as input in single-wire mode */
    UART_TX_PIN_DIR_OUTPUT,       /*!< TX PIN as output in single-wire mode */
}uart_tx_pin_dir_t;

/*!
 * @brief UART hardware flow enumeration.
 */
typedef enum {
    UART_RTSCTS_NONE = 0U,   /*!< RTS CTS: all disable */
    UART_RTS_ONLY,           /*!< RTS noly */
    UART_CTS_ONLY,           /*!< CTS noly */
    UART_RTSCTS_ALL          /*!< RTS CTS: all Enable */
}uart_rts_cts_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief UART initialize
 *
 * @param[in] instance: UART instance
 * @return none
 */
void UART_Init(uint32_t instance);

/*!
 * @brief Uninitialize UART module.
 *
 * @param[in] instance: UART instance
 * @return none
 */
void UART_DeInit(uint32_t instance);

/*!
 * @brief Get UART sample counter value.
 *
 * @param[in] base: UART base pointer
 * @return sampleCnt: UART over sample counter value
 */
uint16_t UART_GetSampleCounter(UART_Type *base);

/*!
 * @brief Set UART baudrate divisor.
 *
 * @param[in] base: UART base pointer
 * @param[in] baudRateDivisor: UART baudrate divisor
 * @return  none
 */
void UART_SetBaudRateDivisor(UART_Type *base, float baudRateDivisor);

/*!
 * @brief UART set bit count per char.
 *
 * @param[in] base: UART base pointer
 * @param[in] bitCountPerChar: 7, 8 or 9 bits per char
 * @return none
 */
void UART_SetBitCountPerChar(UART_Type *base, uart_bit_count_per_char_t bitCountPerChar);

/*!
 * @brief UART set parity mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] parity: parity disabled, odd or even
 * @return none
 */
void UART_SetParityMode(UART_Type *base, uart_parity_mode_t parity);

/*!
 * @brief Gets the UART baud rate divisor.
 *
 * @param[in] base: UART base pointer
 * @return The baud rate divisor
 */
float UART_GetBaudRateDivisor(const UART_Type *base);

/*!
 * @brief Set UART interrupt mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] intSrc: UART interrupt sources
 * @param[in] enable: enable or disable
 * @return  none
 */
void UART_SetIntMode(UART_Type *base, uart_interrupt_t intSrc, bool enable);

/*!
 * @brief Get UART interrupt source is enabled or disabled.
 *
 * @param[in] base: UART base pointer
 * @param[in] intSrc: UART interrupt sources
 * @return  Whether the interrupt source is enabled or disabled
 */
bool UART_GetIntMode(const UART_Type *base, uart_interrupt_t intSrc);

/*!
 * @brief UART send 9-bits data.
 *
 * @param[in] base: UART base pointer
 * @param[in] data: The data which will be send
 * @return none
 */
void UART_Putchar9(UART_Type *base, uint16_t data);

/*!
 * @brief UART get 9-bits data.
 *
 * @param[in] base: UART base pointer
 * @param[in] readData: Get data
 * @return none
 */
void UART_Getchar9(const UART_Type *base, uint16_t *readData);

/*!
 * @brief Get UART status flags.
 *
 * @param[in] base: UART base pointer
 * @param[in] statusFlag: Select which status flag
 * @return  The selected status flag is true or false
 */
bool UART_GetStatusFlag(const UART_Type *base, uart_status_flag_t statusFlag);

/*!
 * @brief Clear UART status flags.
 *
 * @param[in] base: UART base pointer
 * @param[in] statusFlag: Select which status flag to be cleared
 * @return  STATUS_SUCCESS or STATUS_ERROR
 */
status_t UART_ClearStatusFlag(UART_Type *base, uart_status_flag_t statusFlag);

/*!
 * @brief Enable or disable the UART error interrupts.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable error interrupts
 * @return none
 */
void UART_SetErrorInterrupts(UART_Type *base, bool enable);

/*!
 * @brief Set UART address filter function.
 *
 * @param[in] base: UART base pointer
 * @param[in] addr: UART match address
 * @param[in] enable: Enable or disable address filter function
 * @return none
 */
void UART_SetAddrFilter(UART_Type *base, uint16_t addr, bool enable);

/*!
 * @brief Set UART data match function.
 *
 * @param[in] base: UART base pointer
 * @param[in] addr: UART match data
 * @param[in] enable: Enable or disable data match function
 * @return none
 */
void UART_SetDataMatch(UART_Type *base, uint16_t data, bool enable);

/*!
 * @brief Set UART hardware flow control function.
 *
 * @param[in] base: UART base pointer
 * @param[in] rtsCts: Set UART hardware flow control function
 *                   -UART_RTSCTS_NONE: NO RTS or CTS
 *                   -UART_RTS_ONLY: RTS only
 *                   -UART_CTS_ONLY: CTS only
 *                   -UART_RTSCTS_ALL: Enable RTS and CTS
 * @return none
 */
void UART_SetCTSRTS(UART_Type *base, uart_rts_cts_t rtsCts);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enable/Disable the UART transmitter.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable transmitter
 * @return none
 */
static inline void UART_SetTransmitterCmd(UART_Type *base, bool enable)
{
    base->LCR1 = (base->LCR1 & ~UART_LCR1_TXEN_Msk) | ((enable ? 1UL : 0UL) << UART_LCR1_TXEN_Pos);
    /* Wait for the register write operation to complete */
    while ((bool)((base->LCR1 & UART_LCR1_TXEN_Msk) != 0U) != enable)
    {
        /* Do nothing */
    }
}

/*!
 * @brief Enable/Disable the UART receiver.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable receiver
 * @return none
 */
static inline void UART_SetReceiverCmd(UART_Type *base, bool enable)
{
    base->LCR1 = (base->LCR1 & ~UART_LCR1_RXEN_Msk) | ((enable ? 1UL : 0UL) << UART_LCR1_RXEN_Pos);
    /* Wait for the register write operation to complete */
    while ((bool)((base->LCR1 & UART_LCR1_RXEN_Msk) != 0U) != enable)
    {
        /* Do nothing */
    }
}

/*!
 * @brief Set UART sample counter.
 *
 * @param[in] base: UART base pointer
 * @param[in] sampleCnt: UART over sample counter
 * @return  none
 */
static inline void UART_SetSampleCounter(UART_Type *base, uart_sample_cnt_t sampleCnt)
{
    base->SMP_CNT = ((uint32_t)sampleCnt & UART_SMP_CNT_SMP_CNT_Msk);
}

/*!
 * @brief Configures the number of stop bits.
 *
 * @param[in] base: UART base pointer
 * @param[in] stopBitCount: Number of stop bits (1 or 2)
 */
static inline void UART_SetStopBitCount(UART_Type *base, uart_stop_bit_count_t stopBitCount)
{
    MODIFY_REG32(base->LCR0, UART_LCR0_STB_Msk, UART_LCR0_STB_Pos, stopBitCount);
}

/*!
 * @brief Configures UART transmit DMA request.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Transmit DMA request configuration
 */
static inline void UART_SetTxDmaCmd(UART_Type *base, bool enable)
{
    base->DMA_EN = (base->DMA_EN & ~UART_DMA_EN_TX_DMA_EN_Msk) | ((enable ? 1UL : 0UL) << UART_DMA_EN_TX_DMA_EN_Pos);
}

/*!
 * @brief Configures UART receive DMA request.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Receive DMA request configuration (enable: 1/disable: 0)
 */
static inline void UART_SetRxDmaCmd(UART_Type *base, bool enable)
{
    base->DMA_EN = (base->DMA_EN & ~UART_DMA_EN_RX_DMA_EN_Msk) | ((enable ? 1UL : 0UL) << UART_DMA_EN_RX_DMA_EN_Pos);
}

/*!
 * @brief Sends the UART 7/8-bit character.
 *
 * @param[in] base: UART Instance
 * @param[in] data: data to send (7/8-bit)
 * @return none
 */
static inline void UART_Putchar(UART_Type *base, uint8_t data)
{
    base->RBR = (data & UART_RBR_RBR_THR_Msk);
}

/*!
 * @brief Gets the UART 7/8-bit character.
 *
 * @param[in] base: UART base pointer
 * @param[in] readData: Data read from receiver (7/8-bit)
 * @return none
 */
static inline void UART_Getchar(const UART_Type *base, uint8_t *readData)
{
    DEVICE_ASSERT(readData != NULL);

    *readData = (uint8_t)base->RBR;
}

/*!
 * @brief Enable or disable UART fifo.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: enable or disable UART fifo
 * @return none
 */
static inline void UART_SetFIFO(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->FCR, UART_FCR_FIFOE_Msk, UART_FCR_FIFOE_Pos, enable);
}

/*!
 * @brief  Clears the error status flags.
 *
 * @param[in] base: UART base pointer
 * @return none
 */
static inline void UART_DRV_ClearErrorFlags(UART_Type *base)
{
    uint32_t mask = UART_LSR0_OE_Msk \
                     | UART_LSR0_PE_Msk \
                     | UART_LSR0_FE_Msk \
                     | UART_LSR0_BI_Msk \
                     | UART_LSR0_NE_Msk;

    base->LSR0 = mask;
}

/*!
 * @brief UART inverse polarity for tx output.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable tx inverse function
 * @return none
 */
static inline void UART_SetInvTx(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->LCR1, UART_LCR1_INVTX_Msk, UART_LCR1_INVTX_Pos, enable);
}

/*!
 * @brief UART inverse polarity for the rx input.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: enable or disable rx inverse function
 * @return none
 */
static inline void UART_SetInvRx(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->LCR1, UART_LCR1_INVRX_Msk, UART_LCR1_INVRX_Pos, enable);
}

/*!
 * @brief Set UART single-wire mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable single-wire mode
 * @return none
 */
void UART_SetSingleWire(UART_Type *base, bool enable);

/*!
 * @brief UART TX pin direction config in single-wire mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] direction: Set tx pin direction in single-wire mode
 *                       - UART_TX_PIN_DIR_INPUT
 *                       - UART_TX_PIN_DIR_OUTPUT
 * @return none
 */
static inline void UART_SetTxDir(UART_Type *base, uart_tx_pin_dir_t direction)
{
    MODIFY_REG32(base->LCR1, UART_LCR1_TXDIR_Msk, UART_LCR1_TXDIR_Pos, direction);
}

/*!
 * @brief Set UART loop mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable loop mode
 * @return none
 */
void UART_SetLoop(UART_Type *base, bool enable);

/*!
 * @brief Set UART Idle line detect function enable.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable/disable UART Idle line detect function
 * @return none
 */
static inline void UART_SetIdleFunc(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->IDLE, UART_IDLE_IDLEEN_Msk, UART_IDLE_IDLEEN_Pos, enable);
}

/*!
 * @brief Set UART idle interrupt.
 *        Note: Please enable idle function first by called function
 *              UART_SetIdleFun, otherwise idle interrupt is invalid.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable/disable idle interrupt
 * @return none
 */
static inline void UART_SetIdleInterrupt(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->IDLE, UART_IDLE_IDLEIE_Msk, UART_IDLE_IDLEIE_Pos, enable);
}

/*!
 * @brief Enable/disable UART IrDA function.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable/disable UART IrDA function
 * @return none
 */
static inline void UART_SetIrDA(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->LCR1, UART_LCR1_IREN_Msk, UART_LCR1_IREN_Pos, enable);
}

/*!
 * @brief Set IrDA receiver minimum pulse width.
 *
 * @param[in] base: UART base pointer
 * @param[in] pulse: IrDA receiver minimum pulse width(range: 0~65535)
 * @return none
 */
static inline void UART_SetIrDARxWidth(UART_Type *base, uint16_t pulse)
{
    WRITE_REG32(base->PMIN0, pulse & UART_PMIN0_PMIN0_Msk);
    WRITE_REG32(base->PMIN1, (pulse >> 8U) & UART_PMIN1_PMIN1_Msk);
}

/*!
 * @brief Enable or disable RS485 function.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable RS485 function
 *                    -true: enable RS485 function
 *                    -false: disable RS485 function
 * @return none
 */
static inline void UART_SetRS485(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->RS485CR, UART_RS485CR_RS485EN_Msk, UART_RS485CR_RS485EN_Pos, enable);
}

/*!
 * @brief Set RS485 RTS polarity.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable/disable/ UART RS485 RTS invert polarity
 *                    -true: enable RS485 INVPOL
 *                    -false: disable RS485 INVPOL
 * @return none
 */
static inline void UART_SetRS485Invpol(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->RS485CR, UART_RS485CR_INVPOL_Msk, UART_RS485CR_INVPOL_Pos, enable);
}

/*!
 * @brief Set RS485 delay function.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: enable or disable RS485 delay function
 *                    -true: enable RS485 delay
 *                    -false: disable RS485 delay
 * @return none
 */
static inline void UART_SetRS485Delay(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->RS485CR, UART_RS485CR_DLYEN_Msk, UART_RS485CR_DLYEN_Pos, enable);
}

/*!
 * @brief Set RS485 delay time.
 *
 * @param[in] base: UART base pointer
 * @param[in] counter: RS485 delay time[range 0~255]
 * @return none
 */
static inline void UART_SetRS485DelayCnt(UART_Type *base, uint8_t counter)
{
    WRITE_REG32(base->CNTR, counter);
}

/*!
 * @brief Set RS484 Guart Time and set Guard enable.
 *
 * @param[in] base: UART base pointer
 * @param[in] guardTime: UART guard time enable and set guard time
 *                    -bit[4]:  1: enable  0: disable
 *                    -bit[3:0]: GuardTime: 0 to 15
 * @return none
 */
static inline void UART_SetRS485Guard(UART_Type *base, uint8_t guardTime)
{
    WRITE_REG32(base->GUARD, guardTime & 0x1FU);
}

/*!
 * @brief Set UART LIN control.
 *
 * @param[in] base: UART base pointer
 * @param[in] linCtrl: UART linCtrl, the corresponding control bit means
 *                   - bit7: LIN mode enable
 *                   - bit6: LIN break character detect interrupt enable
 *                   - bit5: LIN break detect threshold 0:10bits 1:11bits
 *                   - bit3: LIN auto sync baudrate enable
 *                   - bit2: LIN sync byte error interrupt enable
 *                   - bit1: LIN wake up interrupt enable
 *                   - bit0: LIN sleep mode enable
 * @return none
 */
static inline void UART_SetLINCtrl(UART_Type *base, uint8_t linCtrl)
{
    MODIFY_REG32(base->LINCR, 0xFFU, 0U, linCtrl);
}

/*!
 * @brief Set LIN sleep mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: LIN sleep mode enable
 * @return none
 */
static inline void UART_SetLINSleep(UART_Type *base, bool enable)
{
    MODIFY_REG32(base->LINCR, UART_LINCR_LINSLP_Msk, UART_LINCR_LINSLP_Pos, enable);
}

/*!
 * @brief Get LIN sleep mode state.
 *
 * @param[in] base: UART base pointer
 * @return LIN sleep mode state
 */
static inline bool UART_GetLINSleepState(UART_Type *base)
{
    return (((base->LINCR & UART_LINCR_LINSLP_Msk) >> UART_LINCR_LINSLP_Pos) != 0U);
}

/*!
 * @brief Send LIN break.
 *
 * @param[in] base: UART base pointer
 * @return none
 */
static inline void UART_SendLINBreak(UART_Type *base)
{
    SET_BIT32(base->LINCR, UART_LINCR_SDBRK_Msk);
}

/*!
 * @brief Set LIN break threshold length for slave detect.
 *
 * @param[in] base: UART base pointer
 * @param[in] length:LIN break threshold length
 * @return none
 */
static inline void UART_SetLINBreakThreshold(UART_Type *base, lin_break_threshold_t length)
{
    MODIFY_REG32(base->LINCR, UART_LINCR_LBRKDL_Msk, UART_LINCR_LBRKDL_Pos, (uint32_t)length);
}

/*!
 * @brief Set LIN break length for master send.
 *
 * @param[in] base: UART base pointer
 * @param[in] length:LIN break length
 * @return none
 */
static inline void UART_SetLINBreakLength(UART_Type *base, lin_break_length_t length)
{
    MODIFY_REG32(base->BRKLGH, UART_BRKLGH_BRKLGH_Msk, UART_BRKLGH_BRKLGH_Pos, (uint32_t)length);
}

/*!
 * @brief Get LSR0 register value.
 *
 * @param[in] base: UART base pointer
 * @return LSR0 register value
 */
static inline uint32_t UART_GetLSR0(UART_Type *base)
{
    return (base->LSR0);
}

/*!
 * @brief Get LSR1 register value.
 *
 * @param[in] base: UART base pointer
 * @return LSR1 register value
 */
static inline uint32_t UART_GetLSR1(UART_Type *base)
{
    return (base->LSR1);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UART_HW_H */

/* =============================================  EOF  ============================================== */
