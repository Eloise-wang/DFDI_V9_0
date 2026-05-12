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
 * @file adc_hw.c
 *
 * @brief This file provides analog to digital converter module related ANA registers access functions.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "adc_hw.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* =====================================  Functions definition  ===================================== */
/*!
 * @brief Get ADC ge oe voltage input enable field register information in ANA register.
 *
 * @param[in] instance: ADC instanc number
 * @param[in] reg: register address
 * @param[in] msk: field mask
 * @param[in] pos: field offset position
 * @return none
 */
static inline void ADC_GetGEOEVINEnablePos(const uint32_t instance, uint32_t *reg, uint32_t *msk, uint32_t *pos)
{
    switch (instance)
    {
    case 0U:
        *reg = (uint32_t)&ANA->AUXADC_CFG1;
        *msk = ANA_AUXADC_CFG1_RG_ADC0_GEOECAL_VIN_EN_Msk;
        *pos = ANA_AUXADC_CFG1_RG_ADC0_GEOECAL_VIN_EN_Pos;
        break;
    case 1U:
        *reg = (uint32_t)&ANA->AUXADC_CFG5;
        *msk = ANA_AUXADC_CFG5_RG_ADC1_GEOECAL_VIN_EN_Msk;
        *pos = ANA_AUXADC_CFG5_RG_ADC1_GEOECAL_VIN_EN_Pos;
        break;
    default:
        break;
    }
}

/*!
 * @brief Get ADC ge oe voltage input config field register information in ANA register.
 *
 * @param[in] instance: ADC instanc number
 * @param[in] reg: register address
 * @param[in] msk: field mask
 * @param[in] pos: field offset position
 * @return none
 */
static inline void ADC_GetGEOEVINPos(const uint32_t instance, uint32_t *reg, uint32_t *msk, uint32_t *pos)
{
    switch (instance)
    {
    case 0U:
        *reg = (uint32_t)&ANA->AUXADC_CFG1;
        *msk = ANA_AUXADC_CFG1_RG_ADC0_GEOECAL_VIN_SEL_Msk;
        *pos = ANA_AUXADC_CFG1_RG_ADC0_GEOECAL_VIN_SEL_Pos;
        break;
    case 1U:
        *reg = (uint32_t)&ANA->AUXADC_CFG5;
        *msk = ANA_AUXADC_CFG5_RG_ADC1_GEOECAL_VIN_SEL_Msk;
        *pos = ANA_AUXADC_CFG5_RG_ADC1_GEOECAL_VIN_SEL_Pos;
        break;
    default:
        break;
    }
}

/*!
 * @brief Set GE OE calibration voltage input signal enable flag.
 *
 * @param[in] instance: adc instance number
 * @param[in] state: enable or disable calibration voltage input
 * @return none
 */
void ADC_SetGEOEVINEnableFlag(const uint32_t instance, const bool state)
{
    uint32_t reg = 0U, mask = 0U, pos = 0U;

    ADC_GetGEOEVINEnablePos(instance, &reg, &mask, &pos);

    MODIFY_REG32(*(__IOM uint32_t *)reg, mask, pos, state ? (uint32_t)1U : (uint32_t)0U);
}

/*!
 * @brief Get GE OE calibration voltage input signal enable flag.
 *
 * @param[in] instance: adc instance number
 * @return whether calibration voltage input is enabled
 */
bool ADC_GetGEOEVINEnableFlag(const uint32_t instance)
{
    uint32_t reg = 0U, mask = 0U, pos = 0U, tmp = 0U;

    ADC_GetGEOEVINEnablePos(instance, &reg, &mask, &pos);

    tmp = READ_BIT32(*(__IOM uint32_t *)reg, mask) >> pos;

    return (tmp != 0U);
}

/*!
 * @brief Set GE OE calibration input signal.
 *
 * @param[in] instance: adc instance number
 * @param[in] state: calibration input signal
 * @return none
 */
void ADC_SetGEOEVIN(const uint32_t instance, const ana_geoecal_vin_t vin)
{
    uint32_t reg = 0U, mask = 0U, pos = 0U;

    ADC_GetGEOEVINPos(instance, &reg, &mask, &pos);

    MODIFY_REG32(*(__IOM uint32_t *)reg, mask, pos, vin);
}

/*!
 * @brief Get GE OE calibration input signal.
 *
 * @param[in] instance: adc instance number
 * @return calibration input signal
 */
ana_geoecal_vin_t ADC_GetGEOEVIN(const uint32_t instance)
{
    uint32_t reg = 0U, mask = 0U, pos = 0U;

    ADC_GetGEOEVINPos(instance, &reg, &mask, &pos);

    return (ana_geoecal_vin_t)(READ_BIT32(*(__IOM uint32_t *)reg, mask) >> pos);
}

/* =============================================  EOF  ============================================== */
