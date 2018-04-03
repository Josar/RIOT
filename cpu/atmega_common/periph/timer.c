/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the ATmega family
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 * @}
 */

#include <avr/interrupt.h>

#include "board.h"
#include "cpu.h"
#include "thread.h"

#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "periph/gpio.h"

/*
 * With 8MHz CPU Clock this gives a 0.25us Output Pulse
 * PORTF |= (1<<PORTF6);
 * PORTF &= ~(1<<PORTF6);
 *
 * Debuging pins can be used to determine how long a isr takes by probing
 */

#define DEBUG_TIMER_PIN (1)
#if DEBUG_TIMER_PIN
#define DEBUG_TIMER_PORT_DDR (DDRF)
#define DEBUG_TIMER_PORT     (PORTF)
#define DEBUG_TIMER_PIN1     (0x03)  /*(1<<PORTF2)*/
#endif

/**
 * @brief   All timers have three channels
 */
#define CHANNELS                (3)

/**
 * @brief   We have 5 possible prescaler values
 */
#define PRESCALE_NUMOF          (5U)

/**
 * @brief   Possible prescaler values, encoded as 2 ^ val
 */
static const uint8_t prescalers[] = { 0, 3, 6, 8, 10 };

/**
 * @brief   Timer state context
 */
typedef struct {
    mega_timer_t *dev;          /**< timer device */
    volatile uint8_t *mask;     /**< address of interrupt mask register */
    volatile uint8_t *flag;     /**< address of interrupt flag register */
    timer_cb_t cb;              /**< interrupt callback */
    void *arg;                  /**< interrupt callback argument */
    uint8_t mode;               /**< remember the configured mode */
    uint8_t isrs;               /**< remember the interrupt state */
} ctx_t;

/**
 * @brief   Allocate memory for saving the device states
 * @{
 */
#ifdef TIMER_NUMOF
static ctx_t ctx[] = {
#ifdef TIMER_0
    { TIMER_0, TIMER_0_MASK, TIMER_0_FLAG, NULL, NULL, 0, 0 },
#endif
#ifdef TIMER_1
    { TIMER_1, TIMER_1_MASK, TIMER_1_FLAG, NULL, NULL, 0, 0 },
#endif
#ifdef TIMER_2
    { TIMER_2, TIMER_2_MASK, TIMER_2_FLAG, NULL, NULL, 0, 0 },
#endif
#ifdef TIMER_3
    { TIMER_3, TIMER_3_MASK, TIMER_3_FLAG, NULL, NULL, 0, 0 },
#endif
};
#else
/* fallback if no timer is configured */
static ctx_t *ctx[] = {{ NULL }};
#endif
/** @} */

/**
 * @brief Setup the given timer
 */
int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    DEBUG("timer.c: freq = %ld\n", freq);
    uint8_t pre = 0;

    /* make sure given device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* figure out if freq is applicable */
    for (; pre < PRESCALE_NUMOF; pre++) {
        if ((CLOCK_CORECLOCK >> prescalers[pre]) == freq) {
            break;
        }
    }
    if (pre == PRESCALE_NUMOF) {
        DEBUG("timer.c: prescaling failed!\n");
        return -1;
    }

    /* stop and reset timer */
    ctx[tim].dev->CRA = 0;
    ctx[tim].dev->CRB = 0;
    ctx[tim].dev->CRC = 0;
    ctx[tim].dev->CNT = 0;

    /* save interrupt context and timer mode */
    ctx[tim].cb   = cb;
    ctx[tim].arg  = arg;
    ctx[tim].mode = (pre + 1);

    /* enable timer with calculated prescaler */
    ctx[tim].dev->CRB = (pre + 1);
    DEBUG("timer.c: prescaler set at %d\n", pre + 1);

    /* Timer is now configured as
     * TCCR1A
     * Compare Output Mode: Normal port operation, OCnA/OCnB/OCnC disconnected.
     * Waveform Generation Mode: Normal mode of operation
     *
     * TCCR1B
     * Input Capture 1 Noise Canceler: off
     * Waveform Generation Mode : Normal mode of operation
     * Clock Select: see CRB setting above
     *
     * TCCR1C
     * Force Output Compare for Channel A/B/C: off
     */

#ifdef DEBUG_TIMER_PIN
    /* Port Pin as Output */
    DEBUG_TIMER_PORT_DDR |= DEBUG_TIMER_PIN1;
    /* Pin Low */
    DEBUG_TIMER_PORT &= ~DEBUG_TIMER_PIN1;
#endif

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (channel >= CHANNELS) {
        return -1;
    }

    ctx[tim].dev->OCR[channel] = (uint16_t)value;
    *ctx[tim].flag &= ~(1 << (channel + OCF1A));
    *ctx[tim].mask |=  (1 << (channel + OCIE1A));

    return 1;
}

int timer_clear(tim_t tim, int channel)
{
    if (channel >= CHANNELS) {
        return -1;
    }

    *ctx[tim].mask &= ~(1 << (channel + OCIE1A));

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    return (unsigned int)ctx[tim].dev->CNT;
}

void timer_stop(tim_t tim)
{
    ctx[tim].dev->CRB = 0;
}

void timer_start(tim_t tim)
{
    ctx[tim].dev->CRB = ctx[tim].mode;
}

#ifdef TIMER_NUMOF
static inline void _isr(tim_t tim, int chan)
{
    __enter_isr();

    *ctx[tim].mask &= ~(1 << (chan + OCIE1A));
    ctx[tim].cb(ctx[tim].arg, chan);

    if (sched_context_switch_request) {
        thread_yield();
    }

    __exit_isr();
}
#endif

#ifdef TIMER_0
ISR(TIMER_0_ISRA, ISR_BLOCK)
{
    /* overflow 60 us in callback */
    /* xtimer 237 us in callback */
#ifdef DEBUG_TIMER_PIN
    DEBUG_TIMER_PORT |= DEBUG_TIMER_PIN1;
#endif

    _isr(0, 0);

#ifdef DEBUG_TIMER_PIN
    DEBUG_TIMER_PORT &= ~DEBUG_TIMER_PIN1;
#endif
}

ISR(TIMER_0_ISRB, ISR_BLOCK)
{
    _isr(0, 1);
}

#ifdef TIMER_0_ISRC
ISR(TIMER_0_ISRC, ISR_BLOCK)
{
    _isr(0, 2);
}
#endif /* TIMER_0_ISRC */
#ifdef TIMER_0_OVF
ISR(TIMER_0_OVF, ISR_BLOCK)
{
    /* The whole overflow process takes 23.45 us to complete */
    /* With 8MHz CPU Clock this is 0.25us puls high */
    //PORTF |= (1<<PORTF7);
    // PORTF &= ~(1<<PORTF7);
    __enter_isr();

    if(ovr_ctx[0].cb != NULL){
        /* Execute callback */
        ovr_ctx[0].cb(ctx[0].arg, 0);

        if (sched_context_switch_request) {
            thread_yield();
        }
    }
    __exit_isr();
    /* With 8MHz CPU Clock this is 0.25us puls high */
    // PORTF |= (1<<PORTF7);
    //PORTF &= ~(1<<PORTF7);
}
#endif /* TIMER_0_ISRC */
#endif /* TIMER_0 */

#ifdef TIMER_1
ISR(TIMER_1_ISRA, ISR_BLOCK)
{
    _isr(1, 0);
}

ISR(TIMER_1_ISRB, ISR_BLOCK)
{
    _isr(1, 1);
}

#ifdef TIMER_1_ISRC
ISR(TIMER_1_ISRC, ISR_BLOCK)
{
    _isr(1, 2);
}
#endif /* TIMER_1_ISRC */
#endif /* TIMER_1 */

#ifdef TIMER_2
ISR(TIMER_2_ISRA, ISR_BLOCK)
{
    _isr(2, 0);
}

ISR(TIMER_2_ISRB, ISR_BLOCK)
{
    _isr(2, 1);
}

ISR(TIMER_2_ISRC, ISR_BLOCK)
{
    _isr(2, 2);
}
#endif /* TIMER_2 */

#ifdef TIMER_3
ISR(TIMER_3_ISRA, ISR_BLOCK)
{
    _isr(2, 0);
}

ISR(TIMER_3_ISRB, ISR_BLOCK)
{
    _isr(2, 1);
}

ISR(TIMER_3_ISRC, ISR_BLOCK)
{
    _isr(2, 2);
}
#endif /* TIMER_3 */
