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


#include <bsp_tick.h>
#include "bsp.h"


static ret_status __configure_clocks(void);

static ret_status __configure_usart(void);

static ret_status __configure_i2c(void);


void BSP_init(void) {

    ret_status temp_status = __configure_clocks();
    if (temp_status != STATUS_OK) {
        while (1) { ; };
    }

    BSP_IRQ_init();
    BSP_CLK_enable_periph_clock(ENGPIOA);
    BSP_CLK_enable_periph_clock(ENGPIOB);
    BSP_CLK_enable_periph_clock(ENI2C3);
    BSP_CLK_enable_periph_clock(ENUSART1);

    temp_status = __configure_usart();
    if (temp_status != STATUS_OK) {
        while (1) { ; };
    }

    temp_status = __configure_i2c();
    if (temp_status != STATUS_OK) {
        while (1) { ; };
    }

    BSP_USART_enable(USART1);
    BSP_I2C_enable(I2C3);
}

static ret_status __configure_i2c(void) {

    BSP_IO_conf_af(GPIOA,
                   BSP_IO_PIN_8,
                   2,
                   BSP_IO_NO_PU_PD,
                   BSP_IO_LOW,
                   BSP_IO_OUT_TYPE_OPEN_DRAIN);

    BSP_IO_conf_af(GPIOB,
                   BSP_IO_PIN_5,
                   8,
                   BSP_IO_NO_PU_PD,
                   BSP_IO_LOW,
                   BSP_IO_OUT_TYPE_OPEN_DRAIN);

    bsp_i2c_master_config_t i2c_config;
    i2c_config.AddressingMode = BSP_I2C_ADDRESSING_MODE_7;
    i2c_config.AnalogFilterEnabled = true;
    i2c_config.DigitalFilter = BSP_I2C_DIGITAL_FILTER_OFF;
    i2c_config.SelfAddress = 0x00U;
    i2c_config.FixedSpeed = BSP_I2C_SPEED_100;
    i2c_config.CustomTimming = 0x00U;
    return BSP_I2C_master_conf(I2C3, &i2c_config);
}



static ret_status __configure_usart(void) {


    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    BSP_IO_conf_af(GPIOA,
                   BSP_IO_PIN_9 | BSP_IO_PIN_10,
                   7,
                   BSP_IO_PU,
                   BSP_IO_LOW,
                   BSP_IO_OUT_TYPE_PP);

    bsp_usart_config_t usart_config;
    usart_config.HardwareControl = BSP_USART_HW_CONTROL_NONE;
    usart_config.StopBits = BSP_USART_STOP_BITS_1;
    usart_config.Baudrate = 115200;
    usart_config.Mode = BSP_USART_MODE_TX;
    usart_config.Parity = BSP_USART_PARITY_NONE;
    usart_config.Prescaler = BSP_USART_PRESCALER_2;
    usart_config.BitLengh = BSP_USART_BIT_LENGTH_8;
    usart_config.BitSampling = BSP_USART_SAMPLING_16_BITS;
    return BSP_USART_conf(USART1, &usart_config);


}

static ret_status __configure_clocks(void) {

    ret_status temp_status;

    BSP_TCK_config(BSP_CLK_get_hclk_freq());

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
