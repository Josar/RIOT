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

#include <stdlib.h>

#define ENABLE_DEBUG 		(0)
#include "debug.h"


#define F_CPU	8000000
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
/*
 * Helper function
 * will return 0 if a xtimer is running with less than a second to go so we don't go to sleep
 * will return 1 if the timer runs long enough to go to sleep
 */
uint8_t __goto_sleep_xtimer(xtimer_t* timer)
{
		xtimer_ticks32_t ticks_in_sec = xtimer_ticks_from_usec(1000000);
		uint32_t now = 0;
		while(timer != NULL){
			DEBUG("timer: %p Timer-short: %lu Timer-long %lu \n", timer, timer->long_target, timer->target);
			now = xtimer_now().ticks32;
			if((timer->target-now)< ticks_in_sec.ticks32){
				//one xtimer has less than a second to run -> dont go to sleep
				DEBUG("Dont sleep \n");
				return 0;
			}
			timer = timer->next;
		}
		//printf("Diff %lu \n", timer->target-now);
		return 1;
}

static inline void __correct_sleep_xtimer(void)
{
	extern volatile uint32_t _xtimer_high_cnt;
	//uint32_t cnt_now = timer_read(XTIMER_DEV);
	//printf("old cnt4 %x old-high %lx \n", TCNT4, _xtimer_high_cnt);
	if(timer_read(XTIMER_DEV) < 6072){
		//overflow
		TCNT4 += xtimer_ticks_from_usec(1000000).ticks32;
		_xtimer_high_cnt += 0x10000;
	}else{
		//2 overflows
		TCNT4 += xtimer_ticks_from_usec(1000000).ticks32;
		_xtimer_high_cnt += 0x20000;
	}
	//printf("new cnt4 %x new-high %lx \n", TCNT4, _xtimer_high_cnt);
}

void pm_set_lowest(void) {
	/*printf("pm_set_lowest \n");
	_delay_ms(100);*/
	//DEBUG("pm_set_lowest\n");

	// printf("pm_set_lowest\n");
	/* White LED ON. */
	//LED_PORT &= ~(BLUE|GREEN|RED);

	/* White LED OFF. */
	// All LED OFF
	// LED_PORT |= BLUE|GREEN|RED;
	/*TODO implement power save Modes*/

/*	#ifdef RTC_NUMOF
			//Dumy write to Asyncrone Timer 2/RTC, makes sure Interrupt flag is cleared and powermode can be set again
			TCCR2B = (1<<CS22) | (1<<CS20);
			//TCCR2B = 0x07;
			while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB)));
	#endif*/

	//DEBUG("Going to sleep know!\n");
		//DEBUG("#");
	//PRR1 &= ~((1<<PRTIM5)|(1<<PRTIM4)|(1<<PRTIM3)|(1<<PRUSART1)|(1<<PRTRX24));
	//PRR0 &= ~((1<<PRTIM0)|(1<<PRTIM1)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRADC));
/*	TRXPR = 1 << SLPTR; // sent transceiver to sleep
	MCUCR |= (1<<JTD);
	MCUCR |= (1<<JTD);*/
			//printf("sleep now \n");
	//printf("long-target-long %lu long-target %lu xtimer_now %lu \n",get_xtimer_head_long()->long_target,get_xtimer_head_long()->target, xtimer_now().ticks32);
/*uint8_t goto_sleep_long, goto_sleep_short;
	#ifdef XTIMER_DEV
			goto_sleep_long = __goto_sleep_xtimer(get_xtimer_head_long());
			goto_sleep_short= __goto_sleep_xtimer(get_xtimer_head());
	#elif
			goto_sleep_long = 1;
			goto_sleep_short = 1;
	#endif
	if(goto_sleep_long && goto_sleep_short){
		printf("sleep \n");*/
	/*_delay_ms(5);
	//printf("long-target-long %lu long-target %lu xtimer_now %lu \n",get_xtimer_head_long()->long_target,get_xtimer_head_long()->target, xtimer_now().ticks32);
		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		sleep_mode();
		__asm__("nop");
		__asm__("nop");
		__asm__("nop");
		#ifdef XTIMER_DEV
		__correct_sleep_xtimer();
			//	DEBUG("After timer_long %lu timer %lu \n", get_xtimer_head_long()->target, get_xtimer_head()->target);
		#endif*/
		/*	DEBUG("after sleep %lu \n", xtimer_now().ticks32);
	}*/
		//DEBUG("wake up \n");
	//DEBUG("return \n");
}
