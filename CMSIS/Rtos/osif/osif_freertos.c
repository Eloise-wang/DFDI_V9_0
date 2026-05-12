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
 * @file osif_freertos.c
 *
 * @brief This file provides freertos operating system interface access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include <stdbool.h>
#include "osif.h"
#include "FreeRTOS.h"
#include "task.h"

/* ============================================  Define  ============================================ */
/*!< Converts milliseconds to ticks - in this case, one tick = one millisecond */
#define MSEC_TO_TICK(msec) (msec)

/* define a macro to access the FreeRTOS mutex/semaphore handle from an
 * osif mutex_t or semaphore_t */
#if configSUPPORT_STATIC_ALLOCATION == 1
#define SEM_HANDLE(sem) ((sem).handle)
#else /* configSUPPORT_STATIC_ALLOCATION == 0, it's dynamic allocation */
#define SEM_HANDLE(sem) (sem)
#endif

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */
#if !defined (USING_OS_FREERTOS)
#error "Wrong OSIF selected. Please define symbol USING_OS_FREERTOS in project settings or change the OSIF variant"
#endif

/*!
 * @brief This function returns true if the current execution context
 *        is from an ISR and false if execution is in normal thread mode.
 *
 * @param[in] none
 * @return isr state
 *         - true：the current execution context is from an interrupt
 *         - false：the current execution context is from an thread mode
 */
static inline bool osif_IsIsrContext(void)
{
    bool is_isr = false;

    /* Cortex M device - read ICSR[IPSR] value */
    uint32_t ipsr_code = (uint32_t)( (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos);
    if (ipsr_code != 0U)
    {
        is_isr = true;
    }

    return is_isr;
}

/*!
 * @brief Delays execution for a number of milliseconds.
 *
 * @param[in] delay: Time delay in milliseconds
 * @return none
 */
void OSIF_TimeDelay(uint32_t delay)
{
    /* One dependency for FreeRTOS config file */
    /* INCLUDE_vTaskDelay */
    vTaskDelay(MSEC_TO_TICK(delay));
}

/*!
 * @brief Returns the number of miliseconds elapsed since starting the systick timer.
 *
 * @param[in] none
 * @return the number of miliseconds elapsed
 */
uint32_t OSIF_GetMilliseconds(void)
{
    /*
     * Return the tick count in miliseconds
     * Note: if configTICK_RATE_HZ is less than 1000, the return value will be truncated
     * to 32-bit wide for large values of the tick count
     */
    return (uint32_t)((((uint64_t) xTaskGetTickCount()) * 1000U) / configTICK_RATE_HZ);
}

/*!
 * @brief Waits for a mutex and locks it.
 *
 * @param[in] pMutex: reference to the mutex object
 * @param[in] timeout: time-out value in milliseconds
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: mutex lock operation success
 *          - STATUS_ERROR: mutex already owned by current thread
 *          - STATUS_TIMEOUT: mutex lock operation timed out
 */
status_t OSIF_MutexLock(const mutex_t * const pMutex,
                        const uint32_t timeout)
{
    /* The (pMutex == NULL) case is a valid option, signaling that the mutex does
     * not need to be locked - do not use DEV_ASSERT in this case */
    uint32_t timeoutTicks;
    status_t ret = STATUS_SUCCESS;
    TaskHandle_t mutex_holder_handle;
    TaskHandle_t current_task_handle;
    BaseType_t operation_status = pdFAIL;

    if (pMutex != NULL)
    {
        SemaphoreHandle_t mutex_handle = SEM_HANDLE(*pMutex);
        /* Two dependencies for FreeRTOS config file */
        /* INCLUDE_xQueueGetMutexHolder */
        /* INCLUDE_xTaskGetCurrentTaskHandle */
        mutex_holder_handle = xSemaphoreGetMutexHolder(mutex_handle);
        current_task_handle = xTaskGetCurrentTaskHandle();

        /* If pMutex has been locked by current task, return error. */
        if (mutex_holder_handle == current_task_handle)
        {
            ret = STATUS_ERROR;
        }
        else
        {
            /* Convert timeout from millisecond to tick. */
            if (OSIF_WAIT_FOREVER == timeout)
            {
                timeoutTicks = portMAX_DELAY;
            }
            else
            {
                timeoutTicks = MSEC_TO_TICK(timeout);
            }

            /* Try to take the semaphore */
            operation_status = xSemaphoreTake(mutex_handle, timeoutTicks);

            ret = (operation_status == pdPASS) ? STATUS_SUCCESS : STATUS_TIMEOUT;
        }
    }

    return ret;
}

/*!
 * @brief Unlocks a previously locked mutex.
 *
 * @param[in] pMutex: reference to the mutex object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: mutex unlock operation success
 *          - STATUS_ERROR: mutex unlock failed
 */
status_t OSIF_MutexUnlock(const mutex_t * const pMutex)
{
    /* The (pMutex == NULL) case is a valid option, signaling that the mutex does
     * not need to be unlocked - do not use DEV_ASSERT in this case */

    status_t ret = STATUS_SUCCESS;
    TaskHandle_t mutex_holder_handle;
    TaskHandle_t current_task_handle;
    BaseType_t operation_status = pdFAIL;

    if (pMutex != NULL)
    {
        SemaphoreHandle_t mutex_handle = SEM_HANDLE(*pMutex);
        /* Two dependencies for FreeRTOS config file */
        /* INCLUDE_xQueueGetMutexHolder */
        /* INCLUDE_xTaskGetCurrentTaskHandle */
        mutex_holder_handle = xSemaphoreGetMutexHolder(mutex_handle);
        current_task_handle = xTaskGetCurrentTaskHandle();

        /* If pMutex is not locked by current task, return error. */
        if (mutex_holder_handle != current_task_handle)
        {
            ret = STATUS_ERROR;
        }
        else
        {
            operation_status = xSemaphoreGive(mutex_handle);
            ret = (operation_status == pdPASS) ? STATUS_SUCCESS : STATUS_ERROR;
        }
    }

    return ret;
}

/*!
 * @brief Create an unlocked mutex.
 *
 * @param[in] pMutex: reference to the mutex object
 * @return  One of the possible status code
 *          - STATUS_SUCCESS: mutex created
 *          - STATUS_ERROR: mutex could not be created
 */
status_t OSIF_MutexCreate(mutex_t * const pMutex)
{
    /* The (pMutex == NULL) case is a valid option, signaling that the mutex does
     * not need to be created - do not use DEV_ASSERT in this case */

    status_t ret = STATUS_SUCCESS;

    if (pMutex != NULL)
    {
#if configSUPPORT_STATIC_ALLOCATION == 1
        pMutex->handle = xSemaphoreCreateMutexStatic(&(pMutex->buffer));
        if (pMutex->handle == NULL)
        {
            ret = STATUS_ERROR; /* mutex not created successfully */
        }
#else /* configSUPPORT_STATIC_ALLOCATION == 0, it's dynamic allocation */
        *pMutex = xSemaphoreCreateMutex();
        if (*pMutex == NULL)
        {
            ret = STATUS_ERROR; /* mutex not created successfully */
        }
#endif
    }

    return ret;
}

/*!
 * @brief Destroys a previously created mutex.
 *
 * @param[in] pMutex: reference to the mutex object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: mutex destroyed
 */
status_t OSIF_MutexDestroy(const mutex_t * const pMutex)
{
    /* The (pMutex == NULL) case is a valid option, signaling that the mutex does
     * not need to be destroyed - do not use DEV_ASSERT in this case */

    if (pMutex != NULL)
    {
        SemaphoreHandle_t mutex_handle = SEM_HANDLE(*pMutex);
        DEV_ASSERT(mutex_handle);
        vSemaphoreDelete(mutex_handle);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Decrement a semaphore with timeout.
 *
 * @param[in] pSem: reference to the semaphore object
 * @param[in] timeout: time-out value in milliseconds
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: semaphore wait operation success
 *          - STATUS_TIMEOUT: semaphore wait timed out
 */
status_t OSIF_SemaWait(semaphore_t * const pSem,
                       const uint32_t timeout)
{
    DEV_ASSERT(pSem);

    SemaphoreHandle_t sem_handle = SEM_HANDLE(*pSem);
    uint32_t timeoutTicks;
    BaseType_t operation_status;
    status_t ret;

    /* Convert timeout from millisecond to ticks */
    if (OSIF_WAIT_FOREVER == timeout)
    {
        timeoutTicks = portMAX_DELAY;
    }
    else
    {
        timeoutTicks = MSEC_TO_TICK(timeout);
    }

    /* Try to take the semaphore */
    operation_status = xSemaphoreTake(sem_handle, timeoutTicks);

    ret = (operation_status == pdPASS) ? STATUS_SUCCESS : STATUS_TIMEOUT;

    return ret;
}

/*!
 * @brief Increment a semaphore.
 *
 * @param[in] pSem: reference to the semaphore object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: semaphore post operation success
 *          - STATUS_ERROR: semaphore could not be incremented
 */
status_t OSIF_SemaPost(semaphore_t * const pSem)
{
    DEV_ASSERT(pSem);

    BaseType_t operation_status = pdFAIL;
    status_t ret;

    SemaphoreHandle_t sem_handle = SEM_HANDLE(*pSem);
    /* Check if the post operation is executed from ISR context */
    bool is_isr = osif_IsIsrContext();
    if (is_isr)
    {
        /* Execution from exception handler (ISR) */
        BaseType_t taskWoken = pdFALSE;
        operation_status = xSemaphoreGiveFromISR(sem_handle, &taskWoken);

        if (operation_status == pdPASS)
        {
            /* Perform a context switch if necessary */
            portYIELD_FROM_ISR(taskWoken);
        }
    }
    else
    {
        /* Execution from task */
        operation_status = xSemaphoreGive(sem_handle);
    }

    /* pdFAIL in case that the semaphore is full */
    ret = (operation_status == pdPASS) ? STATUS_SUCCESS : STATUS_ERROR;

    return ret;
}

/*!
 * @brief Creates a semaphore with a given value.
 *
 * @param[in] pSem: reference to the semaphore object
 * @param[in] initValue: initial value of the semaphore
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: semaphore created
 *          - STATUS_ERROR: semaphore could not be created
 */
status_t OSIF_SemaCreate(semaphore_t * const pSem, const uint8_t initValue)
{
    DEV_ASSERT(pSem);

    status_t ret = STATUS_SUCCESS;

#if configSUPPORT_STATIC_ALLOCATION == 1
    pSem->handle = xSemaphoreCreateCountingStatic(0xFFu, initValue, &(pSem->buffer));
    if (pSem->handle == NULL)
    {
        ret = STATUS_ERROR; /* semaphore not created successfully */
    }
#else /* configSUPPORT_STATIC_ALLOCATION == 0, it's dynamic allocation */
    *pSem = xSemaphoreCreateCounting(0xFFu, initValue);
    if (*pSem == NULL)
    {
        ret = STATUS_ERROR; /* semaphore not created successfully */
    }
#endif

    return ret;
}

/*!
 * @brief Destroys a previously created semaphore.
 *
 * @param[in] pSem: reference to the semaphore object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: semaphore destroyed
 */
status_t OSIF_SemaDestroy(const semaphore_t * const pSem)
{
    DEV_ASSERT(pSem);
    SemaphoreHandle_t sem_handle = SEM_HANDLE(*pSem);

    DEV_ASSERT(sem_handle);
    vSemaphoreDelete(sem_handle);

    return STATUS_SUCCESS;
}

/* =============================================  EOF  ============================================== */
