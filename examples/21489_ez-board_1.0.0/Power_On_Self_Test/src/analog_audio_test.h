/*******************************************************************************/
/*                                                                             */
/*   (C) Copyright 2009 - Analog Devices, Inc.  All rights reserved.           */
/*                                                                             */
/*    FILE:     analog_audio_test.h ( )						                   */
/*                                                                             */
/*																			   */
/*******************************************************************************/

#ifndef _ANALOG_AUDIO_TEST_H_
#define _ANALOG_AUDIO_TEST_H_


#ifdef __ECC__

#ifdef __ADSP21489__
#include <cdef21489.h>
#include <def21489.h>
#elif __ADSP21479__
#include <cdef21479.h>
#include <def21479.h>
#endif
#include <signal.h>


typedef enum {
	I2S_48_KHZ_SAMPLE_RATE,			// All channels are ON
	I2S_48_KHZ_SAMPLE_RATE_OUT12,	// channel 1 and 2 are ON, 3 and 4 are OFF
	I2S_48_KHZ_SAMPLE_RATE_OUT34,	// channel 1 and 2 are OFF, 3 and 4 are ON
	I2S_96_KHZ_SAMPLE_RATE,
	I2S_96_KHZ_SAMPLE_RATE_OUT12,
	I2S_96_KHZ_SAMPLE_RATE_OUT34,
	I2S_192_KHZ_SAMPLE_RATE,
	I2S_192_KHZ_SAMPLE_RATE_OUT12,
	I2S_192_KHZ_SAMPLE_RATE_OUT34,
	TDM_48_KHZ_SAMPLE_RATE,
	TDM_48_KHZ_SAMPLE_RATE_OUT12,
	TDM_48_KHZ_SAMPLE_RATE_OUT34,
	TDM_96_KHZ_SAMPLE_RATE,
	TDM_96_KHZ_SAMPLE_RATE_OUT12,
	TDM_96_KHZ_SAMPLE_RATE_OUT34,
	TDM_192_KHZ_SAMPLE_RATE,
	TDM_192_KHZ_SAMPLE_RATE_OUT12,
	TDM_192_KHZ_SAMPLE_RATE_OUT34,
	EXTA_TDM_192_KHZ_SAMPLE_RATE_OUT,
	EXTA_TDM_192_KHZ_SAMPLE_RATE_OUT12,
	EXTA_TDM_192_KHZ_SAMPLE_RATE_OUT34,
	EXTB_TDM_192_KHZ_SAMPLE_RATE_OUT,
	EXTB_TDM_192_KHZ_SAMPLE_RATE_OUT12,
	EXTB_TDM_192_KHZ_SAMPLE_RATE_OUT34,
	EXTC_TDM_192_KHZ_SAMPLE_RATE_OUT,
	EXTC_TDM_192_KHZ_SAMPLE_RATE_OUT12,
	EXTC_TDM_192_KHZ_SAMPLE_RATE_OUT34,
	EXTB_CODEC_OFF,
	EXTC_CODEC_OFF
}MODE_SAMPLE_RATE;

typedef enum {
	extender_codecA,
	extender_codecB,
	extender_codecC
}EXTENDER_CODEC;

#define AUDIO_MAX_SAMPLES	 			4096
#define AUDIO_REQUIRED_SAMPLES				((AUDIO_MAX_SAMPLES) * 100)
#define ACCEPTABLE_DEVIATION_PCT		((float)0.015)
///////////////////////////////////////
//I2S definitions
//////////////////////////////////////



// 48 khz sample rate
#define SAMPLE_RATE_48KHZ 						((float)48000.0)
#define DESIRED_FREQ1_I2S_48_OUT12 				((float)451.0)
#define DESIRED_FREQ2_I2S_48_OUT12				((float)561.0)
#define MAX_SIGNAL_STRENGTH1_I2S_48_OUT12		(float)270.0
#define MAX_SIGNAL_STRENGTH2_I2S_48_OUT12		(float)850.0
#define DESIRED_FREQ1_I2S_48_OUT34 				((float)671.0)
#define DESIRED_FREQ2_I2S_48_OUT34				((float)902.0)
#define MAX_SIGNAL_STRENGTH1_I2S_48_OUT34		(float)290.0
#define MAX_SIGNAL_STRENGTH2_I2S_48_OUT34		(float)500.0

// 96 khz sample rate
#define SAMPLE_RATE_96KHZ 						((float)96000.0)

// 192 kHz sample rate
#define SAMPLE_RATE_192KHZ 						((float)192000.0)
#define DESIRED_FREQ1_I2S_192_OUT12 			((float)1886.0)
#define DESIRED_FREQ2_I2S_192_OUT12 			((float)2346.0)
#define MAX_SIGNAL_STRENGTH1_I2S_192_OUT12		(float)650.0
#define MAX_SIGNAL_STRENGTH2_I2S_192_OUT12		(float)500.0
#define DESIRED_FREQ1_I2S_192_OUT34 			((float)2806.0)
#define DESIRED_FREQ2_I2S_192_OUT34 			((float)3772.0)
#define MAX_SIGNAL_STRENGTH1_I2S_192_OUT34		(float)600.0
#define MAX_SIGNAL_STRENGTH2_I2S_192_OUT34		(float)900.0

//TDM
// 48 khz sample rate
#define DESIRED_FREQ1_TDM_48_OUT12 				((float)451.0)
#define DESIRED_FREQ2_TDM_48_OUT12 				((float)561.0)
#define MAX_SIGNAL_STRENGTH1_TDM_48_OUT12		(float)250.0
#define MAX_SIGNAL_STRENGTH2_TDM_48_OUT12		(float)561.0

#define DESIRED_FREQ1_TDM_48_OUT34 				((float)671.0)
#define DESIRED_FREQ2_TDM_48_OUT34 				((float)902.0)
#define MAX_SIGNAL_STRENGTH1_TDM_48_OUT34		(float)230.0
#define MAX_SIGNAL_STRENGTH2_TDM_48_OUT34		(float)440.0


// 192 kHz sample rate
#define DESIRED_FREQ1_TDM_192_OUT12 			((float)1886.0)
#define DESIRED_FREQ2_TDM_192_OUT12 			((float)2346.0)
#define MAX_SIGNAL_STRENGTH1_TDM_192_OUT12		(float)600.0
#define MAX_SIGNAL_STRENGTH2_TDM_192_OUT12		(float)460.0

#define DESIRED_FREQ1_TDM_192_OUT34 			((float)2806.0)
#define DESIRED_FREQ2_TDM_192_OUT34 			((float)3772.0)
#define MAX_SIGNAL_STRENGTH1_TDM_192_OUT34		(float)600.0
#define MAX_SIGNAL_STRENGTH2_TDM_192_OUT34		(float)900.0

//Sharc audio extender board
//
#define EXTA_DESIRED_FREQ1_TDM_192_OUT12 			((float)46.0)
#define EXTA_DESIRED_FREQ2_TDM_192_OUT12 			((float)92.0)
#define EXTA_MAX_SIGNAL_STRENGTH1_TDM_192_OUT12		(float)800.0
#define EXTA_MAX_SIGNAL_STRENGTH2_TDM_192_OUT12		(float)1200.0

#define EXTA_DESIRED_FREQ1_TDM_192_OUT34 			((float)138.0)
#define EXTA_DESIRED_FREQ2_TDM_192_OUT34 			((float)184.0)
#define EXTA_MAX_SIGNAL_STRENGTH1_TDM_192_OUT34		(float)1000.0
#define EXTA_MAX_SIGNAL_STRENGTH2_TDM_192_OUT34		(float)800.0

#define EXTB_DESIRED_FREQ1_TDM_192_OUT12 			((float)460.0)
#define EXTB_DESIRED_FREQ2_TDM_192_OUT12 			((float)920.0)
#define EXTB_MAX_SIGNAL_STRENGTH1_TDM_192_OUT12		(float)600.0
#define EXTB_MAX_SIGNAL_STRENGTH2_TDM_192_OUT12		(float)300.0

#define EXTB_DESIRED_FREQ1_TDM_192_OUT34 			((float)1426.0)
#define EXTB_DESIRED_FREQ2_TDM_192_OUT34 			((float)1886.0)
#define EXTB_MAX_SIGNAL_STRENGTH1_TDM_192_OUT34		(float)900.0
#define EXTB_MAX_SIGNAL_STRENGTH2_TDM_192_OUT34		(float)800.0

#define EXTC_DESIRED_FREQ1_TDM_192_OUT12 			((float)2346.0)
#define EXTC_DESIRED_FREQ2_TDM_192_OUT12 			((float)2806.0)
#define EXTC_MAX_SIGNAL_STRENGTH1_TDM_192_OUT12		(float)500.0
#define EXTC_MAX_SIGNAL_STRENGTH2_TDM_192_OUT12		(float)600.0

#define EXTC_DESIRED_FREQ1_TDM_192_OUT34 			((float)3312.0)
#define EXTC_DESIRED_FREQ2_TDM_192_OUT34 			((float)3772.0)
#define EXTC_MAX_SIGNAL_STRENGTH1_TDM_192_OUT34		(float)200.0
#define EXTC_MAX_SIGNAL_STRENGTH2_TDM_192_OUT34		(float)900.0


#define EXTB_DESIRED_FREQ1_TDM_192_OUT34_DIFF 			((float)138.0)
#define EXTB_DESIRED_FREQ2_TDM_192_OUT34_DIFF 			((float)184.0)
#define EXTB_MAX_SIGNAL_STRENGTH1_TDM_192_OUT34_DIFF	(float)1400.0
#define EXTB_MAX_SIGNAL_STRENGTH2_TDM_192_OUT34_DIFF	(float)1000.0


#define EXTC_DESIRED_FREQ1_TDM_192_OUT12_DIFF 			((float)460.0)
#define EXTC_DESIRED_FREQ2_TDM_192_OUT12_DIFF 			((float)920.0)
#define EXTC_MAX_SIGNAL_STRENGTH1_TDM_192_OUT12_DIFF	(float)1500.0
#define EXTC_MAX_SIGNAL_STRENGTH2_TDM_192_OUT12_DIFF	(float)400.0

/* global variable definitions */

extern void Delay (int i);
void	SinTableInit(void);
void	SinTableInit_Ext(void);
extern int Test_Channel(float* pfRealIn, const int iMaxSamples, const int iSampleRate,
					const float fExpFreq, const float fFreqTol, const float fExpAmp,
					const float fAmpTol);
void Clear_DAI_Pins(void);

void	Receive_ADC_Samples_I2S(void);
void	Transmit_DAC_Samples_I2S(void);
void	Receive_ADC_Samples_TDM(void);
void	Transmit_DAC_Samples_TDM(void);
void 	Receive_ADC_Samples_TDM_4896KHZ(void);
void 	Transmit_DAC_Samples_TDM_4896KHZ(void);
void	Receive_ADC_Samples_TDM_4896KHZ(void);
void 	Transmit_DAC_Samples_TDM_4896KHZ(void);
void	Receive_ADC_Samples_Ext_TDM(void);
void	Transmit_DAC_Samples_Ext_TDM(void);



extern volatile int  g_iSampleCount;
extern volatile int  g_iSampleIndex;

extern volatile float g_fSineWaveIn1_Left[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveIn1_Right[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveIn2_Left[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveIn2_Right[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveInB1_Left[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveInB1_Right[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveInB2_Left[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveInB2_Right[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveInC1_Left[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveInC1_Right[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveInC2_Left[AUDIO_MAX_SAMPLES];
extern volatile float g_fSineWaveInC2_Right[AUDIO_MAX_SAMPLES];



//extern float Sine_Table[4000];
//extern int Sine_Index;

//extern float * DelayLine;
//extern int Index;

/* input audio channels */
/* input values from the AD1939 internal stereo ADCs */
extern 	float			Left_Channel_In1;
extern 	float 			Right_Channel_In1;
extern	float			Left_Channel_In2;
extern  float			Right_Channel_In2;
extern 	float			Left_Channel_In3;       /* Input values from the AD1939 internal stereo ADCs */
extern 	float 			Right_Channel_In3;
extern	float			Left_Channel_In4;
extern  float			Right_Channel_In4;
extern 	float			Left_Channel_In5;       /* Input values from the AD1939 internal stereo ADCs */
extern 	float 			Right_Channel_In5;
extern	float			Left_Channel_In6;
extern  float			Right_Channel_In6;

/* output audio channels */
/* output values for the 4 AD1939 internal stereo DACs */
extern 	float			Left_Channel_Out1;
extern 	float 			Left_Channel_Out2;
extern 	float 			Left_Channel_Out3;
extern 	float 			Left_Channel_Out4;
extern 	float			Right_Channel_Out1;
extern 	float			Right_Channel_Out2;
extern 	float			Right_Channel_Out3;
extern 	float			Right_Channel_Out4;
extern 	float			Left_Channel_Out5;      /* Output values for the 4 AD1939 internal stereo DACs */
extern 	float 			Left_Channel_Out6;
extern 	float 			Left_Channel_Out7;
extern 	float 			Left_Channel_Out8;
extern 	float			Right_Channel_Out5;
extern 	float			Right_Channel_Out6;
extern 	float			Right_Channel_Out7;
extern 	float			Right_Channel_Out8;
extern 	float			Left_Channel_Out9;      /* Output values for the 4 AD1939 internal stereo DACs */
extern 	float 			Left_Channel_Out10;
extern 	float 			Left_Channel_Out11;
extern 	float 			Left_Channel_Out12;
extern 	float			Right_Channel_Out9;
extern 	float			Right_Channel_Out10;
extern 	float			Right_Channel_Out11;
extern 	float			Right_Channel_Out12;


#define SPIB_MODE (CPHASE | CLKPL)
#define AD1939_CS DS0EN
/* #define AD1939_CS DS1EN */

#define SELECT_SPI_SLAVE(select) (*pSPIFLG &= ~(spiselect<<8))
#define DESELECT_SPI_SLAVE(select) (*pSPIFLG |= (spiselect<<8))
//////////////////////////////////////////////////////
// I2S definitions
//
//////////////////////////////////////////////////////




//////////////////////////////////////////////////////
// TDM definitions
//
//////////////////////////////////////////////////////


#else

/* insert assembly definitions here.... */

#endif

/* AD1939 I2S Timeslot Definitions */
#define     I2S_Internal_ADC_L1    0
#define     I2S_Internal_ADC_R1    1
#define     I2S_Internal_ADC_L2    0
#define     I2S_Internal_ADC_R2    1

#define     I2S_Internal_DAC_L1    0
#define     I2S_Internal_DAC_R1    1
#define     I2S_Internal_DAC_L2    0
#define     I2S_Internal_DAC_R2    1
#define     I2S_Internal_DAC_L3    0
#define     I2S_Internal_DAC_R3    1
#define     I2S_Internal_DAC_L4    0
#define     I2S_Internal_DAC_R4    1

/* AD1939 TDM Timeslot Definitions for Dual-Line 192 kHz DAC TDM Mode */
#define     TDM_Internal_ADC_L1    0
#define     TDM_Internal_ADC_R1    1
#define     TDM_Internal_ADC_L2    2
#define     TDM_Internal_ADC_R2    3

#define     TDM_Internal_DAC_L1    0
#define     TDM_Internal_DAC_R1    1
#define     TDM_Internal_DAC_L2    2
#define     TDM_Internal_DAC_R2    3
#define     TDM_192_Internal_DAC_L3    0
#define     TDM_192_Internal_DAC_R3    1
#define     TDM_192_Internal_DAC_L4    2
#define     TDM_192_Internal_DAC_R4    3

/* TDM 48-96 kHz DAC mode */
#define     TDM_4896_Internal_DAC_L3    4
#define     TDM_4896_Internal_DAC_R3    5
#define     TDM_4896_Internal_DAC_L4    6
#define     TDM_4896_Internal_DAC_R4    7

// used by Sharc audio extender test
/* AD1939 TDM Timeslot Definitions for Dual-Line 192 kHz DAC TDM Mode */
#define     Internal_ADC_L1    0
#define     Internal_ADC_R1    1
#define     Internal_ADC_L2    2
#define     Internal_ADC_R2    3

#define     Internal_ADC_L3    0
#define     Internal_ADC_R3    1
#define     Internal_ADC_L4    2
#define     Internal_ADC_R4    3

#define     Internal_ADC_L5    0
#define     Internal_ADC_R5    1
#define     Internal_ADC_L6    2
#define     Internal_ADC_R6    3


#define     Internal_DAC_L1    0
#define     Internal_DAC_R1    1
#define     Internal_DAC_L2    2
#define     Internal_DAC_R2    3
#define     Internal_DAC_L3    0
#define     Internal_DAC_R3    1
#define     Internal_DAC_L4    2
#define     Internal_DAC_R4    3

#define     Internal_DAC_L5    0
#define     Internal_DAC_R5    1
#define     Internal_DAC_L6    2
#define     Internal_DAC_R6    3
#define     Internal_DAC_L7    0
#define     Internal_DAC_R7    1
#define     Internal_DAC_L8    2
#define     Internal_DAC_R8    3

#define     Internal_DAC_L9    0
#define     Internal_DAC_R9    1
#define     Internal_DAC_L10   2
#define     Internal_DAC_R10   3
#define     Internal_DAC_L11   0
#define     Internal_DAC_R11   1
#define     Internal_DAC_L12   2
#define     Internal_DAC_R12   3

#endif /* _ANALOG_AUDIO_TEST_H_ */
