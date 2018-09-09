/*
 * Copyright (C) 2017 RWTH Aachen, Josua Arndt
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
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "cpu.h"
#include "board.h"


#include <util/delay.h>

#define ENABLE_DEBUG (0)
#include "debug.h"


void clk_init(void)
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

        /* XMEGA A3U [DATASHEET] p.23 After reset, the device starts up running from the 2MHz internal oscillator.
         * The other clock sources, DFLLs and PLL, are turned off by default.*/

        // Configure clock to 32MHz with calibration
        // application note AVR1003

        // From errata http://www.avrfreaks.net/forum/xmega-dfll-does-it-work
        // In order to use the automatic runtime calibration for the 2 MHz or the 32 MHz internal oscillators, the DFLL for both oscillators and both oscillators has to be enabled for one to work.
        // TODO Test if this is also relevant for the atxmega256a3u
        // Quick fix just let the 2MHz clock running
        /* Configure the PLL , Use Internal 2MHz oscillator, do not devide output, no multiplication factor */
        OSC.PLLCTRL = 0;
        OSC.CTRL |= OSC_PLLEN_bm | OSC_RC32MEN_bm | OSC_RC32KEN_bm;  /* Enable the internal PLL & 32MHz & 32KHz oscillators */
        while(!(OSC.STATUS & OSC_RC32KRDY_bm));       /* Wait for 32Khz oscillator to stabilize */
        while(!(OSC.STATUS & OSC_RC32MRDY_bm));       /* Wait for 32MHz oscillator to stabilize */
        DFLLRC32M.CTRL = DFLL_ENABLE_bm ;             /* Enable DFLL - defaults to calibrate against internal 32Khz clock */
        DFLLRC2M.CTRL = DFLL_ENABLE_bm ;              /* Enable DFLL - defaults to calibrate against internal 32Khz clock */

        /* Disable CCP for Protected IO register and set new value*/
        /* Set system clock prescalers to zero */
        /* PSCTRL contains A Prescaler Value and one value for and B and C Prescaler */
        _PROTECTED_WRITE(CLK.PSCTRL, CLK_PSADIV_1_gc|CLK_PSBCDIV_1_1_gc);
        /*
        * Previous instruction takes 3 clk cycles with -Os option
        * we need another clk cycle before we can reuse it.
        */
        __asm__ __volatile__("nop");

        /* Disable CCP for Protected IO register and set new value*/
        /* Switch to 32MHz clock */
        _PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_RC32M_gc);

        /* Disable 2Mhz oscillator */ // QUICK FIX DO NOT DISABLE
        // OSC.CTRL &= ~OSC_RC2MEN_bm;                   
        
        /* wait for calibratin to finisch */
        //_delay_ms( time_to_wait );
        _wait_ms(1);

        /*Set Perifery clock*/
}

void cpu_init(void)
{
    wdt_reset();
    wdt_disable();

    /* Setup clock and auto calibration */
    clk_init();
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

ISR(BADISR_vect){

    printf("BAD ISR\n");

    while (1) {

        if ((PORTF.IN & PIN3_bm) != 0) {
            PORTF.OUTSET = PIN2_bm;
            PORTF.OUTCLR = PIN3_bm;
        }
        else {
            PORTF.OUTCLR = PIN2_bm;
            PORTF.OUTSET = PIN3_bm;
        }

        //_delay_ms( time_to_wait );
        _wait_ms(500);
    }
}
