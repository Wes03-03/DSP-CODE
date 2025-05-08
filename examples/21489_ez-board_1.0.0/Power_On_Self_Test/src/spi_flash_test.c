/*******************************************************************
Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software
you agree to the terms of the associated Analog Devices License Agreement.

Project Name:  	Power_On_Self_Test

Hardware:		ADSP-21479/21489 EZ-Board

Description:	This file tests the SPI flash on the EZ-Board.
*******************************************************************/

/*******************************************************************
*  include files
*******************************************************************/
#ifdef __ADSP21489__
#include <cdef21489.h>
#include <def21489.h>
#elif __ADSP21479__
#include <cdef21479.h>
#include <def21479.h>
#endif
#include <stdlib.h>
#include "common\flash.h"
#include "m25p16\m25p16.h"
#include <sru.h>
#include <sysreg.h>
#include "post_debug.h"


#define MAN_CODE		0x20		/* Numonyx */
#define DEV_CODE		0x15		/* M25P16 (16Mbit SPI flash) */
#define NUM_SECTORS		32			/* sectors */


/*******************************************************************
*  global variables and defines
*******************************************************************/
#if defined(__DEBUG_FILE__)
extern FILE *pDebugFile;				/* debug file */
#endif


/*******************************************************************
*  function prototypes
*******************************************************************/
int CheckForCFISupport_Spi(void);
ERROR_CODE SetupForFlash(void);


/*******************************************************************
*   Function:    Test_SPI_Flash
*   Description: This test will test the SPI flash on the EZ-Board.
*				 Since an image may live in the start of SPI flash
*                we will not erase the entire flash during this test.
*                We will first verify the manufacturer and vendor IDs
*				 then we will perform operations on only a few sectors.
*******************************************************************/
int Test_SPI_Flash(void)
{
	int i, j;								/* indexes */
	unsigned int iPassed = 0;				/* pass flag */
	ERROR_CODE Result = NO_ERR;				/* result */
	COMMAND_STRUCT pCmdBuffer;				/* command buffer */
	unsigned short usRd = 0, usWr = 0;		/* storage for data */
	int nManCode = 0, nDevCode = 0;			/* man and device ids */
	static unsigned int uiSectors = 0;		/* we use different sectors each time */

	/* sectors to be tested: we chose one sector from a few different banks, making
	   sure we don't overwrite any sector of flash where the boot image or data lives */

	int pnTestSectors[] = { 10, 11, -1, 	/* each line must end with -1 */
							20, 21, -1,
							12, 13, -1,
							22, 23, -1,
							14, 15, -1,
							24, 25, -1,
							16, 17, -1,
							26, 27, -1,
							18, 19, -1,
							28, 29, -1 };

	/* choose different sectors each time through the test, this makes the test a
		little more robust and limits the writes to certain flash sectors */
	uiSectors++;
	if (10 == uiSectors)
	uiSectors = 0;

	DEBUG_HEADER( "SPI Flash Test" );

	/* open the SPI flash */
	Result = m25p16_Open();

	if (Result != NO_ERR)
	{
		DEBUG_STATEMENT( "\nCould not open flash driver" );
		DEBUG_STATEMENT( "\nTest failed" );
		return 0;
	}

	/* setup the device so the DSP can access it */
	if (SetupForFlash() != NO_ERR)
	{
		return 0;
	}

	/* get the codes */
	pCmdBuffer.SGetCodes.pManCode = (unsigned long *)&nManCode;
	pCmdBuffer.SGetCodes.pDevCode = (unsigned long *)&nDevCode;
	pCmdBuffer.SGetCodes.ulFlashStartAddr = 0x0;
	Result = m25p16_Control( CNTRL_GET_CODES, &pCmdBuffer );

	DEBUG_PRINT( "\nDetected manufacturer code of 0x%x and device code of 0x%x", nManCode, nDevCode );

	/* if codes don't match what we expect then we should fail */
	if ( (MAN_CODE != nManCode) || (DEV_CODE != nDevCode) )
	{
		DEBUG_STATEMENT( "\nFlash codes do not match what we expected" );
		DEBUG_STATEMENT( "\nTest failed" );
		return 0;
	}

	/* we also show how to determine if this part supports the Common Flash Interface (CFI) and
		if so, we will attempt to get the CFI data */
	if (!CheckForCFISupport_Spi())
	{
		/* this means there was a failure not that the device does not support CFI, so
			therefore we return failure */
		DEBUG_STATEMENT( "\nFailed checking for CFI support" );
		DEBUG_STATEMENT( "\nTest failed" );
		return 0;
	}

	/* for each test sector */
	for ( usWr = 0x1234, j = uiSectors*3; pnTestSectors[j] != -1; j++ )
	{
		/* make sure it's a valid sector */
		if ( (pnTestSectors[j] < 0) || (pnTestSectors[j] >= NUM_SECTORS) )
		{
			DEBUG_PRINT( "\nSector %d is not a valid sector", pnTestSectors[j] );
			DEBUG_STATEMENT( "\nTest failed" );
			return 0;
		}

		/* erase the sector */
		DEBUG_PRINT( "\nErasing sector %d...", pnTestSectors[j] );
		pCmdBuffer.SEraseSect.ulFlashStartAddr = 0x0;
		pCmdBuffer.SEraseSect.nSectorNum = pnTestSectors[j];
		Result = m25p16_Control( CNTRL_ERASE_SECT, &pCmdBuffer );

		/* if the erase passed, continue */
		if(NO_ERR == Result)
		{
			DEBUG_STATEMENT( "passed" );
			DEBUG_PRINT( "\nTesting sector %d...", pnTestSectors[j] );

			/* write some data */
			for( i = 0; i < 0x8; i+=1, usWr+= 0x1111 )
			{
				unsigned long ulOffset;

				/* calculate offset based on sector */
				ulOffset = pnTestSectors[j]*64*1024 + i;	/* 64KB sectors */

				/* write a value to the flash */
				Result = m25p16_Write( &usWr, ulOffset, 0x1);

				if(NO_ERR == Result)
				{
					iPassed = 1;

					/* now do a read */
					Result = m25p16_Read( &usRd, ulOffset, 0x1);

					if(NO_ERR == Result)
					{
						/* compare the data */
						if( (usRd & 0xff) != (usWr & 0xff) )
						{
							DEBUG_STATEMENT( "failed" );
							DEBUG_PRINT( "\n\nFailure at address 0x%x: expected 0x%x, received 0x%x",
	        								ulOffset, 0xff & (char)usWr, 0xff & (char)usRd );
							iPassed = 0;
							break;
						}
					}
				}
			}

			/* print status */
			if ( iPassed )
			{
				DEBUG_STATEMENT( "passed" );
			}
		}

		/* if erase failed, break out */
		else
		{
			DEBUG_STATEMENT( "failed" );
			break;
		}

		/* at this point the sector test passed, erase it again */
		Result = m25p16_Control( CNTRL_ERASE_SECT, &pCmdBuffer );
	}

	/* close driver and return result */
	Result = m25p16_Close();

	/* print status */
	DEBUG_RESULT( iPassed, "\nTest passed", "\nTest failed" );

	return iPassed;
}


/*******************************************************************
*   Function:    SetupForFlash()
*   Description: Performs the necessary setup for the processor to talk to the
*	             flash.
*******************************************************************/
ERROR_CODE SetupForFlash()
{
	/* setup SPI signals */
	SRU(SPI_MOSI_O,DPI_PB01_I);
	SRU(SPI_MOSI_PBEN_O, DPI_PBEN01_I);

	SRU(DPI_PB02_O, SPI_MISO_I);     //Connect DPI PB2 to MISO.
	SRU(SPI_MISO_PBEN_O, DPI_PBEN02_I);

	SRU(SPI_CLK_O,DPI_PB03_I);
	SRU(SPI_CLK_PBEN_O,DPI_PBEN03_I);

	/* for the flag pins to act as chip select */
	SRU(FLAG4_O, DPI_PB05_I);
	SRU(HIGH, DPI_PBEN05_I);

	/* first set flag 4 as an output */
	sysreg_bit_set( sysreg_FLAGS, FLG4O );
	sysreg_bit_set( sysreg_FLAGS, FLG4 );

	/* setup SPI registers */
	*pSPIDMAC = 0;
	*pSPIBAUD = 0;
	*pSPIFLG = 0xF80;
	*pSPICTL = 0x400;

	return NO_ERR;
}


/*******************************************************************
*   Function:    CheckForCFISupport_Spi
*   Description: Checks if the devices supports the Common Flash Interface
*	             (CFI) and if so gets the CFI data.
*******************************************************************/
int CheckForCFISupport_Spi(void)
{
	ERROR_CODE Result = NO_ERR;			/* result from driver */
	COMMAND_STRUCT pCmdBuffer;			/* command buffer union */
	bool bSupportsCFI = false;			/* see if this device supports CFI */
	int nFlashWidth = 0x0;				/* flash width */
	char pCFIData[128*2];				/* provide enough for 16-bit wide flashes */
	char *pQRY = NULL;					/* ptr to QRY string within CFI data */
	unsigned short usPrimary = 0x0;		/* primary pointer within CFI data */
	unsigned short usDeviceSize = 0x0;	/* device size (2^n) within CFI data */
	unsigned short usMajVerASCII = 0x0;	/* major version in ASCII within CFI data */
	unsigned short usMinVerASCII = 0x0;	/* minor version in ASCII within CFI data */

	/* see if this device support CFI */
	pCmdBuffer.SSupportsCFI.pbSupportsCFI = &bSupportsCFI;
	Result = m25p16_Control( CNTRL_SUPPORTS_CFI, &pCmdBuffer );
	if (Result == UNKNOWN_COMMAND)
	{
		return 1;	/* not a failure, but the driver doesn't even support this command */
	}
	else if (Result != NO_ERR)
    {
        return 0;
    }

    /* if it supports CFI we can read the CFI data */
	if ( bSupportsCFI )
	{
		/* first check on the device width so we know how large the CFI data will be and
			how to interpret it */
		pCmdBuffer.SGetFlashWidth.pnFlashWidth = &nFlashWidth;
		Result = m25p16_Control( CNTRL_GET_FLASH_WIDTH, &pCmdBuffer );
		if (Result == UNKNOWN_COMMAND)
		{
			return 1;	/* not a failure, but the driver doesn't even support this command */
		}
		else if (Result != NO_ERR)
	    {
	        return 0;
	    }

	    /* now get the CFI data, make sure enough memory is allocated for the flash width
	    	which is 128 bytes per 8-bit width */
	    pCmdBuffer.SGetCFIData.pCFIData = pCFIData;
	    pCmdBuffer.SGetCFIData.ulFlashStartAddr = 0x0;
		Result = m25p16_Control( CNTRL_GET_CFI_DATA, &pCmdBuffer );
		if (Result == UNKNOWN_COMMAND)
		{
			return 1;	/* not a failure, but the driver doesn't even support this command */
		}
		else if (Result != NO_ERR)
	    {
	        return 0;
	    }

	    /* take a look at some of the CFI data, for example according to the CFI spec, the
	    	string "QRY" should be at offset 0x10, note the byte offset always gets multiplied
	    	by nFlashWidth, so for x16 devices the offset 0x10 becomes byte offset 0x20 */

	    /* if not x8 or x16 we won't check the data, we only allocated enough for up to x16 */
	    if ( (_FLASH_WIDTH_8 == nFlashWidth) || (_FLASH_WIDTH_16 == nFlashWidth) )
	    {
	    	/* check QRY string, note x16 devices will have a NULL after each character, check the
	    		CFI spec for details on other architectures */
	    	pQRY = &pCFIData[0x10 * nFlashWidth];

	    	/* check the primary (P) offset, this is used to offset into the extended area */
	    	usPrimary = pCFIData[0x15 * nFlashWidth];

	    	/* versions are some of the things stored in the extended area */
	    	usMajVerASCII = pCFIData[(usPrimary+0x3) * nFlashWidth];
	    	usMinVerASCII = pCFIData[(usPrimary+0x4) * nFlashWidth];
	    }
	}

	return 1;
}
