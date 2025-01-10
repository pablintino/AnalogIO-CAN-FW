/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "includes/bsp_can.h"
#include "includes/bsp_clocks.h"
#include "includes/bsp_common_utils.h"
#include "includes/bsp_irq_manager.h"
#include "includes/bsp_tick.h"
#include "internal/bsp_can_internal.h"

static const uint8_t __CAN_DLC_TO_BYTE_NUMBER[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};
static const uint8_t __CAN_CLK_DIVIDERS[] = {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};

static const uint32_t __CAN_ISR_GROUP_MASK[] = {
    0x00000007U, 0x00000038U, 0x000001C0U, 0x00001E00U, 0x0000E000U, 0x00030000U, 0x00FC0000U};

static void __bsp_can_irq_handler(bcan_instance_t *can);

#ifdef FDCAN3
static struct __bcan_irqs_state_s __bsp_can_internal_states[3U];
#elif FDCAN2
static struct __bcan_irqs_state_s __bsp_can_internal_states[2U];
#else
static struct __bcan_irqs_state_s __bsp_can_internal_states[1U];
#endif

#ifdef FDCAN2
static void __irq_handler_fdcan2_it0(void)
{
    __bsp_can_irq_handler(FDCAN2);
}

static void __irq_handler_fdcan2_it1(void)
{
    __bsp_can_irq_handler(FDCAN2);
}
#endif

#ifdef FDCAN3
static void __irq_handler_fdcan3_it0(void)
{
    __bsp_can_irq_handler(FDCAN3);
}

static void __irq_handler_fdcan3_it1(void)
{
    __bsp_can_irq_handler(FDCAN3);
}
#endif

#ifdef FDCAN1
static void __irq_handler_fdcan1_it0(void)
{
    __bsp_can_irq_handler(FDCAN1);
}

static void __irq_handler_fdcan1_it1(void)
{
    __bsp_can_irq_handler(FDCAN1);
}
#endif

static void __bsp_can_configure_global_filtering(bcan_instance_t *can, const bcan_config_t *config);

static void __bsp_copy_message_to_ram(const bcan_tx_metadata_t *pTxHeader,
                                      const uint8_t *pTxData,
                                      volatile struct __bcan_ram_tx_fifo_element_s *message_ram);

static void __bsp_copy_message_from_ram(volatile struct __bcan_ram_rx_fifo_element_s *message,
                                        bcan_rx_metadata_t *rx_metadata,
                                        uint8_t *rx_data);

static ret_status __get_can_input_frequency(uint32_t *freq);

static inline struct __bcan_irqs_state_s *__bsp_can_get_instance_state(bcan_instance_t *can);

static inline struct __bcan_ram_s *__bsp_can_get_instance_base_address(bcan_instance_t *can);

static inline void __bsp_can_ensure_isr_line_active(bcan_instance_t *can);

static inline ret_status __bsp_can_conf_validate_isr_group(bcan_isr_group_t isr_group);

/**
 *
 * @param can The FDCAN peripheral instance to configure.
 * @param config The structure containing the configuration details.
 * @return the result of the operation. ::STATUS_OK if no error has occurred, other otherwise.
 *
 * This function puts the given FDCAN peripheral into "SW Initialization" state (as described in RM0440 chapter 44.3.2)
 *  in order to write protected bits of CCCR, TXBC and NBTP. <br>
 *
 * The current implementation does the following changes to registers in order to apply the given configuration:
 *     1. Set CCCR INIT bit to indicate that we are going to enter into initialization state. Wait until is set.
 *     2. Set CCCR CCE bit to unlock protected bits of the FDCAN registers. Wait until is set.
 *     3. Set CCCR DAR bit based on bcan_config_t::auto_retransmission value.
 *     4. Disable CAN FD and bitrate switching functionality. Protocol exception handling is disabled too since its
 *     functionality is implemented in HW for FD operation only. This version of the BSP does not support those
 *     functionalities.
 *     5. Set CCCR MON bit based on bcan_config_t::mode value. If mode is ::BCAN_MODE_BM bus
 *     monitor mode is enabled by writing a 1 to this bit.
 *     6. Configure peripheral nominal timing by writing NSJW, NBRP, NTSEG1 and NTSEG2 fields of NBTP register with
 *     the values provided by bcan_config_t::timing. The values given in that structure are
 *     supposed to be based on "one" index notation but registers are zero based, so all values are written subtracted
 *     by one.
 *     7. Set TXBC TFQM bit based on bcan_config_t::auto_retransmission::tx_mode. If tx_mode is set to be
 *     ::BCAN_TX_MODE_QUEUE transmission FIFO works like a priority queue as described in chapter 44.4.4
 *     "Message RAM, Tx Queue" of RM0440.
 *     8. The whole SRAM associated to the FDCAN instance is wiped by writing zeroes to it.
 *
 *     After configuring the given FDCAN instance the peripheral remains in "SW initialization" state. To put the
 *     instance in normal mode ::bcan_start should be called.
 */
ret_status bcan_config(bcan_instance_t *can, const bcan_config_t *config)
{

    if (can == NULL || config == NULL) {
        return STATUS_ERR;
    }

    uint32_t start_tick = btick_get_ticks();

    /*Request initialization mode of the CAN peripheral */
    __BSP_SET_MASKED_REG(can->CCCR, FDCAN_CCCR_INIT);
    ret_status status = butil_wait_flag_status(&can->CCCR, FDCAN_CCCR_INIT, FDCAN_CCCR_INIT, start_tick, 25U);
    if (status != STATUS_OK) {
        return status;
    }

    /* Request unlock of configuration registers */
    __BSP_SET_MASKED_REG(can->CCCR, FDCAN_CCCR_CCE);
    status = butil_wait_flag_status(&can->CCCR, FDCAN_CCCR_CCE, FDCAN_CCCR_CCE, start_tick, 25U);
    if (status != STATUS_OK) {
        return status;
    }

    /* Enable negated automatic retransmission if requested */
    if (config->auto_retransmission) {
        __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_DAR);
    } else {
        __BSP_SET_MASKED_REG(can->CCCR, FDCAN_CCCR_DAR);
    }

    /* TODO: Now only Classic CAN is supported: Disable FDCAN operation, baudrate switching and exception handling (used
     * only in FD operation) */
    __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_FDOE | FDCAN_CCCR_BRSE | FDCAN_CCCR_PXHD);

    /* If monitor mode has been selected just turn it on */
    if (config->mode == BCAN_MODE_BM) {
        __BSP_SET_MASKED_REG(can->CCCR, FDCAN_CCCR_MON);
    } else {
        __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_MON);
    }

    /* Adjust time specifications
     *
     * phase1: tseg1 = tq * phase1
     * phase2: tseg2 = tq * phase2
     * Total time: tq * (1 + phase1 + phase2)
     * tq = prescaler / fclk_in
     * baudrate = 1 / total_time
     *
     * */
    can->NBTP = ((((config->timing.sync_jump_width & 0x7F) - 1U) << FDCAN_NBTP_NSJW_Pos) |
                 (((config->timing.phase1 & 0xFF) - 1U) << FDCAN_NBTP_NTSEG1_Pos) |
                 (((config->timing.phase2 & 0x7F) - 1U) << FDCAN_NBTP_NTSEG2_Pos) |
                 (((config->timing.prescaler & 0x01FF) - 1U) << FDCAN_NBTP_NBRP_Pos));

    __BSP_SET_MASKED_REG_VALUE(can->TXBC, FDCAN_TXBC_TFQM, config->tx_mode);

    __bsp_can_configure_global_filtering(can, config);

    /*Retrieve the RAM section of the passed CAN instance to clear it */
    struct __bcan_ram_s *instance_ram = __bsp_can_get_instance_base_address(can);
    if (instance_ram == NULL) {
        return STATUS_ERR;
    }

    /* Flush the allocated Message RAM area */
    for (uint32_t *raw_ram_ptr = (uint32_t *)instance_ram; raw_ram_ptr < (uint32_t *)(instance_ram + 1);
         raw_ram_ptr++) {
        *raw_ram_ptr = 0x00000000U;
    }

    return STATUS_OK;
}

ret_status bcan_add_standard_filter(bcan_instance_t *can, const bcan_standard_filter_t *filter, uint8_t index)
{

    /* Obtain the portion of RAM mapped to the FDCAN selected instance */
    struct __bcan_ram_s *instance_ram = __bsp_can_get_instance_base_address(can);
    if (instance_ram == NULL) {
        return STATUS_ERR;
    }

    /* Filters alter RXGFC LSS field which is protected. Ensure that the peripheral has CCE and INIT set to 1 */
    if (!__BSP_IS_FLAG_SET(can->CCCR, FDCAN_CCCR_CCE) || !__BSP_IS_FLAG_SET(can->CCCR, FDCAN_CCCR_INIT)) {
        return STATUS_ERR;
    }

    uint8_t number_of_filters = ((can->RXGFC & FDCAN_RXGFC_LSS) >> FDCAN_RXGFC_LSS_Pos);

    /* Check that we don't get out of bounds by exceeding the total number of filters */
    if (number_of_filters >= sizeof(instance_ram->standard_filters) / sizeof(instance_ram->standard_filters[0])) {
        return STATUS_ERR;
    }

    /* If we already have filters we may need to reorder them */
    if (number_of_filters > 0) {
        for (uint8_t fidx = number_of_filters; fidx > index; fidx--) {
            instance_ram->standard_filters[fidx] = instance_ram->standard_filters[fidx - 1];
        }
    }

    /* Add the filter itself */
    instance_ram->standard_filters[index] =
        ((filter->type << BSP_CAN_STD_FILTER_TYPE_Pos) | (filter->action << BSP_CAN_STD_FILTER_CONFIG_Pos) |
         (filter->standard_id1 << 16) | (filter->standard_id2));

    /* Update the number of active filters in filters global register */
    __BSP_SET_MASKED_REG_VALUE(can->RXGFC, FDCAN_RXGFC_LSS, ((++number_of_filters) << FDCAN_RXGFC_LSS_Pos));

    return STATUS_OK;
}

ret_status bcan_add_tx_message(bcan_instance_t *can, const bcan_tx_metadata_t *tx_metadata, const uint8_t *tx_data)
{

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
    struct __bcan_ram_s *instance_ram = __bsp_can_get_instance_base_address(can);
    if (instance_ram == NULL) {
        return STATUS_ERR;
    }

    /* Write Tx element header to the message RAM */
    __bsp_copy_message_to_ram(tx_metadata, tx_data, &instance_ram->tx_fifoq[tx_index]);

    /* Activate the corresponding transmission request */
    __BSP_SET_REG_VALUE(can->TXBAR, ((uint32_t)1 << tx_index));

    return STATUS_OK;
}

ret_status bcan_get_rx_message(bcan_instance_t *can,
                               bcan_rx_queue_t queue,
                               bcan_rx_metadata_t *rx_metadata,
                               uint8_t *rx_data)
{

    /* Simple validation to avoid NULL pointers */
    if (can == NULL || rx_metadata == NULL || rx_data == NULL) {
        return STATUS_ERR;
    }

    volatile struct __bcan_ram_rx_fifo_element_s *message;
    struct __bcan_ram_s *instance_ram = __bsp_can_get_instance_base_address(can);
    uint8_t fifo_index;

    if (queue == BCAN_RX_QUEUE_O) {
        if ((can->RXF0S & FDCAN_RXF0S_F0FL) == 0) {
            return STATUS_ERR;
        }
        fifo_index = (can->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos;
        message = &instance_ram->rx_fifo0[fifo_index];
    } else if (queue == BCAN_RX_QUEUE_1) {
        if ((can->RXF1S & FDCAN_RXF1S_F1FL) == 0) {
            return STATUS_ERR;
        }
        fifo_index = (can->RXF1S & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;
        message = &instance_ram->rx_fifo1[fifo_index];
    } else {
        return STATUS_ERR;
    }

    __bsp_copy_message_from_ram(message, rx_metadata, rx_data);

    /* Just tell the underlying HW that we have read the message */
    if (queue == BCAN_RX_QUEUE_O) {
        can->RXF0A = fifo_index & FDCAN_RXF0A_F0AI;
    } else {
        can->RXF1A = fifo_index & FDCAN_RXF1A_F1AI;
    }

    return STATUS_OK;
}

/** @brief Starts the given CAN peripheral getting the peripheral out of the software initialization state to one of the
 * possible final states. Check RM0440 to see all the possible final states.
 *
 *  @param can The instance of the peripheral to be started.
 *  @return the result of the operation. ::STATUS_OK if no error has occurred, other otherwise.
 */
ret_status bcan_start(bcan_instance_t *can)
{
    if (can == NULL) {
        return STATUS_ERR;
    }

    __BSP_CLEAR_MASKED_REG(can->CCCR, FDCAN_CCCR_INIT);
    return STATUS_OK;
}

ret_status bcan_config_clk_source(bcan_clock_source_t clock_source)
{
    __BSP_SET_MASKED_REG_VALUE(RCC->CCIPR, RCC_CCIPR_FDCANSEL, clock_source << RCC_CCIPR_FDCANSEL_Pos);
    return STATUS_OK;
}

ret_status bcan_get_baudrate(bcan_instance_t *can, uint32_t *baudrate)
{

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
    *baudrate = input_freq / (float)((((can->NBTP & FDCAN_NBTP_NBRP) >> FDCAN_NBTP_NBRP_Pos) + 1U) * bit_samples);

    return STATUS_OK;
}

ret_status bcan_config_irq_line(bcan_instance_t *can, bcan_isr_group_t isr_group, bcan_isr_line_t isr_line)
{

    if (can == NULL || (isr_line != BCAN_ISR_LINE_0 && isr_line != BCAN_ISR_LINE_1) ||
        __bsp_can_conf_validate_isr_group(isr_group) != STATUS_OK) {
        return STATUS_ERR;
    }

    if (isr_line == BCAN_ISR_LINE_0) {
        __BSP_CLEAR_MASKED_REG(can->ILS, isr_group);
    } else {
        __BSP_SET_MASKED_REG(can->ILS, isr_group);
    };

    __bsp_can_ensure_isr_line_active(can);

    return STATUS_OK;
}

ret_status bcan_config_irq(bcan_instance_t *can, bcan_irq_type_t irq, bcan_isr_handler handler)
{

    if (irq > FDCAN_IE_ARAE_Pos || handler == NULL || can == NULL) {
        return STATUS_ERR;
    }

    struct __bcan_irqs_state_s *instance_state = __bsp_can_get_instance_state(can);

    __BSP_SET_MASKED_REG(can->IE, (1U << irq));

    instance_state->IsrVectors[irq] = handler;

    __bsp_can_ensure_isr_line_active(can);

    return STATUS_OK;
}

ret_status bcan_enable_irqs(bcan_instance_t *can)
{

    if (can == NULL) {
        return STATUS_ERR;
    }

#if defined(FDCAN2)
    if (can == FDCAN2) {

        bool irq_enabled;
        BSP_IRQ_is_enabled(FDCAN2_IT0_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(FDCAN2_IT0_IRQn, __irq_handler_fdcan2_it0);
            BSP_IRQ_enable_irq(FDCAN2_IT0_IRQn);
        }

        BSP_IRQ_is_enabled(FDCAN2_IT1_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(FDCAN2_IT1_IRQn, __irq_handler_fdcan2_it1);
            BSP_IRQ_enable_irq(FDCAN2_IT1_IRQn);
        }
    }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3) {

        bool irq_enabled;
        BSP_IRQ_is_enabled(FDCAN3_IT0_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(FDCAN3_IT0_IRQn, __irq_handler_fdcan3_it0);
            BSP_IRQ_enable_irq(FDCAN3_IT0_IRQn);
        }

        BSP_IRQ_is_enabled(FDCAN3_IT1_IRQn, &irq_enabled);
        if (!irq_enabled) {
            BSP_IRQ_set_handler(FDCAN3_IT1_IRQn, __irq_handler_fdcan3_it1);
            BSP_IRQ_enable_irq(FDCAN3_IT1_IRQn);
        }
    }
#endif
    if (can == FDCAN1) {

        bool irq_enabled;
        birq_is_enabled(FDCAN1_IT0_IRQn, &irq_enabled);
        if (!irq_enabled) {
            birq_set_handler(FDCAN1_IT0_IRQn, __irq_handler_fdcan1_it0);
            birq_enable_irq_with_priority(
                FDCAN1_IT0_IRQn, BSP_IRQ_MANAGER_DEFAULT_PRIORITY, BSP_IRQ_MANAGER_DEFAULT_SUB_PRIORITY);
        }

        birq_is_enabled(FDCAN1_IT1_IRQn, &irq_enabled);
        if (!irq_enabled) {
            birq_set_handler(FDCAN1_IT1_IRQn, __irq_handler_fdcan1_it1);
            birq_enable_irq_with_priority(
                FDCAN1_IT1_IRQn, BSP_IRQ_MANAGER_DEFAULT_PRIORITY, BSP_IRQ_MANAGER_DEFAULT_SUB_PRIORITY);
        }
    }

    return STATUS_OK;
}

static inline void __bsp_can_ensure_isr_line_active(bcan_instance_t *can)
{

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

static ret_status __get_can_input_frequency(uint32_t *freq)
{

    if ((RCC->CCIPR & RCC_CCIPR_FDCANSEL) == (BCAN_CLK_HSE << RCC_CCIPR_FDCANSEL_Pos)) {
        *freq = BSP_HSE_VALUE;
    } else if ((RCC->CCIPR & RCC_CCIPR_FDCANSEL) == (BCAN_CLK_PLLQ << RCC_CCIPR_FDCANSEL_Pos)) {
        *freq = bclk_get_pllq_freq();
    } else if ((RCC->CCIPR & RCC_CCIPR_FDCANSEL) == (BCAN_CLK_PCLK1 << RCC_CCIPR_FDCANSEL_Pos)) {
        *freq = bclk_get_pclk1_freq();
    } else {
        return STATUS_ERR;
    }
    return STATUS_OK;
}

static void __bsp_can_configure_global_filtering(bcan_instance_t *can, const bcan_config_t *config)
{

    __BSP_SET_MASKED_REG_VALUE(can->RXGFC,
                               FDCAN_RXGFC_RRFS | FDCAN_RXGFC_ANFS,
                               (config->global_filters.reject_remote_standard ? FDCAN_RXGFC_RRFS : 0x00U) |
                                   config->global_filters.non_matching_standard_action);
}

static void __bsp_copy_message_to_ram(const bcan_tx_metadata_t *pTxHeader,
                                      const uint8_t *pTxData,
                                      volatile struct __bcan_ram_tx_fifo_element_s *message_ram)
{
    /* If ID is larger than 11 bits the message is sent using extended 29 bits IDs */
    uint32_t extended_id_xtd = (pTxHeader->id & 0xFFFFF800) ? FDCAN_ELEMENT_MASK_XTD : 0x00U;

    /* Write Tx element header to the message RAM */
    message_ram->header_word1 = (pTxHeader->is_rtr ? (1 << 30) : 0x00000000U) |
                                (pTxHeader->id << (extended_id_xtd ? 0 : 18U)) | extended_id_xtd;

    const uint8_t message_size = pTxHeader->size_b & 0x0FU;
    message_ram->header_word2 = (pTxHeader->message_marker << 24U) |
                                (pTxHeader->store_tx_events ? (1 << 23) : 0x00000000U) | (message_size << 16);

    uint8_t element_counter = 0;
    /* Write Tx payload to the message RAM */
    for (uint32_t byte_n = 0; byte_n < __CAN_DLC_TO_BYTE_NUMBER[message_size]; byte_n += 4U) {
        message_ram->message_payload[element_counter] = ((pTxData[byte_n + 3U] << 24U) | (pTxData[byte_n + 2U] << 16U) |
                                                         (pTxData[byte_n + 1U] << 8U) | pTxData[byte_n]);
        element_counter++;
    }
}

static void __bsp_copy_message_from_ram(volatile struct __bcan_ram_rx_fifo_element_s *message,
                                        bcan_rx_metadata_t *rx_metadata,
                                        uint8_t *rx_data)
{

    /* Retrieve Identifier */
    if ((message->header_word1 & FDCAN_ELEMENT_MASK_XTD) != FDCAN_ELEMENT_MASK_XTD) /* Standard ID element */
    {
        rx_metadata->id = ((message->header_word1 & FDCAN_ELEMENT_MASK_STDID) >> 18U);
    } else /* Extended ID element */
    {
        rx_metadata->id = (message->header_word1 & FDCAN_ELEMENT_MASK_EXTID);
    }

    /* Retrieve RxFrameType */
    rx_metadata->is_rtr = ((message->header_word1 & FDCAN_ELEMENT_MASK_RTR) == FDCAN_ELEMENT_MASK_RTR);

    rx_metadata->timestamp = (message->header_word2 & FDCAN_ELEMENT_MASK_TS);
    rx_metadata->size_b = (message->header_word2 & FDCAN_ELEMENT_MASK_DLC) >> 16;
    rx_metadata->matched_filter_index = ((message->header_word2 & FDCAN_ELEMENT_MASK_FIDX) >> 24U);
    rx_metadata->non_matching_element = ((message->header_word2 & FDCAN_ELEMENT_MASK_ANMF) == FDCAN_ELEMENT_MASK_ANMF);

    uint8_t *pData = (uint8_t *)&message->message_payload;
    for (uint32_t byte_n = 0; byte_n < __CAN_DLC_TO_BYTE_NUMBER[rx_metadata->size_b]; byte_n++) {
        rx_data[byte_n] = pData[byte_n];
    }
}

static inline struct __bcan_irqs_state_s *__bsp_can_get_instance_state(bcan_instance_t *can)
{
#if defined(FDCAN2)
    if (can == FDCAN2) {
        return &__bsp_can_internal_states[1U];
    }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3) {
        return &__bsp_can_internal_states[2U];
    }
#endif
    if (can == FDCAN1) {
        return &__bsp_can_internal_states[0U];
    }
    return NULL;
}

static inline struct __bcan_ram_s *__bsp_can_get_instance_base_address(bcan_instance_t *can)
{

    if (can == FDCAN1) {
        return (struct __bcan_ram_s *)SRAMCAN_BASE;
    }

#if defined(FDCAN2)
    if (can == FDCAN2) {
        return (__bsp_fdcan_ram_t *)(SRAMCAN_BASE + SRAMCAN_SIZE);
    }
#endif
#if defined(FDCAN3)
    if (can == FDCAN3) {
        return (__bsp_fdcan_ram_t *)(SRAMCAN_BASE + SRAMCAN_SIZE * 2U);
    }
#endif
    return NULL;
}

static inline ret_status __bsp_can_conf_validate_isr_group(bcan_isr_group_t isr_group)
{
    return (isr_group != BCAN_ISR_GROUP_RXFIFO0 && isr_group != BCAN_ISR_GROUP_RXFIFO1 &&
            isr_group != BCAN_ISR_GROUP_SMSG && isr_group != BCAN_ISR_GROUP_TFERR && isr_group != BCAN_ISR_GROUP_MISC &&
            isr_group != BCAN_ISR_GROUP_BERR && isr_group != BCAN_ISR_GROUP_PERR

            )
               ? STATUS_ERR
               : STATUS_OK;
}

static void __bsp_can_irq_handler(bcan_instance_t *can)
{

    struct __bcan_irqs_state_s *can_instance_state = __bsp_can_get_instance_state(can);
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