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

#ifndef _I2C_HW_H
#define _I2C_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================  Include  ============================================= */
#include "i2c_drv.h"

/* ============================================  Define  ============================================ */
/*!
 * @brief I2C hardware status macro.
 */
#define I2C_HW_STATUS_ERROR_NULL (0x00UL)                    /*!< I2C hardware status : without error */
#define I2C_HW_STATUS_BUS_BUSY (0x01UL)                      /*!< I2C hardware status : bus busy */
#define I2C_HW_STATUS_NACK (0x02UL)                          /*!< I2C hardware status : get nack */
#define I2C_HW_STATUS_BND_TIMEOUT (0x04UL)                   /*!< I2C hardware status : one byte transfer timeout error */
#define I2C_HW_STATUS_CORE_READY_TIMROUT (0x08UL)            /*!< I2C hardware status : wait ready timeout error */
#define I2C_HW_STATUS_START_TO_BUSY_TIMEOUT (0x10UL)         /*!< I2C hardware status : send start to busy timeout error */
#define I2C_HW_STATUS_STOP_TO_IDLE_TIMEOUT (0x20UL)          /*!< I2C hardware status : send stop to idle timeout error */
#define I2C_HW_STATUS_START_TO_STATUSFLAG_TIMEOUT (0x40UL)   /*!< I2C hardware status : send start to STATUS flag timeout error */

/*!
* @brief I2C hardware timeout count macro.
*/
#define I2C_HW_DEADLINE_TIMEOUT (0x80000UL)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Send start signal for master.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_Start(I2C_Type * const base);

/*!
 * @brief Send re-start signal for master.
 *        Make sure to disable start/stop interrupt when use the interface
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_Restart(I2C_Type * const base);

/*!
 * @brief Send stop signal for master.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_Stop(I2C_Type * const base);

/*!
 * @brief Wait one byte transmit finished.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_WaitOneByteFinished(I2C_Type * const base);

/*!
 * @brief Write one byte with polling BND flag,
 *        make sure to disable interrupt when use the interface.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_TransmitOneByte(I2C_Type * const base, uint8_t data);

/*!
 * @brief Read one byte without send next clock for master.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
void I2C_ReceiveLastOneByte(I2C_Type * const base, uint8_t *data);

/*!
 * @brief Get internal hardware ready status.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_WaitForReady(I2C_Type * const base);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Set i2c module enable.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetModuleEnable(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL0, I2C_CTRL0_IICEN_Msk, I2C_CTRL0_IICEN_Pos, enable);
}

/*!
 * @brief Set i2c master/slave.
 *
 * @param[in] base: I2C base pointer
 * @param[in] mode: i2c mode
 *            - I2C_SLAVE
 *            - I2C_MASTER
 * @return None
 */
static inline void I2C_SetMSTR(I2C_Type * const base, i2c_mode_type_t mode)
{
    MODIFY_REG32(base->CTRL0, I2C_CTRL0_MSTR_Msk, I2C_CTRL0_MSTR_Pos, mode);
}

/*!
 * @brief Set i2c interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetInterrupt(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL0, I2C_CTRL0_IICIE_Msk, I2C_CTRL0_IICIE_Pos, enable);
}

/*!
 * @brief Get i2c interrupt enable status.
 *
 * @param[in] base: I2C base pointer
 * @return the i2c interrupt enable status
 */
static inline uint32_t I2C_IsInterruptEnable(I2C_Type * const base)
{
    return (base->CTRL0 & I2C_CTRL0_IICIE_Msk);
}

/*!
 * @brief Set i2c SCL sample step cnt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] sampleCnt: sample cnt
 * @param[in] stepCnt: step cnt
 *            - sample_width = (sampleCnt + 1) * APB period
 *            - half_pulse_width = (stepCnt + 1) * sample_width
 * @return None
 */
static inline void I2C_SetSampleStep(I2C_Type * const base, uint8_t sampleCnt, uint8_t stepCnt)
{
    MODIFY_REG32(base->SAMPLE_CNT, I2C_SAMPLE_CNT_DIV_Msk, I2C_SAMPLE_CNT_DIV_Pos, sampleCnt);
    MODIFY_REG32(base->STEP_CNT, I2C_STEP_CNT_DIV_Msk, I2C_STEP_CNT_DIV_Pos, stepCnt);
}

/*!
 * @brief Get i2c SCL sample cnt.
 *
 * @param[in] base: I2C base pointer
 * @return Sample cnt
 */
static inline uint8_t I2C_GetSampleCnt(const I2C_Type * const base)
{
    uint32_t tmp = base->SAMPLE_CNT;

    tmp = (tmp & I2C_SAMPLE_CNT_DIV_Msk) >> I2C_SAMPLE_CNT_DIV_Pos;

    return (uint8_t)tmp;
}

/*!
 * @brief Get i2c SCL step cnt.
 *
 * @param[in] base: I2C base pointer
 * @return Step cnt
 */
static inline uint8_t I2C_GetStepCnt(const I2C_Type * const base)
{
    uint32_t tmp = base->STEP_CNT;

    tmp = (tmp & I2C_STEP_CNT_DIV_Msk) >> I2C_STEP_CNT_DIV_Pos;

    return (uint8_t)tmp;
}

/*!
 * @brief Set NACK interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetNackInterrupt(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL2, I2C_CTRL2_NACKIE_Msk, I2C_CTRL2_NACKIE_Pos, enable);
}

/*!
 * @brief Get NACK interrupt bit.
 *
 * @param[in] base: I2C base pointer
 * @return NACK interrupt bit
 */
static inline uint32_t I2C_IsNackIntEnable(const I2C_Type * const base)
{
    return (base->CTRL2 & I2C_CTRL2_NACKIE_Msk);
}

/*!
 * @brief Set start stop interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetSSInterrupt(I2C_Type * const base, bool enable)
{
    uint32_t tmp = base->DGLCFG & (~(I2C_DGLCFG_STARTF_Msk | I2C_DGLCFG_STOPF_Msk));

    tmp &= ~I2C_DGLCFG_SSIE_Msk;
    if (enable)
    {
        tmp |= I2C_DGLCFG_SSIE_Msk;
    }
    base->DGLCFG = tmp;
}

/*!
 * @brief Get the start/stop int enable.
 *
 * @param[in] base: I2C base pointer
 * @return the start/stop int enable flag
 */
static inline uint32_t I2C_IsSSIntEnable(const I2C_Type * const base)
{
    return (base->DGLCFG & I2C_DGLCFG_SSIE_Msk);
}

/*!
 * @brief Enable/disable dma tx.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetDMATx(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL3, I2C_CTRL3_DMATXEN_Msk, I2C_CTRL3_DMATXEN_Pos, enable);
}

/*!
 * @brief Enable/disable dma rx.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetDMARx(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL3, I2C_CTRL3_DMARXEN_Msk, I2C_CTRL3_DMARXEN_Pos, enable);
}

/*!
 * @brief Set deglitch DGL cnt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] DGLCnt: deglitch cnt
 * @return None
 */
static inline void I2C_SetDGLCnt(I2C_Type * const base, uint8_t DGLCnt)
{
    uint32_t tmp = base->DGLCFG & (~(I2C_DGLCFG_STARTF_Msk | I2C_DGLCFG_STOPF_Msk));

    tmp &= ~I2C_DGLCFG_DGL_CNT_Msk;
    tmp |= (DGLCnt << I2C_DGLCFG_DGL_CNT_Pos) & I2C_DGLCFG_DGL_CNT_Msk;
    base->DGLCFG = tmp;
}

/*!
 * @brief Enable/disable deglitch DGL function.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetDGL(I2C_Type * const base, bool enable)
{
    uint32_t tmp = base->DGLCFG & (~(I2C_DGLCFG_STARTF_Msk | I2C_DGLCFG_STOPF_Msk));

    tmp &= ~I2C_DGLCFG_DGLEN_Msk;
    if (enable)
    {
        tmp |= I2C_DGLCFG_DGLEN_Msk;
    }
    base->DGLCFG = tmp;
}

/*!
 * @brief Enable/disable I2C wakeup.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetWakeup(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL0, I2C_CTRL0_WUEN_Msk, I2C_CTRL0_WUEN_Pos, enable);
}

/*!
 * @brief Enable/disable I2C master synchronization.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetSYNC(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL1, I2C_CTRL1_SYNCEN_Msk, I2C_CTRL1_SYNCEN_Pos, enable);
}

/*!
 * @brief Enable/disable I2C master arbitration.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetARB(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL1, I2C_CTRL1_ARBEN_Msk, I2C_CTRL1_ARBEN_Pos, enable);
}

/*!
 * @brief Enable/dosable I2C slave stretch.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetStretch(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL1, I2C_CTRL1_STREN_Msk, I2C_CTRL1_STREN_Pos, enable);
}

/*!
 * @brief Get stretch enable bit.
 *
 * @param[in] base: I2C base pointer
 * @return stretch enable bit
 */
static inline uint32_t I2C_IsStretch(const I2C_Type * const base)
{
    return (base->CTRL1 & I2C_CTRL1_STREN_Msk);
}

/*!
 * @brief Enable/disable slave tx buff empty interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetTxEInterrupt(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL2, I2C_CTRL2_TXEIE_Msk, I2C_CTRL2_TXEIE_Pos, enable);
}

/*!
 * @brief Enable/disable slave rx buff full interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetRxFInterrupt(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL2, I2C_CTRL2_RXFIE_Msk, I2C_CTRL2_RXFIE_Pos, enable);
}

/*!
 * @brief Enable/disable slave tx under flow interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetTxUFInterrupt(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL2, I2C_CTRL2_TXUFIE_Msk, I2C_CTRL2_TXUFIE_Pos, enable);
}

/*!
 * @brief Enable/disable slave rx over flow interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetRxOFInterrupt(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL2, I2C_CTRL2_RXOFIE_Msk, I2C_CTRL2_RXOFIE_Pos, enable);
}

/*!
 * @brief Get slave tx under flow bit.
 *
 * @param[in] base: I2C base pointer
 * @return tx under flow bit
 */
static inline uint32_t I2C_IsTxUF(const I2C_Type * const base)
{
    return (base->STATUS1 & I2C_STATUS1_TXUF_Msk);
}

/*!
 * @brief Get slave rx over flow bit.
 *
 * @param[in] base: I2C base pointer
 * @return rx over flow bit
 */
static inline uint32_t I2C_IsRxOF(const I2C_Type * const base)
{
    return (base->STATUS1 & I2C_STATUS1_RXOF_Msk);
}

/*!
 * @brief Set slave address.
 *
 * @param[in] base: I2C base pointer
 * @param[in] slaveAddr: slave 7~10bit address value
 * @return None
 */
static inline void I2C_SetSlaveAddr(I2C_Type * const base, uint16_t slaveAddr)
{
    MODIFY_REG32(base->ADDR0, I2C_ADDR0_AD_Msk, I2C_ADDR0_AD_Pos, slaveAddr & 0x7FU);
    MODIFY_REG32(base->ADDR1, I2C_ADDR1_AD_Msk, I2C_ADDR1_AD_Pos, slaveAddr >> 7U);
}

/*!
 * @brief Set slave range address.
 *
 * @param[in] base: I2C base pointer
 * @param[in] rangeAddr: slave range address value
 * @return None
 */
static inline void I2C_SetSlaveRangeAddr(I2C_Type * const base, uint8_t rangeAddr)
{
    MODIFY_REG32(base->ADDR1, I2C_ADDR1_RAD_Msk, I2C_ADDR1_RAD_Pos, rangeAddr);
}

/*!
 * @brief Enable/disable slave address extention.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetADEXT(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL1, I2C_CTRL1_ADEXT_Msk, I2C_CTRL1_ADEXT_Pos, enable);
}

/*!
 * @brief Enable/disable slave range address.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetRAD(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->ADDR1, I2C_ADDR1_RMEN_Msk, I2C_ADDR1_RMEN_Pos, enable);
}

/*!
 * @brief Enable/disable slave monitor function.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetMNT(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL2, I2C_CTRL2_MNTEN_Msk, I2C_CTRL2_MNTEN_Pos, enable);
}

/*!
 * @brief Enable/disable slave general call.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetGCA(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL1, I2C_CTRL1_GCAEN_Msk, I2C_CTRL1_GCAEN_Pos, enable);
}

/*!
 * @brief Set transfer tx direction.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_TxEn(I2C_Type * const base)
{
    base->CTRL0 |= I2C_CTRL0_TX_Msk;
}

/*!
 * @brief Set transfer rx direction.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_RxEn(I2C_Type * const base)
{
    base->CTRL0 &= ~I2C_CTRL0_TX_Msk;
}

/*!
 * @brief Set transfer ack.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_SendAck(I2C_Type * const base)
{
    base->CTRL0 &= ~I2C_CTRL0_TACK_Msk;
}

/*!
 * @brief Set transfer nack.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_SendNack(I2C_Type * const base)
{
    base->CTRL0 |= I2C_CTRL0_TACK_Msk;
}

/*!
 * @brief Write data register.
 *
 * @param[in] base: I2C base pointer
 * @param[in] data: the data to write
 * @return None
 */
static inline void I2C_WriteDataReg(I2C_Type * const base, uint8_t data)
{
    base->DATA = data;
}

/*!
 * @brief Read data register.
 *
 * @param[in] base: I2C base pointer
 * @return the data read out
 */
static inline uint8_t I2C_ReadDataReg(I2C_Type * const base)
{
    return (base->DATA);
}

/*!
 * @brief Dump read data register.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_DumpReadDataReg(I2C_Type * const base)
{
    uint32_t read = (base->DATA);

    (void)read;
}

/*!
 * @brief Enable transfer start.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_SendStart(I2C_Type * const base)
{
    base->STARTSTOP |= I2C_STARTSTOP_START_Msk;
}

/*!
 * @brief Enable transfer stop.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_SendStop(I2C_Type * const base)
{
    base->STARTSTOP |= I2C_STARTSTOP_STOP_Msk;
}

/*!
 * @brief Clear the start flag bit.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_ClearStartFlag(I2C_Type * const base)
{
    uint32_t tmp = base->DGLCFG & (~I2C_DGLCFG_STOPF_Msk);

    tmp |= I2C_DGLCFG_STARTF_Msk;
    base->DGLCFG = tmp;
}


/*!
 * @brief Clear the stop flag bit.
 *
 * @param[in] base: I2C base pointer
 * @return None
 */
static inline void I2C_ClearStopFlag(I2C_Type * const base)
{
    uint32_t tmp = base->DGLCFG & (~I2C_DGLCFG_STARTF_Msk);

    tmp |= I2C_DGLCFG_STOPF_Msk;
    base->DGLCFG = tmp;
}

/*!
 * @brief Get the slave extend address status.
 *
 * @param[in] base: I2C base pointer
 * @return slave extend address enabling bit
 */
static inline uint32_t I2C_IsADEXT(I2C_Type * const base)
{
    return (base->CTRL1 & I2C_CTRL1_ADEXT_Msk);
}

/*!
 * @brief Get the transfer direction.
 *
 * @param[in] base: I2C base pointer
 * @return transfer direction flag
 */
static inline uint32_t I2C_IsTx(I2C_Type * const base)
{
    return (base->CTRL0 & I2C_CTRL0_TX_Msk);
}

/*!
 * @brief Get the BND status.
 *
 * @param[in] base: I2C base pointer
 * @return BND flag
 */
static inline uint32_t I2C_IsBND(I2C_Type * const base)
{
    return (base->STATUS0 & I2C_STATUS0_BND_Msk);
}

/*!
 * @brief Get the NACK status.
 *
 * @param[in] base: I2C base pointer
 * @return NACK flag
 */
static inline uint32_t I2C_IsNack(I2C_Type * const base)
{
    return (base->STATUS0 & I2C_STATUS0_RACK_Msk);
}


/*!
 * @brief Get the BUS busy status.
 *
 * @param[in] base: I2C base pointer
 * @return busy flag
 */
static inline uint32_t I2C_IsBusy(I2C_Type * const base)
{
    return (base->STATUS0 & I2C_STATUS0_BUSY_Msk);
}

/*!
 * @brief Get the core ready status.
 *
 * @param[in] base: I2C base pointer
 * @return ready flag
 */
static inline uint32_t I2C_IsReady(I2C_Type * const base)
{
    return (base->STATUS0 & I2C_STATUS0_READY_Msk);
}

/*!
 * @brief Get the device role.
 *
 * @param[in] base: I2C base pointer
 * @return wheather is master or not
 */
static inline uint32_t I2C_IsMaster(I2C_Type * const base)
{
    return (base->CTRL0 & I2C_CTRL0_MSTR_Msk);
}

/*!
 * @brief Get the start flag.
 *
 * @param[in] base: I2C base pointer
 * @return the start flag
 */
static inline uint32_t I2C_IsStart(I2C_Type * const base)
{
    return (base->DGLCFG & I2C_DGLCFG_STARTF_Msk);
}

/*!
 * @brief Get the stop flag.
 *
 * @param[in] base: I2C base pointer
 * @return the stop flag
 */
static inline uint32_t I2C_IsStop(I2C_Type * const base)
{
    return (base->DGLCFG & I2C_DGLCFG_STOPF_Msk);
}

/*!
 * @brief Get status0 register.
 *
 * @param[in] base: I2C base pointer
 * @return status0 register
 */
static inline uint32_t I2C_GetStatus0(I2C_Type * const base)
{
    return (base->STATUS0);
}

/*!
 * @brief Get status1 register.
 *
 * @param[in] base: I2C base pointer
 * @return status1 register
 */
static inline uint32_t I2C_GetStatus1(I2C_Type * const base)
{
    return (base->STATUS1);
}

/*!
 * @brief Clear the status1 mask bit.
 *
 * @param[in] base: I2C base pointer
 * @param[in] clearMsk: the mask bit to clear
 * @return None
 */
static inline void I2C_ClearStatus0(I2C_Type * const base, uint32_t clearMsk)
{
    base->STATUS0 = clearMsk;
}

/*!
 * @brief Clear the status2 mask bit.
 *
 * @param[in] base: I2C base pointer
 * @param[in] clearMsk: the mask bit to clear
 * @return None
 */
static inline void I2C_ClearStatus1(I2C_Type * const base, uint32_t clearMsk)
{
    base->STATUS1 = clearMsk;
}

/*!
 * @brief Get whether the Tx is DMA or not.
 *
 * @param[in] base: I2C base pointer
 * @return DMATX status
 */
static inline uint32_t I2C_IsDMATxEnable(I2C_Type * const base)
{
    return (base->CTRL3 & I2C_CTRL3_DMATXEN_Msk);
}

/*!
 * @brief Get whether the Rx is DMA or not.
 *
 * @param[in] base: I2C base pointer
 * @return DMARX status
 */
static inline uint32_t I2C_IsDMARxEnable(I2C_Type * const base)
{
    return (base->CTRL3 & I2C_CTRL3_DMARXEN_Msk);
}

/*!
 * @brief Enable I2C debug mode or not.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetDebug(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL0, I2C_CTRL0_DBGEN_Msk, I2C_CTRL0_DBGEN_Pos, enable);
}

/*!
 * @brief Enable I2C software reset of master and slave or not.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetSoftwareReset(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL0, I2C_CTRL0_SRST_Msk, I2C_CTRL0_SRST_Pos, enable);
}

/*!
 * @brief Enable SMBus Alert or not.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetSMBusAlert(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL1, I2C_CTRL1_SAEN_Msk, I2C_CTRL1_SAEN_Pos, enable);
}

/*!
 * @brief Set BND_DMA interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetBNDDMAInterrupt(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL2, I2C_CTRL2_BND_DMA_IE_Msk, I2C_CTRL2_BND_DMA_IE_Pos, enable);
}

/*!
 * @brief Get BND_DMA interrupt status.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return BND_DMA interrupt enable status
 */
static inline uint32_t I2C_IsBNDDMAInterrupt(I2C_Type * const base, bool enable)
{
    return (base->CTRL2 & I2C_CTRL2_BND_DMA_IE_Msk);
}

/*!
 * @brief Set Pin Low Timeout interrupt.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return None
 */
static inline void I2C_SetPLTInterrupt(I2C_Type * const base, bool enable)
{
    MODIFY_REG32(base->CTRL2, I2C_CTRL2_PLTIE_Msk, I2C_CTRL2_PLTIE_Pos, enable);
}

/*!
 * @brief Get Pin Low Timeout interrupt status.
 *
 * @param[in] base: I2C base pointer
 * @param[in] enable: enabling state
 *            - true to enable
 *            - false to disable
 * @return PLT interrupt enable status
 */
static inline uint32_t I2C_IsPLTInterrupt(I2C_Type * const base, bool enable)
{
    return (base->CTRL2 & I2C_CTRL2_PLTIE_Msk);
}

/*!
 * @brief Set Pin Low Timeout.
 *
 * @param[in] base: I2C base pointer
 * @param[in] pinLowValue: pin low timwout value
 * @return None
 */
static inline void I2C_SetPinLowTimeout(I2C_Type * const base, uint32_t pinLowValue)
{
    MODIFY_REG32(base->CTRL3, I2C_CTRL3_PINLOW_Msk, I2C_CTRL3_PINLOW_Pos, pinLowValue);
}

/*!
 * @brief Configuration Pin Low Timeout.
 *
 * @param[in] base i2c module
 * @param[in] config: config timeout type
              - true to SDA and SCL low timeout detect
              - false to SCL low timeout detect
 * @return None
 */
static inline void I2C_ConfigTimeout(I2C_Type * const base, bool config)
{
    MODIFY_REG32(base->CTRL3, I2C_CTRL3_TIMECFG_Msk, I2C_CTRL3_TIMECFG_Pos, config);
}

/*!
 * @brief Get SMBus Alert Response flag.
 *
 * @param[in] base: I2C base pointer
 * @return the SMBus Alert Response flag
 */
static inline uint32_t I2C_IsSMBusAlertResponse(I2C_Type * const base)
{
    return (base->STATUS1 & I2C_STATUS1_SARF_Msk);
}

/*!
 * @brief Get Pin Low Timeout flag.
 *
 * @param[in] base: I2C base pointer
 * @return the Pin Low Timeout flag
 */
static inline uint32_t I2C_IsPinLowTimeout(I2C_Type * const base)
{
    return (base->STATUS1 & I2C_STATUS1_PLTF_Msk);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _I2C_HW_H */

/* =============================================  EOF  ============================================== */
