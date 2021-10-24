/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#ifndef FW_COMMON_TYPES_H
#define FW_COMMON_TYPES_H

#define __IO32 volatile uint32_t
#define __REG32_T(ADDR) (*(volatile uint32_t *)(ADDR))

typedef void (*bsp_cmn_void_cb)(void);

#endif // FW_COMMON_TYPES_H
