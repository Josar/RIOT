/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_jiminy-xmega256
 * @{
 *
 * @file
 * @brief       Board specific LED initialization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void _wait_ms(uint16_t count){
	while(count--){
		// loops for 1ms,  1e-3*4/F_CPU=value
		_delay_loop_2(8000);
	}
}

/*    PORTF.OUTSET = PIN2_bm;
    _wait_ms(10);
    PORTF.OUTCLR = PIN2_bm;
*/
void led_init(void)
{
    /* initialize the board LED */
    /* Ports Pins as Output */
	PORTF.DIRSET =  PIN3_bm|PIN2_bm ; // Set pins 2, 3 on port F to be output.
	PORTE.DIRSET =  PIN7_bm|PIN6_bm ;
	
    /* All Pins Low so LEDs are off */
    PORTF.OUTCLR = PIN3_bm|PIN2_bm ; // Set pins 2, 3 on port F to low.
	PORTE.OUTCLR = PIN7_bm|PIN6_bm ;

    /* initialize Pins used for stdio */
	PORTC.DIRSET =  PIN7_bm ; // output for UART_TX
	PORTC.DIRCLR =  PIN6_bm ; // input for UART_RX
}
