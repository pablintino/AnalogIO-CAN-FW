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


#include "bsp_can.h"
#include "bsp_tick.h"
#include "bsp_common_utils.h"
#include "bsp_clocks.h"
#include "bsp_irq_manager.h"


#define FDCAN_ELEMENT_MASK_RTR   ((uint32_t)0x20000000U) /* Remote Transmission Request */
#define FDCAN_ELEMENT_MASK_XTD   ((uint32_t)0x40000000U) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_ESI   ((uint32_t)0x80000000U) /* Error State Indicator       */
#define FDCAN_ELEMENT_MASK_TS    ((uint32_t)0x0000FFFFU) /* Timestamp                   */
#define FDCAN_ELEMENT_MASK_DLC   ((uint32_t)0x000F0000U) /* Data Length Code            */
#define FDCAN_ELEMENT_MASK_BRS   ((uint32_t)0x00100000U) /* Bit Rate Switch             */
#define FDCAN_ELEMENT_MASK_FDF   ((uint32_t)0x00200000U) /* FD Format                   */
#define FDCAN_ELEMENT_MASK_EFC   ((uint32_t)0x00800000U) /* Event FIFO Control          */
#define FDCAN_ELEMENT_MASK_MM    ((uint32_t)0xFF000000U) /* Message Marker              */
#define FDCAN_ELEMENT_MASK_FIDX  ((uint32_t)0x7F000000U) /* Filter Index                */
#define FDCAN_ELEMENT_MASK_ANMF  ((uint32_t)0x80000000U) /* Accepted Non-matching Frame */
#define FDCAN_ELEMENT_MASK_STDID ((uint32_t)0x1FFC0000U) /* Standard Identifier         */
#define FDCAN_ELEMENT_MASK_EXTID ((uint32_t)0x1FFFFFFFU) /* Extended Identifier         */

static const uint8_t __CAN_DLC_TO_BYTE_NUMBER[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};
static const uint8_t __CAN_CLK_DIVIDERS[] = {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};

static const uint32_t __CAN_ISR_GROUP_MASK[] = {0x00000007U, 0x00000038U, 0x000001C0U, 0x00001E00U, 0x0000E000U,
                                                0x00030000U, 0x00FC0000U};

typedef struct {
    void (*IsrVectors[24])(BSP_CAN_Instance *can, uint32_t group_flags);
} __bsp_can_irqs_state;

typedef struct {
    __IO32 W1;
    __IO32 W2;
} __bsp_fdcan_ram_extended_filter_t;

typedef struct {
    __IO32 W1;
    __IO32 W2;
} __bsp_fdcan_ram_tx_event_t;

typedef struct {
    __IO32 HeaderWord1;
    __IO32 HeaderWord2;
    __IO32 Data[16];
} __bsp_fdcan_ram_rx_fifo_element_t;

typedef struct {
    __IO32 HeaderWord1;
    __IO32 HeaderWord2;
    __IO32 Data[16];
} __bsp_fdcan_ram_tx_fifo_element_t;

typedef struct {
    __IO32 StandardFilters[28];
    volatile __bsp_fdcan_ram_extended_filter_t ExtendedFilters[8];
    volatile __bsp_fdcan_ram_rx_fifo_element_t RXFIFO0[3];
    volatile __bsp_fdcan_ram_rx_fifo_element_t RXFIFO1[3];
    volatile __bsp_fdcan_ram_tx_event_t TXEvents[3];
    volatile __bsp_fdcan_ram_tx_fifo_element_t TXFIFOQ[3];
} __bsp_fdcan_ram_t;


static void __bsp_can_irq_handler(BSP_CAN_Instance *can);

#ifdef FDCAN2

static void __irq_handler_fdcan2_it0(void){
    __bsp_can_irq_handler(FDCAN2);
}

static void __irq_handler_fdcan2_it1(void){
    __bsp_can_irq_handler(FDCAN2);
}

static __bsp_can_irqs_state __bsp_can_internal_states[2U];

#elif FDCAN3

static void __irq_handler_fdcan3_it0(void){
    __bsp_can_irq_handler(FDCAN3);
}

static void __irq_handler_fdcan3_it1(void){
    __bsp_can_irq_handler(FDCAN3);
}

static void __bsp_can_irqs_state __bsp_can_internal_states[3U];

#else

static void __irq_handler_fdcan1_it0(void) {
    __bsp_can_irq_handler(FDCAN1);
}

static void __irq_handler_fdcan1_it1(void) {
    __bsp_can_irq_handler(FDCAN1);
}

static __bsp_can_irqs_state __bsp_can_internal_states[1U];
#endif


static void __bsp_can_configure_global_filtering(BSP_CAN_Instance *can, bsp_can_config_t *config);

static void __bsp_copy_message_to_ram(bsp_can_tx_metadata *pTxHeader, uint8_t *pTxData,
                                      volatile __bsp_fdcan_ram_tx_fifo_element_t *message_ram);

static void
__bsp_copy_message_from_ram(volatile __bsp_fdcan_ram_rx_fifo_element_t *message, bsp_can_rx_metadata_t *rx_metadata,
                            uint8_t *rx_data);

static ret_status __get_can_input_frequency(uint32_t *freq);

static inline __bsp_can_irqs_state *__bsp_can_get_instance_state(BSP_CAN_Instance *can);

static inline __bsp_fdcan_ram_t *__bsp_can_get_instance_base_address(BSP_CAN_Instance *can);

static inline void __bsp_can_ensure_isr_line_active(BSP_CAN_Instance *can);

static inline ret_status __bsp_can_conf_validate_isr_group(bsp_can_isr_group isr_group);

ret_status BSP_CAN_conf(BSP_CAN_Instance *can, bsp_can_config_t *config) {

    if (can == NULL || config == NULL) {
        return STATUS_ERR;
    }

    uint32_t start_tick = BSP_TICK_get_ticks();

    /*Request initialization mode of the CAN peripheral */
    __BSP_SET_MASKED_REG(can->CCCR, FDCAN_CCCR_INIT);
    ret_status status = BSP_UTIL_wait_flag_status(&can->CCCR, FDCAN_CCCR_INIT, FDCAN_CCCR_INIT, start_tick, 25U);
    if (status != STATUS_OK) {
        return status;
    }

    /* Request unlock of configuration registers */
    __BSP_SET_MASKED_REG(can->CCCR, FDCAN_CCCR_CCE);
    status = BSP_UTIL_wait_flag_status(&can->CCCR, FDCAN_CCCR_CCE, FDCAN_CCCR_CCE, start_tick, 25U);
    if (status != STATUS_OK) {
        return status;
    }

    /* Enable negated automatic retransmission if requested */
    if (config->EnableAutoretransmision) {
        __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_DAR);
    } else {
        __BSP_SET_MASKED_REG(can->CCCR, FDCAN_CCCR_DAR);
    }

    /* TODO: Now only Classic CAN is supported: Disable FDCAN operation and baudrate switching (used only in FD operation) */
    __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_FDOE | FDCAN_CCCR_BRSE);

    /* Disable protocol exceptions by default */
    __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_PXHD);

    /* If monitor mode has been selected just turn it on */
    if (config->InterfaceMode == BSP_CAN_MODE_BM) {
        __BSP_SET_MASKED_REG(can->CCCR, FDCAN_CCCR_MON);
    } else {
        __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_MON);
    }

    /* Adjust time specifications
     *
     * Phase1T: tseg1 = tq * Phase1T
     * Phase2T: tseg2 = tq * Phase2T
     * Total time: tq * (1 + Phase1T + Phase2T)
     * tq = prescaler / fclk_in
     * baudrate = 1 / total_time
     *
     * */
    can->NBTP = ((((config->Timing.SyncJumpWidth & 0x7F) - 1U) << FDCAN_NBTP_NSJW_Pos) |
                 (((config->Timing.Phase1T & 0xFF) - 1U) << FDCAN_NBTP_NTSEG1_Pos) |
                 (((config->Timing.Phase2T & 0x7F) - 1U) << FDCAN_NBTP_NTSEG2_Pos) |
                 (((config->Timing.Prescaler & 0x01FF) - 1U) << FDCAN_NBTP_NBRP_Pos));


    __BSP_SET_MASKED_REG_VALUE(can->TXBC, FDCAN_TXBC_TFQM, config->TXMode);

    __bsp_can_configure_global_filtering(can, config);

    /*Retrieve the RAM section of the passed CAN instance to clear it */
    __bsp_fdcan_ram_t *instance_ram = __bsp_can_get_instance_base_address(can);
    if (instance_ram == NULL) {
        return STATUS_ERR;
    }

    /* Flush the allocated Message RAM area */
    for (uint32_t *raw_ram_ptr = (uint32_t *) instance_ram;
         raw_ram_ptr < (uint32_t *) (instance_ram + 1); raw_ram_ptr++) {
        *raw_ram_ptr = 0x00000000U;
    }

    return STATUS_OK;
}


ret_status BSP_CAN_add_standard_filter(BSP_CAN_Instance *can, bsp_can_standard_filter_t *filter, uint8_t index) {

    /* Obtain the portion of RAM mapped to the FDCAN selected instance */
    __bsp_fdcan_ram_t *instance_ram = __bsp_can_get_instance_base_address(can);
    if (instance_ram == NULL) {
        return STATUS_ERR;
    }

    /* Filters alter RXGFC LSS field which is protected. Ensure that the peripheral has CCE and INIT set to 1 */
    if (!__BSP_IS_FLAG_SET(can->CCCR, FDCAN_CCCR_CCE) || !__BSP_IS_FLAG_SET(can->CCCR, FDCAN_CCCR_INIT)) {
        return STATUS_ERR;
    }

    uint8_t number_of_filters = ((can->RXGFC & FDCAN_RXGFC_LSS) >> FDCAN_RXGFC_LSS_Pos);

    /* Check that we don't get out of bounds by exceeding the total number of filters */
    if (number_of_filters >= sizeof(instance_ram->StandardFilters) / sizeof(instance_ram->StandardFilters[0])) {
        return STATUS_ERR;
    }

    /* If we already have filters we may need to reorder them */
    if (number_of_filters > 0) {
        for (uint8_t fidx = number_of_filters; fidx > index; fidx--) {
            instance_ram->StandardFilters[fidx] = instance_ram->StandardFilters[fidx - 1];
        }
    }

    /* Add the filter itself */
    instance_ram->StandardFilters[index] = ((filter->Type << BSP_CAN_STD_FILTER_TYPE_Pos) |
                                            (filter->Config << BSP_CAN_STD_FILTER_CONFIG_Pos) |
                                            (filter->StandardID1 << 16) |
                                            (filter->StandardID2));

    /* Update the number of active filters in filters global register */
    __BSP_SET_MASKED_REG_VALUE(can->RXGFC, FDCAN_RXGFC_LSS, ((++number_of_filters) << FDCAN_RXGFC_LSS_Pos));

    return STATUS_OK;
}


ret_status BSP_CAN_add_tx_message(BSP_CAN_Instance *can, bsp_can_tx_metadata *tx_metadata, uint8_t *tx_data) {

    /* Simple validation to avoid NULL pointers */
    if (can == NULL || tx_metadata == NULL || tx_data == NULL) {
        return STATUS_ERR;
    }

    /* Obtain the index where we will write the new message */
    uint32_t tx_index = ((can->TXFQS & FDCAN_TXFQS_TFQPI) >> FDCAN_TXFQS_TFQPI_Pos);

    /* If FIFO/Queue is full just return an error */
    if (__BSP_IS_FLAG_SET(can->TXFQS, FDCAN_TXFQS_TFQF)) {
        return STATUS_ERR;
    }

    /* Retrieve the RAM section mapped to the FDCAN peripheral */
    __bsp_fdcan_ram_t *instance_ram = __bsp_can_get_instance_base_address(can);
    if (instance_ram == NULL) {
        return STATUS_ERR;
    }

    /* Write Tx element header to the message RAM */
    __bsp_copy_message_to_ram(tx_metadata, tx_data, &instance_ram->TXFIFOQ[tx_index]);

    /* Activate the corresponding transmission request */
    __BSP_SET_REG_VALUE(can->TXBAR, ((uint32_t) 1 << tx_index));

    return STATUS_OK;
}


ret_status
BSP_CAN_get_rx_message(BSP_CAN_Instance *can, bsp_can_rx_queue queue, bsp_can_rx_metadata_t *rx_metadata,
                       uint8_t *rx_data) {

    /* Simple validation to avoid NULL pointers */
    if (can == NULL || rx_metadata == NULL || rx_data == NULL) {
        return STATUS_ERR;
    }

    volatile __bsp_fdcan_ram_rx_fifo_element_t *message;
    __bsp_fdcan_ram_t *instance_ram = __bsp_can_get_instance_base_address(can);
    uint8_t fifo_index;


    if (queue == BSP_CAN_RX_QUEUE_O) {
        if ((can->RXF0S & FDCAN_RXF0S_F0FL) == 0) {
            return STATUS_ERR;
        }
        fifo_index = (can->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos;
        message = &instance_ram->RXFIFO0[fifo_index];
    } else if (queue == BSP_CAN_RX_QUEUE_1) {
        if ((can->RXF1S & FDCAN_RXF1S_F1FL) == 0) {
            return STATUS_ERR;
        }
        fifo_index = (can->RXF1S & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;
        message = &instance_ram->RXFIFO1[fifo_index];
    } else {
        return STATUS_ERR;
    }


    __bsp_copy_message_from_ram(message, rx_metadata, rx_data);

    /* Just tell the underlying HW that we have read the message */
    if (queue == BSP_CAN_RX_QUEUE_O) {
        can->RXF0A = fifo_index & FDCAN_RXF0A_F0AI;
    } else {
        can->RXF1A = fifo_index & FDCAN_RXF1A_F1AI;
    }

    return STATUS_OK;
}


ret_status BSP_CAN_start(BSP_CAN_Instance *can) {
    if (can == NULL) {
        return STATUS_ERR;
    }

    __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_INIT);
    return STATUS_OK;
}


ret_status BSP_CAN_conf_clock_source(BSP_CAN_Instance *can, bsp_can_clock_source clock_source) {
    __BSP_SET_MASKED_REG_VALUE(RCC->CCIPR, 0x03 << RCC_CCIPR_FDCANSEL_Pos, clock_source << RCC_CCIPR_FDCANSEL_Pos);
    return STATUS_OK;
}


ret_status BSP_CAN_get_baudrate(BSP_CAN_Instance *can, uint32_t *baudrate) {

    if (can == NULL || baudrate == NULL) {
        return STATUS_ERR;
    }

    uint32_t freq;
    ret_status tmp_status = __get_can_input_frequency(&freq);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    uint32_t input_freq = freq / __CAN_CLK_DIVIDERS[FDCAN_CONFIG->CKDIV & FDCAN_CKDIV_PDIV];

    uint32_t bit_samples = (((can->NBTP & FDCAN_NBTP_NTSEG1) >> FDCAN_NBTP_NTSEG1_Pos) + 1U) +
                           (((can->NBTP & FDCAN_NBTP_NTSEG2) >> FDCAN_NBTP_NTSEG2_Pos) + 1U) + 1U;
    *baudrate = input_freq / (float) ((((can->NBTP & FDCAN_NBTP_NBRP) >> FDCAN_NBTP_NBRP_Pos) + 1U) * bit_samples);

    return STATUS_OK;
}


ret_status BSP_CAN_conf_irq_lines(BSP_CAN_Instance *can, bsp_can_isr_group isr_group, bsp_can_isr_line isr_line) {

    if (can == NULL || (isr_line != BSP_CAN_ISR_LINE_0 && isr_line != BSP_CAN_ISR_LINE_1) ||
        __bsp_can_conf_validate_isr_group(isr_group) != STATUS_OK) {
        return STATUS_ERR;
    }

    if (isr_line == BSP_CAN_ISR_LINE_0) {
        __BSP_CLEAR_MASKED_REG(can->ILS, isr_group);
    } else {
        __BSP_SET_MASKED_REG(can->ILS, isr_group);
    };

    __bsp_can_ensure_isr_line_active(can);

    return STATUS_OK;

}


ret_status BSP_CAN_conf_irq(BSP_CAN_Instance *can, bsp_can_irq_type irq, bsp_isr_handler handler) {

    if (irq > FDCAN_IE_ARAE_Pos || handler == NULL || can == NULL) {
        return STATUS_ERR;
    }

    __bsp_can_irqs_state *instance_state = __bsp_can_get_instance_state(can);

    __BSP_SET_MASKED_REG(can->IE, (1U << irq));

    instance_state->IsrVectors[irq] = handler;

    __bsp_can_ensure_isr_line_active(can);

    return STATUS_OK;
}

ret_status BSP_CAN_enable_irqs(BSP_CAN_Instance *can) {

    if (can == NULL) {
        return STATUS_ERR;
    }

#if defined(FDCAN2)
    if (can == FDCAN2)
  {

     bool irq_enabled;
        BSP_IRQ_is_enabled(FDCAN2_IT0_IRQn, &irq_enabled);
        if(!irq_enabled){
            BSP_IRQ_set_handler(FDCAN2_IT0_IRQn, __irq_handler_fdcan2_it0);
            BSP_IRQ_enable_irq(FDCAN2_IT0_IRQn);
        }

        BSP_IRQ_is_enabled(FDCAN2_IT1_IRQn, &irq_enabled);
        if(!irq_enabled){
            BSP_IRQ_set_handler(FDCAN2_IT1_IRQn, __irq_handler_fdcan2_it1);
            BSP_IRQ_enable_irq(FDCAN2_IT1_IRQn);
        }
  }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3)
  {

     bool irq_enabled;
        BSP_IRQ_is_enabled(FDCAN3_IT0_IRQn, &irq_enabled);
        if(!irq_enabled){
            BSP_IRQ_set_handler(FDCAN3_IT0_IRQn, __irq_handler_fdcan3_it0);
            BSP_IRQ_enable_irq(FDCAN3_IT0_IRQn);
        }

        BSP_IRQ_is_enabled(FDCAN3_IT1_IRQn, &irq_enabled);
        if(!irq_enabled){
            BSP_IRQ_set_handler(FDCAN3_IT1_IRQn, __irq_handler_fdcan3_it1);
            BSP_IRQ_enable_irq(FDCAN3_IT1_IRQn);
        }
  }
#endif
    if (can == FDCAN1) {

        bool irq_enabled;
        BSP_IRQ_is_enabled(FDCAN1_IT0_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(FDCAN1_IT0_IRQn, __irq_handler_fdcan1_it0);
            BSP_IRQ_enable_irq(FDCAN1_IT0_IRQn);
        }

        BSP_IRQ_is_enabled(FDCAN1_IT1_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(FDCAN1_IT1_IRQn, __irq_handler_fdcan1_it1);
            BSP_IRQ_enable_irq(FDCAN1_IT1_IRQn);
        }
    }

    return STATUS_OK;
}

static inline void __bsp_can_ensure_isr_line_active(BSP_CAN_Instance *can) {

    for (uint8_t ils_bit = 0; ils_bit <= FDCAN_ILS_PERR_Pos; ils_bit++) {
        if (__CAN_ISR_GROUP_MASK[ils_bit] & can->IE) {
            if ((can->ILS & (1 << ils_bit)) && (!__BSP_IS_FLAG_SET(can->ILE, FDCAN_ILE_EINT1))) {
                __BSP_SET_MASKED_REG(can->ILE, FDCAN_ILE_EINT1);
            } else if (!(can->ILS & (1 << ils_bit)) && (!__BSP_IS_FLAG_SET(can->ILE, FDCAN_ILE_EINT0))) {
                __BSP_SET_MASKED_REG(can->ILE, FDCAN_ILE_EINT0);
            }
        }
    }
}

static ret_status __get_can_input_frequency(uint32_t *freq) {

    if ((RCC->CCIPR & (0x03U << RCC_CCIPR_FDCANSEL_Pos)) == (BSP_CAN_CLK_HSE << RCC_CCIPR_FDCANSEL_Pos)) {
        *freq = BSP_HSE_VALUE;
    } else if ((RCC->CCIPR & (0x03U << RCC_CCIPR_FDCANSEL_Pos)) == (BSP_CAN_CLK_PLLQ << RCC_CCIPR_FDCANSEL_Pos)) {
        *freq = BSP_CLK_get_pllq_freq();
    } else if ((RCC->CCIPR & (0x03U << RCC_CCIPR_FDCANSEL_Pos)) == (BSP_CAN_CLK_PCLK1 << RCC_CCIPR_FDCANSEL_Pos)) {
        *freq = BSP_CLK_get_pclk1_freq();
    } else {
        return STATUS_ERR;
    }
    return STATUS_OK;
}


static void __bsp_can_configure_global_filtering(BSP_CAN_Instance *can, bsp_can_config_t *config) {

    __BSP_SET_MASKED_REG_VALUE(can->RXGFC, FDCAN_RXGFC_RRFS | FDCAN_RXGFC_ANFS,
                               (config->GlobalFiltering.RejectRemoteStandard ? FDCAN_RXGFC_RRFS : 0x00U) |
                               config->GlobalFiltering.NonMatchingStandard);

}


static void __bsp_copy_message_to_ram(bsp_can_tx_metadata *pTxHeader, uint8_t *pTxData,
                                      volatile __bsp_fdcan_ram_tx_fifo_element_t *message_ram) {
    /* If ID is larger than 11 bits the message is sent using extended 29 bits IDs */
    uint32_t extended_id_xtd =
            (pTxHeader->ID & 0xFFFFF800) ? FDCAN_ELEMENT_MASK_XTD : 0x00U;

    /* Write Tx element header to the message RAM */
    message_ram->HeaderWord1 =
            (pTxHeader->IsRTR ? (1 << 30) : 0x00000000U) | (pTxHeader->ID << (extended_id_xtd ? 0 : 18U)) |
            extended_id_xtd;
    message_ram->HeaderWord2 =
            (pTxHeader->MessageMarker << 24U) | (pTxHeader->StoreTxEvents ? (1 << 23) : 0x00000000U) |
            (pTxHeader->DataLength << 16);

    uint8_t element_counter = 0;
    /* Write Tx payload to the message RAM */
    for (uint32_t byte_n = 0; byte_n < __CAN_DLC_TO_BYTE_NUMBER[pTxHeader->DataLength]; byte_n += 4U) {
        message_ram->Data[element_counter] = ((pTxData[byte_n + 3U] << 24U) |
                                              (pTxData[byte_n + 2U] << 16U) |
                                              (pTxData[byte_n + 1U] << 8U) | pTxData[byte_n]);
        element_counter++;
    }
}


static void
__bsp_copy_message_from_ram(volatile __bsp_fdcan_ram_rx_fifo_element_t *message, bsp_can_rx_metadata_t *rx_metadata,
                            uint8_t *rx_data) {

    /* Retrieve Identifier */
    if ((message->HeaderWord1 & FDCAN_ELEMENT_MASK_XTD) != FDCAN_ELEMENT_MASK_XTD) /* Standard ID element */
    {
        rx_metadata->ID = ((message->HeaderWord1 & FDCAN_ELEMENT_MASK_STDID) >> 18U);
    } else /* Extended ID element */
    {
        rx_metadata->ID = (message->HeaderWord1 & FDCAN_ELEMENT_MASK_EXTID);
    }

    /* Retrieve RxFrameType */
    rx_metadata->IsRTR = ((message->HeaderWord1 & FDCAN_ELEMENT_MASK_RTR) == FDCAN_ELEMENT_MASK_RTR);

    rx_metadata->Timestamp = (message->HeaderWord2 & FDCAN_ELEMENT_MASK_TS);
    rx_metadata->DataLength = (message->HeaderWord2 & FDCAN_ELEMENT_MASK_DLC) >> 16;
    rx_metadata->MatchedFilterIndex = ((message->HeaderWord2 & FDCAN_ELEMENT_MASK_FIDX) >> 24U);
    rx_metadata->NonMatchingElement = ((message->HeaderWord2 & FDCAN_ELEMENT_MASK_ANMF) == FDCAN_ELEMENT_MASK_ANMF);


    uint8_t *pData = (uint8_t *) &message->Data;
    for (uint32_t byte_n = 0; byte_n < __CAN_DLC_TO_BYTE_NUMBER[rx_metadata->DataLength]; byte_n++) {
        rx_data[byte_n] = pData[byte_n];
    }
}


static inline __bsp_can_irqs_state *__bsp_can_get_instance_state(BSP_CAN_Instance *can) {
#if defined(FDCAN2)
    if (can == FDCAN2)
      {
        return &__bsp_can_internal_states[1U];
      }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3)
      {
        return &__bsp_can_internal_states[2U];
      }
#endif
    if (can == FDCAN1) {
        return &__bsp_can_internal_states[0U];
    }
    return NULL;
}


static inline __bsp_fdcan_ram_t *__bsp_can_get_instance_base_address(BSP_CAN_Instance *can) {

    if (can == FDCAN1) {
        return (__bsp_fdcan_ram_t *) SRAMCAN_BASE;
    }

#if defined(FDCAN2)
    if (can == FDCAN2)
  {
    return (__bsp_fdcan_ram_t *) (SRAMCAN_BASE + SRAMCAN_SIZE);
  }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3)
  {
    return (__bsp_fdcan_ram_t *) (SRAMCAN_BASE + SRAMCAN_SIZE * 2U);
  }
#endif
    return NULL;
}


static inline ret_status __bsp_can_conf_validate_isr_group(bsp_can_isr_group isr_group) {
    return (
                   isr_group != BSP_CAN_ISR_GROUP_RXFIFO0 &&
                   isr_group != BSP_CAN_ISR_GROUP_RXFIFO1 &&
                   isr_group != BSP_CAN_ISR_GROUP_SMSG &&
                   isr_group != BSP_CAN_ISR_GROUP_TFERR &&
                   isr_group != BSP_CAN_ISR_GROUP_MISC &&
                   isr_group != BSP_CAN_ISR_GROUP_BERR &&
                   isr_group != BSP_CAN_ISR_GROUP_PERR

           ) ? STATUS_ERR : STATUS_OK;
}


static void __bsp_can_irq_handler(BSP_CAN_Instance *can) {

    __bsp_can_irqs_state *can_instance_state = __bsp_can_get_instance_state(can);
    if (can_instance_state != NULL) {

        uint32_t ir_tmp = can->IR;

        /* Process all the IE bits until no one continues flagged */
        while (ir_tmp != 0) {

            /* Get the fist non-zero bit index. As IE is right aligned just assume that it's better to start from LSB */
            uint8_t isr_index = __builtin_ctz(ir_tmp);

            /* Clear the CAN interrupt flag by writting a 1 to the corresponding IR bit */
            __BSP_SET_MASKED_REG(can->IR, (1 << isr_index));

            /* Call, if available, the registered callback */
            if (can_instance_state->IsrVectors[isr_index] != NULL) {
                can_instance_state->IsrVectors[isr_index](can, can->IR);
            }
            /* Clean the ISR in our internal "IR" register. In the next iteration (if ir_tmp is not empty) we'll process
             * the next ISR bit. FDCAN IRQs merge multiple IRQs in the same line by using IRQ groups. */
            ir_tmp &= ~(1 << isr_index);
        }
    }
}