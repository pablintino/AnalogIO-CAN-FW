/*
 * MIT License
 *
 * Copyright (c) 2021 Pablo Rodriguez Nava, @pablintino
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
 */


#ifndef BSP_ADC_H
#define BSP_ADC_H


#include <stdbool.h>
#include "stm32g4xx.h"
#include "bsp_types.h"


enum badc_resolution_e {
    BADC_RESOLUTON_12_BITS = 0x00U,
    BADC_RESOLUTON_10_BITS = ADC_CFGR_RES_0,
    BADC_RESOLUTON_8_BITS = ADC_CFGR_RES_1,
    BADC_RESOLUTON_6_BITS = ADC_CFGR_RES
};


enum badc_mode_e {
    BADC_MODE_NORMAL = 0x00U,
    BADC_MODE_CONTINUOUS = 0x01U,
    BADC_MODE_DISCONTINUOUS = 0x02U
};


enum badc_sampling_time_e {
    BADC_SAMPLING_TIME_2_5 = 0x00,
    BADC_SAMPLING_TIME_6_5 = 0x01,
    BADC_SAMPLING_TIME_12_5 = 0x02,
    BADC_SAMPLING_TIME_24_5 = 0x03,
    BADC_SAMPLING_TIME_47_5 = 0x04,
    BADC_SAMPLING_TIME_92_5 = 0x05,
    BADC_SAMPLING_TIME_247_5 = 0x06,
    BADC_SAMPLING_TIME_640_5 = 0x07,
};

typedef struct badc_config_t {
    enum badc_mode_e mode;
    uint8_t discontinuous_channels;
    enum badc_resolution_e resolution;
    bool preserve_overruns;
} badc_config_t;


typedef struct badc_config_channel_t {
    uint8_t channel_number;
    uint8_t sequencer;
    enum badc_sampling_time_e sampling_time;
    bool differential;
} badc_config_channel_t;


typedef ADC_TypeDef badc_instance_t;


ret_status badc_config(badc_instance_t *adc, const badc_config_t *config);

ret_status badc_config_channel(badc_instance_t *adc, const badc_config_channel_t *config);


#endif //BSP_ADC_H
