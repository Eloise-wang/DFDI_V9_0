/**
 * @file gcu_control_dbc.h
 * @brief Auto-generated from gcu_hp_control_05.dbc
 * @note This file was automatically generated. Do not modify manually.
 * @date 2025-01-27
 */

#ifndef GCU_CONTROL_DBC_H
#define GCU_CONTROL_DBC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EINVAL
#    define EINVAL 22
#endif

/* ========================================================================
 * CAN Message IDs
 * ======================================================================== */
#define CAN_MSG_DEVICE_STATUS_ID       0x980FF16CU  /* 2551181676 - GCU -> TSMaster */
#define CAN_MSG_TSMASTER_CONTROL_ID    0x98080100U  /* 2550661376 - TSMaster -> GCU */
#define CAN_MSG_TSMASTER_CONTROL2_ID   0x98080108U  /* 2550661384 - TSMaster -> GCU */

/* Message cycle times (ms) */
#define CAN_MSG_DEVICE_STATUS_CYCLE    10U
#define CAN_MSG_TSMASTER_CONTROL_CYCLE 100U
#define CAN_MSG_TSMASTER_CONTROL2_CYCLE 100U

/* ========================================================================
 * Signal Structures
 * ======================================================================== */

/**
 * @brief device_status message signals (GCU -> TSMaster)
 * @note Message ID: 0x980FF16C, DLC: 8, Cycle: 10ms
 * All signal values are as on the CAN bus (raw values).
 */
typedef struct {
    /**
     * 旁通阀比例
     * Range: 0..50 (0..50 %)
     * Scale: 1.0
     * Offset: 0
     */
    uint8_t bypass_ratio;
    
    /**
     * 油压
     * Range: 0..200 (0..20 MPa)
     * Scale: 0.1
     * Offset: 0
     */
    uint8_t oil_pressure;
    
    /**
     * 油温
     * Range: 0..1200 (-40..80 °C)
     * Scale: 0.1
     * Offset: -40
     */
    uint16_t oil_temperature;
    
    /**
     * LNG温度
     * Range: 0..1200 (-40..80 °C)
     * Scale: 0.1
     * Offset: -40
     */
    uint16_t LNG_temperature;
    
    /**
     * LNG压力
     * Range: 0..400 (0..40 MPa)
     * Scale: 0.1
     * Offset: 0
     */
    uint16_t LNG_pressure;
    
    /**
     * 换向频率
     * Range: 0..255 (0..255 min/seconds)
     * Scale: 1
     * Offset: 0
     */
    uint8_t rev_freq;
    
    /**
     * 换向阀使能状态
     * Range: 0..1 (0=OFF, 1=ON)
     * Scale: 1
     * Offset: 0
     */
    uint8_t rev_enable;
    
    /**
     * 风冷器使能状态
     * Range: 0..1 (0=OFF, 1=ON)
     * Scale: 1
     * Offset: 0
     */
    uint8_t cooler_enable;
    
    /**
     * 传感器调试/故障标志
     * Range: 0..1 (0=Normal, 1=Error)
     * Scale: 1
     * Offset: 0
     */
    uint8_t debug;
    
    // 注意：bit 59-63 (剩余5位) 未使用，预留未来扩展
} device_status_t;

/**
 * @brief tsmaster_control message signals (TSMaster -> GCU)
 * @note Message ID: 0x98080100, DLC: 8, Cycle: 100ms
 * All signal values are as on the CAN bus (raw values).
 */
typedef struct {
    /**
     * 设置旁通阀起始下降时间
     * Range: 0..60 (0..60 s)
     * Scale: 1.0
     * Offset: 0
     */
    uint8_t set_bypass_initial_decline_time;
    
    /**
     * 换向阀启动油压限制最大值设置
     * Range: 0..100 (0..10 MPa)
     * Scale: 0.1
     * Offset: 0
     */
    uint8_t set_rev_start_oilP_max;
    
    /**
     * 换向阀启动油压限制最小值设置
     * Range: 0..100 (0..10 MPa)
     * Scale: 0.1
     * Offset: 0
     */
    uint8_t set_rev_start_oilP_min;
    
    /**
     * 第一阶段换向阀固定频率设置开启时常
     * Range: 0..100 (0..10 s)
     * Scale: 0.1
     * Offset: 0
     */
    uint8_t set_first_fix_freq_time_on;
    
    /**
     * 第一阶段换向阀固定频率设置关闭时常
     * Range: 0..100 (0..10 s)
     * Scale: 0.1
     * Offset: 0
     */
    uint8_t set_first_fix_freq_time_off;
    
    uint8_t bypass_off;
} tsmaster_control_t;

/**
 * @brief tsmaster_control2 message signals (TSMaster -> GCU)
 * @note Message ID: 0x98080108, DLC: 8, Cycle: 100ms
 * All signal values are as on the CAN bus (raw values).
 */
typedef struct {
    /**
     * 第二阶段手动模式
     * Range: 0..1
     * Scale: 1
     * Offset: 0
     */
    uint8_t set_second_manual;

    /**
     * 第二阶段吸油时间
     * Range: 0..100 (0..10 s)
     * Scale: 0.1
     * Offset: 0
     */
    uint8_t set_second_oilSuction_time;
    
    /**
     * 系统开关
     * Range: 0..1 (0=Disable, 1=Enable)
     * Scale: 1
     * Offset: 0
     */
    uint8_t system_enable;
    
    /**
     * 设置风冷器开启温度
     * Range: 0..1200 (-40..80 °C)
     * Scale: 0.1
     * Offset: -40
     */
    uint16_t set_cooler_temperature_on;
    
    /**
     * 设置风冷器关闭温度
     * Range: 0..1200 (-40..80 °C)
     * Scale: 0.1
     * Offset: -40
     */
    uint16_t set_cooler_temperature_off;
    
    /**
     * 设置旁通阀比例
     * Range: 0..50 (0..50 %)
     * Scale: 1.0
     * Offset: 0
     */
    uint8_t set_bypass_ratio;
    
    /**
     * 第二阶段做功时间
     * Range: 0..100 (0..10 s)
     * Scale: 0.1
     * Offset: 0
     */
    uint8_t set_second_workDone_time;
} tsmaster_control2_t;

/* ========================================================================
 * Signal Value Definitions
 * ======================================================================== */

/* rev_enable */
#define REV_ENABLE_OFF     0U
#define REV_ENABLE_ON      1U

/* cooler_enable */
#define COOLER_ENABLE_OFF  0U
#define COOLER_ENABLE_ON   1U

/* system_enable */
#define SYSTEM_DISABLE     0U
#define SYSTEM_ENABLE      1U

/* ========================================================================
 * Pack/Unpack Functions
 * ======================================================================== */

/**
 * @brief Pack device_status message
 * @param[out] dst_p Buffer to pack the message into
 * @param[in] src_p Data to pack
 * @param[in] size Size of dst_p
 * @return Size of packed data, or negative error code
 */
int device_status_pack(
    uint8_t *dst_p,
    const device_status_t *src_p,
    size_t size);

/**
 * @brief Unpack device_status message
 * @param[out] dst_p Object to unpack the message into
 * @param[in] src_p Message to unpack
 * @param[in] size Size of src_p
 * @return zero(0) or negative error code
 */
int device_status_unpack(
    device_status_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * @brief Pack tsmaster_control message
 * @param[out] dst_p Buffer to pack the message into
 * @param[in] src_p Data to pack
 * @param[in] size Size of dst_p
 * @return Size of packed data, or negative error code
 */
int tsmaster_control_pack(
    uint8_t *dst_p,
    const tsmaster_control_t *src_p,
    size_t size);

/**
 * @brief Unpack tsmaster_control message
 * @param[out] dst_p Object to unpack the message into
 * @param[in] src_p Message to unpack
 * @param[in] size Size of src_p
 * @return zero(0) or negative error code
 */
int tsmaster_control_unpack(
    tsmaster_control_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * @brief Pack tsmaster_control2 message
 * @param[out] dst_p Buffer to pack the message into
 * @param[in] src_p Data to pack
 * @param[in] size Size of dst_p
 * @return Size of packed data, or negative error code
 */
int tsmaster_control2_pack(
    uint8_t *dst_p,
    const tsmaster_control2_t *src_p,
    size_t size);

/**
 * @brief Unpack tsmaster_control2 message
 * @param[out] dst_p Object to unpack the message into
 * @param[in] src_p Message to unpack
 * @param[in] size Size of src_p
 * @return zero(0) or negative error code
 */
int tsmaster_control2_unpack(
    tsmaster_control2_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/* ========================================================================
 * Encode/Decode Functions
 * ======================================================================== */

/* device_status encode/decode functions */
uint8_t device_status_bypass_ratio_encode(double value);
double device_status_bypass_ratio_decode(uint8_t value);
bool device_status_bypass_ratio_is_in_range(uint8_t value);

uint8_t device_status_oil_pressure_encode(double value);
double device_status_oil_pressure_decode(uint8_t value);
bool device_status_oil_pressure_is_in_range(uint8_t value);

uint16_t device_status_oil_temperature_encode(double value);
double device_status_oil_temperature_decode(uint16_t value);
bool device_status_oil_temperature_is_in_range(uint16_t value);

uint16_t device_status_LNG_temperature_encode(double value);
double device_status_LNG_temperature_decode(uint16_t value);
bool device_status_LNG_temperature_is_in_range(uint16_t value);

uint16_t device_status_LNG_pressure_encode(double value);
double device_status_LNG_pressure_decode(uint16_t value);
bool device_status_LNG_pressure_is_in_range(uint16_t value);

uint8_t device_status_rev_freq_encode(double value);
double device_status_rev_freq_decode(uint8_t value);
bool device_status_rev_freq_is_in_range(uint8_t value);

uint8_t device_status_rev_enable_encode(double value);
double device_status_rev_enable_decode(uint8_t value);
bool device_status_rev_enable_is_in_range(uint8_t value);

uint8_t device_status_cooler_enable_encode(double value);
double device_status_cooler_enable_decode(uint8_t value);
bool device_status_cooler_enable_is_in_range(uint8_t value);

uint8_t device_status_debug_encode(double value);
double device_status_debug_decode(uint8_t value);
bool device_status_debug_is_in_range(uint8_t value);

/* tsmaster_control encode/decode functions */
uint8_t tsmaster_control_set_bypass_initial_decline_time_encode(double value);
double tsmaster_control_set_bypass_initial_decline_time_decode(uint8_t value);
bool tsmaster_control_set_bypass_initial_decline_time_is_in_range(uint8_t value);

uint8_t tsmaster_control_set_rev_start_oilP_max_encode(double value);
double tsmaster_control_set_rev_start_oilP_max_decode(uint8_t value);
bool tsmaster_control_set_rev_start_oilP_max_is_in_range(uint8_t value);

uint8_t tsmaster_control_set_rev_start_oilP_min_encode(double value);
double tsmaster_control_set_rev_start_oilP_min_decode(uint8_t value);
bool tsmaster_control_set_rev_start_oilP_min_is_in_range(uint8_t value);

uint8_t tsmaster_control_set_first_fix_freq_time_on_encode(double value);
double tsmaster_control_set_first_fix_freq_time_on_decode(uint8_t value);
bool tsmaster_control_set_first_fix_freq_time_on_is_in_range(uint8_t value);

uint8_t tsmaster_control_set_first_fix_freq_time_off_encode(double value);
double tsmaster_control_set_first_fix_freq_time_off_decode(uint8_t value);
bool tsmaster_control_set_first_fix_freq_time_off_is_in_range(uint8_t value);

uint8_t tsmaster_control_bypass_off_encode(double value);
double tsmaster_control_bypass_off_decode(uint8_t value);
bool tsmaster_control_bypass_off_is_in_range(uint8_t value);

/* tsmaster_control2 encode/decode functions */
uint8_t tsmaster_control2_set_second_manual_encode(double value);
double tsmaster_control2_set_second_manual_decode(uint8_t value);
bool tsmaster_control2_set_second_manual_is_in_range(uint8_t value);

uint8_t tsmaster_control2_set_second_oilSuction_time_encode(double value);
double tsmaster_control2_set_second_oilSuction_time_decode(uint8_t value);
bool tsmaster_control2_set_second_oilSuction_time_is_in_range(uint8_t value);

uint8_t tsmaster_control2_system_enable_encode(double value);
double tsmaster_control2_system_enable_decode(uint8_t value);
bool tsmaster_control2_system_enable_is_in_range(uint8_t value);

uint16_t tsmaster_control2_set_cooler_temperature_on_encode(double value);
double tsmaster_control2_set_cooler_temperature_on_decode(uint16_t value);
bool tsmaster_control2_set_cooler_temperature_on_is_in_range(uint16_t value);

uint16_t tsmaster_control2_set_cooler_temperature_off_encode(double value);
double tsmaster_control2_set_cooler_temperature_off_decode(uint16_t value);
bool tsmaster_control2_set_cooler_temperature_off_is_in_range(uint16_t value);

uint8_t tsmaster_control2_set_bypass_ratio_encode(double value);
double tsmaster_control2_set_bypass_ratio_decode(uint8_t value);
bool tsmaster_control2_set_bypass_ratio_is_in_range(uint8_t value);

uint8_t tsmaster_control2_set_second_workDone_time_encode(double value);
double tsmaster_control2_set_second_workDone_time_decode(uint8_t value);
bool tsmaster_control2_set_second_workDone_time_is_in_range(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* GCU_CONTROL_DBC_H */
