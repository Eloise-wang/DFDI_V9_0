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
 * @file osif.h
 *
 * @brief This file provides osif integration functions interface.
 *
 */

#ifndef _OSIF_H
#define _OSIF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"

/* ============================================  Define  ============================================ */
#define OSIF_WAIT_FOREVER (0xFFFFFFFFUL)

/* ===========================================  Typedef  ============================================ */
#ifdef USING_OS_FREERTOS
/* FreeRTOS implementation */
#include "FreeRTOS.h"
#include "semphr.h"
#include "device_status.h"

#if configSUPPORT_STATIC_ALLOCATION == 1

    typedef struct {
        SemaphoreHandle_t handle;
        StaticSemaphore_t buffer;
    } semaphore_t;
    typedef semaphore_t mutex_t;
 
#else /* configSUPPORT_STATIC_ALLOCATION == 0, it's dynamic allocation */

    typedef SemaphoreHandle_t mutex_t;
    typedef SemaphoreHandle_t semaphore_t;

#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */
#else

typedef uint8_t mutex_t;
typedef volatile uint8_t semaphore_t;

#endif /* ifdef USING_OS_FREERTOS */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Delays execution for a number of milliseconds.
 *
 * @param[in] delay: Time delay in milliseconds
 * @return none
 */
void OSIF_TimeDelay(const uint32_t delay);

/*!
 * @brief Returns the number of miliseconds elapsed since starting the systick timer.
 *
 * @param[in] none
 * @return the number of miliseconds elapsed
 */
uint32_t OSIF_GetMilliseconds(void);

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
status_t OSIF_MutexLock(const mutex_t * const pMutex,const uint32_t timeout);

/*!
 * @brief Unlocks a previously locked mutex.
 *
 * @param[in] pMutex: reference to the mutex object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: mutex unlock operation success
 *          - STATUS_ERROR: mutex unlock failed
 */
status_t OSIF_MutexUnlock(const mutex_t * const pMutex);

/*!
 * @brief Create an unlocked mutex.
 *
 * @param[in] pMutex: reference to the mutex object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: mutex created
 *          - STATUS_ERROR: mutex could not be created
 */
status_t OSIF_MutexCreate(mutex_t * const pMutex);

/*!
 * @brief Destroys a previously created mutex.
 *
 * @param[in] pMutex: reference to the mutex object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: mutex destroyed
 */
status_t OSIF_MutexDestroy(const mutex_t * const pMutex);

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
                       const uint32_t timeout);

/*!
 * @brief Increment a semaphore.
 *
 * @param[in] pSem: reference to the semaphore object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: semaphore post operation success
 *          - STATUS_ERROR: semaphore could not be incremented
 */
status_t OSIF_SemaPost(semaphore_t * const pSem);

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
                         const uint8_t initValue);

/*!
 * @brief Destroys a previously created semaphore.
 *
 * @param[in] pSem: reference to the semaphore object
 * @return  One of the possible status codes
 *          - STATUS_SUCCESS: semaphore destroyed
 */
status_t OSIF_SemaDestroy(const semaphore_t * const pSem);

#if defined (__cplusplus)
}
#endif /* __cplusplus */

#endif /* _OSIF_H */

/* =============================================  EOF  ============================================== */
