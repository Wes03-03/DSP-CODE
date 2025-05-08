/*
 * distortion.h
 *
 *  Created on: Apr 2, 2025
 *      Author: wesle
 */

#ifndef DISTORTION_H_
#define DISTORTION_H_

typedef enum {
    SOFT_CLIP,
    HARD_CLIP,
    ARCTAN,
    FOLDBACK
} DistortionType;

typedef struct {
    float gain;
    float threshold;
    float mix;
    DistortionType type;
} DistortionParams;

void InitDistortion(DistortionParams *params);
float ApplyDistortion(float sample, const DistortionParams *params);

#endif
