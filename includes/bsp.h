/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */


#ifndef BSP_H
#define BSP_H

#include "bsp_config.h"
#include "bsp_irq_manager.h"
#include "bsp_clocks.h"
#include "bsp_io.h"
#include "bsp_usart.h"
#include "bsp_i2c.h"
#include "bsp_can.h"


void BSP_init(void);

#endif //BSP_H
