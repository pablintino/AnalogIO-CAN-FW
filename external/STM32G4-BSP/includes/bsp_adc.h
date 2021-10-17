/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "bsp_types.h"
#include "stm32g4xx.h"
#include <stdbool.h>

enum badc_resolution_e {
    BADC_RESOLUTON_12_BITS = 0x00U,
    BADC_RESOLUTON_10_BITS = ADC_CFGR_RES_0,
    BADC_RESOLUTON_8_BITS = ADC_CFGR_RES_1,
    BADC_RESOLUTON_6_BITS = ADC_CFGR_RES
};

enum badc_mode_e { BADC_MODE_NORMAL = 0x00U, BADC_MODE_CONTINUOUS = 0x01U, BADC_MODE_DISCONTINUOUS = 0x02U };

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

enum badc_clock_source {
    BADC_CLK_NONE = 0x00U,
    BADC_CLK_PLLP = 0x01U,
    BADC_CLK_SYSCLK = 0x02U,
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

ret_status badc_config_clk_source(badc_instance_t *adc, enum badc_clock_source clock_source);

ret_status badc_disable(badc_instance_t *adc);

ret_status badc_calibrate(badc_instance_t *adc, bool differential);

#endif // BSP_ADC_H
