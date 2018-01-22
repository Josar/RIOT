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


#include <avr/eeprom.h>
/* EEPROM values */
extern __attribute__((section(".eeprom"))) uint8_t TPS6274x_input_source_control;
extern __attribute__((section(".eeprom"))) uint8_t TPS6274x_charge_current_control;

typedef enum{
	CHRARGE_FAULT_INPUT_FAULT = 1,
	CHARGE_FAULT_THERMAL_SHUTDOWN =  2,
	CHARGE_FAULT_TIMER_EXPIRED = 3
}charge_fault_t;

typedef enum{
	NTC_FAULT_NORMAL = 0,
	NTC_FAULT_COLD = 2,
	NTC_FAULT_HOT = 1
}ntc_fault_t;

typedef enum{
	VBUS_STAT_UNKNOWN=0,
	VBUS_STAT_USB_HOST=1,
	VBUS_STAT_ADAPTER_PORT=2,
	VBUS_STAT_OTG=3
}vbus_stat_t;

typedef enum{
	CHARGE_STATUS_NOT_CHARGING = 0,
	CHARGE_STATUS_PRE_CHARGE = 1,
	CHARGE_STATUS_FAST_CHARGING = 2,
	CHARGE_STATUS_CHARGE_TERMINATION = 3
}charge_stat_t;


typedef struct {
	uint8_t watchdog_expired;
	uint8_t otg_boost_error;
	charge_fault_t charge_fault;
	uint8_t bat_ovp; //overvoltage
	ntc_fault_t ntc_fault;
	vbus_stat_t status_vbus;
	charge_stat_t status_charge;
	uint8_t power_good;
}charger_status_t;



/* help definitions */

#define REG_INPUT_SOURCE_CONTROL											(0x00)
#define REG_POWER_ON_CONFIG														(0x01)
#define REG_CURRENT_CONTROL														(0x02)
#define REG_PRE_CHARGE_CURRENT_CONTROL							(0x03)
#define REG_VOLTAGE_CONTROL														(0x04)
#define REG_CHARGE_TERMINATION_AND_TIMER						(0x05)
#define REG_BOOST_VOLATGE_AND_THERMAL								(0x06)
#define REG_MISC_OPERATION															(0x07)

#define REG0_EN_HIZ																				(0b10000000)
#define REG0_VINDPM3																			(0b01000000)
#define REG0_VINDPM2																			(0b00100000)
#define REG0_VINDPM1																			(0b00010000)
#define REG0_VINDPM0																			(0b00001000)
#define REG0_VINDPM																			(0b01111000)
#define REG0_IINLIM2																				(0b00000100)
#define REG0_IINLIM1																				(0b00000010)
#define REG0_IINLIM0																				(0b00000001)
#define REG0_IINLIM																				(0b00000111)

#define REG1_REGISTER_RESET															(0b10000000)
#define REG1_I2C_WATCHDOG_TIMER_RESET								(0b01000000)
#define REG1_OTG_CONFIG																	(0b00100000)
#define REG1_CHG_CONFIG																	(0b00010000)
#define REG1_SYS_MIN2																		(0b00001000)
#define REG1_SYS_MIN1																		(0b00000100)
#define REG1_SYS_MIN0																		(0b00000010)
#define REG1_SYS_MIN																			(0b00001110)
#define REG1_BOOST_LIM																		(0b00000001)

#define REG2_ICHG5																				(0b10000000)
#define REG2_ICHG4																				(0b01000000)
#define REG2_ICHG3																				(0b00100000)
#define REG2_ICHG2																				(0b00010000)
#define REG2_ICHG1																				(0b00001000)
#define REG2_ICHG0																				(0b00000100)
#define REG2_ICHG																					(0b11111100)
#define REG2_BCOLD																				(0b00000010)
#define REG2_FORCE_20PCT																(0b00000001)

#define REG3_IPRECHG3																		(0b10000000)
#define REG3_IPRECHG2																		(0b01000000)
#define REG3_IPRECHG1																		(0b00100000)
#define REG3_IPRECHG0																		(0b00010000)
#define REG3_IPRECHG																			(0b11110000)
#define REG3_ITERM2																				(0b00000100)
#define REG3_ITERM1																				(0b00000010)
#define REG3_ITERM0																				(0b00000001)
#define REG3_ITERM																				(0b00000111)

#define REG4_VREG5																				(0b10000000)
#define REG4_VREG4																				(0b01000000)
#define REG4_VREG3																				(0b00100000)
#define REG4_VREG2																				(0b00010000)
#define REG4_VREG1																				(0b00001000)
#define REG4_VREG0																				(0b00000100)
#define REG4_VREG																					(0b11111100)
#define REG4_BATLOWV																		(0b00000010)
#define REG4_VRECHG																			(0b00000001)

#define REG5_EN_TERM																			(0b10000000)
#define REG5_BATFET_RST_EN															(0b01000000)
#define REG5_WATCHDOG1																	(0b00100000)
#define REG5_WATCHDOG0																	(0b00010000)
#define REG5_WATCHDOG																		(0b00110000)
#define REG5_EN_TIMER																		(0b00001000)
#define REG5_CHG_TIMER1																	(0b00000100)
#define REG5_CHG_TIMER2																	(0b00000010)
#define REG5_CHG_TIMER																		(0b00000110)

#define REG6_BOOSTV3																			(0b10000000)
#define REG6_BOOSTV2																			(0b01000000)
#define REG6_BOOSTV1																			(0b00100000)
#define REG6_BOOSTV0																			(0b00010000)
#define REG6_BOOSTV																			(0b11110000)
#define REG6_BHOT1																				(0b00001000)
#define REG6_BHOT0																				(0b00000100)
#define REG6_BHOT																				(0b00001100)
#define REG6_TREG1																				(0b00000010)
#define REG6_TREG0																				(0b00000001)
#define REG6_TREG																					(0b00000011)


#define REG7_DPDM_EN																		(0b10000000)
#define REG7_TMR2X_EN																		(0b01000000)
#define REG7_BATFET_DISABLE															(0b00100000)
#define REG7_INT_MASK1																		(0b00000010)
#define REG7_INT_MASK0																		(0b00000001)

#define REG8_VBUS_STAT1																	(0b10000000)
#define REG8_VBUS_STAT0																	(0b01000000)
#define REG8_VBUS_STAT																		(0b11000000)
#define REG8_CHRG_STAT1																	(0b00100000)
#define REG8_CHRG_STAT0																	(0b00010000)
#define REG8_CHRG_STAT																		(0b00110000)
#define REG8_DPM_STAT																		(0b00001000)
#define REG8_PG_STAT																			(0b00000100)
#define REG8_THERM_STAT																	(0b00000010)
#define REG8_VSYS_STAT																		(0b00000001)

#define REG9_WATCHDOG_FAULT														(0b10000000)
#define REG9_OTG_FAULT																		(0b01000000)
#define REG9_CHRG_FAULT1																	(0b00100000)
#define REG9_CHRG_FAULT0																	(0b00010000)
#define REG9_CHRG_FAULT																	(0b00110000)
#define REG9_BAT_FAULT																		(0b00001000)
#define REG9_NTC_FAULT1																	(0b00000010)
#define REG9_NTC_FAULT0																	(0b00000001)
#define REG9_NTC_FAULT																		(0b00000011)

//Saves the default values, addressed by reset_value[reg]
static const uint8_t reset_value[8] = {0x30, 0x1b, 0x60, 0x11, 0xb2, 0xdc, 0x73, 0x4b};

typedef void (*charger_cb_t)(uint8_t fault, uint8_t status, void *arg);
/**
 * @brief initializes the sensor and i2c
 *
 * @param[in] dev				i2c device to be used
 *
 * @return					0 on a working initialization
 * @return                  -1 on undefined i2c device given in periph_conf
 * @return                  -2 on unsupported speed value
 */
int8_t charger_init(i2c_t dev, gpio_t alarm_pin, charger_cb_t cb, void *arg);

/**
 * @brief  returns charger status struct
 *
 *
 * @return 						status struct
 */
charger_status_t charger_get_status_struct(void);

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
 * @param[in] reg				Register address, use defines starting with REG_...
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t charger_reg_reset(i2c_t dev, uint8_t reg);

/**
 * @brief  writes settings to register
 *
 * @param[in] dev			i2c device to be used
 * @param[in] reg				Register address, use defines starting with REG_...
 * @param[in] settings		Settings for the specified register, use | to set multiple bytes (REG7_DPDM_EN|REG7_TMR2X_EN)
 *
 * @return                  the number of bytes that were written
 * @return                  -1 on undefined device given
 */
int8_t charger_write_reg(i2c_t dev, uint8_t reg, uint8_t settings);
#endif
