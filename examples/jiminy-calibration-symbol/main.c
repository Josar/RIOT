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
    //switch to 16MHz clock, no flash or eeprom functioning
    CLKPR = 1<<CLKPCE;
    CLKPR = 0x0f;
    
    //Adjust clock
    TRXPR &= ~(1<<SLPTR);
    SCCR1 = 0;
    SCCR1 = ((1<<SCCKDIV2) | (1<<SCCKDIV1) | (1<<SCBTSM));
    SCCR0 &= ~((1<<SCCKSEL)|(1<<SCTSE));
    SCCR0 |=(1<<SCEN);
    printf("SCCR0:%02x SCCR1:%02x\n", SCCR0, SCCR1);
    printf("TRXSTATUS: %02x\n", TRX_STATUS);
    while(1) {
    //printf("SCCNTLL: %u\n", SCCNTLL);
        printf("SCCNT: %lu\n", SCCNTLL | (uint16_t)SCCNTLH<<8 | (uint32_t) SCCNTHL<<16 | (uint32_t) SCCNTHH<<24);
    }

    //switch back to 8MHz
    CLKPR = 1<<CLKPCE;
    CLKPR = 0;
}

