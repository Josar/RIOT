#ifndef MAX17048_INTERRUPT_H_
#define MAX17048_INTERRUPT_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include "max17048_def.h"
#include "periph/i2c.h"
#include "periph_conf.h"


void activate_alert_interrupt(void);

void disable_alert_interrupt(void);


#ifdef __cplusplus
}
#endif

#endif /*MAX17048_INTERRUPT_H_ */
