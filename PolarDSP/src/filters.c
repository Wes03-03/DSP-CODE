/*
 * filters.c
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#include "filters.h"
#include <math.h>

void InitHighPass(BiquadFilter* hp, float freq, float sampleRate) {
    float omega = 2.0f * 3.14159265f * freq / sampleRate;
    float sn = sinf(omega);
    float cs = cosf(omega);
    float alpha = sn / (2.0f * 0.707f);

    hp->b0 = (1.0f + cs) / 2.0f;
    hp->b1 = -(1.0f + cs);
    hp->b2 = hp->b0;
    hp->a1 = -2.0f * cs;
    hp->a2 = 1.0f - alpha;

    float a0 = 1.0f + alpha;
    hp->b0 /= a0; hp->b1 /= a0; hp->b2 /= a0;
    hp->a1 /= a0; hp->a2 /= a0;

    hp->x1 = hp->x2 = hp->y1 = hp->y2 = 0.0f;
}

float ProcessBiquad(BiquadFilter* filter, float input) {
    float output = filter->b0 * input + filter->b1 * filter->x1
                 + filter->b2 * filter->x2 - filter->a1 * filter->y1
                 - filter->a2 * filter->y2;

    filter->x2 = filter->x1;
    filter->x1 = input;
    filter->y2 = filter->y1;
    filter->y1 = output;

    return output;
}
