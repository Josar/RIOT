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
 * @brief       power path test
 *
 *@author	Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include "bq24298.h"
#include "lc709203f.h"

#define F_CPU		CLOCK_CORECLOCK
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>


static void _charger_cb(uint8_t fault, uint8_t status, void* arg){
	printf("ALARM CHARGER \n");
	//LED_PORT |= RED;
}

static void _gauge_cb(void* arg)
{
	printf("\n ALARM GAUGE \n");
	//LED_PORT |= RED;
}


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
	LED_PORT |= (RED|GREEN|BLUE);
    puts("Jimminy Power Path test");
    gauge_init(I2C_0, GPIO_PIN(PORT_E, 6), _gauge_cb, NULL);
    charger_init(I2C_0, GPIO_PIN(PORT_B,4), _charger_cb, NULL);
    //charger_write_reg(I2C_0, REG_POWER_ON_CONFIG, reset_value[REG_POWER_ON_CONFIG]|REG1_I2C_WATCHDOG_TIMER_RESET);
    printf("Status register: 0x%x \n", charger_get_system_status(I2C_0));
    printf("new fault register: 0x%x \n", charger_get_new_fault(I2C_0));

    while(1){
    	//printf("new fault register: 0x%x \n", charger_get_new_fault(I2C_0));
    //	printf("RSOC %u \n", gauge_get_rsoc(I2C_0));
    	//_delay_ms(1000);
    }



    return 0;
}
