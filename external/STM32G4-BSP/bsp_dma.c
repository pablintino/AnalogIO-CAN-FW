/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#include "bsp_dma.h"
#include "bsp_common_utils.h"
#include "bsp_irq_manager.h"
#include "bsp_tick.h"
#include <stddef.h>

typedef DMAMUX_Channel_TypeDef bdma_dmamux_channel_t;

struct __bdma_channel_irqs_state_s {
    bdma_isr_handler_t error_handler;
    bdma_isr_handler_t half_xfer_handler;
    bdma_isr_handler_t xfer_complete_handler;
};

#if defined(DMA1_Channel7) | defined(DMA1_Channel8)
static struct __badc_irqs_state_s __bdma_channel_irqs_state[16U];
#else
static struct __bdma_channel_irqs_state_s __bdma_channel_irqs_state[12U];
#endif

#if defined(DMA1_Channel7) | defined(DMA1_Channel8)
#define __BDMA_DMUX_DMA_INSTANCE_CHAN_OFFSET 8UL
#else
#define __BDMA_DMUX_DMA_INSTANCE_CHAN_OFFSET 6UL
#endif

static void __bdma_irq_handler(bdma_instance_t *dma, bdma_channel_instance_t *chan);

static void __irq_handler_dma1_chan1(void)
{
    __bdma_irq_handler(DMA1, DMA1_Channel1);
}
static void __irq_handler_dma1_chan2(void)
{
    __bdma_irq_handler(DMA1, DMA1_Channel2);
}
static void __irq_handler_dma1_chan3(void)
{
    __bdma_irq_handler(DMA1, DMA1_Channel3);
}
static void __irq_handler_dma1_chan4(void)
{
    __bdma_irq_handler(DMA1, DMA1_Channel4);
}
static void __irq_handler_dma1_chan5(void)
{
    __bdma_irq_handler(DMA1, DMA1_Channel5);
}
static void __irq_handler_dma1_chan6(void)
{
    __bdma_irq_handler(DMA1, DMA1_Channel6);
}

#if defined(DMA1_Channel7)
static void __irq_handler_dma1_chan7(void)
{
    __bdma_irq_handler(DMA1, DMA1_Channel7);
}
#endif
#if defined(DMA1_Channel8)
static void __irq_handler_dma1_chan8(void)
{
    __bdma_irq_handler(DMA1, DMA1_Channel8);
}
#endif

static void __irq_handler_dma2_chan1(void)
{
    __bdma_irq_handler(DMA2, DMA2_Channel1);
}
static void __irq_handler_dma2_chan2(void)
{
    __bdma_irq_handler(DMA2, DMA2_Channel2);
}
static void __irq_handler_dma2_chan3(void)
{
    __bdma_irq_handler(DMA2, DMA2_Channel3);
}
static void __irq_handler_dma2_chan4(void)
{
    __bdma_irq_handler(DMA2, DMA2_Channel4);
}
static void __irq_handler_dma2_chan5(void)
{
    __bdma_irq_handler(DMA2, DMA2_Channel5);
}
static void __irq_handler_dma2_chan6(void)
{
    __bdma_irq_handler(DMA2, DMA2_Channel6);
}

#if defined(DMA2_Channel7)
static void __irq_handler_dma2_chan7(void)
{
    __bdma_irq_handler(DMA2, DMA2_Channel7);
}
#endif

#if defined(DMA2_Channel8)
static void __irq_handler_dma2_chan8(void)
{
    __bdma_irq_handler(DMA2, DMA2_Channel8);
}
#endif

static inline struct __bdma_channel_irqs_state_s *__bdma_get_chan_instance_state(const bdma_instance_t *dma,
                                                                                 bdma_chan_t channel);

static inline bdma_channel_instance_t *__get_channel_instance(const bdma_instance_t *dma, bdma_chan_t channel);

static inline bdma_dmamux_channel_t *__get_dmamux_channel(const bdma_instance_t *dma, bdma_chan_t channel);

static inline uint8_t __get_channel_index(bdma_chan_t channel);

static inline ret_status __enable_irq_for_channel(IRQn_Type irq, bsp_cmn_void_cb handler);

static inline ret_status __enable_channel_dma(bdma_instance_t *dma, bdma_channel_instance_t *channel_instance);

static inline uint8_t __get_channel_index_by_addr(bdma_instance_t *dma, bdma_channel_instance_t *channel_instance);

ret_status bdma_config(bdma_instance_t *dma, bdma_chan_t channel, const bdma_config_t *config)
{
    bdma_channel_instance_t *channel_instance = __get_channel_instance(dma, channel);

    /* Most of the flags cannot be changed while the channel is enabled */
    if (__BSP_IS_FLAG_SET(channel_instance->CCR, DMA_CCR_EN)) {
        return STATUS_ERR;
    }

    /* Set here the following bits:
     * DMA_CCR_CIRC: Circular mode ON/OFF
     * DMA_CCR_DIR and DMA_CCR_MEM2MEM: Peripheral read direction and memory to memory transfer indicator (zero means
     *                                  the transfer is from/to peripheral)
     * DMA_CCR_PSIZE: Size of the data to be written/read in the peripheral side
     * DMA_CCR_MSIZE: Size of the data to be written/read in the memory side
     * DMA_CCR_PINC: Increment the address direction where to transfer the next transfer in the peripheral side
     * DMA_CCR_MINC: Increment the address direction where to transfer the next transfer in the memory side
     * DMA_CCR_PL: Priority of the transfer
     */
    __BSP_SET_MASKED_REG_VALUE(
        channel_instance->CCR,
        DMA_CCR_CIRC | DMA_CCR_DIR | DMA_CCR_MEM2MEM | DMA_CCR_PSIZE | DMA_CCR_MSIZE | DMA_CCR_PINC | DMA_CCR_MINC |
            DMA_CCR_PL,
        config->direction | (config->circular_mode * DMA_CCR_CIRC) | (config->peripheral_size << DMA_CCR_PSIZE_Pos) |
            (config->memory_size << DMA_CCR_MSIZE_Pos) | (config->peripheral_increment * DMA_CCR_PINC) |
            (config->memory_increment * DMA_CCR_MINC) | config->priority);

    /* Configure XFER source/target addrs and length if given */
    if (config->source_addr != 0 && config->target_addr != 0 && channel_instance->CNDTR != 0) {
        if (config->direction == BDMA_XFER_DIR_P2M) {
            channel_instance->CPAR = (uint32_t)config->source_addr;
            channel_instance->CMAR = (uint32_t)config->target_addr;
        } else {
            /* Valid for M2M and M2P modes */
            channel_instance->CMAR = (uint32_t)config->source_addr;
            channel_instance->CPAR = (uint32_t)config->target_addr;
        }
        channel_instance->CNDTR = config->data_count;
    }

    /* Compute the pointer to the associated DMAMUX channel */
    bdma_dmamux_channel_t *dmamux_channel = __get_dmamux_channel(dma, channel);

    /* Set the request ID in the proper DMAMUX channel */
    __BSP_SET_MASKED_REG_VALUE(dmamux_channel->CCR, DMAMUX_CxCR_DMAREQ_ID, config->request & DMAMUX_CxCR_DMAREQ_ID);

    return STATUS_OK;
}

ret_status bdma_enable(bdma_instance_t *dma, bdma_chan_t channel)
{
    return __enable_channel_dma(dma, __get_channel_instance(dma, channel));
}

ret_status bdma_enable_new_xfer(
    bdma_instance_t *dma, bdma_chan_t channel, uint8_t *source_addr, uint8_t *target_addr, uint16_t data_count)
{
    bdma_channel_instance_t *channel_instance = __get_channel_instance(dma, channel);

    /* Cannot set addresses and length if the channel is already enabled */
    if (__BSP_IS_FLAG_SET(channel_instance->CCR, DMA_CCR_EN)) {
        __BSP_CLEAR_MASKED_REG(channel_instance->CCR, DMA_CCR_EN); // TODO REVIEW
    }

    if (!(channel_instance->CCR & (DMA_CCR_MEM2MEM | DMA_CCR_DIR))) {
        /* P2M xfer */
        channel_instance->CPAR = (uint32_t)source_addr;
        channel_instance->CMAR = (uint32_t)target_addr;
    } else {
        /* M2M or M2P xfer */
        channel_instance->CMAR = (uint32_t)source_addr;
        channel_instance->CPAR = (uint32_t)target_addr;
    }

    channel_instance->CNDTR = data_count;

    return __enable_channel_dma(dma, channel_instance);
}

ret_status bdma_enable_irq(const bdma_instance_t *dma, bdma_chan_t channel)
{
    if (dma == DMA1) {
        switch (channel) {
        case BDMA_CHANNEL_1:
            return __enable_irq_for_channel(DMA1_Channel1_IRQn, __irq_handler_dma1_chan1);
        case BDMA_CHANNEL_2:
            return __enable_irq_for_channel(DMA1_Channel2_IRQn, __irq_handler_dma1_chan2);
        case BDMA_CHANNEL_3:
            return __enable_irq_for_channel(DMA1_Channel3_IRQn, __irq_handler_dma1_chan3);
        case BDMA_CHANNEL_4:
            return __enable_irq_for_channel(DMA1_Channel4_IRQn, __irq_handler_dma1_chan4);
        case BDMA_CHANNEL_5:
            return __enable_irq_for_channel(DMA1_Channel5_IRQn, __irq_handler_dma1_chan5);
        case BDMA_CHANNEL_6:
            return __enable_irq_for_channel(DMA1_Channel6_IRQn, __irq_handler_dma1_chan6);
#if defined(BDMA_CHANNEL_7)
            return __enable_irq_for_channel(DMA1_Channel7_IRQn, __irq_handler_dma1_chan7);
#endif
#if defined(BDMA_CHANNEL_8)
            return __enable_irq_for_channel(DMA1_Channel8_IRQn, __irq_handler_dma1_chan8);
#endif
        default:
            return STATUS_ERR;
        }
    }

    if (dma == DMA2) {
        switch (channel) {
        case BDMA_CHANNEL_1:
            return __enable_irq_for_channel(DMA2_Channel1_IRQn, __irq_handler_dma2_chan1);
        case BDMA_CHANNEL_2:
            return __enable_irq_for_channel(DMA2_Channel2_IRQn, __irq_handler_dma2_chan2);
        case BDMA_CHANNEL_3:
            return __enable_irq_for_channel(DMA2_Channel3_IRQn, __irq_handler_dma2_chan3);
        case BDMA_CHANNEL_4:
            return __enable_irq_for_channel(DMA2_Channel4_IRQn, __irq_handler_dma2_chan4);
        case BDMA_CHANNEL_5:
            return __enable_irq_for_channel(DMA2_Channel5_IRQn, __irq_handler_dma2_chan5);
        case BDMA_CHANNEL_6:
            return __enable_irq_for_channel(DMA2_Channel6_IRQn, __irq_handler_dma2_chan6);
#if defined(BDMA_CHANNEL_7)
            return __enable_irq_for_channel(DMA2_Channel7_IRQn, __irq_handler_dma2_chan7);
#endif
#if defined(BDMA_CHANNEL_8)
            return __enable_irq_for_channel(DMA2_Channel8_IRQn, __irq_handler_dma2_chan8);
#endif
        default:
            return STATUS_ERR;
        }
    }

    return STATUS_ERR;
}

ret_status bdma_config_irq(const bdma_instance_t *dma,
                           bdma_chan_t channel,
                           bdma_isr_type_t isr,
                           bdma_isr_handler_t handler)
{
    bdma_channel_instance_t *channel_instance = __get_channel_instance(dma, channel);

    /* Interrupt flags cannot be changed while the channel is enabled */
    if (__BSP_IS_FLAG_SET(channel_instance->CCR, DMA_CCR_EN)) {
        return STATUS_ERR;
    }

    __BSP_SET_MASKED_REG(channel_instance->CCR, isr);
    struct __bdma_channel_irqs_state_s *channel_state = __bdma_get_chan_instance_state(dma, channel);
    switch (isr) {

    case BDMA_ISR_TYPE_XFER_COMPL:
        channel_state->xfer_complete_handler = handler;
        break;
    case BDMA_ISR_TYPE_HALF_XFER:
        channel_state->half_xfer_handler = handler;
        break;
    case BDMA_ISR_TYPE_XFER_ERROR:
        channel_state->error_handler = handler;
        break;
    default:
        return STATUS_ERR;
    }

    return STATUS_OK;
}

static inline bdma_channel_instance_t *__get_channel_instance(const bdma_instance_t *dma, bdma_chan_t channel)
{
    return (bdma_channel_instance_t *)((uint8_t *)dma + channel);
}

static inline bdma_dmamux_channel_t *__get_dmamux_channel(const bdma_instance_t *dma, bdma_chan_t channel)
{

    /* Calculate the DMAMUX channel index associated to the DMA instance and the selected DMA channel
     * Keep in mind that the association depends on the device category. Category 2 devices only can map 6 channels per
     * DMA instance.
     */
    const uint8_t dmamux_chan_index =
        (dma == DMA2 ? __BDMA_DMUX_DMA_INSTANCE_CHAN_OFFSET : 0) + __get_channel_index(channel);
    return (bdma_dmamux_channel_t *)(DMAMUX1_Channel0_BASE +
                                     dmamux_chan_index * (DMAMUX1_Channel1_BASE - DMAMUX1_Channel0_BASE));
}

static inline struct __bdma_channel_irqs_state_s *__bdma_get_chan_instance_state(const bdma_instance_t *dma,
                                                                                 bdma_chan_t channel)
{
    const uint8_t index =
        (dma == DMA2) * (sizeof(__bdma_channel_irqs_state) / sizeof(struct __bdma_channel_irqs_state_s)) / 2U +
        __get_channel_index(channel);
    return &__bdma_channel_irqs_state[index];
}

static inline uint8_t __get_channel_index(bdma_chan_t channel)
{
    /* Calculate the index (zero based) of the selected DMA channel */
    return (channel - BDMA_CHANNEL_1) / (BDMA_CHANNEL_2 - BDMA_CHANNEL_1);
}

static inline uint8_t __get_channel_index_by_addr(bdma_instance_t *dma, bdma_channel_instance_t *channel_instance)
{
    return dma == DMA1 ? ((uint32_t)channel_instance - DMA1_Channel1_BASE) / (DMA1_Channel2_BASE - DMA1_Channel1_BASE)
                       : ((uint32_t)channel_instance - DMA2_Channel1_BASE) / (DMA2_Channel2_BASE - DMA2_Channel1_BASE);
}

static inline ret_status __enable_irq_for_channel(IRQn_Type irq, bsp_cmn_void_cb handler)
{
    bool irq_enabled;
    birq_is_enabled(irq, &irq_enabled);
    if (!irq_enabled) {
        birq_set_handler(irq, handler);
        birq_enable_irq(irq);
        return STATUS_OK;
    }
    return STATUS_ERR;
}

static inline ret_status __enable_channel_dma(bdma_instance_t *dma, bdma_channel_instance_t *channel_instance)
{
    const uint8_t chan_index = __get_channel_index_by_addr(dma, channel_instance);
    /* Clear all flags before start */
    __BSP_SET_MASKED_REG(dma->IFCR, DMA_IFCR_CGIF1 << (4 * chan_index));

    __BSP_SET_MASKED_REG(channel_instance->CCR, DMA_CCR_EN);
    return butil_wait_flag_status_now(&channel_instance->CCR, DMA_CCR_EN, DMA_CCR_EN, 25u);
}

static void __bdma_irq_handler(bdma_instance_t *dma, bdma_channel_instance_t *chan)
{
    const uint8_t chan_index = __get_channel_index_by_addr(dma, chan);
    uint32_t isr_tmp = dma->ISR & ((DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_HTIF1 | DMA_ISR_TEIF1) << chan_index);

    /* Process all the ISR bits until no one continues flagged */
    while (isr_tmp != 0) {

        const uint8_t isr_index = 31 - __builtin_clz(isr_tmp);

        const struct __bdma_channel_irqs_state_s *chan_state = __bdma_get_chan_instance_state(dma, chan_index);

        /* Clear the channel IRQ by writing a one */
        __BSP_SET_MASKED_REG(dma->IFCR, (1 << isr_index));

        const uint8_t aligned_flag = 1 << (isr_index - 4U * chan_index);

        if ((aligned_flag & DMA_ISR_TCIF1) && chan_state->xfer_complete_handler != NULL) {
            chan_state->xfer_complete_handler(dma, chan, isr_tmp);
        }

        if ((aligned_flag & DMA_ISR_HTIF1) && chan_state->half_xfer_handler != NULL) {
            chan_state->half_xfer_handler(dma, chan, isr_tmp);
        }

        if ((aligned_flag & DMA_ISR_TEIF1) && chan_state->error_handler != NULL) {
            chan_state->error_handler(dma, chan, isr_tmp);
        }

        isr_tmp &= ~(1 << isr_index);
    }
}