/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
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
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "xtimer.h"
#include "periph/rtc.h"
#include "board.h"




//#include <stdlib.h>

/* set interval to 1 second */
#define INTERVAL (2 * US_PER_SEC)

int main(void)
{
	LED_PORT &= ~(RED|GREEN|BLUE);
	/*printf("start \n");
	LED_PORT &= ~(RED|GREEN|BLUE);
	LED_PORT |= GREEN;
	printf("Green");
	xtimer_sleep(1);
	LED_PORT &= ~GREEN ;
	LED_PORT |= RED;
	printf("red");
	xtimer_sleep(1);
	LED_PORT &= ~RED ;
	LED_PORT |= BLUE;
	printf("stop\n");
	xtimer_sleep(1);
	printf("slept\n");*/
	rtc_init();
    puts("Pinoccio Hello World!");
   //extern volatile uint32_t _xtimer_high_cnt;
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);
    xtimer_ticks32_t last_wakeup = xtimer_now();

    while(1) {
    	//printf("xtrimer now: %lu \n", xtimer_now().ticks32);
    	xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
    	//printf("next wakeup: %lu", get_xtimer_head_long()->long_target);
    	//printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));
    	printf("slept until %" PRIu32 "\n", xtimer_now());
        // Toggle red led
        LED_PORT ^= RED;
    }
    return 0;
}
