/*******************************************************************************/
/*                                                                             */
/*   (C) Copyright 2008 - Analog Devices, Inc.  All rights reserved.           */
/*                                                                             */
/*    FILE:     m25p16.c                                                       */
/*                                                                             */
/*    PURPOSE:  Performs operations specific to the M25P16 flash device.       */
/*                                                                             */
/*******************************************************************************/

/* includes */
#ifdef __ADSP21469__
#include <cdef21469.h>
#include <def21469.h>
#elif __ADSP21479__
#include <cdef21479.h>
#include <def21479.h>
#elif __ADSP21489__
#include <cdef21489.h>
#include <def21489.h>
#else
	#error "*** The flash driver does not yet support this processor ***"
#endif
#include <stdio.h>
#include "../common/flash.h"
#include "../m25p16/m25p16.h"

#include <sru.h>
#include <sysreg.h>

#define NRDY			BIT_0
#define PAGE_LENGTH		64  //(in 32-bit words)
#define NUM_SECTORS 	32			/* number of sectors in the flash device */

static char 	*pFlashDesc =		"STMicro. M25P16";
static char 	*pDeviceCompany	=	"STMicroelectronics";

static int		gNumSectors = NUM_SECTORS;

#undef TIMEOUT
#undef DELAY

/* flash commands */
#define SPI_WREN            (0x06)  //Set Write Enable Latch
#define SPI_WRDI            (0x04)  //Reset Write Enable Latch
#define SPI_RDID            (0x9F)  //Read Identification
#define SPI_RDSR            (0x05)  //Read Status Register
#define SPI_WRSR            (0x01)  //Write Status Register
#define SPI_READ            (0x03)  //Read data from memory
#define SPI_FAST_READ       (0x0B)  //Read data from memory
#define SPI_PP              (0x02)  //Program Data into memory
#define SPI_SE              (0xD8)  //Erase one sector in memory
#define SPI_BE              (0xC7)  //Erase all memory
#define WIP                  (0x1)	//Check the write in progress bit of the SPI status register
#define WEL                  (0x2)	//Check the write enable bit of the SPI status register

#define SPI_PAGE_SIZE		(528)
#define SPI_SECTORS		    (512)
#define SPI_SECTOR_SIZE		(4224)
#define SPI_SECTOR_DIFF		(3968)
#define PAGE_BITS			(10)
#define PAGE_SIZE_DIFF		(496)

#define DELAY				300
#define TIMEOUT        		35000*64


/* function prototypes */
static ERROR_CODE EraseFlash(unsigned long ulStartAddr);
static ERROR_CODE EraseBlock( int nBlock, unsigned long ulStartAddr );
static ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulStartAddr);
static ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector );
static ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );
static ERROR_CODE ReadFlash(unsigned long ulOffset, unsigned short *pusValue );
static ERROR_CODE ResetFlash(unsigned long ulStartAddr);
static ERROR_CODE WriteFlash(unsigned long ulOffset, unsigned short usValue );
static unsigned long GetFlashStartAddress( unsigned long ulAddr);

static ERROR_CODE ReadStatusRegister(int *pStatus);
static ERROR_CODE Wait_For_SPIF(void);
static ERROR_CODE SendSingleCommand( const int nCommand );
static ERROR_CODE Wait_For_RDY( void );
static void Assert_SPI_CS(void);
static void Clear_SPI_CS(void);
static ERROR_CODE WriteByteToSPI(const int byByte, const int msb_lsb);
static ERROR_CODE ReadByteFromSPI(int *pbyByte, const int msb_lsb);


ERROR_CODE m25p16_Open(void)
{
	/* setup baud rate */
	*pSPIBAUD = BAUD_RATE_DIVISOR;

	return (NO_ERR);
}


ERROR_CODE m25p16_Close(void)
{
	return (NO_ERR);
}


ERROR_CODE m25p16_Read( unsigned short *pusData,
                        unsigned long ulStartAddress,
                        unsigned int uiCount )
{
    ERROR_CODE Result = NO_ERR;
    unsigned int i = 0;
    unsigned short *pusCurrentData = pusData;
    unsigned long ulCurrentAddress = ulStartAddress;


    for (i = 0; i < uiCount; i++, ulCurrentAddress++, pusCurrentData++)
    {
		Assert_SPI_CS();

		// 1 byte of command
		if( NO_ERR != WriteByteToSPI( SPI_READ, MSBF ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of address
		if( NO_ERR != WriteByteToSPI( (ulCurrentAddress >> 16), MSBF ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of address
		if( NO_ERR != WriteByteToSPI( (ulCurrentAddress >> 8), MSBF ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of address
		if( NO_ERR != WriteByteToSPI( ulCurrentAddress, MSBF ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of garbage data
		if( NO_ERR != ReadByteFromSPI( (int*)pusCurrentData, 0 ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of GOOD data
		if( NO_ERR != ReadByteFromSPI( (int*)pusCurrentData, 0 ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		Clear_SPI_CS();
    }

	return(Result);
}


ERROR_CODE m25p16_Write( unsigned short *pusData,
                         unsigned long ulStartAddress,
                         unsigned int uiCount )
{
    ERROR_CODE Result = NO_ERR;
    unsigned int i = 0;
    unsigned short *pusCurrentData = pusData;
    unsigned long ulCurrentAddress = ulStartAddress;


    for (i = 0; i < uiCount; i++, ulCurrentAddress++, pusCurrentData++)
    {
		SendSingleCommand( SPI_WREN );	// write enable

		Assert_SPI_CS();

		// 1 byte of command
		if( NO_ERR != WriteByteToSPI( SPI_PP, MSBF ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of address
		if( NO_ERR != WriteByteToSPI( (ulCurrentAddress >> 16), MSBF ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of address
		if( NO_ERR != WriteByteToSPI( (ulCurrentAddress >> 8), MSBF ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of address
		if( NO_ERR != WriteByteToSPI( ulCurrentAddress, MSBF ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		// 1 byte of data
		if( NO_ERR != WriteByteToSPI( *pusCurrentData, 0 ) )
		{
			Clear_SPI_CS();
			return POLL_TIMEOUT;
		}

		Clear_SPI_CS();

		// wait for the write to complete.
		if( NO_ERR != Wait_For_RDY() )
		{
			return POLL_TIMEOUT;
		}


		// send the write disable command
		return SendSingleCommand( SPI_WRDI );	// write disable
    }

    return(Result);
}


ERROR_CODE m25p16_Control(  unsigned int uiCmd,
                            COMMAND_STRUCT *pCmdStruct)
{
	ERROR_CODE ErrorCode = NO_ERR;

	// switch on the command
	switch ( uiCmd )
	{
		// erase all
		case CNTRL_ERASE_ALL:
			ErrorCode = EraseFlash(pCmdStruct->SEraseAll.ulFlashStartAddr);
			break;

		// erase sector
		case CNTRL_ERASE_SECT:
			ErrorCode = EraseBlock( pCmdStruct->SEraseSect.nSectorNum, pCmdStruct->SEraseSect.ulFlashStartAddr );
			break;

		// get manufacturer and device codes
		case CNTRL_GET_CODES:
			ErrorCode = GetCodes((int *)pCmdStruct->SGetCodes.pManCode, (int *)pCmdStruct->SGetCodes.pDevCode, (unsigned long)pCmdStruct->SGetCodes.ulFlashStartAddr);
			break;

		case CNTRL_GET_DESC:
			//Filling the contents with data
			pCmdStruct->SGetDesc.pDesc  = pFlashDesc;
			pCmdStruct->SGetDesc.pFlashCompany  = pDeviceCompany;
			break;

		// get sector number based on address
		case CNTRL_GET_SECTNUM:
			ErrorCode = GetSectorNumber( pCmdStruct->SGetSectNum.ulOffset, (int *)pCmdStruct->SGetSectNum.pSectorNum );
			break;

		// get sector number start and end offset
		case CNTRL_GET_SECSTARTEND:
			ErrorCode = GetSectorStartEnd( pCmdStruct->SSectStartEnd.pStartOffset, pCmdStruct->SSectStartEnd.pEndOffset, pCmdStruct->SSectStartEnd.nSectorNum );
			break;

		// get the number of sectors
		case CNTRL_GETNUM_SECTORS:
			pCmdStruct->SGetNumSectors.pnNumSectors[0] = gNumSectors;
			break;

		// reset
		case CNTRL_RESET:
			ErrorCode = ResetFlash(pCmdStruct->SReset.ulFlashStartAddr);
			break;

		// no command or unknown command do nothing
		default:
			// set our error
			ErrorCode = UNKNOWN_COMMAND;
			break;
	}

	// return
	return(ErrorCode);
}


//----------- R e s e t F l a s h  ( ) ----------//
//
//  PURPOSE
//  	Sends a "reset" command to the flash.
//
//	INPUTS
//		unsigned long ulStartAddr - flash start address
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE ResetFlash(unsigned long ulAddr)
{

	ERROR_CODE ErrorCode = NO_ERR;
	int nStatus;

	ErrorCode = ReadStatusRegister(&nStatus);

	return ErrorCode;

}


//----------- E r a s e F l a s h  ( ) ----------//
//
//  PURPOSE
//  	Sends an "erase all" command to the flash.
//
//	INPUTS
//		unsigned long ulStartAddr - flash start address
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE EraseFlash(unsigned long ulAddr)
{

	int nTimeout = 1000;

	if( NO_ERR != SendSingleCommand( SPI_WREN ) )	// write enable
	{
		return POLL_TIMEOUT;
	}

	if( NO_ERR != SendSingleCommand( SPI_BE ) )	// erase command
	{
		return POLL_TIMEOUT;
	}

	// The Wait_For_RDY() function will timeout after 1000 loops,
	// however that is not long enough for an erase, so it's enclosed
	// here to give it 1000 * 1000 loops, long enough for an erase  operation
	while(nTimeout-- > 0 )
	{
		if( NO_ERR == Wait_For_RDY() )
		{
			// send the write disable command
			return SendSingleCommand( SPI_WRDI );	// write disable
		}
	}

	return POLL_TIMEOUT;
}


//----------- E r a s e B l o c k ( ) ----------//
//
//  PURPOSE
//  	Sends an "erase block" command to the flash.
//
//	INPUTS
//		int nBlock - block to erase
//		unsigned long ulStartAddr - flash start address
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE EraseBlock( int nBlock, unsigned long ulAddr )
{

	ERROR_CODE 	  ErrorCode   = NO_ERR;		//tells us if there was an error erasing flash
 	unsigned long ulSectStart = 0x0;		//stores the sector start offset
 	unsigned long ulSectEnd   = 0x0;		//stores the sector end offset(however we do not use it here)
	int nTimeout = 1000;
	int nSecAddr = 0;

	// Get the sector start offset
	// we get the end offset too however we do not actually use it for Erase sector
	GetSectorStartEnd( &ulSectStart, &ulSectEnd, nBlock );

	SendSingleCommand( SPI_WREN );	// write enable

	Assert_SPI_CS();

	// 1 byte of data
	if( NO_ERR != WriteByteToSPI( SPI_SE, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulSectStart >> 16), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulSectStart >> 8), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( ulSectStart, MSBF ) )
		{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

	// The Wait_For_RDY() function will timeout after 1000 loops,
	// however that is not long enough for an erase, so it's enclosed
	// here to give it 1000 * 1000 loops, long enough for an erase  operation
	while(nTimeout-- > 0 )
	{
		if( NO_ERR == Wait_For_RDY() )
		{
			// send the write disable command
			return SendSingleCommand( SPI_WRDI );	// write disable
		}
	}

	return POLL_TIMEOUT;
}


//----------- G e t C o d e s ( ) ----------//
//
//  PURPOSE
//  	Sends an "auto select" command to the flash which will allow
//		us to get the manufacturer and device codes.
//
//  INPUTS
//  	int *pnManCode - pointer to manufacture code
//		int *pnDevCode - pointer to device code
//		unsigned long ulStartAddr - flash start address
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulAddr)
{
	int wWord = 0;

	Assert_SPI_CS();

	if( NO_ERR != WriteByteToSPI( SPI_RDID, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// This is a dummy read which pulls in the
	// SO data clocked in from the write.
	if( NO_ERR != ReadByteFromSPI(&wWord, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// this is the real data after the write
	if( NO_ERR != ReadByteFromSPI(pnManCode, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// This is a dummy read which pulls in the
	// SO data clocked in from the write.
	if( NO_ERR != ReadByteFromSPI(&wWord, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// this is the real data after the write
	if( NO_ERR != ReadByteFromSPI(pnDevCode, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

	return ResetFlash(ulAddr);
}

//----------- G e t S e c t o r N u m b e r ( ) ----------//
//
//  PURPOSE
//  	Gets a sector number based on the offset.
//
//  INPUTS
//  	unsigned long ulAddr - absolute address
//		int 	 *pnSector     - pointer to sector number
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector )
{
	int nSector = 0;
	int i;
	int error_code = 1;
	unsigned long ulMask;					//offset mask
	unsigned long ulOffset;					//offset
	unsigned long ulStartOff;
	unsigned long ulEndOff;

	ulMask      	  = 0x7ffffff;
	ulOffset		  = ulAddr & ulMask;

	for(i = 0; i < gNumSectors; i++)
	{
	    GetSectorStartEnd(&ulStartOff, &ulEndOff, i);
		if ( (ulOffset >= ulStartOff)
			&& (ulOffset <= ulEndOff) )
		{
			error_code = 0;
			nSector = i;
			break;
		}
	}

	// if it is a valid sector, set it
	if (error_code == 0)
		*pnSector = nSector;
	// else it is an invalid sector
	else
		return INVALID_SECTOR;

	// ok
	return NO_ERR;
}


//----------- G e t S e c t o r S t a r t E n d ( ) ----------//
//
//  PURPOSE
//  	Gets a sector start and end address based on the sector number.
//
//  INPUTS
//  	unsigned long *ulStartOff - pointer to the start offset
//		unsigned long *ulEndOff - pointer to the end offset
//		int nSector - sector number
//
//	RETURN VALUE
//		ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector )
{
	unsigned long ulSectorSize = 0x10000;

	if( ( nSector >= 0 ) && ( nSector < gNumSectors ) ) // 32 sectors
		{
			*ulStartOff = nSector * ulSectorSize;
			*ulEndOff = ( (*ulStartOff) + ulSectorSize - 1 );
		}
	else
		return INVALID_SECTOR;


	// ok
	return NO_ERR;
}


//----------- G e t F l a s h S t a r t A d d r e s s ( ) ----------//
//
//  PURPOSE
//  	Gets flash start address from an absolute address.
//
//  INPUTS
//  	unsigned long ulAddr - absolute address
//
//	RETURN VALUE
//		unsigned long - Flash start address

unsigned long GetFlashStartAddress( unsigned long ulAddr)
{

	ERROR_CODE 	  ErrorCode = NO_ERR;		//tells us if there was an error erasing flash
	unsigned long ulFlashStartAddr;			//flash start address

	ulFlashStartAddr  =  0;

	return(ulFlashStartAddr);
}


//----------- R e a d F l a s h ( ) ----------//
//
//  PURPOSE
//  	Reads a value from an address in flash.
//
//  INPUTS
// 		unsigned long ulAddr - the address to read from
// 		int pnValue - pointer to store value read from flash
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE ReadFlash( unsigned long ulAddr, unsigned short *pusValue )
{

	Assert_SPI_CS();

	// 1 byte of command
	if( NO_ERR != WriteByteToSPI( SPI_READ, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulAddr >> 16), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulAddr >> 8), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( ulAddr, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of garbage data
	if( NO_ERR != ReadByteFromSPI( (int*)pusValue, 0 ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of GOOD data
	if( NO_ERR != ReadByteFromSPI( (int*)pusValue, 0 ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

	// ok
	return NO_ERR;
}


//----------- W r i t e F l a s h ( ) ----------//
//
//  PURPOSE
//  	Write a value to an address in flash.
//
//  INPUTS
//	 	unsigned long  ulAddr - address to write to
//		unsigned short nValue - value to write
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

ERROR_CODE WriteFlash( unsigned long ulAddr, unsigned short usValue )
{
	SendSingleCommand( SPI_WREN );	// write enable

	Assert_SPI_CS();

	// 1 byte of command
	if( NO_ERR != WriteByteToSPI( SPI_PP, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulAddr >> 16), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulAddr >> 8), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( ulAddr, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of data
	if( NO_ERR != WriteByteToSPI( usValue, 0 ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}


	Clear_SPI_CS();

	// wait for the write to complete.
	if( NO_ERR != Wait_For_RDY() )
	{
		return POLL_TIMEOUT;
	}


	// send the write disable command
	return SendSingleCommand( SPI_WRDI );	// write disable

}


//----------- R e a d S t a t u s R e g i s t e r ( ) ----------//
//
//  PURPOSE (2 Bytes)
// 		Returns the 8-bit value of the status register.
//
//	OUTPUTS  	second read byte ,
//        		first read byte is garbage.
//				Core sends the command
//
//  RETURN VALUE
//  	Staus of the register

ERROR_CODE ReadStatusRegister(int *pStatus)
{
	int wWord = 0;

	// clear the RX buffer
	*pSPICTL |= (RXFLSH);
	asm("nop;");
	asm("nop;");
	asm("nop;");

	Assert_SPI_CS();

	if( NO_ERR != WriteByteToSPI( SPI_RDSR, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// This is a dummy read which pulls in the
	// SO data clocked in from the write.
	if( NO_ERR != ReadByteFromSPI(pStatus, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// this is the real data after the write
	if( NO_ERR != ReadByteFromSPI(pStatus, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();


	return NO_ERR;

}

//////////////////////////////////////////////////////////////////////////////
//
// ERROR_CODE WriteByteToSPI(const int byByte, const int msb_lsb)
//
// Writes one byte to the SPI port can write in either msb or lsb format
// waits for the spi to clear the SPIF bit meaning the data
// has been sent
//
//////////////////////////////////////////////////////////////////////////////

ERROR_CODE WriteByteToSPI(const int byByte, const int msb_lsb)
{
	int nTimeOut = 100000;
	int n;

	if( NO_ERR != Wait_For_SPIF() )
	{
		return POLL_TIMEOUT;
	}

	while( (TXS & *pSPISTAT) )
	{
		if( nTimeOut-- < 0 )
		{
			return POLL_TIMEOUT;
		}
	}

	*pSPICTL = (SPIEN|SPIMS|SENDZ|TIMOD1|WL8|msb_lsb);
	asm("nop;");
	asm("nop;");
	asm("nop;");
	*pTXSPI = byByte;

	if( NO_ERR != Wait_For_SPIF() )
	{
		return POLL_TIMEOUT;
	}

	return NO_ERR;
}


//////////////////////////////////////////////////////////////////////////////
//
// ERROR_CODE ReadByteFromSPI(int *pbyByte, const int msb_lsb)
//
// Reads one byte from the spi port.  This may or may not cause a sclk or send
// event.  If there is something waiting in the spi RX buffer, this will not
// cause an sclk shift from the spi
//
//////////////////////////////////////////////////////////////////////////////

ERROR_CODE ReadByteFromSPI(int *pbyByte, const int msb_lsb)
{
	int nTimeOut = 1000;

	if( NO_ERR != Wait_For_SPIF() )
	{
		return POLL_TIMEOUT;
	}


	// don't read until there is something to read.
	nTimeOut = 1000;
	while( !(RXS & *pSPISTAT) )
	{
		if( nTimeOut-- < 0 )
		{
			return POLL_TIMEOUT;
		}
	}

	*pSPICTL = (SPIEN|SPIMS|SENDZ|WL8|msb_lsb);
	asm("nop;");
	asm("nop;");
	asm("nop;");
	*pbyByte = *pRXSPI;

	return NO_ERR;

}

//////////////////////////////////////////////////////////////////////////////
//
// void Assert_SPI_CS(void)
//
// Asserts the CS on FLG4 setup by the SRU
//
//////////////////////////////////////////////////////////////////////////////

void Assert_SPI_CS(void)
{
	int n;

#if ( defined(__ADSP21375__) || defined(__ADSP21369__) || defined(__ADSP21469__) || defined (__ADSP21479__)|| defined (__ADSP21489__) )
	//Then control the level of flag 4
	sysreg_bit_clr( sysreg_FLAGS, FLG4 );  //logic low
#elif (__ADSP21364__) || (__ADSP21262__)
	//Then control the level of flag 0
	sysreg_bit_clr( sysreg_FLAGS, FLG0 );  //logic low
#endif

	*pSPIBAUD = BAUD_RATE_DIVISOR;

}

//////////////////////////////////////////////////////////////////////////////
//
// void Clear_SPI_CS(void)
//
// DE-Asserts the CS on FLG4 setup by the SRU
//
//////////////////////////////////////////////////////////////////////////////

void Clear_SPI_CS(void)
{
	int n;


#if ( defined(__ADSP21375__) || defined(__ADSP21369__) || defined(__ADSP21469__) || defined (__ADSP21479__) || defined (__ADSP21489__) )
	//Then control the level of flag 4
	sysreg_bit_set( sysreg_FLAGS, FLG4 );  //Logic high
#elif (__ADSP21364__) || (__ADSP21262__)
	//Then control the level of flag 0
	sysreg_bit_set( sysreg_FLAGS, FLG0 );  //Logic high
#endif
	*pSPIBAUD = 0;

}



//----------- W a i t _ f o r _ S P I F ( ) ----------//
//
//  PURPOSE (1 Byte)
// 		Polls the SPIF (SPI single word transfer complete) bit
//		of SPISTAT until the transfer is complete.
//

ERROR_CODE Wait_For_SPIF(void)
{

	int nTimeout = 10000;
	// status updates can be delayed up to 10 cycles
	// so wait at least 10 cycles before even
	// checking them
	int n;

	// make sure nothing is waiting to be sent
	while( !(SPIF & *pSPISTAT) )
	{
		if( nTimeout-- < 0 )
		{
			return POLL_TIMEOUT;
		}
	}

	return NO_ERR;

}




ERROR_CODE SendSingleCommand( const int iCommand )
{

	Assert_SPI_CS();

	if( NO_ERR != WriteByteToSPI( iCommand, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

	return NO_ERR;

}


//----------- W a i t _ f o r _ R D Y ( ) ----------//
//
//  PURPOSE (1 Byte)
//		Polls the RDY (Write In Progress) bit of the Flash's status
//		register until the Flash is finished with its access. Accesses
//		that are affected by a latency are Page_Program, Sector_Erase,
//		and Block_Erase.

ERROR_CODE Wait_For_RDY( void )
{
	int nTimeout = 10000;
	int n;
	int iTest;

	while(nTimeout-- > 0)
	{
		ReadStatusRegister(&iTest);
		if( !(iTest & NRDY) )
		{
			return NO_ERR;
		}
	};


	// we can return
	return POLL_TIMEOUT;
}

