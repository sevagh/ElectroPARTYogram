#ifndef ANIMALS_AS_METER_AUDIOENGINE_H
#define ANIMALS_AS_METER_AUDIOENGINE_H

#ifndef MODULE_NAME
#define MODULE_NAME "AudioEngine"
#endif

#include "DrawParams.h"
#include "RecordingCallback.h"
#include "SoundRecording.h"
#include "logging_macros.h"
#include <oboe/AudioStream.h>
#include <oboe/Definitions.h>

class AudioEngine {

public:
	AudioEngine(){};
	~AudioEngine();

	RecordingCallback recordingCallback
	    = RecordingCallback(&mSoundRecording, mSampleRate);

	void startRecording();
	void stopRecording();
	const DrawParams& GetDrawParams();

private:
	const char* TAG = "AudioEngine:: %s";

	// VoiceRecognition is the lowest latency on any device
	// https://github.com/google/oboe/blob/master/docs/FullGuide.md
	int32_t mRecordingDeviceId = oboe::VoiceRecognition;

	oboe::AudioFormat mFormat = oboe::AudioFormat::Float;
	static constexpr int32_t mSampleRate = 48000;
	int32_t mInputChannelCount = oboe::ChannelCount::Mono;

	oboe::AudioApi mAudioApi = oboe::AudioApi::AAudio;
	oboe::AudioStream* mRecordingStream = nullptr;
	oboe::AudioStream* mPlaybackStream = nullptr;
	SoundRecording mSoundRecording;

	void openRecordingStream();

	void startStream(oboe::AudioStream* stream);
	void stopStream(oboe::AudioStream* stream);
	void closeStream(oboe::AudioStream* stream);

	oboe::AudioStreamBuilder*
	setUpRecordingStreamParameters(oboe::AudioStreamBuilder* builder);
};

#endif // ANIMALS_AS_METER_AUDIOENGINE_H
