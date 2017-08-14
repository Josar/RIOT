/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <board.h>

#include "xtimer.h"
#include "timex.h"

#include "arch/thread_arch.h"

#define F_CPU   32000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include "mutex.h"


/* set interval to 1 second */
#define INTERVAL (1U * US_PER_SEC)

// #define INTERVAL (100000) // us

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

	PORTF.OUTCLR = PIN3_bm ;
	PORTF.OUTCLR = PIN2_bm ;
	PORTE.OUTCLR = PIN7_bm ;
	PORTE.OUTCLR = PIN6_bm ;

//		uint8_t i=6;
	while(1){
		PORTF.OUTSET = PIN3_bm ;
		 PORTA.OUTTGL=PIN1_bm;      // works to thread_arch_yield and AVR_CONTEXT_SWAP_TRIGGER  but hangs
		// AVR_CONTEXT_SWAP_TRIGGER;  // works to thread_arch_yield and AVR_CONTEXT_SWAP_TRIGGER  but hangs


		// thread_arch_yield();       // works to call interrupt and AVR_CONTEXT_SWAP_TRIGGER does not hang.

		// xtimer_usleep64(500); // thread_arch_yield but no  AVR_CONTEXT_SWAP_TRIGGER is not fired
		 // xtimer_sleep(1);         // thread_arch_yield but no AVR_CONTEXT_SWAP_TRIGGER
		_delay_ms(500);
	}


//	    if( (PORTF.IN & PIN3_bm)!=0 ){
//			  PORTF.OUTSET = PIN2_bm ;
//
//			  PORTF.OUTCLR = PIN3_bm ;
//		 }else
//		 {
//			 PORTF.OUTCLR = PIN2_bm ;
//			 PORTF.OUTSET = PIN3_bm ;
//		 }
//
//		// 6983034
//		_delay_us(795770);


		printf("Now %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));

		_delay_ms(500);

		//xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

//		xtimer_ticks32_t time;
		//time.ticks32 = 30000;
		//xtimer_tsleep32(time);




		printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));

//	    while(1) {
//	        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
//	        printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));
//	    }


//		int i=4;
//
////		PORTA.DIR |= PIN1_bm;
////		PORTA.INT0MASK = PIN1_bm;
////		PORTA.PIN1CTRL	= PORT_OPC_WIREDORPULL_gc | PORT_ISC_BOTHEDGES_gc;
////		PORTA.INTCTRL	= PORT_INT0LVL_HI_gc;
////		PMIC.CTRL|=PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
////		sei();
//
//		while(i--) {
//
//			//time.ticks32 = 3000000;
//
////			 xtimer_tsleep32(time);
//
//			 xtimer_sleep(5);
//
//			PORTF.OUTCLR = PIN3_bm ;
//			PORTF.OUTCLR = PIN2_bm ;
//			PORTE.OUTCLR = PIN7_bm ;
//			PORTE.OUTCLR = PIN6_bm ;
////
////			time.ticks32 = 65535;
////			xtimer_tsleep32(time);
//		}


		sei();
		PORTF.OUTCLR = PIN3_bm ;
		PORTF.OUTCLR = PIN2_bm ;
		PORTE.OUTCLR = PIN7_bm ;
		PORTE.OUTCLR = PIN6_bm ;

//		uint8_t i=6;
		while(1){
			PORTF.OUTTGL = PIN3_bm ;
			_delay_ms(500);
		}

		while(1){

			xtimer_sleep(1);

			PORTF.OUTCLR = PIN3_bm ;
			PORTF.OUTCLR = PIN2_bm ;
			PORTE.OUTCLR = PIN7_bm ;
			PORTE.OUTCLR = PIN6_bm ;

			printf("Call AVR_CONTEXT_SWAP_TRIGGER\n");
			_delay_ms(1000);
			AVR_CONTEXT_SWAP_TRIGGER;
			_delay_ms(1000);
			PORTE.OUTCLR = PIN6_bm ;
			_delay_ms(500);

			printf("Call thread_arch_yield\n");
			_delay_ms(1000);
			thread_arch_yield();
			_delay_ms(1000);
			PORTE.OUTCLR = PIN7_bm ;
			PORTE.OUTCLR = PIN6_bm ;
			_delay_ms(500);

			printf("Call thread_yield_higher\n");
			_delay_ms(1000);
			thread_yield_higher();
			_delay_ms(1000);
			PORTE.OUTCLR = PIN7_bm ;
			PORTE.OUTCLR = PIN6_bm ;
			_delay_ms(500);

			PORTA.OUTSET=PIN1_bm;

		};

return 0;
}


