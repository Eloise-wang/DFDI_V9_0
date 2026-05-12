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
 * @file device_status.h
 *
 * @brief This file provides all device status enum.
 *
 */ 
 
#ifndef _DEVICE_STATUS_H
#define _DEVICE_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    /* Generic error codes */
    STATUS_SUCCESS                         = 0x000U,    /*!< Generic operation success status */
    STATUS_ERROR                           = 0x001U,    /*!< Generic operation failure status */
    STATUS_BUSY                            = 0x002U,    /*!< Generic operation busy status */
    STATUS_TIMEOUT                         = 0x003U,    /*!< Generic operation timeout status */
    STATUS_UNSUPPORTED                     = 0x004U,    /*!< Generic operation unsupported status */
    /* MCU specific error codes */
    STATUS_MCU_GATED_OFF                   = 0x100U,    /*!< Module is gated off */
    STATUS_MCU_TRANSITION_FAILED           = 0x101U,    /*!< Error occurs during transition. */
    STATUS_MCU_INVALID_STATE               = 0x102U,    /*!< Unsupported in current state. */
    STATUS_MCU_NOTIFY_BEFORE_ERROR         = 0x103U,    /*!< Error occurs during send "BEFORE" notification. */
    STATUS_MCU_NOTIFY_AFTER_ERROR          = 0x104U,    /*!< Error occurs during send "AFTER" notification. */
    /* I2C specific error codes */
    STATUS_I2C_RECEIVED_NACK               = 0x200U,    /*!< NACK signal received  */
    STATUS_I2C_TX_UNDERRUN                 = 0x201U,    /*!< TX underrun error */
    STATUS_I2C_RX_OVERRUN                  = 0x202U,    /*!< RX overrun error */
    STATUS_I2C_ARBITRATION_LOST            = 0x203U,    /*!< Arbitration lost */
    STATUS_I2C_ABORTED                     = 0x204U,    /*!< A transfer was aborted */
    STATUS_I2C_BUS_BUSY                    = 0x205U,    /*!< I2C bus is busy, cannot start transfer */
    /* Security specific error codes */
    STATUS_SEC_SEQUENCE_ERROR              = 0x402U,    /*!< The sequence of commands or subcommands is out of
                                                             sequence */
    STATUS_SEC_KEY_NOT_AVAILABLE           = 0x403U,    /*!< A key is locked due to failed boot measurement or
                                                             an active debugger */
    STATUS_SEC_KEY_INVALID                 = 0x404U,    /*!< A function is called to perform an operation with
                                                             a key that is not allowed for the given operation */
    STATUS_SEC_KEY_EMPTY                   = 0x405U,    /*!< Attempt to use a key that has not been initialized yet */
    STATUS_SEC_NO_SECURE_BOOT              = 0x406U,    /*!< The conditions for a secure boot process are not met */
    STATUS_SEC_KEY_WRITE_PROTECTED         = 0x407U,    /*!< Request for updating a write protected key slot,
                                                             or activating debugger with write protected key(s) */
    STATUS_SEC_KEY_UPDATE_ERROR            = 0x408U,    /*!< Key update did not succeed due to errors in
                                                             verification of the messages */
    STATUS_SEC_RNG_SEED                    = 0x409U,    /*!< Returned by CMD_RND and CMD_DEBUG if the seed has not
                                                             been initialized before */
    STATUS_SEC_NO_DEBUGGING                = 0x40AU,    /*!< DEBUG command authentication failed */
    STATUS_SEC_MEMORY_FAILURE              = 0x40CU,    /*!< General memory technology failure
                                                            (multibit ECC error, common fault detected) */
    STATUS_SEC_HSM_INTERNAL_MEMORY_ERROR   = 0x410U,    /*!< An internal memory error encountered while
                                                             executing the command */
    STATUS_SEC_INVALID_COMMAND             = 0x411U,    /*!< Command value out of range */
    STATUS_SEC_TRNG_ERROR                  = 0x412U,    /*!< One or more statistical tests run on the TRNG output failed */
    STATUS_SEC_HSM_FLASH_BLOCK_ERROR       = 0x413U,    /*!< Error reading, programming or erasing one of the HSM flash blocks */
    STATUS_SEC_INTERNAL_CMD_ERROR          = 0x414U,    /*!< An internal command processor error while executing a command */
    STATUS_SEC_MAC_LENGTH_ERROR            = 0x415U,    /*!< MAC/Message length out of range */
    STATUS_SEC_INVALID_ARG                 = 0x421U,    /*!< Invalid command argument */
    STATUS_SEC_TRNG_CLOCK_ERROR            = 0x423U,    /*!< TRNG not provided with a stable clock */
    /* SPI specific error codes */
    STATUS_SPI_TX_UNDERRUN                 = 0x500U,    /*!< TX underrun error */
    STATUS_SPI_RX_OVERRUN                  = 0x501U,    /*!< RX overrun error */
    STATUS_SPI_ABORTED                     = 0x502U,    /*!< A transfer was aborted */
    /* UART specific error codes */
    STATUS_UART_TX_UNDERRUN                = 0x600U,    /*!< TX underrun error */
    STATUS_UART_RX_OVERRUN                 = 0x601U,    /*!< RX overrun error */
    STATUS_UART_ABORTED                    = 0x602U,    /*!< A transfer was aborted */
    STATUS_UART_FRAMING_ERROR              = 0x603U,    /*!< Framing error */
    STATUS_UART_PARITY_ERROR               = 0x604U,    /*!< Parity error */
    STATUS_UART_BREAK_ERROR                = 0x605U,    /*!< Break error */
    STATUS_UART_NOISE_ERROR                = 0x606U,    /*!< Noise error */
    /* I2S specific error codes */
    STATUS_I2S_TX_UNDERRUN                 = 0x700U,    /*!< TX underrun error */
    STATUS_I2S_RX_OVERRUN                  = 0x701U,    /*!< RX overrun error */
    STATUS_I2S_ABORTED                     = 0x702U,    /*!< A transfer was aborted */
} status_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEVICE_STATUS_H */

/* =============================================  EOF  ============================================== */
