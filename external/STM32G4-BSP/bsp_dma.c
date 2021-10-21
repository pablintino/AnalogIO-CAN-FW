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

typedef DMA_Channel_TypeDef bdma_channel_instance_t;
typedef DMAMUX_Channel_TypeDef bdma_dmamux_channel_t;

static inline bdma_channel_instance_t *__get_channel_instance(const bdma_instance_t *dma, bdma_chan_t channel)
{
    return (bdma_channel_instance_t *)((uint8_t *)dma + channel);
}

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
    __BSP_SET_MASKED_REG_VALUE(channel_instance->CCR,
                               DMA_CCR_CIRC | DMA_CCR_DIR | DMA_CCR_MEM2MEM | DMA_CCR_PSIZE | DMA_CCR_MSIZE |
                                   DMA_CCR_PINC | DMA_CCR_MINC | DMA_CCR_PL,
                               config->direction | (config->circular_mode * DMA_CCR_CIRC) |
                                   (config->peripheral_size << DMA_CCR_PSIZE_Pos) |
                                   (config->memory_size << DMA_CCR_MSIZE_Pos) |
                                   (config->peripheral_increment * DMA_CCR_PINC) | config->priority);

    uint8_t chan_index = (uint8_t)(((uint32_t)channel_instance - (uint32_t)DMA1_Channel1_BASE) / 0x0014UL);
    bdma_dmamux_channel_t *dmamux_channel =
        (bdma_dmamux_channel_t *)((uint32_t)DMA1_Channel1_BASE + ((uint32_t)chan_index) * 0x0004UL);

    /* Set the request ID in the proper DMAMUX channel */
    __BSP_SET_MASKED_REG_VALUE(dmamux_channel->CCR, DMAMUX_CxCR_DMAREQ_ID, config->request & DMAMUX_CxCR_DMAREQ_ID);
}