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
 * @brief       sht21 sensor  test
 *
 *@author	Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include "sht21.h"
#include "tps6274x.h"

#define F_CPU		CLOCK_CORECLOCK
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

int main(void)
{
	LED_PORT &= ~(RED|GREEN|BLUE);
    puts("Jimminy SHT21 test");
    printf("This will test the Temperature and Humidity sensor\n");
    stepdown_init(3000);
    sht21_init(I2C_0);
    _delay_ms(10);
    sht21_write_settings(I2C_0, RES_RH12_T14);
    printf("Temp: %0.2f \n", sht21_measure(I2C_0, CRC_ENABLED, TEMP));
   printf("Humidity: %0.2f \n", sht21_measure(I2C_0, CRC_ENABLED, HUMIDITY));
   _delay_ms(1000);
   printf("Read_setting: %x \n", sht21_read_settings(I2C_0));
  // sht21_write_settings(I2C_0, RES_RH11_T11| EN_CHIP_HEATER);
   //printf("Read_setting: %x \n", sht21_read_settings(I2C_0));
while(1){
	printf("Temp: %0.2f \n", sht21_measure(I2C_0, CRC_ENABLED, TEMP));
	printf("Humidity: %0.2f \n", sht21_measure(I2C_0, CRC_ENABLED, HUMIDITY));
	_delay_ms(1000);
}



    return 0;
}
