#include "AudioEngine.h"
#include "oboe/src/common/OboeDebug.h"
#include <oboe/Oboe.h>

AudioEngine::~AudioEngine()
{
	stopStream(mRecordingStream);
	closeStream(mRecordingStream);

	stopStream(mPlaybackStream);
	closeStream(mPlaybackStream);
}

void AudioEngine::startRecording()
{
	LOGD(TAG, "startRecording(): ");

	openRecordingStream();

	if (mRecordingStream) {
		startStream(mRecordingStream);
	}
	else {
		LOGE(TAG, "startRecording(): Failed to create recording (%p) stream",
		     mRecordingStream);
		closeStream(mRecordingStream);
	}
}

void AudioEngine::stopRecording()
{
	LOGD(TAG, "stopRecording(): ");

	stopStream(mRecordingStream);
	closeStream(mRecordingStream);
}

void AudioEngine::openRecordingStream()
{
	LOGD(TAG, "openRecordingStream(): ");

	oboe::AudioStreamBuilder builder;

	setUpRecordingStreamParameters(&builder);

	oboe::Result result = builder.openStream(&mRecordingStream);
	if (result == oboe::Result::OK && mRecordingStream) {
		assert(mRecordingStream->getChannelCount() == mInputChannelCount);
		assert(mRecordingStream->getSampleRate() == mSampleRate);
		assert(mRecordingStream->getFormat() == mFormat);

		mFormat = mRecordingStream->getFormat();
		LOGV(TAG, "openRecordingStream(): mSampleRate = ");
		LOGV(TAG, std::to_string(mSampleRate).c_str());

		LOGV(TAG, "openRecordingStream(): mFormat = ");
		LOGV(TAG, oboe::convertToText(mFormat));
	}
	else {
		LOGE(TAG, "Failed to create recording stream. Error: ");
		LOGE(TAG, oboe::convertToText(result));
	}
}

void AudioEngine::startStream(oboe::AudioStream* stream)
{
	LOGD(TAG, "startStream(): ");
	assert(stream);
	oboe::Result result = stream->requestStart();
	if (result != oboe::Result::OK) {
		LOGE(TAG, "Error starting stream. %s", oboe::convertToText(result));
	}
}

void AudioEngine::stopStream(oboe::AudioStream* stream)
{
	LOGD(TAG, "stopStream(): ");

	if (stream) {
		oboe::Result result = stream->stop(0L);
		if (result != oboe::Result::OK) {
			LOGE(TAG, "Error stopping stream. ");
			LOGE(TAG, oboe::convertToText(result));
		}
		LOGW(TAG, "stopStream(): mTotalSamples = ");
		LOGW(TAG, std::to_string(mSoundRecording.getTotalSamples()).c_str());
	}
}

void AudioEngine::closeStream(oboe::AudioStream* stream)
{
	LOGD(TAG, "closeStream(): ");

	if (stream) {
		oboe::Result result = stream->close();
		if (result != oboe::Result::OK) {
			LOGE(TAG, "Error closing stream. ");
			LOGE(TAG, oboe::convertToText(result));
		}
		else {
			stream = nullptr;
		}

		LOGW(TAG, "closeStream(): mTotalSamples = ");
		LOGW(TAG, std::to_string(mSoundRecording.getTotalSamples()).c_str());
	}
}

oboe::AudioStreamBuilder*
AudioEngine::setUpRecordingStreamParameters(oboe::AudioStreamBuilder* builder)
{
	LOGD(TAG, "setUpRecordingStreamParameters(): ");

	builder->setAudioApi(mAudioApi)
	    ->setFormat(mFormat)
	    ->setSharingMode(oboe::SharingMode::Exclusive)
	    ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
	    ->setCallback(&recordingCallback)
	    ->setDeviceId(mRecordingDeviceId)
	    ->setDirection(oboe::Direction::Input)
	    ->setSampleRate(mSampleRate)
	    ->setChannelCount(mInputChannelCount);
	return builder;
}

const DrawParams& AudioEngine::GetDrawParams()
{
	return recordingCallback.GetDrawParams();
}
