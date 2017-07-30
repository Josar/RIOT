#include "periph/i2c.h"
#include "periph_conf.h"
#define ENABLE_DEBUG 	(1U)
#include "debug.h"
#include <inttypes.h>
#include <avr/io.h>
#include <util/twi.h>

#define ENABLE_DEBUG 	(1U)


uint8_t _twbr_values(int speed)
{
	return ((16*speed-CLOCK_CORECLOCK)/(-2*speed));
}

uint32_t speeds[5] = {10000, 100000, 400000, 1000000, 3400000};

uint8_t __send_start(i2c_t dev)
{

	TWCR = *(i2c_config[dev].mask)|(1<<TWSTA);
	while (!(TWCR & (1<<TWINT)));
	return ((TWSR & 0xF8));
}

void __send_stop(i2c_t dev)
{
	TWCR = *(i2c_config[dev].mask)|(1<<TWSTO);
	DEBUG("Stop Send \n");
}
/**
 * Sends Adress+R and reads length bytes and stores in data
 */

int __read_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
	uint8_t *my_data = (uint8_t*) data;
	TWDR = (address<<1)|TW_READ;
	TWCR = *(i2c_config[dev].mask);
	while (!(TWCR & (1<<TWINT))); //ACK/NACK should be received now
	if ((TWSR & 0xF8) != TW_MR_SLA_ACK) {
		DEBUG("I2C: Slave didnt ACK Adress-read %x\n",(TWSR & 0xF8));
		i2c_release(dev);
		return 0;
	}
	for(int i=0; i<length; i++){
		if(i+1 == length) {
			//ready to receive last byte, for the max17048 this needs to be Nacked instead of Acked
			TWCR = *(i2c_config[dev].mask);
		}else {
			TWCR = *(i2c_config[dev].mask)|(1<<TWEA);
		}
		while (!(TWCR & (1<<TWINT)));
		if ((TWSR & 0xF8) != TW_MR_DATA_ACK && (i+1) != length){
			DEBUG("I2C: Error receiving Data from Slave Errorcode: %x on Byte %u", (TWSR & 0xF8), i);
			i2c_release(dev);
			return (i+1);
		}else if((TWSR & 0xF8) != TW_MR_DATA_NACK && (i+1) == length) {
			DEBUG("I2C: Error receiving Data from Slave, could not nack Errorcode: %x on Byte %u", (TWSR & 0xF8), i);
			i2c_release(dev);
			return (i+1);
		}
		my_data[i] = TWDR;
	}
	return length;
}

int __write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
		uint8_t *my_data = (uint8_t*) data;
        TWDR = (address<<1) | TW_WRITE;
        TWCR = *(i2c_config[dev].mask);
        while (!(TWCR & (1<<TWINT))); //ACK/NACK should be received now
        if ((TWSR & 0xF8) != TW_MT_SLA_ACK) {
                DEBUG("I2C: Slave didnt ACK Adress Error:%x\n", (TWSR & 0xF8));
                return 0;
        }
        for(int i = 0; i <length; i++) {
                TWDR = my_data[i];
                TWCR = *(i2c_config[dev].mask);
                while (!(TWCR & (1<<TWINT)));
                if ((TWSR & 0xF8) != TW_MT_DATA_ACK){
                        DEBUG("I2C: Slave didnt ACK BYTE Error:%x  on Byte %x\n",(TWSR & 0xF8), i);
                        return (i+1);
                }
        }
        return length;
}





int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
	DEBUG("I2C: Starting i2c master init \n");
	if(speed >4)
		return -2;
	if(dev >= I2C_NUMOF)
		return -1;
	/* Set up gpio pins*/
	//gpio_init(i2c_config[dev].scl_pin, )

	/* Setup a master mask for later */
	*(i2c_config[dev].mask) = (1<<TWEN)|(1<<TWINT);
	/* clear status register */
	TWSR = 0x00;
	/* set speed speed*/
	TWBR = _twbr_values(speeds[speed]);

	return 0;
}

int i2c_acquire(i2c_t dev)
{
	mutex_lock(i2c_config[dev].used);
	return 0;
}

int i2c_release(i2c_t dev)
{
	mutex_unlock(i2c_config[dev].used);
	return 0;
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
	return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
	if(i2c_acquire(dev) != 0)
		DEBUG("MUTEX ERROR");
	if(dev >= I2C_NUMOF) {
		i2c_release(dev);
		return -1;
	}
	if (__send_start(dev) != TW_START){
		DEBUG("I2C: error on sending Start \n");
		i2c_release(dev);
		return 0;
	}
	int bytes_written = __write_bytes(dev, address, data, length);
	__send_stop(dev);
   	i2c_release(dev);
   	return bytes_written;
}


int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
	/*Write syntax is S. SAddr W. A. MAddr. A. Data0. A. Data1. A... DataN. A. P
	 * S: Start
	 * SAddr: Slave Adress
	 * MAddr: Memory adress
	 * DataN: Data Bytes
	 * A: Ack
	 * P: Stop
	 */
	uint8_t data_array[2] ={reg, data};
	return i2c_write_bytes(dev, address, data_array, 2);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data, int length)
{
	/*Write syntax is S. SAddr W. A. MAddr. A. Data0. A. Data1. A... DataN. A. P
	 * S: Start
	 * SAddr: Slave Adress
	 * MAddr: Memory adress
	 * DataN: Data Bytes
	 * A: Ack
	 * P: Stop
	 */
	uint8_t data_array[length +1];
	uint8_t *casted_data = (uint8_t*) data;
	DEBUG("data_array: 0x%x%x \n", casted_data[0], casted_data[1]);
	data_array[0] = reg;
	for(uint8_t i=1; i<=length; i++) {
		data_array[i]= casted_data[i-1];
	}
	return i2c_write_bytes(dev, address, data_array, (length+1))-1;
}


int i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
	return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
	if(i2c_acquire(dev) != 0)
			DEBUG("MUTEX ERROR");
	if(dev >= I2C_NUMOF) {
		i2c_release(dev);
		return -1;
	}
	if (__send_start(dev) != TW_START){
		DEBUG("I2C: Error on sending start \n");
		i2c_release(dev);
		return 0;
	}
	int bytes_read = __read_bytes(dev, address, data, length);
	__send_stop(dev);
	i2c_release(dev);
	return bytes_read;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
	return i2c_read_regs(dev, address, reg, data,1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
		if(i2c_acquire(dev) != 0)
				DEBUG("MUTEX ERROR \n");
		DEBUG("I2c acquired \n");
		if(dev >= I2C_NUMOF) {
			i2c_release(dev);
			return -1;
		}
		DEBUG("Sending Start \n");
		if (__send_start(dev) != TW_START){
			DEBUG("I2C: error on sending Start \n");
			i2c_release(dev);
			return 0;
		}
		DEBUG("Start Send \n");
		if(__write_bytes(dev, address, &reg, 1) != 1) {
			DEBUG("I2C: error on writing register \n");
			i2c_release(dev);
			return 0;
		}
		//Send repeated start
		if (__send_start(dev) != TW_REP_START) {
			DEBUG("I2C: Error on sending repeated start Error:%x \n",(TWSR & 0xF8));
			i2c_release(dev);
			return 0;
		}
		int8_t bytes_read = __read_bytes(dev, address, data, length);
		__send_stop(dev);
		i2c_release(dev);
		if(bytes_read != length)
		{
			DEBUG("I2C: Error not enough Bytes read \n");
		}
		return bytes_read;
}

