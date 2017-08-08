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
#include "bq24298.h"

#define F_CPU		CLOCK_CORECLOCK
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

int main(void)
{
	/*while(1) {
	LED_PORT &= ~(RED|GREEN|BLUE);
	LED_PORT |= RED;
	_delay_ms(500);
	LED_PORT&= ~RED;
	LED_PORT |= GREEN;
	_delay_ms(500);
	LED_PORT &= ~GREEN;
	LED_PORT |= BLUE;
	_delay_ms(500);
	}*/
    puts("Jimminy Power Path test");
    charger_init(I2C_0);
    printf("Status register: 0x%x \n", charger_get_system_status(I2C_0));
    printf("new fault register: 0x%x \n", charger_get_new_fault(I2C_0));
    printf("new fault register: 0x%x \n", charger_get_new_fault(I2C_0));
    printf("new fault register: 0x%x \n", charger_get_new_fault(I2C_0));
    printf("new fault register: 0x%x \n", charger_get_new_fault(I2C_0));

    	while(1);



    return 0;
}
