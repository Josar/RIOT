/*
 * Copyright (C) Josua Arndt, Steffen Robertz 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author          Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"
#include "atmega_regs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Length of the CPU_ID in octets
 * @{
 */
#define CPUID_LEN           (8U)

/** @} */

/**
 * @name   Available ports on the ATmega256rfr family
 * @{
 */
enum {
    PORT_B  = 1,        /**< port B */
    PORT_D  = 3,        /**< port D */
    PORT_E  = 4,        /**< port E */
    PORT_F  = 5,        /**< port F */
    PORT_G  = 6,        /**< port G */
};

/**
 * @name   Defines for the I2C interface
 * @{
 */
#define I2C_PORT_REG            PORTD
#define I2C_PIN_MASK            (1 << PORTD1) | (1 << PORTD0)
/** @} */

/**
 * @name   GPIO pin not defined
 * @{
 */
#ifndef GPIO_UNDEF
#define GPIO_UNDEF          (0xFFFF)
#endif
/** @}*/

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
