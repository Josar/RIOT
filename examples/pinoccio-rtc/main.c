#define ENABLE_DEBUG    (1)
#include "periph/pm.h"
#include "debug.h"
#include <stdio.h>

#include <avr/io.h>
#include <avr/sleep.h>

int main(void)
{
	TRXPR = 1 << SLPTR; // sent transceiver to sleep
	 set_sleep_mode(SLEEP_MODE_PWR_DOWN); // select power down mode
	sleep_mode();
	 // sleep_enable();
	 //sleep_cpu(); // go to deep sleep
}
