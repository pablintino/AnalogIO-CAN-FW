/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "bsp.h"
#include "bsp_tick.h"

#include <SEGGER_RTT.h>

static ret_status __configure_clocks(void);

static ret_status __configure_usart(void);

static ret_status __configure_i2c(void);

static ret_status __configure_can(void);


void BSP_init(void) {

    ret_status temp_status = __configure_clocks();
    if (temp_status != STATUS_OK) {
        SEGGER_RTT_WriteString(0, "[ERR] Failed to configure system clocks\r\n");
        while (1) { ; };
    }

    BSP_IRQ_init();
    BSP_CLK_enable_periph_clock(ENGPIOA);
    BSP_CLK_enable_periph_clock(ENGPIOB);
    BSP_CLK_enable_periph_clock(ENI2C3);
    BSP_CLK_enable_periph_clock(ENUSART1);
    BSP_CLK_enable_periph_clock(ENFDCAN);

    temp_status = __configure_usart();
    if (temp_status != STATUS_OK) {
        SEGGER_RTT_WriteString(0, "[ERR] Failed to configure USART\r\n");
        while (1) { ; };
    }

    temp_status = __configure_i2c();
    if (temp_status != STATUS_OK) {
        SEGGER_RTT_WriteString(0, "[ERR] Failed to configure i2c\r\n");
        while (1) { ; };
    }

    temp_status = __configure_can();
    if (temp_status != STATUS_OK) {
        SEGGER_RTT_WriteString(0, "[ERR] Failed to configure CAN\r\n");
        while (1) { ; };
    }
    SEGGER_RTT_WriteString(0, "[INFO] Enabling USART1\r\n");
    busart_enable(USART1);

    SEGGER_RTT_WriteString(0, "[INFO] Enabling I2C3\r\n");
    bi2c_enable(I2C3);

    SEGGER_RTT_WriteString(0, "[INFO] Enabling FDCAN1\r\n");
    bcan_start(FDCAN1);
}

static ret_status __configure_i2c(void) {

    bio_config_af_port(GPIOA,
                       BSP_IO_PIN_8,
                       2,
                       BSP_IO_NO_PU_PD,
                       BSP_IO_LOW,
                       BSP_IO_OUT_TYPE_OPEN_DRAIN);

    bio_config_af_port(GPIOB,
                       BSP_IO_PIN_5,
                       8,
                       BSP_IO_NO_PU_PD,
                       BSP_IO_LOW,
                       BSP_IO_OUT_TYPE_OPEN_DRAIN);

    bsp_i2c_master_config_t i2c_config;
    i2c_config.addressing_mode = BSP_I2C_ADDRESSING_MODE_7;
    i2c_config.analog_filter = true;
    i2c_config.digital_filter = BSP_I2C_DIGITAL_FILTER_OFF;
    i2c_config.self_address = 0x00U;
    i2c_config.fixed_speed = BSP_I2C_SPEED_100;
    i2c_config.custom_timming = 0x00U;
    return bi2c_master_config(I2C3, &i2c_config);
}


static ret_status __configure_can(void) {


    bio_config_af_port(GPIOA,
                       BSP_IO_PIN_11 | BSP_IO_PIN_12,
                       9,
                       BSP_IO_NO_PU_PD,
                       BSP_IO_VERY_HIGH,
                       BSP_IO_OUT_TYPE_PP);


    bcan_config_t can_config;
    can_config.tx_mode = BCAN_TX_MODE_FIFO;
    can_config.mode = BCAN_MODE_NORMAL;
    can_config.timing.phase1 = 13; // 1mbps
    can_config.timing.phase2 = 2;
    can_config.timing.sync_jump_width = 1;
    can_config.timing.prescaler = 3;
    can_config.auto_retransmission = false;
    can_config.global_filters.non_matching_standard_action = BCAN_NON_MATCHING_ACCEPT_RX_0;
    can_config.global_filters.reject_remote_standard = true;

    bcan_config_clk_source(BCAN_CLK_PCLK1);

    ret_status tmp_status = bcan_config(FDCAN1, &can_config);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    bcan_standard_filter_t filter_1;
    filter_1.standard_id1 = 123;
    filter_1.standard_id2 = 321;
    filter_1.type = BCAN_STD_FILTER_TYPE_RANGE;
    filter_1.action = BCAN_STD_FILTER_ACTION_PRIORITIZE_STORE_RX0;
    tmp_status = bcan_add_standard_filter(FDCAN1, &filter_1, 0);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    uint32_t can_baudrate;
    bcan_get_baudrate(FDCAN1, &can_baudrate);
    SEGGER_RTT_printf(0, "[INFO] CAN baudrate set to %u\r\n", can_baudrate);

    tmp_status = bcan_config_irq_line(FDCAN1, BCAN_ISR_GROUP_RXFIFO0, BCAN_ISR_LINE_1);
    if (tmp_status != STATUS_OK) {
        return tmp_status;
    }

    return bcan_enable_irqs(FDCAN1);
}


static ret_status __configure_usart(void) {


    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    bio_config_af_port(GPIOA,
                       BSP_IO_PIN_9 | BSP_IO_PIN_10,
                       7,
                       BSP_IO_PU,
                       BSP_IO_LOW,
                       BSP_IO_OUT_TYPE_PP);

    bsp_usart_config_t usart_config;
    usart_config.hardware_control = BSP_USART_HW_CONTROL_NONE;
    usart_config.stop_bits = BSP_USART_STOP_BITS_1;
    usart_config.baudrate = 115200;
    usart_config.Mode = BSP_USART_MODE_TX;
    usart_config.parity = BSP_USART_PARITY_NONE;
    usart_config.prescaler = BSP_USART_PRESCALER_2;
    usart_config.bit_lengh = BSP_USART_BIT_LENGTH_8;
    usart_config.bit_sampling = BSP_USART_SAMPLING_16_BITS;
    return busart_config(USART1, &usart_config);


}

static ret_status __configure_clocks(void) {

    ret_status temp_status;

    btick_config(BSP_CLK_get_hclk_freq());

    bsp_clk_osc_config_t oscConfig;
    oscConfig.ClockType = BSP_CLK_CLOCK_TYPE_PLL | BSP_CLK_CLOCK_TYPE_HSE;
    oscConfig.HSEState = BSP_CLK_CLOCK_STATE_HSE_STATE_ENABLE;

    oscConfig.PLL.PLLSource = BSP_CLK_CLOCK_PLL_SRC_HSE;
    oscConfig.PLL.PLLState = BSP_CLK_CLOCK_STATE_PLL_STATE_ENABLE;
    oscConfig.PLL.PLLM = 2;
    oscConfig.PLL.PLLN = 8;
    oscConfig.PLL.PLLP = 2;
    oscConfig.PLL.PLLQ = 2;
    oscConfig.PLL.PLLR = 2;

    bsp_clk_clock_config_t clockConfig;
    clockConfig.ClockType = BSP_CLK_CLOCK_TYPE_SYSCLK | BSP_CLK_CLOCK_TYPE_HCLK;
    clockConfig.SystemClockSource = BSP_CLK_CLOCK_SOURCE_PLL;
    clockConfig.AHBDivider = BSP_CLK_AHB_PRESCALER_1;
    clockConfig.APB1_prescaler = APB1_PRESCALER_1;
    clockConfig.APB2_prescaler = APB2_PRESCALER_1;

    temp_status = BSP_CLK_config_clocks_osc(&oscConfig);
    if (temp_status == STATUS_OK) {
        temp_status = BSP_CLK_config_clocks(&clockConfig);
    }

    return temp_status;
}
