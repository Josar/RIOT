/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common
 * @{
 *
 * @file
 * @brief       Common implementations for Atmega boards
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "periph/gpio.h"
#include "periph/init.h"

#ifndef CPU_ATMEGA_CLK_SCALE_INIT
#define CPU_ATMEGA_CLK_SCALE_INIT    CPU_ATMEGA_CLK_SCALE_DIV1
#endif

//void led_init(void);

void board_init(void)
{
    atmega_set_prescaler(CPU_ATMEGA_CLK_SCALE_INIT);
    cpu_init();
    led_init();
    atmega_stdio_init();
    /* Initialize peripherals for which modules are included in the makefile.*/
    /* I2C, SPI, RTC, RTT, HWRNG */
    periph_init();
    irq_enable();
}
