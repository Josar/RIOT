/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#include "tps6274x.h"
#include "tps6274x_settings.h"

#define ENABLE_DEBUG		(0)
#include "debug.h"

uint8_t stepdown_init(uint16_t voltage)
{
	if(voltage>=1800 && voltage <=3300){
		uint8_t vsel = (voltage-1800)/100;
		if(converter_config.vsel1_pin != GPIO_UNDEF){
				gpio_init(converter_config.vsel1_pin, GPIO_OUT);
				gpio_write(converter_config.vsel1_pin, vsel&0b1);
				DEBUG("vsel1: %u \n", vsel&0b1);
		}
		if(converter_config.vsel2_pin != GPIO_UNDEF){
				gpio_init(converter_config.vsel2_pin, GPIO_OUT);
				gpio_write(converter_config.vsel2_pin, vsel&0b10);
				DEBUG("vsel2: %u \n", vsel&0b10);
		}
		if(converter_config.vsel3_pin != GPIO_UNDEF){
				gpio_init(converter_config.vsel3_pin, GPIO_OUT);
				gpio_write(converter_config.vsel3_pin, vsel&0b100);
				DEBUG("vsel3: %u \n", vsel&0b100);
		}
		if(converter_config.vsel4_pin != GPIO_UNDEF){
				gpio_init(converter_config.vsel4_pin, GPIO_OUT);
				gpio_write(converter_config.vsel4_pin, vsel&0b1000);
				DEBUG("vsel4: %u \n", vsel&0b1000);
		}
		gpio_init(converter_config.ctrl_pin, GPIO_OUT);
		return 0;
	}else{
		return 1;
	}
}

uint8_t stepdown_switch_voltage(uint16_t voltage)
{
	if(voltage>=1800 && voltage <=3300){
			uint8_t vsel = (voltage-1800)/100;
			if(converter_config.vsel1_pin != GPIO_UNDEF){
					gpio_write(converter_config.vsel1_pin, vsel&0b1);
					DEBUG("vsel1: %u \n", vsel&0b1);
			}
			if(converter_config.vsel2_pin != GPIO_UNDEF){
					gpio_write(converter_config.vsel2_pin, vsel&0b10);
					DEBUG("vsel2: %u \n", vsel&0b10);
			}
			if(converter_config.vsel3_pin != GPIO_UNDEF){
					gpio_write(converter_config.vsel3_pin, vsel&0b100);
					DEBUG("vsel3: %u \n", vsel&0b100);
			}
			if(converter_config.vsel4_pin != GPIO_UNDEF){
					gpio_write(converter_config.vsel4_pin, vsel&0b1000);
					DEBUG("vsel4: %u \n", vsel&0b1000);
			}
			return 0;
		}else{
			return 1;
		}
}

void stepdown_enable_load(void)
{
	gpio_set(converter_config.ctrl_pin);
}

void stepdown_disable_load(void)
{
	gpio_clear(converter_config.ctrl_pin);
}
