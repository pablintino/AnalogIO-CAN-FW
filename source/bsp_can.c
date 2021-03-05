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


#include "bsp_can.h"
#include "bsp_tick.h"
#include "bsp_common_utils.h"
#include "bsp_clocks.h"
#include "bsp_irq_manager.h"


#define SRAMCAN_FLS_NBR                  (28U)         /* Max. Filter List Standard Number      */
#define SRAMCAN_FLE_NBR                  ( 8U)         /* Max. Filter List Extended Number      */
#define SRAMCAN_RF0_NBR                  ( 3U)         /* RX FIFO 0 Elements Number             */
#define SRAMCAN_RF1_NBR                  ( 3U)         /* RX FIFO 1 Elements Number             */
#define SRAMCAN_TEF_NBR                  ( 3U)         /* TX Event FIFO Elements Number         */
#define SRAMCAN_TFQ_NBR                  ( 3U)         /* TX FIFO/Queue Elements Number         */
#define SRAMCAN_FLS_SIZE            ( 1U * 4U)         /* Filter Standard Element Size in bytes */
#define SRAMCAN_FLE_SIZE            ( 2U * 4U)         /* Filter Extended Element Size in bytes */
#define SRAMCAN_RF0_SIZE            (18U * 4U)         /* RX FIFO 0 Elements Size in bytes      */
#define SRAMCAN_RF1_SIZE            (18U * 4U)         /* RX FIFO 1 Elements Size in bytes      */
#define SRAMCAN_TEF_SIZE            ( 2U * 4U)         /* TX Event FIFO Elements Size in bytes  */
#define SRAMCAN_TFQ_SIZE            (18U * 4U)         /* TX FIFO/Queue Elements Size in bytes  */


#define SRAMCAN_FLSSA ((uint32_t)0)                                                      /* Filter List Standard Start
                                                                                            Address                  */
#define SRAMCAN_FLESA ((uint32_t)(SRAMCAN_FLSSA + (SRAMCAN_FLS_NBR * SRAMCAN_FLS_SIZE))) /* Filter List Extended Start
                                                                                            Address                  */
#define SRAMCAN_RF0SA ((uint32_t)(SRAMCAN_FLESA + (SRAMCAN_FLE_NBR * SRAMCAN_FLE_SIZE))) /* Rx FIFO 0 Start Address  */
#define SRAMCAN_RF1SA ((uint32_t)(SRAMCAN_RF0SA + (SRAMCAN_RF0_NBR * SRAMCAN_RF0_SIZE))) /* Rx FIFO 1 Start Address  */
#define SRAMCAN_TEFSA ((uint32_t)(SRAMCAN_RF1SA + (SRAMCAN_RF1_NBR * SRAMCAN_RF1_SIZE))) /* Tx Event FIFO Start
                                                                                            Address */
#define SRAMCAN_TFQSA ((uint32_t)(SRAMCAN_TEFSA + (SRAMCAN_TEF_NBR * SRAMCAN_TEF_SIZE))) /* Tx FIFO/Queue Start
                                                                                            Address                  */
#define SRAMCAN_SIZE  ((uint32_t)(SRAMCAN_TFQSA + (SRAMCAN_TFQ_NBR * SRAMCAN_TFQ_SIZE))) /* Message RAM size         */


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

void __default_irq_handler(BSP_CAN_Instance *can, uint32_t group_flags) {
}

typedef struct {
    void (*IsrVectors[24])(BSP_CAN_Instance *can, uint32_t group_flags);
} __bsp_can_irqs_state;


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


static uint32_t *__bsp_can_get_instance_base_address(BSP_CAN_Instance *can);

static void __bsp_can_configure_global_filtering(BSP_CAN_Instance *can, bsp_can_config_t *config);

static void __bsp_copy_message_to_ram(BSP_CAN_Instance *can, bsp_can_tx_metadata *pTxHeader, uint8_t *pTxData,
                                      uint32_t BufferIndex);

static void
__bsp_copy_message_from_ram(BSP_CAN_Instance *can, uint32_t *message_base, bsp_can_rx_metadata_t *rx_metadata,
                            uint8_t *rx_data);

static ret_status
__get_can_rx_last_message_addr(BSP_CAN_Instance *can, enum bsp_can_rx_queue queue, uint32_t **message_base,
                               uint8_t *fifo_index);

static ret_status __get_can_input_frequency(BSP_CAN_Instance *can, uint32_t *freq);

static inline uint8_t __bsp_can_get_instance_index(BSP_CAN_Instance *can);

static void __subscribe_irqs(BSP_CAN_Instance *can);


ret_status BSP_CAN_conf(BSP_CAN_Instance *can, bsp_can_config_t *config) {

    uint32_t start_tick = BSP_TICK_get_ticks();
    uint32_t RAMcounter;
    uint32_t SramCanInstanceBase = __bsp_can_get_instance_base_address(can);


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

    /* Flush the allocated Message RAM area */
    for (RAMcounter = SramCanInstanceBase; RAMcounter < (SramCanInstanceBase + SRAMCAN_SIZE); RAMcounter += 4U) {
        *(uint32_t *) (RAMcounter) = 0x00000000U;
    }

    return STATUS_OK;
}


ret_status BSP_CAN_add_standard_filter(BSP_CAN_Instance *can, bsp_can_standard_filter_t *filter, uint8_t index) {

    uint32_t *can_periph_base = __bsp_can_get_instance_base_address(can);
    uint8_t number_of_filters = ((can->RXGFC & FDCAN_RXGFC_LSS) >> FDCAN_RXGFC_LSS_Pos);

    /* A maximum of 28 filters are allowed */
    if (number_of_filters >= 28) {
        return STATUS_ERR;
    }

    /* Filters can only be added in initialization state */
    if (!__BSP_IS_FLAG_SET(can->CCCR, FDCAN_CCCR_CCE) || !!__BSP_IS_FLAG_SET(can->CCCR, FDCAN_CCCR_INIT)) {
        return STATUS_ERR;
    }

    /* If we already have filters we may need to reorder them */
    if (number_of_filters > 0) {
        for (uint8_t fidx = number_of_filters; fidx > index; fidx--) {
            uint32_t *dst_filter_addr = (uint32_t *) (can_periph_base + ((fidx) * 4U));
            *dst_filter_addr = (uint32_t *) (can_periph_base + ((fidx - 1) * 4U));
        }
    }

    /* Just get the address where the filter needs to be inserted */
    uint32_t *filter_addr = (uint32_t *) (can_periph_base + (index * 4U));

    /* Add the filter itself */
    *filter_addr = ((filter->Type << BSP_CAN_STD_FILTER_TYPE_Pos) |
                    (filter->Config << BSP_CAN_STD_FILTER_CONFIG_Pos) | (filter->StandardID1 << 16) |
                    (filter->StandardID2));

    /* Update the number of active filters in filters global register */
    __BSP_SET_MASKED_REG_VALUE(can->RXGFC, FDCAN_RXGFC_LSS, ((++number_of_filters) << FDCAN_RXGFC_LSS_Pos));

    return STATUS_OK;
}


ret_status BSP_CAN_add_tx_message(BSP_CAN_Instance *can, bsp_can_tx_metadata *pTxHeader, uint8_t *pTxData) {

    /* Obtain the index where we will write the new message */
    uint32_t tx_index = ((can->TXFQS & FDCAN_TXFQS_TFQPI) >> FDCAN_TXFQS_TFQPI_Pos);

    /* If FIFO/Queue is full just return an error */
    if (__BSP_IS_FLAG_SET(can->TXFQS, FDCAN_TXFQS_TFQF)) {
        return STATUS_ERR;
    }

    __bsp_copy_message_to_ram(can, pTxHeader, pTxData, tx_index);

    /* Activate the corresponding transmission request */
    __BSP_SET_REG_VALUE(can->TXBAR, ((uint32_t) 1 << tx_index));

    return STATUS_OK;
}


ret_status
BSP_CAN_get_rx_message(BSP_CAN_Instance *can, enum bsp_can_rx_queue queue, bsp_can_rx_metadata_t *rx_metadata,
                       uint8_t *rx_data) {

    uint32_t *message_base;
    uint8_t fifo_index;

    ret_status tmp_status = __get_can_rx_last_message_addr(can, queue, &message_base, &fifo_index);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    __bsp_copy_message_from_ram(can, message_base, rx_metadata, rx_data);

    /* Just tell the underlying HW that we have read the message */
    if (queue == BSP_CAN_RX_QUEUE_O) {
        can->RXF0A = fifo_index & FDCAN_RXF0A_F0AI;
    } else {
        can->RXF1A = fifo_index & FDCAN_RXF1A_F1AI;
    }

    return STATUS_OK;
}


ret_status BSP_CAN_start(BSP_CAN_Instance *can) {
    __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_INIT);
    return STATUS_OK;
}


ret_status BSP_CAN_conf_clock_source(BSP_CAN_Instance *can, enum bsp_can_clock_source clock_source) {
    __BSP_SET_MASKED_REG_VALUE(RCC->CCIPR, 0x03 << RCC_CCIPR_FDCANSEL_Pos, clock_source << RCC_CCIPR_FDCANSEL_Pos);
    return STATUS_OK;
}


ret_status BSP_CAN_get_baudrate(BSP_CAN_Instance *can, uint32_t *baudrate) {

    uint32_t freq;
    ret_status tmp_status = __get_can_input_frequency(can, &freq);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    uint32_t input_freq = freq / __CAN_CLK_DIVIDERS[FDCAN_CONFIG->CKDIV & FDCAN_CKDIV_PDIV];

    uint32_t bit_samples = (((can->NBTP & FDCAN_NBTP_NTSEG1) >> FDCAN_NBTP_NTSEG1_Pos) + 1U) +
                           (((can->NBTP & FDCAN_NBTP_NTSEG2) >> FDCAN_NBTP_NTSEG2_Pos) + 1U) + 1U;
    *baudrate = input_freq / (float) ((((can->NBTP & FDCAN_NBTP_NBRP) >> FDCAN_NBTP_NBRP_Pos) + 1U) * bit_samples);

    return STATUS_OK;
}


ret_status BSP_CAN_config_irq(BSP_CAN_Instance *can, bsp_can_irq_type irq, bsp_isr_handler handler) {

    if(irq >= 24 || handler <= 0){
        return STATUS_ERR;
    }

    uint8_t can_instance_index = __bsp_can_get_instance_index(can);

    if(!__BSP_IS_FLAG_SET(can->IE, (1<<irq))){
        /** TODO: Currently using only IRQ Line 0 only */
        if(!__BSP_IS_FLAG_SET(can->ILE, FDCAN_ILE_EINT0)){
            can->ILE |= FDCAN_ILE_EINT0;
            __BSP_SET_MASKED_REG(can->ILE, FDCAN_ILE_EINT0);
        }

        can->IE |= (1 << irq);
        __bsp_can_internal_states[can_instance_index].IsrVectors[irq] = handler;
        __subscribe_irqs(can);
    }

    return STATUS_OK;

}


static ret_status __get_can_input_frequency(BSP_CAN_Instance *can, uint32_t *freq) {

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


static ret_status
__get_can_rx_last_message_addr(BSP_CAN_Instance *can, enum bsp_can_rx_queue queue, uint32_t **message_base,
                               uint8_t *fifo_index) {
    switch (queue) {

        case BSP_CAN_RX_QUEUE_O:
            if ((can->RXF0S & FDCAN_RXF0S_F0FL) == 0) {
                return STATUS_ERR;
            }
            *fifo_index = (can->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos;
            *message_base =
                    __bsp_can_get_instance_base_address(can) + (SRAMCAN_RF0SA + ((*fifo_index) * SRAMCAN_RF0_SIZE)) / 4;
            break;
        case BSP_CAN_RX_QUEUE_1:
            if ((can->RXF1S & FDCAN_RXF1S_F1FL) == 0) {
                return STATUS_ERR;
            }
            *fifo_index = (can->RXF0S & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;
            *message_base =
                    __bsp_can_get_instance_base_address(can) + (SRAMCAN_RF1SA + ((*fifo_index) * SRAMCAN_RF1_SIZE)) / 4;
            break;
        default:
            *fifo_index = 0;
            return STATUS_ERR;
    }
    return STATUS_OK;
}


static void __bsp_can_configure_global_filtering(BSP_CAN_Instance *can, bsp_can_config_t *config) {

    __BSP_SET_MASKED_REG_VALUE(can->RXGFC, FDCAN_RXGFC_RRFS | FDCAN_RXGFC_ANFS,
                               (config->GlobalFiltering.RejectRemoteStandard ? FDCAN_RXGFC_RRFS : 0x00U) |
                               config->GlobalFiltering.NonMatchingStandard);

}


static void __bsp_copy_message_to_ram(BSP_CAN_Instance *can, bsp_can_tx_metadata *pTxHeader, uint8_t *pTxData,
                                      uint32_t BufferIndex) {
    uint32_t *tx_message_element = __bsp_can_get_instance_base_address(can) + (SRAMCAN_TFQSA +
                                                                               (BufferIndex * SRAMCAN_TFQ_SIZE)) / 4;

    /* Write Tx element header to the message RAM */
    *tx_message_element = (pTxHeader->IsRTR ? (1 << 30) : 0x00000000U) | (pTxHeader->ID << 18U);
    tx_message_element++;
    *tx_message_element = (pTxHeader->MessageMarker << 24U) | (pTxHeader->StoreTxEvents ? (1 << 23) : 0x00000000U) |
                          (pTxHeader->DataLength << 16);
    tx_message_element++;

    /* Write Tx payload to the message RAM */
    for (uint32_t byte_n = 0; byte_n < __CAN_DLC_TO_BYTE_NUMBER[pTxHeader->DataLength]; byte_n += 4U) {
        *tx_message_element = ((pTxData[byte_n + 3U] << 24U) | (pTxData[byte_n + 2U] << 16U) |
                               (pTxData[byte_n + 1U] << 8U) | pTxData[byte_n]);
        tx_message_element++;
    }
}


static void
__bsp_copy_message_from_ram(BSP_CAN_Instance *can, uint32_t *message_base, bsp_can_rx_metadata_t *rx_metadata,
                            uint8_t *rx_data) {

    uint8_t *pData;

    /* Retrieve Identifier */
    if ((*message_base & FDCAN_ELEMENT_MASK_XTD) != FDCAN_ELEMENT_MASK_XTD) /* Standard ID element */
    {
        rx_metadata->ID = ((*message_base & FDCAN_ELEMENT_MASK_STDID) >> 18U);
    } else /* Extended ID element */
    {
        rx_metadata->ID = (*message_base & FDCAN_ELEMENT_MASK_EXTID);
    }

    /* Retrieve RxFrameType */
    rx_metadata->IsRTR = ((*message_base & FDCAN_ELEMENT_MASK_RTR) == FDCAN_ELEMENT_MASK_RTR);


    /* Increment RxAddress pointer to second word of Rx FIFO element */
    message_base++;

    rx_metadata->Timestamp = (*message_base & FDCAN_ELEMENT_MASK_TS);
    rx_metadata->DataLength = (*message_base & FDCAN_ELEMENT_MASK_DLC);
    rx_metadata->MatchedFilterIndex = ((*message_base & FDCAN_ELEMENT_MASK_FIDX) >> 24U);
    rx_metadata->NonMatchingElement = ((*message_base & FDCAN_ELEMENT_MASK_ANMF) == FDCAN_ELEMENT_MASK_ANMF);

    /* Increment RxAddress pointer to payload of Rx FIFO element */
    message_base++;

    pData = (uint8_t *) message_base;
    for (uint32_t byte_n = 0; byte_n < __CAN_DLC_TO_BYTE_NUMBER[rx_metadata->DataLength >> 16U]; byte_n++) {
        rx_data[byte_n] = pData[byte_n];
    }
}


static void __subscribe_irqs(BSP_CAN_Instance *can) {

#if defined(FDCAN2)
    if (can == FDCAN2)
  {
    BSP_IRQ_set_handler(FDCAN2_IT0_IRQn, __irq_handler_fdcan2_it0);
    BSP_IRQ_set_handler(FDCAN2_IT1_IRQn, __irq_handler_fdcan2_it0);
    BSP_IRQ_enable_irq(FDCAN2_IT0_IRQn);
    BSP_IRQ_enable_irq(FDCAN2_IT1_IRQn);
  }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3)
  {
    BSP_IRQ_set_handler(FDCAN3_IT0_IRQn, __irq_handler_fdcan3_it0);
    BSP_IRQ_set_handler(FDCAN3_IT1_IRQn, __irq_handler_fdcan3_it0);
    BSP_IRQ_enable_irq(FDCAN3_IT0_IRQn);
    BSP_IRQ_enable_irq(FDCAN3_IT1_IRQn);

  }
#endif
    if (can == FDCAN1) {
        BSP_IRQ_set_handler(FDCAN1_IT0_IRQn, __irq_handler_fdcan1_it0);
        BSP_IRQ_set_handler(FDCAN1_IT1_IRQn, __irq_handler_fdcan1_it1);
        BSP_IRQ_enable_irq(FDCAN1_IT0_IRQn);
        BSP_IRQ_enable_irq(FDCAN1_IT1_IRQn);
    }
}


static inline uint8_t __bsp_can_get_instance_index(BSP_CAN_Instance *can) {

#if defined(FDCAN2)
    if (can == FDCAN2)
      {
        return 1U;
      }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3)
      {
        return 2U;
      }
#endif
    return 0U;
}


static uint32_t *__bsp_can_get_instance_base_address(BSP_CAN_Instance *can) {

    uint32_t fdcan_start_address = SRAMCAN_BASE;

#if defined(FDCAN2)
    if (can == FDCAN2)
  {
    fdcan_start_address += SRAMCAN_SIZE;
  }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3)
  {
    fdcan_start_address += SRAMCAN_SIZE * 2U;
  }
#endif
    return fdcan_start_address;
}


static void __bsp_can_irq_handler(BSP_CAN_Instance *can) {
    uint8_t can_instance_index = __bsp_can_get_instance_index(can);
    __bsp_can_irqs_state can_instance_state = __bsp_can_internal_states[can_instance_index];
    uint32_t ir_tmp = can->IR;

    while (ir_tmp != 0) {
        uint8_t isr_index = __builtin_ctz(ir_tmp);
        __BSP_SET_MASKED_REG(can->IR, (1 << isr_index));
        if (can_instance_state.IsrVectors[isr_index] > 0) {
            can_instance_state.IsrVectors[isr_index](can, can->IR);
        }
        ir_tmp &= ~(1 << isr_index);
    }
}