/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       xtimer_usleep test application
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "xtimer.h"
#include "timex.h"

#define RUNS                (5U)
#define SLEEP_TIMES_NUMOF   (sizeof(sleep_times) / sizeof(sleep_times[0]))

static const uint32_t sleep_times[] = { 100, 150, 1000, 1500, 10000, 15000, 50000, 100000, };

int main(void)
{
    printf("Running test %u times with %u distinct sleep times\n", RUNS,
           (unsigned)SLEEP_TIMES_NUMOF);

    for (unsigned m = 0; m < RUNS; m++) {
        for (unsigned n = 0;
             n < sizeof(sleep_times) / sizeof(sleep_times[0]);
             n++) {
            xtimer_ticks32_t diff, start, sleep_time_ticks;
            start = xtimer_now();
            xtimer_usleep(sleep_times[n]);
            diff = xtimer_diff(xtimer_now(), start);
            sleep_time_ticks = xtimer_ticks_from_usec(sleep_times[n]);

            printf("Slept for %" PRIu32 " ticks, expected %" PRIu32 " ticks, XTIMER_OVERHEAD = %" PRIu32 "\n",
                   diff.ticks32, sleep_time_ticks.ticks32, diff.ticks32-sleep_time_ticks.ticks32);
        }
    }

    return 0;
}
