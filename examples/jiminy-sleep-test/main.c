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
 * @brief       vsetpdown regulator test
 *
 *@author	Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include "tps6274x.h"
#include "at86rf2xx.h"
#include "xtimer.h"
#include "periph/rtc.h"
#include <avr/sleep.h>


at86rf2xx_t dev;
uint8_t data[] = "This is a test data package for the riot driver. It will be used to check jimminy's power consumption and needs to be very long so this text will be sent as random data";

void testcase(void){
    //receive
    at86rf2xx_set_state(&dev, AT86RF2XX_STATE_RX_ON);
    xtimer_usleep(200000);
    //Transmitt
    at86rf2xx_send(&dev, data, sizeof(data));
    xtimer_usleep(200000);
    at86rf2xx_set_state(&dev, AT86RF2XX_STATE_SLEEP);
    PRR1 |= PRTRX24;
    //idle
    xtimer_sleep(2);
    //power save
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    rtc_init();
    xtimer_usleep(200);
    uint8_t i=0;
    while(i<10){
		TIFR2 &= ~(1<<TOV2);
        TCNT2 = 0;
	    while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB)));
        sleep_mode();
        i++;
    }
    xtimer_usleep(200);
}

int main(void)
{
    
    at86rf2xx_params_t params;
    at86rf2xx_setup(&dev, &params); 
    stepdown_init(1800);
    testcase();
    stepdown_switch_voltage(3300);
    PRR1 &= ~PRTRX24;
    at86rf2xx_reset(&dev);
    testcase();
    stepdown_switch_voltage(1800);
    //deep sleep
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
    return 0;   
}
