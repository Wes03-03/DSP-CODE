/*******************************************************************
Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software
you agree to the terms of the associated Analog Devices License Agreement.

Project Name:  	Power_On_Self_Test

Hardware:		ADSP-21489 EZ-Board or ADSP-21479 EZ-Board

Description:	This example performs POST on the 21489 or 21479 EZ-Board.

Please view the readme.txt file for detailed information related to
switch and jumper settings, LED indicators, etc.

Enter POST standard test loop by pressing PB1 at startup, or by entering
a sequence using PB1 and PB2 to choose a specific test.

*******************************************************************/
#include <cdef21489.h>
#include <def21489.h>
#include <sysreg.h>
#include <SRU.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include "post_common.h"
#include "post_debug.h"
#include "pb_led_test.h"
/* Managed drivers and/or services include */
#include "../system/adi_initialize.h"


/*******************************************************************
*  function prototypes
*******************************************************************/
void Init_PLL(void);					/* PLL setup */
void Init_SDRAM(void);					/* external memory setup */

#if defined(__DEBUG_UART__)
void Init_UART(void);					/* UART setup */
#endif

int Test_Dummy_Fail(void) {return 0;}	/* a dummy test that returns fail */


/*******************************************************************
*  global variables and defines
*******************************************************************/
extern volatile bool gb_sw8_pushed;

static int g_loops = 0;						/* loop counter */
bool g_bLoopFlagCheck = false;				/* flag used to check for loop flag test */
bool g_bLoopOnTest = false;					/* when set will cause next test to loop forever */
bool g_bIsTestChosen = false;				/* test chosen flag */
int g_nSelectedTest = 0;					/* chosen test */
volatile int g_nPostState = STATE_START;	/* POST state used when entering a test */

/* firmware version definitions */
#define FIRMWARE_MAJOR_REV 1
#define FIRMWARE_MINOR_REV 6				/* revision 1.6 for LED flash */

/* force user to specify a valid EZ-Board rev */
#ifdef _21489_EZBOARD_REV_0_1_
char g_ezboard_rev[] = "0.1";
#elif defined(_21489_EZBOARD_REV_0_2_)
char g_ezboard_rev[] = "0.2";
#else
#error *** EZ-Board rev not specified, please define a valid rev ***
#endif

#if defined(__DEBUG_FILE__)
FILE *pDebugFile;				/* debug file when directing output to a file */
#endif


typedef int (*pfnTests)(void);

typedef struct _PostTestParameters
{
	pfnTests		m_pTestFunction;	/* test function pointer */
	bool			m_bStandardLoop;	/* is this part of the standard loop? */
	bool			m_bOneTime;			/* does this test run one-time? */
	bool			m_bIgnoreFailures;	/* ignore failures for this test? */
	unsigned int	m_uiPassed;			/* number of times this test passed */
	unsigned int	m_uiFailed;			/* number of times this test failed */
	char			m_pszName[16];		/* NULL terminated string with test name */
} PostTestParameters;


/* test structure - Configurable structure allows you to specify function pointers,
	whether the test is part of the standard loop, whether it should run one-time,
	and whether to ignore failures.  We also track the number of passes and fails
	for each test which can be displayed with the Display_Status command. */
PostTestParameters g_Tests[] =
{
	/* ================================================================================================ */
	/* 							| PART OF |ONE  |IGNORE|# OF |# OF |	TEST NAME		| TEST |  PB &	*/
	/*    TEST FUNCTION			| STANDARD|TIME?|FAILS?|PASS |FAIL |   	15 CHARS + NULL	|  ID  |  LED	*/
	/*							| LOOP?	  |		|	   |	 |	   | 123456789ABCDEF0	|	   |SEQUENCE*/
	/* ================================================================================================ */
	{ Test_Standard_Loop,			true,  false, false,  0,   0,	"Standard Loop" },	/*  0  | 00000  */
	{ Test_Processor_Version,		true,  true,  true,   0,   0,	"Proc Version" },	/*  1  | 00001  */
    { Test_Pushbuttons_LEDs,		true,  true,  false,  0,   0,	"Pushbuttons/LED" },/*  2  | 00010  */
#ifdef __ADSP21489__
   	{ Test_Temp_Sensor,       		true,  true,  false,  0,   0,	"Temp Sensor" },	/*  3  | 00011  */
#else
	{ Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #3" },	/*  3  | 00011  */
#endif
    { Test_SRAM,					true,  false, false,  0,   0,	"SRAM" },			/*  4  | 00100  */
    { Test_Parallel_Flash,   		true,  false, false,  0,   0,	"Parallel Flash" },	/*  5  | 00101  */
    { Test_SPI_Flash,		 	 	true,  false, false,  0,   0,	"SPI Flash" },		/*  6  | 00110  */
    { Test_SPDIF,  	    			true,  false, false,  0,   0,	"SPDIF" },			/*  7  | 00111  */
    /* ------------------------------------------------------------------------------------------------ */
    { Test_UART,					true,  false, false,  0,   0,	"UART" },			/*  8  | 01000  */
    { Test_SDRAM,					true,  false, false,  0,   0,	"SDRAM" },			/*  9  | 01001  */
	{ Test_Analog_Audio_RCA,		false, true,  false,  0,   0,	"Audio RCA test" },	/* 10  | 01010  */
	{ Test_Analog_Audio_Diff,		false, true,  false,  0,   0,	"Audio Diff test" },/* 11  | 01011  */
	{ Check_Config_Data,			false, false, false,  0,   0,	"Check Cfg Data" }, /* 12  | 01100  */
	{ Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #13" },	/* 13  | 01101  */
	{ Test_Watchdog_Timer,			false, true,  false,  0,   0,	"Watchdog Timer" },	/* 14  | 01110  */
	{ Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #15" },	/* 15  | 01111  */
	/* ------------------------------------------------------------------------------------------------ */
	{ Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #16" },	/* 16  | 10000  */
	{ Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #17" },	/* 17  | 10001  */
	{ Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #18" },	/* 18  | 10010  */
	{ Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #19" },	/* 19  | 10011  */
    { Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #20" },	/* 20  | 10100  */
    { Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #21" },	/* 21  | 10101  */
	{ Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #22" },	/* 22  | 10110  */
    { Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #23" },	/* 23  | 10111  */
    /* ------------------------------------------------------------------------------------------------ */
    { Test_Dummy_Fail,              false, false, false,  0,   0,	"Not Used #24" },	/* 24  | 11000  */
    { Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #25" },	/* 25  | 11001  */
    { Test_Dummy_Fail,      		false, false, false,  0,   0,	"Not Used #26" },	/* 26  | 11010  */
#ifdef __ADSP21479__
    { Set_RTC,      				false, true, false,   0,   0,	"RTC Set" },	    /* 27  | 11011  */
    { Check_RTC,	    			false, true, false,   0,   0,	"RTC Check" },	    /* 28  | 11100  */
#else
    { Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #27" },	/* 27  | 11011  */
    { Test_Dummy_Fail,				false, false, false,  0,   0,	"Not Used #28" },	/* 28  | 11100  */
#endif
	{ Display_Post_Status,	  		false, false, false,  0,   0,	"Display Status" },	/* 29  | 11101  */
    { Blink_FW_Version,				false, false, false,  0,   0,	"Display FW Ver" },	/* 30  | 11110  */
    { Toggle_Loop_Flag,				false, false, false,  0,   0,	"Toggle LoopFlag" }	/* 31  | 11111  */
    /* ------------------------------------------------------------------------------------------------ */
};


/* 32 different test states */
enTEST_STATES states[] = {TEST_0_SET, TEST_1_SET, TEST_2_SET, TEST_3_SET,
						  TEST_4_SET, TEST_5_SET, TEST_6_SET, TEST_7_SET,
						  TEST_8_SET, TEST_9_SET, TEST_10_SET, TEST_11_SET,
						  TEST_12_SET, TEST_13_SET, TEST_14_SET, TEST_15_SET,
						  TEST_16_SET, TEST_17_SET, TEST_18_SET, TEST_19_SET,
						  TEST_20_SET, TEST_21_SET, TEST_22_SET, TEST_23_SET,
						  TEST_24_SET, TEST_25_SET, TEST_26_SET, TEST_27_SET,
						  TEST_28_SET, TEST_29_SET, TEST_30_SET, TEST_31_SET };


/*******************************************************************
*   Function:    PerformTest
*   Description: This executes a specified test and displays status
*				 LEDs accordingly.
*******************************************************************/
int PerformTest( int nTestId )
{
	int nResult = 0;				/* result */
	int	nRound = 0;					/* which round does this test run in */
	int n = 0, m = 0;				/* indexes */
	int test_indicator;				/* test indicator */


	/* determine the test pattern */
	test_indicator = nTestId % NUM_STATUS_PATTERNS;
	if (0 == test_indicator)
	{
		if (0 != nTestId)
		{
			test_indicator = NUM_STATUS_PATTERNS;
		}
	}

	/* display test pattern */
	Delay(BLINK_SLOW * 25);
	ClearSet_LED_Bank( 0x0000 );
	ClearSet_LED_Bank( states[test_indicator] );


	if (nTestId < NUM_POST_TESTS)
	{
		nResult = g_Tests[nTestId].m_pTestFunction();	/* call test function */
	}
	else
	{
		nResult = 0; 									/* else fail, invalid test ID */
	}

    /* if test failed */
    if( 0 == nResult )
	{
		/* update count */
		g_Tests[nTestId].m_uiFailed++;

		/* display test stats */
		DEBUG_PRINT( "\n\nTest %d: passed %d  failed %d",	nTestId,
															g_Tests[nTestId].m_uiPassed,
															g_Tests[nTestId].m_uiFailed );

		/* determine which round this test runs in, for example
			with 3 LEDs tests 1 through 7 are in round 1,
			tests 8 through 14 are round 2 and so on, note that
			a pattern of all zeros (all OFF) is not used */
		nRound = nTestId / NUM_STATUS_PATTERNS;
		if ( nTestId % NUM_STATUS_PATTERNS )
			nRound += 1;

		/* blink forever or if we are to ignore failures we break out
			after a while */
		while (1)
		{
			/* we blink the test number once for each round so the user
				can determine which test failed since multiple tests use
				the same LED pattern such as tests 1, 8, 15, etc. */
			for ( n = 0; n < nRound; n++ )
			{
				ClearSet_LED_Bank( 0x0000);
				Delay(BLINK_SLOW * 3);
				ClearSet_LED_Bank( states[test_indicator]);
				Delay(BLINK_SLOW * 3);
			}

			/* long delay off after it blinks for each round */
			ClearSet_LED_Bank( 0x0000);
			Delay(BLINK_SLOW * 18);

			/* if we are to ignore failures only blink a handful
				times, then return */
			m++;
			if ( (10 == m) && (IGNORE_FAILURES == g_Tests[nTestId].m_bIgnoreFailures) )
				return nResult;
		}
	}
	/* test passed */
	else
	{
		/* update count */
		g_Tests[nTestId].m_uiPassed++;

		/* display test stats */
		DEBUG_PRINT( "\n\n%s: pass %d  fail %d", g_Tests[nTestId].m_pszName,
												g_Tests[nTestId].m_uiPassed,
												g_Tests[nTestId].m_uiFailed );
	}

	return nResult;
}


/*******************************************************************
*   Function:    main
*   Description: This is the main entry point where everything starts.
*******************************************************************/
void main(void)
{
    int nTestIndex = 0x0;				/* test index */
	unsigned int Result;				/* result */
	int i = 0;							/* index */
	bool bExit = false;					/* flag indicating we should exit */

	/* Initialize managed drivers and/or services */
	adi_initComponents();

#if defined(__DEBUG_FILE__)
    /* open the debug file */
    pDebugFile = fopen(__DEBUG_FILE_NAME__, "w");
    if (pDebugFile == 0)
    {
    	fclose(pDebugFile);
    	return;
    }
#endif

	/* init PLL, do this before UART, etc. */
	Init_PLL();

#if defined(__DEBUG_UART__)
    Init_UART();
#endif

    Init_SDRAM();
	Init_LEDs();

	/* See if the Watch Dog Timer test had started. Each time the timer times
	   out, the system gets reset and POST restarts. On the last timeout (as
	   programmed in pWDTTRIP, the system is held in reset until the hardware
	   reset button is pressed. This will clear the WDT registers and restart
	   POST. Note this is a destructive test, all test pass counts are cleared.
	*/
	if(*pWDTTRIP)
	{
		/* call Test_Watchdog_Timer();
		   call directly so the user can see the LEDs count up
		*/
		Test_Watchdog_Timer();

		while(1)
		{
			asm("nop;");
			asm("nop;");
			asm("nop;");
		}
	}

	/* display some debug info */
	DEBUG_HEADER( "21489 EZ-Board Power-On Self-Test" );
	DEBUG_PRINT( "\nBuilt on %s, at %s", __DATE__, __TIME__ );
	DEBUG_PRINT( "\nBuilt for rev %s EZ-Board", g_ezboard_rev );
	DEBUG_STATEMENT("\nConfigured PLL for 400 MHz CCLK, 160 MHz SDRAM clock");

	/* loop forever */
	while (!bExit)
	{
		gb_sw8_pushed = false;
		g_bIsTestChosen = false;
		g_nSelectedTest = 0;
		g_nPostState = STATE_START;

	    /* initialize pieces required to run POST */
	    DEBUG_STATEMENT( "\n" );
		Init_AMI();
		Init_LEDs();
		Init_PushButtons();

		/* blink the LEDs while waiting for user to enter test via pushbuttons */
		DEBUG_STATEMENT( "\n\nWaiting for a test to be selected..." );
#if defined(__DEBUG_CONSOLE__)
		DEBUG_STATEMENT( "\n" );
#endif
		ClearSet_LED_Bank( 0x0000 );
		Strobe_LEDs(0x0055, 0x5500, 0xe00, &g_bIsTestChosen);
		ClearSet_LED_Bank( 0x0000 );
		DEBUG_PRINT( "\nTest %d selected", g_nSelectedTest );

		/* make sure it's a valid test */
		if( g_nSelectedTest < NUM_POST_TESTS )
		{
			/* execute the test */
			do
			{
				g_bLoopFlagCheck = false;

				/*******************************************************
		   			run test
		   		*******************************************************/
				PerformTest( g_nSelectedTest );

				/* wait, then reset the LEDs */
				Delay(BLINK_SLOW * 25);

				/* indicate everything passed */
				ClearSet_LED_Bank( 0x0000 );

				if ( g_bLoopOnTest && !g_bLoopFlagCheck )
				{
					/* if we're looping, give the user time to break out of test */
					DEBUG_STATEMENT( "\n\n<<< Press SW8 to break out of the test loop >>>" );
				}

				/* blink a few times to indicate the test is complete before looping */
				for (i = 0; i < 4; i++)
				{
					LED_Bar( BLINK_SLOW );
				}

				/* was sw8 pushed?, this would break us out of the loop */
				if( gb_sw8_pushed )
				{
					DEBUG_STATEMENT( "\n\n<<< SW8 break out detected >>>" );
					gb_sw8_pushed = false;
					g_bLoopFlagCheck = true;
				}

			/* while we want to loop */
			} while ( g_bLoopOnTest && !g_bLoopFlagCheck );
		}
	}

	DEBUG_STATEMENT( "\nExiting POST" );

	#if defined(__DEBUG_FILE__)
	DEBUG_CLOSE();
	#endif
}

/*******************************************************************
*   Function:    Test_Standard_Loop
*   Description: Runs each test that is part of the standard loop.
*******************************************************************/
int Test_Standard_Loop( void )
{
	bool bLoop = true;			/* flag to breakout for debug */
	bool bFirst = true;			/* first time flag */
	unsigned int test = 0;		/* test index */
	int i = 0;					/* index */


	DEBUG_STATEMENT( "\nStandard loop tests selected..." );

	/* loop on these tests forever */
	while ( bLoop )
	{
		/* start at 1 (skip standard loop test) or we end up in an infinite loop */
		for( test = 1; (test < NUM_POST_TESTS) && bLoop; test++ )
		{
			/* if part of standard loop */
			if ( g_Tests[test].m_bStandardLoop )
			{
				/* if first pass or not a one-time test we can run it */
				if ( bFirst || !(g_Tests[test].m_bOneTime) )
				{
					/* run the test */
					PerformTest( test );
					Delay(BLINK_FAST * 25);

					/* indicate everything passed */
					ClearSet_LED_Bank( 0x0000 );

					/* give the user time to break out of test */
					DEBUG_STATEMENT( "\n\n<<< Press SW8 to break out of the test loop >>>" );

					/* blink a few times to indicate the test is complete before looping */
					for (i = 0; i < 4; i++)
					{
						LED_Bar( BLINK_SLOW );
					}
				}
			}

			/* was sw8 pushed?, this would break us out of the loop */
			if( gb_sw8_pushed )
			{
				DEBUG_STATEMENT( "\n\n<<< SW8 break out detected >>>" );
				gb_sw8_pushed = false;
				bLoop = false;
			}
		}

		/* update flag and count */
		bFirst = false;
		g_loops++;

		/* indicate everything passed */
		ClearSet_LED_Bank( 0x0000 );
		LED_Bar( BLINK_SLOW);
	}

	return 1;
}


/*******************************************************************
*   Function:    Toggle_Loop_Flag
*   Description: Toggles the loop flag which allows us to loop
*				 continuously on a test or not.
*******************************************************************/
int Toggle_Loop_Flag(void)
{
	g_bLoopFlagCheck = true;			/* set flag so we don't run this test more than once */
	g_bLoopOnTest = !g_bLoopOnTest;		/* toggle the state of the loop flag */

	DEBUG_PRINT( "\n\nTest loop flag state changed to %d", (int)g_bLoopOnTest );
	return 1;
}


/*******************************************************************
*   Function:    Display_Post_Status
*   Description: Prints out the status of the POST including pass and fail
*					counts for each test.
*******************************************************************/
int Display_Post_Status(void)
{
	int test = 0;

	DEBUG_STATEMENT( "\n\nPOST stats:\n" );

	DEBUG_STATEMENT( "\n\n     TEST NAME     PASS   FAIL   STD LOOP   ONE TIME   IGNORE" );
	DEBUG_STATEMENT( "\n==============================================================" );

	/* only show status for implemented tests */
	for ( test = 1; test < 14; test++ )
	{
		DEBUG_PRINT( "\n%16s   %3d    %3d        %d          %d         %d",
													g_Tests[test].m_pszName,
													g_Tests[test].m_uiPassed,
													g_Tests[test].m_uiFailed,
													(int)g_Tests[test].m_bStandardLoop,
													(int)g_Tests[test].m_bOneTime,
													(int)g_Tests[test].m_bIgnoreFailures );
	}

	return 1;
}


/*******************************************************************
*   Function:    Blink_FW_Version
*   Description: displays firmware version via LEDs
*******************************************************************/
int Blink_FW_Version( void )
{
	int n = 0;				/* index */

	DEBUG_HEADER( "Blink Firmware Version" );

	DEBUG_PRINT( "\nFirmware version: %d.%d", FIRMWARE_MAJOR_REV, FIRMWARE_MINOR_REV );
	DEBUG_PRINT( "\nBuilt on %s, at %s", __DATE__, __TIME__ );
	DEBUG_PRINT( "\nBuilt for rev %s EZ-Board", g_ezboard_rev );

	DEBUG_STATEMENT("\n\nThe major rev will blink on LED1");
	DEBUG_STATEMENT("\nThe minor rev will blink on LED2");

	/* blink the MAJOR rev on LED0 */
	for ( n = 0; n < FIRMWARE_MAJOR_REV; n++ )
	{
		ClearSet_LED_Bank( 0x0000 );
		Delay(BLINK_SLOW * 10);
		ClearSet_LED_Bank( TEST_1_SET );
		Delay(BLINK_SLOW * 10);
	}

	/* delay */
	ClearSet_LED_Bank( 0x0000 );
	Delay(BLINK_SLOW * 5);

	/* blink the MINOR rev on LED1 */
	for ( n = 0; n < FIRMWARE_MINOR_REV; n++ )
	{
		ClearSet_LED_Bank( 0x0000 );
		Delay(BLINK_SLOW * 10);
		ClearSet_LED_Bank( TEST_2_SET );
		Delay(BLINK_SLOW * 10);
	}

	/* blink a bar to indicate we're done showing both versions */
	ClearSet_LED_Bank( 0x0000 );

	return 1;
}


/*******************************************************************
*   Function:    Check_Config_Data
*   Description: Performs a check on the configuration data stored in parallel flash.
*				 This test is not implemented at this time. Return as "pass" temporary
*******************************************************************/
int Check_Config_Data(void)
{
	unsigned char *cfgdata = (unsigned char*) 0x43f0000;		 /* start of configuration data */
	unsigned char EzProj[7] = {'A', 'A', '0', '2', '2', '8', 0}; /* project number for this board */
	unsigned char current[7] = { 0, 0, 0, 0, 0, 0, 0 };			 /* place to store project number */
	int nResult = 1;					/* status flag */
	int i = 0;							/* index */
	int project_offset = 48;			/* project number offset */

	DEBUG_HEADER( "\nCheck Configuration Data" );
#if 0
	/* setup parallel flash where config data is stored */
	Init_ParallelFlash();

	/* store the project number */
	for ( i = 0; i < 6; i++ )
	{
		current[i] = cfgdata[project_offset + i];
	}

	/* make sure it's programmed as the correct board */
	for ( i = 0; i < 6; i++ )
	{
		if( current[i] != EzProj[i] )
		{
			nResult = 0;
			break;
		}
	}

	DEBUG_PRINT( "\nExpected project %s, detected project %s", EzProj, current );
#endif

	DEBUG_RESULT( nResult, "\nTest passed", "\nTest failed" );

	return nResult;
}


