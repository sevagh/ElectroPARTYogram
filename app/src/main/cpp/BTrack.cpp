#include "BTrack.h"
#include "BTrackPrecomputed.h"
#include "CircularBuffer.h"
#include "NE10.h"
#include "OnsetDetection.h"
#include <array>
#include <complex>
#include <cstddef>
#include <vector>
#include "logging_macros.h"

static float calculateMeanOfArray(const float* array, std::size_t start, std::size_t end);
static void adaptiveThreshold(float* x, size_t N);
static void normalizeArray(float* x, size_t N);

namespace btrack {
BTrack::BTrack(int sampleRate)
		: sampleRate(sampleRate)
		, acfFFT(ne10_fft_alloc_c2c_float32_neon(FFTLengthForACFCalculation))
		, tempoToLagFactor(60.0F * (( float )sampleRate) / ( float )HopSize)
		, latestCumulativeScoreValue(0.0F)
		, lastOnset(0.0F)
		, odf(OnsetDetectionFunction())
		, beatPeriod(roundf(
				60.0F / (((( float )HopSize) / ( float )sampleRate) * 120.0F)))
		, m0(10)
		, beatCounter(-1)
		, discardSamples(sampleRate / 2)
		, beatDueInFrame(false)
		, estimatedTempo(120.0F)
		, currentFrame((float*)malloc(FrameSize*sizeof(float)))
{
	exit = new std::atomic_bool;
	notifiedFromCallback = new std::condition_variable;
	std::fill(prevDelta.begin(), prevDelta.end(), 1.0F);

	// initialise df_buffer
	for (size_t i = 0; i < OnsetDFBufferSize; ++i) {
		if ((i % (( size_t )round(beatPeriod)))
			== 0) { // TODO faster modulo
			onsetDF[i] = 1.0F;
		}
	}
};

BTrack::~BTrack()
{
	// destroy FFT things here
	ne10_fft_destroy_c2c_float32(acfFFT);
	delete exit;
	delete notifiedFromCallback;
	free(currentFrame);
};

void BTrack::copyFrameAndNotify(std::vector<float> samples) {
	currentFrameVec = std::move(samples);
	notifiedFromCallback->notify_one();
}

void BTrack::exitThread() {
	exit->store(true);
}

void BTrack::processFrames()
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lock{ mtx };
	while (!exit->load()) {
		notifiedFromCallback->wait(lock);
		memcpy(currentFrame, currentFrameVec.data(), FrameSize*sizeof(float));
		float sample = odf.calculate_sample(currentFrameVec);
		lastOnset = sample;
		processOnsetDetectionFunctionSample(sample);
	}
};

void BTrack::processOnsetDetectionFunctionSample(float sample)
{
	sample = fabs(sample) + Epsilon;
	m0--;
	beatCounter--;
	beatDueInFrame = false;

	onsetDF.append(sample);
	updateCumulativeScore(sample);

	if (m0 == 0) {
		predictBeat();
	}

	if (beatCounter == 0) {
		beatDueInFrame = true;
		// resampleOnsetDetectionFunction();
		calculateTempo();
	}
};

void createWindow(std::array<float, 512> &w1, std::size_t start, std::size_t end, float beatPeriod, float Tightness ){
	float v = -2.0F * beatPeriod;
	std::size_t winsize = end - start + 1;
	// create window
	for (size_t i = 0; i < winsize; ++i) {
		// TODO replace with faster MathNeon computations
		w1[i] = expf((-1 * powf(Tightness * logf(-v / beatPeriod), 2.0F))
					 / 2.0F);
		v += 1.0F;
	}
}

void BTrack::updateCumulativeScore(float odfSample)
{
	auto start = (size_t)(OnsetDFBufferSize - roundf(2.0F * beatPeriod));
	auto end = (size_t)(OnsetDFBufferSize - roundf(beatPeriod / 2.0F));

	createWindow(w1, start, end, beatPeriod, Tightness);

	float max = 0.0F;
	for (size_t i = start; i <= end; ++i) {
		max = std::max(cumulativeScore[i] * w1[i-start], max);
	}

	latestCumulativeScoreValue
		= ((1.0F - Alpha) * odfSample) + (Alpha * max);
	cumulativeScore.append(latestCumulativeScoreValue);
};

void BTrack::predictBeat()
{
	auto windowSize = ( size_t )beatPeriod;
	float futureCumulativeScore[OnsetDFBufferSize + windowSize];
	float w2[windowSize];

	// copy cumscore to first part of fcumscore
	for (size_t i = 0; i < OnsetDFBufferSize; ++i) {
		futureCumulativeScore[i] = cumulativeScore[i];
	}

	// create future window - TODO can this be precomputed?
	float v = 1.0F;
	for (size_t i = 0; i < windowSize; ++i) {
		w2[i] = expf((-1.0F * powf((v - (beatPeriod / 2.0F)), 2.0F))
					 / (2.0F * powf((beatPeriod / 2.0F), 2.0F)));
		v += 1.0F;
	}

	// calculate future cumulative score
	float max;
	int n;
	float wcumscore;
	for (size_t i = OnsetDFBufferSize;
		 i < (OnsetDFBufferSize + windowSize); ++i) {
		auto start = (size_t)(i - roundf(2.0F * beatPeriod));
		auto end = (size_t)(i - roundf(beatPeriod / 2.0F));

		max = 0;
		n = 0;
		for (size_t k = start; k <= end; ++k) {
			wcumscore = futureCumulativeScore[k] * w1[n];

			if (wcumscore > max) {
				max = wcumscore;
			}
			n++;
		}

		futureCumulativeScore[i] = max;
	}

	// predict beat
	max = 0;
	n = 0;

	for (size_t i = OnsetDFBufferSize;
		 i < (OnsetDFBufferSize + windowSize); ++i) {
		wcumscore = futureCumulativeScore[i] * w2[n];

		if (wcumscore > max) {
			max = wcumscore;
			beatCounter = n;
		}

		n++;
	}

	m0 = ( int )(beatCounter + roundf(beatPeriod / 2.0F));
};

void BTrack::calculateTempo()
{
	// adaptive threshold on input - TODO faster math with MathNeon
	adaptiveThreshold(onsetDF.buffer.data(), onsetDF.buffer.size());

	// calculate auto-correlation function of detection function
	calculateBalancedACF(onsetDF.buffer);

	// calculate output of comb filterbank - TODO faster math with MathNeon
	calculateOutputOfCombFilterBank();

	// adaptive threshold on rcf
	adaptiveThreshold(
		combFilterBankOutput.data(), combFilterBankOutput.size());

	size_t t_index;
	size_t t_index2;
	// calculate tempo observation vector from beat period observation vector
	for (size_t i = 0; i < 41; ++i) {
		t_index
			= ( size_t )roundf(tempoToLagFactor / (((2.0F * i) + 80.0F)));
		t_index2
			= ( size_t )roundf(tempoToLagFactor / (((4.0F * i) + 160.0F)));

		tempoObservationVector[i] = combFilterBankOutput[t_index - 1]
									+ combFilterBankOutput[t_index2 - 1];
	}

	float maxval;
	float maxind;
	float curval;

	for (size_t j = 0; j < 41; ++j) {
		maxval = -1;
		for (size_t i = 0; i < 41; ++i) {
			curval = prevDelta[i]
					 * precomputed::TempoTransitionMatrix[i][j];

			if (curval > maxval) {
				maxval = curval;
			}
		}

		delta[j] = maxval * tempoObservationVector[j];
	}

	normalizeArray(delta.data(), 41);

	maxind = -1;
	maxval = -1;

	for (size_t j = 0; j < 41; ++j) {
		if (delta[j] > maxval) {
			maxval = delta[j];
			maxind = j;
		}

		prevDelta[j] = delta[j];
	}

	beatPeriod = roundf((60.0F * (( float )sampleRate))
						/ (((2.0F * maxind) + 80.0F) * (( float )HopSize)));

	if (beatPeriod > 0) {
		estimatedTempo
			= 60.0F
			  / (((( float )HopSize) / (( float )sampleRate)) * beatPeriod);
	}
};

void BTrack::calculateOutputOfCombFilterBank()
{
	std::fill(
		combFilterBankOutput.begin(), combFilterBankOutput.end(), 0.0F);

	for (int i = 2; i <= 127; ++i) {   // max beat period
		for (int a = 1; a <= 4; ++a) { // number of comb elements
			for (int b = 1 - a;
				 b <= a - 1; ++b) { // general state using normalisation of
									// comb elements
				combFilterBankOutput[i - 1]
					= combFilterBankOutput[i - 1]
					  + (acf[(a * i + b) - 1]
						 * precomputed::RayleighWeightingVector128[i - 1])
							/ (2 * a - 1); // calculate value for comb
										   // filter row
			}
		}
	}
};

void BTrack::calculateBalancedACF(
	std::array<float, OnsetDFBufferSize>& onsetDetectionFunction)
{
	// copy 512 samples of onsetDetection into 1024 float
	for (size_t i = 0; i < OnsetDFBufferSize; ++i) {
		complexIn[i].r = onsetDetectionFunction[i];
		complexIn[i].i = 0.0F;
	}

	// zero pad the remaining 512
	for (size_t i = OnsetDFBufferSize; i < FFTLengthForACFCalculation; ++i) {
		complexIn[i].r = 0.0F;
		complexIn[i].i = 0.0F;
	}

	ne10_fft_c2c_1d_float32_neon(
		complexOut.data(), complexIn.data(), acfFFT, 0);

	// multiply by complex conjugate
	for (int i = 0; i < FFTLengthForACFCalculation; i++) {
		complexOut[i].r = complexOut[i].r * complexOut[i].r
						  + complexOut[i].i * complexOut[i].i;
		complexOut[i].i = 0.0F;
	}

	// perform the ifft
	ne10_fft_c2c_1d_float32_neon(
		complexIn.data(), complexOut.data(), acfFFT, 1);

	auto lag = ( float )OnsetDFBufferSize;

	for (size_t i = 0; i < OnsetDFBufferSize; i++) {
		float absValue = sqrtf(complexIn[i].r * complexIn[i].r
							   + complexIn[i].i * complexIn[i].i);

		// divide by inverse lag to deal with scale bias towards small lags
		acf[i] = absValue / lag;

		// this division by 1024 is technically unnecessary but it ensures
		// the algorithm produces exactly the same ACF output as the old
		// time domain implementation. The time difference is minimal so I
		// decided to keep it acf[i] = acf[i] / ( float
		// )FFTLengthForACFCalculation;
		lag -= 1.0F;
	}
};

} // namespace btrack

static void normalizeArray(float* x, size_t N)
{
	float sum = 0.0F;

	for (size_t i = 0; i < N; i++) {
		if (x[i] > 0) {
			sum += x[i];
		}
	}

	if (sum > 0) {
		for (size_t i = 0; i < N; i++) {
			x[i] /= sum;
		}
	}
};

static void adaptiveThreshold(float* x, size_t N)
{
	std::size_t i = 0;
	std::size_t k = 0;
	std::size_t t = 0;
	float x_thresh[N];

	std::size_t p_post = 7;
	std::size_t p_pre = 8;

	t = std::min(
			N, p_post); // what is smaller, p_post or df size. This is to
	// avoid accessing outside of arrays

	// find threshold for first 't' samples, where a full average cannot be
	// computed yet
	for (i = 0; i <= t; ++i) {
		k = std::min((i + p_pre), N);
		x_thresh[i] = calculateMeanOfArray(x, 1, k);
	}
	// find threshold for bulk of samples across a moving average from
	// [i-p_pre,i+p_post]
	for (i = t + 1; i < N - p_post; ++i) {
		x_thresh[i] = calculateMeanOfArray(x, i - p_pre, i + p_post);
	}

	// for last few samples calculate threshold, again, not enough samples
	// to do as above
	for (i = N - p_post; i < N; ++i) {
		k = std::max<std::size_t>(i - p_post, 1);
		x_thresh[i] = calculateMeanOfArray(x, k, N);
	}

	// subtract the threshold from the detection function and check that it
	// is not less than 0
	for (i = 0; i < N; ++i) {
		x[i] = x[i] - x_thresh[i];
		if (x[i] < 0) {
			x[i] = 0;
		}
	}
};

static float calculateMeanOfArray(const float* array, std::size_t start, std::size_t end)
{
	float sum = 0;
	std::size_t length = end - start;

	// find sum
	for (std::size_t i = start; i < end; i++) {
		sum = sum + array[i];
	}

	return (length > 0) ? sum / length : 0;
}

