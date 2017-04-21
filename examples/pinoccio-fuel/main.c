#define F_CPU 16000000
#include "max17048.h"
#include "periph_conf.h"
#include <stdio.h>
#include <util/delay.h>

int main (void)
{
	gauge_init(I2C_0);
	//gauge_reboot();
	//gauge_read_id();
	//gauge_voltage();
	gauge_soc();
	gauge_read_id();
	//printf("Voltage start:");
	//gauge_discharge_rate();
	return 0;
}
