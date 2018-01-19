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

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
    /* should be never reached */


    return 0;
}
