/*
 * Copyright 2017, RWTH-Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MAX17048_H_
#define MAX17048_H_

/**
 * @defgroup    drivers_max17048 max17048
 * @ingroup     drivers_sensors
 * @brief       Driver to read and write to a max17048 Fuel Gauge
 *
 * 				Driver for the max17048 Fuel Gauge. For more information on settings etc
 * 				please check https://datasheets.maximintegrated.com/en/ds/MAX17048-MAX17049.pdf
 * @{
 *
 * @file
 * @brief       Driver to read and write to a max17048 Fuel Gauge
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */


#ifdef __cplusplus
extern "C" {
#endif


#include "periph/i2c.h"

enum on_off {OFF, ON};

//i2c_t i2c_dev;

int gauge_init(i2c_t dev);

void gauge_setup(uint16_t reg_mode_config, uint16_t reg_hibrt_config, uint16_t reg_config_config, \
		uint16_t reg_valrt_config, uint16_t reg_vreset_id_config);

void gauge_reset(void);

uint8_t gauge_id(void);

uint8_t gauge_soc(void);

float gauge_voltage(void);

float gauge_discharge_rate(void);

uint16_t gauge_version(void);

void gauge_sleep(enum on_off cmd);

void gauge_soc_alert_change(enum on_off cmd);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*MAX17048_H_*/
