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
 * @file uart_hw.c
 *
 * @brief This file provides uart hardware access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "uart_hw.h"
#include "ckgen_drv.h"

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table to save UART ckgen interface clocks. */
static const ckgen_clock_t s_uartBusClock[UART_INSTANCE_MAX] = UART_CKGEN_CLOCKS;

/* Table to save UART soft resets. */
static const ckgen_softreset_t s_uartSoftReset[UART_INSTANCE_MAX] = UART_SOFT_RESETS;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief UART initialize.
 *
 * @param[in] instance: UART instance
 * @return none
 */
void UART_Init(uint32_t instance)
{
    if (instance < UART_INSTANCE_MAX)
    {
        /* Enable the modue bus clock and release module from reset state */
        CKGEN_DRV_Enable(s_uartBusClock[instance], true);
        CKGEN_DRV_SoftReset(s_uartSoftReset[instance], true);
    }
}

/*!
 * @brief Uninitialize UART module.
 *
 * @param[in] instance: UART instance
 * @return none
 */
void UART_DeInit(uint32_t instance)
{
    if (instance < UART_INSTANCE_MAX)
    {
        /* Disable the modue bus clock and set module at reset state */
        CKGEN_DRV_SoftReset(s_uartSoftReset[instance], false);
        CKGEN_DRV_Enable(s_uartBusClock[instance], false);
    }
}

/*!
 * @brief UART set bit count per char.
 *
 * @param[in] base: UART base pointer
 * @param[in] bitCountPerChar: 7, 8 or 9 bits per char
 * @return none
 */
void UART_SetBitCountPerChar(UART_Type *base, uart_bit_count_per_char_t bitCountPerChar)
{
    MODIFY_REG32(base->LCR0, UART_LCR0_WLS1_WLS0_Msk, UART_LCR0_WLS1_WLS0_Pos, bitCountPerChar);
}

/*!
 * @brief UART set parity mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] parityModeType: parity disabled, odd or even
 * @return none
 */
void UART_SetParityMode(UART_Type *base, uart_parity_mode_t parity)
{
    uint32_t regVal = base->LCR0;

    base->LCR0 &= ~UART_LCR0_SP_Msk;

    switch (parity)
    {
    case UART_PARITY_ODD:                         /* UART_PARITY_ODD */
        regVal |= UART_LCR0_PEN_Msk;              /* parity enable */
        regVal &= ~UART_LCR0_EPS_Msk;             /* set EPS to odd */
        break;

    case UART_PARITY_EVEN:                        /* UART_PARITY_EVEN */
        regVal |= UART_LCR0_PEN_Msk;              /* parity enable */
        regVal |= UART_LCR0_EPS_Msk;              /* set EPS to even*/
        break;

    case UART_PARITY_DISABLED:                    /* UART_PARITY_DISABLED */
        regVal &= ~UART_LCR0_PEN_Msk;             /* parity disable */
        regVal &= ~UART_LCR0_EPS_Msk;             /* set EPS to odd*/
        break;

    default:
        regVal &= ~UART_LCR0_PEN_Msk;             /* parity disable */
        regVal &= ~UART_LCR0_EPS_Msk;             /* set EPS to odd*/
        break;
    }

    WRITE_REG32(base->LCR0, regVal);
}

/*!
 * @brief UART send 9-bits data.
 *
 * @param[in] base: UART base pointer
 * @param[in] data: The data which will be send
 * @return none
 */
void UART_Putchar9(UART_Type *base, uint16_t data)
{
    base->RBR = (data & UART_RBR_RBR_THR_Msk);
}

/*!
 * @brief UART get 9-bits data.
 *
 * @param[in] base: UART base pointer
 * @param[in] readData: Get data
 * @return none
 */
void UART_Getchar9(const UART_Type *base, uint16_t *readData)
{
    DEVICE_ASSERT(readData != NULL);

    /* get 9-bits data from uart RBR register */
    *readData = (uint16_t)((base->RBR) & UART_RBR_RBR_THR_Msk) ;
}

/*!
 * @brief Set UART baudrate divisor.
 *
 * @param[in] base: UART base pointer
 * @param[in] baudRateDivisor: UART baudrate divisor
 * @return  none
 */
void UART_SetBaudRateDivisor(UART_Type *base, float baudRateDivisor)
{
    uint32_t intDivisor = (uint32_t)baudRateDivisor;
    uint32_t fracDivisor = (uint32_t)((baudRateDivisor - intDivisor) * 32U);

    base->DIV_L = intDivisor & 0xFFU;
    base->DIV_H = (intDivisor >> 8U) & 0xFFU;

    base->DIV_FRAC = fracDivisor;
}

/*!
 * @brief Get UART sample counter value.
 *
 * @param[in] base: UART base pointer
 * @return sampleCnt: UART over sample counter value
 */
uint16_t UART_GetSampleCounter(UART_Type *base)
{
    uint16_t intVal = 0U;
    uart_sample_cnt_t sampleCnt = (uart_sample_cnt_t)(base->SMP_CNT & UART_SMP_CNT_SMP_CNT_Msk);

    switch (sampleCnt)
    {
    case UART_SMP_CNT16:
        intVal = 16U;
        break;

    case UART_SMP_CNT8:
        intVal = 8U;
        break;

    case UART_SMP_CNT4:
        intVal = 4U;
        break;

    default:
        break;
    }

    return intVal;
}

/*!
 * @brief Gets the UART baud rate divisor.
 *
 * @param[in] base: UART base pointer
 * @return The baud rate divisor
 */
float UART_GetBaudRateDivisor(const UART_Type *base)
{
    uint16_t intDivisor = 0U;
    float divisor = 0U;

    intDivisor = (uint16_t)((base->DIV_H & UART_DIV_H_DIV_H_Msk) << 8U);
    intDivisor |= (uint16_t)(base->DIV_L & UART_DIV_L_DIV_L_Msk);
    divisor = (float)intDivisor + (base->DIV_FRAC / (float)32);

    return divisor;
}

/*!
 * @brief Set UART interrupt mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] intSrc: UART interrupt sources
 * @param[in] enable: enable or disable
 * @return  none
 */
void UART_SetIntMode(UART_Type *base, uart_interrupt_t intSrc, bool enable)
{
    uint32_t reg = (uint32_t)(intSrc) >> UART_SHIFT;
    uint32_t intRegOffset = (uint16_t)(intSrc);

    switch (reg)
    {
    case UART_IER_REG_ID:
        base->IER = (base->IER & ~(1U << intRegOffset)) \
                     | ((enable ? 1U : 0U) << intRegOffset);
        break;

    case UART_MATCHCR_REG_ID:
        base->MTCHCR = (base->MTCHCR & ~(1U << intRegOffset)) \
                        | ((enable ? 1U : 0U) << intRegOffset);
        break;

    case UART_LINCR_REG_ID:
        base->LINCR = (base->LINCR & ~(1U << intRegOffset)) \
                        | ((enable ? 1U : 0U) << intRegOffset);
        break;

    default :
        break;
    }
}

/*!
 * @brief Get UART interrupt source is enabled or disabled.
 *
 * @param[in] base: UART base pointer
 * @param[in] intSrc: UART interrupt sources
 * @return  Whether the interrupt source is enabled or disabled
 */
bool UART_GetIntMode(const UART_Type *base, uart_interrupt_t intSrc)
{
    uint32_t reg = (uint32_t)(intSrc) >> UART_SHIFT;
    bool retVal = false;

    switch (reg)
    {
    case UART_IER_REG_ID:
        retVal = (((base->IER >> (uint16_t)(intSrc)) & 1U) > 0U);
        break;

    case UART_MATCHCR_REG_ID:
        retVal = (((base->MTCHCR >> (uint16_t)(intSrc)) & 1U) > 0U);
        break;

    case UART_LINCR_REG_ID:
        retVal = (((base->LINCR >> (uint16_t)(intSrc)) & 1U) > 0U);
        break;

    default:
        break;
    }

    return retVal;
}

/*!
 * @brief Get UART status flags.
 *
 * @param[in] base: UART base pointer
 * @param[in] statusFlag: Select which status flag
 * @return  The selected status flag is true or false
 */
bool UART_GetStatusFlag(const UART_Type *base, uart_status_flag_t statusFlag)
{
    uint32_t reg = (uint32_t)(statusFlag) >> UART_SHIFT;
    bool retVal = false;

    switch ( reg )
    {
    case UART_LSR0_REG_ID:
        retVal = (((base->LSR0 >> (uint16_t)(statusFlag)) & 1U) > 0U);
        break;

    case UART_LSR1_REG_ID:
        retVal = (((base->LSR1 >> (uint16_t)(statusFlag)) & 1U) > 0U);
        break;

    default:
        break;
    }

    return retVal;
}

/*!
 * @brief Clear UART status flags.
 *
 * @param[in] base: UART base pointer
 * @param[in] statusFlag: Select which status flag to be cleared
 * @return  STATUS_SUCCESS or STATUS_ERROR
 */
status_t UART_ClearStatusFlag(UART_Type *base, uart_status_flag_t statusFlag)
{
    status_t returnCode = STATUS_SUCCESS;

    uint32_t reg = (uint32_t)(statusFlag) >> UART_SHIFT;

    switch (statusFlag)
    {
    /* These flags will be cleared by hardware */
    case UART_RX_DATA_READY:
    case UART_TX_DATA_NOT_FULL:
    case UART_TX_COMPLETE:
    case UART_TX_DATA_FULL:
    case UART_IDLE:
    case UART_CTS:
    case UART_RTS:
        returnCode = STATUS_ERROR;
        break;

    default: /* Write "1" to cleared */
        if (UART_LSR0_REG_ID == reg)
        {
            base->LSR0 = 1U << (uint16_t)(statusFlag);
        }
        else /* UART_LSR1_REG_ID */
        {
            base->LSR1 = 1U << (uint16_t)(statusFlag);
        }
        break;
    }

    return (returnCode);
}

/*!
 * @brief UART set error interrupts.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable error interrupts
 * @return none
 */
void UART_SetErrorInterrupts(UART_Type *base, bool enable)
{
    /* Configure the error interrupts */
    UART_SetIntMode(base, UART_INT_RX_OVERRUN, enable);
    UART_SetIntMode(base, UART_INT_PARITY_ERR, enable);
    UART_SetIntMode(base, UART_INT_NOISE_ERR, enable);
    UART_SetIntMode(base, UART_INT_FRAME_ERR, enable);
}

/*!
 * @brief Set UART address filter function
 *
 * @param[in] base: UART base pointer
 * @param[in] addr: UART match address
 * @param[in] enable: Enable or disable address filter function
 * @return none
 */
void UART_SetAddrFilter(UART_Type *base, uint16_t addr, bool enable)
{
    WRITE_REG32(base->ADDR, addr & UART_ADDR_ADDR_Msk);

    if (enable)
    {
        SET_BIT32(base->MTCHCR, UART_MTCHCR_ADDMTCH_EN_Msk);
    }
    else
    {
        CLEAR_BIT32(base->MTCHCR, UART_MTCHCR_ADDMTCH_EN_Msk);
    }
}

/*!
 * @brief Set UART data match function.
 *
 * @param[in] base: UART base pointer
 * @param[in] addr: UART match data
 * @param[in] enable: Enable or disable data match function
 * @return none
 */
void UART_SetDataMatch(UART_Type *base, uint16_t data, bool enable)
{
    WRITE_REG32(base->DATA, data & UART_DATA_DATA_Msk);

    if (enable)
    {
        SET_BIT32(base->MTCHCR, UART_MTCHCR_DTMTCH_EN_Msk);
    }
    else
    {
        CLEAR_BIT32(base->MTCHCR, UART_MTCHCR_DTMTCH_EN_Msk);
    }
}

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
void UART_SetCTSRTS(UART_Type *base, uart_rts_cts_t rtsCts)
{
    uint32_t regVal = base->EFR;

    switch (rtsCts)
    {
    case UART_RTSCTS_NONE:            /* NO RTS or CTS */
        regVal &= ~UART_EFR_AUTO_RTS_Msk;
        regVal &= ~UART_EFR_AUTO_CTS_Msk;
        break;

    case UART_RTS_ONLY:               /* RTS only */
        regVal |= UART_EFR_AUTO_RTS_Msk;
        regVal &= ~UART_EFR_AUTO_CTS_Msk;
        break;

    case UART_CTS_ONLY:               /* CTS only */
        regVal &= ~UART_EFR_AUTO_RTS_Msk;
        regVal |= UART_EFR_AUTO_CTS_Msk;
        break;

    case UART_RTSCTS_ALL:             /* Enable RTS and CTS */
        regVal |= UART_EFR_AUTO_RTS_Msk;
        regVal |= UART_EFR_AUTO_CTS_Msk;
        break;

    default:
        break;
    }

    WRITE_REG32(base->EFR, regVal);
}

/*!
 * @brief Set UART single-wire mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable single-wire mode
 * @return none
 */
void UART_SetSingleWire(UART_Type *base, bool enable)
{
    if (enable)
    {
        SET_BIT32(base->LCR1, UART_LCR1_RSRC_Msk);
        SET_BIT32(base->LCR1, UART_LCR1_LOOP_Msk);
    }
    else
    {
        CLEAR_BIT32(base->LCR1, UART_LCR1_RSRC_Msk);
        CLEAR_BIT32(base->LCR1, UART_LCR1_LOOP_Msk);
    }
}

/*!
 * @brief Set UART loop mode.
 *
 * @param[in] base: UART base pointer
 * @param[in] enable: Enable or disable loop mode
 * @return none
 */
void UART_SetLoop(UART_Type *base, bool enable)
{
    CLEAR_BIT32(base->LCR1, UART_LCR1_RSRC_Msk);

    if (enable)
    {
        SET_BIT32(base->LCR1, UART_LCR1_LOOP_Msk);
    }
    else
    {
        CLEAR_BIT32(base->LCR1, UART_LCR1_LOOP_Msk);
    }
}

/* =============================================  EOF  ============================================== */
