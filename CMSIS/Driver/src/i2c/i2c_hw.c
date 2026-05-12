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
 * @file i2c_hw.c
 *
 * @brief This file provides i2c integration functions.
 *
 */

/* ===========================================  Include  ============================================ */
#include "i2c_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Send start signal for master.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_Start(I2C_Type * const base)
{
    uint32_t timeout = 0U;
    uint32_t status = I2C_HW_STATUS_ERROR_NULL;

    I2C_TxEn(base);
    I2C_SendStart(base);

    /* wait start status */
    while ((!I2C_IsStart(base)) && (I2C_HW_DEADLINE_TIMEOUT > timeout++));
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_START_TO_STATUSFLAG_TIMEOUT;
    }

    /* wait busy status */
    timeout = 0U;
    while ((!I2C_IsBusy(base)) && (I2C_HW_DEADLINE_TIMEOUT > timeout++));
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_START_TO_BUSY_TIMEOUT;
    }
    I2C_ClearStartFlag(base);

    /* wait core ready */
    timeout = 0U;
    while ((!I2C_IsReady(base)) && (I2C_HW_DEADLINE_TIMEOUT > timeout++));
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_CORE_READY_TIMROUT;
    }

    return status;
}

/*!
 * @brief Send re-start signal for master.
 *        Make sure to disable start/stop interrupt when use the interface
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_Restart(I2C_Type * const base)
{
    uint32_t timeout = 0U;
    uint32_t status = I2C_HW_STATUS_ERROR_NULL;

    I2C_ClearStartFlag(base);
    I2C_SendStart(base);

    /* wait start status */
    while ((!I2C_IsStart(base)) && (I2C_HW_DEADLINE_TIMEOUT > timeout++));
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_START_TO_STATUSFLAG_TIMEOUT;
    }
    I2C_ClearStartFlag(base);

    /* wait core ready */
    timeout = 0U;
    while ((!I2C_IsReady(base)) && (I2C_HW_DEADLINE_TIMEOUT > timeout++));
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_CORE_READY_TIMROUT;
    }

    return status;
}

/*!
 * @brief Send stop signal for master.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_Stop(I2C_Type * const base)
{
    uint32_t timeout = 0U;
    uint32_t status = I2C_HW_STATUS_ERROR_NULL;

    I2C_TxEn(base);
    I2C_SendStop(base);

    /* wait idle status */
    while (I2C_IsBusy(base) && (I2C_HW_DEADLINE_TIMEOUT > timeout++));
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_STOP_TO_IDLE_TIMEOUT;
    }
    I2C_ClearStopFlag(base);

    /* wait core ready */
    timeout = 0U;
    while ((!I2C_IsReady(base)) && (I2C_HW_DEADLINE_TIMEOUT > timeout++));
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_CORE_READY_TIMROUT;
    }

    return status;
}

/*!
 * @brief Wait one byte transmit finished.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_WaitOneByteFinished(I2C_Type * const base)
{
    uint32_t timeout = 0U, tmpReg = 0U;
    uint32_t status = I2C_HW_STATUS_ERROR_NULL;

    tmpReg = I2C_GetStatus0(base);
    while ((!(tmpReg & I2C_STATUS0_BND_Msk)) && (I2C_HW_DEADLINE_TIMEOUT > timeout++))
    {
        tmpReg = I2C_GetStatus0(base);
    }
    if (tmpReg & I2C_STATUS0_RACK_Msk)
    {
        I2C_ClearStatus0(base, I2C_STATUS0_RACK_Msk);
        status |= I2C_HW_STATUS_NACK;
    }
    if (tmpReg & I2C_STATUS0_BND_Msk)
    {
        I2C_ClearStatus0(base, I2C_STATUS0_BND_Msk);
    }
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_BND_TIMEOUT;
    }

    return status;
}

/*!
 * @brief Write one byte with polling BND flag,
 *        make sure to disable interrupt when use the interface.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_TransmitOneByte(I2C_Type * const base, uint8_t data)
{
    uint32_t status = I2C_HW_STATUS_ERROR_NULL;

    I2C_TxEn(base);
    I2C_WriteDataReg(base, data);
    status = I2C_WaitOneByteFinished(base);

    return status;
}

/*!
 * @brief Read one byte without send next clock for master.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
void I2C_ReceiveLastOneByte(I2C_Type * const base, uint8_t *data)
{
    /* change direction to avoid send out next byte clock */
    I2C_TxEn(base);

    *data = I2C_ReadDataReg(base);

    /* when direction is Tx, read data won't clear BND flag instead of write clear */
    I2C_ClearStatus0(base, I2C_STATUS0_BND_Msk);
}

/*!
 * @brief Get internal hardware ready status.
 *
 * @param[in] base: I2C base pointer
 * @return i2c hardware status
 */
uint32_t I2C_WaitForReady(I2C_Type * const base)
{
    uint32_t timeout = 0U;
    uint32_t status = I2C_HW_STATUS_ERROR_NULL;

    while ((!(I2C_GetStatus0(base) & I2C_STATUS0_READY_Msk)) && (I2C_HW_DEADLINE_TIMEOUT > timeout++));
    if (I2C_HW_DEADLINE_TIMEOUT <= timeout)
    {
        status |= I2C_HW_STATUS_CORE_READY_TIMROUT;
    }

    return status;
}

/* =============================================  EOF  ============================================== */
