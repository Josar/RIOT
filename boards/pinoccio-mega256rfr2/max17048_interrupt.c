#include "max17048_interrupt.h"
#include "stdio.h"

void  activate_alert_interrupt(void)
{
	EICRB &= ~((1<<ISC71)&(1<<ISC70));
	//EICRB |= (1<<ISC70);
	EIMSK |= (1<<INT7);
	sei();
}

void disable_alert_interrupt(void)
{
	EIMSK &= ~(1<<INT7);
}

void restart_alert_interrupt(void)
{
	EIMSK |= (1<<INT7);
}



ISR(INT7_vect)
{
	//get statusregister
	//disable_alert_interrupt();
	EIMSK &= ~(1<<INT7);
	uint8_t status[2];
	 i2c_read_regs(I2C_0, MAX17048, REG_STATUS, status,2);
	 //printf("%x \n", status[0]);
	 //status uses only in msb therefore only status[0] needed
	 if(status[0] & STATUS_VH) {
		 //Voltage high
		 printf("Voltage High \n");
	 }
	 if(status[0] & STATUS_VL) {
		 //Voltage low
		 printf("Voltage Low \n");
	 }
	 if(status[0] & STATUS_VR) {
		 //Voltage reset
		 printf("Voltage Reset \n");
	 }
	 if(status[0] & STATUS_HD) {
		 //SOC low
		 printf("SOC low \n");
	 }
	 if(status[0] & STATUS_SC) {
		 //SOC changed 1%
		 printf("Soc changed 1%% \n");
	 }
	 uint8_t config[2];
	 i2c_read_regs(I2C_0, MAX17048, REG_CONFIG, config,2);
	config[1] &= ~CONFIG_ALRT;
	i2c_write_regs(I2C_0, MAX17048, REG_CONFIG, config,2);
	restart_alert_interrupt();
}
