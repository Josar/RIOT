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
		int bytes_read = i2c_read_regs(i2c_dev, (MAX17048), REG_VRESET_ID, my_data, 2);
		if(bytes_read == 2) {
			//read everything
			DEBUG("Read: %x%x \n", my_data[0], my_data[1]);
			DEBUG("\n\nID %x \n\n”", my_data[0]);
			return my_data[0];
		}
	}else{
		DEBUG("Trying to use I2C without initialisation \n");
	}
	return 0;
}

uint8_t gauge_soc(void)
{
	if(i2c_dev != I2C_INVALID) {
		uint8_t my_data[2];
		int bytes_read = i2c_read_regs(i2c_dev, MAX17048, REG_SOC, my_data, 2);
		if(bytes_read != 2){
				DEBUG("Error while reading VCELL, only %u Bytes read \n", bytes_read);
				return 0;
		}
		DEBUG("Read: %u %u \n", my_data[0], my_data[1]);
		DEBUG("\n\n SOC: %u \n\n", my_data[0]);
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
				int bytes_read = i2c_read_regs(i2c_dev, MAX17048, REG_VCELL, my_data, 2);
				if(bytes_read != 2){
					DEBUG("Error while reading VCELL, only %u Bytes read \n", bytes_read);
					return 0;
				}
				DEBUG("Read: %u %u \n", my_data[0], my_data[1]);
				uint16_t vcell_temp = ((my_data[0]<<8)|(my_data[1]));
				float vcell = vcell_temp * 0.000078125;
				DEBUG(" \n\nVCELL: %.2f \n\n", vcell);
				return vcell;
		}else{
			DEBUG("Trying to use I2C without initialisation \n");
		}
		return 0;
}

float gauge_discharge_rate(void)
{
	if(i2c_dev != I2C_INVALID){
			uint8_t my_data[2];
			int bytes_read = i2c_read_regs(i2c_dev, MAX17048, REG_CRATE, my_data, 2);
			if(bytes_read != 2){
				DEBUG("Error while reading CRATE, only %u Bytes read \n", bytes_read);
				return 0;
			}
			DEBUG("Read: %u %u \n", my_data[0], my_data[1]);
			uint16_t rate_temp = ((my_data[0]<<8)|(my_data[1]));
			float rate = rate_temp * 0.208;
			DEBUG("\n \nCharging Rate: %.2f%% \n\n", rate_temp);
			return rate;
	}else{
		DEBUG("Trying to use I2C without initialisation \n");
		return 0;
	}
}

#ifdef __cplusplus
}
#endif
