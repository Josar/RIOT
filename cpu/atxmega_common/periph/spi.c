/*
 * Copyright (C) 2015 Daniel Amkaer Sorensen
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation for ATmega family
 *
 * @author      Daniel Amkaer Sorensen <daniel.amkaer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"

#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

/**
 * @brief   Extract BR0, BR1 and SPI2X bits from speed value
 * @{
 */
#define CLK_MASK            (0x3)
#define S2X_SHIFT           (2)
/** @} */

static mutex_t lock = MUTEX_INIT;

void led_blink(void){

    PORTE.OUTSET = PIN6_bm ;
    _delay_ms(100);
    PORTE.OUTCLR = PIN6_bm ;
}


void spi_init(spi_t bus)
{

    led_blink();

    assert(bus == 0);
   /* power off the SPI peripheral */
   PR.PRPD = PR_SPI_bm;
   /* trigger the pin configuration */
   spi_init_pins(bus);
}


void spi_init_pins(spi_t bus)
{

    led_blink();

    (void)bus;
    /* the pin configuration for this CPU is for now :
     * - PD4: SS   (configure as output, but unused)
     * - PD5: MOSI (configure as output)
     * - PD6: MISO (configure as input - done automatically)
     * - PD7: SCK  (configure as output)
     *
     * The SS pin must be configured as output for the SPI device to work as
     * master correctly, though we do not use it for now (as we handle the chip
     * select externally for now)
     */

    PORTD.DIRSET = PIN4;
    PORTD.OUTSET = PIN4;
    PORTD.DIRCLR = PIN5;
    PORTD.DIRSET = PIN6;
    PORTD.DIRSET = PIN7;
}

int spi_init_cs(spi_t bus, spi_cs_t cs){

    led_blink();

    PORTD.OUTSET = PIN4;

    return 0;
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;

    (void) clk;

    led_blink();

    /* lock the bus and power on the SPI peripheral */
    mutex_lock(&lock);
    PR.PRPD &= ~PR_SPI_bm;

    /* configure active, as master, with given mode and clock */
    SPID.CTRL = SPI_MASTER_bm |mode|SPI_PRESCALER_DIV16_gc;

    /* clear interrupt flag by reading STATUS register by reading DATA */
    (void)SPID.STATUS;
    (void)SPID.DATA;

    /* Enable Module*/
    SPID.CTRL =  SPI_ENABLE_bm;

    return SPI_OK;
	return 0;
}

void spi_release(spi_t bus)
{

    led_blink();

    /* Disable Module*/
    SPID.CTRL &= ~ SPI_ENABLE_bm;

    /* power off */
    PR.PRPD = PR_SPI_bm;

    mutex_unlock(&lock);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    uint8_t *out_buf = (uint8_t *)out;
    uint8_t *in_buf = (uint8_t *)in;

    assert(out_buf || in_buf);

    led_blink();

    if (cs != SPI_CS_UNDEF) {
        // gpio_clear((gpio_t)cs);
        PORTD.OUTCLR = PIN4;
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;

        SPID.DATA = tmp;

        while(!(SPID.STATUS & SPI_IF_bm )){}

        tmp = SPID.DATA ;

        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        //gpio_set((gpio_t)cs);
        PORTD.OUTSET = PIN4;
    }
}

uint8_t spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont, uint8_t out)
{

    led_blink();

        if (cs != SPI_CS_UNDEF) {
           // gpio_clear((gpio_t)cs);
           PORTD.OUTCLR = PIN4;
       }

       SPID.DATA = out;

       while(!(SPID.STATUS & SPI_IF_bm )){}


       if ((!cont) && (cs != SPI_CS_UNDEF)) {
           //gpio_set((gpio_t)cs);
           PORTD.OUTSET = PIN4;
       }

       return SPID.DATA ;
}

uint8_t spi_transfer_reg(spi_t bus, spi_cs_t cs, uint8_t reg, uint8_t out)
{

    assert(reg);

    led_blink();

    if (cs != SPI_CS_UNDEF) {
        // gpio_clear((gpio_t)cs);
        PORTD.OUTCLR = PIN4;
    }

    SPID.DATA = reg;

    while(!(SPID.STATUS & SPI_IF_bm )){}

    SPID.DATA = (out) ? out: 0;

    while(!(SPID.STATUS & SPI_IF_bm )){}

    if (cs != SPI_CS_UNDEF) {
        //gpio_set((gpio_t)cs);
        PORTD.OUTSET = PIN4;
    }

    return SPID.DATA ;
}

void spi_transfer_regs(spi_t bus, spi_cs_t cs, uint8_t reg,
                       const void *out, void *in, size_t len)
{
    uint8_t *out_buf = (uint8_t *)out;
        uint8_t *in_buf = (uint8_t *)in;

        assert(out_buf || in_buf);

        led_blink();

        if (cs != SPI_CS_UNDEF) {
            // gpio_clear((gpio_t)cs);
            PORTD.OUTCLR = PIN4;
        }

        SPID.DATA = reg;

        while(!(SPID.STATUS & SPI_IF_bm )){}

        for (size_t i = 0; i < len; i++) {
            uint8_t tmp = (out_buf) ? out_buf[i] : 0;

            SPID.DATA = tmp;

            while(!(SPID.STATUS & SPI_IF_bm )){}

            tmp = SPID.DATA ;

            if (in_buf) {
                in_buf[i] = tmp;
            }
        }

        if (cs != SPI_CS_UNDEF) {
            //gpio_set((gpio_t)cs);
            PORTD.OUTSET = PIN4;
        }
}

