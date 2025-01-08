/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "bsp_types.h"
#include "stm32g4xx.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum bi2c_digital_filter_e {
    BSP_I2C_DIGITAL_FILTER_OFF = 0,
    BSP_I2C_DIGITAL_FILTER_1 = 0x01U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_2 = 0x02U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_3 = 0x03U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_4 = 0x04U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_5 = 0x05U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_6 = 0x06U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_7 = 0x07U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_8 = 0x08U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_9 = 0x09U << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_10 = 0x0AU << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_11 = 0x0BU << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_12 = 0x0CU << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_13 = 0x0DU << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_14 = 0x0EU << I2C_CR1_DNF_Pos,
    BSP_I2C_DIGITAL_FILTER_15 = 0x0FU << I2C_CR1_DNF_Pos,

} bi2c_digital_filter_t;

typedef enum bi2c_clock_source {
    BSP_I2C_CLK_PCLK = 0x00U,
    BSP_I2C_CLK_SYSCLK = 0x01U,
    BSP_I2C_CLK_HSI = 0x02U
} bi2c_clock_source_t;

typedef enum bi2c_speed_source {
    BSP_I2C_SPEED_NONE = 0x00U,
    BSP_I2C_SPEED_100 = 0x01U,
    BSP_I2C_SPEED_400 = 0x02U,
    BSP_I2C_SPEED_1000 = 0x03U
} bi2c_speed_source_t;

typedef enum bi2c_addressing_mode_e {
    BSP_I2C_ADDRESSING_MODE_7 = 0,
    BSP_I2C_ADDRESSING_MODE_10 = I2C_CR2_ADD10,
} bi2c_addressing_mode_t;

typedef struct {
    bi2c_addressing_mode_t addressing_mode;
    bool analog_filter;
    bi2c_digital_filter_t digital_filter;
    uint32_t self_address;
    bi2c_speed_source_t fixed_speed;
    uint32_t custom_timming;
} bsp_i2c_master_config_t;

typedef I2C_TypeDef bi2c_instance;

ret_status bi2c_master_config(bi2c_instance *i2c, const bsp_i2c_master_config_t *config);

ret_status bi2c_master_transfer(
    bi2c_instance *i2c, uint16_t address, uint8_t *data, uint16_t size, bool is_write, uint32_t timeout);

void bi2c_enable(bi2c_instance *i2c);

void bi2c_disable(bi2c_instance *i2c);

#endif // BSP_I2C_H
