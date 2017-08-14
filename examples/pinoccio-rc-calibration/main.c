#include "rc-calibration.h"
#include <stdio.h>
#include <avr/io.h>

int main(void)
{
	printf("OSCCAL1: %x \n", OSCCAL);
	rc_calibration();
	printf("OSCCAL2: %x \n", OSCCAL);
	return 0;
}
