/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of public functions for AT86RF2xx drivers
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */
/* As of now the implementation only supports the atmega256rfr2 */
#ifdef MODULE_AT86RFR2

#include "luid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_netdev.h"
#include "avr/interrupt.h"
#include "board.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define STRINGIFY(x) STRING(x)
#define STRING(x) #x

#ifndef RXTX_LED_ENABLE
    #define RXTX_LED_ENABLE (1)
    #define RXTX_LED_INIT (LED_PORT_DDR |= LED0_MASK)
    #define RXTX_LED_ON   LED0_ON
    #define RXTX_LED_OFF  LED0_OFF
#endif

#define DEBUG_ATRFR2_PINS (1)
#ifdef DEBUG_ATRFR2_PINS
    #define DEBUG_ATRFR2_PORT_DDR     (DDRF)
    #define DEBUG_ATRFR2_PORT         (PORTF)
    #define DEBUG_ATRFR2_PIN_TX_START (1<<PORTF7)
    #define DEBUG_ATRFR2_PIN_TX_END   (1<<PORTF6)
    #define DEBUG_ATRFR2_PIN_RX_END   (1<<PORTF5)
#endif

/* netdev_t to at86rfr2_dev for Interrupt handling */
netdev_t *at86rfr2_dev=0;

/* Counting Retries */
int8_t retries = 0;


void enable_rxtx_led(void){

#if RXTX_LED_ENABLE
    RXTX_LED_INIT;
#endif

#if DEBUG_ATRFR2_PINS
    /*initialize Debug Pins */
    /* Port Pin as Output */
    DEBUG_ATRFR2_PORT_DDR |=  ( DEBUG_ATRFR2_PIN_TX_START
                              | DEBUG_ATRFR2_PIN_TX_END|DEBUG_ATRFR2_PIN_RX_END);
    /* Pin Low */
    DEBUG_ATRFR2_PORT     &= ~( DEBUG_ATRFR2_PIN_TX_START
                              |DEBUG_ATRFR2_PIN_TX_END|DEBUG_ATRFR2_PIN_RX_END);
#endif
}

/**
 * \brief ISR for transceiver's receive end interrupt
 *
 *  Is triggered when valid data is received. FCS check passed.
 *  Save IRQ status and inform upper layer of data reception.
 */
ISR(TRX24_RX_END_vect, ISR_BLOCK){
    __enter_isr();

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT |= DEBUG_ATRFR2_PIN_RX_END;
#endif

    DEBUG("TRX24_RX_END\n");
    ((at86rf2xx_t *)at86rfr2_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__RX_END;
    /* Upper layer could also be called after TRX24_TX_END instead */
    at86rfr2_dev->event_callback(at86rfr2_dev, NETDEV_EVENT_ISR);

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT &= ~DEBUG_ATRFR2_PIN_RX_END;
#endif
    __exit_isr();
}

/**
 * \brief  Transceiver Frame Address Match, indicates incoming frame
 *
 *  Is triggered when Frame with valid Address is received.
 *  Can be sused to wake up MCU from sleep, etc.
 */
ISR(TRX24_XAH_AMI_vect, ISR_BLOCK){
    __enter_isr();
    DEBUG("TRX24_XAH_AMI\n");
    ((at86rf2xx_t *)at86rfr2_dev)->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__AMI;
    __exit_isr();
}

/**
 * \brief ISR for transceiver's transmit end interrupt
 *
 *  Is triggered when data or when acknowledge frames where send.
 */
ISR(TRX24_TX_END_vect, ISR_BLOCK){
    at86rf2xx_t *dev = (at86rf2xx_t *) at86rfr2_dev;

    __enter_isr();

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT |= DEBUG_ATRFR2_PIN_TX_END;
#endif
    /* As the call takes approx. 51 us this is a short not to bright light */
#if RXTX_LED_ENABLE
    RXTX_LED_ON;
#endif
    dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__TX_END;

    /* Only radios with the XAH_CTRL_2 register support frame retry reporting
     * but with TX_START and TX_END retries can also be count */
    ((at86rf2xx_t *)at86rfr2_dev)->tx_retries = retries;
    retries = -1;
    uint8_t state = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATE_MASK__TRAC;

    DEBUG("TRX24_TX_END\n");
    /* only inform upper layer when a transmission was done,
     * not for sending acknowledge frames if we received data */
    if (state != AT86RF2XX_STATE_RX_AACK_ON ) {
        /* set device back in receiving state */
        /* TODO
         * I seems that the upper layer don't set the receiver back to receive.
         * I consider this a preliminary solution
         * until i know where and how it should be done.
         * */
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);
        /* Call Upper layer to for statistics*/
        at86rfr2_dev->event_callback(at86rfr2_dev, NETDEV_EVENT_ISR);
    }
#if RXTX_LED_ENABLE
    RXTX_LED_OFF;
#endif

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT &= ~DEBUG_ATRFR2_PIN_TX_END;
#endif

    __exit_isr();
}

/**
 * \brief ISR for transceiver's TX_START interrupt
 *
 * Indicates the frame start of a transmitted acknowledge frame in procedure RX_AACK.
 * In procedure TX_ARET the TRX24_TX_START interrupt is issued separately for every
 * frame transmission and frame retransmission.
 */
ISR(TRX24_TX_START_vect){
    __enter_isr();

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT |= DEBUG_ATRFR2_PIN_TX_START;
#endif

    /* As the call takes approx. 55 us this is a short not to bright light */
#if RXTX_LED_ENABLE
    RXTX_LED_ON;
#endif

    retries++;
    DEBUG("TRX24_TX_START\n");
    ((at86rf2xx_t *)at86rfr2_dev)->irq_status1 |= AT86RF2XX_IRQ_STATUS_MASK1__TX_START;
    at86rfr2_dev->event_callback(at86rfr2_dev, NETDEV_EVENT_ISR);

#if DEBUG_ATRFR2_PINS
    DEBUG_ATRFR2_PORT &= ~DEBUG_ATRFR2_PIN_TX_START;
#endif

    /* As the call takes approx. 45 us this is a short not to bright light */
#if RXTX_LED_ENABLE
    RXTX_LED_OFF;
#endif
    __exit_isr();
}

#endif /* MODULE_AT86RFR2 */
