/*
 * Copyright (C) 2016 RWTH Aachen, Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include "shell.h"
#include "tps6274x.h"
#include "periph/i2c.h"

#include "bq24298.h"

#include <avr/eeprom.h>
/* EEPROM values */
/* make sure to set proper values faulty values make the brick the board*/
__attribute__((section(".eeprom"))) uint8_t TPS6274x_input_source_control = 55;
__attribute__((section(".eeprom"))) uint8_t TPS6274x_charge_current_control = 96;

extern int i2c_write_reg_own(int argc, char **argv);
extern int i2c_read_reg_own(int argc, char **argv);
extern int i2c_write_byte_own(int argc, char **argv);
extern int i2c_read_byte_own(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "i2c_write_reg", "write value to i2c register", i2c_write_reg_own },
    { "i2c_read_reg", "read values from i2c register", i2c_read_reg_own },
    { "i2c_write_byte", "write value to i2c address", i2c_write_byte_own },
    { "i2c_read_byte", "read value from i2c address", i2c_read_byte_own},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Jiminy I2C Shell!");
    stepdown_init(3000);
    i2c_init_master(I2C_0, I2C_SPEED_FAST);

    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* set values to charger IC */
    uint8_t sourceControl = eeprom_read_byte( &TPS6274x_input_source_control );
    uint8_t currentControl = eeprom_read_byte( &TPS6274x_charge_current_control );

    printf("TPS6274x_input_source_control register: %u\n", sourceControl);
    printf("TPS6274x_charge_current_control register: %u\n", currentControl);

    /* avoid programming when no values where configured.*/
    if ( (sourceControl!=0xFF) && (currentControl!=0xFF) ){
    i2c_acquire(I2C_0);
    i2c_write_regs(I2C_0, 0x6b, 0x00 , &sourceControl,  1);
    i2c_write_regs(I2C_0, 0x6b, 0x02 , &currentControl, 1);
    i2c_release(I2C_0);
    /* END set values to charger IC */
    }

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
    /* should be never reached */




    return 0;
}
