#include "RecordingCallback.h"
#include <__threading_support>
#include <algorithm>
#include <numeric>
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
	// LOGI("RecordingCallback: received %d frames",
	//	 numFrames);

	// necessary assumption for the correct functioning of the app
	assert(numFrames < btrack::BeatTracker::FrameSize);

	// shift samples to the right by numSamples to make space
	std::copy(sampleAccumulator.begin(), sampleAccumulator.end() - numFrames,
	          sampleAccumulator.begin() + numFrames);

	// copy data into sampleAccumulator with optional gain
	// TODO this can be removed once gain is proven to not help the algorithm
	std::transform(audioData, audioData + numFrames, sampleAccumulator.begin(),
	               std::bind(std::multiplies<>(), std::placeholders::_1, Gain));

	nWritten += numFrames;

	// if we have enough data to BeatTrack, do it in the background
	if (nWritten >= btrack::BeatTracker::FrameSize) {
		std::thread(&btrack::BeatTracker::processCurrentFrame,
		            std::ref(beatDetector), sampleAccumulator)
		    .detach();
		nWritten = 0;
	}

	// ensure Vulkan can get the new draw data
	mDrawData.beat = beatDetector.beatDueInFrame;
	mDrawData.tempo = beatDetector.estimatedTempo;

	return oboe::DataCallbackResult::Continue;
}

const DrawParams& RecordingCallback::GetDrawParams() { return mDrawData; }
