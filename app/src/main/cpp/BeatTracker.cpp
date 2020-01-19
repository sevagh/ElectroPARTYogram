#include "BeatTracker.h"
#include "logging_macros.h"
#include "NE10.h"
#include <algorithm>
#include "MathNeon.h"
#include <unistd.h>

void btrack::BeatTracker::accumulateFrame(float *audioData, int32_t numSamples) {
    // if we're done, continue on to processing in the background
    if (currentFrameProcessed) {
        LOGI("BeatTracker: appending %d and processing complete frame", numSamples);
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

    // call to ODF to ensure it's actually doing work
    odf.calculateOnsetDetectionFunctionSample(sampleAccumulator);

    // sleep to simulate a computation that's slower than the latency of the audio in stream
    //usleep(8500);

    // at the end, shift samples to the right
    std::copy(sampleAccumulator.begin(), sampleAccumulator.end() - nWritten,
              sampleAccumulator.begin() + nWritten);

    // mark as being done
    currentFrameProcessed = true;
}

btrack::BeatTracker::~BeatTracker() {
}
