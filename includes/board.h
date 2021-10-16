/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */


#ifndef BOARD_H
#define BOARD_H

#include "bsp_tick.h"
#include "bsp_irq_manager.h"
#include "bsp_clocks.h"
#include "bsp_io.h"
#include "bsp_usart.h"
#include "bsp_i2c.h"
#include "bsp_can.h"
#include "bsp_adc.h"

void board_init(void);

#endif //BOARD_H
