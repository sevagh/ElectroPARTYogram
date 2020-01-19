#ifndef ANIMALS_AS_METER_BEATTRACKER_H
#define ANIMALS_AS_METER_BEATTRACKER_H

#include <oboe/Definitions.h>
#include <vector>
#include <complex>
#include <atomic>
#include "NE10.h"
#include "CircularBuffer.h"
#include "OnsetDetection.h"

/*
 * A re-implementation of https://github.com/adamstark/BTrack
 */
namespace btrack {

class BeatTracker {

private:
    static constexpr int HopSize = 512;
    static constexpr size_t OnsetDFBufferSize = 512;

    int32_t sampleRate;
    int32_t nWritten;

    circbuf::CircularBuffer onsetDF;
    circbuf::CircularBuffer cumulativeScore;

    onset::OnsetDetectionFunction odf;

    std::atomic_bool currentFrameProcessed;

    std::vector<float> sampleAccumulator;

    void processCurrentFrame();

public:
    static constexpr int FrameSize = 1024;

    BeatTracker(int32_t sampleRate) :
        sampleRate(sampleRate),
        sampleAccumulator(FrameSize),
        currentFrameProcessed(true), // start this off at true as a seed value
        onsetDF(circbuf::CircularBuffer(OnsetDFBufferSize)),
        cumulativeScore(circbuf::CircularBuffer(OnsetDFBufferSize)),
        odf(onset::OnsetDetectionFunction(HopSize, FrameSize))
        {};

    ~BeatTracker();

    void accumulateFrame(float *audioData, int32_t numSamples);
};
}

#endif //ANIMALS_AS_METER_BEATTRACKER_H
