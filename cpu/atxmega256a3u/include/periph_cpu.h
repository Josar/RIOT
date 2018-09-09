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
 * @brief   Available ports on the ATxmega256a3u family
 */
enum
{
    PORT_A = 0,  /**< port A */
    PORT_B = 1,  /**< port B */
    PORT_C = 2,  /**< port C */
    PORT_D = 3,  /**< port D */
    PORT_E = 4,  /**< port E */
    PORT_F = 5,  /**< port F */
//    PORT_H = 6  /**< port H */
//    PORT_J = 7,  /**< port J */
//    PORT_K = 8,  /**< port K */
//    PORT_Q = 9,  /**< port Q */
    PORT_R = 10, /**< port R */
};


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
