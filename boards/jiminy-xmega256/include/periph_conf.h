/*
 * Copyright (C) 2018 Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_jiminy-xmega256a3u-at86rf233
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the
 *              Jiminy Xmega256a3u at86rfr233 board
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */
#include "mutex.h"

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <avr/io.h>

#include "periph_cpu_common.h"

/**
 * @brief   Timer configuration
 *
 * ATTETION Timer 0 is used for Xtimer which is system Timer
 *
 * RIOT Timer 0 is Timer Counter D1
 * @{
 */
#define TIMER_NUMOF         (1U)

#define TIMER_0             (&TCD1)
#define TIMER_0_MASK 		()
#define TIMER_0_FLAG		()
#define TIMER_0_OVF         TCD1_OVF_vect
#define TIMER_0_ISRA        TCD1_CCA_vect
#define TIMER_0_ISRB        TCD1_CCB_vect
/** @} */


/**
 * @brief   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (1U)

// UART0 is used for stdio
#define UART_0              (&USARTC1)
#define UART_0_RXC_ISR		USARTC1_RXC_vect /* Reception Complete Interrupt */
#define UART_0_DRE_ISR		USARTC1_DRE_vect /* Data Register Empty Interrupt */
#define UART_0_TXC_ISR		USARTC1_TXC_vect /* Transmission Complete Interrupt */
/** @} */

//typedef struct XMEGA_UART_T{
//    USART_t dev;
//    register8_t pr;
//    uart_isr_ctx_t isr;
//
//} xmega_uart_t;
//
//#define UART0_CONF {.dev = (&USARTC1),
//		        .pr = (&PR.PRPC),
//                .rx_isr = {NULL} }
//extern xmega_uart_t const UART0;       /**< UART0 Instance */

/**
 * @name    SPI configuration
 *
 * The atmega256a3u has only one hardware SPI with fixed pin configuration, so all
 * we can do here, is to enable or disable it...
 *
 * The fixed pins for jiminy-xmega256a3u-at86rf233 are:
 *
 * RF_/RST   - PD1
 * RF_IRQ    - PD2
 * RF_SLP_TR - PD3
 *
 * SS   - PD4
 * MOSI - PD5
 * MISO - PD6
 * SCK  - PD7
 *
 *
 * @{
 */
/* set to 0 to disable SPI */
#define SPI_NUMOF           1

/* Values for connected transceiver */

/* Used for fixed SPI port D config in atxmega_common/spi.c */
#define AT86RF2XX_PARAM_SPI 1 // is fixed to SPID in the moment
#define AT86RF2XX_PARAM_SPI_CLK (SPI_CLK_4MHZ)
#define AT86RF2XX_PARAM_SPI_CLK_PIN (PIN7_bm)
#define AT86RF2XX_PARAM_MISO PIN6_bm
#define AT86RF2XX_PARAM_MOSI PIN5_bm
#define AT86RF2XX_PARAM_CS PIN4_bm
#define AT86RF2XX_PARAM_SLEEP PIN3_bm
#define AT86RF2XX_PARAM_INT PIN2_bm
#define AT86RF2XX_PARAM_RESET PIN1_bm



// initialize SPI
/**
 * @brief   Set default configuration parameters for the AT86RF2xx driver for the IAS-UDB
 * @{
 */

#define AT86RF2XX_PARAMS    {.spi = SPI_DEV(PORT_D), \
                            .spi_clk = SPI_CLK_4MHZ, \
                            .cs_pin = GPIO_PIN(PORT_D,4), \
                            .int_pin = GPIO_PIN(PORT_D,2), \
                            .sleep_pin = GPIO_PIN(PORT_D,3), \
                            .reset_pin = GPIO_PIN(PORT_D,1) }
/**@}*/


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */