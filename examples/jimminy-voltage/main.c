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

#define F_CPU		CLOCK_CORECLOCK
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

int main(void)
{
	LED_PORT &= ~(RED|GREEN|BLUE);
    puts("Jimminy voltage test");
    printf("This will test the step-down converter by switching from 1.8V - 2.2V - 2.6V - 3.0V \n");
    printf("Breaks inbetween will be about 7 seconds \n");
while(1){
    stepdown_init(1800);
    printf("1.8V \n");
    printf("DDRG: %u PORTG %u \n", DDRG, PORTG);
    _delay_ms(7000);
    stepdown_switch_voltage(2200);
    printf("2.2V \n");
    _delay_ms(7000);
    stepdown_switch_voltage(2600);
    printf("2.6V \n");
    _delay_ms(7000);
    stepdown_switch_voltage(3000);
    printf("3.0V \n");
    _delay_ms(7000);
}



    return 0;
}
