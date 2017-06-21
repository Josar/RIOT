/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       CMSIS style register definitions for the atmega family
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef ATMEGA_REGS_COMMON_H
#define ATMEGA_REGS_COMMON_H

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Peripheral register definitions and instances
 * @{
 */
#if defined(USARTC0)
// #define MEGA_UART0_BASE         ((uint16_t *)(&USARTC1))
#define XMEGA_UART0              ((USART_t *)(&USARTC1))
#define XMEGA_UART0_RX_ISR       USARTC1_RXC_vect
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ATMEGA_REGS_COMMON_H */
/** @} */
