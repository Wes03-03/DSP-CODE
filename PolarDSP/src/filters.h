/*
 * filters.h
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#ifndef FILTERS_H_
#define FILTERS_H_

typedef struct {
    float b0, b1, b2, a1, a2;
    float x1, x2, y1, y2;
} BiquadFilter;

void InitHighPass(BiquadFilter* hp, float freq, float sampleRate);
void InitLowPass(BiquadFilter* lp, float freq, float sampleRate);
float ProcessBiquad(BiquadFilter* filter, float input);

#endif
