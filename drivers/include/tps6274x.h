/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tps6274x TPS6274x
 * @ingroup     drivers_power
 * @brief       Device driver interface for the TPS6274x Step Down Converter
 * @{
 *
 * @file
 * @brief       Device driver interface for the PS6274x Step Down Converter
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef TPS6274X_H
#define TPS6274X_H

#include "periph/gpio.h"
#include "inttypes.h"


/**
 * @brief init converter
 *
 * @param[in] voltage			Voltage to set in mV (needs to be between 1.8V-3.3V
 * @return		0						init completed
 * @return 		1						voltage not between 1.8-3.3V
 *
 */

uint8_t stepdown_init(uint16_t voltage);

/**
 * @brief switch to different voltage level
 *
 * @param[in] voltage			Voltage to set in mV (needs to be between 1.8V-3.3V
 * @return		0						init completed
 * @return 		1						voltage not between 1.8-3.3V
 *
 */

uint8_t stepdown_switch_voltage(uint16_t voltage);


/**
 * @brief sets ctrl pin high to power a subsystem connected on the load pin
 *
 */
void stepdown_enable_load(void);

/**
 * @brief sets ctrl pin low to poweroff a subsystem connected on the load pin
 *
 */
void stepdown_disabel_load(void);

#endif
