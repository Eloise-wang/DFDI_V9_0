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
 * @file debugout_ac7840x.c
 *
 * @brief This file provides debug information output integration functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "gpio_drv.h"
#include "ckgen_drv.h"
#include "debugout_ac7840x.h"
#include "../src/uart/uart_hw.h"
#if defined (__ICCARM__)
#include <LowLevelIOInterface.h>

#pragma module_name = "?__write"
#endif
/* ============================================  Define  ============================================ */
#define DEBUG_INSTANCE           1UL
#define DEBUG_UART               UART1              /* define uart1 for debug output  */
#define DEBUG_UART_IRQ           UART1_IRQn         /* define uart1 interrupt vector  */
#define DEBUG_UART_CLK           CLK_UART1          /* define uart1 ckgen clock value */
#define DEBUG_UART_SRST          SRST_UART1         /* define uart1 ckgen reset value */
#define DEBUG_UART_BAUDRATE      115200UL
#if 0
#define DEBUG_UART_TX_PIN        PORTB, 22UL        /* define uart1 tx gpio */
#define DEBUG_UART_RX_PIN        PORTB, 23UL        /* define uart1 rx gpio */
#define DEBUG_UART_TX_FUNC       PORT_MUX_ALT5      /* define uart1 tx pinmux */
#define DEBUG_UART_RX_FUNC       PORT_MUX_ALT3      /* define uart1 rx pinmux */
#else
#define DEBUG_UART_TX_PIN        PORTC, 9UL         /* define uart1 tx gpio */
#define DEBUG_UART_RX_PIN        PORTC, 8UL         /* define uart1 rx gpio */
#define DEBUG_UART_TX_FUNC       PORT_MUX_ALT2      /* define uart1 tx pinmux */
#define DEBUG_UART_RX_FUNC       PORT_MUX_ALT2      /* define uart1 rx pinmux */
#endif

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* debug init flag */
static uint8_t s_debugInit = 0U;

#if defined (__clang__)

#else
/* define std FILE struct */
struct __FILE
{
    int handle;
};
#endif
/* refine stdout,stdin,stderr */
FILE __stdout;
FILE __stdin;
FILE __stderr;

/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */
/*!
 * @brief Refine _sys_exit.
 *
 * @param[in] x: no use
 * @return always 0
 */
int _sys_exit(int x)
{
    x = x;
    (void)x;

    return 0;
}

/*!
 * @brief Refine _ttywrch.
 *
 * @param[in] x: no use
 * @return always 0
 */
int _ttywrch(int x)
{
    x = x;
    (void)x;

    return 0;
}

#ifdef ITM_DEBUG_OUT
/*!
 * @brief Put a char to ITM.
 *
 * @param[in] ch: the char to put
 * @param[in] f:  file pointer for the std input
 * @return return the char of be put
 */
int fputc(int ch, FILE *f)
{
    if (s_debugInit != 0U)
    {
        ITM_SendChar(ch);
    }

    return ch;
}

/* for ITM use */
volatile int ITM_RxBuffer;

/*!
 * @brief Get a char from ITM.
 *
 * @param[in] f: file pointer for the std input
 * @return -1: not get char, other: the char get from ITM
 */
int fgetc(FILE *f)
{
    int ch = -1;

    if (s_debugInit != 0U)
    {
        ch = ITM_ReceiveChar();
    }

    return ch;
}
#else
/*!
 * @brief Put a char to UART.
 *
 * @param[in] ch: the char to put
 * @param[in] f:  file pointer for the std input
 * @return return the char of be put
 */
int fputc(int ch, FILE *f)
{
    if (s_debugInit != 0U)
    {
        while (!UART_GetStatusFlag(DEBUG_UART, UART_TX_DATA_NOT_FULL));
        UART_Putchar(DEBUG_UART, (uint8_t)ch);
    }

    return ch;
}

/*!
 * @brief Get a char from UART.
 *
 * @param[in] f: file pointer for the std input
 * @return -1: not get char, other: the char get from UART
 */
int fgetc(FILE *f)
{
    int ch = -1;

    if (s_debugInit != 0U)
    {
        if (UART_GetStatusFlag(DEBUG_UART, UART_RX_DATA_READY))
        {
            UART_Getchar(DEBUG_UART, (uint8_t *)&ch);
            ch &= 0xFF;
        }
    }

    return ch;
}
#endif /* ITM_DEBUG_OUT */

/*!
 * @brief Set the debug out is invalid.
 *
 * @param[in] none
 * @return none
 */
void DeinitDebug(void)
{
    s_debugInit = 0U;
}

/*!
 * @brief Init debug out, and set the debug out is valid.
 *
 * @param[in] none
 * @return none
 */
void InitDebug(void)
{
#ifndef ITM_DEBUG_OUT
#ifdef DEBUG_CMD_INTERRUPT
    NVIC_SetPriority(DEBUG_UART_IRQ, 3U);
    NVIC_ClearPendingIRQ(DEBUG_UART_IRQ);
    NVIC_EnableIRQ(DEBUG_UART_IRQ);
#endif /* DEBUG_CMD_INTERRUPT */

    /* Set pinmux */
    GPIO_DRV_SetMuxModeSel(DEBUG_UART_TX_PIN, DEBUG_UART_TX_FUNC);
    GPIO_DRV_SetMuxModeSel(DEBUG_UART_RX_PIN, DEBUG_UART_RX_FUNC);

    /* Check UART function clock */
    if (0U == (CKGEN->PERI_CLK_MUX2 & \
        (CKGEN_PERI_CLK_MUX2_UART0_MUX_Msk << (DEBUG_INSTANCE * 4U))))
    {
        /* Default set clock to HSIDIV2 */
        CKGEN->PERI_CLK_MUX2 |= 5U << (DEBUG_INSTANCE * 4U);
    }
    /* Enable UART bus clock and release UART module from reset status */
    CKGEN_DRV_Enable(DEBUG_UART_CLK, true);
    CKGEN_DRV_SoftReset(DEBUG_UART_SRST, true);

    /* Set the baudrate */
    (void)UART_DRV_SetBaudRate(DEBUG_INSTANCE, DEBUG_UART_BAUDRATE);

    UART_SetBitCountPerChar(DEBUG_UART, UART_8_BITS_PER_CHAR);
    UART_SetStopBitCount(DEBUG_UART, UART_ONE_STOP_BIT);
    UART_SetParityMode(DEBUG_UART, UART_PARITY_DISABLED);
    UART_SetFIFO(DEBUG_UART, true);

    /* Enable UART TX and RX */
    UART_SetTransmitterCmd(DEBUG_UART, true);
    UART_SetReceiverCmd(DEBUG_UART, true);
#endif /* ITM_DEBUG_OUT */
    s_debugInit = 1U;
}

#if defined (__ICCARM__)
/*!
 * @brief Put a char to UART.
 *
 * @param[in] x: the char to put
 * @return return the char of be put
 */
int MyLowLevelPutchar(int x)
{
    if (s_debugInit)
    {
        while (!UART_GetStatusFlag(DEBUG_UART, UART_TX_DATA_NOT_FULL));
        UART_Putchar(DEBUG_UART, (uint8_t)x);
    }

    return x;
}

/*!
 * @brief If the __write implementation uses internal buffering, uncomment
 * the following line to ensure that we are called with "buffer" as 0
 * (i.e. flush) when the application terminates.
 *
 * @param[in] handle: the char to put
 * @param[in] buffer: file pointer for the std input
 * @param[in] size:   file pointer for the std input
 * @return return the char of be put
 */
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  /* Remove the #if #endif pair to enable the implementation */
#if 1

  size_t nChars = 0;

  if (buffer == 0)
  {
    /*
     * This means that we should flush internal buffers.  Since we
     * don't we just return.  (Remember, "handle" == -1 means that all
     * handles should be flushed.)
     */
    return 0;
  }

  /* This template only writes to "standard out" and "standard err",
   * for all other file handles it returns failure. */
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size != 0; --size)
  {
    if (MyLowLevelPutchar(*buffer++) < 0)
    {
      return _LLIO_ERROR;
    }

    ++nChars;
  }

  return nChars;

#else

  /* Always return error code when implementation is disabled. */
  return _LLIO_ERROR;
#endif
}

#endif //(__ICCARM__)
/* =============================================  EOF  ============================================== */
