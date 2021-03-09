/**
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
 **/


#ifndef BSP_CAN_H
#define BSP_CAN_H


#include <stddef.h>
#include <stdbool.h>
#include "bsp_types.h"
#include "stm32g4xx.h"


enum bcan_mode_source_e {
    BSP_CAN_MODE_NORMAL = 0x00U,
    BSP_CAN_MODE_BM = 0x01U
};


enum bcan_tx_mode_e {
    BSP_CAN_TX_MODE_FIFO = 0x00U,
    BSP_CAN_TX_MODE_QUEUE = FDCAN_TXBC_TFQM
};


enum bcan_non_matching_filter_e {
    BSP_CAN_NON_MATCHING_ACCEPT_RX_0 = 0x00U,
    BSP_CAN_NON_MATCHING_ACCEPT_RX_1 = 0x01U,
    BSP_CAN_NON_MATCHING_REJECT = 0x02U
};


enum bcan_clock_source_e {
    BSP_CAN_CLK_HSE = 0x00U,
    BSP_CAN_CLK_PLLQ = 0x01U,
    BSP_CAN_CLK_PCLK1 = 0x02U
};


enum bcan_rx_queue_e {
    BSP_CAN_RX_QUEUE_O = 0x00U,
    BSP_CAN_RX_QUEUE_1 = 0x01U
};


enum bcan_isr_line_e {
    BSP_CAN_ISR_LINE_0 = 0x00U,
    BSP_CAN_ISR_LINE_1 = 0x01U
};


enum bcan_isr_group_e {
    BSP_CAN_ISR_GROUP_RXFIFO0 = FDCAN_ILS_RXFIFO0,
    BSP_CAN_ISR_GROUP_RXFIFO1 = FDCAN_ILS_RXFIFO1,
    BSP_CAN_ISR_GROUP_SMSG = FDCAN_ILS_SMSG,
    BSP_CAN_ISR_GROUP_TFERR = FDCAN_ILS_TFERR,
    BSP_CAN_ISR_GROUP_MISC = FDCAN_ILS_MISC,
    BSP_CAN_ISR_GROUP_BERR = FDCAN_ILS_BERR,
    BSP_CAN_ISR_GROUP_PERR = FDCAN_ILS_PERR
};


#define BSP_CAN_STD_FILTER_TYPE_Pos 30
enum bcan_standard_filter_type_e {
    BSP_CAN_STD_FILTER_TYPE_RANGE = 0x00000000U,
    BSP_CAN_STD_FILTER_TYPE_DUAL = 0x00000001U,
    BSP_CAN_STD_FILTER_TYPE_CLASSIC = 0x00000002U,
    BSP_CAN_STD_FILTER_TYPE_DISABLED = 0x00000003U,
};


#define BSP_CAN_STD_FILTER_CONFIG_Pos 27
enum bcan_std_filter_action_e {
    BSP_CAN_STD_FILTER_ACTION_DISABLED = 0x00000000U,
    BSP_CAN_STD_FILTER_ACTION_STORE_RX0 = 0x00000001U,
    BSP_CAN_STD_FILTER_ACTION_STORE_RX1 = 0x00000002U,
    BSP_CAN_STD_FILTER_ACTION_REJECT = 0x00000003U,
    BSP_CAN_STD_FILTER_ACTION_PRIORITIZE = 0x00000004U,
    BSP_CAN_STD_FILTER_ACTION_PRIORITIZE_STORE_RX0 = 0x00000005U,
    BSP_CAN_STD_FILTER_ACTION_PRIORITIZE_STORE_RX1 = 0x00000006U
};


enum bcan_irq_type_e {
    BSP_CAN_IRQ_TYPE_RF0NE = FDCAN_IE_RF0NE_Pos,
    BSP_CAN_IRQ_TYPE_RF0FE = FDCAN_IE_RF0FE_Pos,
    BSP_CAN_IRQ_TYPE_RF0LE = FDCAN_IE_RF0LE_Pos,
    BSP_CAN_IRQ_TYPE_RF1NE = FDCAN_IE_RF1NE_Pos,
    BSP_CAN_IRQ_TYPE_RF1FE = FDCAN_IE_RF1FE_Pos,
    BSP_CAN_IRQ_TYPE_RF1LE = FDCAN_IE_RF1LE_Pos,
    BSP_CAN_IRQ_TYPE_HPME = FDCAN_IE_HPME_Pos,
    BSP_CAN_IRQ_TYPE_TCE = FDCAN_IE_TCE_Pos,
    BSP_CAN_IRQ_TYPE_TCFE = FDCAN_IE_TCFE_Pos,
    BSP_CAN_IRQ_TYPE_TFEE = FDCAN_IE_TFEE_Pos,
    BSP_CAN_IRQ_TYPE_TEFNE = FDCAN_IE_TEFNE_Pos,
    BSP_CAN_IRQ_TYPE_TEFFE = FDCAN_IE_TEFFE_Pos,
    BSP_CAN_IRQ_TYPE_TEFLE = FDCAN_IE_TEFLE_Pos,
    BSP_CAN_IRQ_TYPE_TSWE = FDCAN_IE_TSWE_Pos,
    BSP_CAN_IRQ_TYPE_MRAFE = FDCAN_IE_MRAFE_Pos,
    BSP_CAN_IRQ_TYPE_TOOE = FDCAN_IE_TOOE_Pos,
    BSP_CAN_IRQ_TYPE_ELOE = FDCAN_IE_ELOE_Pos,
    BSP_CAN_IRQ_TYPE_EPE = FDCAN_IE_EPE_Pos,
    BSP_CAN_IRQ_TYPE_EWE = FDCAN_IE_EWE_Pos,
    BSP_CAN_IRQ_TYPE_BOE = FDCAN_IE_BOE_Pos,
    BSP_CAN_IRQ_TYPE_WDIE = FDCAN_IE_WDIE_Pos,
    BSP_CAN_IRQ_TYPE_PEAE = FDCAN_IE_PEAE_Pos,
    BSP_CAN_IRQ_TYPE_PEDE = FDCAN_IE_PEDE_Pos,
    BSP_CAN_IRQ_TYPE_ARAE = FDCAN_IE_ARAE_Pos
};


typedef struct bcan_config_global_filters_t {
    bool reject_remote_standard;
    enum bcan_non_matching_filter_e non_matching_standard_action;
} bcan_config_global_filters_t;


typedef struct bcan_tx_metadata_t {
    uint32_t id;
    bool is_rtr;
    uint32_t size_b;
    bool store_tx_events;
    uint32_t message_marker;
} bcan_tx_metadata_t;


typedef struct bcan_rx_metadata_t {
    uint32_t id;
    bool is_rtr;
    uint32_t size_b;
    uint8_t timestamp;
    uint8_t matched_filter_index;
    bool non_matching_element;
} bcan_rx_metadata_t;


typedef struct bcan_standard_filter_t {
    enum bcan_standard_filter_type_e type;
    enum bcan_std_filter_action_e action;
    uint16_t standard_id1;
    uint16_t standard_id2;
} bcan_standard_filter_t;


typedef struct bcan_config_timing_t {
    uint8_t phase1;
    uint8_t phase2;
    uint8_t sync_jump_width;
    uint16_t prescaler;
} bcan_config_timing_t;


typedef struct bcan_config_t {
    bcan_config_timing_t timing;
    bool auto_retransmission;
    enum bcan_mode_source_e mode;
    enum bcan_tx_mode_e tx_mode;
    bcan_config_global_filters_t global_filters;
} bcan_config_t;


typedef FDCAN_GlobalTypeDef bcan_instance_t;

typedef void (*bcan_isr_handler)(bcan_instance_t *can, uint32_t group_flags);

ret_status bcan_config(bcan_instance_t *can, const bcan_config_t *config);

ret_status bcan_config_clk_source(bcan_instance_t *can, enum bcan_clock_source_e clock_source);

ret_status bcan_config_irq(bcan_instance_t *can, enum bcan_irq_type_e irq, bcan_isr_handler handler);

ret_status bcan_config_irq_line(bcan_instance_t *can, enum bcan_isr_group_e isr_group, enum bcan_isr_line_e isr_line);

ret_status bcan_enable_irqs(bcan_instance_t *can);

ret_status bcan_start(bcan_instance_t *can);

ret_status bcan_add_standard_filter(bcan_instance_t *can, const bcan_standard_filter_t *filter, uint8_t index);

ret_status bcan_add_tx_message(bcan_instance_t *can, const bcan_tx_metadata_t *tx_metadata, const uint8_t *tx_data);

ret_status bcan_get_rx_message(bcan_instance_t *can, enum bcan_rx_queue_e queue,
                               bcan_rx_metadata_t *rx_metadata, uint8_t *rx_data);

ret_status bcan_get_baudrate(bcan_instance_t *can, uint32_t *baudrate);


#endif //BSP_CAN_H
