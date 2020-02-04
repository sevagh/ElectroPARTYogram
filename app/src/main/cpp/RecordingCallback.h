#ifndef ELECTROPARTYOGRAM_RECORDINGCALLBACK_H
#define ELECTROPARTYOGRAM_RECORDINGCALLBACK_H

#include "BTrack.h"
#include "OnsetDetection.h"
#include "DrawParams.h"
#include "logging_macros.h"
#include <oboe/AudioStream.h>
#include <oboe/Definitions.h>
#include <optional>

#ifndef MODULE_NAME
#define MODULE_NAME "RecordingCallback"
#endif

class RecordingCallback : public oboe::AudioStreamCallback {

private:
	DrawParams *mDrawData;
	std::vector<float> sampleAccumulator;
	size_t nWritten;

public:
	btrack::BTrack beatDetector;

	explicit RecordingCallback(int32_t sampleRate)
	    : beatDetector(btrack::BTrack(sampleRate,
	    		btrack::OnsetDetectionFunctionType::ComplexSpectralDifferenceHWR))
		, sampleAccumulator(btrack::BTrack::FrameSize)
		, mDrawData((DrawParams*) malloc(sizeof(DrawParams)))
		, nWritten(0){};

	~RecordingCallback() {
		free(mDrawData);
	}

	const DrawParams* GetDrawParams();

	oboe::DataCallbackResult onAudioReady(oboe::AudioStream* audioStream,
	                                      void* audioData,
	                                      int32_t numFrames);

	oboe::DataCallbackResult
	processRecordingFrames(oboe::AudioStream* audioStream,
	                       float* audioData,
	                       int32_t numFrames);
};

#endif // ELECTROPARTYOGRAM_RECORDINGCALLBACK_H
