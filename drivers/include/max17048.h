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

/* Start with defines for the configuration */

#define REG_MODE				(0x06)
#define REG_VERSION				(0x08)
#define REG_HIBRT				(0x0A)
#define REG_CONFIG				(0x0C)
#define REG_VALRT				(0x14)
#define REG_CRATE				(0x16)
#define REG_VRESET_ID			(0x18)
#define	REG_STATUS				(0x1A)
#define REG_CMD					(0xFE)

#define MODE_QUICK_START		(0x4000)
#define MODE_ENSLEEP			(0x2000)
#define MODE_HIBSTAT			(0x1000)

#define HIBRT_ACTTHR			(0x00FF)
#define HIBRT_HIBTHR			(0xFF00)

#define CONFIG_RCOMP_DEFAULT 	(0x9700)
#define CONFIG_SLEEP_FORCE_ON	(0x0080)
#define CONFIG_ALSC				(0x0040)
#define CONFIG_ALRT				(0x0020)
#define CONFIG_ATHD				(0x0010)

#define VRESET_ID_DIS			(0x0100)	//Set to 1 to disable Analog Comparator in Hibernation

#define STATUS_RI				(0x0100)	//Set to 1: Device is powering up
#define STATUS_VH				(0x0200)	//Alert desciptor Voltage High
#define STATUS_VL				(0x0400)	//Alert descriptor Voltage Low
#define STATUS_VR				(0x0800)	//Alert descriptor Voltage reset
#define STATUS_HD				(0x1000)	//Alert descriptor State of charge low
#define STATUS_SC				(0x2000)	//alert descriptor state of charge 1% change
#define STATUS_ENVR				(0x4000)	/* when set to 1 asserts
												the ALRT pin when a voltage-reset event occurs*/
#define CMD_RESET				(0x5400)

#define MAX17048_WRITE			(0x6C<<1)
#define MAX17048_READ			(0x6D<<1)

#define I2C_WRITE				(0x00)
#define I2C_READ				(0x01)


#define I2C_INVALID				(0xFF)

//i2c_t i2c_dev;

int gauge_init(i2c_t dev);

void gauge_setup(uint16_t reg_mode_config, uint16_t reg_hibrt_config, uint16_t reg_config_config, \
		uint16_t reg_valrt_config, uint16_t reg_vreset_id_config);

void gauge_reboot(void);

uint8_t gauge_read_id(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*MAX17048_H_*/
