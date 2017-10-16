/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sht21 SHT21
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the Sensirion SHT21 Humidity and Temp Sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for the SHT21 Humidity and Temperature Sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef SHT21_H
#define SHT21_H

#include "periph/i2c.h"
#include <stdint.h>

/* I2C Speeds defined in /drivers/include/periph/i2c.h I2C_SPEED_FAST = 400Hz */
#define SHTC1_SPEED		I2C_SPEED_FAST

typedef enum{
	CRC_DISABLED = 0,
	CRC_ENABLED
}crc_type;

typedef enum{
	TEMP = 0b11100011,
	HUMIDITY = 0b11100101
} measure_type;

#define RES_RH12_T14					0b00000000
#define RES_RH8_T12						0b00000001
#define RES_RH10_T13					0b10000000
#define RES_RH11_T11					0b10000001
#define STATUS_END_OF_BAT		0b01000000
#define EN_CHIP_HEATER				0b00000100
#define DIS_OTP_RELOAD				0b00000010

/**
 * @brief initializes the sensor and i2c
 *
 * @param[in] dev			i2c device to be used
 *
 * @return					0 on a working initialization
 * @return                  -1 on undefined i2c device given in periph_conf
 * @return                  -2 on unsupported speed value
 */
int8_t sht21_init(i2c_t dev);

/**
 * @brief reads temperature and humidity values
 *
 * @param[in] dev		The I2C Device
 * @param[in] type		the type of the measurement (Temp/Humidity)
 * @param[in] crc			Disable or enable crc checking
 *
 * @return					value defined by type (Temparture or Humidity)
 */
float sht21_measure(i2c_t dev, crc_type crc, measure_type type);

/**
 * @brief resets sensor
 *
 *This will reset all internal state machines and reload calibration data from the memory
 *
 * @param[in] dev		The I2C Device
 *
 * @return 					0 on everything done
 * @return					-1 on error occured
 */
int8_t sht21_reset(i2c_t dev);

/**
 * @brief this will read the settings register
 *
 * @param[in] dev 		The I2C Device
 *
 * @return 				1 on read correctly
 * @return				everything else on error
 */
uint8_t sht21_read_settings(i2c_t dev);

/**
 * @brief this will write the settings register
 *
 * @param[in] dev 		The I2C Device
 *
 * @return 				0 on correct write
 * @return				1 on error
 */
uint8_t sht21_write_settings(i2c_t dev, uint8_t setting);

/**
 * @brief this will check if the battery is getting empty
 *
 * @param[in] dev 		The I2C Device
 *
 * @return 				0 VDD > 2.25V
 * @return				1 VDD < 2.25V
 */
uint8_t sht21_is_end_of_battery(i2c_t dev);

#endif
