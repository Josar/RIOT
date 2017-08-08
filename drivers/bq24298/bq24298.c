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

#define ENABLE_DEBUG		(1)
#include "debug.h"

int8_t charger_init(i2c_t dev)
{
	return i2c_init_master(dev, I2C_SPEED_FAST);
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

uint8_t charger_reg_reset(i2c_t dev, uint8_t reg)
{
	return i2c_write_reg(dev, 0x6b, reg, reset_value[reg]);
}
