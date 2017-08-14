/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pinoccio-mega256rfr2
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Pinoccio Mega 256rfr2 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <avr/cpufunc.h>

#include "board.h"

#include <stdio.h>
#include <avr/io.h>

#include "cpu.h"
#include "uart_stdio.h"

#define F_CPU   32000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

void system_stdio_init(void);
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

#define time_to_wait 50


void board_init(void)
{
	uint8_t *reg = (uint8_t *)&PR.PRGEN;
		uint8_t i;

	/* Turn off all peripheral clocks that can be turned off. */
	for (i = 0; i <= 7; i++) {
		*(reg++) = 0xff;
	}

	reg = (uint8_t *)&PR.PRGEN;
	/* Turn on all peripheral clocks that can be turned on. */
	for (i = 0; i <= 7; i++) {
		*(reg++) = 0x00;
	}

	/* Set system clock prescalers to zero */
	CLK.CTRL = CLK_PSADIV_1_gc;


	/*
	* From avr1003 (doc8072.pdf)
	* There are five internal clock sources (including the internal PLL),
	* ranging from an ultra low-power 32 kHz RC oscillator to a 32 MHz
	* factory-calibrated ring oscillator with auto-calibration features.
	* All but one can be used for the main system clock.
	*/
	OSC.CTRL |=OSC_RC32MEN_bm;
	/*
	* Wait for OSC to stabilize
	*/
	while(   !( (OSC.STATUS) & OSC_RC32MRDY_bm)   );
//	/*
//	* Set Regirester Protection signature
//	*/
//	CCP=CCP_IOREG_gc; //0xD8;
//	/*
//	* Possible prescaler config:
//	* 0x00 = NO prescaler
//	* CLK_PSADIVx_bm | CLK_PSBCDIVy_bm;
//	* x = A prescaler config: 0, 1, 2, 3, 4;
//	* y = B & C prescaler config: 0, 1;
//	*
//	* Now Disabling prescaler
//	*/
//	CLK.PSCTRL = 0x00;

	/* Disable CCP for Protected IO registerand set new value*/
     PROTECTED_WRITE(CLK.PSCTRL, 0x00);
	/*
	* Previous instruction takes 3 clk cycles with -Os option
	* we need another clk cycle before we can reuse it.
	*/
	__asm__ __volatile__("nop");

//	/*
//	* Set Regirester Protection signature
//	*/
//	CCP=CCP_IOREG_gc; //0xD8;
//	/*
//	* Selecting Clock source
//	* Equivalent to CCPWrite(&CLK.CTRL, CLK_SCLKSEL0_bm);
//	*/
//	CLK.CTRL = CLK_SCLKSEL0_bm;

	/* Disable CCP for Protected IO registerand set new value*/
     PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL0_bm);

	/* config LEDs*/
	PORTF.DIRSET =  PIN3_bm|PIN2_bm ; // Set pins 2, 3 on port F to be output.
	PORTE.DIRSET =  PIN7_bm|PIN6_bm ;

	PORTF.OUTCLR = PIN3_bm|PIN2_bm ; // Set pins 2, 3 on port F to low.
	PORTE.OUTCLR = PIN7_bm|PIN6_bm ;

	/* config stdio UART */
	PORTC.DIRSET =  PIN7_bm; // output for TX
	PORTC.DIRCLR =  PIN6_bm;  // input for RX


//	uint8_t wait=0;

//	 while( wait < 5){ // loop
//		 wait++;
//
//
//		 if( (PORTF.IN & PIN3_bm)!=0 ){
//			  PORTF.OUTSET = PIN2_bm ;
//
//			  PORTF.OUTCLR = PIN3_bm ;
//		 }else
//		 {
//			 PORTF.OUTCLR = PIN2_bm ;
//			 PORTF.OUTSET = PIN3_bm ;
//		 }
//
//		 _delay_ms( time_to_wait );
//	 }

//	 PORTF.OUTCLR = PIN3_bm ;
//	 PORTF.OUTCLR = PIN2_bm ;


	/* initialize stdio via USART_0 */
	system_stdio_init();


    /* Disable power saving for TIMER4 which is used as xtimer, PRTIM4*/
//	PRR1 &= ~(1<<PRTIM4);

    /* initialize the CPU */
    cpu_init();



    //TODO led init

    /* initialize the board LED (connected to pin PB7) */
    /* Ports Pins as Output */
//    LED_PORT_DDR |= BLUE|RED|GREEN;
    /* All Pins High so LEDs are off */
//    LED_PORT |= BLUE|RED|GREEN;

     irq_enable();

    puts("Board init");

}

/**
 * @brief Initialize the System, initialize IO via UART_0
 */
void system_stdio_init(void)
{

	/* initialize Pins used for stdout */
	PORTC.DIRSET =  PIN7_bm ; // output for TX
	PORTC.DIRCLR =  PIN6_bm ; // input for RX

	/* initialize UART_0 for use as stdout */
    uart_stdio_init();

    stdout = &uart_stdout;
    stdin = &uart_stdin;

    /* Flush stdout */
    /* Make a very visible start printout */
    puts("\n_________________________________\n");
}

static int uart_putchar(char c, FILE *stream)
{
    (void) stream;
    uart_stdio_write(&c, 1);
    return 0;
}

int uart_getchar(FILE *stream)
{
    (void) stream;
    char c;
    uart_stdio_read(&c, 1);
    return (int)c;
}
