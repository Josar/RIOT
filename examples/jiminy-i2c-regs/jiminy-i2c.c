#define ENABLE_DEBUG    (0)

#include "board.h"
#include "debug.h"
#include "periph/i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int i2c_write_reg_own(int argc, char **argv)
{
	if (argc < 5) {
	        printf("usage: %s [i2c address] [register] [value] [num bytes]\n", argv[0]);
	        return 1;
	}
     
    uint8_t data[atoi(argv[4])];
    for(int i=0; i<atoi(argv[4]); i++){
        data[i] = atoi(argv[3]);
    }

    i2c_acquire(I2C_0);
    i2c_write_regs(I2C_0, atoi(argv[1]), atoi(argv[2]), data, atoi(argv[4]));
    i2c_release(I2C_0);
	return 0;
}

int i2c_read_reg_own(int argc, char **argv)
{
	if (argc < 4) {
	        printf("usage: %s [i2c address] [register] [num bytes]\n", argv[0]);
	        return 1;
	}
    
    uint8_t data[atoi(argv[3])];
    
    i2c_acquire(I2C_0);
    i2c_read_regs(I2C_0, atoi(argv[1]), atoi(argv[2]), data, atoi(argv[3]));
    i2c_release(I2C_0);
    printf("Got: ");
    for(uint8_t i=0; i<atoi(argv[3]); i++){
        printf("%x ", data[i]);
    }
	return 0;
}


int i2c_write_byte_own(int argc, char **argv)
{
	if (argc < 5) {
	        printf("usage: %s [i2c address] [data]\n", argv[0]);
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
	        printf("usage: %s [i2c address]\n", argv[0]);
	        return 1;
	}
    uint8_t data;
    i2c_acquire(I2C_0);
    i2c_read_byte(I2C_0, atoi(argv[1]), &data);
    i2c_release(I2C_0);
    printf("Got: %x\n", data);
	return 0;
}
