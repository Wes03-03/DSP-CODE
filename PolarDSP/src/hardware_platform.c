/*
 * hardware_platform.c
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#include "hardware_platform.h"
#include <cdef21489.h>

void Init_EZKIT(void) {
    *pPLLCTL = 0x2000;
    *pPLLDIV = 0x0005;
    *pPLLCTL = 0x3000;
    while(!(*pPLLSTAT & 0x0001));

    *pGPIO_DIR |= 0x0001;
    *pGPIO_SET = 0x0001;
}

void Init_Interrupts(void) {
    interrupt(SIG_SP0, I2S_ISR);
    asm("sti;");
}
