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
 * @file spi_hw.h
 *
 * @brief This file provides spi hardware integration functions.
 *
 */

#ifndef _SPI_HW_H
#define _SPI_HW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "spi_shared_function.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */
/*!
 * @brief SPI status flags.
 */
typedef enum
{
    SPI_TX_DATA_FLAG      = SPI_STATUS_TXEF_Pos,   /*!< TX data not full flag */
    SPI_RX_DATA_FLAG      = SPI_STATUS_RXFF_Pos,   /*!< RX data not empty flag */
    SPI_TRANSMIT_ERROR    = SPI_STATUS_TXUF_Pos,   /*!< Transmit Error flag (TX underrun) */
    SPI_RECEIVE_ERROR     = SPI_STATUS_RXOF_Pos,   /*!< Receive Error flag (RX overrun) */
    SPI_MODE_FAULT_ERROR  = SPI_STATUS_MODEF_Pos,  /*!< Master mode fault error flag */
    SPI_DATA_MATCH        = SPI_STATUS_RDMF_Pos,   /*!< Data match flag */
    SPI_MASTER_BUSY       = SPI_STATUS_MEBY_Pos,   /*!< SPI engine busy flag */
    SPI_MODULE_IDLE       = SPI_STATUS_IDLEF_Pos,  /*!< Module idle flag */
    SPI_ALL_STATUS        = 0x00000026U            /*!< Used for clearing all w1c status flags */
} spi_status_flag_t;

/*!
 * @brief SPI interrupt source.
 */
typedef enum
{
    SPI_INT_TX_DATA           = SPI_CFG1_TXEIE_Pos,   /*!< TX data not full interrupt */
    SPI_INT_RX_DATA           = SPI_CFG1_RXFIE_Pos,   /*!< RX data not empty interrupt */
    SPI_INT_TRANSMIT_ERROR    = SPI_CFG1_TXUIE_Pos,   /*!< Transmit Error interrupt (TX underrun) */
    SPI_INT_RECEIVE_ERROR     = SPI_CFG1_RXOIE_Pos,   /*!< Receive Error interrupt (RX overrun) */
    SPI_INT_MODE_FAULT_ERROR  = SPI_CFG1_MODFIE_Pos,  /*!< Master mode fault error interrupt */
    SPI_INT_DATA_MATCH        = SPI_CFG1_DMIE_Pos,    /*!< Data match interrupt */
} spi_interrupt_t;

/*!
 * @brief SPI master or slave configuration.
 */
typedef enum
{
    SPI_SLAVE  = 0U,     /*!< SPI peripheral operates in slave mode. */
    SPI_MASTER = 1U      /*!< SPI peripheral operates in master mode. */
} spi_master_slave_mode_t;

/*!
 * @brief SPI pin (SOUT and SIN) configuration.
 */
typedef enum
{
    SPI_SOUT_MOSI_SIN_MISO = 0U,  /*!< SPI SOUT pin set as MOSI, SIN pin set as MISO */
    SPI_SOUT_MISO_SIN_MOSI        /*!< SPI SOUT pin set as MISO, SIN pin set as MOSI */
} spi_pin_config_t;

/*!
 * @brief SPI transfer data cwidth configuration.
 */
typedef enum
{
    SPI_DATA_WIDTH_1BIT = 0U,  /*!< SPI data line 1 bit, data out on SOUT, in on SIN (standby SPI) */
    SPI_DATA_WIDTH_2BIT,       /*!< SPI data line 2 bits, data out/in on SOUT, SIN */
    SPI_DATA_WIDTH_4BIT        /*!< SPI data line 4 bits, out on SDO/SDI/PCS[3:2] and in on SDO/SDI/PCS[3:2] */
} spi_transfer_width_t;

/*!
 * @brief SPI delay type selection.
 */
typedef enum
{
    SPI_SCK_TO_PCS = 0U,          /*!< CS_SETUP: from the last SCK edge to the CS negation */
    SPI_PCS_TO_SCK = 1U,          /*!< CS_HOLD: from the CS assertion to the first SCK edge */
    SPI_BETWEEN_TRANSFER = 2U     /*!< CS_IDLE: from the CS assertion to the next CS negation */
} spi_delay_type_t;

/*!
 * @brief SPI hreq polarity type selection.
 */
typedef enum
{
    SPI_HREQ_POLARITY_LOW = 0U,   /*!< SPI host request active polarity is low */
    SPI_HREQ_POLARITY_HIGH        /*!< SPI host request active polarity is high */
}SPI_hreq_polarity_t;

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief Clears the SPI status flag, which can write "1" to clear.
 *
 * @param[in] base: SPI base pointer
 * @param[in] statusFlag: Select which status will be cleared
 * @return STATUS_SUCCESS or STATUS_ERROR
 */
status_t SPI_ClearStatusFlag(SPI_Type *base, spi_status_flag_t statusFlag);

/*!
 * @brief SPI Module softreset. And it only reset master engine/buffer/flag logic,
 *        slave buffer/flag logic, the configuration of control registers(CFG0/
 *        CFG1/CFG2/CMD) will not be reset.
 *
 * @param[in] base: SPI base pointer
 * @return none
 */
void SPI_SoftwareReset(SPI_Type *base);

/*!
 * @brief Set CS_HOLD, CS_SETUP or CS_IDLE time.
 *
 * @param[in] base: SPI base pointer.
 * @param[in] whichDelay: Select which CS time
 * @param[in] delay: The 8-bit delay value 0x00 to 0xFF (255).
 * @return none
 */
void SPI_SetDelay(SPI_Type *base, spi_delay_type_t whichDelay, uint8_t delay);

/*!
 * @brief Configures the SPI PCS polarity.
 *
 * @param[in] base: SPI base pointer
 * @param[in] whichPcs: Select which PCS to configured
 *                     -SPI_PCS0
 *                     -SPI_PCS1
 *                     -SPI_PCS2
 *                     -SPI_PCS3
 * @param[in] pcsPolarity: Set PCS as active high or low
 *                     -SPI_ACTIVE_LOW
 *                     -SPI_ACTIVE_HIGH
 * @return none
 */
void SPI_SetPcsPolarityMode(SPI_Type *base, spi_which_pcs_t whichPcs, spi_signal_polarity_t pcsPolarity);

/*!
 * @brief Configure the SPI baud rate in bits per second.
 *
 * @param[in] base: SPI base pointer
 * @param[in] bitsPerSec: The desired baud rate in bits per second
 * @param[in] sourceClockInHz: SPI functional source input clock in Hertz
 * @return  The actual calculated baud rate. This function may also return a "0" if the
 *          bitPerSec is greater than sourceClockInHz/2
 */
uint32_t SPI_SetBaudRate(SPI_Type *base, uint32_t bitsPerSec, uint32_t sourceClockInHz);

/*!
 * @brief Set SPI frame size(4~32 bits per frame).
 *
 * @param[in] base: SPI base pointer
 * @param[in] frameSize: SPI frame size, support 4~32 bits
 *                      -4: 4 bits
 *                      -5: 5 bits
 *                      -...
 *                      -32: 32 bits
 * @return none
 */
void SPI_SetFrameSize(SPI_Type *base, uint32_t frameSize);

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Enable/Disable SPI module.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable/disable SPI module
 * @return none
 */
static inline void SPI_SetEnable(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CMD, SPI_CMD_SPIEN_Msk, SPI_CMD_SPIEN_Pos, enable);
}

/*!
 * @brief Check if SPI module is enabled.
 *
 * @param[in] base: SPI base pointer
 * @return SPI module is enabled or not
 *         -true: SPI module is enabled
 *         -false: SPI module is disabled
 */
static inline bool SPI_IsModuleEnabled(const SPI_Type *base)
{
    return (bool)(((base->CMD) & SPI_CMD_SPIEN_Msk) >> SPI_CMD_SPIEN_Pos);
}

/*!
 * @brief Set SPI master or slave mode.
 *
 * @param[in] base: SPI base pointer
 * @param[in] mode: set SPI master mode
 *                  -SPI_MASTER: SPI master mode
 *                  -SPI_SLAVE: SPI slave mode
 * @return none
 */
static inline void SPI_SetMasterSlaveMode(SPI_Type *base, spi_master_slave_mode_t mode)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_MSTR_Msk, SPI_CFG1_MSTR_Pos, mode);
}

/*!
 * @brief Returns whether the SPI module is in master mode.
 *
 * @param[in] base：SPI base pointer
 * @return Returns the status of SPI master or slave mode
 *         -true: SPI work in master mode
 *         -false: SPI work in salve mode
 */
static inline bool SPI_IsMaster(const SPI_Type *base)
{
    return (bool)(((base->CFG1) & SPI_CFG1_MSTR_Msk) >> SPI_CFG1_MSTR_Pos);
}

/*!
 * @brief Returns whether the SPI module is in Cs Continuous mode.
 *
 * @param[in] base：SPI base pointer
 * @return Returns the status of SPI Continuous or DisContinuous mode
 *         -true: SPI work in Continuous mode
 *         -false: SPI work in DisContinuous mode
 */
static inline bool SPI_IsCsContinuousMode(const SPI_Type *base)
{
    return (bool)(((base)->CFG1 & SPI_CFG1_CSOE_Msk) && ((base)->CFG1 & SPI_CFG1_CONT_CS_Msk));
}

/*!
 * @brief Gets the SPI status flag enable.
 *
 * @param[in] base: SPI base pointer
 * @param[in] statusFlag: Select which status will be return status
 * @return State of the status flag
 *         -true: the status flag is "1"
 *         -false: the status flag is "0"
 */
static inline bool SPI_GetStatusFlag(const SPI_Type *base, spi_status_flag_t statusFlag)
{
    return (bool)(((base->STATUS) >> (uint8_t)statusFlag) & 1U);
}

/*!
 * @brief Configures the SPI interrupts.
 *
 * @param[in] base: SPI base pointer
 * @param[in] interruptSrc: The interrupt source, which will be enabled
 * @param[in] enable Enable or disable the interrupt source
 * @return none
 */
static inline void SPI_SetIntMode(SPI_Type *base, spi_interrupt_t interruptSrc, bool enable)
{
    if (enable)
    {
        base->CFG1 |= (uint32_t)1U << (uint8_t)interruptSrc;
    }
    else
    {
        base->CFG1 &= ~((uint32_t)1U << (uint8_t)interruptSrc);
    }
}

/*!
 * @brief Returns if the SPI interrupt request is enabled or disabled.
 *
 * @param[in] base: SPI base pointer
 * @param[in] interruptSrc: The interrupt source, which will be enabled
 * @return Return the interrupt source enable status
 *         -true: the interrupt source is enabled
 *         -false: the interrupt source is disabled
 */
static inline bool SPI_GetIntMode(const SPI_Type *base, spi_interrupt_t interruptSrc)
{
    return (bool)(((base->CFG1) >> (uint8_t)interruptSrc) & 1U);
}

/*!
 * @brief Enable/disable the SPI Transmit Data DMA request.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or disable the TX DMA request
 * @return none
 */
static inline void SPI_SetTxDmaCmd(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_DMATXEN_Msk, SPI_CFG1_DMATXEN_Pos, enable);
}

/*!
 * @brief Enable/disable the SPI Receive Data DMA request.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or disable the RX DMA request
 * @return none
 */
static inline void SPI_SetRxDmaCmd(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_DMARXEN_Msk, SPI_CFG1_DMARXEN_Pos, enable);
}

/*!
 * @brief Configures the SPI SOUT/SIN pin configuration mode.
 *
 * @param[in] base: SPI base pointer
 * @param[in] pinCfg: Select configuration for the SOUT/SIN pins (see spi_pin_config_t)
 * @return none
 */
static inline void SPI_SetPinConfigMode(SPI_Type *base, spi_pin_config_t pinCfg)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_PINCFG_Msk, SPI_CFG1_PINCFG_Pos, pinCfg);
}

/*!
 * @brief Set SPI CPHA.
 *
 * @param[in] base: SPI base pointer
 * @param[in] clkPhase: SPI clock CPHA
 *                      -SPI_CLOCK_PHASE_1ST_EDGE
 *                      -SPI_CLOCK_PHASE_2ND_EDGE
 * @return none
 */
static inline void SPI_SetCPHA(SPI_Type *base, spi_clock_phase_t clkPhase)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_CPHA_Msk, SPI_CFG1_CPHA_Pos, clkPhase);
}

/*!
 * @brief Set SPI CPOL.
 *
 * @param[in] base: SPI base pointer
 * @param[in] clkPolarity: SPI cpol select
 *                         - SPI_SCK_ACTIVE_HIGH
 *                         - SPI_SCK_ACTIVE_LOW
 * @return none
 */
static inline void SPI_SetCPOL(SPI_Type *base, spi_sck_polarity_t clkPolarity)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_CPOL_Msk, SPI_CFG1_CPOL_Pos, clkPolarity);
}

/*!
 * @brief Set SPI RX MSB first.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or isable SPI RX MSB first
 *                  -true: RX MSB
 *                  -false: RX LSB
 * @return none
 */
static inline void SPI_SetRxMSB(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_RMSBF_Msk, SPI_CFG1_RMSBF_Pos, enable);
}

/*!
 * @brief Set SPI TX MSB first.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or disable SPI TX MSB first
 *                  -true: TX MSB
 *                  -false: TX LSB
 * @return none
 */
static inline void SPI_SetTxMSB(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_MSBF_Msk, SPI_CFG1_MSBF_Pos, enable);
}

/*!
 * @brief Get SPI frame size.
 *
 * @param[in] base: SPI base pointer
 * @return The frame size, the value will be 4~32, corresponding to 4~32 bits
 */
static inline uint32_t SPI_GetFrameSize(SPI_Type *base)
{
    return (((base->CFG1 & SPI_CFG1_FRMSIZE_Msk) >> SPI_CFG1_FRMSIZE_Pos) + 1U);
}

/*!
 * @brief Set SPI CS continuous or disContinuous mode.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or diaable SPI CS output continuous mode
 *                    -true: CS continuous mode
 *                    -false: CS disContinuous mode
 * @return none
 */
static inline void SPI_SetContinuousCS(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_CONT_CS_Msk, SPI_CFG1_CONT_CS_Pos, enable);
}

/*!
 * @brief Set SPI CS output enable
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or diaable SPI CS hardware output
 *                    -true: enable the CS hardware output(CS control by hardware)
 *                    -false: disable the CS hardware output(CS control by software)
 * @return none
 */
static inline void SPI_SetCSOE(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_CSOE_Msk, SPI_CFG1_CSOE_Pos, enable);
}

/*!
 * @brief Enable/Disable SPI CS mode fault detect function.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or disable SPI CS mode fault detect function
 *                    -true: enable the master mode fault detect function
 *                    -false: disable the master mode fault detect function
 * @return none
 */
static inline void SPI_SetModeFault(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG1, SPI_CFG1_MODFEN_Msk, SPI_CFG1_MODFEN_Pos, enable);
}

/*!
 * @brief Release SPI CS, which will be changed to inactive enable.
 *
 * @param[in] base: SPI base pointer
 * @return none
 */
static inline void SPI_ReleaseCS(SPI_Type *base)
{
    SET_BIT32(base->CMD, SPI_CMD_CSRLS_Msk);
}

/*!
 * @brief Set SPI match data.
 *
 * @param[in] base: SPI base pointer
 * @return none
 */
static inline void SPI_SetMatchData(SPI_Type *base, uint32_t data)
{
    WRITE_REG32(base->DMV, data);
}

/*!
 * @brief Sets the PCS flag to the value of the whichPcs parameter.
 *
 * @param[in] base: SPI base pointer
 * @param[in] whichPcs: Select which CS is assert
 *                      -SPI_PCS0
 *                      -SPI_PCS1
 *                      -SPI_PCS2
 *                      -SPI_PCS3
 * @return none
 */
static inline void SPI_SetPcs(SPI_Type *base, spi_which_pcs_t whichPcs)
{
    MODIFY_REG32(base->CFG2, SPI_CFG2_PCSCFG_Msk, SPI_CFG2_PCSCFG_Pos, whichPcs);
}

/*!
 * @brief Set SPI transfer data width.
 *
 * @param[in] base: SPI base pointer
 * @param[in] dataWidth: transfer data width
 *                       - SPI_DATA_WIDTH_1BIT
 *                       - SPI_DATA_WIDTH_2BIT
 *                       - SPI_DATA_WIDTH_4BIT
 * @return none
 */
static inline void SPI_SetDataWidth(SPI_Type *base, spi_transfer_width_t dataWidth)
{
    MODIFY_REG32(base->CFG2, SPI_CFG2_WIDTH_Msk, SPI_CFG2_WIDTH_Pos, dataWidth);
}

/*!
 * @brief Writes data into the TX data register.
 *
 * @param[in] base: SPI base pointer
 * @param[in] data: The data word to be sent
 * @return none
 */
static inline void SPI_WriteData(SPI_Type *base, uint32_t data)
{
    base->DATA = data;
}

/*!
 * @brief Reads data from the data register.
 *
 * @param[in] base: SPI base pointer
 * @return The RX data read from the data register
 */
static inline uint32_t SPI_ReadData(const SPI_Type *base)
{
    return (uint32_t)base->DATA;
}

/*!
 * @brief  Config Master no overflow mode.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or disable SPI no over flow mode
 * @return none
 */
static inline void SPI_SetMasterNoOverflowMode(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG2, SPI_CFG2_MNOV_Msk, SPI_CFG2_MNOV_Pos, enable);
}

/*!
 * @brief Enable/Disable TX Only mode.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: enable/disable SPI TX only mode
 * @return none
 */
static inline void SPI_SetTxOnly(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG2, SPI_CFG2_TOEN_Msk, SPI_CFG2_TOEN_Pos, enable);
}

/*!
 * @brief Get TX Only mode.
 *
 * @param[in] base: SPI base pointer
 * @return SPI TX only mode is enabled/disabled
 */
static inline bool SPI_GetTxOnly(SPI_Type *base)
{
    return (bool)(((base->CFG2) >> (uint8_t)SPI_CFG2_TOEN_Pos) & 1U);
}

/*!
 * @brief Enable/Disable RX Only mode.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: enable/disable SPI RX only mode
 * @return none
 */
static inline void SPI_SetRxOnly(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG2, SPI_CFG2_ROEN_Msk, SPI_CFG2_ROEN_Pos, enable);
}

/*!
 * @brief Get RX Only mode.
 *
 * @param[in] base: SPI base pointer
 * @return SPI RX only mode is enabled/disabled
 */
static inline bool SPI_GetRxOnly(SPI_Type *base)
{
    return (bool)(((base->CFG2) >> (uint8_t)SPI_CFG2_ROEN_Pos) & 1U);
}

/*!
 * @brief Start Master Receive With RX Only Mode.
 *
 * @param[in] base: SPI base pointer
 * @return none
 */
static inline void SPI_RxOnlyModeTrig(SPI_Type *base)
{
    SET_BIT32(base->CMD, SPI_CMD_ROTRIG_Msk);
}

/*!
 * @brief Set SPI host request polarity.
 *
 * @param[in] base: SPI base pointer
 * @param[in] hreqPolarity: host request polarity
 *                  -SPI_HREQ_POLARITY_LOW
 *                  -SPI_HREQ_POLARITY_HIGH
 * @return none
 */
static inline void SPI_SetHreqPolarity(SPI_Type *base, SPI_hreq_polarity_t hreqPolarity)
{
    MODIFY_REG32(base->CFG2, SPI_CFG2_HRPOL_Msk, SPI_CFG2_HRPOL_Pos, hreqPolarity);
}

/*!
 * @brief Enable or disable SPI host request.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or disable SPI host request
 * @return none
 */
static inline void SPI_SetHreq(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG2, SPI_CFG2_HREN_Msk, SPI_CFG2_HREN_Pos, enable);
}

/*!
 * @brief Enable or disable SPI in debug mode.
 *
 * @param[in] base: SPI base pointer
 * @param[in] enable: Enable or disable SPI in debug mode
 * @return none
 */
static inline void SPI_SetDebug(SPI_Type *base, bool enable)
{
    MODIFY_REG32(base->CFG2, SPI_CFG2_DBGEN_Msk, SPI_CFG2_DBGEN_Pos, enable);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SPI_HW_H */

/* =============================================  EOF  ============================================== */
