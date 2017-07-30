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
#endif
