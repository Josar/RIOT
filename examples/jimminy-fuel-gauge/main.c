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
#include "lc709203f.h"
#include "tps6274x.h"

#define F_CPU		CLOCK_CORECLOCK
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

static void _gauge_cb(void* arg)
{
	printf("\n ALARM \n");
	//LED_PORT |= RED;
}

int main(void)
{
	LED_PORT &= ~(RED|GREEN|BLUE);
    puts("Jimminy Fuel Gauge test");
    stepdown_init(3000);
    gauge_init(I2C_0, GPIO_PIN(PORT_E, 6), _gauge_cb, NULL);
    gauge_set_alarm_low_rsoc(I2C_0, 0);
    gauge_set_alarm_low_rsoc(I2C_0, 70);
    printf("Alarm Low RSOC: %u \n", gauge_get_alarm_low_rsoc(I2C_0));
while(1){
   printf("Voltage in mV: %u \n", gauge_get_voltage(I2C_0));
   printf("RSOC: %u \n", gauge_get_rsoc(I2C_0));
   printf("Ite in 0.1: %u \n", gauge_get_ite(I2C_0));
   printf("ID: %u \n", gauge_get_id(I2C_0));
   printf("Cell Temp in 0.1C: %u \n", gauge_get_cell_temp(I2C_0));
   _delay_ms(5000);
}



    return 0;
}
