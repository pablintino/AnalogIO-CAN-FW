/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "bsp_dma.h"
#include "bsp_types.h"
#include "stm32g4xx.h"
#include <stdbool.h>

typedef enum badc_resolution_e {
    BADC_RESOLUTON_12_BITS = 0x00U,
    BADC_RESOLUTON_10_BITS = ADC_CFGR_RES_0,
    BADC_RESOLUTON_8_BITS = ADC_CFGR_RES_1,
    BADC_RESOLUTON_6_BITS = ADC_CFGR_RES
} badc_resolution_t;

typedef enum badc_mode_e {
    BADC_MODE_NORMAL = 0x00U,
    BADC_MODE_CONTINUOUS = 0x01U,
    BADC_MODE_DISCONTINUOUS = 0x02U
} badc_mode_t;

typedef enum badc_sampling_time_e {
    BADC_SAMPLING_TIME_2_5 = 0x00,
    BADC_SAMPLING_TIME_6_5 = 0x01,
    BADC_SAMPLING_TIME_12_5 = 0x02,
    BADC_SAMPLING_TIME_24_5 = 0x03,
    BADC_SAMPLING_TIME_47_5 = 0x04,
    BADC_SAMPLING_TIME_92_5 = 0x05,
    BADC_SAMPLING_TIME_247_5 = 0x06,
    BADC_SAMPLING_TIME_640_5 = 0x07,
} badc_sampling_time_t;

typedef enum badc_clock_source {
    BADC_CLK_NONE = 0x00U,
    BADC_CLK_PLLP = 0x01U,
    BADC_CLK_SYSCLK = 0x02U,
} badc_clock_source_t;

typedef enum badc_isr_type_e {
    BADC_ISR_TYPE_ADRDY = ADC_IER_ADRDYIE_Pos,
    BADC_ISR_TYPE_EOSMP = ADC_IER_EOSMPIE_Pos,
    BADC_ISR_TYPE_EOC = ADC_IER_EOCIE_Pos,
    BADC_ISR_TYPE_EOS = ADC_IER_EOSIE_Pos,
    BADC_ISR_TYPE_OVR = ADC_IER_OVRIE_Pos,
    BADC_ISR_TYPE_JEOC = ADC_IER_JEOCIE_Pos,
    BADC_ISR_TYPE_JEOS = ADC_IER_JEOSIE_Pos,
    BADC_ISR_TYPE_AWD1 = ADC_IER_AWD1IE_Pos,
    BADC_ISR_TYPE_AWD2 = ADC_IER_AWD2IE_Pos,
    BADC_ISR_TYPE_AWD3 = ADC_IER_AWD3IE_Pos,
    BADC_ISR_TYPE_JQOVF = ADC_IER_JQOVFIE_Pos
} badc_isr_type_t;

typedef struct badc_config_t {
    badc_mode_t mode;
    uint8_t discontinuous_channels;
    badc_resolution_t resolution;
    bool preserve_overruns;
    bool dma_circular_mode;
} badc_config_t;

typedef struct badc_config_channel_t {
    uint8_t channel_number;
    badc_sampling_time_t sampling_time;
    bool differential;
} badc_config_channel_t;

typedef ADC_TypeDef badc_instance_t;

typedef void (*badc_isr_handler_t)(badc_instance_t *adc, uint32_t group_flags);

ret_status badc_config(badc_instance_t *adc, const badc_config_t *config);

ret_status badc_config_channels(badc_instance_t *adc, const badc_config_channel_t *channels, uint8_t size);

ret_status badc_config_clk_source(badc_instance_t *adc, badc_clock_source_t clock_source);

ret_status badc_enable(badc_instance_t *adc);

ret_status badc_disable(badc_instance_t *adc);

ret_status badc_calibrate(badc_instance_t *adc, bool differential);

ret_status badc_start_conversion(badc_instance_t *adc);

ret_status badc_start_conversion_dma(
    badc_instance_t *adc, bdma_instance_t *dma, bdma_chan_t channel, uint8_t *data_address, uint16_t data_count);

uint16_t badc_get_conversion(badc_instance_t *adc);

ret_status badc_wait_conversion(badc_instance_t *adc, uint32_t timeout);

ret_status badc_enable_irqs(badc_instance_t *adc);

ret_status badc_config_irq(badc_instance_t *adc, badc_isr_type_t irq, badc_isr_handler_t handler);

#endif // BSP_ADC_H
