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


#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

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

#define XTIMER_DEV					TIMER_DEV(0)   	// set ctx[0] as system counter
#define XTIMER_CHAN 				(0)				// choose channel 0
#define XTIMER_WIDTH                (16)			// 16bit timer
#define XTIMER_HZ                   (125000)	// set Timer frequency TODO think about slowing down timer for power saving
#define XTIMER_SHIFT                (3) // xtimer prescaler
#define XTIMER_BACKOFF              (40)			// TODO look into this , All timers that are less than XTIMER_BACKOFF microseconds in the future willjust spin. This is supposed to be defined per-device in e.g., periph_conf.h.
/** @} */


/**
 * @brief   Timer configuration
 *
 * ATTETION Timer 0 is used for Xtimer which is system Timer
 *
 * RIOT Timer 0 is Timer Counter D1
 * @{
 */
#define TIMER_NUMOF         (1U)

#define TIMER_0             (&TCD1)
#define TIMER_0_MASK 		()
#define TIMER_0_FLAG		()
#define TIMER_0_OVF         TCD1_OVF_vect
#define TIMER_0_ISRA        TCD1_CCA_vect
#define TIMER_0_ISRB        TCD1_CCB_vect
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
#define UART_0              (&USARTC1)
#define UART_0_RXC_ISR		USARTC1_RXC_vect /* Reception Complete Interrupt */
#define UART_0_DRE_ISR		USARTC1_DRE_vect /* Data Register Empty Interrupt */
#define UART_0_TXC_ISR		USARTC1_TXC_vect /* Transmission Complete Interrupt */
/** @} */



#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
