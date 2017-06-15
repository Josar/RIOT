/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief       Implementation of common AVR periph/pm functions
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author		 Steffen Robertz <steffen.robertz@rwth-aachen.de>
 *
 * @}
 */

#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdio.h>
#include "irq.h"
#include "periph/pm.h"
#include "xtimer.h"

#include "board.h"

#include "xtimer.h"

#define ENABLE_DEBUG 		(1)
#include "debug.h"



#define F_CPU	16000000
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

void pm_off(void)
{
	// TODO
	/* White LED ON. */
	LED_PORT &= ~(BLUE|GREEN|RED);

	/* White LED OFF. */
	// All LED OFF
	LED_PORT |= BLUE|GREEN|RED;

	printf("pm_off\n");
}

void pm_reboot(void)
{
    /*
     * Since the AVR doesn't support a real software reset, we set the Watchdog
     * Timer on a 250ms timeout. Consider this a kludge.
     */
    irq_disable();
    wdt_enable(WDTO_250MS);
    while(1);
}

void pm_set_lowest(void) {

	DEBUG("pm_set_lowest\n");

	// printf("pm_set_lowest\n");
	/* White LED ON. */
	//LED_PORT &= ~(BLUE|GREEN|RED);

	/* White LED OFF. */
	// All LED OFF
	// LED_PORT |= BLUE|GREEN|RED;
	/*TODO implement power save Modes*/

	#ifdef RTC_NUMOF
			/*Dumy write to Asyncrone Timer 2/RTC, makes sure Interrupt flag is cleared and powermode can be set again */
			TCCR2B = (1<<CS22) | (1<<CS20);
			while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB)));
	#endif

	uint8_t goto_sleep = 1;
	//#ifdef XTIMER_DEV
		/*	printf("xtimer \n");
			xtimer_ticks32_t ticks_in_sec = xtimer_ticks_from_usec(1000000);
			xtimer_t* timer = get_xtimer_head();
			xtimer_ticks32_t target;
			printf("before while \n");
			while(timer != NULL){
				printf("Timer: %p \n", timer);
				target.ticks32 = timer-> target;
				if(xtimer_diff(target, xtimer_now()).ticks32 < ticks_in_sec.ticks32){
					//one xtimer has less than a second to run -> dont go to sleep
					goto_sleep = 0;
					printf("Sleep to short");
				}
				timer = timer->next;
			}*/
	//#endif

	DEBUG("Going to sleep know!\n");
	#ifdef UART_NUMOF
		/* Test for empty output stream, so that all UART communication is send */
		while(!feof(stdout));
	#endif

	PRR1 &= ~((1<<PRTIM5)|(1<<PRTIM4)|(1<<PRTIM3)|(1<<PRUSART1)|(1<<PRTRX24));
	PRR0 &= ~((1<<PRTIM0)|(1<<PRTIM1)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRADC));
	TRXPR = 1 << SLPTR; // sent transceiver to sleep

	if(goto_sleep) {
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
	}
}
