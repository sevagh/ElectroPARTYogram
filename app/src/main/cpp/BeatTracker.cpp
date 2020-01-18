#include "BeatTracker.h"
#include "logging_macros.h"
#include "NE10.h"
#include <algorithm>
#include "MathNeon.h"

void btrack::BeatTracker::accumulateFrame(float *audioData, int32_t numSamples) {
    // if we're done, continue on to processing in the background
    if (currentFrameProcessed) {
        LOGI("BeatTracker: processing complete frame");
        // copy data into sampleAccumulator
        std::copy(audioData, audioData + numSamples, sampleAccumulator.begin());
        nWritten = numSamples;
        processCurrentFrame();
        return;
    }
    // if we're still processing the previous thread, we no choice but to lose this data?
    LOGI("BeatTracker: dropping %d samples", numSamples);
}

void btrack::BeatTracker::processCurrentFrame() {
    // in the beginning, mark as not done yet
    currentFrameProcessed = false;

    // do all the BeatTracking here

    // at the end, shift samples to the right
    std::copy(sampleAccumulator.begin(), sampleAccumulator.end() - nWritten,
              sampleAccumulator.begin() + nWritten);

    // mark as being done
    currentFrameProcessed = true;
}

btrack::BeatTracker::~BeatTracker() {
}
