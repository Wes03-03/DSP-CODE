/*
 * distortion.c
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#include "distortion.h"
#include <math.h>

void InitDistortion(DistortionParams *params) {
    params->gain = 1.0f;
    params->threshold = 0.5f;
    params->mix = 0.5f;
    params->type = SOFT_CLIP;
}

float ApplyDistortion(float sample, const DistortionParams *params) {
    float distorted = sample * params->gain;

    switch(params->type) {
        case HARD_CLIP:
            if(distorted > params->threshold) distorted = params->threshold;
            else if(distorted < -params->threshold) distorted = -params->threshold;
            break;

        case ARCTAN:
            distorted = atanf(distorted * 3.0f) * 0.6366f;
            break;

        case FOLDBACK:
            if(fabsf(distorted) > 1.0f) {
                distorted = fabsf(fabsf(fmodf(distorted - params->threshold, 4.0f * params->threshold))
                        - 2.0f * params->threshold) - params->threshold;
            }
            break;

        case SOFT_CLIP:
        default:
            if(distorted > params->threshold) {
                distorted = params->threshold + (distorted - params->threshold) /
                          (1.0f + ((distorted - params->threshold) / (1.0f - params->threshold)));
            }
            else if(distorted < -params->threshold) {
                distorted = -params->threshold + (distorted + params->threshold) /
                          (1.0f + ((-distorted - params->threshold) / (1.0f - params->threshold)));
            }
    }

    return (sample * (1.0f - params->mix)) + (distorted * params->mix);
}
