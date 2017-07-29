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

#define ASYNC_BACKOFF 			(3UL)
#define PM_OVERHEAD				(500UL)
#define SLEEP_OVERHEAD		(50UL)
uint32_t smallest_diff = 0xffffffff;

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
	if(timer->target != 0){
		uint32_t now = 0;
		//DEBUG("timer: %p Timer-short: %lu Timer-long %lu \n", timer, timer->target, timer->long_target);
		now = xtimer_now().ticks32;
		DEBUG("long-target: %lu now: %lu diff: %lx ticks-sec: %lx \n", timer->target, now, (timer->target-now), xtimer_ticks_from_usec(1000000).ticks32);
		uint32_t diff = timer->target-now;
		if(diff< (ASYNC_BACKOFF*500+PM_OVERHEAD) ){
			//one xtimer has less than a second to run -> dont go to sleep
			//DEBUG("Dont sleep \n");
			//cli();
			//DEBUG("Before while: target %lu \n", timer->target);

		        //sei();
			return 0;
		}
		if(diff < smallest_diff){
			smallest_diff= diff;
			DEBUG("smallest-diff Target: %lu now: %lu diff: %lx  \n", timer->target, now, (timer->target-now));
		}
	}
		//printf("Diff %lu \n", timer->target-now);
		return 1;
}

static inline void __correct_sleep_xtimer(uint32_t xtimer_ticks)
{
	extern volatile uint32_t _xtimer_high_cnt;
	//uint32_t cnt_now = timer_read(XTIMER_DEV);
	//DEBUG("old cnt4 %u old-high %lx \n", TCNT4, _xtimer_high_cnt);
	uint8_t cnt_ovf = (timer_read(XTIMER_DEV) + xtimer_ticks) / (~XTIMER_MASK+1);
	TCNT4 += xtimer_ticks;
	_xtimer_high_cnt += 0x10000 * cnt_ovf;
	if((get_xtimer_head_long()->target &XTIMER_MASK) == _xtimer_high_cnt){
		_add_timer_to_list(get_xtimer_head_pointer(), get_xtimer_head_long());
		*(get_xtimer_head_pointer_long()) = get_xtimer_head_long()->next;
		DEBUG("Fix list \n");
		if(get_xtimer_head()->target-xtimer_now().ticks32 > 2000){
			DEBUG("Set compare for %u \n", 0xffff&get_xtimer_head()->target);
			timer_set_absolute(XTIMER_DEV, XTIMER_CHAN, get_xtimer_head()->target-XTIMER_OVERHEAD);
			timer_start(XTIMER_DEV);
		}else{
				xtimer_t* timer = get_xtimer_head();
				timer_start(XTIMER_DEV);
			while(timer->target-10 > xtimer_now().ticks32);
						DEBUG("Manual shoot target %lu now: %lu \n", timer->target, xtimer_now().ticks32);
					    /* advance list */
					        *get_xtimer_head_pointer() = (*get_xtimer_head_pointer())->next;

					        /* make sure timer is recognized as being already fired */
					        timer->target = 0;
					        timer->long_target = 0;

					        /* fire timer */
					        timer->callback(timer->arg);
					        DEBUG("after callback lnglngtarget %lu lng-target %lu target-lng %lu targer %lu now: %lu \n", get_xtimer_head_long()->long_target, get_xtimer_head_long()->target, get_xtimer_head()->long_target, get_xtimer_head()->target, xtimer_now().ticks32);

		}
	}
	timer_start(XTIMER_DEV);
	//DEBUG("new cnt4 %u new-high %lx cnt-ovf %u added-ticks %lu \n", TCNT4, _xtimer_high_cnt, cnt_ovf, xtimer_ticks);
}

void pm_set_lowest(void) {
	//DEBUG("pm_set_lowest \n");
	//DEBUG("pm_set_lowest\n");

	// printf("pm_set_lowest\n");
	/* White LED ON. */
	//LED_PORT &= ~(BLUE|GREEN|RED);

	/* White LED OFF. */
	// All LED OFF
	// LED_PORT |= BLUE|GREEN|RED;
	/*TODO implement power save Modes*/

	//DEBUG("Going to sleep know!\n");
		//DEBUG("#");
	//PRR1 &= ~((1<<PRTIM5)|(1<<PRTIM4)|(1<<PRTIM3)|(1<<PRUSART1)|(1<<PRTRX24));
	//PRR0 &= ~((1<<PRTIM0)|(1<<PRTIM1)|(1<<PRSPI)|(1<<PRUSART0)|(1<<PRADC));
	TRXPR = 1 << SLPTR; // sent transceiver to sleep
	MCUCR |= (1<<JTD);
	MCUCR |= (1<<JTD);
			//printf("sleep now \n");
	//printf("long-target-long %lu long-target %lu xtimer_now %lu \n",get_xtimer_head_long()->long_target,get_xtimer_head_long()->target, xtimer_now().ticks32);
	uint8_t goto_sleep_long, goto_sleep_short;
	#ifdef XTIMER_DEV
			goto_sleep_long = __goto_sleep_xtimer(get_xtimer_head_long());
			goto_sleep_short= __goto_sleep_xtimer(get_xtimer_head());
	#elif
			goto_sleep_long = 0;
			goto_sleep_short = 0;
	#endif
			//timer_stop(XTIMER_DEV);
	if(goto_sleep_long && goto_sleep_short){
		#ifdef RTC_NUMOF
		//Dumy write to Asyncrone Timer 2/RTC, makes sure Interrupt flag is cleared and powermode can be set again
		TCCR2B = (1<<CS22) | (1<<CS20);
		TCNT2 = 0;
		TIFR2 |= (1<<TOV2) |  (1<<OCF2A); //clear interrupt flag, this shouldnt be needed, but for some reason won't work without
			while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB)));
		#endif
			DEBUG("Smallest diff: %lu \n", smallest_diff);
		if(smallest_diff != 0xffffffff && (smallest_diff) < (xtimer_ticks_from_usec(1000000).ticks32 + ASYNC_BACKOFF*489)){
			//smallest value is known as xtimer ticks, set compare match for Timer2 ticks
			smallest_diff -= PM_OVERHEAD;
			TIMSK2 |= (1<<OCIE2A);
			 uint8_t ticks_async =smallest_diff/488;
			 OCR2A = ticks_async-ASYNC_BACKOFF;
			 smallest_diff =((uint32_t)OCR2A)*488;
			 DEBUG("Async-ticks %u smallest_diff: %lu \n", OCR2A, smallest_diff);
			 DEBUG("new smallest diff %lu \n", smallest_diff);
			 DEBUG("short sleep \n");
		}else{
			smallest_diff = xtimer_ticks_from_usec(1000000).ticks32;
			DEBUG("long sleep \n");
		}

		while(stdout->size);
	//printf("long-target-long %lu long-target %lu xtimer_now %lu \n",get_xtimer_head_long()->long_target,get_xtimer_head_long()->target, xtimer_now().ticks32);
		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		timer_stop(XTIMER_DEV);
		sleep_mode();
		__asm__("nop");
		__asm__("nop");
		__asm__("nop");
		#ifdef XTIMER_DEV
		__correct_sleep_xtimer(smallest_diff);
		smallest_diff = 0xffffffff; //reset smallest_diff
			//	DEBUG("After timer_long %lu timer %lu \n", get_xtimer_head_long()->target, get_xtimer_head()->target);
		#endif
		//thread_yield();
		//	DEBUG("after sleep %lu \n", xtimer_now().ticks32);
	}
	///DEBUG("End: long-target: %lu target-short: %lu now: %lu  ticks-sec: %lx \n", get_xtimer_head_long()->target,get_xtimer_head()->target, xtimer_now().ticks32,  xtimer_ticks_from_usec(1000000).ticks32);
	//timer_start(XTIMER_DEV);
		//DEBUG("wake up \n");
	//DEBUG("return \n");
}
ISR(TIMER2_COMPA_vect)
{
	TIMSK2 &= ~(1<<OCIE2A);
}

