#ifndef ANIMALS_AS_METER_BEATTRACKER_H
#define ANIMALS_AS_METER_BEATTRACKER_H

#include <oboe/Definitions.h>
#include <vector>
#include <complex>
#include "NE10.h"

/*
 * A re-implementation of https://github.com/adamstark/BTrack
 */
namespace obtain {

class BeatTracker {

private:
    static constexpr int HopSize = 128; // creates an overlap ratio of 87.5 i.e. (1024-128)/1024
    static constexpr float NoiseCancellationThreshold_dB = 74.0; // threshold values below 74dB to 0

    int iSampleRate; // initial audio sample rate
    int eSampleRate; // effective sample rate i.e. iSampleRate/WindowSize

    std::vector<float> sampleAccumulator;
    std::vector<ne10_fft_cpx_float32_t> fftResult;
    std::vector<float> fftResultMags;

    ne10_fft_r2c_cfg_float32_t fftCfg;

public:
    static constexpr int WindowSize = 1024;

    BeatTracker(int32_t sampleRate) :
        iSampleRate(sampleRate),
        eSampleRate(sampleRate/WindowSize),
        sampleAccumulator(2*WindowSize),
        fftResult(WindowSize/2 + 1),
        fftResultMags(WindowSize/2 + 1),
        fftCfg(ne10_fft_alloc_r2c_float32(WindowSize))
            {}

    ~BeatTracker();

    void processData(float *audioData, int32_t numSamples);
};
}

#endif //ANIMALS_AS_METER_BEATTRACKER_H
