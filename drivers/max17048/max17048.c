/*
 * Copyright 2017, RWTH-Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define ENABLE_DEBUG		(1)

#include "max17048.h"
#include "periph_conf.h"
#include "debug.h"

static i2c_t i2c_dev = I2C_INVALID;

int gauge_init(i2c_t dev)
{
	int error = i2c_init_master(dev, I2C_SPEED_FAST);
	if(error){
		DEBUG("Error initialising i2c \n");
		return error;
	}
	DEBUG("No Errors \n");
	i2c_dev = dev;
	return 0;

}

void gauge_setup(uint16_t reg_mode_config, uint16_t reg_hibrt_config, uint16_t reg_config_config, \
		uint16_t reg_valrt_config, uint16_t reg_vreset_id_config)
{
	if(i2c_dev != I2C_INVALID){
		int error=0;
			/* only change if the value isn't equal to 0 */
		uint8_t my_data[3];
		if(reg_mode_config != (int)NULL){
			my_data[0] = REG_MODE;
			my_data[1] = (reg_mode_config >> 8);
			my_data[2] = (reg_mode_config);
			uint8_t count=0;
			do{
				error = i2c_write_bytes(i2c_dev, (MAX17048), my_data, 3);
				count++;
				DEBUG("writing regg_mode Error: %d", error);
			}while(error != 0 && count<=3);
			error = 0;
		}
		if(reg_hibrt_config != (int)NULL){
			my_data[0] = REG_HIBRT;
			my_data[1] = (reg_hibrt_config >> 8);
			my_data[2] = (reg_hibrt_config);
			uint8_t count=0;
			do{
				error = i2c_write_bytes(i2c_dev, (MAX17048), my_data, 3);
				count++;
				DEBUG("writing reg_hibrt Error: %d", error);
			}while(error != 0 && count<=3);
			error = 0;
		}
		if(reg_config_config != (int)NULL){
			my_data[0] = REG_CONFIG;
			my_data[1] = (reg_config_config >> 8);
			my_data[2] = (reg_config_config);
			uint8_t count=0;
			do{
				error = i2c_write_bytes(i2c_dev, (MAX17048), my_data, 3);
				count++;
				DEBUG("writing reg_config Error: %d", error);
			}while(error != 0 && count<=3);
			error = 0;
		}
		if(reg_vreset_id_config != (int)NULL){
			my_data[0] = REG_VRESET_ID;
			my_data[1] = (reg_vreset_id_config >> 8);
			my_data[2] = (reg_vreset_id_config);
			uint8_t count=0;
			do{
				error = i2c_write_bytes(i2c_dev, (MAX17048), my_data, 3);
				count++;
				DEBUG("writing reg_vreset_id Error: %d", error);
			}while(error != 0 && count<=3);
			error = 0;
		}
		if(reg_valrt_config != (int)NULL){
			my_data[0] = REG_VALRT;
			my_data[1] = (reg_valrt_config >> 8);
			my_data[2] = (reg_valrt_config);
			uint8_t count=0;
			do{
				error = i2c_write_bytes(i2c_dev, (MAX17048), my_data, 3);
				count++;
				DEBUG("writing reg_hibrt Error: %d", error);
			}while(error != 0 && count<=3);
			error = 0;
		}
	}else{
		DEBUG("Trying to use I2C without initialisation");
	}
}

void gauge_reboot(void)
{
	if(i2c_dev != I2C_INVALID){
		uint8_t my_data[3] = {REG_CMD, (CMD_RESET>>8), (uint8_t)CMD_RESET};
		i2c_write_bytes(i2c_dev, (MAX17048), my_data, 3);
	}else{
		DEBUG("Trying to use I2C without initialisation");
	}
}

uint8_t gauge_read_id(void)
{
	if(i2c_dev != I2C_INVALID){
		uint8_t my_data[2];
		int error = i2c_write_byte(i2c_dev, (MAX17048), REG_VRESET_ID);
		DEBUG("Write Error %d \n", error);
		my_data[0] = 0;
		i2c_read_bytes(i2c_dev, (MAX17048), my_data, 2);
		DEBUG("Read: %u %u \n", my_data[0], my_data[1]);
	}else{
		DEBUG("Trying to use I2C without initialisation \n");
	}
	return 0;
}

uint8_t gauge_soc(void)
{
	if(i2c_dev != I2C_INVALID){
		uint8_t my_data[2];
		int error = i2c_write_byte(i2c_dev, (MAX17048), REG_SOC);
		DEBUG("Write Error %d \n", error);
		i2c_read_bytes(i2c_dev, (MAX17048), my_data, 2);
		DEBUG("Read: %u %u \n", my_data[0], my_data[1]);
		DEBUG("SOC: %u \n", my_data[0]);
		return my_data[0];
	}else{
		DEBUG("Trying to use I2C without initialisation \n");
	}
	return 0;
}

float gauge_voltage(void)
{
	if(i2c_dev != I2C_INVALID){
		uint8_t my_data[2];
		int error = i2c_write_byte(i2c_dev, (MAX17048), REG_VCELL);
		DEBUG("Write Error %d \n", error);
		i2c_read_bytes(i2c_dev, (MAX17048), my_data, 2);
		uint16_t temp_voltage = ((my_data[0]<<8) | my_data[1]);
		float voltage = temp_voltage * 0.000078125;
		DEBUG("Read: %x %x \n", my_data[0], my_data[1]);
		DEBUG("Voltage: %.2fV \n", voltage);
		return voltage;
	}else{
		DEBUG("Trying to use I2C without initialisation \n");
	}
	return 0;
}

float gauge_charge_rate(void)
{
	if(i2c_dev != I2C_INVALID){
			uint8_t my_data[2];
			int error = i2c_write_byte(i2c_dev, (MAX17048), REG_CRATE);
			DEBUG("Write Error %d \n", error);
			i2c_read_bytes(i2c_dev, (MAX17048), my_data, 2);
			uint16_t temp_charge_rate = ((my_data[0]<<8) | my_data[1]);
			float charge_rate = temp_charge_rate * 0.208;
			DEBUG("Read: %x %x \n", my_data[0], my_data[1]);
			DEBUG("Charge Rate: %.2f%/h \n", charge_rate);
			return charge_rate;
		}else{
			DEBUG("Trying to use I2C without initialisation \n");
		}
		return 0;
}

#ifdef __cplusplus
}
#endif
