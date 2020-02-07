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
	assert(numFrames < btrack::BTrack::FrameSize);

	// shift samples to the right by numSamples to make space
	std::copy(sampleAccumulator.begin(), sampleAccumulator.end() - numFrames,
	          sampleAccumulator.begin() + numFrames);

	// copy data into sampleAccumulator with optional gain
	std::copy(audioData, audioData + numFrames, sampleAccumulator.begin());

	nWritten += numFrames;

	// if we have enough data to BeatTrack, do it in the background
	if (nWritten >= btrack::BTrack::FrameSize) {

		beatDetector.copyFrameAndNotify(sampleAccumulator);
		nWritten = btrack::BTrack::FrameSize - nWritten;
	}

	// set the member mDrawParams for the next iteration of the SFML draw loop
	// to use it
	mDrawData->beat = beatDetector.beatDueInFrame;
	mDrawData->tempo = beatDetector.estimatedTempo;
	mDrawData->cumScore = beatDetector.latestCumulativeScoreValue;
	mDrawData->lastOnset = beatDetector.lastOnset;

	return oboe::DataCallbackResult::Continue;
}

const DrawParams* RecordingCallback::GetDrawParams() { return mDrawData; }
