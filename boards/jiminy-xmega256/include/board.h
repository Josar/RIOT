/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_jiminy-mega256rfr2
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Jiminy Mega 256rfr2 board.
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include <util/delay_basic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000ul)

void _wait_ms(uint16_t count);
/* F_CPU definition for <util/delay.h> _delay_ms() */
#define F_CPU CLOCK_CORECLOCK

/**
 * @name   Baudrate for STDIO terminal
 *
 * The standard configuration for STDIO in spu/atmega_comman/periph/uart.c
 * is to use double speed.
 *
 * For 8MHz F_CPU following Baudrate have good error rates
 *  76923
 *  38400
 *
 * Matches this with BAUD in Board/Makefile.include
 *
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (38400U)       /**< Sets Baudrate for e.g. Shell */
#endif
/** @} */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PORTF

#define LED0_PIN            GPIO_PIN(5, 2)
#define LED1_PIN            GPIO_PIN(5, 3)

#define LED0_MASK           (PIN2_bm)
#define LED1_MASK           (PIN3_bm)

#define LED0_ON             (LED_PORT |=  LED0_MASK)
#define LED0_OFF            (LED_PORT &= ~LED0_MASK)
#define LED0_TOGGLE         (LED_PORT ^=  LED0_MASK)

#define LED1_ON             (LED_PORT |=  LED1_MASK)
#define LED1_OFF            (LED_PORT &= ~LED1_MASK)
#define LED1_TOGGLE         (LED_PORT ^=  LED1_MASK)
/** @} */

/**
 * @name xtimer configuration values
 * if XTIMER_HZ > 1MHz then (XTIMER_HZ != (1000000ul << XTIMER_SHIFT))
 * if XTIMER_HZ < 1MHz then ((XTIMER_HZ << XTIMER_SHIFT) != 1000000ul)
 *
 * 32MHz Core Clock
 * XTIMER_HZ 4000000 (clkdiv 8 )    XTIMER_SHIFT 2
 * XTIMER_HZ 1000000 ()             XTIMER_SHIFT 0
 * XTIMER_HZ  500000 (clkdiv 64)    XTIMER_SHIFT 1
 * XTIMER_HZ  250000 (clkdiv 128)   XTIMER_SHIFT 2
 * XTIMER_HZ   31250 (clkdiv 1024)  XTIMER_SHIFT 5
 *
 * @{
 */

#define XTIMER_DEV					TIMER_DEV(0)   	// set ctx[0] as system counter
#define XTIMER_CHAN 				(0)				// choose channel 0
#define XTIMER_WIDTH                (16)			// 16bit timer
#define XTIMER_HZ                   (4000000UL)		// set Timer frequency TODO think about slowing down timer for power saving
#define XTIMER_BACKOFF              (150)			// TODO look into this , All timers that are less than XTIMER_BACKOFF microseconds in the future willjust spin. This is supposed to be defined per-device in e.g., periph_conf.h.
/** @} */
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

/**
 * @brief   Initialize board specific hardware LEDs
 */
void led_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
