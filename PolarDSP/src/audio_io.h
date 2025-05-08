/*
 * audio_io.h
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#ifndef AUDIO_IO_H_
#define AUDIO_IO_H_

#include "config.h"
#include "stdint.h"

typedef struct {
    int32_t left[AUDIO_BUFFER_SIZE];
    int32_t right[AUDIO_BUFFER_SIZE];
} AudioBuffer;

void Init_I2S(void);
void ProcessAudioBlock(AudioBuffer *input, AudioBuffer *output);
void I2S_ISR(void);

#endif
