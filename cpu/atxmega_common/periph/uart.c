/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 *
 * Support static BAUD rate calculation using UART_STDIO_BAUDRATE.
 * Set UART_STDIO_BAUDRATE to the desired baud rate and pass it as a -D argument
 * at compliation time (e.g. in the boards Makefile.include file).
 * UART_BAUD_TOL can be set to guarantee a BAUD rate tolerance at compile time or
 * to switch to double speed transmission (U2X) to achieve a lower tolerance.
 * At runtime, this tolerance is not guaranteed to be met.
 * However, an error message will be displayed at compile time.
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"

#include "include/board.h"
#include "periph/uart.h"


#include <avr/io.h>

#include <stdlib.h>
#include <stdio.h>
/**
 * @brief   Maximum percentage error in calculated baud before switching to
 *          double speed transmission (U2X)
 *
 * Takes whole numbers from 0 to 100, inclusive, with a default of 2.
 */
#if defined(UART_BAUD_TOL)
/* BAUD_TOL is defined here as it is used by the setbaud.h utility */
#define BAUD_TOL UART_BAUD_TOL
#else
#define BAUD_TOL 2
#endif

#if defined(UART_STDIO_BAUDRATE)
/* BAUD and F_CPU are required by setbaud.h to calculated BRR */
#define BAUD UART_STDIO_BAUDRATE
#define F_CPU CLOCK_CORECLOCK
#include <util/setbaud.h>
#endif

/**
 * @brief   Configured device map
 * @{
 */
#if UART_NUMOF
static USART_t* dev[] = {
#ifdef UART_0
    UART_0,
#endif
#ifdef UART_1
    UART_1,
#endif
#ifdef UART_2
    UART_2,
#endif
#ifdef UART_3
    UART_3
#endif
};
#else
/* fallback if no UART is defined */
static const mega_uart_t *dev[] = { NULL };
#endif

/**
 * @brief   Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

static void _update_brr(uart_t uart, uint16_t BSEL, bool double_speed)
{
	/* set four most-significant bits of BSEL; BAUDCTRLB = BSCALE[3:0] BSEL[11:8] */
	dev[uart]->BAUDCTRLB = (uint8_t)( 0x0f & ( BSEL>>8 ) );
	/* set lower 8 bits of BAUDCTRLA = BSEL[7:0] */
	dev[uart]->BAUDCTRLA = (uint8_t)(0xff & BSEL);

    if (double_speed) {
        dev[uart]->CTRLB |= USART_CLK2X_bm ;
    }
}

static void _set_baut(uart_t uart, uint32_t baudrate)
{
	//uint8_t BSCALE;
	uint16_t BSEL;

#if defined(UART_DOUBLE_SPEED)
    BSEL = (CLOCK_CORECLOCK ) / (8UL * baudrate) - 1UL;
    _update_brr(uart, BSEL, true);
#else
    BSEL = (CLOCK_CORECLOCK) / (16UL * baudrate) - 1UL;
    _update_brr(uart, BSEL, false);
#endif
}



int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* make sure the given device is valid */
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    /* register interrupt context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg   = arg;

    /* disable and reset UART */
    dev[uart]->CTRLA = 0;
    dev[uart]->CTRLB = 0;
    dev[uart]->CTRLC = 0;

    /* configure UART to 8N1 mode */
    dev[uart]->CTRLC = USART_CMODE_ASYNCHRONOUS_gc|USART_PMODE_DISABLED_gc| USART_CHSIZE_8BIT_gc ;
    /* set clock divider */

    _set_baut(uart, baudrate);

    // set uasart to 9600BAUD
//     dev[uart]->BAUDCTRLA = (3317 & 0xff) << USART_BSEL_gp;
//     dev[uart]->BAUDCTRLB = ((-4) << USART_BSCALE_gp) | ((3317 >> 8) << USART_BSEL_gp);

     // set uasart to 115200BAUD
     dev[uart]->BAUDCTRLA = ( 2158 & 0xff)<< USART_BSEL_gp;
     dev[uart]->BAUDCTRLB = ((-7) << USART_BSCALE_gp) | ( ( 2158 >> 8) << USART_BSEL_gp );

#if defined(UART_DOUBLE_SPEED)
         dev[uart]->CTRLB |= USART_CLK2X_bm ;
#endif



     /* enable RX and TX Interrupts and set level*/
    if (rx_cb) {

    	dev[uart]->CTRLA = USART_RXCINTLVL_HI_gc ;
        dev[uart]->CTRLB = 0x10 | 0x08 ;

        /* Enable PMIC interrupt level low. */
        PMIC.CTRL |= PMIC_HILVLEN_bm;
    }
    else {
    	/* only transmit */
    	 dev[uart]->CTRLB =  USART_TXEN_bm ;
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
	// PORTE.OUTSET = PIN6_bm ;
	for (size_t i = 0; i < len; i++) {
		while ( !( dev[uart]->STATUS & USART_DREIF_bm)  ) {};
		dev[uart]->DATA = data[i];
    }
	//PORTE.OUTCLR = PIN6_bm ;
}

static inline void isr_handler(int num)
{
    if( (dev[num]->STATUS & USART_RXCIF_bm) == 0) {
    	return;
    }

	isr_ctx[num].rx_cb(isr_ctx[num].arg, dev[num]->DATA);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

// #ifdef USARTC1_RXC_vect

ISR(USARTC1_RXC_vect, ISR_BLOCK)
{
    __enter_isr();
    // PORTE.OUTTGL = PIN6_bm ;
    isr_handler(0);
    __exit_isr();
}
// #endif /* UART_0_RXC_ISR */

#ifdef UART_1_ISR
ISR(UART_1_ISR, ISR_BLOCK)
{
    __enter_isr();
    isr_handler(1);
    __exit_isr();
}
#endif /* UART_1_ISR */

#ifdef UART_2_ISR
ISR(UART_2_ISR, ISR_BLOCK)
{
    __enter_isr();
    isr_handler(2);
    __exit_isr();
}
#endif /* UART_2_ISR */

#ifdef UART_3_ISR
ISR(UART_3_ISR, ISR_BLOCK)
{
    __enter_isr();
    isr_handler(3);
    __exit_isr();
}
#endif /* UART_3_ISR */
