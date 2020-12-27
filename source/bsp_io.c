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


#include "bsp_io.h"


/* internal type */
enum bsp_port_io_type_t {
    INPUT = 0,
    OUTPUT = 1,
    ALTERNATE = 2,
    ANALOG = 3
};
typedef enum bsp_port_io_type_t bsp_port_io_type_t;


static void configure_io_type(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_io_type_t io_type);

static void
conf_digital_io(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed,
                bsp_port_io_type_t type);

void
BSP_IO_conf_output_pin(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed) {
    conf_digital_io(port, pin_number, pull_up_down, speed, OUTPUT);
}

void
BSP_IO_write_toggle_pin(GPIO_TypeDef *port, uint8_t pin_number) {
    if (pin_number < 16) {
        uint16_t pin_bin = 1 << pin_number;
        port->BSRR = (port->ODR & pin_bin) != 0X00u ? (uint32_t) pin_bin << 16ul : (uint32_t) pin_bin;

    }
}

void
BSP_IO_write_pin(GPIO_TypeDef *port, uint8_t pin_number, uint8_t value) {
    if (pin_number < 16) {
        uint16_t pin_bin = 1 << pin_number;

        if (value == 0) {
            port->BRR = (uint32_t) pin_bin;
        } else {
            port->BSRR = (uint32_t) pin_bin;
        }
    }
}

uint8_t
BSP_IO_read_pin(GPIO_TypeDef *port, uint8_t pin_number) {
    uint8_t res = 0;
    if (pin_number < 16) {
        res = (uint8_t)((port->IDR & (uint32_t)(1 << pin_number)) >> pin_number);
    }
    return res;
}

void
BSP_IO_conf_input_pin(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed) {
    conf_digital_io(port, pin_number, pull_up_down, speed, OUTPUT);
}


static void configure_io_type(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_io_type_t io_type) {
    uint32_t tmp_reg;

    /* First configure de IO type by using the MODER register */
    tmp_reg = port->MODER;
    tmp_reg &= ~(0x03UL << (pin_number * 2));
    tmp_reg |= ((io_type & 0x3UL) << (pin_number * 2));
    port->MODER = tmp_reg;
}


static void
conf_digital_io(GPIO_TypeDef *port, uint16_t pin_number, bsp_port_pp_pd_t pull_up_down, bsp_port_speed_t speed,
                bsp_port_io_type_t type) {

    uint32_t tmp_reg;

    /* First configure de IO type by using the MODER register */
    configure_io_type(port, pin_number, type);

    /* Configure PP or PD output configuration */
    tmp_reg = port->PUPDR;
    tmp_reg &= ~(0x03UL << (pin_number * 2));
    tmp_reg |= ((pull_up_down & 0x3UL) << (pin_number * 2));
    port->PUPDR = tmp_reg;

    /* Configure speed */
    tmp_reg = port->OSPEEDR;
    tmp_reg &= ~(0x03UL << (pin_number * 2));
    tmp_reg |= ((speed & 0x3UL) << (pin_number * 2));
    port->OSPEEDR = tmp_reg;
}