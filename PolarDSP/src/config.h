/*
 * config.h
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* System Configuration */
#define SAMPLE_RATE         48000
#define AUDIO_BUFFER_SIZE   256
#define NUM_AUDIO_CHANNELS  2

/* Distortion Defaults */
#define DEFAULT_GAIN        5.0f
#define DEFAULT_THRESHOLD   0.3f
#define DEFAULT_MIX         0.7f
#define DEFAULT_TYPE        0

/* Guitar Specific */
#define GUITAR_INPUT_GAIN   5.0f
#define LOW_CUT_FREQ        80.0f
#define HIGH_CUT_FREQ      5000.0f

#endif
