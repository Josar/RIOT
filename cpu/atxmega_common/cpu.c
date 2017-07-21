/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
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
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include "cpu.h"
#include <avr/io.h>

#include "avr/wdt.h"
#include "avr/pgmspace.h"
#include "board.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/* Remove when unused*/
#define F_CPU   32000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{


	wdt_reset();
	wdt_disable();

	/* Right now we need to do nothing here */
    //;
}

/* This is a vector which is aliased to __vector_default,
 * the vector executed when an ISR fires with no accompanying
 * ISR handler. This may be used along with the ISR() macro to
 * create a catch-all for undefined but used ISRs for debugging
 * purposes.
 */
// SCIRQS – Symbol Counter Interrupt Status Register
// BATMON – Battery Monitor Control and Status Register
// IRQ_STATUS /1 – Transceiver Interrupt Status Register
// EIFR – External Interrupt Flag Register
// PCIFR – Pin Change Interrupt Flag Register

ISR(BADISR_vect)
{
	while(1){

		if( (PORTF.IN & PIN3_bm)!=0 ){
			  PORTF.OUTSET = PIN2_bm ;

			  PORTF.OUTCLR = PIN3_bm ;
		 }else
		 {
			 PORTF.OUTCLR = PIN2_bm ;
			 PORTF.OUTSET = PIN3_bm ;
		 }

		 _delay_ms( 50 );
	};
}

/* All Interrupts are liste here to test them.*/
/*TODO Remove not needed interupt vector routines */

/*INT0 - 7 defined in gpio.c */
//	ISR(INT0_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT0_vect \n");
//		__exit_isr();
//	}
//	ISR(INT1_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT1_vect \n");
//		__exit_isr();
//	}
//	ISR(INT2_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT2_vect \n");
//		__exit_isr();
//	}
//	ISR(INT3_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT3_vect \n");
//		__exit_isr();
//	}
//
//	ISR(INT4_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT4_vect \n");
//		__exit_isr();
//	}
//	ISR(INT5_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT5_vect \n");
//		__exit_isr();
//	}
//	ISR(INT6_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT6_vect \n");
//		__exit_isr();
//	}
//	ISR(INT7_vect , ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("INT7_vect \n");
//		__exit_isr();
//	}


// context save interrupt
//	ISR(PCINT0_vect, ISR_BLOCK)
//	{
//		__enter_isr();
//		DEBUG("PCINT0_vect \n");
//		LED_PORT |= RED;
//		__exit_isr();
//	}

