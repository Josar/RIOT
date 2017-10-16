/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "lc709203f.h"

#include "avr/interrupt.h"

#define ENABLE_DEBUG		(1)
#include "debug.h"


gauge_setting_t conf;

/*
 * does a crc check and returns 0 for passed and -1 for failed
 */
uint8_t _get_crc(uint8_t *rec_values, uint8_t len)
{
	uint8_t crc = 0x00;
	uint8_t current_byte;
	uint8_t bit;

	for (current_byte = 0; current_byte < len; current_byte++)
	{
		crc ^= (rec_values[current_byte]);
		for (bit = 8; bit > 0; bit--)
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ 0x07;
			else
				crc = (crc << 1);
		}
	}
	return crc;
}


int8_t gauge_init(i2c_t dev, gpio_t alarm_pin, gauge_cb_t cb, void *arg)
{
	conf.alarm_pin = alarm_pin;
	conf.cb = cb;
	conf.arg = arg;
	gpio_init_int(alarm_pin, GPIO_IN, GPIO_FALLING,cb, arg);
	return i2c_init_master(dev, I2C_SPEED_FAST);
}

uint16_t gauge_get_voltage(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x9, rec_buf, 3);

	if(control != 3){
		DEBUG("get_voltage(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x9, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint8_t gauge_get_rsoc(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0xd, rec_buf, 3);

	if(control != 3){
		DEBUG("get_rsoc(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0xd, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_ite(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0xf, rec_buf, 3);

	if(control != 3){
		DEBUG("get_rsoc(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0xf, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_id(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x11, rec_buf, 3);

	if(control != 3){
		DEBUG("get_id(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x11, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_cell_temp(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x8, rec_buf, 3);

	if(control != 3){
		DEBUG("get_cell_temp(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x8, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return ((((uint16_t)rec_buf[1]<<8)|rec_buf[0])-2731.5);
}

uint8_t gauge_get_status_bit(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x16, rec_buf, 3);

	if(control != 3){
		DEBUG("get_status_bit(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x16, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint8_t gauge_get_power_mode(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x15, rec_buf, 3);

	if(control != 3){
		DEBUG("get_power_mode(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x15, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_alarm_low_voltage(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x14, rec_buf, 3);

	if(control != 3){
		DEBUG("get_alarm_low_voltage(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x14, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint8_t gauge_get_alarm_low_rsoc(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x13, rec_buf, 3);

	if(control != 3){
		DEBUG("get_alarm_low_rsoc(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x13, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint8_t gauge_get_change_of_parameter(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x12, rec_buf, 3);

	if(control != 3){
		DEBUG("get_change_of_parameter(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x12, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_apt(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0xc, rec_buf, 3);

	if(control != 3){
		DEBUG("get_apt(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0xc, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint8_t gauge_get_apa(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0xb, rec_buf, 3);

	if(control != 3){
		DEBUG("get_apa(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0xb, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_current_direction(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0xa, rec_buf, 3);

	if(control != 3){
		DEBUG("get_current_direction(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0xa, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_thermistor_b(i2c_t dev)
{
	uint8_t rec_buf[3];
	int8_t control = i2c_read_regs(dev, 0xb, 0x6, rec_buf, 3);

	if(control != 3){
		DEBUG("get_thermistor_b(): Error  reading or writing\n");
		return 0;
	}
	uint8_t crc_buf[5] = {0x16, 0x6, 0x17, rec_buf[0], rec_buf[1]};
	if(_get_crc(crc_buf,5) != rec_buf[2]){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

int8_t gauge_set_rsoc_before(i2c_t dev)
{
	uint8_t crc_buf[4] = {0x16, 0x4, 0x55, 0xAA};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_thermistor_b(i2c_t dev, uint16_t value)
{
	uint8_t crc_buf[4] = {0x16, 0x6, value, value<<8};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_rsoc_initial(i2c_t dev)
{
	uint8_t crc_buf[4] = {0x16, 0x7, 0x55, 0xAA};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_cell_temp(i2c_t dev, uint16_t value)
{
	if(!(value >= 0x9e4 && value <=0xD04)){
		DEBUG("gauge_set_cell_temp(): temp outside of range \n");
		return -2;
	}
	uint8_t crc_buf[4] = {0x16, 0x8, value, value<<8};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_current_direction(i2c_t dev, current_direction direction)
{
	uint8_t crc_buf[4] = {0x16, 0xA,(uint8_t)direction, (uint8_t)direction<<8};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_apa(i2c_t dev, uint8_t value)
{
	uint8_t crc_buf[4] = {0x16, 0xb, value,0};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_apt(i2c_t dev, uint16_t value)
{
	uint8_t crc_buf[4] = {0x16, 0xc, value,value<<8};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_change_of_parameter(i2c_t dev, battery_profile value)
{
	uint8_t crc_buf[4] = {0x16, 0x12, (uint8_t) value, (uint8_t) value<<8};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_alarm_low_rsoc(i2c_t dev, uint8_t value)
{
	uint8_t crc_buf[4] = {0x16, 0x13, value, 0};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_alarm_low_cell_voltage(i2c_t dev, uint16_t value)
{
	uint8_t crc_buf[4] = {0x16, 0x14, value, value<<8};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_power_mode(i2c_t dev, power_mode value)
{
	uint8_t crc_buf[4] = {0x16, 0x15, (uint8_t) value, 0};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}

int8_t gauge_set_status_bit(i2c_t dev, temp_obtaining_mode value)
{
	uint8_t crc_buf[4] = {0x16, 0x15, (uint8_t) value, 0};
	uint8_t send_buf[3] = {crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4)};
	return i2c_write_regs(dev, 0xb, crc_buf[1], send_buf, 3);
}
