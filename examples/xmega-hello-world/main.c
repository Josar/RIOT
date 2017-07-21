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

    // xtimer_ticks32_t last_wakeup = xtimer_now();



	    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
	    printf("This board features a(n) %s MCU.\n", RIOT_MCU);
		if( (PORTF.IN & PIN3_bm)!=0 ){
			  PORTF.OUTSET = PIN2_bm ;

			  PORTF.OUTCLR = PIN3_bm ;
		 }else
		 {
			 PORTF.OUTCLR = PIN2_bm ;
			 PORTF.OUTSET = PIN3_bm ;
		 }

		// 6983034
		_delay_us(795770);


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

		int i=4;

//		PORTA.DIR |= PIN1_bm;
//		PORTA.INT0MASK = PIN1_bm;
//		PORTA.PIN1CTRL	= PORT_OPC_WIREDORPULL_gc | PORT_ISC_BOTHEDGES_gc;
//		PORTA.INTCTRL	= PORT_INT0LVL_HI_gc;
//		PMIC.CTRL|=PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
//		sei();

		while(i--) {

			//time.ticks32 = 3000000;

//			 xtimer_tsleep32(time);

			 xtimer_sleep(5);

			PORTF.OUTCLR = PIN3_bm ;
			PORTF.OUTCLR = PIN2_bm ;
			PORTE.OUTCLR = PIN7_bm ;
			PORTE.OUTCLR = PIN6_bm ;
//
//			time.ticks32 = 65535;
//			xtimer_tsleep32(time);
		}

		while(1);

return 0;
}
