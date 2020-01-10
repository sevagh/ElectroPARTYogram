#ifndef ANIMALS_AS_METER_OBTAIN_H
#define ANIMALS_AS_METER_OBTAIN_H

#include <oboe/Definitions.h>
#include <vector>
#include <complex>
#include <ffts.h>

/*
 * A from-scratch implementation of the OBTAIN beat tracking algorithm
 *
 * From paper "Real-time Beat Tracking in Audio Signals"
 */
namespace obtain {
// precomputed filter coefficients from numpy, L = 15 as described in the paper
static constexpr float HammingCoefficients[15] = {
        0.08000000000000002f,
        0.4376403703800954f,
        0.1255543207648872f,
        0.6423596296199047f,
        0.25319469114498255f,
        0.8268053088550175f,
        0.9544456792351128f,
        1.0f,
        0.8268053088550176f,
        0.6423596296199048f,
        0.25319469114498266f,
        0.1255543207648871f,
        0.4376403703800959f};

class Obtain {

private:
    static constexpr int HopSize = 128; // creates an overlap ratio of 87.5 i.e. (1024-128)/1024
    static constexpr int NoiseCancellationThreshold = 74; // threshold values below 74dB to 0
    std::vector<float> sampleAccumulator;
    std::vector<std::complex<float>> fftResult;

    ffts_plan_t* fftPlan  = ffts_init_1d_real(WindowSize, FFTS_FORWARD);

    int iSampleRate; // initial audio sample rate
    int eSampleRate; // effective sample rate i.e. iSampleRate/WindowSize

    ~Obtain();

public:
    static constexpr int WindowSize = 1024;

    Obtain(int32_t sampleRate) :
        iSampleRate(sampleRate),
        eSampleRate(sampleRate/WindowSize),
        sampleAccumulator(2*WindowSize),
        fftResult(WindowSize/2 + 1) // https://github.com/sevagh/ffts/blob/master/include/ffts.h#L88
            {}

    void processData(float *audioData, int32_t numSamples);
};
}

#endif //ANIMALS_AS_METER_OBTAIN_H
