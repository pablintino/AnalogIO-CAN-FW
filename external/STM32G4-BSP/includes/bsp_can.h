/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

/**
 * @file bsp_can.h
 * @brief FDCAN peripheral driver header file that exposes all public definitions and functions needed to operate with
 * the STM32G4XX advanced FDCAN peripheral.
 */
#ifndef BSP_CAN_H
#define BSP_CAN_H

#include "bsp_types.h"
#include "stm32g4xx.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum bcan_mode_source_e { BCAN_MODE_NORMAL = 0x00U, BCAN_MODE_BM = 0x01U } bcan_mode_source_t;

typedef enum bcan_tx_mode_e { BCAN_TX_MODE_FIFO = 0x00U, BCAN_TX_MODE_QUEUE = FDCAN_TXBC_TFQM } bcan_tx_mode_t;

typedef enum bcan_non_matching_filter_e {
    BCAN_NON_MATCHING_ACCEPT_RX_0 = 0x00U,
    BCAN_NON_MATCHING_ACCEPT_RX_1 = 0x01U,
    BCAN_NON_MATCHING_REJECT = 0x02U
} bcan_non_matching_filter_t;

typedef enum bcan_clock_source_e {
    BCAN_CLK_HSE = 0x00U,
    BCAN_CLK_PLLQ = 0x01U,
    BCAN_CLK_PCLK1 = 0x02U
} bcan_clock_source_t;

typedef enum bcan_rx_queue_e { BCAN_RX_QUEUE_O = 0x00U, BCAN_RX_QUEUE_1 = 0x01U } bcan_rx_queue_t;

typedef enum bcan_isr_line_e { BCAN_ISR_LINE_0 = 0x00U, BCAN_ISR_LINE_1 = 0x01U } bcan_isr_line_t;

typedef enum bcan_isr_group_e {
    BCAN_ISR_GROUP_RXFIFO0 = FDCAN_ILS_RXFIFO0,
    BCAN_ISR_GROUP_RXFIFO1 = FDCAN_ILS_RXFIFO1,
    BCAN_ISR_GROUP_SMSG = FDCAN_ILS_SMSG,
    BCAN_ISR_GROUP_TFERR = FDCAN_ILS_TFERR,
    BCAN_ISR_GROUP_MISC = FDCAN_ILS_MISC,
    BCAN_ISR_GROUP_BERR = FDCAN_ILS_BERR,
    BCAN_ISR_GROUP_PERR = FDCAN_ILS_PERR
} bcan_isr_group_t;

#define BSP_CAN_STD_FILTER_TYPE_Pos 30
/**
 * Enumeration that holds all the possible STD filter types.
 */
typedef enum bcan_standard_filter_type_e {
    /**
     * Filter matches if incoming ID is between bcan_standard_filter_t::standard_id1 and filter and
     * bcan_standard_filter_t::standard_id2.
     */
    BCAN_STD_FILTER_TYPE_RANGE = 0x00000000U,
    /**
     * Filter matches if incoming ID is one of bcan_standard_filter_t::standard_id1 or
     * bcan_standard_filter_t::standard_id2.
     */
    BCAN_STD_FILTER_TYPE_DUAL = 0x00000001U,
    /**
     * bcan_standard_filter_t::standard_id1 acts as filter and bcan_standard_filter_t::standard_id2 is the filter mask.
     */
    BCAN_STD_FILTER_TYPE_CLASSIC = 0x00000002U,
    /**
     * Filter is disabled.
     */
    BCAN_STD_FILTER_TYPE_DISABLED = 0x00000003U,
} bcan_standard_filter_type_t;

#define BSP_CAN_STD_FILTER_CONFIG_Pos 27
/**
 * Enumeration that holds all the possible values of a STD filter action.
 */
typedef enum bcan_std_filter_action_e {
    /**
     * Filter is disabled.
     */
    BCAN_STD_FILTER_ACTION_DISABLED = 0x00000000U,
    /**
     * Store matching element in RX 0 FIFO.
     */
    BCAN_STD_FILTER_ACTION_STORE_RX0 = 0x00000001U,
    /**
     * Store matching element in RX 1 FIFO.
     */
    BCAN_STD_FILTER_ACTION_STORE_RX1 = 0x00000002U,
    /**
     * Discard the matching element.
     */
    BCAN_STD_FILTER_ACTION_REJECT = 0x00000003U,
    /**
     * Matching element flags IR.HPM flag.
     */
    BCAN_STD_FILTER_ACTION_PRIORITIZE = 0x00000004U,
    /**
     * Store in RX FIFO 0 and flag IR.HPM if message matches.
     */
    BCAN_STD_FILTER_ACTION_PRIORITIZE_STORE_RX0 = 0x00000005U,
    /**
     * Store in RX 1 FIFO and flag IR.HPM if message matches.
     */
    BCAN_STD_FILTER_ACTION_PRIORITIZE_STORE_RX1 = 0x00000006U
} bcan_std_filter_action_t;

/**
 * Enumeration that holds of possible FDCAN interruption sources.
 */
typedef enum bcan_irq_type_e {
    /**
     * New message arrived at RX FIFO 0.
     */
    BCAN_IRQ_TYPE_RF0NE = FDCAN_IE_RF0NE_Pos,
    /**
     * RX FIFO 0 is full.
     */
    BCAN_IRQ_TYPE_RF0FE = FDCAN_IE_RF0FE_Pos,
    /**
     * RX FIFO 0 has lost a message.
     */
    BCAN_IRQ_TYPE_RF0LE = FDCAN_IE_RF0LE_Pos,
    /**
     * New message arrived at RX FIFO 1.
     */
    BCAN_IRQ_TYPE_RF1NE = FDCAN_IE_RF1NE_Pos,
    /**
     * RX FIFO 1 is full.
     */
    BCAN_IRQ_TYPE_RF1FE = FDCAN_IE_RF1FE_Pos,
    /**
     * RX FIFO 1 has lost a message.
     */
    BCAN_IRQ_TYPE_RF1LE = FDCAN_IE_RF1LE_Pos,
    /**
     * High-priority message received.
     */
    BCAN_IRQ_TYPE_HPME = FDCAN_IE_HPME_Pos,
    /**
     * Transmission completed.
     */
    BCAN_IRQ_TYPE_TCE = FDCAN_IE_TCE_Pos,
    /**
     * Transmission cancellation finished.
     */
    BCAN_IRQ_TYPE_TCFE = FDCAN_IE_TCFE_Pos,
    /**
     * TX FIFO empty.
     */
    BCAN_IRQ_TYPE_TFEE = FDCAN_IE_TFEE_Pos,
    /**
     * New TX event element.
     */
    BCAN_IRQ_TYPE_TEFNE = FDCAN_IE_TEFNE_Pos,
    /**
     * TX Event FIFO full.
     */
    BCAN_IRQ_TYPE_TEFFE = FDCAN_IE_TEFFE_Pos,
    /**
     * TX Event FIFO element lost.
     */
    BCAN_IRQ_TYPE_TEFLE = FDCAN_IE_TEFLE_Pos,
    /**
     * Timestamp counter wrapped around.
     */
    BCAN_IRQ_TYPE_TSWE = FDCAN_IE_TSWE_Pos,
    /**
     * Message RAM access failure.
     */
    BCAN_IRQ_TYPE_MRAFE = FDCAN_IE_MRAFE_Pos,
    /**
     * Timeout occurred.
     */
    BCAN_IRQ_TYPE_TOOE = FDCAN_IE_TOOE_Pos,
    /**
     * Overflow of CAN error logging counter occurred.
     */
    BCAN_IRQ_TYPE_ELOE = FDCAN_IE_ELOE_Pos,
    /**
     * Error passive status changed.
     */
    BCAN_IRQ_TYPE_EPE = FDCAN_IE_EPE_Pos,
    /**
     * Warning status changed.
     */
    BCAN_IRQ_TYPE_EWE = FDCAN_IE_EWE_Pos,
    /**
     * Bus-Off status changed.
     */
    BCAN_IRQ_TYPE_BOE = FDCAN_IE_BOE_Pos,
    /**
     * Message RAM Watchdog.
     */
    BCAN_IRQ_TYPE_WDIE = FDCAN_IE_WDIE_Pos,
    /**
     * Protocol error in arbitration phase.
     */
    BCAN_IRQ_TYPE_PEAE = FDCAN_IE_PEAE_Pos,
    /**
     * Protocol error in data phase.
     */
    BCAN_IRQ_TYPE_PEDE = FDCAN_IE_PEDE_Pos,
    /**
     * Access to reserved memory occurred.
     */
    BCAN_IRQ_TYPE_ARAE = FDCAN_IE_ARAE_Pos
} bcan_irq_type_t;

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

/**
 * Struct that describes all the time related values of the FDCAN peripheral.
 *
 * These values are all 1-based ones (no zero-based).
 * FDCAN timing calculation are all based on bcan_config_timing_t::phase1, bcan_config_timing_t::phase2 and
 * bcan_config_timing_t::prescaler. To obtain more information about this three values check the individual field
 * documentation or check chapter 44.3.1 of the RM0440.
 *
 */
typedef struct bcan_config_timing_t {
    /**
     * Bit time before the sample point.
     */
    uint8_t phase1;
    /**
     * Bit time after the sample point.
     */
    uint8_t phase2;
    /**
     * Maximum resynchronization jump width.
     */
    uint8_t sync_jump_width;
    /**
     * FDCAN instance clock prescaler.
     */
    uint16_t prescaler;
} bcan_config_timing_t;

typedef struct bcan_config_t {
    bcan_config_timing_t timing;
    bool auto_retransmission;
    bcan_mode_source_t mode;
    bcan_tx_mode_t tx_mode;
    bcan_config_global_filters_t global_filters;
} bcan_config_t;

typedef FDCAN_GlobalTypeDef bcan_instance_t;

typedef void (*bcan_isr_handler)(bcan_instance_t *can, uint32_t group_flags);

ret_status bcan_config(bcan_instance_t *can, const bcan_config_t *config);

ret_status bcan_config_clk_source(bcan_clock_source_t clock_source);

ret_status bcan_config_irq(bcan_instance_t *can, bcan_irq_type_t irq, bcan_isr_handler handler);

ret_status bcan_config_irq_line(bcan_instance_t *can, bcan_isr_group_t isr_group, bcan_isr_line_t isr_line);

ret_status bcan_enable_irqs(bcan_instance_t *can);

ret_status bcan_start(bcan_instance_t *can);

ret_status bcan_add_standard_filter(bcan_instance_t *can, const bcan_standard_filter_t *filter, uint8_t index);

ret_status bcan_add_tx_message(bcan_instance_t *can, const bcan_tx_metadata_t *tx_metadata, const uint8_t *tx_data);

ret_status bcan_get_rx_message(bcan_instance_t *can,
                               bcan_rx_queue_t queue,
                               bcan_rx_metadata_t *rx_metadata,
                               uint8_t *rx_data);

ret_status bcan_get_baudrate(bcan_instance_t *can, uint32_t *baudrate);

#endif // BSP_CAN_H
