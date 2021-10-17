/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#include "bsp_adc.h"
#include "bsp_common_utils.h"
#include <bsp_tick.h>
#include <stddef.h>

static ret_status __badc_exit_power_down(badc_instance_t *adc, uint32_t tick_start);

static inline bool __badc_is_adc_in_power_down_mode(const badc_instance_t *adc);

static inline ret_status __badc_get_sequencer_position(badc_instance_t *adc,
                                                       uint8_t sequence_number,
                                                       volatile uint32_t **sequencer_register,
                                                       uint8_t *offset);

static ret_status __bcan_enable_regulator(badc_instance_t *adc, uint32_t tick_start);

static void __badc_config_channel_sampling_time(badc_instance_t *adc,
                                                uint8_t channel_number,
                                                enum badc_sampling_time_e sampling_time);

ret_status badc_config(badc_instance_t *adc, const badc_config_t *config)
{

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
    cfgr_value |= config->mode == BADC_MODE_DISCONTINUOUS ? config->discontinuous_channels << ADC_CFGR_DISCNUM_Pos
                                                          : 0x00;
    cfgr_value |= config->resolution;

    __BSP_SET_MASKED_REG_VALUE(
        adc->CFGR, ADC_CFGR_CONT | ADC_CFGR_OVRMOD | ADC_CFGR_ALIGN | ADC_CFGR_DISCEN, cfgr_value);

    /* Disable oversampling mode and gain corrections TODO: This features could be useful, try to implement them */
    __BSP_CLEAR_MASKED_REG(adc->CFGR2, ADC_CFGR2_ROVSE | ADC_CFGR2_GCOMP);

    __BSP_CLEAR_MASKED_REG(adc->SQR1, ADC_SQR1_L);

    return tmp_status;
}

ret_status badc_config_channels(badc_instance_t *adc, const badc_config_channel_t *channels, uint8_t size)
{

    if (adc == NULL || channels == NULL || size > 16 || size == 0) {
        return STATUS_ERR;
    }

    for (uint8_t chan_index = 0; chan_index < size; chan_index++) {

        const badc_config_channel_t *chan_config = (const badc_config_channel_t *)(channels + chan_index);

        uint8_t offset = 0;
        volatile uint32_t *sequencer_register;
        /* Configure the channel conversion sequence. Channel rank is based on the order on the config array */
        ret_status tmp_status = __badc_get_sequencer_position(adc, chan_index + 1, &sequencer_register, &offset);
        if (tmp_status != STATUS_OK) {
            return tmp_status;
        }

        __BSP_SET_MASKED_REG_VALUE(*sequencer_register, 0x1F << offset, chan_config->channel_number << offset);

        /* Configure sample time */
        __badc_config_channel_sampling_time(adc, chan_config->channel_number, chan_config->sampling_time);

        __BSP_SET_MASKED_REG_VALUE(adc->DIFSEL,
                                   1U << chan_config->channel_number,
                                   (chan_config->differential ? 1U : 0) << chan_config->channel_number);

        if (chan_config->differential) {
            __badc_config_channel_sampling_time(adc, chan_config->channel_number + 1, chan_config->sampling_time);
        }
    }

    /* Tell the HW the size of the conversion group */
    __BSP_SET_MASKED_REG_VALUE(adc->SQR1, ADC_SQR1_L, (size - 1) & ADC_SQR1_L);

    return STATUS_OK;
}

ret_status badc_enable(badc_instance_t *adc)
{
    /* Check if already enabled */
    if (__BSP_IS_FLAG_SET(adc->CR, ADC_CR_ADEN)) {
        return STATUS_OK;
    }

    /* To enable the ADC ADCAL=0, JADSTART=0, ADSTART=0, ADSTP=0, ADDIS=0 and ADEN=0 and ADVREGEN=1 */
    if ((adc->CR & (ADC_CR_ADCAL | ADC_CR_JADSTART | ADC_CR_ADSTART | ADC_CR_ADSTP | ADC_CR_ADDIS | ADC_CR_ADEN |
                    ADC_CR_ADVREGEN)) != ADC_CR_ADVREGEN) {
        return STATUS_ERR;
    }

    /* It seems that the calibration can interfere in the enabled bit if calibration was done just before enabling.
     * We'll try to enable 3 times */
    ret_status status = STATUS_ERR;
    for (uint8_t retries = 0; retries < 3; retries++) {
        if (!__BSP_IS_FLAG_SET(adc->CR, ADC_CR_ADEN)) {
            __BSP_SET_MASKED_REG(adc->CR, ADC_CR_ADEN);
        }

        /* TODO: Fine tune this timeout */
        status = BSP_UTIL_wait_flag_status_now(&adc->CR, ADC_CR_ADEN, ADC_CR_ADEN, 25u);
        /* If OK we are done */
        if (status == STATUS_OK) {
            break;
        }
    }

    return status;
}

ret_status badc_disable(badc_instance_t *adc)
{
    if (!__BSP_IS_FLAG_SET(adc->CR, ADC_CR_ADEN)) {
        /* Already disabled */
        return STATUS_OK;
    }

    if ((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART | ADC_CR_ADEN)) != ADC_CR_ADEN) {
        /* Cannot disable if a there is an ongoing conversion or the ADC is not enabled*/
        return STATUS_ERR;
    }

    /* Disabling ADC is done by setting ADDIS, not clearing the ADEN one */
    __BSP_SET_MASKED_REG(adc->CR, ADC_CR_ADDIS);
    return BSP_UTIL_wait_flag_status_now(&adc->CR, ADC_CR_ADDIS, 0U, 25u);
}

ret_status badc_calibrate(badc_instance_t *adc, bool differential)
{
    ret_status status = badc_disable(adc);
    if (status != STATUS_OK) {
        return status;
    }

    __BSP_SET_MASKED_REG_VALUE(
        adc->CR, ADC_CR_ADCAL | ADC_CR_ADCALDIF, ADC_CR_ADCAL | ((ADC_CR_ADCALDIF * differential) & ADC_CR_ADCALDIF));
    /* TODO This timeout depends on system freq and ADC clk source. Calculate it properly */
    return BSP_UTIL_wait_flag_status_now(&adc->CR, ADC_CR_ADCAL, 0U, 1000U);
}

ret_status badc_config_clk_source(badc_instance_t *adc, enum badc_clock_source clock_source)
{
    if (adc == ADC1 || adc == ADC2) {
        __BSP_SET_MASKED_REG_VALUE(RCC->CCIPR, RCC_CCIPR_ADC12SEL, clock_source << RCC_CCIPR_ADC12SEL_Pos);
    }
#if defined(ADC3) || defined(ADC4) || defined(ADC5)
    else {
        __BSP_SET_MASKED_REG_VALUE(RCC->CCIPR, RCC_CCIPR_ADC345SEL, clock_source << RCC_CCIPR_ADC345SEL_Pos);
    }
#endif
    return STATUS_OK;
}

ret_status badc_start_conversion(badc_instance_t *adc)
{

    /* Cannot continue if conversion is ongoing or ADC is not enabled */
    if ((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART | ADC_CR_ADEN)) != ADC_CR_ADEN) {
        return STATUS_ERR;
    }

    __BSP_CLEAR_MASKED_REG(adc->ISR, ADC_ISR_EOC | ADC_ISR_EOS | ADC_ISR_OVR);

    __BSP_SET_MASKED_REG(adc->CR, ADC_CR_ADSTART);
}

ret_status badc_wait_conversion(badc_instance_t *adc, uint32_t timeout)
{
    /* If ADC not enabled just return error */
    if (!__BSP_IS_FLAG_SET(adc->CR, ADC_CR_ADEN)) {
        return STATUS_ERR;
    }

    ret_status status = BSP_UTIL_wait_flag_status_now(&adc->ISR, ADC_ISR_EOC, ADC_ISR_EOC, timeout);
    if (status == STATUS_OK) {
        __BSP_CLEAR_MASKED_REG(adc->ISR, ADC_ISR_EOC);
    }
    return status;
}

uint16_t badc_get_conversion(badc_instance_t *adc)
{

    return adc->DR;
}

static void __badc_config_channel_sampling_time(badc_instance_t *adc,
                                                uint8_t channel_number,
                                                enum badc_sampling_time_e sampling_time)
{ /* Configure sampling time */
    uint8_t channel_sample_time_bit = (channel_number > 9 ? channel_number - 10 : channel_number) * 3;
    if (channel_number > 9) {
        __BSP_SET_MASKED_REG_VALUE(
            adc->SMPR2, 0x03 << channel_sample_time_bit, sampling_time << channel_sample_time_bit);
    } else {
        __BSP_SET_MASKED_REG_VALUE(
            adc->SMPR1, 0x03 << channel_sample_time_bit, sampling_time << channel_sample_time_bit);
    }
}

static inline bool __badc_is_adc_in_power_down_mode(const badc_instance_t *adc)
{
    return __BSP_IS_FLAG_SET(adc->CR, ADC_CR_DEEPPWD) != 0;
}

static ret_status __bcan_enable_regulator(badc_instance_t *adc, uint32_t tick_start)
{

    const uint32_t disabled_bits = ADC_CR_ADCAL | ADC_CR_ADSTP | ADC_CR_JADSTART | ADC_CR_ADSTART | ADC_CR_ADDIS |
                                   ADC_CR_ADEN;

    ret_status tmp_status = BSP_UTIL_wait_flag_status(&adc->CR, disabled_bits, 0U, tick_start, 25u);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    __BSP_SET_MASKED_REG(adc->CR, ADC_CR_ADVREGEN);
    return BSP_UTIL_wait_flag_status(&adc->CR, ADC_CR_ADVREGEN, ADC_CR_ADVREGEN, tick_start, 25u);
}

static ret_status __badc_exit_power_down(badc_instance_t *adc, uint32_t tick_start)
{

    const uint32_t disabled_bits = ADC_CR_ADCAL | ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART | ADC_CR_ADSTART |
                                   ADC_CR_ADDIS | ADC_CR_ADEN;
    __BSP_CLEAR_MASKED_REG(adc->CR, disabled_bits);
    ret_status tmp_status = BSP_UTIL_wait_flag_status(&adc->CR, disabled_bits, 0U, tick_start, 25u);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    __BSP_CLEAR_MASKED_REG(adc->CR, ADC_CR_DEEPPWD);
    return BSP_UTIL_wait_flag_status(&adc->CR, ADC_CR_DEEPPWD, 0U, tick_start, 25u);
}

static inline ret_status __badc_get_sequencer_position(badc_instance_t *adc,
                                                       uint8_t sequence_number,
                                                       volatile uint32_t **sequencer_register,
                                                       uint8_t *offset)
{

    if (adc == NULL || sequence_number <= 0 || sequence_number > 16 || sequencer_register == NULL || offset == NULL) {
        return STATUS_ERR;
    }

    if (sequence_number >= 1 && sequence_number < 5) {
        *sequencer_register = &adc->SQR1;
        *offset = sequence_number * 6;
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