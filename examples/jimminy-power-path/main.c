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
#include "tps6274x.h"

#define F_CPU		CLOCK_CORECLOCK
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>


#include "shell.h"

#include "msg.h"
#include "thread.h"
#include "xtimer.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];


int bq_cmd(int argc, char **argv){
	if (argc < 2) {
	        printf("usage: %s [read|write]\n", argv[0]);
	        return 1;
	    }

	    if (strcmp(argv[1], "read") == 0) {

	        if (argc < 2) {
	            printf("usage: %s read <reg integer decimal>\n", argv[0]);
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

	        if ( (argc < 3) ) {
	            printf("usage: %s write <reg integer decimal> <setting integer decimal>\n", argv[0]);
	            return 1;
	        }

	        charger_write_reg(I2C_0, atoi(argv[2]), atoi(argv[3]));

	        uint8_t rec_buf;
			int8_t control = i2c_read_reg(I2C_0, 0x6b, atoi(argv[2]) , &rec_buf);

			if(control != 1){
				printf("Charger: Error reading\n");
				return 0;
			}

			printf("Written and read back register value: 0x%02x\n", rec_buf );

	    }else if (strcmp(argv[1], "charge") == 0) {
	    	charger_write_reg(I2C_0,01, 0x1B);
	    }
	    else {
	        puts("error: invalid command");
	    }
	    return 0;
}

int supply_cmd(int argc, char **argv){
	if (argc < 2) {
	        printf("usage: %s [volt|load]\n", argv[0]);
	        return 1;
	    }

	    if (strcmp(argv[1], "volt") == 0) {

	        if (argc < 2) {
	            printf("usage: %s volt <value in mV 1800 - 3300>\n", argv[0]);
	            return 1;
	        }

	        uint8_t err = stepdown_switch_voltage( atoi(argv[2]) );

	        if(err != 0){
	        	printf("Supply voltage change Error, to small or big value.\n");
	        	return 0;
	        }

	        printf("Supply voltage set.\n");

	    }
	    else if (strcmp(argv[1], "load") == 0) {

	        if ( (argc < 2) | (argc < 3) ) {
	            printf("usage: %s load <on|off>\n", argv[0]);
	            return 1;
	        }

	        if( strcmp(argv[2], "on") == 0 )
	        {
	        	stepdown_enable_load();
	        }
	        else if( strcmp(argv[2], "off") == 0 )
	        {
	        	stepdown_disable_load();
	        }
	        else
	        {
	        	printf("usage: %s load <on|off>\n", argv[0]);
	        	return 1;
	        }

			printf("Load supply switched %s.\n", argv[2] );
	    }
	    else {
	        puts("error: invalid command");
	    }
	    return 0;
}

/* set interval to 1 second */
#define INTERVAL (30 * US_PER_SEC)

void *charger_status_thread(void *arg)
{
    (void) arg;
    xtimer_ticks32_t last_wakeup = xtimer_now();

    printf("\nStarted charger status thread PID: %" PRIkernel_pid "\n", thread_getpid());


    while (1) {
    xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

    printf("Charger_fault: 0x%02x  status: 0x%02x \n",
    			charger_get_new_fault(I2C_0),
				charger_get_system_status(I2C_0));
	printf("\n");

	printf("Voltage in mV: %u \n", gauge_get_voltage(I2C_0));
	printf("RSOC: %u \n", gauge_get_rsoc(I2C_0));
	printf("Indicator to empty ITE in 0.1: %u \n", gauge_get_ite(I2C_0));
	printf("\n");

    }

    return NULL;
}

char charger_status_thread_stack[256];

static const shell_command_t shell_commands[] = {
    { "charger", "Configure charger", bq_cmd },
	{ "supply", "Configure supply", supply_cmd },
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

	LED_PORT &= ~(RED|GREEN|BLUE);

	puts("Jimminy Supply\n");
	stepdown_init(3000);


    puts("\nJimminy Fuel Gauge\n");
    gauge_init(I2C_0, GPIO_PIN(PORT_E, 6), _gauge_cb, NULL);
    gauge_set_alarm_low_rsoc(I2C_0, 0);
    gauge_set_alarm_low_rsoc(I2C_0, 70);
    printf("Alarm Low RSOC: %u \n", gauge_get_alarm_low_rsoc(I2C_0));

    puts("\nJimminy Power Path");
    charger_init(I2C_0, GPIO_PIN(PORT_B,4), _charger_cb, NULL);
	// disabled watchdog [reset = 11011100, or 0xDC]
    charger_write_reg(I2C_0, 0x05 , 0xCC );

    //charger_write_reg(I2C_0, REG_POWER_ON_CONFIG, reset_value[REG_POWER_ON_CONFIG]|REG1_I2C_WATCHDOG_TIMER_RESET);
    printf("Status register: 0x%02x \n", charger_get_system_status(I2C_0));
    printf("Status register: 0x%02x \n", charger_get_system_status(I2C_0));

    printf("new fault register: 0x%02x \n", charger_get_new_fault(I2C_0));
    charger_status_t status = charger_get_status_struct();
    printf("power good %u \n", status.power_good);
    printf("Status_charge %u \n", status.status_charge);
    if(status.power_good && status.status_charge == CHARGE_STATUS_CHARGE_TERMINATION){
    	printf("Power good, charging stopped \n");
    }

	//Power-On Configuration
    // reset watchdog
    // disabled charging
    charger_write_reg(I2C_0, 0x01 , 0x0B );

	// set current limit to 500mA
    charger_write_reg(I2C_0, 0x02 , 0x00 );




    // thread to read out charger status
    thread_create(charger_status_thread_stack, sizeof(charger_status_thread_stack),
                                THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
								charger_status_thread, NULL, "Charger Status");

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("\nRIOT network stack example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
    /* should be never reached */


    return 0;
}
