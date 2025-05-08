/*
 * audio_io.c
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#include "audio_io.h"
#include "distortion.h"
#include "filters.h"
#include "config.h"
#include <cdef21489.h>

static AudioBuffer inputBuffer, outputBuffer;
static BiquadFilter hpFilter, lpFilter;

void Init_I2S(void) {
    *pSPCTL0 = 0x0F31 | (0x17 << 5);
    *pDIV0 = 0x000F;
    *pFSDIV0 = 0x00FF;

    InitHighPass(&hpFilter, LOW_CUT_FREQ, SAMPLE_RATE);
    InitLowPass(&lpFilter, HIGH_CUT_FREQ, SAMPLE_RATE);
}

section("seg_int_code")
void I2S_ISR(void) {
    static int pos = 0;

    inputBuffer.left[pos] = sport0_rx() << 8;
    inputBuffer.right[pos] = sport0_rx() << 8;

    if(++pos >= AUDIO_BUFFER_SIZE) {
        pos = 0;
        ProcessAudioBlock(&inputBuffer, &outputBuffer);

        for(int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
            sport0_tx(outputBuffer.left[i] >> 8);
            sport0_tx(outputBuffer.right[i] >> 8);
        }
    }
}

void ProcessAudioBlock(AudioBuffer *input, AudioBuffer *output) {
    for(int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
        float inL = (float)input->left[i] / 8388608.0f * GUITAR_INPUT_GAIN;
        float inR = (float)input->right[i] / 8388608.0f * GUITAR_INPUT_GAIN;

        inL = ProcessBiquad(&hpFilter, inL);
        inL = ProcessBiquad(&lpFilter, inL);

        output->left[i] = (int32_t)(inL * 8388607.0f);
        output->right[i] = (int32_t)(inR * 8388607.0f);
    }
}
