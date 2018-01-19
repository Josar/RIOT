#define ENABLE_DEBUG    (0)

#include "board.h"
#include "debug.h"
#include "periph/i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bq24298.h"

int i2c_write_reg_own(int argc, char **argv)
{
	if (argc < 5) {
	        printf("usage: %s [i2c address int] [register int] [value int] [num bytes]\n", argv[0]);
	        return 1;
	}
     
    uint8_t data[atoi(argv[4])];
    for(int i=0; i<atoi(argv[4]); i++){
        data[i] = atoi(argv[3]);
    }

    i2c_acquire(I2C_0);
    i2c_write_regs(I2C_0, atoi(argv[1]), atoi(argv[2]), data, atoi(argv[4]));
    i2c_release(I2C_0);

    /* save input source control  register */
    if( (atoi(argv[1])== 0x6b) & (atoi(argv[2])==0x00) ){
    	printf("save TPS6274x_input_source_control register: %u\n", data[0]);
    	eeprom_write_byte( &TPS6274x_input_source_control, data[0] );
    }else /*save charge current control register */
    	if( (atoi(argv[1])== 0x6b) & (atoi(argv[2])==0x02) ){
    		printf("save TPS6274x_charge_current_control register: %u\n", data[0]);
    	eeprom_write_byte( &TPS6274x_charge_current_control, data[0] );
    }

	return 0;
}

int i2c_read_reg_own(int argc, char **argv)
{
	if (argc < 4) {
	        printf("usage: %s [i2c address int] [register int] [num bytes int]\n", argv[0]);
	        return 1;
	}
    
    uint8_t data[atoi(argv[3])];
    
    i2c_acquire(I2C_0);
    i2c_read_regs(I2C_0, atoi(argv[1]), atoi(argv[2]), data, atoi(argv[3]));
    i2c_release(I2C_0);
    printf("Got: ");
    for(uint8_t i=0; i<atoi(argv[3]); i++){
        printf("%u ", data[i]);
    }
    printf(" [int]\n");
	return 0;
}


int i2c_write_byte_own(int argc, char **argv)
{
	if (argc < 5) {
	        printf("usage: %s [i2c address int] [data int]\n", argv[0]);
	        return 1;
	}
    i2c_acquire(I2C_0);
    i2c_write_byte(I2C_0, atoi(argv[1]), atoi(argv[2]));
    i2c_release(I2C_0);
	return 0;
}

int i2c_read_byte_own(int argc, char **argv)
{
	if (argc < 5) {
	        printf("usage: %s [i2c address int]\n", argv[0]);
	        return 1;
	}
    uint8_t data;
    i2c_acquire(I2C_0);
    i2c_read_byte(I2C_0, atoi(argv[1]), &data);
    i2c_release(I2C_0);
    printf("Got: %x [int]\n", data);
	return 0;
}
