#ifndef ELECTROPARTYOGRAM_BTRACK_H
#define ELECTROPARTYOGRAM_BTRACK_H

#include "CircularBuffer.h"
#include "NE10.h"
#include "OnsetDetection.h"
#include <array>
#include <complex>
#include <cstddef>
#include <vector>

namespace btrack {
class BTrack {
private:
	static constexpr std::size_t OnsetDFBufferSize = 512;
	static constexpr std::size_t FFTLengthForACFCalculation = 1024;
	static constexpr float Tightness = 5.0F;
	static constexpr float Alpha = 0.9F;
	static constexpr float Epsilon = 0.0001F;

	int sampleRate;

	std::array<ne10_fft_cpx_float32_t, FFTLengthForACFCalculation> complexIn
	    = {};
	std::array<ne10_fft_cpx_float32_t, FFTLengthForACFCalculation> complexOut
	    = {};
	ne10_fft_cfg_float32_t acfFFT;

	float tempoToLagFactor;
	float beatPeriod;
	int m0;
	int beatCounter;

	int discardSamples;
	std::atomic_bool *exit;
	std::condition_variable *notifiedFromCallback;

	std::array<float, 512> acf = {};
	std::array<float, 128> combFilterBankOutput = {};
	std::array<float, 41> tempoObservationVector = {};
	std::array<float, 41> delta = {};
	std::array<float, 41> prevDelta = {};

	void processOnsetDetectionFunctionSample(float sample);
	void updateCumulativeScore(float odfSample);
	void predictBeat();
	void calculateTempo();
	void calculateOutputOfCombFilterBank();
	void calculateBalancedACF(std::array<float, OnsetDFBufferSize>& onsetDetectionFunction);

public:
	static constexpr std::size_t FrameSize = 1024;
	static constexpr std::size_t HopSize = 512;

	bool beatDueInFrame;
	float estimatedTempo;
    float latestCumulativeScoreValue;
    std::vector<float> currentFrameVec;
    float lastOnset;
    float *currentFrame;

	OnsetDetectionFunction odf;

	CircularBuffer<OnsetDFBufferSize> onsetDF
			= {};
	CircularBuffer<OnsetDFBufferSize> cumulativeScore
			= {};

	explicit BTrack(int sampleRate);
	~BTrack();

	void exitThread();
	void copyFrameAndNotify(std::vector<float> samples);
	void processFrames();
};
} // namespace btrack

#endif // ELECTROPARTYOGRAM_BTRACK_H
