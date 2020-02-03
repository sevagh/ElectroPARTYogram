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
                                          int32_t numFrames) {
	assert(numFrames < FrameSize);

	// shift samples to the right by numSamples to make space
	std::copy(sampleAccumulator.begin(), sampleAccumulator.end() - numFrames,
			  sampleAccumulator.begin() + numFrames);

	// copy data into sampleAccumulator with optional gain
	std::copy(audioData, audioData + numFrames, sampleAccumulator.begin());

	nWritten += numFrames;

	// if we have enough data to BeatTrack, do it in the background
	if (nWritten >= FrameSize) {
		std::thread(
				&btrack::BTrack::processCurrentFrame,
				std::ref(beatDetector), sampleAccumulator)
				.detach();
		nWritten = FrameSize - nWritten;
	}

	// ship draw data to Java draw thread
	DrawParams drawData{};
	drawData.beat = beatDetector.beatDueInFrame;
	drawData.tempo = beatDetector.estimatedTempo;
	drawData.cumScore = beatDetector.latestCumulativeScoreValue;
	//LOGI("1.2 recording callback mDrawData copies: %s %f %f", mDrawData.beat? "true" : "false", mDrawData.tempo, mDrawData.cumScore);
	memcpy(mDrawData, &drawData, sizeof(DrawParams));

	return oboe::DataCallbackResult::Continue;
}

const DrawParams* RecordingCallback::GetDrawParams() {
    LOGI("1.A recordingCallback drawParams: %s %f %f", mDrawData->beat? "true" : "false", mDrawData->tempo, mDrawData->cumScore);
	return mDrawData;
}
