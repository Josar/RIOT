/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "lc709203f.h"

#define ENABLE_DEBUG		(1)
#include "debug.h"


/*
 * does a crc check and returns 0 for passed and -1 for failed
 */
int8_t _check_crc(uint8_t *rec_values, uint8_t right_crc)
{
	uint8_t crc = 0xFF;
	//uint8_t crc = 0x00;
	uint8_t current_byte;
	uint8_t bit;

	for (current_byte = 0; current_byte < 2; current_byte++)
	{
		crc ^= (rec_values[current_byte]);
		for (bit = 8; bit > 0; bit--)
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ 0xD5;
			else
				crc = (crc << 1);
		}
	}
	return !(crc == right_crc);
}


int8_t gauge_init(i2c_t dev)
{
	return i2c_init_master(dev, I2C_SPEED_FAST);
}

uint16_t gauge_get_voltage(i2c_t dev)
{
	int8_t control = 0;
	control = i2c_write_byte(dev, 0xb, 0x9);
	uint8_t rec_buf[3];
	control += i2c_write_bytes(dev, 0xb, rec_buf, 3);

	if(control != 4){
		DEBUG("get_voltage(): Error  reading or writing\n");
		return 0;
	}
	/*if(_check_crc(rec_buf, rec_buf[2])){
		DEBUG("CRC Error \n");
		return 0;
	}*/
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint8_t gauge_get_rsoc(i2c_t dev)
{
	int8_t control = 0;
	control = i2c_write_byte(dev, 0xb, 0xD);
	uint8_t rec_buf[3];
	control += i2c_write_bytes(dev, 0xb, rec_buf, 3);

	if(control != 4){
		DEBUG("get_rsoc(): Error  reading or writing\n");
		return 0;
	}
	/*if(_check_crc(rec_buf, rec_buf[2])){
		DEBUG("CRC Error \n");
		return 0;
	}*/
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_ite(i2c_t dev)
{
	int8_t control = 0;
	control = i2c_write_byte(dev, 0xb, 0xf);
	uint8_t rec_buf[3];
	control += i2c_write_bytes(dev, 0xb, rec_buf, 3);

	if(control != 4){
		DEBUG("get_rsoc(): Error  reading or writing\n");
		return 0;
	}
	if(_check_crc(rec_buf, rec_buf[2])){
		DEBUG("CRC Error \n");
		return 0;
	}
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_id(i2c_t dev)
{
	int8_t control = 0;
	control = i2c_write_byte(dev, 0xb, 0x11);
	uint8_t rec_buf[3];
	control += i2c_write_bytes(dev, 0xb, rec_buf, 3);

	if(control != 4){
		DEBUG("get_id(): Error  reading or writing\n");
		return 0;
	}
	/*if(_check_crc(rec_buf, rec_buf[2])){
		DEBUG("CRC Error \n");
		return 0;
	}*/
	return (((uint16_t)rec_buf[1]<<8)|rec_buf[0]);
}

uint16_t gauge_get_cell_temp(i2c_t dev)
{
	int8_t control = 0;
	control = i2c_write_byte(dev, 0xb, 0x8);
	uint8_t rec_buf[3];
	control += i2c_write_bytes(dev, 0xb, rec_buf, 3);

	if(control != 4){
		DEBUG("get_cell_temp(): Error  reading or writing\n");
		return 0;
	}
	if(_check_crc(rec_buf, rec_buf[2])){
		DEBUG("CRC Error \n");
		return 0;
	}
	return ((((uint16_t)rec_buf[1]<<8)|rec_buf[0])-273.15);
}
