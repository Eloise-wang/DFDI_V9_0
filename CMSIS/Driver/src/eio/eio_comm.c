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
 * @file eio_comm.c
 *
 * @brief This file provides eio comm integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "eio_hw.h"
#include "eio_comm.h"
#include "ckgen_drv.h"
#include "dma_drv.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* Table of base addresses for EIO instances */
EIO_Type * const g_eioBase[EIO_INSTANCE_COUNT] = EIO_BASE_PTRS;

/* Pointer to device state structures. This structure contains data common to all drivers on one device */
eio_device_state_t *g_eioDeviceStatePtr[EIO_INSTANCE_COUNT] = {NULL};

/* Table for EIO IRQ numbers */
const IRQn_Type g_eioIrqId[EIO_INSTANCE_COUNT] = EIO_IRQS;

/* EIO clock sources, for getting the input clock frequency */
const clock_names_t g_eioClock[EIO_INSTANCE_COUNT] = EIO_CLOCK_NAMES ;

/* EIO DMA request sources */
const uint8_t g_eioDMASrc[EIO_INSTANCE_COUNT][EIO_MAX_SHIFTER_COUNT] =
    {{(uint8_t)DMA_REQ_EIO_SHIFTER0, (uint8_t)DMA_REQ_EIO_SHIFTER1,
      (uint8_t)DMA_REQ_EIO_SHIFTER2, (uint8_t)DMA_REQ_EIO_SHIFTER3}};

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Allocate timers and shifters for a new driver instance
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] driver: Pointer to the EIO common driver context structure
 * @return Error or success status returned by API
 */
static status_t EIO_DRV_Allocate(uint32_t instance, eio_common_state_t *driver)
{
    uint8_t count;                  /* used to iterate through resources (shifters/timers) */
    uint8_t step;                   /* keeps track of how many resources are needed for this driver */
    uint8_t size;                   /* total number of resources */
    uint8_t mask;                   /* bit-mask corresponding to current resources */
    uint8_t resourceAllocation;     /* current resource allocation map */
    status_t status = STATUS_ERROR; /* assume the worst: no resources found for this driver */

    /* Find free resources for a new driver. Drivers may need one or two adjacent shifters and timers */
    resourceAllocation = g_eioDeviceStatePtr[instance]->resourceAllocation;
    step = driver->resourceCount;
    size = EIO_MAX_SHIFTER_COUNT;
    count = 0U;
    mask = (uint8_t)((1U << step) - 1U);
    (void)OSIF_MutexLock(&(g_eioDeviceStatePtr[instance]->resourceLock), OSIF_WAIT_FOREVER);

    /* find available shifters and timers for the driver */
    while ((status == STATUS_ERROR) && (((uint8_t)(count + step)) <= size))
    {
        if ((resourceAllocation & mask) == 0U)
        {
            /* resources found */
            driver->resourceIndex = count;
            /* mark resources as busy */
            g_eioDeviceStatePtr[instance]->resourceAllocation |= mask;
            status = STATUS_SUCCESS;
        }
        /* continue searching */
        count += 1U;
        mask <<= 1;
    }
    (void)OSIF_MutexUnlock(&(g_eioDeviceStatePtr[instance]->resourceLock));

    return status;
}

/*!
 * @brief De-allocate timers and shifters for a driver instance
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] driver: Pointer to the EIO common driver context structure
 * @return none
 */
static void EIO_DRV_Deallocate(uint32_t instance, const eio_common_state_t *driver)
{
    uint8_t mask;

    mask = (uint8_t)((1U << driver->resourceCount) - 1U);
    mask <<= driver->resourceIndex;
    (void)OSIF_MutexLock(&(g_eioDeviceStatePtr[instance]->resourceLock), OSIF_WAIT_FOREVER);
    g_eioDeviceStatePtr[instance]->resourceAllocation &= (uint8_t)~mask;
    (void)OSIF_MutexUnlock(&(g_eioDeviceStatePtr[instance]->resourceLock));
}

/*!
 * @brief Initializes the resources for the current driver
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] driver: Pointer to the EIO common driver context structure
 * @return none
 */
static void EIO_DRV_ResourcesInit(uint32_t instance, const eio_common_state_t *driver)
{
    uint8_t resource;
    uint8_t resourceCount;
    uint8_t resourceIndex;
    EIO_Type *baseAddr;

    baseAddr = g_eioBase[instance];
    resourceCount = driver->resourceCount;
    resourceIndex = driver->resourceIndex;
    for (resource = resourceIndex; resource < (resourceIndex + resourceCount); resource++)
    {
        /* Ensure all shifters/timers are disabled */
        EIO_SetShifterMode(baseAddr, resource, EIO_SHIFTER_MODE_DISABLED);
        EIO_SetTimerMode(baseAddr, resource, EIO_TIMER_MODE_DISABLED);
        /* Ensure all interrupts and DMA requests are disabled */
        EIO_SetShifterInterrupt(baseAddr, (uint8_t)(1U << resource), false);
        EIO_SetShifterErrorInterrupt(baseAddr, (uint8_t)(1U << resource), false);
        EIO_SetTimerInterrupt(baseAddr, (uint8_t)(1U << resource), false);
        EIO_SetShifterDMARequest(baseAddr, (uint8_t)(1U << resource), false);
        /* Clear any leftover flags */
        EIO_ClearShifterStatus(baseAddr, resource);
        EIO_ClearShifterErrorStatus(baseAddr, resource);
        EIO_ClearTimerStatus(baseAddr, resource);
    }
}

/*!
 * @brief Initializes the EIO device
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] deviceState: Pointer to the EIO device context structure
 * @return Error or success status returned by API
 */
status_t EIO_DRV_InitDevice(uint32_t instance, eio_device_state_t *deviceState)
{
    EIO_Type *baseAddr;
    uint8_t count;
    status_t osifStat = STATUS_SUCCESS;
    if (g_eioDeviceStatePtr[instance] == NULL)
    {
        CKGEN_DRV_Enable(CLK_EIO, true);
        CKGEN_DRV_SoftReset(SRST_EIO, true);
        baseAddr = g_eioBase[instance];
        /* Create resource allocation mutex */
        osifStat = OSIF_MutexCreate(&(deviceState->resourceLock));
        DEVICE_ASSERT(osifStat == STATUS_SUCCESS);
        /* Reset EIO module */
        EIO_Init(baseAddr);
        /* Enable EIO interrupt in the interrupt manager */
        NVIC_EnableIRQ(g_eioIrqId[instance]);
        /* Enable module */
        EIO_SetEnable(baseAddr, true);
        /* Initialize device structure */
        deviceState->resourceAllocation = 0U;
        /* Clear state structures array */
        for (count = 0U; count < EIO_MAX_SHIFTER_COUNT; count++)
        {
            deviceState->eioStatePtr[count] = NULL;
        }
        g_eioDeviceStatePtr[instance] = deviceState;
    }

    (void)osifStat;
    return STATUS_SUCCESS;
}

/*!
 * @brief De-initializes the EIO device
 *
 * @param[in] instance: EIO peripheral instance number
 * @return Error or success status returned by API
 */
status_t EIO_DRV_DeinitDevice(uint32_t instance)
{
    EIO_Type *baseAddr;
    baseAddr = g_eioBase[instance];
    CKGEN_DRV_Enable(CLK_EIO, true);
    CKGEN_DRV_SoftReset(SRST_EIO, true);
    /* Reset EIO module */
    EIO_Init(baseAddr);

    CKGEN_DRV_SoftReset(SRST_EIO, false);
    CKGEN_DRV_Enable(CLK_EIO, false);
    /* Disable EIO interrupt in the interrupt manager */
    NVIC_DisableIRQ(g_eioIrqId[instance]);
    /* Destroy resource allocation mutex */
    (void)OSIF_MutexDestroy(&(g_eioDeviceStatePtr[instance]->resourceLock));
    /* Free resources */
    g_eioDeviceStatePtr[instance] = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Resets the EIO device
 *
 * @param[in] instance: EIO peripheral instance number
 * @return Error or success status returned by API
 */
status_t EIO_DRV_Reset(uint32_t instance)
{
    EIO_Type *baseAddr;

    baseAddr = g_eioBase[instance];
    /* Reset EIO module */
    EIO_Init(baseAddr);

    return STATUS_SUCCESS;
}

/*!
 * @brief Initializes an instance of EIO driver
 *
 * @param[in] instance: EIO peripheral instance number
 * @param[in] driver: Pointer to the EIO common driver context structure
 * @return Error or success status returned by API
 */
status_t EIO_DRV_InitDriver(uint32_t instance, eio_common_state_t *driver)
{
    uint16_t count;
    status_t retCode;
    eio_common_state_t **eioStatePtr;

    /* allocate times and shifters for the driver */
    retCode = EIO_DRV_Allocate(instance, driver);
    if (retCode != STATUS_SUCCESS)
    {   /* no more resources available */
        return retCode;
    }
    /* get driver list for this device instance */
    eioStatePtr = g_eioDeviceStatePtr[instance]->eioStatePtr;
    /* find an empty state structure slot for the driver */
    for (count = 0U; count < EIO_MAX_SHIFTER_COUNT; count++)
    {
        if (eioStatePtr[count] == NULL)
        {
            /* found it, place the new driver struct here */
            eioStatePtr[count] = driver;
            break;
        }
    }
    /* no need to check if there was room since allocation was successful */
    DEVICE_ASSERT(count < EIO_MAX_SHIFTER_COUNT);

    /* initialize the allocated resources */
    EIO_DRV_ResourcesInit(instance, driver);

    /* init the rest of the common state structure */
    driver->isr = NULL;
    driver->instance = instance;

    return STATUS_SUCCESS;
}

/*!
 * @brief De-initializes an instance of EIO driver
 *
 * @param[in] driver: Pointer to the EIO common driver context structure
 * @return Error or success status returned by API
 */
status_t EIO_DRV_DeinitDriver(const eio_common_state_t *driver)
{
    uint16_t count;
    uint32_t instance;
    eio_common_state_t **eioStatePtr;

    instance = driver->instance;
    /* get driver list for this device instance */
    eioStatePtr = g_eioDeviceStatePtr[instance]->eioStatePtr;
    /* find the driver in the list */
    for (count = 0U; count < EIO_MAX_SHIFTER_COUNT; count++)
    {
        if (eioStatePtr[count] == driver)
        {
            /* found it - remove it */
            eioStatePtr[count] = NULL;
            break;
        }
    }
    DEVICE_ASSERT(count < EIO_MAX_SHIFTER_COUNT);

    /* de-allocate timers and shifters for the driver */
    EIO_DRV_Deallocate(instance, driver);

    return STATUS_SUCCESS;
}

/*!
 * @brief Declaration of EIO_IRQHandler.
 *
 * @param[in] none
 * @return none
 */
void EIO_IRQHandler(void)
{
    uint16_t count;
    uint32_t instance;
    uint32_t resourceMask;
    uint32_t shifterEvents;
    uint32_t shifterErrorEvents;
    uint32_t timerEvents;
    uint32_t enabledInterrupts;
     EIO_Type *baseAddr;
    eio_common_state_t * const *driverList;
    eio_common_state_t *driverState;

    instance = 0U;
    baseAddr = g_eioBase[instance];
    /* get masks of EIO events */
    /* read enabled interrupts in a separate instruction to avoid MISRA violation */
    enabledInterrupts = EIO_GetAllShifterInterrupt(baseAddr);
    shifterEvents = EIO_GetAllShifterStatus(baseAddr) & enabledInterrupts;
    enabledInterrupts = EIO_GetAllShifterErrorInterrupt(baseAddr);
    shifterErrorEvents = EIO_GetAllShifterErrorStatus(baseAddr) & enabledInterrupts;
    enabledInterrupts = EIO_GetAllTimerInterrupt(baseAddr);
    timerEvents = EIO_GetAllTimerStatus(baseAddr) & enabledInterrupts;
    /* get driver list for this device instance */
    driverList = g_eioDeviceStatePtr[instance]->eioStatePtr;
    /* check which driver instances need to be serviced */
    for (count = 0U; count < EIO_MAX_SHIFTER_COUNT; count++)
    {
        driverState = driverList[count];
        /* check if driver is initialized and uses interrupts */
        if ((driverState != NULL) && (driverState->isr != NULL))
        {
            /* compute mask of shifters/timers used by this driver */
            resourceMask = ((1UL << driverState->resourceCount) - 1U) << driverState->resourceIndex;
            /* check if this instance has any pending events */
            if (((shifterEvents & resourceMask) != 0U) ||
                    ((shifterErrorEvents & resourceMask) != 0U) ||
                    ((timerEvents & resourceMask) != 0U))
            {
                /* there is an event for the current instance - call the isr */
                (driverState->isr)(driverState);
            }
        }
    }
}

/* =============================================  EOF  ============================================== */
