#include "RecordingCallback.h"
#include <__threading_support>
#include <algorithm>
#include <numeric>
#include <thread>
#include <vector>

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
    LOGI("Recording callback received: %d frames", numFrames);

	// necessary assumption for the correct functioning of the app
	assert(numFrames < global::FrameSize);

	// shift samples to the right by numSamples to make space
	std::copy(sampleAccumulator.begin(), sampleAccumulator.end() - numFrames,
	          sampleAccumulator.begin() + numFrames);

	// copy data into sampleAccumulator with optional gain
	std::copy(audioData, audioData + numFrames, sampleAccumulator.begin());

	nWritten += numFrames;

	// if we have enough data to BeatTrack, do it in the background
	if (nWritten >= global::FrameSize) {
		std::thread(&btrack::BeatTracker::processCurrentFrame,
		            std::ref(beatDetector), sampleAccumulator)
		    .detach();
		nWritten = global::FrameSize - nWritten;
	}

	// ensure Vulkan can get the new draw data
	mDrawData.beat = beatDetector.beatDueInFrame;
	mDrawData.tempo = beatDetector.estimatedTempo;

	return oboe::DataCallbackResult::Continue;
}

const DrawParams& RecordingCallback::GetDrawParams() { return mDrawData; }
