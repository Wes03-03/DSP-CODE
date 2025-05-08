/*
 * ad1939.c
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#include "ad1939.h"
#include <drivers/spi/adi_spi.h>

#include "stdint.h"

/* Fallback definitions if headers not found */
#ifndef ADI_SPI_HANDLE
typedef void* ADI_SPI_HANDLE;
#define ADI_SPI_MASTER 1
#endif

/* SPI Handle */
static ADI_SPI_HANDLE hSPI;

void AD1939_Init(void) {
    adi_spi_Open(0, ADI_SPI_TYPE_MASTER, 0, &hSPI);
    adi_spi_SetBitRate(hSPI, 1000000);
    adi_spi_SetClockPolarity(hSPI, false);
    adi_spi_SetClockPhase(hSPI, true);

    AD1939_WriteRegister(AD1939_PLL_CLK_CTRL0, 0x80);
    AD1939_WriteRegister(AD1939_DAC_CTRL0, 0x00);
    AD1939_WriteRegister(AD1939_ADC_CTRL0, 0x80);
}

void AD1939_WriteRegister(uint8_t regAddr, uint8_t regData) {
    uint8_t txBuf[2] = {regAddr & 0x7F, regData};
    uint8_t rxBuf[2];
    adi_spi_ReadWrite(hSPI, txBuf, rxBuf, 2);
}

uint8_t AD1939_ReadRegister(uint8_t regAddr) {
    uint8_t txBuf[2] = {regAddr | 0x80, 0x00};
    uint8_t rxBuf[2];
    adi_spi_ReadWrite(hSPI, txBuf, rxBuf, 2);
    return rxBuf[1];
}
