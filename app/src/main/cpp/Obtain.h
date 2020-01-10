#ifndef ANIMALS_AS_METER_OBTAIN_H
#define ANIMALS_AS_METER_OBTAIN_H

#include "HammingWindow.h"
#include <oboe/Definitions.h>

/*
 * A from-scratch implementation of the OBTAIN beat tracking algorithm
 *
 * From paper "Real-time Beat Tracking in Audio Signals"
 */
namespace obtain {
class Obtain {

private:
    constexpr int WindowSize = 1024;
    constexpr int HopSize = 128; // creates an overlap ratio of 87.5 i.e. (1024-128)/1024
    constexpr int NoiseCancellationThreshold = 74; // threshold values below 74dB to 0
    constexpr unsigned long HammingWindowLength = 15;

    //const int HammingWindowCutoffFrequency = 7;
    HammingWindow hammingWindow;
    int iSampleRate; // initial audio sample rate
    int eSampleRate; // effective sample rate i.e. iSampleRate/WindowSize

public:
    Obtain(int32_t sampleRate) :
        iSampleRate(sampleRate),
        eSampleRate(sampleRate/WindowSize),
        hammingWindow(HammingWindow(HammingWindowLength))
    {}
};
}

#endif //ANIMALS_AS_METER_OBTAIN_H
