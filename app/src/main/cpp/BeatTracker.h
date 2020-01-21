#ifndef ANIMALS_AS_METER_BEATTRACKER_H
#define ANIMALS_AS_METER_BEATTRACKER_H

#include "CircularBuffer.h"
#include "NE10.h"
#include "OnsetDetection.h"
#include <array>
#include <atomic>
#include <complex>
#include <oboe/Definitions.h>
#include <vector>

/*
 * A re-implementation of https://github.com/adamstark/BTrack
 */
namespace btrack {

class BeatTracker {

private:
	static constexpr int HopSize = 512;
	static constexpr size_t OnsetDFBufferSize = 512;
	static constexpr size_t FFTLengthForACFCalculation = 1024;
	static constexpr float Tightness = 5.0f;
	static constexpr float Alpha = 0.9f;
	static constexpr float Epsilon = 0.0001f;

	int32_t sampleRate;
	std::vector<float> sampleAccumulator;
	std::atomic_bool currentFrameProcessed;

	circbuf::CircularBuffer onsetDF;
	std::vector<float> resampledOnsetDF;
	circbuf::CircularBuffer cumulativeScore;
	onset::OnsetDetectionFunction odf;

	std::vector<ne10_fft_cpx_float32_t> complexIn;
	std::vector<ne10_fft_cpx_float32_t> complexOut;
	ne10_fft_cfg_float32_t acfFFT;

	float tempoToLagFactor;
	float estimatedTempo;
	float latestCumulativeScoreValue;
	float beatPeriod;
	int m0;
	int beatCounter;

	std::array<float, 512> acf;
	std::array<float, 128> combFilterBankOutput;
	std::array<float, 41> tempoObservationVector;
	std::array<float, 41> delta;
	std::array<float, 41> prevDelta;

	int32_t nWritten;

	void processCurrentFrame();
	void processOnsetDetectionFunctionSample(float sample);
	void resampleOnsetDetectionFunction();
	void updateCumulativeScore(float odfSample);
	void predictBeat();
	void calculateTempo();
	void adaptiveThreshold(float* x, size_t N);
	void calculateOutputOfCombFilterBank();
	void normalizeArray(float* x, size_t N);
	float calculateMeanOfArray(const float* x, size_t start, size_t end);
	void calculateBalancedACF(std::vector<float>& onsetDetectionFunction);

public:
	static constexpr int FrameSize = 1024;
	bool beatDueInFrame;

	BeatTracker(int32_t sampleRate);
	~BeatTracker();

	void accumulateFrame(float* audioData, int32_t numSamples);
};
} // namespace btrack

#endif // ANIMALS_AS_METER_BEATTRACKER_H
