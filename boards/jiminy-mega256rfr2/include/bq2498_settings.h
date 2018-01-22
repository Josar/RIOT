
/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     bq2498_settings.h
 * @{
 *
 * @file
 * @brief       BQ2498 EEPROM values for Pinoccio Mega 256rfr2 board
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */


#ifndef BQ2498_SETTINGS_H_
#define BQ2498_SETTINGS_H_

#include <avr/eeprom.h>

extern __attribute__((section(".eeprom"))) uint8_t EEPROM_BQ2496[8];

#endif /* BQ2498_SETTINGS_H_ */
