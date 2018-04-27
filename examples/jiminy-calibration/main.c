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
 * @brief       rc calibration test
 *
 *@author	Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include "xtimer.h"

#define ASYNC_TIMER                         AS2
#define NO_PRESCALING                       CS20
#define ASYNC_TIMER_CONTROL_REGISTER        TCCR2B
#define ASYNC_TIMER_CONTROL_UPDATE_BUSY     TCR2AUB
#define OUTPUT_COMPARE_UPDATE_BUSY          OCR2AUB
#define TIMER_UPDATE_BUSY                   TCN2UB
#define TIMER                               TCNT2
#define OSCCAL_RESOLUTION                   8
#define LOOP_CYCLES                         7
#define XTAL_FREQUENCY                      32768U
#define EXTERNAL_TICKS                      50U

/* Add calibration functions */

static inline unsigned int Counter(void){
  unsigned int cnt;
  cnt = 0; // Reset counter
  TIMER = 0x00; // Reset async timer/counter
  while (ASSR & ((1<<OUTPUT_COMPARE_UPDATE_BUSY)|(1<<TIMER_UPDATE_BUSY)|(1<<ASYNC_TIMER_CONTROL_UPDATE_BUSY))); // Wait until async timer is updated  (Async Status reg. busy flags).
  do{ 
    cnt++; 
 } while (TIMER < EXTERNAL_TICKS);                             // CPI takes 1 cycle, BRCS takes 2 cycles, resulting in: 2+1(or 2)+1+2=6(or 7) CPU cycles
  return cnt;                                                   // NB! Different compilers may give different CPU cycles!
}

static inline void CalibrateInternalRc(void){

	uint16_t countVal = ((EXTERNAL_TICKS*(uint32_t)8000000)/(((uint32_t)XTAL_FREQUENCY)*LOOP_CYCLES));
	ASSR |= (1<<ASYNC_TIMER); \
	ASYNC_TIMER_CONTROL_REGISTER = (1<<NO_PRESCALING);
	TIMER = 0;
	while (ASSR & ((1<<OUTPUT_COMPARE_UPDATE_BUSY)|(1<<TIMER_UPDATE_BUSY)|(1<<ASYNC_TIMER_CONTROL_UPDATE_BUSY)));

  unsigned int count = 0;
  unsigned char cycles = 0x80;
  uint8_t prev_osccal[2] = {0,0};
  for(;cycles > 0; cycles--)
  {
	  prev_osccal[1] = prev_osccal[0];
	  prev_osccal[0] = OSCCAL;
	  //printf("OSCCAL %x \n", OSCCAL);
	  count = Counter();
	 // if(cycles < 0x80-5) {
	  //printf("count %u \n", count);
	  if(count > countVal){
		  OSCCAL--;
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
	  }else if(count < countVal){
		  OSCCAL++;
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
		  __asm__("NOP");
	  }else{
		  cycles = 1;
	  }
	  //printf("OSCCAL %x prev0 %x prev1 %x \n", OSCCAL, prev_osccal[0], prev_osccal[1]);
	  if(OSCCAL == prev_osccal[1]) {
		  cycles = 1;
	  }
	 // }
  }
}
int main(void){
    OSCCAL = 0x87;
   /* printf("Calibration after bootloader: %02x\n", OSCCAL);
    xtimer_sleep(1);
    OSCCAL = 0x80;
    xtimer_sleep(1); 
    CalibrateInternalRc();
    xtimer_sleep(1); */
    //uint8_t calib = OSCCAL;
    //OSCCAL = 0x87;
    //printf("Calibration after function: %02x\n", calib);
    while(1);
    
}

