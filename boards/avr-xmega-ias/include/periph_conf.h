/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2014 Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pinoccio-mega256rfr2
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Pinoccio Mega 256rfr2 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */
#include "mutex.h"
#include "periph_cpu.h"

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "periph_cpu.h"
#include "atmega_regs_common.h"
#include "periph_cpu_common.h"
#include <stdint.h>
#include <avr/io.h>
/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000L)
/** @} */





/**
 * @brief xtimer configuration values
 * @{
 */

//
#define XTIMER_DEV					TIMER_DEV(0)   	// set ctx[0] as system counter this is MEGA_TIMER4
#define XTIMER_CHAN 				(0)				// choose channel 0
#define XTIMER_WIDTH                (16)			// 16bit timer
#define XTIMER_SHIFT                (2)				// xtimer prescaler value, If the underlying hardware timer is running at a power of two multiple ofFor a 16 MHz hardware timer, set XTIMER_SHIFT to 4.
#define XTIMER_HZ                   (250000UL)	// set Timer frequency TODO think about slowing down timer for power saving

//#define XTIMER_SHIFT                (4)				// xtimer prescaler value, If the underlying hardware timer is running at a power of two multiple ofFor a 16 MHz hardware timer, set XTIMER_SHIFT to 4.
//#define XTIMER_HZ                   (16000000UL)	// set Timer frequency TODO think about slowing down timer for power saving

#define XTIMER_BACKOFF              (40)			// TODO look into this , All timers that are less than XTIMER_BACKOFF microseconds in the future willjust spin. This is supposed to be defined per-device in e.g., periph_conf.h.
/** @} */


/**
 * @brief   Timer configuration
 *
 * ATTETION Timer 0 is used for Xtimer which is system Timer
 *
 * The timer driver only supports the four 16-bit timers
 * (Timer1, TST, Timer3, Timer4, Timer5), so those are the Timer we can use here.
 * Timer 1, TST, PORT B5/6/7 Out, Port D4/6 In,  Analog Comparator Input Capture, Output Compare Modulator, PWM
 * Timer 3, TST, PORT E3/4/5 Out, Port E/6/7 In, Input or Output Compare and PWM Output
 * Timer 4, TST, It can not be connected to any I/O Pin,
 * Timer 5, TST, It can not be connected to any I/O Pin,
 *
 * Using Timer 4 and 5 seems to be the best choice
 * TODO Change the timer
 * @{
 */
#define TIMER_NUMOF         (2U)

#define TIMER_0             MEGA_TIMER4
#define TIMER_0_MASK        &TIMSK4
#define TIMER_0_FLAG        &TIFR4
#define TIMER_0_ISRA        TIMER4_COMPA_vect
#define TIMER_0_ISRB        TIMER4_COMPB_vect
#define TIMER_0_ISRC        TIMER4_COMPC_vect

#define TIMER_1             MEGA_TIMER5
#define TIMER_1_MASK        &TIMSK5
#define TIMER_1_FLAG        &TIFR5
#define TIMER_1_ISRA        TIMER5_COMPA_vect
#define TIMER_1_ISRB        TIMER5_COMPB_vect
#define TIMER_1_ISRC        TIMER5_COMPC_vect

/** @} */




/**
 * @brief   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (1U)

// UART0 is used for stdio
#define UART_0              XMEGA_UART0
#define UART_0_ISR          XMEGA_UART0_RX_ISR


/** @} */



#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
