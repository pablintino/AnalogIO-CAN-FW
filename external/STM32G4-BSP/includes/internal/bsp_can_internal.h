/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */


/**
 * @file bsp_can_internal.h
 * @brief Internal, non intended to be use out of the bsp_can driver, definitions.
 */
#ifndef BSP_CAN_INTERNAL_H
#define BSP_CAN_INTERNAL_H


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


#define __BCAN_STD_FILTER_SIZE 28U
#define __BCAN_EXTD_FILTER_SIZE 8U
#define __BCAN_RX_FIFO_SIZE 3U
#define __BCAN_TX_FIFOQ_SIZE 3U
#define __BCAN_TX_EVENTS_SIZE 3U
#define __BCAN_ISR_SOURCES_N 24U
#define __BCAN_MESSAGE_PAYLOAD_SIZE 16U


/**
 * @brief Internal structure that stores information (like ISR handlers) for a particular FDCAN peripheral instance.
 *
 * Currently the only state that needs to be modeled for each FDCAN instance are only the callbacks for each subscribed
 * ISR.
 */
struct __bcan_irqs_state_s {
    /**
     *  Table of ISR handlers for each possible ISR source of the FDCAN peripheral.
     *
     * @param can The FDCAN peripheral that triggered the ISR.
     * @param group_flags The state of the FDCAN IR register when the ISR has been processed.
     * @return Nothing.
     */
    void (*IsrVectors[__BCAN_ISR_SOURCES_N])(bcan_instance_t *can, uint32_t group_flags);
};


/**
 * @brief Extended filter element in FDCAN SRAM
 *
 * Represents an extended filter element placed on the SRAM mapped to a FDCAN peripheral. The disposition of the members
 * is documented in chapter 44.3.9 of RMM0440 and is as follows:
 *
 *      Word 0: EFEC[31:29]:    Extended filter element configuration
 *              EFID1[28:0]:    Extended filter ID 1
 *      Word 1: EFTI[31:30]:    Extended filter type
 *              EFID2[28:0]:    Extended filter ID 2
 *
 */
struct __bcan_ram_extended_filter_s {
    __IO32 header_word1;                /**< First word (F1) of the FDCAN RAM Extended filter [RMM0440 44.3.9] */
    __IO32 header_word2;                /**< Second word (F2) of the filter element  */
};


/**
 * @brief Transmission event element in FDCAN SRAM
 *
 * Represents a transmission event element placed on the SRAM of a FDCAN peripheral. The disposition of the members
 * is documented in chapter 44.3.7 of RMM0440 and is as follows:
 *
 *      Word 0: ESI[31:31]:     Error state indicator
 *              XTD[30:30]:     Extended identifier
 *              RTR[29:29]:     Remote transmission request
 *              ID[28:0]:       Identifier
 *      Word 1: MM[31:24]:      Extended filter type
 *              ET[23:22]:      Event type
 *              EDL[21:21]:     Extended data length
 *              BRS[20:20]:     Bit rate switching
 *              DLC[19:16]:     CAN Data Length Code (number of bytes transmitted)
 *              TXTS[15:0]:     TX timestamp
 *
 */
struct __bcan_ram_tx_event_s {
    __IO32 header_word1;    /**< First word (E0) of the FDCAN RAM TX Event [RMM0440 44.3.7] */
    __IO32 header_word2;    /**< Second word (E1) of the FDCAN RAM TX Event */
};


/**
 * @brief Reception message element for each of the FDCAN SRAM RX FIFOs
 *
 * Represents a message on any of the available reception FIFOs placed in the SRAM of a FDCAN peripheral. The
 * disposition of the members is documented in chapter 44.3.5 of RMM0440 and is as follows:
 *
 *      Word 0: ESI[31:31]:     Error state indicator
 *              XTD[30:30]:     Extended identifier
 *              RTR[29:29]:     Remote transmission request
 *              ID[28:0]:       Identifier
 *      Word 1: ANMF[31:31]:    Accepted non-matching frame
 *              FIDX[30:24]:    Filter index
 *              FDF[21:21]:     FD format
 *              BRS[20:20]:     Bit rate switching
 *              DLC[19:16]:     CAN Data Length Code (number of bytes transmitted)
 *              RXTS[15:0]:     RX timestamp
 *      Word N:                 Payload data.
 *
 */
struct __bcan_ram_rx_fifo_element_s {
    /**
     * First word (R0) of the FDCAN RAM RX Message [RMM0440 44.3.5]
     */
    __IO32 header_word1;
    /**
     * Second word (R1) of the FDCAN RAM RX Message
     */
    __IO32 header_word2;
    /**
    * Message payload. Size indicated by DLC field.
    */
    __IO32 message_payload[__BCAN_MESSAGE_PAYLOAD_SIZE];
};


/**
 * @brief Reception message element of the FDCAN SRAM TX FIFO/Queue
 *
 * Represents a message in the TX FIFO/Queue of the SRAM of a FDCAN peripheral. The disposition of the members is
 * documented in chapter 44.3.6 of RMM0440 and is as follows:
 *
 *      Word 0: ESI[31:31]:     Error state indicator
 *              XTD[30:30]:     Extended identifier
 *              RTR[29:29]:     Remote transmission request
 *              ID[28:0]:       Identifier
 *      Word 1: MM[31:24]:      Message marker
 *              EFC[23:23]:     Event FIFO Control
 *              FDF[21:21]:     FD format
 *              BRS[20:20]:     Bit rate switching
 *              DLC[19:16]:     CAN Data Length Code (number of bytes transmitted)
 *      Word N:                 Payload data.
 *
 */
struct __bcan_ram_tx_fifo_element_s {
    /**
    * First word (T0) of the FDCAN RAM TX Message [RMM0440 44.3.6]
    */
    __IO32 header_word1;
    /**
    * Second word (T1) of the FDCAN RAM TX Message
    */
    __IO32 header_word2;
    /**
     * Message payload. Size indicated by DLC field.
     */
    __IO32 message_payload[__BCAN_MESSAGE_PAYLOAD_SIZE];
};


/**
 * @brief Single FDCAN SRAM peripheral representation as a struct
 *
 * The organization of a single FDCAN SRAM memory is described in chapter 44.3.3 of the RM0440.
 */
struct __bcan_ram_s {
    /**
     * Standard ID (11-bits) filters. One word per filter.
     *
     * The STD filter words are arranged using the following format:
     *
     *     SFT[31:30]   : Standard filter element type
     *     SFEC[29:27]  : Standard filter element configuration
     *     SFID1[26:16] : Standard filter ID 1
     *     SFID2[10:0]  : Standard filter ID 2
     */
    __IO32 standard_filters[__BCAN_STD_FILTER_SIZE];
    /**
     * Extended ID (29-bits) filters. Two words per filter that are modeled by @refitem __bcan_ram_extended_filter_s.
     */
    volatile struct __bcan_ram_extended_filter_s extended_filters[__BCAN_EXTD_FILTER_SIZE];
    /**
     * Reception FIFO 0 for incoming messages. Each message is described by @refitem __bcan_ram_rx_fifo_element_s.
     */
    volatile struct __bcan_ram_rx_fifo_element_s rx_fifo0[__BCAN_RX_FIFO_SIZE];
    /**
     * Reception FIFO 0 for incoming messages. Each message is described by @refitem __bcan_ram_rx_fifo_element_s.
     */
    volatile struct __bcan_ram_rx_fifo_element_s rx_fifo1[__BCAN_RX_FIFO_SIZE];
    /**
    * Message transmission event list. Every transmission event is modeled by @refitem __bcan_ram_tx_event_s.
    */
    volatile struct __bcan_ram_tx_event_s tx_events[__BCAN_TX_FIFOQ_SIZE];
    /**
    * Transmission FIFO/Queue for outgoing messages. Each message is described by @refitem __bcan_ram_tx_fifo_element_s.
    */
    volatile struct __bcan_ram_tx_fifo_element_s tx_fifoq[__BCAN_TX_EVENTS_SIZE];
};


#endif //BSP_CAN_INTERNAL_H
