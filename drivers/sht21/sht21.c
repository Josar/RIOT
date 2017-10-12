/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#define ENABLE_DEBUG	(0)
#include "debug.h"

#include "sht21.h"
#include <avr/io.h>


/*
 * does a crc check and returns 0 for passed and -1 for failed
 */
int8_t _check_crc(uint8_t *rec_values, uint8_t right_crc)
{
	uint8_t crc = 0xFF;
	uint8_t current_byte;
	uint8_t bit;

	for (current_byte = 0; current_byte < 2; current_byte++)
	{
		crc ^= (rec_values[current_byte]);
		for (bit = 8; bit > 0; bit--)
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ 0x31;
			else
				crc = (crc << 1);
		}
	}
	return !(crc == right_crc);
}

int8_t sht21_init(i2c_t dev)
{
	int8_t error = i2c_init_master(dev, I2C_SPEED_FAST);
	return error;
}

float sht21_measure(i2c_t dev, crc_type crc, measure_type type)
{
	if(!(type == TEMP || type == HUMIDITY)){
		DEBUG("Invalid measurement type %x \n", (uint8_t) type);
		return 0;
	}
	i2c_write_byte(dev, 0x40, (uint8_t) type);
	uint8_t received[3];
	i2c_read_bytes(I2C_0, 0x40, received, 3);
	uint16_t tmp = ((received[0]<<8)|received[1]);
	if(crc){
		if((_check_crc(&received[0], received[2]))==-1){
			/* crc check failed */
			DEBUG("CRC Error");
			return -1;
		}
		DEBUG("CRC Passed! \n");
	}
	if(type == TEMP){
		return (((float)(tmp&0xfffc)/65536)*175.72-46.85);
	}else {
		return (((float)(tmp&0xfffc)/65536)*125-6);
	}

	return 0;
}


int8_t sht21_reset(i2c_t dev)
{
	int8_t check = i2c_write_byte(dev, 0x40, 0b11111110);
	if(check != 1){
		/* error occured return -1 */
		return -1;
	}
	return 0;
}

uint8_t sht21_read_settings(i2c_t dev)
{
	i2c_write_byte(dev, 0x40, 0b11100111);
	uint8_t received;
	i2c_read_bytes(I2C_0, 0x40,&received, 1);
	return received;
}

uint8_t sht21_write_settings(i2c_t dev, uint8_t setting)
{
	uint8_t write_buf[2] = {0b11100110, setting};
	uint8_t cnt_wrote= i2c_write_bytes(dev, 0x40, write_buf, 2);
	if(cnt_wrote == 2){
		return 0;
	}
	return 1;
}

uint8_t sht21_is_end_of_battery(i2c_t dev)
{
	return ((sht21_read_settings(dev)&STATUS_END_OF_BAT) >> 6);
}
