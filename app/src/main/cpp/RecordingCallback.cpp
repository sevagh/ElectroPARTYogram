#include "RecordingCallback.h"
#include <__threading_support>
#include <thread>

oboe::DataCallbackResult
RecordingCallback::onAudioReady(oboe::AudioStream* audioStream,
                                void* audioData,
                                int32_t numFrames)
{
	return processRecordingFrames(audioStream, static_cast<float*>(audioData),
	                              numFrames * audioStream->getChannelCount());
}

oboe::DataCallbackResult
RecordingCallback::processRecordingFrames(oboe::AudioStream* audioStream,
                                          float* audioData,
                                          int32_t numFrames)
{
	// necessary assumption for the correct functioning of the app
	// see BeatTracker.cpp for comments on how the accumulator buffer is filled
	// with Oboe callbacks
	assert(numFrames < btrack::BeatTracker::FrameSize);

	// perform beat detection on the audio data in the background
	std::thread(&btrack::BeatTracker::accumulateFrame, std::ref(beatDetector),
	            audioData, numFrames)
	    .detach();

	// get something back from beatDetector - it should be async and data
	// should probably be from a previous run

	// ensure Vulkan can get the new draw data
	mDrawData.streak++;

	return oboe::DataCallbackResult::Continue;
}

const DrawParams& RecordingCallback::GetDrawParams() { return mDrawData; }
