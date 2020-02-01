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
	static constexpr std::size_t HopSize = 512;
	static constexpr std::size_t OnsetDFBufferSize = 512;
	static constexpr std::size_t FFTLengthForACFCalculation = 1024;
	static constexpr float Tightness = 5.0F;
	static constexpr float Alpha = 0.1F;
	static constexpr float Epsilon = 0.0001F;

	int sampleRate;

	OnsetDetectionFunction odf;

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
	bool beatDueInFrame;
	float estimatedTempo;
    float latestCumulativeScoreValue;

	CircularBuffer<OnsetDFBufferSize> onsetDF
			= {};
	CircularBuffer<OnsetDFBufferSize> cumulativeScore
			= {};

	explicit BTrack(
	    int sampleRate,
	    OnsetDetectionFunctionType onsetType);
	~BTrack();

	void processCurrentFrame(std::vector<float> samples);
};
} // namespace btrack

#endif // ELECTROPARTYOGRAM_BTRACK_H
