/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
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
 * @brief       power path test
 *
 *@author	Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>


#include "board.h"
#include "periph/i2c.h"

#include "periph_conf.h"
#include "bq24298.h"
#include "lc709203f.h"

#define F_CPU		CLOCK_CORECLOCK
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>


#include "shell.h"
#include "msg.h"


#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];


int bq_cmd(int argc, char **argv){
	if (argc < 2) {
	        printf("usage: %s [read|write]\n", argv[0]);
	        return 1;
	    }

	    if (strcmp(argv[1], "read") == 0) {

	        if (argc < 3) {
	            printf("usage: %s read <reg int>\n", argv[0]);
	            return 1;
	        }

	        uint8_t rec_buf=0;
	        int8_t control = i2c_read_reg(I2C_0, 0x6b, atoi(argv[2]) , &rec_buf);

	        if(control != 1){
	        	printf("Charger: Error reading\n");
	        	return 0;
	        }

	        printf("Register value: 0x%02x\n", rec_buf );

	    }
	    else if (strcmp(argv[1], "write") == 0) {

	        if ( (argc < 2) | (argc < 3) ) {
	            printf("usage: %s write <reg int> <setting int>\n", argv[0]);
	            return 1;
	        }

	        i2c_write_reg(I2C_0, 0x6b, atoi(argv[2]), atoi(argv[3]));

	        uint8_t rec_buf;
			int8_t control = i2c_read_reg(I2C_0, 0x6b, atoi(argv[2]) , &rec_buf);

			if(control != 1){
				printf("Charger: Error reading\n");
				return 0;
			}

			printf("Written and read back register value: 0x%02x\n", rec_buf );

	    }
	    else {
	        puts("error: invalid command");
	    }
	    return 0;
}

static const shell_command_t shell_commands[] = {
    { "charger", "Configure charger", bq_cmd },
    { NULL, NULL, NULL }
};


static void _charger_cb(uint8_t fault, uint8_t status, void* arg){
	printf("ALARM CHARGER \n");
	//LED_PORT |= RED;
}

static void _gauge_cb(void* arg)
{
	printf("\n ALARM GAUGE \n");
	//LED_PORT |= RED;
}


int main(void)
{
	/*while(1) {
	LED_PORT &= ~(RED|GREEN|BLUE);
	LED_PORT |= RED;
	_delay_ms(500);
	LED_PORT&= ~RED;
	LED_PORT |= GREEN;
	_delay_ms(500);
	LED_PORT &= ~GREEN;
	LED_PORT |= BLUE;
	_delay_ms(500);
	}*/
	LED_PORT |= (RED|GREEN|BLUE);
    puts("Jimminy Power Path test");
    gauge_init(I2C_0, GPIO_PIN(PORT_E, 6), _gauge_cb, NULL);
    charger_init(I2C_0, GPIO_PIN(PORT_B,4), _charger_cb, NULL);
    //charger_write_reg(I2C_0, REG_POWER_ON_CONFIG, reset_value[REG_POWER_ON_CONFIG]|REG1_I2C_WATCHDOG_TIMER_RESET);
    printf("Status register: 0x%x \n", charger_get_system_status(I2C_0));
    printf("new fault register: 0x%x \n", charger_get_new_fault(I2C_0));
    charger_status_t status = charger_get_status_struct();
    printf("power good %u \n", status.power_good);
    printf("Status_charge %u \n", status.status_charge);
    if(status.power_good && status.status_charge == CHARGE_STATUS_CHARGE_TERMINATION){
    	printf("Power good, charging stopped \n");
    }

    // read Charge Current Control Register REG02 an set bits
    // uint8_t rec_buf;
	// int8_t control = i2c_read_reg(I2C_0, 0x6b, 0x00 , &rec_buf);

	// set current limit to 500mA
    i2c_write_reg(I2C_0, 0x6b, 0x02 , 0x00 );

//    while(1){
//    	//printf("new fault register: 0x%x \n", charger_get_new_fault(I2C_0));
//    //	printf("RSOC %u \n", gauge_get_rsoc(I2C_0));
//    	//_delay_ms(1000);
//
//    }

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
    /* should be never reached */


    return 0;
}
