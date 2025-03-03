/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "bsp_io.h"
#include "bsp_common_utils.h"

/* internal type */
typedef enum bsp_port_io_type_e { INPUT = 0x00U, OUTPUT = 0x01U, ALTERNATE = 0x02U, ANALOG = 0x03U } bsp_port_io_type_t;

static void __configure_output_characteristics(bio_port *port,
                                               bsp_io_pin_number pin_number,
                                               bsp_port_speed_t speed,
                                               bsp_port_output_type_t output_type);

static ret_status __configure_io_ports(bio_port *port,
                                       bsp_io_pin_number pin_number,
                                       bsp_port_io_type_t io_type,
                                       uint8_t af_function,
                                       bsp_port_pp_pd_t pull_up_down,
                                       bsp_port_speed_t speed,
                                       bsp_port_output_type_t output_type);

ret_status bio_conf_output_port(bio_port *port,
                                bsp_io_pin_number pin_number,
                                bsp_port_pp_pd_t pull_up_down,
                                bsp_port_speed_t speed,
                                bsp_port_output_type_t output_type)
{
    return __configure_io_ports(port, pin_number, OUTPUT, 0U, pull_up_down, speed, output_type);
}

ret_status bio_config_input_port(bio_port *port,
                                 bsp_io_pin_number pin_number,
                                 bsp_port_pp_pd_t pull_up_down,
                                 bsp_port_speed_t speed)
{
    return __configure_io_ports(port, pin_number, INPUT, 0U, pull_up_down, speed, BSP_IO_OUT_TYPE_PP);
}

ret_status bio_config_af_port(bio_port *port,
                              bsp_io_pin_number pin_number,
                              uint8_t af_function,
                              bsp_port_pp_pd_t pull_up_down,
                              bsp_port_speed_t speed,
                              bsp_port_output_type_t output_type)
{
    return __configure_io_ports(port, pin_number, ALTERNATE, af_function, pull_up_down, speed, output_type);
}

ret_status bio_config_analog_port(bio_port *port, bsp_io_pin_number pin_number, bsp_port_pp_pd_t pull_up_down)
{
    return __configure_io_ports(port, pin_number, ANALOG, 0U, pull_up_down, 0U, 0U);
}

void bio_toggle_port(bio_port *port, uint8_t pin_number)
{
    if (BSP_IO_IS_PIN_VALID(pin_number)) {
        uint16_t pin_bin = 1 << pin_number;
        port->BSRR = (port->ODR & pin_bin) != 0x00U ? (uint32_t)pin_bin << 16ul : (uint32_t)pin_bin;
    }
}

ret_status bio_write_port(bio_port *port, uint8_t pin_number, bool value)
{

    if (!BSP_IO_IS_PIN_VALID(pin_number)) {
        return STATUS_ERR;
    }

    if (value == 0) {
        __BSP_SET_MASKED_REG(port->BRR, 0x01U << pin_number);
    } else {
        __BSP_SET_MASKED_REG(port->BSRR, 0x01U << pin_number);
    }
    return STATUS_OK;
}

uint8_t bio_read_port(bio_port *port, uint8_t pin_number)
{
    uint8_t res = 0;
    if (BSP_IO_IS_PIN_VALID(pin_number)) {
        res = (uint8_t)((port->IDR & (uint32_t)(1 << pin_number)) >> pin_number);
    }
    return res;
}

ret_status __configure_io_ports(bio_port *port,
                                bsp_io_pin_number pin_number,
                                bsp_port_io_type_t io_type,
                                uint8_t af_function,
                                bsp_port_pp_pd_t pull_up_down,
                                bsp_port_speed_t speed,
                                bsp_port_output_type_t output_type)
{
    if (!__BSP_IO_IS_PIN_VALID(pin_number)) {
        return STATUS_ERR;
    }

    /* RM0440 indicates that for ANALOG pins only Pull-Down or no weak resistors can be configured */
    if (io_type == ANALOG && pull_up_down != BSP_IO_NO_PU_PD && pull_up_down != BSP_IO_PD) {
        return STATUS_ERR;
    }

    for (uint8_t pin_n = 0; pin_n < BSP_IO_PORT_LENGTH; pin_n++) {
        /* Check if the current pin_n needs to be configured */
        if (pin_number & (1 << (pin_n & 0x0F))) {

            /* First configure de IO type by using the MODER register */
            __BSP_SET_MASKED_REG_VALUE(port->MODER, 0x03UL << (pin_n * 2), (io_type & 0x3UL) << (pin_n * 2));

            /* Configure Pull-Up / Pull-Down resistors */
            __BSP_SET_MASKED_REG_VALUE(port->PUPDR, 0x03UL << (pin_n * 2), (pull_up_down & 0x3UL) << (pin_n * 2));

            /* If pins are declared as output or AF configure speed and output type */
            if (io_type == OUTPUT || io_type == ALTERNATE) {
                __configure_output_characteristics(port, pin_n, speed, output_type);
            } else {
                /* Reset to input defaults. Note: PA14 has a special speed default value */
                __configure_output_characteristics(
                    port, pin_n, port == GPIOA && pin_n == 14 ? BSP_IO_VERY_HIGH : BSP_IO_LOW, BSP_IO_OUT_TYPE_PP);
            }

            /* TODO: REVIEW THIS COMMENTED CODE Setup alternate MUX */
            /*_BSP_SET_MASKED_REG_VALUE(port->AFR[pin_n >> 3U], 0x0FU << (pin_n * 4),
                                       io_type == ALTERNATE ? (af_function & 0x0FU) << (pin_n * 4) : 0x00U);*/
            uint32_t temp;
            temp = port->AFR[pin_n >> 3U];
            temp &= ~(0xFU << ((pin_n & 0x07U) * 4U));
            temp |= ((af_function) << ((pin_n & 0x07U) * 4U));
            port->AFR[pin_n >> 3U] = temp;
        }
    }

    return STATUS_OK;
}

static void __configure_output_characteristics(bio_port *port,
                                               bsp_io_pin_number pin_number,
                                               bsp_port_speed_t speed,
                                               bsp_port_output_type_t output_type)
{
    /* Configure speed */
    __BSP_SET_MASKED_REG_VALUE(port->OSPEEDR, 0x03UL << (pin_number * 2), (speed & 0x3UL) << (pin_number * 2));

    /* Configure Push-Pull or Open Drain output mode */
    __BSP_SET_MASKED_REG_VALUE(port->OTYPER, 0x01 << pin_number, (output_type & 0x01U) << pin_number);
}
