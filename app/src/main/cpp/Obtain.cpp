#include "Obtain.h"
#include "logging_macros.h"
#include "ffts.h"
//#include <complex.h>

void obtain::Obtain::processData(float *audioData, int32_t numSamples) {
    // only fill a fresh HopSize to keep 87.5% overlap with the previous runs
    // data that we can't/won't process this iteration goes into sampleAccumulator[WindowSize:]
    std::copy(audioData, audioData+numSamples, sampleAccumulator.begin()+WindowSize-HopSize);

    LOGI("OBTAIN process Data, len: %ld", sampleAccumulator.size());

    // OBTAIN processing code here
    // the data we care about is in sampleAccumulator[0:WindowSize], while [WindowSize:] contains
    // leftover samples to be computed in the next window

    // apply FFT on each window
    ffts_execute(fftPlan, sampleAccumulator.data(), fftResult.data()); // causing crashes

    // process it and then shift the array HopSize to the left
    // this should create a rotating buffer of WindowSize - a few initial runs will have incomplete
    // data but this shouldn't be a big deal
    std::copy(sampleAccumulator.begin()+HopSize, sampleAccumulator.end(), sampleAccumulator.begin());
}

void obtain::Obtain::~Obtain() {
    ffts_free(fftPlan);
}
