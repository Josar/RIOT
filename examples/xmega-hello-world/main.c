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

#include "mutex.h"

#include "periph/spi.h"

/* set interval to 1 second 1 000 000*/
#define INTERVAL (1U * US_PER_SEC)

// #define INTERVAL (100000) // us

/* This Test the basic functionality of the avr--xmega-ias Board which uses an atxmega256a3u
 * This example shows the running board configured with 32Mhz internal clock configured with automatic calibration with DFLL.
 * Also supported are
 * 		LED F2/F3 and LED E6/E7
 * 		stdio using USARTC1 with 115200 Baud
 * 		xtimer using TIMER_0
 *
 * This should compile an run on the Board.
 *
 * The printf output can be seen on a terminal.
 * The periodic wakeup will display a text.
 * in power management thread pm_set_lowest the LEDs are blinking.
 *
 */
int main(void)
{
   uint8_t reg=0;

    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

	PORTF.OUTCLR = PIN3_bm ;
	PORTF.OUTCLR = PIN2_bm ;
	PORTE.OUTCLR = PIN7_bm ;
	PORTE.OUTCLR = PIN6_bm ;


	printf( "Now %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()) );

	xtimer_ticks32_t last_wakeup = xtimer_now();




	spi_init( SPID );

	spi_init_cs( SPID, 0);
	spi_acquire( SPID, 0, SPI_MODE_0_gc,  SPI_PRESCALER_DIV64_gc );

    reg = spi_transfer_reg(SPID, AT86RF2XX_PARAM_CS, 0x80 |0x1C ,0);
    printf("Part_Num AT86RF231 0x03 = 0x%02x \n\r", reg);

    reg = spi_transfer_reg(SPID, AT86RF2XX_PARAM_CS, 0x80 |0x1D ,0);
    printf("Version_Num 0x02 = 0x%02x \n\r", reg);

    reg = spi_transfer_reg(SPID, AT86RF2XX_PARAM_CS, 0x80 |0x1E ,0);
    printf("Man_ID_0 0x1F = 0x%02x \n\r", reg);


	while(1) {
		xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
		printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));
		_delay_ms(500);
	}


return 0;
}


