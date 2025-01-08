/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, October 2021
 */

#ifndef BSP_DMA_H
#define BSP_DMA_H

#include "bsp_types.h"
#include "stm32g4xx.h"
#include <stdbool.h>

typedef enum bdma_rqst_id_s {
    BDMA_REQ_ID_MEM2MEM = 0U,
    BDMA_REQ_ID_GENERATOR0 = 1U,
    BDMA_REQ_ID_GENERATOR1 = 2U,
    BDMA_REQ_ID_GENERATOR2 = 3U,
    BDMA_REQ_ID_GENERATOR3 = 4U,
    BDMA_REQ_ID_ADC1 = 5U,
    BDMA_REQ_ID_DAC1_CHANNEL1 = 6U,
    BDMA_REQ_ID_DAC1_CHANNEL2 = 7U,
    BDMA_REQ_ID_TIM6_UP = 8U,
    BDMA_REQ_ID_TIM7_UP = 9U,
    BDMA_REQ_ID_SPI1_RX = 10U,
    BDMA_REQ_ID_SPI1_TX = 11U,
    BDMA_REQ_ID_SPI2_RX = 12U,
    BDMA_REQ_ID_SPI2_TX = 13U,
    BDMA_REQ_ID_SPI3_RX = 14U,
    BDMA_REQ_ID_SPI3_TX = 15U,
    BDMA_REQ_ID_I2C1_RX = 16U,
    BDMA_REQ_ID_I2C1_TX = 17U,
    BDMA_REQ_ID_I2C2_RX = 18U,
    BDMA_REQ_ID_I2C2_TX = 19U,
    BDMA_REQ_ID_I2C3_RX = 20U,
    BDMA_REQ_ID_I2C3_TX = 21U,
#if defined(I2C4)
    BDMA_REQ_ID_I2C4_RX = 22U,
    BDMA_REQ_ID_I2C4_TX = 23U,
#endif
    BDMA_REQ_ID_USART1_RX = 24U,
    BDMA_REQ_ID_USART1_TX = 25U,
    BDMA_REQ_ID_USART2_RX = 26U,
    BDMA_REQ_ID_USART2_TX = 27U,
    BDMA_REQ_ID_USART3_RX = 28U,
    BDMA_REQ_ID_USART3_TX = 29U,
    BDMA_REQ_ID_UART4_RX = 30U,
    BDMA_REQ_ID_UART4_TX = 31U,
#if defined(UART5)
    BDMA_REQ_ID_UART5_RX = 32U,
    BDMA_REQ_ID_UART5_TX = 33U,
#endif
    BDMA_REQ_ID_LPUART1_RX = 34U,
    BDMA_REQ_ID_LPUART1_TX = 35U,
    BDMA_REQ_ID_ADC2 = 36U,
#if defined(ADC3)
    BDMA_REQ_ID_ADC3 = 37U,
#endif
#if defined(ADC4)
    BDMA_REQ_ID_ADC4 = 38U,
#endif
#if defined(ADC5)
    BDMA_REQ_ID_ADC5 = 39U,
#endif

#if defined(QUADSPI)
    BDMA_REQ_ID_QUADSPI = 40U,
#endif

#if defined(DAC2)
    BDMA_REQ_ID_DAC2_CHANNEL1 = 41U,
#endif
    BDMA_REQ_ID_TIM1_CH1 = 42U,
    BDMA_REQ_ID_TIM1_CH2 = 43U,
    BDMA_REQ_ID_TIM1_CH3 = 44U,
    BDMA_REQ_ID_TIM1_CH4 = 45U,
    BDMA_REQ_ID_TIM1_UP = 46U,
    BDMA_REQ_ID_TIM1_TRIG = 47U,
    BDMA_REQ_ID_TIM1_COM = 48U,
    BDMA_REQ_ID_TIM8_CH1 = 49U,
    BDMA_REQ_ID_TIM8_CH2 = 50U,
    BDMA_REQ_ID_TIM8_CH3 = 51U,
    BDMA_REQ_ID_TIM8_CH4 = 52U,
    BDMA_REQ_ID_TIM8_UP = 53U,
    BDMA_REQ_ID_TIM8_TRIG = 54U,
    BDMA_REQ_ID_TIM8_COM = 55U,
    BDMA_REQ_ID_TIM2_CH1 = 56U,
    BDMA_REQ_ID_TIM2_CH2 = 57U,
    BDMA_REQ_ID_TIM2_CH3 = 58U,
    BDMA_REQ_ID_TIM2_CH4 = 59U,
    BDMA_REQ_ID_TIM2_UP = 60U,
    BDMA_REQ_ID_TIM3_CH1 = 61U,
    BDMA_REQ_ID_TIM3_CH2 = 62U,
    BDMA_REQ_ID_TIM3_CH3 = 63U,
    BDMA_REQ_ID_TIM3_CH4 = 64U,
    BDMA_REQ_ID_TIM3_UP = 65U,
    BDMA_REQ_ID_TIM3_TRIG = 66U,
    BDMA_REQ_ID_TIM4_CH1 = 67U,
    BDMA_REQ_ID_TIM4_CH2 = 68U,
    BDMA_REQ_ID_TIM4_CH3 = 69U,
    BDMA_REQ_ID_TIM4_CH4 = 70U,
    BDMA_REQ_ID_TIM4_UP = 71U,
#if defined(TIM5)
    BDMA_REQ_ID_TIM5_CH1 = 72U,
    BDMA_REQ_ID_TIM5_CH2 = 73U,
    BDMA_REQ_ID_TIM5_CH3 = 74U,
    BDMA_REQ_ID_TIM5_CH4 = 75U,
    BDMA_REQ_ID_TIM5_UP = 76U,
    BDMA_REQ_ID_TIM5_TRIG = 77U,
#endif
    BDMA_REQ_ID_TIM15_CH1 = 78U,
    BDMA_REQ_ID_TIM15_UP = 79U,
    BDMA_REQ_ID_TIM15_TRIG = 80U,
    BDMA_REQ_ID_TIM15_COM = 81U,
    BDMA_REQ_ID_TIM16_CH1 = 82U,
    BDMA_REQ_ID_TIM16_UP = 83U,
    BDMA_REQ_ID_TIM17_CH1 = 84U,
    BDMA_REQ_ID_TIM17_UP = 85U,
#if defined(TIM20)
    BDMA_REQ_ID_TIM20_CH1 = 86U,
    BDMA_REQ_ID_TIM20_CH2 = 87U,
    BDMA_REQ_ID_TIM20_CH3 = 88U,
    BDMA_REQ_ID_TIM20_CH4 = 89U,
    BDMA_REQ_ID_TIM20_UP = 90U,
#endif
    BDMA_REQ_ID_AES_IN = 91U,
    BDMA_REQ_ID_AES_OUT = 92U,
#if defined(TIM20)
    BDMA_REQ_ID_TIM20_TRIG = 93U,
    BDMA_REQ_ID_TIM20_COM = 94U,
#endif
#if defined(HRTIM1)
    BDMA_REQ_ID_HRTIM1_M = 95U,
    BDMA_REQ_ID_HRTIM1_A = 96U,
    BDMA_REQ_ID_HRTIM1_B = 97U,
    BDMA_REQ_ID_HRTIM1_C = 98U,
    BDMA_REQ_ID_HRTIM1_D = 99U,
    BDMA_REQ_ID_HRTIM1_E = 100U,
    BDMA_REQ_ID_HRTIM1_F = 101U,
#endif
    BDMA_REQ_ID_DAC3_CHANNEL1 = 102U,
    BDMA_REQ_ID_DAC3_CHANNEL2 = 103U,
#if defined(DAC4)
    BDMA_REQ_ID_DAC4_CHANNEL1 = 104U,
    BDMA_REQ_ID_DAC4_CHANNEL2 = 105U,
#endif
#if defined(SPI4)
    BDMA_REQ_ID_SPI4_RX = 106U,
    BDMA_REQ_ID_SPI4_TX = 107U,
#endif
    BDMA_REQ_ID_SAI1_A = 108U,
    BDMA_REQ_ID_SAI1_B = 109U,
    BDMA_REQ_ID_FMAC_READ = 110U,
    BDMA_REQ_ID_FMAC_WRITE = 111U,
    BDMA_REQ_ID_CORDIC_READ = 112U,
    BDMA_REQ_ID_CORDIC_WRITE = 113U,
    BDMA_REQ_ID_UCPD1_RX = 114U,
    BDMA_REQ_ID_UCPD1_TX = 115U
} bdma_rqst_id_t;

typedef enum bdma_chan_s {
    BDMA_CHANNEL_1 = 0x0008UL,
    BDMA_CHANNEL_2 = 0x001CUL,
    BDMA_CHANNEL_3 = 0x0030UL,
    BDMA_CHANNEL_4 = 0x0044UL,
    BDMA_CHANNEL_5 = 0x0058UL,
    BDMA_CHANNEL_6 = 0x006CUL,
#if defined(DMA1_Channel7)
    BDMA_CHANNEL_7 = 0x0080UL,
#endif
#if defined(DMA1_Channel8)
    BDMA_CHANNEL_8 = 0x0094UL
#endif
} bdma_chan_t;

typedef enum bdma_xfer_data_size_e {
    BDMA_XFER_SIZE_8 = 0x0000U,
    BDMA_XFER_SIZE_16 = 0x0001U,
    BDMA_XFER_SIZE_32 = 0x0002U
} bdma_xfer_data_size_t;

typedef enum bdma_xfer_direction_e {
    BDMA_XFER_DIR_P2M = 0x0000U,
    BDMA_XFER_DIR_M2P = DMA_CCR_DIR,
    BDMA_XFER_DIR_M2M = DMA_CCR_MEM2MEM
} bdma_xfer_direction_t;

typedef enum bdma_channel_prio_e {
    BDMA_CHAN_PRIO_LOW = 0x0000U,
    BDMA_CHAN_PRIO_MED = DMA_CCR_PL_0,
    BDMA_CHAN_PRIO_HI = DMA_CCR_PL_1,
    BDMA_CHAN_PRIO_VHI = DMA_CCR_PL_Msk
} bdma_channel_prio_t;

typedef enum bdma_isr_type_e {
    BDMA_ISR_TYPE_XFER_COMPL = DMA_CCR_TCIE,
    BDMA_ISR_TYPE_HALF_XFER = DMA_CCR_HTIE,
    BDMA_ISR_TYPE_XFER_ERROR = DMA_CCR_TEIE
} bdma_isr_type_t;

typedef struct bdma_config_s {
    bool circular_mode;
    bdma_xfer_direction_t direction;
    bdma_xfer_data_size_t peripheral_size;
    bdma_xfer_data_size_t memory_size;
    bool peripheral_increment;
    bool memory_increment;
    bdma_channel_prio_t priority;
    bdma_rqst_id_t request;
    uint8_t *source_addr;
    uint8_t *target_addr;
    uint16_t data_count;
} bdma_config_t;

typedef DMA_TypeDef bdma_instance_t;
typedef DMA_Channel_TypeDef bdma_channel_instance_t;

typedef void (*bdma_isr_handler_t)(bdma_instance_t *dma, bdma_channel_instance_t *channel, uint32_t group_flags);

ret_status bdma_config(bdma_instance_t *dma, bdma_chan_t channel, const bdma_config_t *config);

ret_status bdma_enable(bdma_instance_t *dma, bdma_chan_t channel);

ret_status bdma_enable_new_xfer(
    bdma_instance_t *dma, bdma_chan_t channel, uint8_t *source_addr, uint8_t *target_addr, uint16_t data_count);

ret_status bdma_enable_irq(const bdma_instance_t *dma, bdma_chan_t channel);

ret_status bdma_config_irq(const bdma_instance_t *dma,
                           bdma_chan_t channel,
                           bdma_isr_type_t isr,
                           bdma_isr_handler_t handler);

#endif // BSP_DMA_H
