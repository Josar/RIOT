/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief        Device driver interface for the BQ24298 Single Cell USB Charger
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#include "bq24298.h"
#include "board.h"
#include <stdlib.h>

#define ENABLE_DEBUG		(1)
#include "debug.h"

#define JIMMINY_BOARD

charger_status_t my_charger_status;

typedef struct{
	i2c_t dev;
	charger_cb_t cb;
	void* arg;
}callback_t;
static callback_t mycallback;

typedef enum{
		PCINT_FALLING = 0,
		PCINT_RISING		= 1
}pcint_stat;
static volatile pcint_stat my_stat= PCINT_FALLING;

static inline void _charger_cb(void)
{
	uint8_t fault = charger_get_new_fault(mycallback.dev);
	uint8_t status = charger_get_system_status(mycallback.dev);
	my_charger_status.watchdog_expired = (fault&(1<<7))>>7;
	my_charger_status.otg_boost_error = (fault&(1<<6))>>6;
	my_charger_status.charge_fault = (charge_fault_t)((fault&(3<<4))>>4);
	my_charger_status.bat_ovp = (fault&(1<<3))>>3;
	my_charger_status.ntc_fault = (ntc_fault_t)(fault&3);
	my_charger_status.status_vbus = (vbus_stat_t) ((status&(3<<6))>>6);
	my_charger_status.status_charge = (charge_stat_t) ((status&(3<<4))>>4);
	my_charger_status.power_good = (status&(1<<2))>>2;
	DEBUG("Charger_fault: 0x%x  status: 0x%x \n", fault, status);
	if(mycallback.cb != NULL){
			mycallback.cb(fault, status, mycallback.arg);
	}
}


int8_t charger_init(i2c_t dev, gpio_t alarm_pin, charger_cb_t cb, void *arg)
{
	#ifdef JIMMINY_BOARD
		DEBUG("Pinoccio detected \n");
		gpio_init(alarm_pin, GPIO_IN);
		PCICR |= (1<<PCIE0);
		PCMSK0 |= (1<<PCINT4);
		DEBUG("PCICR %x PCMSK %x \n", PCICR, PCMSK0);
	#else
		gpio_init_int(alarm_pin, GPIO_IN, GPIO_FALLING, (gpio_cb_t)cb, arg);
	#endif
	mycallback.cb = cb;
	mycallback.arg = arg;
	mycallback.dev = dev;
	int8_t error = i2c_init_master(dev, I2C_SPEED_FAST);
	uint8_t fault = charger_get_new_fault(mycallback.dev);
	uint8_t status = charger_get_system_status(mycallback.dev);
	my_charger_status.watchdog_expired = (fault&(1<<7))>>7;
	my_charger_status.otg_boost_error = (fault&(1<<6))>>6;
	my_charger_status.charge_fault = (charge_fault_t)((fault&(3<<4))>>4);
	my_charger_status.bat_ovp = (fault&(1<<3))>>3;
	my_charger_status.ntc_fault = (ntc_fault_t)(fault&3);
	my_charger_status.status_vbus = (vbus_stat_t) ((status&(3<<6))>>6);
	my_charger_status.status_charge = (charge_stat_t) ((status&(3<<4))>>4);
	my_charger_status.power_good = (status&(1<<2))>>2;
	return error;
}


charger_status_t charger_get_status_struct(void){
	return my_charger_status;
}

uint8_t charger_get_system_status(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x8, &rec_buf);

	if(control != 1){
		DEBUG("get_system_status(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_new_fault(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x9, &rec_buf);

	if(control != 1){
		DEBUG("get_new_fault(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_input_source_control(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x0, &rec_buf);

	if(control != 1){
		DEBUG("get_input_source_control(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_power_on_config(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x1, &rec_buf);

	if(control != 1){
		DEBUG("get_power_on_config(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_current_control(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x2, &rec_buf);

	if(control != 1){
		DEBUG("get_current_control(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_pre_charge_current_control(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x3, &rec_buf);

	if(control != 1){
		DEBUG("get_pre_charge_control(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_voltage_control(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x4, &rec_buf);

	if(control != 1){
		DEBUG("get_voltage_control(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_charge_termination_and_timer(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x5, &rec_buf);

	if(control != 1){
		DEBUG("get_charge_termination_and_timer(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_boost_voltage_and_thermal(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x6, &rec_buf);

	if(control != 1){
		DEBUG("get_boost_voltage_and thermal(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

uint8_t charger_get_misc_operation(i2c_t dev)
{
	uint8_t rec_buf;
	int8_t control = i2c_read_reg(dev, 0x6b, 0x7, &rec_buf);

	if(control != 1){
		DEBUG("get_misc_operation(): Error  reading or writing\n");
		return 0;
	}
	return rec_buf;
}

int8_t charger_reg_reset(i2c_t dev, uint8_t reg)
{
	return i2c_write_reg(dev, 0x6b, reg, reset_value[reg]);
}

int8_t charger_write_reg(i2c_t dev, uint8_t reg, uint8_t settings)
{
	return i2c_write_reg(dev, 0x6b, reg, settings);
}

ISR(PCINT0_vect){
	__enter_isr();
	if(my_stat == PCINT_FALLING){
		my_stat = PCINT_RISING;
		_charger_cb();
	}else{
		my_stat = PCINT_FALLING;
	}
	 __exit_isr();
}

