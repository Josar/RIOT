/*
 * Copyright (C) 2018 Josau Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       Implementation of common AVR periph/pm functions
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "periph_conf.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


void pm_off(void)
{
	// TODO
	printf("pm_off\n");
}

void pm_reboot(void)
{
    DEBUG("Reboot Software Reset\n" );

    /* XMEGA AU [MANUAL] p. 116 CTRL -Control register
     * page 13 3.12.1 Sequence for write operation to protected I/O registers
     * page 15 3.14.1 CCP – Configuration Change Protection register
     */

    /* Disable CCP for Protected IO registerand set new value*/
    _PROTECTED_WRITE(RST_CTRL, RST_SWRST_bm);
    while (1) {}
}

void pm_set_lowest(void) {

	PORTF.OUTCLR = PIN3_bm;
	_wait_ms(3000);
	PORTF.OUTSET = PIN3_bm;
	_wait_ms(50);

//    printf("\npm_set_lowest\n");
//	_delay_ms(100);
//    thread_stack_print();


//	PORTF.OUTCLR = PIN3_bm ;
//	PORTF.OUTCLR = PIN2_bm ;
//	PORTE.OUTCLR = PIN7_bm ;
//	PORTE.OUTCLR = PIN6_bm ;

	// _delay_ms(WAIT_100);
	// int16_t i = UINT16_MAX;
	// while(i){i--;}
	//_wait_ms(3000);

//	PORTF.OUTSET = PIN3_bm ;
//	PORTF.OUTSET = PIN2_bm ;
//	PORTE.OUTSET = PIN6_bm ;
//	PORTE.OUTSET = PIN7_bm ;
 

	//_delay_ms(20);
	// i = UINT8_MAX;
	// while(i){i--;}
	//_wait_ms(1000);


	/*TODO implement power save Modes*/
}
