#ifndef MAX17048_DEF_H_
#define MAX17048_DEF_H_

/* Start with defines for the configuration */
#define REG_VCELL									(0x02)
#define REG_SOC										0x04
#define REG_MODE									(0x06)
#define REG_VERSION							(0x08)
#define REG_HIBRT									(0x0A)
#define REG_CONFIG								(0x0C)
#define REG_VALRT									(0x14)
#define REG_CRATE									(0x16)
#define REG_VRESET_ID							(0x18)
#define	REG_STATUS								(0x1A)
#define REG_CMD										(0xFE)

#define MODE_QUICK_START				(0x4000)
#define MODE_ENSLEEP						(0x2000)
#define MODE_HIBSTAT							(0x1000)

#define HIBRT_ACTTHR							(0xFF00)
#define HIBRT_HIBTHR							(0x00FF)

#define CONFIG_RCOMP_DEFAULT 		(0x9700)
#define CONFIG_SLEEP							(0x0080)
#define CONFIG_ALSC								(0x0040)
#define CONFIG_ALRT								(0x0020)
#define CONFIG_ATHD							(0x0010)

#define VRESET_ID_DIS_ON					(0x0100)	//Set to 1 to disable Analog Comparator in Hibernation

#define STATUS_RI									(0x01)	//Set to 1: Device is powering up
#define STATUS_VH									(0x02)	//Alert desciptor Voltage High
#define STATUS_VL									(0x04)	//Alert descriptor Voltage Low
#define STATUS_VR									(0x08)	//Alert descriptor Voltage reset
#define STATUS_HD									(0x10)	//Alert descriptor State of charge low
#define STATUS_SC									(0x20)	//alert descriptor state of charge 1% change
#define STATUS_ENVR							(0x40)	/* when set to 1 asserts
												the ALRT pin when a voltage-reset event occurs*/
#define CMD_RESET								(0x5400)

#define MAX17048									(0b00110110)


#define I2C_INVALID								(0xFF)

#endif
