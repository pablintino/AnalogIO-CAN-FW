/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#include "bsp_adc.h"
#include "bsp_common_utils.h"
#include "bsp_irq_manager.h"
#include "bsp_tick.h"

#define __BADC_ISR_SOURCES_N (ADC_IER_JQOVFIE_Pos + 1)

struct __badc_irqs_state_s {
    void (*isr_vectors[__BADC_ISR_SOURCES_N])(badc_instance_t *adc, uint32_t flags);
};

static ret_status __badc_exit_power_down(badc_instance_t *adc, uint32_t tick_start);

static inline bool __badc_is_adc_in_power_down_mode(const badc_instance_t *adc);

static inline ret_status __badc_get_sequencer_position(badc_instance_t *adc,
                                                       uint8_t sequence_number,
                                                       volatile uint32_t **sequencer_register,
                                                       uint8_t *offset);

static ret_status __bcan_enable_regulator(badc_instance_t *adc, uint32_t tick_start);

static void __badc_config_channel_sampling_time(badc_instance_t *adc,
                                                uint8_t channel_number,
                                                badc_sampling_time_t sampling_time);

static void __badc_irq_handler(badc_instance_t *adc);

static inline struct __badc_irqs_state_s *__badc_get_instance_state(badc_instance_t *adc);

#if defined(ADC5)
static struct __badc_irqs_state_s __badc_internal_states[5U];
#elif defined(ADC4)
static struct __badc_irqs_state_s __badc_internal_states[4U];
#elif defined(ADC3)
static struct __badc_irqs_state_s __badc_internal_states[3U];
#elif defined(ADC2)
static struct __badc_irqs_state_s __badc_internal_states[2U];
#else
static struct __badc_irqs_state_s __badc_internal_states[1U];
#endif

#if defined(ADC1) || defined(ADC2)
static void __irq_handler_adc12(void)
{
    /* ADC1 and ADC2 share the same IRQ line. Should check here the source */
    if (__BSP_IS_FLAG_SET(ADC1->ISR, ADC_ISR_EOC) || __BSP_IS_FLAG_SET(ADC1->ISR, ADC_ISR_EOS) ||
        __BSP_IS_FLAG_SET(ADC1->ISR, ADC_ISR_OVR)) {
        __badc_irq_handler(ADC1);
    }
    if (__BSP_IS_FLAG_SET(ADC2->ISR, ADC_ISR_EOC) || __BSP_IS_FLAG_SET(ADC2->ISR, ADC_ISR_EOS) ||
        __BSP_IS_FLAG_SET(ADC2->ISR, ADC_ISR_OVR)) {
        __badc_irq_handler(ADC2);
    }
}
#endif

#ifdef ADC3
static void __irq_handler_adc3(void)
{
    __badc_irq_handler(ADC3);
}
#endif

#ifdef ADC4
static void __irq_handler_adc4(void)
{
    __badc_irq_handler(ADC4);
}
#endif

#ifdef ADC5
static void __irq_handler_adc5(void)
{
    __badc_irq_handler(ADC5);
}
#endif

ret_status badc_config(badc_instance_t *adc, const badc_config_t *config)
{

    if (adc == NULL || config == NULL) {
        return STATUS_ERR;
    }

    if (config->mode == BADC_MODE_DISCONTINUOUS && config->discontinuous_channels > 8) {
        return STATUS_ERR;
    }

    const uint32_t tickstart = btick_get_ticks();

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
    cfgr_value |= config->dma_circular_mode ? ADC_CFGR_DMACFG : 0x00;

    __BSP_SET_MASKED_REG_VALUE(adc->CFGR,
                               ADC_CFGR_CONT | ADC_CFGR_OVRMOD | ADC_CFGR_ALIGN | ADC_CFGR_DISCEN | ADC_CFGR_DMACFG |
                                   ADC_CFGR_DISCNUM,
                               cfgr_value);

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
        status = butil_wait_flag_status_now(&adc->CR, ADC_CR_ADEN, ADC_CR_ADEN, 25u);
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
    return butil_wait_flag_status_now(&adc->CR, ADC_CR_ADDIS, 0U, 25u);
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
    return butil_wait_flag_status_now(&adc->CR, ADC_CR_ADCAL, 0U, 1000U);
}

ret_status badc_config_clk_source(badc_instance_t *adc, badc_clock_source_t clock_source)
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

    /* Caution, this register is cleared by writing ones */
    __BSP_SET_MASKED_REG(adc->ISR, ADC_ISR_EOC | ADC_ISR_EOS | ADC_ISR_OVR);

    __BSP_SET_MASKED_REG(adc->CR, ADC_CR_ADSTART);

    return STATUS_OK;
}

ret_status badc_wait_conversion(badc_instance_t *adc, uint32_t timeout)
{
    /* If ADC not enabled just return error */
    if (!__BSP_IS_FLAG_SET(adc->CR, ADC_CR_ADEN)) {
        return STATUS_ERR;
    }

    const ret_status status = butil_wait_flag_status_now(&adc->ISR, ADC_ISR_EOC, ADC_ISR_EOC, timeout);
    if (status == STATUS_OK) {
        __BSP_SET_MASKED_REG(adc->ISR, ADC_ISR_EOC);
    }
    return status;
}

uint16_t badc_get_conversion(badc_instance_t *adc)
{
    return adc->DR;
}

ret_status badc_enable_irqs(badc_instance_t *adc)
{

    if (adc == NULL) {
        return STATUS_ERR;
    }

#if defined(ADC5)
    if (adc == ADC5) {
        bool irq_enabled;
        BSP_IRQ_is_enabled(ADC5_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(ADC5_IRQn, __irq_handler_adc5);
            BSP_IRQ_enable_irq(ADC5_IRQn);
        }
    }
#endif

#if defined(ADC4)
    if (adc == ADC4) {
        bool irq_enabled;
        BSP_IRQ_is_enabled(ADC4_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(ADC4_IRQn, __irq_handler_adc4);
            BSP_IRQ_enable_irq(ADC4_IRQn);
        }
    }
#endif

#if defined(ADC3)
    if (adc == ADC3) {
        bool irq_enabled;
        BSP_IRQ_is_enabled(ADC3_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(ADC3_IRQn, __irq_handler_adc3);
            BSP_IRQ_enable_irq(ADC3_IRQn);
        }
    }
#endif

#if defined(ADC2)
    if (adc == ADC2) {
        bool irq_enabled;
        birq_is_enabled(ADC1_2_IRQn, &irq_enabled);
        if (!irq_enabled) {
            birq_set_handler(ADC1_2_IRQn, __irq_handler_adc12);
            birq_enable_irq_with_priority(
                ADC1_2_IRQn, BSP_IRQ_MANAGER_DEFAULT_PRIORITY, BSP_IRQ_MANAGER_DEFAULT_SUB_PRIORITY);
        }
    }
#endif

    if (adc == ADC1) {
        bool irq_enabled;
        birq_is_enabled(ADC1_2_IRQn, &irq_enabled);
        if (!irq_enabled) {
            birq_set_handler(ADC1_2_IRQn, __irq_handler_adc12);
            birq_enable_irq_with_priority(
                ADC1_2_IRQn, BSP_IRQ_MANAGER_DEFAULT_PRIORITY, BSP_IRQ_MANAGER_DEFAULT_SUB_PRIORITY);
        }
    }

    return STATUS_OK;
}

ret_status badc_config_irq(badc_instance_t *adc, badc_isr_type_t irq, badc_isr_handler_t handler)
{

    if (irq > ADC_ISR_JQOVF_Pos || handler == NULL || adc == NULL) {
        return STATUS_ERR;
    }

    struct __badc_irqs_state_s *instance_state = __badc_get_instance_state(adc);
    if (instance_state != NULL) {
        instance_state->isr_vectors[irq] = handler;
        __BSP_SET_MASKED_REG(adc->IER, (1U << irq));

        return STATUS_OK;
    }

    return STATUS_ERR;
}

ret_status badc_start_conversion_dma(
    badc_instance_t *adc, bdma_instance_t *dma, bdma_chan_t channel, uint8_t *data_address, uint16_t data_count)
{
    /* Cannot continue if conversion is ongoing or ADC is not enabled */
    if ((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART | ADC_CR_ADEN)) != ADC_CR_ADEN) {
        return STATUS_ERR;
    }

    /* Caution, this register is cleared by writing ones */
    __BSP_SET_MASKED_REG(adc->ISR, ADC_ISR_EOC | ADC_ISR_EOS | ADC_ISR_OVR);

    /* Enable DMA request generation */
    __BSP_SET_MASKED_REG(adc->CFGR, ADC_CFGR_DMAEN);

    ret_status status = bdma_enable_new_xfer(dma, channel, (uint8_t *)&adc->DR, data_address, data_count);
    if (status != STATUS_OK) {
        return status;
    }

    __BSP_SET_MASKED_REG(adc->CR, ADC_CR_ADSTART);
    return STATUS_OK;
}

static void __badc_config_channel_sampling_time(badc_instance_t *adc,
                                                uint8_t channel_number,
                                                badc_sampling_time_t sampling_time)
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

    ret_status tmp_status = butil_wait_flag_status(&adc->CR, disabled_bits, 0U, tick_start, 25u);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    __BSP_SET_MASKED_REG(adc->CR, ADC_CR_ADVREGEN);
    return butil_wait_flag_status(&adc->CR, ADC_CR_ADVREGEN, ADC_CR_ADVREGEN, tick_start, 25u);
}

static ret_status __badc_exit_power_down(badc_instance_t *adc, uint32_t tick_start)
{

    const uint32_t disabled_bits = ADC_CR_ADCAL | ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART | ADC_CR_ADSTART |
                                   ADC_CR_ADDIS | ADC_CR_ADEN;
    __BSP_CLEAR_MASKED_REG(adc->CR, disabled_bits);
    ret_status tmp_status = butil_wait_flag_status(&adc->CR, disabled_bits, 0U, tick_start, 25u);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    __BSP_CLEAR_MASKED_REG(adc->CR, ADC_CR_DEEPPWD);
    return butil_wait_flag_status(&adc->CR, ADC_CR_DEEPPWD, 0U, tick_start, 25u);
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

static inline struct __badc_irqs_state_s *__badc_get_instance_state(badc_instance_t *adc)
{
#if defined(ADC5)
    if (adc == ADC5) {
        return &__badc_internal_states[4U];
    }
#endif
#if defined(ADC4)
    if (adc == ADC4) {
        return &__badc_internal_states[3U];
    }
#endif
#if defined(ADC3)
    if (adc == ADC3) {
        return &__badc_internal_states[2U];
    }
#endif
#if defined(ADC2)
    if (adc == ADC2) {
        return &__badc_internal_states[1U];
    }
#endif
    if (adc == ADC1) {
        return &__badc_internal_states[0U];
    }
    return NULL;
}

static void __badc_irq_handler(badc_instance_t *adc)
{
    const struct __badc_irqs_state_s *adc_instance_state = __badc_get_instance_state(adc);
    if (adc_instance_state != NULL) {

        uint32_t isr_tmp = adc->ISR;

        /* Process all the ISR bits until no one continues flagged */
        while (isr_tmp != 0) {

            /* Get the fist non-zero bit index. As ISR is right aligned just assume that it's better to start from LSB
             */
            uint8_t isr_index = __builtin_ctz(isr_tmp);

            /* Clear the ADC interrupt flag by writing a 1 to the corresponding ISR bit */
            __BSP_SET_MASKED_REG(adc->ISR, (1 << isr_index));

            /* Call, if available, the registered callback */
            if (adc_instance_state->isr_vectors[isr_index] != NULL) {
                adc_instance_state->isr_vectors[isr_index](adc, adc->ISR);
            }
            /* Clean the ISR in our internal "ISR" register. In the next iteration (if isr_tmp is not empty) we'll
             * process the next ISR bit. ADCs merge all IRQs in their single instance line. */
            isr_tmp &= ~(1 << isr_index);
        }
    }
}