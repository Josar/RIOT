/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @ingroup cpu_atmega_common
 * @ingroup drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       Low-level ATmega RTT driver implementation
 *
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * In order to safely sleep when using the RTT:
 * 1. Disable interrupts
 * 2. Write to one of the asynch registers (e.g. TCCR2A)
 * 3. Wait for ASSR register's busy flags to clear
 * 4. Re-enable interrupts
 * 5. Sleep before interrupt re-enable takes effect
 *
 * @}
 */

#include <avr/interrupt.h>

#include "cpu.h"
#include "irq.h"
#include "periph/rtt.h"
#include "periph_conf.h"
#include "thread.h"

#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no RTT device is defined */
#if RTT_NUMOF

#ifdef __cplusplus
extern "C" {
#endif

#if MODULE_PERIPH_RTC
extern void atmega_rtc_incr(void);
#endif

static inline void __asynch_wait(void);

typedef struct {
    uint16_t ext_cnt;        /* Counter to make 8-bit timer 24-bit */
    uint16_t ext_comp;       /* Extend compare to 24-bits */
    rtt_cb_t alarm_cb;       /* callback called from RTT alarm */
    void *alarm_arg;         /* argument passed to the callback */
    rtt_cb_t overflow_cb;    /* callback called when RTT overflows */
    void *overflow_arg;      /* argument passed to the callback */
} rtt_state_t;

static volatile rtt_state_t rtt_state;

void rtt_init(void) {
    DEBUG("Initializing RTT\n");

    rtt_poweron();

    /*
     * From the datasheet section "Asynchronous Operation of Timer/Counter2"
     * p148 for ATmega1284P.
     * 1. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2.
     * 2. Select clock source by setting AS2 as appropriate.
     * 3. Write new values to TCNT2, OCR2x, and TCCR2x.
     * 4. To switch to asynchronous: Wait for TCN2UB, OCR2xUB, TCR2xUB.
     * 5. Clear the Timer/Counter2 Interrupt Flags.
     * 6. Enable interrupts, if needed
    */

    /* Disable all timer 2 interrupts */
    TIMSK2 = 0;

    /* Select asynchronous clock source */
    ASSR = (1 << AS2);

    /* Set counter to 0 */
    TCNT2 = 0;

    /* Reset compare values */
    OCR2A = 0;
    OCR2B = 0;

    /* Reset timer control */
    TCCR2A = 0;

    /* 32768Hz / 1024 = 32 ticks per second */
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 <<  CS20);

    /* Wait until not busy anymore */
    DEBUG("RTT waits until ASSR not busy\n");
    __asynch_wait();

    /* Clear interrupt flags */
    /* Oddly, this is done by writing ones; see datasheet */
    TIFR2 = (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);

    /* Enable 8-bit overflow interrupt */
    TIMSK2 |= (1 << TOIE2);

    DEBUG("RTT initialized\n");
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg) {
    /* Interrupt safe order of assignment */
    rtt_state.overflow_cb = NULL;
    rtt_state.overflow_arg = arg;
    rtt_state.overflow_cb = cb;
}

void rtt_clear_overflow_cb(void) {
    /* Interrupt safe order of assignment */
    rtt_state.overflow_cb = NULL;
    rtt_state.overflow_arg = NULL;
}

uint32_t rtt_get_counter(void) {
    /* Make sure it is safe to read TCNT2, in case we just woke up */
    DEBUG("RTT sleeps until safe to read TCNT2\n");
    TCCR2A = 0;
    __asynch_wait();

    return (((uint32_t)rtt_state.ext_cnt << 8) | (uint32_t)TCNT2);
}

void rtt_set_counter(uint32_t counter) {
    /* Wait until not busy anymore (should be immediate) */
    DEBUG("RTT sleeps until safe to write TCNT2\n");
    __asynch_wait();

    rtt_state.ext_cnt = (uint16_t)(counter >> 8);
    TCNT2 = (uint8_t)counter;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg) {
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2A);
    rtt_state.alarm_cb = NULL;

    /* Wait until not busy anymore (should be immediate) */
    DEBUG("RTT sleeps until safe to write OCR2A\n");
    __asynch_wait();

    /* Set the alarm value */
    rtt_state.ext_comp = (uint16_t)(alarm >> 8);
    OCR2A = (uint8_t)alarm;

    /* Interrupt safe order of assignment */
    rtt_state.alarm_arg = arg;
    rtt_state.alarm_cb = cb;

    /* Enable alarm interrupt only if it will trigger before overflow */
    if (rtt_state.ext_comp <= rtt_state.ext_cnt) {
        TIMSK2 |= (1 << OCIE2A);
    }
}

uint32_t rtt_get_alarm(void) {
    return (((uint32_t)rtt_state.ext_comp << 8) | (uint32_t)OCR2A);
}

void rtt_clear_alarm(void) {
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2A);

    /* Interrupt safe order of assignment */
    rtt_state.alarm_cb = NULL;
    rtt_state.alarm_arg = NULL;
}

void rtt_poweron(void) {
    power_timer2_enable();
}

void rtt_poweroff(void) {
    power_timer2_disable();
}

void __asynch_wait(void) {
    /* Wait until all busy flags clear. According to the datasheet,
     * this can take up to 2 positive edges of TOSC1 (32kHz). */
    while( ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB)
                 | (1 << TCR2AUB) | (1 << TCR2BUB)) );
}

ISR(TIMER2_OVF_vect) {
    __enter_isr();
    /* Enable RTT alarm if overflowed enough times */
    if (rtt_state.ext_comp == rtt_state.ext_cnt) {
        TIMSK2 |= (1 << OCIE2A);
    }

#if MODULE_PERIPH_RTC
    /* Increment RTC by 8 seconds */
    atmega_rtc_incr();
#endif

    /* Virtual 24-bit timer overflow */
    if (rtt_state.ext_cnt == 0xFFFF) {
        rtt_state.ext_cnt = 0;

        /* Execute callback */
        if (rtt_state.overflow_cb != NULL) {
            rtt_state.overflow_cb(rtt_state.overflow_arg);
        }
    } else {
        rtt_state.ext_cnt++;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}

ISR(TIMER2_COMPA_vect) {
    __enter_isr();
    /* Disable alarm until overflowed enough times */
    TIMSK2 &= ~(1 << OCIE2A);

    /* Execute callback */
    if (rtt_state.alarm_cb != NULL) {
        rtt_state.alarm_cb(rtt_state.alarm_arg);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
    __exit_isr();
}

#ifdef __cplusplus
}
#endif

#endif /* RTT_NUMOF */
