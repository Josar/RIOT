#include "max17048.h"
#include "periph_conf.h"

int main (void)
{
	gauge_init(I2C_0);
	gauge_read_id();
	return 0;
}
