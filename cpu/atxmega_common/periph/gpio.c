/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA

 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for ATmega family
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 *
 * @}
 */


#include <stdio.h>

#include <avr/interrupt.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define GPIO_BASE_PORT_A        (0x0600)

/*
 * @brief     Define GPIO interruptions for an specific atmega CPU, by default
 *            2 (for small atmega CPUs)
 */
#if defined(INT2_vect)
#define GPIO_EXT_INT_NUMOF      (3U)
#elif defined(INT3_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT4_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT5_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT6_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT7_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#else
#define GPIO_EXT_INT_NUMOF      (2U)
#endif

static gpio_isr_ctx_t config[GPIO_EXT_INT_NUMOF];

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
//    switch (mode) {
//        case GPIO_OUT:
//            _SFR_MEM8(_ddr_addr(pin)) |= (1 << _pin_num(pin));
//            break;
//        case GPIO_IN:
//            _SFR_MEM8(_ddr_addr(pin)) &= ~(1 << _pin_num(pin));
//            _SFR_MEM8(_port_addr(pin)) &= ~(1 << _pin_num(pin));
//            break;
//        case GPIO_IN_PU:
//            _SFR_MEM8(_port_addr(pin)) |= (1 << _pin_num(pin));
//            break;
//        default:
//            return -1;
//    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    uint8_t pin_num = 1;


    /* set callback */
    config[pin_num].cb = cb;
    config[pin_num].arg = arg;

    /* set global interrupt flag */
//    sei();

    return 0;
}


void gpio_irq_enable(gpio_t pin)
{
    /* RF_IRQ    - PD2 */

    // input
    PORTD.DIRCLR = PIN2;
    // interrupt
    PORTD.INT0MASK = PIN2;
    PORTD.PIN2CTRL = PORT_ISC_RISING_gc;
    // int level
    PORTD.INTCTRL = PORT_INT0LVL_HI_gc;

    PMIC.CTRL|=PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm; //Intrrupts

//    sei();
}

void gpio_irq_disable(gpio_t pin)
{

}


void gpio_set(gpio_t pin)
{

}

void gpio_clear(gpio_t pin)
{

}


static inline void irq_handler(uint8_t pin_num)
{
    __enter_isr();
    config[pin_num].cb(config[pin_num].arg);
    __exit_isr();
}

ISR(PORTD_INT0_vect, ISR_BLOCK)
{
    irq_handler(0); /**< predefined interrupt pin */
}

ISR(PORTD_INT1_vect, ISR_BLOCK)
{
    irq_handler(1); /**< predefined interrupt pin */
}

#if defined(INT2_vect)
ISR(INT2_vect, ISR_BLOCK)
{
    irq_handler(2); /**< predefined interrupt pin */
}
#endif

#if defined(INT3_vect)
ISR(INT3_vect, ISR_BLOCK)
{
    irq_handler(3); /**< predefined interrupt pin */
}
#endif

#if defined(INT4_vect)
ISR(INT4_vect, ISR_BLOCK)
{
    irq_handler(4); /**< predefined interrupt pin */
}
#endif

#if defined(INT5_vect)
ISR(INT5_vect, ISR_BLOCK)
{
    irq_handler(5); /**< predefined interrupt pin */
}
#endif

#if defined(INT6_vect)
ISR(INT6_vect, ISR_BLOCK)
{
    irq_handler(6); /**< predefined interrupt pin */
}
#endif

#if defined(INT7_vect)
ISR(INT7_vect, ISR_BLOCK)
{
    irq_handler(7); ///< predefined interrupt pin
}
#endif

