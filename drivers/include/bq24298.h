/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bq24298 BQ24298
 * @ingroup     drivers_power
 * @brief       Device driver interface for the BQ24298 Single Cell USB Charger
 * @{
 *
 * @file
 * @brief        Device driver interface for the BQ24298 Single Cell USB Charger
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef BQ24298_H
#define BQ24298_H

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "inttypes.h"

/* help definitions */

//Saves the default values, addressed by reset_value[reg]
static const uint8_t reset_value[8] = {0x30, 0x1b, 0x60, 0x11, 0xb2, 0xdc, 0x73, 0x4b};

#define REG_INPUT_SOURCE_CONTROL								(0x00)
#define REG_POWER_ON_CONFIG											(0x01)
#define REG_CURRENT_CONTROL											(0x02)
#define REG_PRE_CHARGE_CURRENT_CONTROL				(0x03)
#define REG_VOLTAGE_CONTROL											(0x04)
#define REG_CHARGE_TERMINATION_AND_TIMER			(0x05)
#define REG_BOOST_VOLATGE_AND_THERMAL					(0x06)
#define REG_MISC_OPERATION												(0x07)


/**
 * @brief initializes the sensor and i2c
 *
 * @param[in] dev				i2c device to be used
 *
 * @return					0 on a working initialization
 * @return                  -1 on undefined i2c device given in periph_conf
 * @return                  -2 on unsupported speed value
 */
int8_t charger_init(i2c_t dev);

/**
 * @brief  reads system status from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						system status register
 */
uint8_t charger_get_system_status(i2c_t dev);


/**
 * @brief  reads new fault register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						new fault register
 */
uint8_t charger_get_new_fault(i2c_t dev);

/**
 * @brief  reads input source control register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						input source control register
 */
uint8_t charger_get_input_source_control(i2c_t dev);

/**
 * @brief  reads power-on configuration register register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						 power-on configuration register
 */
uint8_t charger_get_power_on_config(i2c_t dev);

/**
 * @brief  reads current control register register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						current control register
 */
uint8_t charger_get_current_control(i2c_t dev);

/**
 * @brief  reads Pre-Charge/Termination current control register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						Pre-Charge/Termination current control
 */
uint8_t charger_get_pre_charge_current_control(i2c_t dev);

/**
 * @brief  reads charge voltage control register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						charge voltage control register
 */
uint8_t charger_get_voltage_control(i2c_t dev);

/**
 * @brief  reads charge Termination/Timer control register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						charge Termination/Timer control register
 */
uint8_t charger_get_charge_termination_and_timer(i2c_t dev);

/**
 * @brief  reads Boost Voltage/Thermal Regulation Control register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						Boost Voltage/Thermal Regulation Control register
 */
uint8_t charger_get_boost_voltage_and_thermal(i2c_t dev);

/**
 * @brief  reads misc operation control register from sensor
 *
 * @param[in] dev			i2c device to be used
 *
 * @return 						misc operation control register
 */
uint8_t charger_get_misc_operation(i2c_t dev);


/**
 * @brief  resets register to its default Values
 *
 * @param[in] dev			i2c device to be used
 * @param[in] reg				Register adress, use defines starting with REG_...
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
uint8_t charger_reg_reset(i2c_t dev, uint8_t reg);
#endif
