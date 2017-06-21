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

#define F_CPU   32000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

	while(1){
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

		 _delay_ms( 500 );
	};

    return 0;
}
