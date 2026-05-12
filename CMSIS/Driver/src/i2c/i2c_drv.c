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
 * @file i2c_drv.c
 *
 * @brief This file provides i2c integration functions.
 *
 */

/* ===========================================  Include  ============================================ */
#include "i2c_hw.h"
#include "ckgen_drv.h"
#include "dma_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of base addresses for I2C instances */
static I2C_Type * const s_i2cBase[I2C_INSTANCE_MAX] = I2C_BASE_PTRS;

/* Pointer to runtime state structure */
static i2c_master_state_t* s_i2cMasterStatePtr[I2C_INSTANCE_MAX] = {NULL};
static i2c_slave_state_t* s_i2cSlaveStatePtr[I2C_INSTANCE_MAX] = {NULL};

/* Table to save I2C IRQ numbers. */
static const IRQn_Type s_i2cIrqId[I2C_INSTANCE_MAX] = I2C_IRQS;

/* Table to save I2C clock names as defined in clock manager. */
static const ckgen_clock_t s_i2cClkNames[I2C_INSTANCE_MAX] = I2C_CKGEN_CLOCKS;

/* Table to save I2C reset names as defined in clock manager. */
static const ckgen_softreset_t s_i2cResetNames[I2C_INSTANCE_MAX] = I2C_SOFT_RESETS;

/* Table to save PCC clock source, for getting the input clcok frequency. */
static const clock_names_t s_i2cClock[I2C_INSTANCE_MAX] = I2C_CLOCK_NAMES;

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get I2C module base address.
 *
 * @param[in] instance: I2C module instance
 * @return I2C module base address
 */
I2C_Type *I2C_DRV_GetBase(uint8_t instance)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    return (s_i2cBase[instance]);
}

/*!
 * @brief Ends current transmission or reception.
 *
 * @param[in] base: I2C base pointer
 * @param[in] master: I2C master driver pointer
 * @param[in] sendStop: If true generate stop condition after the transmission, otherwise not
 * @return None
 */
static void I2C_MasterEndTransfer(I2C_Type *base, i2c_master_state_t *master, bool sendStop)
{
    DEVICE_ASSERT(master != NULL);
    DEVICE_ASSERT(base != NULL);

    /* Disable all interrupt events */
    I2C_SetInterrupt(base, false);
    if (I2C_USING_DMA == master->transferType)
    {
        /* Stop DMA Channel */
        (void)DMA_DRV_StopChannel(master->dmaChannel);
        /* Disable I2C DMA request */
        if (master->rxSize != (uint16_t)0U)
        {
            (void)I2C_SetDMARx(base, false);
        }
        else
        {
            (void)I2C_SetDMATx(base, false);
        }
    }

    /* Send stop single */
    if (sendStop)
    {
        I2C_SendStop(base);
    }

    master->txBuff = NULL;
    master->txSize = 0U;
    master->rxPointer = 0U;
    master->rxBuff = NULL;
    master->rxSize = 0U;
    master->i2cIdle = true;
}

/*!
 * @brief Ends current transmission or reception.
 *
 * @param[in] base: I2C base pointer
 * @param[in] slave: I2C slave driver pointer
 * @return None
 */
static void I2C_SlaveEndTransfer(I2C_Type *base, i2c_slave_state_t *slave)
{
    DEVICE_ASSERT(slave != NULL);
    DEVICE_ASSERT(base != NULL);

    /* Disable all interrupt events */
    I2C_SetInterrupt(base, false);
    if (I2C_USING_DMA == slave->transferType)
    {
        /* Stop DMA Channel */
        (void)DMA_DRV_StopChannel(slave->dmaChannel);
        /* Disable I2C DMA request */
        if (slave->rxSize != (uint16_t)0U)
        {
            (void)I2C_SetDMARx(base, false);
        }
        else
        {
            (void)I2C_SetDMATx(base, false);
        }
    }

    slave->isTransferInProgress = false;
    slave->txBuff = NULL;
    slave->txSize = 0U;
    slave->rxBuff = NULL;
    slave->rxSize = 0U;
    slave->rxPointer = 0U;
}

/*!
 * @brief Handle a transmit request for master.
 *
 * @param[in] base: I2C base pointer
 * @param[in] master: I2C master driver pointer
 * @return None
 */
static void I2C_MasterHandleTransmitDataRequest(I2C_Type *base, i2c_master_state_t *master)
{
    if (master->txBuff != NULL)
    {
        /* A transmission is in progress */
        if (0U == master->txSize)
        {
            /* There is no more data in buffer, the transmission is over */
            I2C_MasterEndTransfer(base, master, master->sendStop);
            /* Clear I2C BND flag */
            I2C_ClearStatus0(base, I2C_STATUS0_BND_Msk);
            /* Signal transfer end for blocking transfers */
            if (master->blocking)
            {
                (void)OSIF_SemaPost(&(master->idleSemaphore));
            }
            master->status = STATUS_SUCCESS;

            if (master->callback != NULL)
            {
                master->callback((uint32_t)I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }
        }
        else
        {
            /* Write data to send */
            I2C_WriteDataReg(base, master->txBuff[0U]);
            master->txBuff++;
            master->txSize--;
        }
    }
}

/*!
 * @brief Handle a receive request for master.
 *
 * @param[in] base: I2C base pointer
 * @param[in] master: I2C master driver pointer
 * @return None
 */
static void I2C_MasterHandleReceiveDataReadyEvent(I2C_Type *base, i2c_master_state_t *master)
{
    if (I2C_IsTx(base))
    {
        if (0U == master->rxSize)
        {
            /* There is no more data in buffer, the transmission is over */
            I2C_MasterEndTransfer(base, master, master->sendStop);
            /* Clear I2C BND flag */
            I2C_ClearStatus0(base, I2C_STATUS0_BND_Msk);
            /* Signal transfer end for blocking transfers */
            if (master->blocking)
            {
                (void)OSIF_SemaPost(&(master->idleSemaphore));
            }
            master->status = STATUS_SUCCESS;

            if (master->callback != NULL)
            {
                master->callback((uint32_t)I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }
        }
        else
        {
            I2C_RxEn(base);
            if (1U == master->rxSize)
            {
                I2C_SendNack(base);
            }
            else
            {
                I2C_SendAck(base);
            }
            I2C_DumpReadDataReg(base);
        }
    }
    else
    {
        if (1U == master->rxSize)
        {
            I2C_SendAck(base);
            I2C_ReceiveLastOneByte(base, &master->rxBuff[0U]);
            master->rxPointer++;
            /* There is no more data in buffer, the transmission is over */
            I2C_MasterEndTransfer(base, master, master->sendStop);
            /* Signal transfer end for blocking transfers */
            if (master->blocking)
            {
                (void)OSIF_SemaPost(&(master->idleSemaphore));
            }
            master->status = STATUS_SUCCESS;

            if (master->callback != NULL)
            {
                master->callback((uint32_t)I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }
        }
        else if ((master->rxSize - 2U) > master->rxPointer)
        {
            I2C_SendAck(base);
            master->rxBuff[0U] = I2C_ReadDataReg(base);
            master->rxBuff++;
            master->rxPointer++;
        }
        else if ((master->rxSize - 2U) == master->rxPointer)
        {
            I2C_SendNack(base);
            master->rxBuff[0U] = I2C_ReadDataReg(base);
            master->rxBuff++;
            master->rxPointer++;
        }
        else
        {
            I2C_SendAck(base);
            I2C_ReceiveLastOneByte(base, &master->rxBuff[0U]);
            master->rxPointer++;
            /* There is no more data in buffer, the transmission is over */
            I2C_MasterEndTransfer(base, master, master->sendStop);
            /* Signal transfer end for blocking transfers */
            if (master->blocking)
            {
                (void)OSIF_SemaPost(&(master->idleSemaphore));
            }
            master->status = STATUS_SUCCESS;

            if (master->callback != NULL)
            {
                master->callback((uint32_t)I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }
        }
    }
}

/*!
 * @brief I2C DMA transfer callback function for master.
 *
 * @param[in] parameter: Function parameter
 * @param[in] status: I2C tansfer DMA channel status
 * @return None
 */
static void I2C_MasterDmaCallback(void *parameter, dma_chn_status_t status)
{
    uint8_t instance = (uint32_t)parameter;
    I2C_Type *base = s_i2cBase[instance];
    i2c_master_state_t *master = s_i2cMasterStatePtr[instance];

    /* Set status error if an error occurred in DMA channel */
    if (DMA_CHN_ERROR == status)
    {
        master->status = STATUS_ERROR;
    }
    else
    {
        master->status = STATUS_SUCCESS;
    }

    if (I2C_WRITE == master->dirType)
    {
        I2C_SetDMATx(base, false);
    }
    else
    {
        I2C_MasterEndTransfer(base, master, master->sendStop);

        if (master->blocking)
        {
            (void)OSIF_SemaPost(&(master->idleSemaphore));
        }

        if (master->callback != NULL)
        {
            master->callback((uint32_t)I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
        }
    }
}

/*!
 * @brief Start I2C transmission and send slave read address.
 *
 * @param[in] base: I2C base pointer
 * @param[in] master: I2C master driver pointer
 * @return The result of execution
 */
static uint32_t I2C_MasterSendReadAddress(I2C_Type *base, i2c_master_state_t *master)
{
    uint32_t hwStatus = I2C_HW_STATUS_ERROR_NULL;

    hwStatus |= I2C_WaitForReady(base);
    hwStatus |= I2C_Start(base);

    if (I2C_HW_STATUS_ERROR_NULL == hwStatus)
    {
        if (master->is10bitAddr)
        {
            I2C_SetInterrupt(base, false);
            hwStatus |= I2C_TransmitOneByte(base, ((I2C_ADDEXT_PRIMARY_BYTE_FIX + (master->slaveAddress >> 8U)) << 1U) \
                                                    | (uint8_t)I2C_WRITE);
            hwStatus |= I2C_TransmitOneByte(base, (uint8_t)master->slaveAddress);
            hwStatus |= I2C_Restart(base);
            I2C_SetInterrupt(base, true);
            if (I2C_HW_STATUS_ERROR_NULL == hwStatus)
            {
                I2C_TxEn(base);
                I2C_WriteDataReg(base, ((I2C_ADDEXT_PRIMARY_BYTE_FIX + (master->slaveAddress >> 8U)) << 1U) \
                                        | (uint8_t)I2C_READ);
            }
        }
        else
        {
            I2C_TxEn(base);
            I2C_WriteDataReg(base, (uint8_t)((master->slaveAddress << 1U) | (uint8_t)I2C_READ));
        }
    }
    else
    {
        /* Do nothing */
    }

    return hwStatus;
}

/*!
 * @brief Start I2C transmission and send slave write address.
 *
 * @param[in] base: I2C base pointer
 * @param[in] master: I2C master driver pointer
 * @return The result of execution
 */
static uint32_t I2C_MasterSendWriteAddress(I2C_Type *base, i2c_master_state_t *master)
{
    uint32_t hwStatus = I2C_HW_STATUS_ERROR_NULL;

    hwStatus |= I2C_WaitForReady(base);
    hwStatus |= I2C_Start(base);

    if (I2C_HW_STATUS_ERROR_NULL == hwStatus)
    {
        if (master->is10bitAddr)
        {
            I2C_SetInterrupt(base, false);
            if (I2C_USING_DMA == master->transferType)
            {
                I2C_SetDMATx(base, false);
            }
            hwStatus |= I2C_TransmitOneByte(base, ((I2C_ADDEXT_PRIMARY_BYTE_FIX + (master->slaveAddress >> 8U)) << 1U) \
                                                    | (uint8_t)I2C_WRITE);
            I2C_SetInterrupt(base, true);
            if (I2C_USING_DMA == master->transferType)
            {
                I2C_SetDMATx(base, true);
            }
            if (I2C_HW_STATUS_ERROR_NULL == hwStatus)
            {
                I2C_TxEn(base);
                I2C_WriteDataReg(base, (uint8_t)master->slaveAddress);
            }
        }
        else
        {
            I2C_TxEn(base);
            I2C_WriteDataReg(base, (((uint8_t)master->slaveAddress << 1U) | (uint8_t)I2C_WRITE));
        }
    }
    else
    {
        /* Do nothing */
    }

    return hwStatus;
}

/*!
 * @brief Start DMA receive for master.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] master: I2C master driver pointer
 * @return None
 */
static void I2C_MasterReceiveDma(uint8_t instance)
{
    I2C_Type *base = s_i2cBase[instance];
    i2c_master_state_t *master = s_i2cMasterStatePtr[instance];
    uint32_t status = I2C_HW_STATUS_ERROR_NULL;

    /* Configure the transfer control descriptor */
    (void)DMA_DRV_ConfigTransfer(master->dmaChannel, DMA_TRANSFER_PERIPH2MEM, \
                                  (uint32_t)(&(base->DATA)), ((uint32_t)master->rxBuff), \
                                 DMA_TRANSFER_SIZE_1B, master->rxSize);

    /* Call driver function to end the reception when the DMA transfer is done */
    (void)DMA_DRV_InstallCallback(master->dmaChannel, (dma_callback_t)(I2C_MasterDmaCallback), (void*)(instance));

    /* Start the DMA channel */
    (void)DMA_DRV_StartChannel(master->dmaChannel);
    I2C_SetInterrupt(base, true);
    I2C_ClearStatus0(base, I2C_STATUS0_RACK_Msk);
    /* Enable NACK interrupt */
    I2C_SetNackInterrupt(base, true);
    /* Enable arbitration interrupt */
    I2C_SetARB(base, true);
    /* Enable I2C DMA RX */
    I2C_SetDMARx(base, true);
    /* Master send slave address */
    status |= I2C_WaitForReady(base);
    status |= I2C_Start(base);
    if (master->is10bitAddr)
    {
        status |= I2C_TransmitOneByte(base, ((I2C_ADDEXT_PRIMARY_BYTE_FIX + (master->slaveAddress >> 8U)) << 1U) \
                                             | (uint8_t)I2C_WRITE);
        status |= I2C_TransmitOneByte(base, (uint8_t)master->slaveAddress);
        status |= I2C_Restart(base);
        status |= I2C_TransmitOneByte(base, ((I2C_ADDEXT_PRIMARY_BYTE_FIX + (master->slaveAddress >> 8U)) << 1U) \
                                             | (uint8_t)I2C_READ);
    }
    else
    {
                status |= I2C_TransmitOneByte(base, (uint8_t)((master->slaveAddress << 1U) | (uint8_t)I2C_READ));
    }
    if (I2C_HW_STATUS_ERROR_NULL == status)
    {
        I2C_RxEn(base);
        I2C_DumpReadDataReg(base);
    }
    else
    {
        I2C_SendStop(base);
    }
}

/*!
 * @brief Start DMA transmit for master.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] master: I2C master driver pointer
 * @return None
 */
static void I2C_MasterTransmitDma(uint8_t instance)
{
    I2C_Type *base = s_i2cBase[instance];
    i2c_master_state_t *master = s_i2cMasterStatePtr[instance];

    /* Configure the transfer control descriptor */
    (void)DMA_DRV_ConfigTransfer(master->dmaChannel, DMA_TRANSFER_MEM2PERIPH, \
                                  (uint32_t)master->txBuff, (uint32_t)(&(base->DATA)), \
                                 DMA_TRANSFER_SIZE_1B, master->txSize);

    /* Call driver function to end the reception when the DMA transfer is done */
    (void)DMA_DRV_InstallCallback(master->dmaChannel, (dma_callback_t)(I2C_MasterDmaCallback), (void *)(instance));

    /* Start the DMA channel */
    (void)DMA_DRV_StartChannel(master->dmaChannel);

    I2C_SetInterrupt(base, true);
    I2C_ClearStatus0(base, I2C_STATUS0_RACK_Msk);
    /* Enable NACK interrupt */
    I2C_SetNackInterrupt(base, true);
    /* Enable arbitration interrupt */
    I2C_SetARB(base, true);
    /* Enable I2C DMA TX */
    I2C_SetDMATx(base, true);
    /* Master send slave address */
    (void)I2C_MasterSendWriteAddress(base, master);
}

/*!
 * @brief Handle an address valid event for slave.
 *
 * @param[in] base: I2C base pointer
 * @param[in] slave: I2C slave driver pointer
 * @return None
 */
static void I2C_SlaveHandleAddressValidEvent(I2C_Type *base, i2c_slave_state_t *slave)
{
    uint32_t intStatus = 0U;

    intStatus = I2C_GetStatus0(base);
    I2C_ClearStatus0(base, I2C_STATUS0_SAMF_Msk);

    if (intStatus & I2C_STATUS0_SRW_Msk)
    {
        I2C_WriteDataReg(base, slave->txBuff[0U]);
        slave->txBuff++;
        slave->txSize--;
    }
    else
    {
        if (slave->rxSize > 0U)
        {
            I2C_SendAck(base);
        }
        else
        {
            I2C_SendNack(base);
        }
    }

    slave->status = STATUS_BUSY;
}

/*!
 * @brief Handle a transmit data event for slave.
 *
 * @param[in] base: I2C base pointer
 * @param[in] slave: I2C slave driver pointer
 * @return None
 */
static void I2C_SlaveHandleTransmitDataEvent(I2C_Type *base, i2c_slave_state_t *slave)
{
    if (0U == slave->txSize)
    {
        /* Clear I2C BND flag */
        I2C_ClearStatus0(base, I2C_STATUS0_BND_Msk);
        /* Out of data, call callback to allow user to provide a new buffer */
        if (slave->callback != NULL)
        {
            slave->callback((uint32_t)I2C_SLAVE_EVENT_TX_EMPTY, slave->callbackParam);
        }
    }
    else
    {
        /* Write data to send */
        I2C_WriteDataReg(base, slave->txBuff[0U]);
        slave->txBuff++;
        slave->txSize--;
    }
    if (I2C_IsTxUF(base))
    {
        slave->status = STATUS_I2C_TX_UNDERRUN;
        I2C_ClearStatus1(base, I2C_STATUS1_TXUF_Msk);
        /* I2C tx is underflow, record tx underflow event and send dummy char */
        I2C_WriteDataReg(base, 0xFFU);
    }
}

/*!
 * @brief Handle a receive data event for slave.
 *
 * @param[in] base: I2C base pointer
 * @param[in] slave: I2C slave driver pointer
 * @return None
 */
static void I2C_SlaveHandleReceiveDataEvent(I2C_Type *base, i2c_slave_state_t *slave)
{
    if ((I2C_IsRxOF(base)) || (0U == slave->rxSize) || (slave->rxPointer >= slave->rxSize))
    {
        slave->status = STATUS_I2C_RX_OVERRUN;
        I2C_ClearStatus1(base, I2C_STATUS1_RXOF_Msk);
        /* I2C rx is overflow, record rx overflow event and dummy read data */
        (void)I2C_ReadDataReg(base);
    }
    else
    {
        I2C_SendAck(base);
        slave->rxBuff[0U] = I2C_ReadDataReg(base);
        slave->rxBuff++;
        slave->rxPointer++;
    }
}

/*!
 * @brief Handle slave end transfer operations.
 *
 * @param[in] base: I2C base pointer
 * @param[in] slave: I2C slave driver pointer
 * @return None
 */
static void I2C_SlaveEndTransferHandler(I2C_Type *base, i2c_slave_state_t *slave)
{
    DEVICE_ASSERT(slave != NULL);

    /* Stop DMA Channel if slave is transferring data in DMA mode */
    if (I2C_USING_DMA == slave->transferType)
    {
        I2C_ClearStatus0(base, I2C_STATUS0_RACK_Msk);
        /* Stop DMA Channel */
        (void)DMA_DRV_StopChannel(slave->dmaChannel);
    }

    if (!slave->slaveListening)
    {
        I2C_SlaveEndTransfer(base, slave);
        /* Signal transfer end for blocking transfers */
        if (slave->blocking)
        {
            (void)OSIF_SemaPost(&(slave->idleSemaphore));
        }
    }

    if (slave->callback != NULL)
    {
        slave->callback((uint32_t)I2C_SLAVE_EVENT_STOP, slave->callbackParam);
    }
}

/*!
 * @brief Start DMA receive for slave.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] slave: I2C slave driver pointer
 * @return None
 */
static void I2C_SlaveReceiveDma(uint8_t instance)
{
    I2C_Type *base = s_i2cBase[instance];
    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];

    /* Enable I2C DMA RX */
    I2C_SetDMARx(base, true);

    /* Configure the transfer control descriptor */
    (void)DMA_DRV_ConfigTransfer(slave->dmaChannel, DMA_TRANSFER_PERIPH2MEM, \
                                  (uint32_t)(&(base->DATA)), ((uint32_t)slave->rxBuff), \
                                 DMA_TRANSFER_SIZE_1B, slave->rxSize);

    /* Start the DMA channel */
    (void)DMA_DRV_StartChannel(slave->dmaChannel);
}

/*!
 * @brief Start DMA transmit for slave.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] slave: I2C slave driver pointer
 * @return None
 */
static void I2C_SlaveTransmitDma(uint8_t instance)
{
    I2C_Type *base = s_i2cBase[instance];
    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];

    I2C_ClearStatus0(base, I2C_STATUS0_RACK_Msk);
    /* Enable NACK interrupt */
    I2C_SetNackInterrupt(base, true);
    /* Enable I2C DMA TX */
    I2C_SetDMATx(base, true);

    /* Configure the transfer control descriptor */
    (void)DMA_DRV_ConfigTransfer(slave->dmaChannel, DMA_TRANSFER_MEM2PERIPH, \
                                  ((uint32_t)slave->txBuff), (uint32_t)(&(base->DATA)), \
                                 DMA_TRANSFER_SIZE_1B, slave->txSize);

    /* Start the DMA channel */
    (void)DMA_DRV_StartChannel(slave->dmaChannel);
}

/*!
 * @brief Start DMA for slave.
 *
 * @param[in] instance: The I2C instance number
 * @return None
 */
static void I2C_SlaveStartDmaTransfer(uint8_t instance)
{
    const i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];

    if (slave->txSize > (uint32_t)0U)
    {
        I2C_SlaveTransmitDma(instance);
    }
    else
    {
        I2C_SlaveReceiveDma(instance);
    }
}

/*!
 * @brief Waits for the end of a blocking transfer.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] timeout: Timeout for the transfer
 * @return The status of master
 */
static status_t I2C_MasterWaitTransferEnd(uint8_t instance, uint32_t timeout)
{
    I2C_Type *base = s_i2cBase[instance];
    status_t osifError = STATUS_SUCCESS;
    i2c_master_state_t *master;

    master = s_i2cMasterStatePtr[instance];

    /* Wait for transfer to be completed by the IRQ */
    osifError = OSIF_SemaWait(&(master->idleSemaphore), timeout);

    if (STATUS_TIMEOUT == osifError)
    {
        /* If master is sending data transfer is aborted now in case timeout occurred */
        if (master->txSize != 0U)
        {
            I2C_MasterEndTransfer(base, master, false);
            master->status = STATUS_TIMEOUT;
        }
        else
        {
            if (I2C_USING_DMA == master->transferType)
            {
                /* Stop DMA channel and activate interrupts */
                (void)DMA_DRV_StopChannel(master->dmaChannel);
            }

            if (master->rxSize != master->rxPointer)
            {
                I2C_MasterEndTransfer(base, master, false);
                master->status = STATUS_TIMEOUT;
            }
        }
    }
    /* Blocking trandfer is over */
    master->blocking = false;

    return master->status;
}

/*!
 * @brief Waits for the end of a blocking transfer.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] timeout: Timeout for the transfer
 * @return The status of slave
 */
static status_t I2C_SlaveWaitTransferEnd(uint8_t instance, uint32_t timeout)
{
    I2C_Type *base = s_i2cBase[instance];
    status_t osifError = STATUS_SUCCESS;
    i2c_slave_state_t *slave;

    slave = s_i2cSlaveStatePtr[instance];
    /* Wait for transfer to be completed by the IRQ */
    osifError = OSIF_SemaWait(&(slave->idleSemaphore), timeout);

    if (STATUS_TIMEOUT == osifError)
    {
        I2C_SlaveEndTransfer(base, slave);
        slave->status = STATUS_TIMEOUT;
    }
    /* Blocking transfer is over */
    slave->blocking = false;

    return slave->status;
}

/*!
 * @brief Initialize the I2C master mode driver based on configuration input.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] userConfig: I2C master user configuration pointer
 * @param[in] master: I2C master driver pointer
 * @return The result of execution
 */
status_t I2C_DRV_MasterInit(uint8_t instance, const i2c_master_user_config_t *userConfig, \
                            i2c_master_state_t *master)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(userConfig != NULL);
    DEVICE_ASSERT(master != NULL);

    I2C_Type *base = s_i2cBase[instance];
    status_t retVal = STATUS_SUCCESS;
    uint32_t inputClock = 0U;

    /* Check to see if the I2C master instance is already initialized */
    DEVICE_ASSERT(s_i2cMasterStatePtr[instance] == NULL);

    /* Check the protocol clock frequency */
    retVal = CKGEN_DRV_GetFreq(s_i2cClock[instance], &inputClock);
    DEVICE_ASSERT(STATUS_SUCCESS == retVal);
    DEVICE_ASSERT(inputClock > 0U);

    s_i2cMasterStatePtr[instance] = master;

    /* Initialize driver status structure */
    master->rxBuff = NULL;
    master->rxSize = 0U;
    master->txBuff = NULL;
    master->txSize = 0U;
    master->status = STATUS_SUCCESS;
    master->i2cIdle = true;
    master->slaveAddress = userConfig->slaveAddress;
    master->is10bitAddr = userConfig->is10bitAddr;
    master->transferType = userConfig->transferType;
    /* Store DMA channel number used in transfer */
    master->dmaChannel = userConfig->dmaChannel;
    master->callback = userConfig->callback;
    master->callbackParam = userConfig->callbackParam;
    master->blocking = false;
    master->rxPointer = 0U;
    master->dirType = I2C_WRITE;

    /* Initialize the semaphore */
    retVal = OSIF_SemaCreate(&(master->idleSemaphore), 0U);
    DEVICE_ASSERT(STATUS_SUCCESS == retVal);

    /* Enable clock and reset I2C module */
    CKGEN_DRV_Enable(s_i2cClkNames[instance], true);
    CKGEN_DRV_SoftReset(s_i2cResetNames[instance], true);
    /* Enable I2C interrupt */
    NVIC_EnableIRQ(s_i2cIrqId[instance]);
    /* Set slave address */
    I2C_DRV_MasterSetSlaveAddr(instance, userConfig->slaveAddress, userConfig->is10bitAddr);
    /* Set master synchronization, arbitration */
    I2C_SetSYNC(base, userConfig->syncEn);
    I2C_SetARB(base, userConfig->arbitration);
    /* i2c master/slave */
    I2C_SetMSTR(base, I2C_MASTER);
    /* i2c module enable */
    I2C_SetModuleEnable(base, true);
    /* Set baud rate */
    (void)I2C_DRV_MasterSetBaudRate(instance, userConfig->baudRate);

    (void)retVal;

    return STATUS_SUCCESS;
}

/*!
 * @brief De-initialize the I2C master mode driver.
 *
 * @param[in] instance: The I2C instance number
 * @return The result of execution
 */
status_t I2C_DRV_MasterDeinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    const i2c_master_state_t *master;

    master = s_i2cMasterStatePtr[instance];
    DEVICE_ASSERT(master != NULL);

    /* Destroy the semaphore */
    (void)OSIF_SemaDestroy(&(master->idleSemaphore));

    s_i2cMasterStatePtr[instance] = NULL;
    /* Disable I2C interrupt */
    NVIC_DisableIRQ(s_i2cIrqId[instance]);
    NVIC_ClearPendingIRQ(s_i2cIrqId[instance]);
    /* Reset I2C module,and disable clock */
    CKGEN_DRV_SoftReset(s_i2cResetNames[instance], false);
    CKGEN_DRV_Enable(s_i2cClkNames[instance], false);

    return STATUS_SUCCESS;
}

/*!
 * @brief Get the currently configured baud rate.
 *
 * @param[in] instance: The I2C instance number
 * @param[out] baudRate: The current baud rate of I2C
 * @return None
 */
void I2C_DRV_MasterGetBaudRate(uint8_t instance, uint32_t *baudRate)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    const I2C_Type *base = s_i2cBase[instance];
    const i2c_master_state_t *master;
    status_t retVal = STATUS_SUCCESS;
    uint32_t sampleCnt = 0U;
    uint32_t stepCnt = 0U;
    uint32_t inputClock = 0U;

    master = s_i2cMasterStatePtr[instance];
    DEVICE_ASSERT(master != NULL);

    /* Get the protocol clock frequency */
    retVal = CKGEN_DRV_GetFreq(s_i2cClock[instance], &inputClock);
    DEVICE_ASSERT(STATUS_SUCCESS == retVal);
    DEVICE_ASSERT(inputClock > 0U);

    /* SCL_freq = Input_freq/((sampleCnt + 1)(stepCnt + 1) * 2) */
    sampleCnt = I2C_GetSampleCnt(base);
    stepCnt = I2C_GetStepCnt(base);

    *baudRate = inputClock / ((stepCnt + 1U) * (sampleCnt + 1U) * 2U);

    (void)retVal;
    (void)master;
}

/*!
 * @brief This is a helper function which implements absolute difference between
 *        two numbers.
 *
 * @param[in] a: The one of number
 * @param[in] b: Another one of number
 * @return Difference between two number
 */
static uint32_t I2C_AbsDif(uint32_t a, uint32_t b)
{
    if (a>b)
    {
        return (a-b);
    }
    else
    {
        return (b-a);
    }
}

/*!
 * @brief Set the baud rate for I2C communication.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] baudRate: The current setting baud rate value
 * @return The result of execution
 */
status_t I2C_DRV_MasterSetBaudRate(uint8_t instance, uint32_t baudRate)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    I2C_Type *base = s_i2cBase[instance];
    const i2c_master_state_t *master;
    status_t retVal = STATUS_SUCCESS;
    uint8_t sampleCnt = 0U;
    uint8_t stepCnt = 0U;
    uint32_t inputClock = 0U;
    uint32_t bestBaudRate = 0xFFFFFFFFU;
    uint32_t bestStep = 0U;
    uint32_t bestSample = 0U;
    uint32_t freq1 = 0U;
    uint32_t freq2 = 0U;
    uint32_t low,high;
    uint32_t tempBestBaud = 0U;
    uint32_t tempBestSample = 0U;

    master = s_i2cMasterStatePtr[instance];
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        return STATUS_BUSY;
    }

    /* Get the protocol clock frequency */
    retVal = CKGEN_DRV_GetFreq(s_i2cClock[instance], &inputClock);
    DEVICE_ASSERT(STATUS_SUCCESS == retVal);
    DEVICE_ASSERT(inputClock > 0U);

    /* Disable I2C module */
    I2C_SetModuleEnable(base, false);

    if (baudRate != 0U)
    {
        for (stepCnt = 3U; stepCnt < 255U; stepCnt++)
        {
            low = 0U;
            high = 255U;

            /* Implement golden section search algorithm */
            do
            {
                sampleCnt = (uint8_t)((low + high) / 2U);
                freq1 = inputClock / ((stepCnt + 1U)*(sampleCnt + 1U) * (uint32_t)2U);

                if (I2C_AbsDif(baudRate, bestBaudRate) > I2C_AbsDif(baudRate, freq1))
                {
                    bestBaudRate = freq1;
                }
                if (freq1 < baudRate)
                {
                    high = sampleCnt;
                }
                else
                {
                    low = sampleCnt;
                }
            } while ((high - low) > 1U);

            /* Evaluate last 2 scaler values */
            freq1 = inputClock / ((stepCnt + 1U)*(low + 1U) * (uint32_t)2U);
            freq2 = inputClock / ((stepCnt + 1U)*(high + 1U) * (uint32_t)2U);

            if (I2C_AbsDif(baudRate, freq1) > I2C_AbsDif(baudRate, freq2))
            {
                tempBestBaud = freq2;
                tempBestSample = high;
            }
            else
            {
                tempBestBaud = freq1;
                tempBestSample = low;
            }

            if (I2C_AbsDif(baudRate, bestBaudRate) >= I2C_AbsDif(baudRate, tempBestBaud))
            {
                bestBaudRate = tempBestBaud;
                bestSample = tempBestSample;
                bestStep = stepCnt;
            }

            /* If currently baudrate is equal to target baudrate stop the search */
            if (bestBaudRate == baudRate)
            {
                break;
            }
        }
    }
    else
    {
        return STATUS_ERROR;
    }

    /* Write the best stepCnt and sampleCnt to register */
    I2C_SetSampleStep(base, (uint8_t)bestSample, (uint8_t)bestStep);
    /* Re-enable I2C */
    I2C_SetModuleEnable(base, true);
    (void)master;
    (void)retVal;

    return STATUS_SUCCESS;
}

/*!
 * @brief Set the slave address for I2C communication.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] address: Slave address, 7-bit or 10-bit
 * @param[in] is10bitAddr: Enable/disable 10-bit address
 * @return None
 */
void I2C_DRV_MasterSetSlaveAddr(uint8_t instance, const uint16_t address, const bool is10bitAddr)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    i2c_master_state_t *master;

    master = s_i2cMasterStatePtr[instance];
    DEVICE_ASSERT(master != NULL);

    master->slaveAddress = address;
    master->is10bitAddr = is10bitAddr;
}

/*!
 * @brief Perform a non-blocking send transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @param[in] sendStop: If true generate stop condition after the transmission, otherwise not
 * @return The result of execution
 */
status_t I2C_DRV_MasterSendData(uint8_t instance, const uint8_t *txBuff, uint32_t txSize, bool sendStop)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    I2C_Type *base = s_i2cBase[instance];
    i2c_master_state_t *master;
    status_t status = STATUS_SUCCESS;

    master = s_i2cMasterStatePtr[instance];
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        return STATUS_BUSY;
    }

    /* Copy parameters to driver state structure */
    master->txBuff = txBuff;
    master->txSize = txSize;
    master->sendStop = sendStop;
    master->i2cIdle = false;
    master->status = STATUS_BUSY;
    master->dirType = I2C_WRITE;

    if (I2C_USING_DMA == master->transferType)
    {
        /* Enable i2c DMA transmit */
        I2C_MasterTransmitDma(instance);
    }
    else
    {
        /* Enable arbitration interrupt */
        I2C_SetARB(base, true);
        I2C_SetInterrupt(base, true);
        /* Master send slave address */
        if (I2C_MasterSendWriteAddress(base, master) != I2C_HW_STATUS_ERROR_NULL)
        {
            status = STATUS_ERROR;
        }

        return status;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a blocking send transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @param[in] sendStop: If true generate stop condition after the transmission, otherwise not
 * @param[in] timeout: Timeout for the transfer
 * @return The result of execution
 */
status_t I2C_DRV_MasterSendDataBlocking(uint8_t instance, const uint8_t *txBuff, uint32_t txSize, \
                                        bool sendStop, uint32_t timeout)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    i2c_master_state_t *master = s_i2cMasterStatePtr[instance];
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        return STATUS_BUSY;
    }

    /* Set transfer as blocking */
    master->blocking = true;

    /* Dummy wait to ensure the semaphore is 0, no need to check result */
    (void)OSIF_SemaWait(&(master->idleSemaphore), 0U);

    (void)I2C_DRV_MasterSendData(instance, txBuff, txSize, sendStop);

    /* Wait for transfer to end */
    return I2C_MasterWaitTransferEnd(instance, timeout);
}

/*!
 * @brief Abort a non-blocking I2C master transmission or reception.
 *
 * @param[in] instance: The I2C instance number
 * @return The result of execution
 */
status_t I2C_DRV_MasterAbortTransferData(uint8_t instance)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    I2C_Type *base = s_i2cBase[instance];
    i2c_master_state_t *master = s_i2cMasterStatePtr[instance];

    DEVICE_ASSERT(master != NULL);

    /* End transfer: force stop generation */
    master->status = STATUS_I2C_ABORTED;
    I2C_MasterEndTransfer(base, master, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a non-blocking receive transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @param[in] sendStop: Specifies whether or not to generate stop condition after the reception
 * @return The result of execution
 */
status_t I2C_DRV_MasterReceiveData(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize, bool sendStop)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    status_t status = STATUS_SUCCESS;
    I2C_Type *base = s_i2cBase[instance];
    i2c_master_state_t *master = s_i2cMasterStatePtr[instance];
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        return STATUS_BUSY;
    }

    /* Copy parameters to driver state structure */
    master->rxBuff = rxBuff;
    master->rxSize = rxSize;
    master->sendStop = sendStop;
    master->i2cIdle = false;
    master->status = STATUS_BUSY;
    master->dirType = I2C_READ;

    if (I2C_USING_DMA == master->transferType)
    {
        /* Enable i2c DMA receive */
        I2C_MasterReceiveDma(instance);
    }
    else
    {
        /* Enable arbitration interrupt */
        I2C_SetARB(base, true);
        I2C_SetInterrupt(base, true);
        /* Master send slave address */
        if (I2C_MasterSendReadAddress(base, master) != I2C_HW_STATUS_ERROR_NULL)
        {
            status = STATUS_ERROR;
        }

        return status;
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a blocking receive transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @param[in] sendStop: Specifies whether or not to generate stop condition after the reception
 * @param[in] timeout: Timeout for the transfer
 * @return The result of execution
 */
status_t I2C_DRV_MasterReceiveDataBlocking(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize, \
                                           bool sendStop, uint32_t timeout)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    i2c_master_state_t *master = s_i2cMasterStatePtr[instance];
    DEVICE_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        return STATUS_BUSY;
    }

    /* Set transfer as blocking */
    master->blocking = true;

    /* Dummy wait to ensure the semaphore is 0, no need to check result */
    (void)OSIF_SemaWait(&(master->idleSemaphore), 0U);

    (void)I2C_DRV_MasterReceiveData(instance, rxBuff, rxSize, sendStop);

    /* Wait for transfer to end */
    return I2C_MasterWaitTransferEnd(instance, timeout);
}

/*!
 * @brief Return the current status of the I2C master transfer.The user can call this function to ascertain
 *        the state of the current transfer in an  non-blocking transfer.
 *
 * @param[in] instance: The I2C instance number
 * @param[out] byteRemaining: The number of remaining bytes in the active I2C transfer
 * @return The status of master
 */
status_t I2C_DRV_MasterGetTransferStatus(uint8_t instance, uint32_t *bytesRemaining)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    const i2c_master_state_t *master = s_i2cMasterStatePtr[instance];

    DEVICE_ASSERT(master != NULL);

    if ((bytesRemaining != NULL))
    {
        if (I2C_USING_INTERRUPTS == master->transferType)
        {
            if (master->txSize > 0U)
            {
                /* Send data, remaining bytes = bytes in buffer */
                *bytesRemaining = master->txSize;
            }
            else if (master->rxSize > 0U)
            {
                /* Receive data, remaining bytes = free space in buffer */
                *bytesRemaining = master->rxSize - master->rxPointer;
            }
            else
            {
                *bytesRemaining = 0U;
            }
        }
        else
        {
            /* In DMA communication, the remaining bytes are retrieved
                from the current DMA channel */
            *bytesRemaining = DMA_DRV_GetRemainingBytes(master->dmaChannel);
        }
    }

    return master->status;
}

/*!
 * @brief Handle master operation when I2C interrupt occurs.
 *
 * @param[in] instance: The I2C instance number
 * @return None
 */
void I2C_DRV_MasterIRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    I2C_Type *base = s_i2cBase[instance];
    i2c_master_state_t *master = s_i2cMasterStatePtr[instance];
    uint32_t status0 = 0U;

    DEVICE_ASSERT(master != NULL);

    status0 = I2C_GetStatus0(base);
    /* Check which event caused the interrupt */
    if (I2C_IsStart(base) && I2C_IsSSIntEnable(base))
    {
        I2C_ClearStartFlag(base);
    }
    else if (I2C_IsStop(base) && I2C_IsSSIntEnable(base))
    {
        I2C_ClearStopFlag(base);
    }
    else
    {
        /* Do nothing */
    }

    if (status0 & I2C_STATUS0_BND_Msk)
    {
        if (I2C_USING_INTERRUPTS == master->transferType)
        {
            if (!(master->dirType))
            {
                I2C_MasterHandleTransmitDataRequest(base, master);
            }
            else
            {
                I2C_MasterHandleReceiveDataReadyEvent(base, master);
            }
        }
        else
        {
            /* Clear BND flag */
            I2C_ClearStatus0(base, I2C_STATUS0_BND_Msk);
            /* End I2C master transfer */
            I2C_MasterEndTransfer(base, master, master->sendStop);
            if (master->blocking)
            {
                (void)OSIF_SemaPost(&(master->idleSemaphore));
            }

            if (master->callback != NULL)
            {
                master->callback((uint32_t)I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }
        }
    }

    if (status0 & I2C_STATUS0_ARBLOST_Msk)
    {
        /* Arbitration lost */
        I2C_ClearStatus0(base, I2C_STATUS0_ARBLOST_Msk);

        /* Signal transfer end for blocking transfers */
        if (master->blocking)
        {
            (void)OSIF_SemaPost(&(master->idleSemaphore));
        }
        master->status = STATUS_I2C_ARBITRATION_LOST;

        if (master->callback != NULL)
        {
            master->callback((uint32_t)I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
        }
    }

    if (status0 & I2C_STATUS0_RACK_Msk)
    {
        if ((master->dirType) || (master->txSize != 0))
        {
            /* End transfer: no stop generation (the module will handle that by itself if needed) */
            I2C_MasterEndTransfer(base, master, false);

            /* Signal transfer end for blocking transfers */
            if (master->blocking)
            {
                (void)OSIF_SemaPost(&(master->idleSemaphore));
            }
            master->status = STATUS_I2C_RECEIVED_NACK;

            if (master->callback != NULL)
            {
                master->callback((uint32_t)I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
            }
        }
        /* Receive NACK */
        I2C_ClearStatus0(base, I2C_STATUS0_RACK_Msk);
    }
}

/*!
 * @brief Initialize the I2C slave mode driver.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] userConfig: Pointer to the I2C slave user configuration structure
 * @param[in] slave: Pointer to the I2C slave driver context structure
 * @return The result of execution
 */
status_t I2C_DRV_SlaveInit(uint8_t instance, const i2c_slave_user_config_t *userConfig, i2c_slave_state_t *slave)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(userConfig != NULL);
    DEVICE_ASSERT(slave != NULL);

    I2C_Type *base = s_i2cBase[instance];
    status_t retVal = STATUS_SUCCESS;
    uint32_t inputClock = 0U;

    /* Check to see if the I2C slave instance is already initialized */
    DEVICE_ASSERT(s_i2cSlaveStatePtr[instance] == NULL);

    /* Check the protocol clock frequency */
    retVal = CKGEN_DRV_GetFreq(s_i2cClock[instance], &inputClock);
    DEVICE_ASSERT(STATUS_SUCCESS == retVal);
    DEVICE_ASSERT(inputClock > 0U);

    s_i2cSlaveStatePtr[instance] = slave;

    /* Initialize driver status structure */
    slave->rxBuff = NULL;
    slave->rxSize = 0U;
    slave->rxPointer = 0U;
    slave->txBuff = NULL;
    slave->txSize = 0U;
    slave->status = STATUS_SUCCESS;
    slave->isTransferInProgress = false;
    slave->transferType = userConfig->transferType;
    /* Store DMA channel number used in transfer */
    slave->dmaChannel = userConfig->dmaChannel;
    slave->callback = userConfig->callback;
    slave->callbackParam = userConfig->callbackParam;
    slave->blocking = false;
    slave->is10bitAddr = userConfig->is10bitAddr;
    slave->slaveListening = userConfig->slaveListening;

    /* Initialize the semaphore */
    retVal = OSIF_SemaCreate(&(slave->idleSemaphore), 0U);
    DEVICE_ASSERT(STATUS_SUCCESS == retVal);

    /* Enable clock and reset I2C module */
    CKGEN_DRV_Enable(s_i2cClkNames[instance], true);
    CKGEN_DRV_SoftReset(s_i2cResetNames[instance], true);
    /* Enable I2C interrupt */
    NVIC_EnableIRQ(s_i2cIrqId[instance]);

    /* Set slave address */
    I2C_SetSlaveAddr(base, userConfig->slaveAddress);
    if (userConfig->is10bitAddr)
    {
        /* Set 10bit address */
        I2C_SetADEXT(base, true);
    }
    /* slave wakeup setting */
    if (userConfig->wakeupEn)
    {
        I2C_SetWakeup(base, true);
    }
    /* slave stretch */
    I2C_SetStretch(base, userConfig->stretchEn);
    /* i2c master/slave */
    I2C_SetMSTR(base, I2C_SLAVE);
    if (userConfig->slaveListening)
    {
        I2C_SetInterrupt(base, true);
        /* i2c start stop int */
        I2C_SetSSInterrupt(base, true);
        /* i2c module enable */
        I2C_SetModuleEnable(base, true);
    }

    (void)retVal;

    return STATUS_SUCCESS;
}

/*!
 * @brief De-initialize the I2C slave mode driver.
 *
 * @param[in] instance: The I2C instance number
 * @return The result of execution
 */
status_t I2C_DRV_SlaveDeinit(uint8_t instance)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    const i2c_slave_state_t *slave;

    slave = s_i2cSlaveStatePtr[instance];
    DEVICE_ASSERT(slave != NULL);

    /* Destroy the semaphore */
    (void)OSIF_SemaDestroy(&(slave->idleSemaphore));

    s_i2cSlaveStatePtr[instance] = NULL;
    /* Disable I2C interrupt */
    NVIC_DisableIRQ(s_i2cIrqId[instance]);
    NVIC_ClearPendingIRQ(s_i2cIrqId[instance]);
    /* Reset I2C module,and disable clock */
    CKGEN_DRV_SoftReset(s_i2cResetNames[instance], false);
    CKGEN_DRV_Enable(s_i2cClkNames[instance], false);

    return STATUS_SUCCESS;
}

/*!
 * @brief Provide a buffer for transmitting data.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @return The result of execution
 */
status_t I2C_DRV_SlaveSetTxBuffer(uint8_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];
    DEVICE_ASSERT(slave != NULL);

    slave->txBuff = txBuff;
    slave->txSize = txSize;

    return STATUS_SUCCESS;
}

/*!
 * @brief Provide a buffer for receiving data.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @return The result of execution
 */
status_t I2C_DRV_SlaveSetRxBuffer(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];
    DEVICE_ASSERT(slave != NULL);

    slave->rxBuff = rxBuff;
    slave->rxSize = rxSize;

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a non-blocking send transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @return The result of execution
 */
status_t I2C_DRV_SlaveSendData(uint8_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    I2C_Type *base = s_i2cBase[instance];
    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];
    DEVICE_ASSERT(slave != NULL);

    /* If the slave is in listening mode the user should not use this function or blocking counterpart */
    DEVICE_ASSERT(slave->slaveListening == false);

    /* Check if slave is busy */
    if (slave->isTransferInProgress)
    {
        return STATUS_BUSY;
    }

    slave->txBuff = txBuff;
    slave->txSize = txSize;
    slave->status = STATUS_BUSY;

    if (I2C_USING_DMA == slave->transferType)
    {
        slave->isTransferInProgress = true;
        /* i2c module enable */
        I2C_SetModuleEnable(base, true);
        /* i2c start stop int */
        I2C_SetSSInterrupt(base, true);
        I2C_SetInterrupt(base, true);
        I2C_SlaveTransmitDma(instance);
    }
    else
    {
        slave->isTransferInProgress = true;
        /* i2c start stop int */
        I2C_SetSSInterrupt(base, true);
        I2C_SetInterrupt(base, true);
        /* i2c module enable */
        I2C_SetModuleEnable(base, true);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Perform a blocking send transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] txBuff: Pointer to the data to be transferred
 * @param[in] txSize: Length of the data to be transferred
 * @param[in] timeout: Timeout for the transfer
 * @return The result of execution
 */
status_t I2C_DRV_SlaveSendDataBlocking(uint8_t instance, const uint8_t *txBuff, uint32_t txSize, uint32_t timeout)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(txBuff != NULL);
    DEVICE_ASSERT(txSize > 0U);

    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];
    DEVICE_ASSERT(slave != NULL);

    /* Check if slave is busy */
    if (slave->isTransferInProgress)
    {
        return STATUS_BUSY;
    }

    /* mark transfer as blocking */
    slave->blocking = true;

    /* Dummy wait to ensure the semaphore is 0, no need to check result */
    (void)OSIF_SemaWait(&(slave->idleSemaphore), 0U);

    (void)I2C_DRV_SlaveSendData(instance, txBuff, txSize);

    /* Wait for transfer to end */
    return I2C_SlaveWaitTransferEnd(instance, timeout);
}

/*!
 * @brief Perform a non-blocking receive transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @return The result of execution
 */
status_t I2C_DRV_SlaveReceiveData(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    I2C_Type *base = s_i2cBase[instance];
    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];
    DEVICE_ASSERT(slave != NULL);

    /* If the slave is in listening mode the user should not use this function or blocking counterpart */
    DEVICE_ASSERT(slave->slaveListening == false);

    /* Check if slave is busy */
    if (slave->isTransferInProgress)
    {
        return STATUS_BUSY;
    }

    slave->rxBuff = rxBuff;
    slave->rxSize = rxSize;
    slave->status = STATUS_BUSY;

    if (I2C_USING_DMA == slave->transferType)
    {
        slave->isTransferInProgress = true;
        /* i2c module enable */
        I2C_SetModuleEnable(base, true);
        /* i2c start stop int */
        I2C_SetSSInterrupt(base, true);
        I2C_SetInterrupt(base, true);
        I2C_SlaveReceiveDma(instance);
    }
    else
    {
        slave->isTransferInProgress = true;
        /* i2c start stop int */
        I2C_SetSSInterrupt(base, true);
        I2C_SetInterrupt(base, true);
        /* i2c module enable */
        I2C_SetModuleEnable(base, true);
    }

    return STATUS_SUCCESS;

}

/*!
 * @brief Perform a blocking receive transaction on the I2C bus.
 *
 * @param[in] instance: The I2C instance number
 * @param[in] rxBuff: Pointer to the buffer where to store received data
 * @param[in] rxSize: Length of the data to be transferred
 * @param[in] timeout: Timeout for the transfer
 * @return The result of execution
 */
status_t I2C_DRV_SlaveReceiveDataBlocking(uint8_t instance, uint8_t *rxBuff, uint32_t rxSize, uint32_t timeout)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);
    DEVICE_ASSERT(rxBuff != NULL);
    DEVICE_ASSERT(rxSize > 0U);

    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];
    DEVICE_ASSERT(slave != NULL);

    /* Check if slave is busy */
    if (slave->isTransferInProgress)
    {
        return STATUS_BUSY;
    }

    /* mark transfer as blocking */
    slave->blocking = true;

    /* Dummy wait to ensure the semaphore is 0, no need to check result */
    (void)OSIF_SemaWait(&(slave->idleSemaphore), 0U);

    (void)I2C_DRV_SlaveReceiveData(instance, rxBuff, rxSize);

    /* Wait for transfer to end */
    return I2C_SlaveWaitTransferEnd(instance, timeout);
}

/*!
 * @brief Return the current status of the I2C slave transfer. The user can call this function to ascertain
 *        the state of the current transfer in an non-blocking transfer.
 *
 * @param[in] instance: The I2C instance number
 * @param[out] byteRemaining: The number of remaining bytes in the active I2C transfer
 * @return The status of slave
 */
status_t I2C_DRV_SlaveGetTransferStatus(uint8_t instance, uint32_t *bytesRemaining)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    const i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];

    DEVICE_ASSERT(slave != NULL);

    if ((bytesRemaining != NULL))
    {
        if (I2C_USING_INTERRUPTS == slave->transferType)
        {
            if (slave->txSize > 0U)
            {
                /* Send data, remaining bytes = bytes in buffer */
                *bytesRemaining = slave->txSize;
            }
            else if (slave->rxSize > 0U)
            {
                /* Receive data, remaining bytes = free space in buffer */
                *bytesRemaining = slave->rxSize - slave->rxPointer;
            }
            else
            {
                *bytesRemaining = 0U;
            }
        }
        else
        {
            /* In DMA communication, the remaining bytes are retrieved
                from the current DMA channel */
            *bytesRemaining = DMA_DRV_GetRemainingBytes(slave->dmaChannel);
        }
    }

    return slave->status;
}

/*!
 * @brief Abort a non-blocking I2C slave transmission or reception.
 *
 * @param[in] instance: The I2C instance number
 * @return The result of execution
 */
status_t I2C_DRV_SlaveAbortTransferData(uint8_t instance)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    I2C_Type *base = s_i2cBase[instance];
    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];

    DEVICE_ASSERT(slave != NULL);

    if (!slave->slaveListening)
    {
        /* End transfer: force stop generation */
        slave->status = STATUS_I2C_ABORTED;
        I2C_SlaveEndTransfer(base, slave);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Handle slave operation when I2C interrupt occurs.
 *
 * @param[in] instance: The I2C instance number
 * @return None
 */
void I2C_DRV_SlaveIRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    I2C_Type *base = s_i2cBase[instance];
    i2c_slave_state_t *slave = s_i2cSlaveStatePtr[instance];
    uint32_t status0 = 0U;
    DEVICE_ASSERT(slave != NULL);

    status0 = I2C_GetStatus0(base);

    if (I2C_IsStart(base) && I2C_IsSSIntEnable(base))
    {
        I2C_ClearStartFlag(base);
        if ((slave->slaveListening) && (I2C_USING_DMA == slave->transferType))
        {
            I2C_ClearStatus0(base, I2C_STATUS0_RACK_Msk);
            I2C_SlaveStartDmaTransfer(instance);
        }
    }
    else if (I2C_IsStop(base) && I2C_IsSSIntEnable(base))
    {
        I2C_ClearStopFlag(base);
        if (STATUS_BUSY == slave->status)
        {
            /* Report success if no error was recorded */
            slave->status = STATUS_SUCCESS;
        }
        I2C_SlaveEndTransferHandler(base, slave);
    }
    if (I2C_USING_INTERRUPTS == slave->transferType)
    {
        /* Check which event caused the interrupt */
        if (status0 & I2C_STATUS0_SAMF_Msk)
        {
            /* Address match */
            I2C_SlaveHandleAddressValidEvent(base, slave);
        }
        if (status0 & I2C_STATUS0_BND_Msk)
        {
            /* Check slave transfer direction */
            if (I2C_GetStatus0(base) & I2C_STATUS0_SRW_Msk)
            {
                I2C_SlaveHandleTransmitDataEvent(base, slave);
            }
            else
            {
                I2C_SlaveHandleReceiveDataEvent(base, slave);
            }
        }
    }
    else
    {
        if (status0 & I2C_STATUS0_SAMF_Msk)
        {
            I2C_ClearStatus0(base, I2C_STATUS0_SAMF_Msk);
        }
        if (status0 & I2C_STATUS0_RACK_Msk)
        {
            /* DISABLE NACK interrupt */
            I2C_SetNackInterrupt(base, false);
        }
        if (status0 & I2C_STATUS0_BND_Msk)
        {
            I2C_ClearStatus0(base, I2C_STATUS0_BND_Msk);
        }
    }
}

/*!
 * @brief Handle operation when I2C interrupt occurs.
 *
 * @param[in] instance: The I2C instance number
 * @return None
 */
void I2C_DRV_IRQHandler(uint8_t instance)
{
    DEVICE_ASSERT(instance < I2C_INSTANCE_MAX);

    I2C_Type *base = s_i2cBase[instance];

    if (I2C_IsMaster(base) || (I2C_GetStatus0(base) & I2C_STATUS0_ARBLOST_Msk))
    {
        I2C_DRV_MasterIRQHandler(instance);
    }
    else
    {
        I2C_DRV_SlaveIRQHandler(instance);
    }
}

/*!
 * @brief Handle master operation when I2C interrupt occurs.
 *
 * @param[out] instance: The I2C instance number
 * @return None
 */
void I2C_DRV_MasterGetDefaultConfig(i2c_master_user_config_t *config)
{
    config->slaveAddress = 0x60U;
    config->is10bitAddr = false;
    config->baudRate = 100000U;
    config->transferType = I2C_USING_INTERRUPTS;
    config->dmaChannel = 0U;
    config->callback = NULL;
    config->callbackParam = NULL;
    config->syncEn = true;
    config->arbitration = false;
}

/*!
 * @brief Gets the default configuration structure for slave.
 *
 * @param[out] config: Pointer to configuration structure
 * @return None
 */
void I2C_DRV_SlaveGetDefaultConfig(i2c_slave_user_config_t *config)
{
    config->slaveAddress = 0x60U;
    config->is10bitAddr = false;
    config->stretchEn = true;
    config->wakeupEn = false;
    config->slaveListening = true;
    config->transferType = I2C_USING_INTERRUPTS;
    config->dmaChannel = 0U;
    config->callback = NULL;
    config->callbackParam = NULL;
}

/* =============================================  EOF  ============================================== */
