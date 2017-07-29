#include "periph/gpio.h"
#include "board.h"
#include "tps6274x.h"

typedef struct{
	gpio_t vsel1_pin;
	gpio_t vsel2_pin;
	gpio_t vsel3_pin;
	gpio_t vsel4_pin;
	gpio_t ctrl_pin;
}converter_config_t;

static const converter_config_t converter_config = {
		.vsel1_pin = GPIO_UNDEF,
		.vsel2_pin = 	GPIO_UNDEF,
		.vsel3_pin = GPIO_PIN(PORT_G, 0),
		.vsel4_pin = GPIO_PIN(PORT_G,2),
		.ctrl_pin = GPIO_PIN(PORT_G, 5)
};
