/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lc709203f LC709203F
 * @ingroup     drivers_sensor
 * @brief       Device driver interface for the LC709203F Battery Fuel Gauge
 * @{
 *
 * @file
 * @brief       Device driver interface for the LC709203F Battery Fuel Gauge
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef LC709203F_H
#define LC709203F_H

#include "periph/i2c.h"

typedef enum{
	AUTO_MODE = 0x0,
	CHARGE_MODE = 0x1,
	DISCHARGE_MODE = 0xffff
}current_direction;

typedef enum{
	BAT_PROFILE_1 = 0,
	BAT_PROFILE_2 = 1
}battery_profile;

typedef enum{
	OPERATIONAL_MODE = 1,
	SLEEP_MODE = 2
}power_mode;

typedef enum{
	I2C_MODE = 0,
	THERMISTOR_MODE = 1
} temp_obtaining_mode;

/**
 * @brief initializes the sensor and i2c
 *
 * @param[in] dev			i2c device to be used
 *
 * @return					0 on a working initialization
 * @return                  -1 on undefined i2c device given in periph_conf
 * @return                  -2 on unsupported speed value
 */
int8_t gauge_init(i2c_t dev);

/**
 * @brief  reads battery voltage from Sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						voltage in mV
 */
uint16_t gauge_get_voltage(i2c_t dev);

/**
 * @brief  reads battery state of charge from Sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							rsoc in %
 */
uint8_t gauge_get_rsoc(i2c_t dev);

/**
 * @brief  reads battery indicator to empty from Sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							ite in 0.1%
 */
uint16_t gauge_get_ite(i2c_t dev);

/**
 * @brief  reads Sensor id
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							id value
 */
uint16_t gauge_get_id(i2c_t dev);

/**
 * @brief  reads cell temperature from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							temp in 0.1°C steps
 */
uint16_t gauge_get_cell_temp(i2c_t dev);

/**
 * @brief  reads status bit register (Temperature obtaining mode)
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							1	Thermistor mode
 * @return							0	I2C Mode
 */
uint8_t gauge_get_status_bit(i2c_t dev);

/**
 * @brief  reads IC Power Mode register
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							1	Operational Mode
 * @return							2	Sleep mode
 */
uint8_t gauge_get_power_mode(i2c_t dev);

/**
 * @brief  reads Alarm Low Cell Voltage Register
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							0	Disable
 * @return							else Threshold value in 1mV steps
 */
uint16_t gauge_get_alarm_low_voltage(i2c_t dev);

/**
 * @brief  reads Alarm Low RSOC Register
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							0	Disable
 * @return							else Threshold value in %
 */
uint8_t gauge_get_alarm_low_rsoc(i2c_t dev);

/**
 * @brief  reads Change of the Parameter register, Battery Profile selection
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							Bateery profile number (1 or 0)
 */
uint8_t gauge_get_change_of_parameter(i2c_t dev);

/**
 * @brief  readsAPT (Adjustment Pack Thermistor)
 * value to adjust temperature measurement delay timing.
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							 value to adjust temperature measurement delay timing.
 */
uint16_t gauge_get_apt(i2c_t dev);


/**
 * @brief  readsAPA (Adjustment Pack Application) in 1mOhm steps
 * value to adjust temperature measurement delay timing.
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							 parasitic impedance
 */
uint8_t gauge_get_apa(i2c_t dev);

/**
 * @brief  reads Current Direction Register
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							 0 Auto mode
 * @return							1	charge mode
 * @return							0xffff	DIscharge mode
 */
uint16_t gauge_get_current_direction(i2c_t dev);

/**
 * @brief  readsThermistor-B register (Thermistor B constant to be measured)
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 							 B Value in 1K steps
 */
uint16_t gauge_get_thermistor_b(i2c_t dev);

/**
 * @brief  Executes RSOC initialization with sampled maximum voltage
 *
 * @param[in] dev			i2c device to be used
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_rsoc_before(i2c_t dev);

/**
 * @brief  Sets B constant of the Thermistor
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			Bconstant of thermistor
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_thermistor_b(i2c_t dev, uint16_t value);

/**
 * @brief  Executes RSOC initialization
 *
 * @param[in] dev			i2c device to be used
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_rsoc_initial(i2c_t dev);

/**
 * @brief  Sets cell temperature in i2c-mode
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			temp in 0.1K 0xAAC=0°C
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 * @return					-2 on invalid value
 */
int8_t gauge_set_cell_temp(i2c_t dev, uint16_t value);

/**
 * @brief  Sets current direction
 *
 * @param[in] dev			i2c device to be used
 * @param[in] direction	current direction (enum), AUTO_MODE, CHARGE_MODE, DISCHARGE_MODE
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_current_direction(i2c_t dev, current_direction direction);

/**
 * @brief  Sets parasitic impedance (adjustment pack application)
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			impedance in 1mOhm steps
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_apa(i2c_t dev, uint8_t value);

/**
 * @brief  Sets value to adjust temperature measurement delay timing (adjusmtent pack thermistor)
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			adjustment value
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_apt(i2c_t dev, uint16_t value);

/**
 * @brief  Sets battery profile
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			profile (enum) BAT_PROFILE_1 BAT_PROFILE_2
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_change_of_parameter(i2c_t dev, battery_profile value);

/**
 * @brief  Sets threshold for low rsoc alert
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			alert threshold (0-100%)
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_alarm_low_rsoc(i2c_t dev, uint8_t value);

/**
 * @brief  Sets threshold for low cell voltage alert
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			alert threshold in 1mV steps
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_alarm_low_rsoc(i2c_t dev, uint16_t value);

/**
 * @brief  Sets power mode
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			Power mode (enum) OPERATIONAL_MODE, SLEEP_MODE
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_power_mode(i2c_t dev, power_mode value);


/**
 * @brief  Sets temperature obtaining method
 *
 * @param[in] dev			i2c device to be used
 * @param[in] value			method to be used (enum) I2C_MODE, THERMISTOR_MODE
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t gauge_set_status_bit(i2c_t dev, temp_obtaining_mode value);
#endif
