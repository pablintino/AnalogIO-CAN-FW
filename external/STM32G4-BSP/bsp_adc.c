/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#include <stddef.h>
#include <bsp_tick.h>
#include "bsp_common_utils.h"
#include "bsp_adc.h"


static ret_status __badc_exit_power_down(badc_instance_t *adc, uint32_t tick_start);

static inline bool __badc_is_adc_in_power_down_mode(const badc_instance_t *adc);

static inline ret_status __badc_get_sequencer_position(const badc_instance_t *adc, uint8_t sequence_number,
                                                       volatile uint32_t **sequencer_register, uint8_t *offset);

static ret_status __bcan_enable_regulator(badc_instance_t *adc, uint32_t tick_start);

static void __badc_config_channel_sampling_time(badc_instance_t *adc, uint8_t channel_number,
                                                enum badc_sampling_time_e sampling_time);

ret_status badc_config(badc_instance_t *adc, const badc_config_t *config) {

    if (adc == NULL || config == NULL) {
        return STATUS_ERR;
    }

    if (config->mode == BADC_MODE_DISCONTINUOUS && config->discontinuous_channels > 8) {
        return STATUS_ERR;
    }

    uint32_t tickstart = btick_get_ticks();

    ret_status tmp_status;
    /* If the ADC is in power down mode just get the ADC out of that state */
    if (__badc_is_adc_in_power_down_mode(adc)) {
        tmp_status = __badc_exit_power_down(adc, tickstart);
        if (tmp_status != STATUS_OK) {
            return tmp_status;
        }
    }

    tmp_status = __bcan_enable_regulator(adc, tickstart);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    /*
     * Alignment: Right->FIXED VALUE
     *
     */
    uint32_t cfgr_value = config->mode == BADC_MODE_DISCONTINUOUS ? ADC_CFGR_DISCEN : 0x00;
    cfgr_value |= config->mode == BADC_MODE_CONTINUOUS ? ADC_CFGR_CONT : 0x00;
    cfgr_value |= config->preserve_overruns ? 0x00 : ADC_CFGR_OVRMOD;
    cfgr_value |=
            config->mode == BADC_MODE_DISCONTINUOUS ? config->discontinuous_channels << ADC_CFGR_DISCNUM_Pos : 0x00;
    cfgr_value |= config->resolution;


    __BSP_SET_MASKED_REG_VALUE(adc->CFGR, ADC_CFGR_CONT | ADC_CFGR_OVRMOD | ADC_CFGR_ALIGN | ADC_CFGR_DISCEN,
                               cfgr_value);

    /* Disable oversampling mode and gain corrections TODO: This features could be useful, try to implement them */
    __BSP_CLEAR_MASKED_REG(adc->CFGR2, ADC_CFGR2_ROVSE | ADC_CFGR2_GCOMP);

    __BSP_CLEAR_MASKED_REG(adc->SQR1, ADC_SQR1_L);

    return tmp_status;

}


ret_status badc_config_channel(badc_instance_t *adc, const badc_config_channel_t *config) {

    if (adc == NULL || config == NULL) {
        return STATUS_ERR;
    }

    volatile uint32_t *sequencer_register;
    uint8_t offset;
    /* Configure the channel conversion sequence */
    ret_status tmp_status = __badc_get_sequencer_position(adc, config->sequencer, &sequencer_register, &offset);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    __BSP_SET_MASKED_REG_VALUE(*sequencer_register, 0x1F << offset, config->channel_number << offset);

    /* Configure sample time */
    __badc_config_channel_sampling_time(adc, config->channel_number, config->sampling_time);

    __BSP_SET_MASKED_REG_VALUE(adc->DIFSEL, 1U << config->channel_number,
                               (config->differential ? 1U : 0) << config->channel_number);

    if (config->differential) {
        __badc_config_channel_sampling_time(adc, config->channel_number + 1, config->sampling_time);
    }

    return STATUS_OK;

}


static void __badc_config_channel_sampling_time(badc_instance_t *adc, uint8_t channel_number,
                                                enum badc_sampling_time_e sampling_time) {/* Configure sampling time */
    uint8_t channel_sample_time_bit =
            (channel_number > 9 ? channel_number - 10 : channel_number) * 3;
    if (channel_number > 9) {
        __BSP_SET_MASKED_REG_VALUE(adc->SMPR2, 0x03 << channel_sample_time_bit,
                                   sampling_time << channel_sample_time_bit);
    } else {
        __BSP_SET_MASKED_REG_VALUE(adc->SMPR1, 0x03 << channel_sample_time_bit,
                                   sampling_time << channel_sample_time_bit);
    }
}

static inline bool __badc_is_adc_in_power_down_mode(const badc_instance_t *adc) {
    return __BSP_IS_FLAG_SET(adc->CR, ADC_CR_DEEPPWD) != 0;
}


static ret_status __bcan_enable_regulator(badc_instance_t *adc, uint32_t tick_start) {

    const uint32_t disabled_bits =
            ADC_CR_ADCAL | ADC_CR_ADSTP | ADC_CR_JADSTART | ADC_CR_ADSTART | ADC_CR_ADDIS | ADC_CR_ADEN;

    ret_status tmp_status = BSP_UTIL_wait_flag_status(&adc->CR, disabled_bits, 0U, tick_start, 25u);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    __BSP_SET_MASKED_REG(adc->CR, ADC_CR_ADVREGEN);
    return BSP_UTIL_wait_flag_status(&adc->CR, ADC_CR_ADVREGEN, ADC_CR_ADVREGEN, tick_start, 25u);
}


static ret_status __badc_exit_power_down(badc_instance_t *adc, uint32_t tick_start) {

    const uint32_t disabled_bits =
            ADC_CR_ADCAL | ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART | ADC_CR_ADSTART | ADC_CR_ADDIS | ADC_CR_ADEN;
    __BSP_CLEAR_MASKED_REG(adc->CR, disabled_bits);
    ret_status tmp_status = BSP_UTIL_wait_flag_status(&adc->CR, disabled_bits, 0U, tick_start, 25u);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    __BSP_CLEAR_MASKED_REG(adc->CR, ADC_CR_DEEPPWD);
    return BSP_UTIL_wait_flag_status(&adc->CR, ADC_CR_DEEPPWD, 0U, tick_start, 25u);
}

static inline ret_status __badc_get_sequencer_position(const badc_instance_t *adc, uint8_t sequence_number,
                                                       volatile uint32_t **sequencer_register, uint8_t *offset) {

    if (adc == NULL || sequence_number <= 0 || sequence_number > 16 || sequencer_register == NULL || offset == NULL) {
        return STATUS_ERR;
    }

    if (sequence_number >= 1 && sequence_number < 5) {
        *sequencer_register = &adc->SQR1;

    } else if (sequence_number >= 5 && sequence_number < 10) {
        *sequencer_register = &adc->SQR2;
        *offset = (sequence_number - 5) * 6;
    } else if (sequence_number >= 10 && sequence_number < 15) {
        *sequencer_register = &adc->SQR3;
        *offset = (sequence_number - 10) * 6;
    } else {
        *sequencer_register = &adc->SQR4;
        *offset = (sequence_number - 15) * 6;
    }

    return STATUS_OK;
}