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
	LOGI("RecordingCallback: received %d frames",
		 numFrames);

	// necessary assumption for the correct functioning of the app
	// see BeatTracker.cpp for comments on how the accumulator buffer is filled
	// with Oboe callbacks
	assert(numFrames < btrack::BeatTracker::FrameSize);

	// shift samples to the right by numSamples to make space
	std::copy(sampleAccumulator.begin(), sampleAccumulator.end() - numFrames,
			  sampleAccumulator.begin() + numFrames);

	// copy data into sampleAccumulator
	std::copy(audioData, audioData + numFrames, sampleAccumulator.begin());

	nWritten += numFrames;

	// if we have enough data to BeatTrack, do it
	if (nWritten >= btrack::BeatTracker::FrameSize) {
		std::thread(&btrack::BeatTracker::processCurrentFrame, std::ref(beatDetector),
					sampleAccumulator)
			.detach();
		nWritten = 0;
	}

	// get something back from beatDetector - it should be async and data
	// should probably be from a previous run

	// ensure Vulkan can get the new draw data
	mDrawData.beat = beatDetector.beatDueInFrame;
	mDrawData.tempo = beatDetector.estimatedTempo;

	return oboe::DataCallbackResult::Continue;
}

const DrawParams& RecordingCallback::GetDrawParams() { return mDrawData; }
