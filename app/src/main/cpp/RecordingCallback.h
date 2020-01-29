#ifndef ANIMALS_AS_METER_RECORDINGCALLBACK_H
#define ANIMALS_AS_METER_RECORDINGCALLBACK_H

#include "GlobalParams.h"
#include "stompbox/BTrack.h"
#include "stompbox/OnsetDetection.h"
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
	stompbox::btrack::BTrack<global::FrameSize, global::HopSize> beatDetector;
	std::vector<float> sampleAccumulator;
	size_t nWritten;

public:
	explicit RecordingCallback(int32_t sampleRate)
	    : beatDetector(stompbox::btrack::BTrack<global::FrameSize, global::HopSize>(sampleRate,
	    		stompbox::onset_detection::OnsetDetectionFunctionType::ComplexSpectralDifferenceHWR))
		, sampleAccumulator(global::FrameSize)
		, nWritten(0){};

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
