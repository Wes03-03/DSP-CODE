a/*
 * ad1939.h
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#ifndef AD1939_H_
#define AD1939_H_

/* CrossCore Studio SHARC-compatible types */
#define uint8 unsigned char  // Using #define instead of typedef
#define int8  char
#define uint16 unsigned short
#define int16  short
#define uint32 unsigned int
#define int32  int

/* Control Registers */
#define AD1939_PLL_CLK_CTRL0     0x00
#define AD1939_DAC_CTRL0         0x02
#define AD1939_ADC_CTRL0         0x06

/* Function declarations */
void AD1939_Init(void);
void AD1939_WriteRegister(uint8 regAddr, uint8 regData);
uint8 AD1939_ReadRegister(uint8 regAddr);

#endif /* AD1939_H_ */
