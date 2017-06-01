#include "max17048.h"
#include "periph_conf.h"
#include <stdio.h>


int main (void)
{
	gauge_init(I2C_0);
	//gauge_reset();
	//gauge_voltage();
	//gauge_soc();
	//gauge_id();
	//gauge_discharge_rate();
	//gauge_version();
	//gauge_reset();
	gauge_sleep(ON);
	//gauge_soc();
	//gauge_version();
	gauge_soc_alert_change(ON);
	gauge_soc();
	return 0;
}
