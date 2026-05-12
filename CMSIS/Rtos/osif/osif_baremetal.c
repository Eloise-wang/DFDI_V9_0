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
 * @file osif_baremetal.c
 *
 * @brief This file provides baremetal operating system interface access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "osif.h"
#include "ckgen_drv.h"

/* ============================================  Define  ============================================ */
/*!< Converts milliseconds to ticks - in this case, one tick = one millisecond */
#define MSEC_TO_TICK(msec) (msec)

/* ===========================================  Typedef  ============================================ */


/* ==========================================  Variables  =========================================== */
static volatile uint32_t s_osif_tick_cnt = 0U;

/* ====================================  Functions declaration  ===================================== */
void SysTick_Handler(void);

/* ======================================  Functions define  ======================================== */

#if defined (USING_OS_FREERTOS)
#error "Wrong OSIF selected. Please define symbol USING_OS_BAREMETAL (or no OS define) \
        in project settings or change the OSIF variant"
#endif

/*!
 * @brief tick count.
 *
 * @param[in] none
 * @return  none
 */
static inline void osif_Tick(void)
{
    s_osif_tick_cnt++;
}

/*!
 * @brief Get current tick count value.
 *
 * @param[in] none
 * @return  current tick count value
 */
static inline uint32_t osif_GetCurrentTickCount(void)
{
    return s_osif_tick_cnt;
}

/*!
 * @brief Systick interrupt function.
 *
 * @param[in] none
 * @return  none
 */
void SysTick_Handler(void)
{
    osif_Tick();
}

/*!
 * @brief Update tick config, the counting period is 1ms.
 *
 * @param[in] none
 * @return  none
 */
static inline void osif_UpdateTickConfig(void)
{
    uint32_t core_freq = 0U;
    static bool first_init = true;

    /* Get the correct name of the core clock */
    (void)CKGEN_DRV_GetFreq(CORE_CLK, &core_freq);
    DEVICE_ASSERT(core_freq > 0U);

    AC784X_SYSTICK->CSR &= ~AC784X_SysTick_CSR_ENABLE_Msk;
    AC784X_SYSTICK->RVR = (core_freq / 1000U) - 1U;
    if (first_init)
    {
        /* only initialize CVR on the first entry, to not cause time drift */
        AC784X_SYSTICK->CVR = 0U;
        first_init = false;
    }
    AC784X_SYSTICK->CSR = AC784X_SysTick_CSR_ENABLE_Msk  | \
                          AC784X_SysTick_CSR_TICKINT_Msk | \
                          AC784X_SysTick_CSR_CLKSOURCE_Msk;
}

/*!
 * @brief Disable the global interrupt.
 *
 * @param[in] none
 * @return  none
 */
static inline void osif_DisableIrqGlobal(void)
{
    /* Disable the global interrupt */
    __disable_irq();
}

/*!
 * @brief Enable the global interrupt.
 *
 * @param[in] none
 * @return  none
 */
static inline void osif_EnableIrqGlobal(void)
{
    /* Enable the global interrupt */
    __enable_irq();
}

/*!
 * @brief Delays execution for a number of milliseconds.
 *
 * @param[in] delay: Time delay in milliseconds
 * @return none
 */
void OSIF_TimeDelay(const uint32_t delay)
{
    uint32_t crt_ticks, delta;

    osif_UpdateTickConfig();
    uint32_t start = osif_GetCurrentTickCount();
    uint32_t delay_ticks = MSEC_TO_TICK(delay);
    do
    {
        crt_ticks = osif_GetCurrentTickCount();
        delta = crt_ticks - start;
    } while (delta < delay_ticks);
}

/*!
 * @brief Returns the number of miliseconds elapsed since starting the systick timer.
 *
 * @param[in] none
 * @return the number of miliseconds elapsed
 */
uint32_t OSIF_GetMilliseconds(void)
{
    /* This assumes that 1 tick = 1 millisecond */
    return osif_GetCurrentTickCount();
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
status_t OSIF_MutexLock(const mutex_t * const pMutex, const uint32_t timeout)
{
    (void)pMutex;
    (void)timeout;

    return STATUS_SUCCESS;
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
    (void)pMutex;

    return STATUS_SUCCESS;
}

/*!
 * @brief Create an unlocked mutex.
 *
 * @param[in] pMutex: reference to the mutex object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: mutex created
 *          - STATUS_ERROR: mutex could not be created
 */
status_t OSIF_MutexCreate(mutex_t * const pMutex)
{
    (void)pMutex;

    return STATUS_SUCCESS;
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
    (void)pMutex;

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
status_t OSIF_SemaWait(semaphore_t * const pSem, const uint32_t timeout)
{
    DEVICE_ASSERT(pSem != NULL);

    status_t ret = STATUS_SUCCESS;

    osif_UpdateTickConfig();
    if (0U == timeout)
    {
        /* when the timeout is 0 the wait operation is the equivalent of try_wait,
            meaning that if the semaphore is 0 return immediately with an error code
        */
        if (0U == *pSem)
        {
            ret = STATUS_TIMEOUT;
        }
    }
    else
    {
        /* timeout is not 0 */
        uint32_t timeoutTicks;
        if (OSIF_WAIT_FOREVER == timeout)
        {
            timeoutTicks = OSIF_WAIT_FOREVER;
        }
        else
        {
            /* Convert timeout from milliseconds to ticks. */
            timeoutTicks = MSEC_TO_TICK(timeout);
        }
        uint32_t start = osif_GetCurrentTickCount();
        uint32_t end = (uint32_t)(start + timeoutTicks);
        uint32_t max = end - start;
        while (0U == *pSem)
        {
            uint32_t crt_ticks = osif_GetCurrentTickCount();
            uint32_t delta = crt_ticks - start;
            if ((timeoutTicks != OSIF_WAIT_FOREVER) && (delta > max))
            {
                /* Timeout occured, stop waiting and return fail code */
                ret = STATUS_TIMEOUT;
                break;
            }
        }
    }

    if (STATUS_SUCCESS == ret)
    {
        osif_DisableIrqGlobal();
        --(*pSem);
        osif_EnableIrqGlobal();
    }

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
    DEVICE_ASSERT(pSem != NULL);

    status_t ret = STATUS_SUCCESS;
    osif_DisableIrqGlobal();
    if (*pSem != 255U)
    {
        ++(*pSem);
    }
    else
    {
        ret = STATUS_ERROR;
    }

    osif_EnableIrqGlobal();

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
status_t OSIF_SemaCreate(semaphore_t * const pSem,
                         const uint8_t initValue)
{
    DEVICE_ASSERT(pSem != NULL);

    osif_DisableIrqGlobal();
    *pSem = initValue;
    osif_EnableIrqGlobal();

    return STATUS_SUCCESS;
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
    DEVICE_ASSERT(pSem != NULL);

    (void)pSem;

    return STATUS_SUCCESS;
}

/* =============================================  EOF  ============================================== */
