#include "Obtain.h"
#include "logging_macros.h"
#include "NE10.h"
#include <algorithm>
#include "MathNeon.h"

void obtain::Obtain::processData(float *audioData, int32_t numSamples) {
    // only fill a fresh HopSize to keep 87.5% overlap with the previous runs
    // data that we can't/won't process this iteration goes into sampleAccumulator[WindowSize:]
    std::copy(audioData, audioData+numSamples, sampleAccumulator.begin()+WindowSize-HopSize);

    LOGI("OBTAIN: process Data, len: %ld", sampleAccumulator.size());

    // OBTAIN processing code here

    // the window we care about is in sampleAccumulator[0:WindowSize], while [WindowSize:] contains
    // leftover samples to be computed in the next window

    // apply FFT on each window
    ne10_fft_r2c_1d_float32_neon(fftResult.data(), sampleAccumulator.data(), fftCfg);

    math_neon::Complex2Magnitude(fftResult, fftResultMags);

    // normalize the FFT
    math_neon::NormalizeByMax(fftResultMags);

    // after processing, shift the array HopSize to the left
    // this should create a rotating buffer of WindowSize - a few initial runs will have incomplete
    // data but this shouldn't be a big deal
    std::copy(sampleAccumulator.begin()+HopSize, sampleAccumulator.end(), sampleAccumulator.begin());
}

obtain::Obtain::~Obtain() {
    ne10_fft_destroy_r2c_float32(fftCfg);
}
