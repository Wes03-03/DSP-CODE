/*****************************************************************************
 * PolarDSP.c
 *****************************************************************************/

#include "ad1939.h"
#include "audio_io.h"
#include "distortion.h"
#include "hardware_platform.h"

DistortionParams distortionParams;

int main(void) {
    Init_EZKIT();
    AD1939_Init();
    Init_I2S();

    InitDistortion(&distortionParams);
    distortionParams.gain = DEFAULT_GAIN;
    distortionParams.threshold = DEFAULT_THRESHOLD;
    distortionParams.mix = DEFAULT_MIX;
    distortionParams.type = DEFAULT_TYPE;

    Init_Interrupts();

    while(1) {
        /* Processing happens in interrupts */
    }

    return 0;
}
