#include "RecordingCallback.h"

oboe::DataCallbackResult
RecordingCallback::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames)
{
    return processRecordingFrames(audioStream, static_cast<float *>(audioData), numFrames * audioStream->getChannelCount());
}

oboe::DataCallbackResult
RecordingCallback::processRecordingFrames(oboe::AudioStream *audioStream, float *audioData, int32_t numFrames)
{
    // necessary assumption for the correct functioning of the app
    // see Obtain.cpp for comments on how the accumulator buffer is filled with Oboe callbacks
    assert(numFrames < obtain::Obtain::WindowSize);

    // do some signal processing here
    // beat detection, pitch detection, etc.
    obtainBeatDetector.processData(audioData, numFrames);

    // now, how to ship the data back to Vulkan?
    mDrawData.streak++;

    return oboe::DataCallbackResult::Continue;
}

const DrawParams&
RecordingCallback::GetDrawParams(){
    return mDrawData;
}
