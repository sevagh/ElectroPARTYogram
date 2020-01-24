#ifndef ANIMALS_AS_METER_RECORDINGCALLBACK_H
#define ANIMALS_AS_METER_RECORDINGCALLBACK_H

#include "BeatTracker.h"
#include "DrawParams.h"
#include "logging_macros.h"
#include <oboe/AudioStream.h>
#include <oboe/Definitions.h>

#ifndef MODULE_NAME
#define MODULE_NAME "RecordingCallback"
#endif

class RecordingCallback : public oboe::AudioStreamCallback {

private:
	DrawParams mDrawData{};
	btrack::BeatTracker beatDetector;
	std::vector<float> sampleAccumulator;
	int32_t nWritten;
	static constexpr float Gain = 1.0F;

public:
	explicit RecordingCallback(int32_t sampleRate)
	    : beatDetector(btrack::BeatTracker(sampleRate))
	    , nWritten(0)
	    , sampleAccumulator(btrack::BeatTracker::FrameSize){};

	const DrawParams& GetDrawParams();

	oboe::DataCallbackResult onAudioReady(oboe::AudioStream* audioStream,
	                                      void* audioData,
	                                      int32_t numFrames);

	oboe::DataCallbackResult
	processRecordingFrames(oboe::AudioStream* audioStream,
	                       float* audioData,
	                       int32_t numFrames);
};

#endif // ANIMALS_AS_METER_RECORDINGCALLBACK_H
