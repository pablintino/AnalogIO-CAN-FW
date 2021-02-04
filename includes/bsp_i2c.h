/**
 * MIT License
 *
 * Copyright (c) 2020 Pablo Rodriguez Nava, @pablintino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **/


#ifndef BSP_I2C_H
#define BSP_I2C_H

#include <stdbool.h>
#include <stddef.h>
#include "bsp_types.h"
#include "stm32g4xx.h"



typedef enum {
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

} bsp_i2c_digital_filter;

typedef enum {
    BSP_I2C_CLK_PCLK = 0x00U,
    BSP_I2C_CLK_SYSCLK = 0x01U,
    BSP_I2C_CLK_HSI = 0x02U
} bsp_i2c_clock_source;


typedef enum {
    BSP_I2C_SPEED_NONE = 0x00U,
    BSP_I2C_SPEED_100 = 0x01U,
    BSP_I2C_SPEED_400 = 0x02U,
    BSP_I2C_SPEED_1000 = 0x03U
} bsp_i2c_speed_source;

typedef enum {
    BSP_I2C_ADDRESSING_MODE_7 = 0,
    BSP_I2C_ADDRESSING_MODE_10 = I2C_CR2_ADD10,
} bsp_i2c_addressing_mode;

typedef struct {
    bsp_i2c_addressing_mode AddressingMode;
    bool AnalogFilterEnabled;
    bsp_i2c_digital_filter DigitalFilter;
    uint32_t SelfAddress;
    bsp_i2c_speed_source FixedSpeed;
    uint32_t CustomTimming;
} bsp_i2c_master_config_t;


typedef I2C_TypeDef BSP_I2C_Instance;


ret_status BSP_I2C_master_conf(BSP_I2C_Instance *i2c, bsp_i2c_master_config_t *config);
ret_status
BSP_I2C_master_transfer(BSP_I2C_Instance *i2c, uint16_t address, uint8_t *pData, uint16_t size, bool is_write,
                        uint32_t timeout);

void BSP_I2C_enable(BSP_I2C_Instance *i2c);
void BSP_I2C_disable(BSP_I2C_Instance *i2c);

#endif //BSP_I2C_H