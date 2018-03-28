/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_rtc
 *
 * @{
 *
 * @file
 * @brief       RTC interface wrapper for use with RTT modules.
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <avr/interrupt.h>
#include <time.h>

#include "cpu.h"
#include "periph/rtc.h"
#include "periph/rtt.h"
#include "periph_conf.h"
#include "thread.h"

#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard file in case no RTC device is defined */
#if RTC_NUMOF

#ifdef __cplusplus
extern "C" {
#endif

static inline void __asynch_wait(uint8_t num_cycles);

typedef struct {
    time_t time;          /* seconds since the epoch */
    time_t alarm;         /* alarm_cb when time == alarm */
    rtt_cb_t alarm_cb;    /* callback called from RTC alarm */
    void *alarm_arg;      /* argument passed to the callback */
} rtc_state_t;

static volatile rtc_state_t rtc_state;

void rtc_init(void)
{
    /* Initialize rtc_state*/
    rtc_state.time = 0;
    rtc_state.alarm = 0;
    rtc_state.alarm_cb = NULL;
    rtc_state.alarm_arg = NULL;

    /* RTC depends on RTT */
    rtt_init();
}

int rtc_set_time(struct tm *time)
{
    /* Convert to seconds since the epoch */
    rtc_state.time = mktime(time);

    return 0;
}

int rtc_get_time(struct tm *time)
{
    /* Convert from seconds since the epoch */
    /* Note: Cast tells the compiler to discard volatile */
    gmtime_r((time_t*)&rtc_state.time, time);

    return 0;
}

int rtc_set_alarm(struct tm *time, rtc_alarm_cb_t cb, void *arg)
{
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2B);

    /* Set callback */
    rtc_state.alarm_cb = cb;
    rtc_state.alarm_arg = arg;

    /* Wait until not busy anymore (should be immediate) */
    DEBUG("RTC sleeps until safe to write OCR2B\n");
    __asynch_wait(1);

    /* Set alarm time */
    rtc_state.alarm = mktime(time);

    /* Prepare the counter for sub 8-second precision */
    OCR2B = ((uint8_t)rtc_state.alarm & 0x07) << 5;

    /* Wait until alarm value takes effect */
    DEBUG("RTC sleeps until safe power-save\n");
    __asynch_wait(2);

    /* Enable alarm only if it will trigger in < 8 seconds */
    if ((rtc_state.time & 0xFFFFFFF8) == (rtc_state.alarm & 0xFFFFFFF8)) {
        TIMSK2 |= (1 << OCIE2B);
    }

    return 0;
}

int rtc_get_alarm(struct tm *time)
{
    /* Convert from seconds since the epoch */
    /* Note: Cast tells the compiler to discard volatile */
    gmtime_r((time_t*)&rtc_state.alarm, time);

    return 0;
}

void rtc_clear_alarm(void)
{
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2B);

    rtc_state.alarm_cb = NULL;
    rtc_state.alarm_arg = NULL;
}

void rtc_poweron(void)
{
    rtt_poweron();
}

void rtc_poweroff(void)
{
    rtt_poweroff();
}

void atmega_rtc_incr(void)
{
    rtc_state.time += 8;

    /* Enable alarm only if it will trigger in < 8 seconds */
    if ((rtc_state.time & 0xFFFFFFF8) == (rtc_state.alarm & 0xFFFFFFF8)) {
        TIMSK2 |= (1 << OCIE2B);
    }
}

void __asynch_wait(uint8_t num_cycles) {
    /* Wait until all busy flags clear */
    while( ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB)
                 | (1 << TCR2AUB) | (1 << TCR2BUB)) ) {
#ifdef MODULE_XTIMER
        /* Sleep for num_cycles RTC cycles */
        xtimer_usleep(num_cycles * 35 * US_PER_MS);
#else
        thread_yield();
#endif
    }
}

ISR(TIMER2_COMPB_vect) {
    __enter_isr();
    /* Disable alarm interrupt */
    TIMSK2 &= ~(1 << OCIE2B);

    if (rtc_state.alarm_cb != NULL) {
        rtc_state.alarm_cb(rtc_state.alarm_arg);
    }

    /* Wait until it is safe to re-enter power-save */
    TCCR2A = TCCR2A;
    while( ASSR & (1 << TCR2AUB) ) {
        if (sched_context_switch_request) {
            thread_yield();
        }
    }
    __exit_isr();
}

#ifdef __cplusplus
}
#endif

#endif /* RTC_NUMOF */
