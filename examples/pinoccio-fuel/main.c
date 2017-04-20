#include "max17048.h"
#include "periph_conf.h"
#include <stdio.h>

int main (void)
{
	gauge_init(I2C_0);
	//gauge_reboot();
	gauge_soc();
	printf("Voltage start:");
	gauge_voltage();
	gauge_charge_rate();
	return 0;
}
