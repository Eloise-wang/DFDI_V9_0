/**
 * @file gcu_control_dbc.c
 * @brief Auto-generated from gcu_hp_control_05.dbc
 * @note This file was automatically generated. Do not modify manually.
 * @date 2025-01-27
 */

#include "gcu_control_dbc.h"
#include <string.h>

/* ========================================================================
 * Bit Manipulation Helper Functions
 * ======================================================================== */

static inline uint8_t pack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t unpack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) << shift);
}

static inline uint16_t unpack_left_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) << shift);
}

static inline uint8_t unpack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) >> shift);
}

static inline uint16_t unpack_right_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) >> shift);
}

/* ========================================================================
 * Initialization Functions
 * ======================================================================== */

int device_status_init(device_status_t *msg_p)
{
    if (msg_p == NULL) {
        return -1;
    }
    
    memset(msg_p, 0, sizeof(device_status_t));
    
    return 0;
}

int tsmaster_control_init(tsmaster_control_t *msg_p)
{
    if (msg_p == NULL) {
        return -1;
    }
    
    memset(msg_p, 0, sizeof(tsmaster_control_t));
    
    return 0;
}

int tsmaster_control2_init(tsmaster_control2_t *msg_p)
{
    if (msg_p == NULL) {
        return -1;
    }
    
    memset(msg_p, 0, sizeof(tsmaster_control2_t));
    
    return 0;
}

/* ========================================================================
 * device_status Message Pack/Unpack (ID: 0x980FF16C)
 * ======================================================================== */

int device_status_pack(
    uint8_t *dst_p,
    const device_status_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }
    
    if (dst_p == NULL || src_p == NULL) {
        return (-EINVAL);
    }
    
    memset(&dst_p[0], 0, 8);
    
    /* bypass_ratio: bit 0, 6 bits */
    dst_p[0] |= pack_left_shift_u8(src_p->bypass_ratio, 0u, 0x3fu);
    
    /* oil_pressure: bit 6, 8 bits */
    dst_p[0] |= pack_left_shift_u8(src_p->oil_pressure, 6u, 0xc0u);
    dst_p[1] |= pack_right_shift_u8(src_p->oil_pressure, 2u, 0x3fu);
    
    /* oil_temperature: bit 14, 12 bits */
    dst_p[1] |= pack_left_shift_u16(src_p->oil_temperature, 6u, 0xc0u);
    dst_p[2] |= pack_right_shift_u16(src_p->oil_temperature, 2u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->oil_temperature, 10u, 0x03u);
    
    /* LNG_temperature: bit 26, 12 bits */
    dst_p[3] |= pack_left_shift_u16(src_p->LNG_temperature, 2u, 0xfcu);
    dst_p[4] |= pack_right_shift_u16(src_p->LNG_temperature, 6u, 0x3fu);
    
    /* LNG_pressure: bit 38, 10 bits */
    dst_p[4] |= pack_left_shift_u16(src_p->LNG_pressure, 6u, 0xc0u);
    dst_p[5] |= pack_right_shift_u16(src_p->LNG_pressure, 2u, 0xffu);
    
    /* rev_freq: bit 48, 8 bits */
    dst_p[6] |= pack_left_shift_u8(src_p->rev_freq, 0u, 0xffu);
    
    /* rev_enable: bit 56, 1 bit */
    dst_p[7] |= pack_left_shift_u8(src_p->rev_enable, 0u, 0x01u);
    
    /* cooler_enable: bit 57, 1 bit */
    dst_p[7] |= pack_left_shift_u8(src_p->cooler_enable, 1u, 0x02u);
    
    /* debug: bit 58, 1 bit */
    dst_p[7] |= pack_left_shift_u8(src_p->debug, 2u, 0x04u);
    
    /* bit 59-63 (剩余5位) 未使用，填充为0 */
    
    return (8);
}

int device_status_unpack(
    device_status_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }
    
    if (dst_p == NULL || src_p == NULL) {
        return (-EINVAL);
    }
    
    /* bypass_ratio: bit 0, 6 bits */
    dst_p->bypass_ratio = unpack_right_shift_u8(src_p[0], 0u, 0x3fu);
    
    /* oil_pressure: bit 6, 8 bits */
    dst_p->oil_pressure = unpack_right_shift_u8(src_p[0], 6u, 0xc0u);
    dst_p->oil_pressure |= unpack_left_shift_u8(src_p[1], 2u, 0x3fu);
    
    /* oil_temperature: bit 14, 12 bits */
    dst_p->oil_temperature = unpack_right_shift_u16(src_p[1], 6u, 0xc0u);
    dst_p->oil_temperature |= unpack_left_shift_u16(src_p[2], 2u, 0xffu);
    dst_p->oil_temperature |= unpack_left_shift_u16(src_p[3], 10u, 0x03u);
    
    /* LNG_temperature: bit 26, 12 bits */
    dst_p->LNG_temperature = unpack_right_shift_u16(src_p[3], 2u, 0xfcu);
    dst_p->LNG_temperature |= unpack_left_shift_u16(src_p[4], 6u, 0x3fu);
    
    /* LNG_pressure: bit 38, 10 bits */
    dst_p->LNG_pressure = unpack_right_shift_u16(src_p[4], 6u, 0xc0u);
    dst_p->LNG_pressure |= unpack_left_shift_u16(src_p[5], 2u, 0xffu);
    
    /* rev_freq: bit 48, 8 bits */
    dst_p->rev_freq = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
    
    /* rev_enable: bit 56, 1 bit */
    dst_p->rev_enable = unpack_right_shift_u8(src_p[7], 0u, 0x01u);
    
    /* cooler_enable: bit 57, 1 bit */
    dst_p->cooler_enable = unpack_right_shift_u8(src_p[7], 1u, 0x02u);
    
    /* debug: bit 58, 1 bit */
    dst_p->debug = unpack_right_shift_u8(src_p[7], 2u, 0x04u);
    
    /* bit 59-63 (剩余5位) 未使用，忽略 */
    
    return (0);
}

/* ========================================================================
 * tsmaster_control Message Pack/Unpack (ID: 0x98080100)
 * ======================================================================== */

int tsmaster_control_pack(
    uint8_t *dst_p,
    const tsmaster_control_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }
    
    if (dst_p == NULL || src_p == NULL) {
        return (-EINVAL);
    }
    
    memset(&dst_p[0], 0, 8);
    
    /* set_bypass_initial_decline_time: bit 0, 6 bits */
    dst_p[0] |= pack_left_shift_u8(src_p->set_bypass_initial_decline_time, 0u, 0x3fu);
    
    /* set_rev_start_oilP_max: bit 6, 8 bits */
    dst_p[0] |= pack_left_shift_u8(src_p->set_rev_start_oilP_max, 6u, 0xc0u);
    dst_p[1] |= pack_right_shift_u8(src_p->set_rev_start_oilP_max, 2u, 0x3fu);
    
    /* set_rev_start_oilP_min: bit 14, 8 bits */
    dst_p[1] |= pack_left_shift_u8(src_p->set_rev_start_oilP_min, 6u, 0xc0u);
    dst_p[2] |= pack_right_shift_u8(src_p->set_rev_start_oilP_min, 2u, 0x3fu);
    
    /* set_first_fix_freq_time_on: bit 22, 8 bits */
    dst_p[2] |= pack_left_shift_u8(src_p->set_first_fix_freq_time_on, 6u, 0xc0u);
    dst_p[3] |= pack_right_shift_u8(src_p->set_first_fix_freq_time_on, 2u, 0x3fu);
    
    /* set_first_fix_freq_time_off: bit 30, 8 bits */
    dst_p[3] |= pack_left_shift_u8(src_p->set_first_fix_freq_time_off, 6u, 0xc0u);
    dst_p[4] |= pack_right_shift_u8(src_p->set_first_fix_freq_time_off, 2u, 0x3fu);
    
    /* bypass_off: bit 46, 1 bit */
    dst_p[5] |= pack_left_shift_u8(src_p->bypass_off, 6u, 0x40u);
    
    return (8);
}

int tsmaster_control_unpack(
    tsmaster_control_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }
    
    if (dst_p == NULL || src_p == NULL) {
        return (-EINVAL);
    }
    
    /* set_bypass_initial_decline_time: bit 0, 6 bits */
    dst_p->set_bypass_initial_decline_time = unpack_right_shift_u8(src_p[0], 0u, 0x3fu);
    
    /* set_rev_start_oilP_max: bit 6, 8 bits */
    dst_p->set_rev_start_oilP_max = unpack_right_shift_u8(src_p[0], 6u, 0xc0u);
    dst_p->set_rev_start_oilP_max |= unpack_left_shift_u8(src_p[1], 2u, 0x3fu);
    
    /* set_rev_start_oilP_min: bit 14, 8 bits */
    dst_p->set_rev_start_oilP_min = unpack_right_shift_u8(src_p[1], 6u, 0xc0u);
    dst_p->set_rev_start_oilP_min |= unpack_left_shift_u8(src_p[2], 2u, 0x3fu);
    
    /* set_first_fix_freq_time_on: bit 22, 8 bits */
    dst_p->set_first_fix_freq_time_on = unpack_right_shift_u8(src_p[2], 6u, 0xc0u);
    dst_p->set_first_fix_freq_time_on |= unpack_left_shift_u8(src_p[3], 2u, 0x3fu);
    
    /* set_first_fix_freq_time_off: bit 30, 8 bits */
    dst_p->set_first_fix_freq_time_off = unpack_right_shift_u8(src_p[3], 6u, 0xc0u);
    dst_p->set_first_fix_freq_time_off |= unpack_left_shift_u8(src_p[4], 2u, 0x3fu);
    
    /* bypass_off: bit 46, 1 bit */
    dst_p->bypass_off = unpack_right_shift_u8(src_p[5], 6u, 0x40u);
    
    return (0);
}

/* ========================================================================
 * tsmaster_control2 Message Pack/Unpack (ID: 0x98080108)
 * ======================================================================== */

int tsmaster_control2_pack(
    uint8_t *dst_p,
    const tsmaster_control2_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }
    
    if (dst_p == NULL || src_p == NULL) {
        return (-EINVAL);
    }
    
    memset(&dst_p[0], 0, 8);
    
    /* auto_mode: bit 0, 1 bit */
    dst_p[0] |= pack_left_shift_u8(src_p->auto_mode, 0u, 0x01u);

    /* set_extend_time: bit 1, 6 bits */
    dst_p[0] |= pack_left_shift_u8(src_p->set_extend_time, 1u, 0x7eu);

    /* set_extend_pressure: bit 8, 8 bits */
    dst_p[1] |= pack_left_shift_u8(src_p->set_extend_pressure, 0u, 0xffu);
    
    /* system_enable: bit 16, 1 bit */
    dst_p[2] |= pack_left_shift_u8(src_p->system_enable, 0u, 0x01u);
    
    /* set_cooler_temperature_on: bit 17, 12 bits */
    dst_p[2] |= pack_left_shift_u16(src_p->set_cooler_temperature_on, 1u, 0xfeu);
    dst_p[3] |= pack_right_shift_u16(src_p->set_cooler_temperature_on, 7u, 0x1fu);
    
    /* set_cooler_temperature_off: bit 29, 12 bits */
    dst_p[3] |= pack_left_shift_u16(src_p->set_cooler_temperature_off, 5u, 0xe0u);
    dst_p[4] |= pack_right_shift_u16(src_p->set_cooler_temperature_off, 3u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(src_p->set_cooler_temperature_off, 11u, 0x01u);
    
    /* set_bypass_ratio: bit 41, 6 bits */
    dst_p[5] |= pack_left_shift_u8(src_p->set_bypass_ratio, 1u, 0x7eu);
    
    /* set_retract_pressure: bit 48, 7 bits */
    dst_p[6] |= pack_left_shift_u8(src_p->set_retract_pressure, 0u, 0x7fu);

    /* set_retract_time: bit 56, 5 bits */
    dst_p[7] |= pack_left_shift_u8(src_p->set_retract_time, 0u, 0x1fu);
    
    return (8);
}

int tsmaster_control2_unpack(
    tsmaster_control2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }
    
    if (dst_p == NULL || src_p == NULL) {
        return (-EINVAL);
    }
    
    /* set_retract_pressure: bit 48, 7 bits */
    dst_p->set_retract_pressure = unpack_right_shift_u8(src_p[6], 0u, 0x7fu);

    /* set_extend_pressure: bit 8, 8 bits */
    dst_p->set_extend_pressure = unpack_right_shift_u8(src_p[1], 0u, 0xffu);

    /* set_extend_time: bit 1, 6 bits */
    dst_p->set_extend_time = unpack_right_shift_u8(src_p[0], 1u, 0x7eu);

    /* auto_mode: bit 0, 1 bit */
    dst_p->auto_mode = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    
    /* system_enable: bit 16, 1 bit */
    dst_p->system_enable = unpack_right_shift_u8(src_p[2], 0u, 0x01u);
    
    /* set_cooler_temperature_on: bit 17, 12 bits */
    dst_p->set_cooler_temperature_on = unpack_right_shift_u16(src_p[2], 1u, 0xfeu);
    dst_p->set_cooler_temperature_on |= unpack_left_shift_u16(src_p[3], 7u, 0x1fu);
    
    /* set_cooler_temperature_off: bit 29, 12 bits */
    dst_p->set_cooler_temperature_off = unpack_right_shift_u16(src_p[3], 5u, 0xe0u);
    dst_p->set_cooler_temperature_off |= unpack_left_shift_u16(src_p[4], 3u, 0xffu);
    dst_p->set_cooler_temperature_off |= unpack_left_shift_u16(src_p[5], 11u, 0x01u);
    
    /* set_bypass_ratio: bit 41, 6 bits */
    dst_p->set_bypass_ratio = unpack_right_shift_u8(src_p[5], 1u, 0x7eu);

    /* set_retract_time: bit 56, 5 bits */
    dst_p->set_retract_time = unpack_right_shift_u8(src_p[7], 0u, 0x1fu);
    
    return (0);
}

/* ========================================================================
 * device_status Encode/Decode Functions
 * ======================================================================== */

uint8_t device_status_bypass_ratio_encode(double value)
{
    // DBC定义范围: 0..50%, Scale: 1.0, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 50.0) value = 50.0;
    return (uint8_t)(value / 1.0);
}

double device_status_bypass_ratio_decode(uint8_t value)
{
    return ((double)value * 1.0);
}

bool device_status_bypass_ratio_is_in_range(uint8_t value)
{
    return (value <= 50u);
}

uint8_t device_status_oil_pressure_encode(double value)
{
    // DBC定义范围: 0..20 MPa, Scale: 0.1, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 20.0) value = 20.0;
    return (uint8_t)(value / 0.1);
}

double device_status_oil_pressure_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool device_status_oil_pressure_is_in_range(uint8_t value)
{
    return (value <= 200u);
}

uint16_t device_status_oil_temperature_encode(double value)
{
    // DBC定义范围: -40..80°C, Scale: 0.1, Offset: -40
    // 编码公式: raw = (physical - offset) / scale = (value + 40) / 0.1
    if (value < -40.0) value = -40.0;
    if (value > 80.0) value = 80.0;
    return (uint16_t)((value + 40.0) / 0.1);
}

double device_status_oil_temperature_decode(uint16_t value)
{
    // 解码公式: physical = raw * scale + offset = value * 0.1 - 40
    return ((double)value * 0.1 - 40.0);
}

bool device_status_oil_temperature_is_in_range(uint16_t value)
{
    return (value <= 1200u);
}

uint16_t device_status_LNG_temperature_encode(double value)
{
    // DBC定义范围: -40..80°C, Scale: 0.1, Offset: -40
    // 编码公式: raw = (physical - offset) / scale = (value + 40) / 0.1
    if (value < -40.0) value = -40.0;
    if (value > 80.0) value = 80.0;
    return (uint16_t)((value + 40.0) / 0.1);
}

double device_status_LNG_temperature_decode(uint16_t value)
{
    // 解码公式: physical = raw * scale + offset = value * 0.1 - 40
    return ((double)value * 0.1 - 40.0);
}

bool device_status_LNG_temperature_is_in_range(uint16_t value)
{
    return (value <= 1200u);
}

uint16_t device_status_LNG_pressure_encode(double value)
{
    // DBC定义范围: 0..40 MPa, Scale: 0.1, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 40.0) value = 40.0;
    return (uint16_t)(value / 0.1);
}

double device_status_LNG_pressure_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool device_status_LNG_pressure_is_in_range(uint16_t value)
{
    return (value <= 400u);
}

uint8_t device_status_rev_freq_encode(double value)
{
    // DBC定义范围: 0..255 min/seconds, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 255.0) value = 255.0;
    return (uint8_t)(value);
}

double device_status_rev_freq_decode(uint8_t value)
{
    return ((double)value);
}

bool device_status_rev_freq_is_in_range(uint8_t value)
{
    return (value <= 255u);
}

uint8_t device_status_rev_enable_encode(double value)
{
    return (uint8_t)(value);
}

double device_status_rev_enable_decode(uint8_t value)
{
    return ((double)value);
}

bool device_status_rev_enable_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t device_status_cooler_enable_encode(double value)
{
    return (uint8_t)(value);
}

double device_status_cooler_enable_decode(uint8_t value)
{
    return ((double)value);
}

bool device_status_cooler_enable_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t device_status_debug_encode(double value)
{
    return (uint8_t)(value);
}

double device_status_debug_decode(uint8_t value)
{
    return ((double)value);
}

bool device_status_debug_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t gcu_debug1_reversal_valve_st_encode(double value)
{
    return (uint8_t)(value);
}

double gcu_debug1_reversal_valve_st_decode(uint8_t value)
{
    return ((double)value);
}

bool gcu_debug1_reversal_valve_st_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint16_t gcu_debug1_LNG_pressure_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double gcu_debug1_LNG_pressure_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool gcu_debug1_LNG_pressure_is_in_range(uint16_t value)
{
    return (value <= 511u);
}

uint8_t gcu_debug1_reversal_valve_hz_encode(double value)
{
    return (uint8_t)(value);
}

double gcu_debug1_reversal_valve_hz_decode(uint8_t value)
{
    return ((double)value);
}

bool gcu_debug1_reversal_valve_hz_is_in_range(uint8_t value)
{
    return (value <= 127u);
}

uint16_t gcu_debug1_LNG_temperature_encode(double value)
{
    return (uint16_t)((value - -80.0) / 0.1);
}

double gcu_debug1_LNG_temperature_decode(uint16_t value)
{
    return (((double)value * 0.1) + -80.0);
}

bool gcu_debug1_LNG_temperature_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint8_t gcu_debug1_oil_pressure_encode(double value)
{
    return (uint8_t)(value / 0.1);
}

double gcu_debug1_oil_pressure_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool gcu_debug1_oil_pressure_is_in_range(uint8_t value)
{
    (void)value;
    return (true);
}

uint16_t gcu_debug1_oil_temperature_encode(double value)
{
    return (uint16_t)((value - -80.0) / 0.1);
}

double gcu_debug1_oil_temperature_decode(uint16_t value)
{
    return (((double)value * 0.1) + -80.0);
}

bool gcu_debug1_oil_temperature_is_in_range(uint16_t value)
{
    return (value <= 4095u);
}

uint8_t gcu_debug1_reserve_debug1_encode(double value)
{
    return (uint8_t)(value);
}

double gcu_debug1_reserve_debug1_decode(uint8_t value)
{
    return ((double)value);
}

bool gcu_debug1_reserve_debug1_is_in_range(uint8_t value)
{
    (void)value;
    return (true);
}

/* ========================================================================
 * gcu_control Encode/Decode Functions
 * ======================================================================== */

uint8_t gcu_control_ctrl_reversal_valve_enable_encode(double value)
{
    return (uint8_t)(value);
}

double gcu_control_ctrl_reversal_valve_enable_decode(uint8_t value)
{
    return ((double)value);
}

bool gcu_control_ctrl_reversal_valve_enable_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint16_t gcu_control_ctrl_bypass_valve_duty_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double gcu_control_ctrl_bypass_valve_duty_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool gcu_control_ctrl_bypass_valve_duty_is_in_range(uint16_t value)
{
    return (value <= 1000u);
}

uint8_t gcu_control_ctrl_reversal_valve_freq_encode(double value)
{
    return (uint8_t)(value);
}

double gcu_control_ctrl_reversal_valve_freq_decode(uint8_t value)
{
    return ((double)value);
}

bool gcu_control_ctrl_reversal_valve_freq_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

uint8_t gcu_control_ctrl_cooler_enable_encode(double value)
{
    return (uint8_t)(value);
}

double gcu_control_ctrl_cooler_enable_decode(uint8_t value)
{
    return ((double)value);
}

bool gcu_control_ctrl_cooler_enable_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t gcu_control_ctrl_system_enable_encode(double value)
{
    return (uint8_t)(value);
}

double gcu_control_ctrl_system_enable_decode(uint8_t value)
{
    return ((double)value);
}

bool gcu_control_ctrl_system_enable_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint32_t gcu_control_ctrl_reserved_encode(double value)
{
    return (uint32_t)(value);
}

double gcu_control_ctrl_reserved_decode(uint32_t value)
{
    return ((double)value);
}

bool gcu_control_ctrl_reserved_is_in_range(uint32_t value)
{
    return (value <= 1073741823u);  /* 30 bits: 2^30 - 1 */
}

/* ========================================================================
 * tsmaster_control Encode/Decode Functions
 * ======================================================================== */

uint8_t tsmaster_control_set_bypass_initial_decline_time_encode(double value)
{
    // DBC定义范围: 0..60 s, Scale: 1.0, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 60.0) value = 60.0;
    return (uint8_t)(value / 1.0);
}

double tsmaster_control_set_bypass_initial_decline_time_decode(uint8_t value)
{
    return ((double)value * 1.0);
}

bool tsmaster_control_set_bypass_initial_decline_time_is_in_range(uint8_t value)
{
    return (value <= 60u);
}


uint8_t tsmaster_control_set_rev_start_oilP_max_encode(double value)
{
    // DBC定义范围: 0..10 MPa, Scale: 0.1, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 10.0) value = 10.0;
    return (uint8_t)(value / 0.1);
}

double tsmaster_control_set_rev_start_oilP_max_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool tsmaster_control_set_rev_start_oilP_max_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

uint8_t tsmaster_control_set_rev_start_oilP_min_encode(double value)
{
    // DBC定义范围: 0..10 MPa, Scale: 0.1, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 10.0) value = 10.0;
    return (uint8_t)(value / 0.1);
}

double tsmaster_control_set_rev_start_oilP_min_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool tsmaster_control_set_rev_start_oilP_min_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

uint8_t tsmaster_control_set_first_fix_freq_time_on_encode(double value)
{
    // DBC定义范围: 0..10 s, Scale: 0.1, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 10.0) value = 10.0;
    return (uint8_t)(value / 0.1);
}

double tsmaster_control_set_first_fix_freq_time_on_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool tsmaster_control_set_first_fix_freq_time_on_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

uint8_t tsmaster_control_set_first_fix_freq_time_off_encode(double value)
{
    // DBC定义范围: 0..10 s, Scale: 0.1, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 10.0) value = 10.0;
    return (uint8_t)(value / 0.1);
}

double tsmaster_control_set_first_fix_freq_time_off_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool tsmaster_control_set_first_fix_freq_time_off_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

uint8_t tsmaster_control_bypass_off_encode(double value)
{
    return (uint8_t)(value);
}

double tsmaster_control_bypass_off_decode(uint8_t value)
{
    return ((double)value);
}

bool tsmaster_control_bypass_off_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

/* ========================================================================
 * tsmaster_control2 Encode/Decode Functions
 * ======================================================================== */

uint8_t tsmaster_control2_auto_mode_encode(double value)
{
    return (uint8_t)(value);
}

double tsmaster_control2_auto_mode_decode(uint8_t value)
{
    return ((double)value);
}

bool tsmaster_control2_auto_mode_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t tsmaster_control2_set_extend_time_encode(double value)
{
    if (value < 0.0) value = 0.0;
    if (value > 6.3) value = 6.3;
    return (uint8_t)(value / 0.1);
}

double tsmaster_control2_set_extend_time_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool tsmaster_control2_set_extend_time_is_in_range(uint8_t value)
{
    return (value <= 63u);
}

uint8_t tsmaster_control2_set_extend_pressure_encode(double value)
{
    if (value < 0.0) value = 0.0;
    if (value > 25.5) value = 25.5;
    return (uint8_t)(value / 0.1);
}

double tsmaster_control2_set_extend_pressure_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool tsmaster_control2_set_extend_pressure_is_in_range(uint8_t value)
{
    return (value <= 255u);
}

uint8_t tsmaster_control2_system_enable_encode(double value)
{
    return (uint8_t)(value);
}

double tsmaster_control2_system_enable_decode(uint8_t value)
{
    return ((double)value);
}

bool tsmaster_control2_system_enable_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint16_t tsmaster_control2_set_cooler_temperature_on_encode(double value)
{
    // DBC定义范围: -40..80°C, Scale: 0.1, Offset: -40
    // 编码公式: raw = (physical - offset) / scale = (value + 40) / 0.1
    if (value < -40.0) value = -40.0;
    if (value > 80.0) value = 80.0;
    return (uint16_t)((value + 40.0) / 0.1);
}

double tsmaster_control2_set_cooler_temperature_on_decode(uint16_t value)
{
    // 解码公式: physical = raw * scale + offset = value * 0.1 - 40
    return ((double)value * 0.1 - 40.0);
}

bool tsmaster_control2_set_cooler_temperature_on_is_in_range(uint16_t value)
{
    // raw值范围: 0..1200 (对应物理值 -40..80°C)
    return (value <= 1200u);
}

uint16_t tsmaster_control2_set_cooler_temperature_off_encode(double value)
{
    // DBC定义范围: -40..80°C, Scale: 0.1, Offset: -40
    // 编码公式: raw = (physical - offset) / scale = (value + 40) / 0.1
    if (value < -40.0) value = -40.0;
    if (value > 80.0) value = 80.0;
    return (uint16_t)((value + 40.0) / 0.1);
}

double tsmaster_control2_set_cooler_temperature_off_decode(uint16_t value)
{
    // 解码公式: physical = raw * scale + offset = value * 0.1 - 40
    return ((double)value * 0.1 - 40.0);
}

bool tsmaster_control2_set_cooler_temperature_off_is_in_range(uint16_t value)
{
    // raw值范围: 0..1200 (对应物理值 -40..80°C)
    return (value <= 1200u);
}

uint8_t tsmaster_control2_set_bypass_ratio_encode(double value)
{
    // DBC定义范围: 0..50%, Scale: 1.0, 自动限制到有效范围
    if (value < 0.0) value = 0.0;
    if (value > 50.0) value = 50.0;
    return (uint8_t)(value / 1.0);
}

double tsmaster_control2_set_bypass_ratio_decode(uint8_t value)
{
    return ((double)value * 1.0);
}

bool tsmaster_control2_set_bypass_ratio_is_in_range(uint8_t value)
{
    return (value <= 50u);
}

uint8_t tsmaster_control2_set_retract_pressure_encode(double value)
{
    if (value < 0.0) value = 0.0;
    if (value > 12.7) value = 12.7;
    return (uint8_t)(value / 0.1);
}

double tsmaster_control2_set_retract_pressure_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool tsmaster_control2_set_retract_pressure_is_in_range(uint8_t value)
{
    return (value <= 127u);
}

uint8_t tsmaster_control2_set_retract_time_encode(double value)
{
    if (value < 0.0) value = 0.0;
    if (value > 3.1) value = 3.1;
    return (uint8_t)(value / 0.1);
}

double tsmaster_control2_set_retract_time_decode(uint8_t value)
{
    return ((double)value * 0.1);
}

bool tsmaster_control2_set_retract_time_is_in_range(uint8_t value)
{
    return (value <= 31u);
}
