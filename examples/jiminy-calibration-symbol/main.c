/*
 * Copyright (C) 2018 RWTH Aachen, Steffen Robertz
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

int main(void){
    
    #define SYMBOL_CMP1 0xFE
    //OSCCAL = 0x90;
    
    //switch to 16MHz clock, no flash or eeprom functioning
    CLKPR = 1<<CLKPCE;
    CLKPR = 0x0f;
    
    while(1); 
    //Adjust clock
    TRXPR &= ~(1<<SLPTR);
    SCCR1 = 0;
    SCCR1 = ((1<<SCCKDIV2) | (1<<SCCKDIV1) | (1<<SCBTSM));
    SCCR0 = 0;
    SCCNTLH = 0;
    SCCNTHL = 0;
    SCCNTHH = 0;
    SCCNTLL = 0;
    SCOCR1HH = 0;
    SCOCR1HL = 0;
    SCOCR1LH = 0;
    SCOCR1LL = SYMBOL_CMP1;
    SCIRQM = (1<<IRQMCP1); 

    //Set up compare clock
    TCCR5A = 0;
    TCNT5 = 0xffff;
    TIMSK5 = 0;    
    
    TCCR5B = 1<<CS11; //3 Cycles
    SCCR0 |=(1<<SCEN); //7 Cycles
    while(1);
    //switch back to 8MHz
    CLKPR = 1<<CLKPCE;
    CLKPR = 0;
}


ISR(SCNT_CMP1_vect){
    //44 cycles to get here => TCNT5 already increased by 5.5
    printf("Timer5_count = %u\n", (TCNT5-6)*2); //saving takes 4 cyles => 0.5 increased TCNT5
    SCCNTLH = 0;
    SCCNTHL = 0;
    SCCNTHH = 0;
    SCCNTLL = 0;
    TCNT5 = 0;
}
