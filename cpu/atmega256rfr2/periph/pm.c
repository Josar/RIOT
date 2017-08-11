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

typedef enum{
	SLEEP_LONG = 1,
	SLEEP_SHORT = 0
}sleep_state_t;

sleep_state_t sleep_state;

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

static inline uint8_t __goto_sleep_xtimer(void)
{
	xtimer_t* timer = get_xtimer_head_long();
	if(timer != NULL){
			//DEBUG("timer: %lx %lx \n", timer->target, timer->long_target);
			//DEBUG("target: %lx xtimer-now: %lx diff %lx \n", timer->target, xtimer_now().ticks32, timer->target-xtimer_now().ticks32);
			if((timer->target-xtimer_now().ticks32) > xtimer_ticks_from_usec(1600000).ticks32){
				//one overflow of xtimer will occure -> just sleep one second
				DEBUG("long SLeep \n");
				sleep_state = SLEEP_LONG;
				return SLEEP_LONG;
			}
	}
	return 0;
}

static inline void __correct_sleep_xtimer(sleep_state_t sleep_state)
{
	#ifdef XTIMER_DEV
	DEBUG("Xtimer-now1: %lx \n", xtimer_now().ticks32);
	extern volatile uint32_t _xtimer_high_cnt;
	uint32_t slept_ticks = 0;
	if(sleep_state == SLEEP_LONG){
		slept_ticks = xtimer_ticks_from_usec(1000000).ticks32;
	}
	uint32_t cnt_adjust = (TCNT4 + slept_ticks) / 0xffff;
	DEBUG("cnt adjust %u \n", cnt_adjust);
	TCNT4 += slept_ticks;
	_xtimer_high_cnt += (~XTIMER_MASK +1) * cnt_adjust;
	DEBUG("Xtimer-now2: %lx \n", xtimer_now().ticks32);
	#endif
}

static inline void _dummy_async_write(void)
{
	#ifdef RTC_NUMOF
		//Dumy write to Asyncrone Timer 2/RTC, makes sure Interrupt flag is cleared and powermode can be set again
		TCCR2B &= ~((1<<CS22) | (1<<CS20));
		TCNT2 = 0;
		if(sleep_state == SLEEP_SHORT){
			TIMSK2 |=( 1<<OCIE2A);
		}
		TIFR2 &= ~(1<<TOV2);
		TCCR2B |= (1<<CS22) | (1<<CS20);
			while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB)));
		#endif
}

void pm_set_lowest(void) {
	if(__goto_sleep_xtimer()){
		DEBUG("Sleep now \n");
		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		while(stdout->size);
		timer_stop(XTIMER_DEV);
		_dummy_async_write();
		sleep_mode();
		__correct_sleep_xtimer(sleep_state);
		timer_start(XTIMER_DEV);
	}
	//__goto_sleep_xtimer();
}

ISR(TIMER2_COMPA_vect){

}
