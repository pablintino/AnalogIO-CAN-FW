/**
 * MIT License
 *
 * Copyright (c) 2020 Pablo Rodriguez Nava, @pablintino
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



typedef enum {
    BSP_CAN_MODE_NORMAL = 0x00U,
    BSP_CAN_MODE_BM = 0x01U
} bsp_can_mode_source;

typedef enum {
    BSP_CAN_TX_MODE_FIFO = 0x00U,
    BSP_CAN_TX_MODE_QUEUE = FDCAN_TXBC_TFQM
} bsp_can_tx_mode;

typedef enum {
    BSP_CAN_NON_MATCHING_ACCEPT_RX_0 = 0x00U,
    BSP_CAN_NON_MATCHING_ACCEPT_RX_1 = 0x01U,
    BSP_CAN_NON_MATCHING_REJECT = 0x02U
} bsp_can_non_matching_filter_type;

typedef enum {
    BSP_CAN_CLK_HSE = 0x00U,
    BSP_CAN_CLK_PLLQ = 0x01U,
    BSP_CAN_CLK_PCLK1 = 0x02U
} bsp_can_clock_source;

typedef enum {
    BSP_CAN_RX_QUEUE_O = 0x00U,
    BSP_CAN_RX_QUEUE_1 = 0x01U
} bsp_can_rx_queue;

typedef struct {
    bool RejectRemoteStandard;
    bsp_can_non_matching_filter_type NonMatchingStandard;
} bsp_can_rx_global_filtering;

typedef struct
{
    uint32_t ID;
    bool IsRTR;
    uint32_t DataLength;
    bool StoreTxEvents;
    uint32_t MessageMarker;
} bsp_can_tx_metadata;

typedef struct
{
    uint32_t ID;
    bool IsRTR;
    uint32_t DataLength;
    uint8_t Timestamp;
    uint8_t MatchedFilterIndex;
    bool NonMatchingElement;
} bsp_can_rx_metadata_t;

#define BSP_CAN_STD_FILTER_TYPE_Pos 30
typedef enum{
    BSP_CAN_STD_FILTER_TYPE_RANGE = 0x00000000U,
    BSP_CAN_STD_FILTER_TYPE_DUAL = 0x00000001U,
    BSP_CAN_STD_FILTER_TYPE_CLASSIC = 0x00000002U,
    BSP_CAN_STD_FILTER_TYPE_DISABLED = 0x00000003U,
}bsp_can_standard_filter_type;


#define BSP_CAN_STD_FILTER_CONFIG_Pos 27
typedef enum{
    BSP_CAN_STD_FILTER_CONFIG_DISABLED = 0x00000000U,
    BSP_CAN_STD_FILTER_CONFIG_STORE_RX0 = 0x00000001U,
    BSP_CAN_STD_FILTER_CONFIG_STORE_RX1 = 0x00000002U,
    BSP_CAN_STD_FILTER_CONFIG_REJECT = 0x00000003U,
    BSP_CAN_STD_FILTER_CONFIG_PRIORITIZE = 0x00000004U,
    BSP_CAN_STD_FILTER_CONFIG_PRIORITIZE_STORE_RX0 = 0x00000005U,
    BSP_CAN_STD_FILTER_CONFIG_PRIORITIZE_STORE_RX1 = 0x00000006U
}bsp_can_standard_filter_config;


typedef enum{
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

}bsp_can_irq_type;


typedef struct
{
    bsp_can_standard_filter_type Type;
    bsp_can_standard_filter_config Config;
    uint16_t StandardID1;
    uint16_t StandardID2;

} bsp_can_standard_filter_t;

typedef struct {
    uint8_t Phase1T;
    uint8_t Phase2T;
    uint8_t SyncJumpWidth;
    uint16_t Prescaler;
} bsp_i2c_can_timming_config_t;

typedef struct {
    bsp_i2c_can_timming_config_t Timing;
    bool EnableAutoretransmision;
    bsp_can_mode_source InterfaceMode;
    bsp_can_tx_mode TXMode;
    bsp_can_rx_global_filtering GlobalFiltering;
} bsp_can_config_t;

typedef FDCAN_GlobalTypeDef BSP_CAN_Instance;

typedef void (*bsp_isr_handler)(BSP_CAN_Instance *can, uint32_t group_flags);

ret_status BSP_CAN_conf(BSP_CAN_Instance *can, bsp_can_config_t *config);
ret_status BSP_CAN_conf_clock_source(BSP_CAN_Instance *can, bsp_can_clock_source clock_source);
ret_status BSP_CAN_config_irq(BSP_CAN_Instance *can, bsp_can_irq_type irq, bsp_isr_handler handler);
ret_status BSP_CAN_start(BSP_CAN_Instance *can);
ret_status BSP_CAN_add_standard_filter(BSP_CAN_Instance *can, bsp_can_standard_filter_t *filter, uint8_t index);
ret_status BSP_CAN_add_tx_message(BSP_CAN_Instance *can, bsp_can_tx_metadata *pTxHeader, uint8_t *pTxData);
ret_status BSP_CAN_get_rx_message(BSP_CAN_Instance *can, bsp_can_rx_queue queue, bsp_can_rx_metadata_t *rx_metadata, uint8_t *rx_data);
ret_status BSP_CAN_get_baudrate(BSP_CAN_Instance *can, uint32_t *baudrate);


#endif //BSP_CAN_H
