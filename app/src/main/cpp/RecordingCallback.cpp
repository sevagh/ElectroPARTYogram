#include "RecordingCallback.h"

oboe::DataCallbackResult
RecordingCallback::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames)
{
    return processRecordingFrames(audioStream, static_cast<int16_t *>(audioData), numFrames * audioStream->getChannelCount());
}

oboe::DataCallbackResult
RecordingCallback::processRecordingFrames(oboe::AudioStream *audioStream, int16_t *audioData, int32_t numFrames)
{
    LOGD(TAG, "processRecordingFrames(): ");
    int32_t framesWritten = mSoundRecording->write(audioData, numFrames);
    LOGD(TAG, "processRecordingFrames(): ", framesWritten);

    // do some signal processing here
    // beat detection, pitch detection, etc.
    // now, how to ship the data back to Vulkan?
    mDrawData.streak++;

    return oboe::DataCallbackResult::Continue;
}

const DrawParams&
RecordingCallback::GetDrawParams(){
    return mDrawData;
}
