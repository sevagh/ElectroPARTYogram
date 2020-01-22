#include "BeatTracker.h"
#include "MathNeon.h"
#include "NE10.h"
#include "Precomputed.h"
#include "logging_macros.h"
#include "samplerate.h"
#include <algorithm>
#include <numeric>

btrack::BeatTracker::BeatTracker(int32_t sampleRate_)
    : sampleRate(44100)
    , nWritten(0)
    , sampleAccumulator(FrameSize)
    , currentFrameProcessed(true)
    , onsetDF(circbuf::CircularBuffer(OnsetDFBufferSize))
    , resampledOnsetDF(OnsetDFBufferSize)
    , cumulativeScore(circbuf::CircularBuffer(OnsetDFBufferSize))
    , odf(onset::OnsetDetectionFunction())
    , complexIn(std::vector<ne10_fft_cpx_float32_t>(FFTLengthForACFCalculation))
    , complexOut(std::vector<ne10_fft_cpx_float32_t>(FFTLengthForACFCalculation))
    , acfFFT(ne10_fft_alloc_c2c_float32_neon(FFTLengthForACFCalculation))
    , tempoToLagFactor(60.0F * (( float )sampleRate) / ( float )HopSize)
    , latestCumulativeScoreValue(0.0F)
    , beatPeriod(
          roundf(60.0F / (((( float )HopSize) / ( float )sampleRate) * 120.0F)))
    , m0(10)
    , beatCounter(-1)
    , beatDueInFrame(false)
	, estimatedTempo(120.0F)
{
	std::fill(prevDelta.begin(), prevDelta.end(), 1.0F);

	// initialise df_buffer
	for (size_t i = 0; i < OnsetDFBufferSize; ++i) {
		if ((i % (( size_t )round(beatPeriod))) == 0) { // TODO faster modulo
			                                            // here? arm neon/simd
			onsetDF[i] = 1.0F;
		}
	}
}

void btrack::BeatTracker::accumulateFrame(float* audioData, int32_t numSamples)
{
	// shift samples to the right by numSamples to make space
	std::copy(sampleAccumulator.begin(), sampleAccumulator.end() - numSamples,
			  sampleAccumulator.begin() + numSamples);

	// copy data into sampleAccumulator
	std::copy(audioData, audioData + numSamples, sampleAccumulator.begin());

	nWritten += numSamples;

	std::transform(sampleAccumulator.begin(), sampleAccumulator.end(),
				   sampleAccumulator.begin(),
				   std::bind(std::multiplies<float>(), std::placeholders::_1, 10000));

	if (currentFrameProcessed && (nWritten >= FrameSize)) {
		LOGI("BeatTracker: filled %d and processing complete frame",
			 nWritten);
		processCurrentFrame(sampleAccumulator);
		nWritten = 0;
	}

	return;
}

//intentionally copy the data
void btrack::BeatTracker::processCurrentFrame(std::vector<float> samples)
{
	// in the beginning, mark as not done yet
	currentFrameProcessed = false;

	/* these lines encompass the full computation of BTrack */
	float sample = odf.calculateOnsetDetectionFunctionSample(samples);
	processOnsetDetectionFunctionSample(sample);

	LOGI("BeatTracker: beat expected: %s, tempo: %f",
	     beatDueInFrame ? "true" : "false", estimatedTempo);
	/* end of BTrack */

	// mark as being done
	currentFrameProcessed = true;
}

void btrack::BeatTracker::processOnsetDetectionFunctionSample(float sample)
{
	sample = fabs(sample) + Epsilon;
	m0--;
	beatCounter--;
	beatDueInFrame = false;

	onsetDF.addSampleToEnd(sample);
	updateCumulativeScore(sample);

	if (m0 == 0) {
		predictBeat();
	}

	if (beatCounter == 0) {
		beatDueInFrame = true;
		resampleOnsetDetectionFunction();
		calculateTempo();
	}
}

void btrack::BeatTracker::resampleOnsetDetectionFunction() {
	float output[512];
	float input[OnsetDFBufferSize];

	for (int i = 0;i < OnsetDFBufferSize;i++)
	{
		input[i] = (float) onsetDF[i];
	}

	float src_ratio = 512.0/((float) OnsetDFBufferSize);
	int BUFFER_LEN = OnsetDFBufferSize;
	int output_len;
	SRC_DATA	src_data ;

	//output_len = (int) floor (((float) BUFFER_LEN) * src_ratio) ;
	output_len = 512;

	src_data.data_in = input;
	src_data.input_frames = BUFFER_LEN;

	src_data.src_ratio = src_ratio;

	src_data.data_out = output;
	src_data.output_frames = output_len;

	src_simple (&src_data, SRC_SINC_BEST_QUALITY, 1);

	for (int i = 0;i < output_len;i++)
	{
		resampledOnsetDF[i] = (float) src_data.data_out[i];
	}
}

void btrack::BeatTracker::calculateTempo()
{
	// adaptive threshold on input - TODO faster math with MathNeon
	adaptiveThreshold(resampledOnsetDF.data(), resampledOnsetDF.size());

	// calculate auto-correlation function of detection function
	calculateBalancedACF(resampledOnsetDF);

	// calculate output of comb filterbank - TODO faster math with MathNeon
	calculateOutputOfCombFilterBank();

	// adaptive threshold on rcf
	adaptiveThreshold(combFilterBankOutput.data(), combFilterBankOutput.size());

	size_t t_index;
	size_t t_index2;
	// calculate tempo observation vector from beat period observation vector
	for (size_t i = 0; i < 41; ++i) {
		t_index = ( size_t )roundf(tempoToLagFactor / (((2.0F * i) + 80.0F)));
		t_index2 = ( size_t )roundf(tempoToLagFactor / (((4.0F * i) + 160.0F)));

		tempoObservationVector[i] = combFilterBankOutput[t_index - 1]
		                            + combFilterBankOutput[t_index2 - 1];
	}

	float maxval;
	float maxind;
	float curval;

	for (size_t j = 0; j < 41; ++j) {
		maxval = -1;
		for (size_t i = 0; i < 41; ++i) {
			curval = prevDelta[i] * precomputed::TempoTransitionMatrix[i][j];

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
}

void btrack::BeatTracker::predictBeat()
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

	// create past window
	v = -2.0F * beatPeriod;
	auto start = (size_t)(OnsetDFBufferSize - roundf(2.0F * beatPeriod));
	auto end = (size_t)(OnsetDFBufferSize - roundf(beatPeriod / 2.0F));
	size_t pastwinsize = end - start + 1;
	float w1[pastwinsize];

	// TODO replace logs, exp, power, etc. with MathNeon faster versions
	for (size_t i = 0; i < pastwinsize; ++i) {
		w1[i] = expf((-1.0F * powf(Tightness * logf(-v / beatPeriod), 2.0F))
		             / 2.0F);
		v = v + 1;
	}

	// calculate future cumulative score
	float max;
	int n;
	float wcumscore;
	for (size_t i = OnsetDFBufferSize; i < (OnsetDFBufferSize + windowSize);
	     ++i) {
		start = (size_t)(i - roundf(2.0F * beatPeriod));
		end = (size_t)(i - roundf(beatPeriod / 2.0F));

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

	for (size_t i = OnsetDFBufferSize; i < (OnsetDFBufferSize + windowSize);
	     ++i) {
		wcumscore = futureCumulativeScore[i] * w2[n];

		if (wcumscore > max) {
			max = wcumscore;
			beatCounter = n;
		}

		n++;
	}

	m0 = ( int )(beatCounter + roundf(beatPeriod / 2.0F));
}

void btrack::BeatTracker::updateCumulativeScore(float odfSample)
{
	size_t start;
	size_t end;
	size_t winsize;
	float max;

	start = (size_t)(OnsetDFBufferSize - roundf(2.0F * beatPeriod));
	end = (size_t)(OnsetDFBufferSize - roundf(beatPeriod / 2.0F));

	winsize = end - start + 1;

	float w1[winsize];
	float v = -2.0F * beatPeriod;
	float wcumscore;

	// create window
	for (size_t i = 0; i < winsize; ++i) {
		// TODO replace with faster MathNeon computations
		w1[i] = expf((-1 * powf(Tightness * logf(-v / beatPeriod), 2.0F)) / 2.0F);
		v += 1.0F;
	}

	max = 0;
	size_t n = 0;
	for (size_t i = start; i <= end; ++i) {
		wcumscore = cumulativeScore[i] * w1[n++];
		if (wcumscore > max)
			max = wcumscore;
	}

	latestCumulativeScoreValue = ((1.0F - Alpha) * odfSample) + (Alpha * max);
	cumulativeScore.addSampleToEnd(latestCumulativeScoreValue);
}

// TODO ne10 FFT has some weird scaling rules - ensure this doesn't screw with
// BTrack
void btrack::BeatTracker::calculateBalancedACF(
    std::vector<float>& onsetDetectionFunction)
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

	float lag = ( float )OnsetDFBufferSize;

	for (size_t i = 0; i < OnsetDFBufferSize; i++) {
		float absValue = sqrtf(complexIn[i].r * complexIn[i].r
		                       + complexIn[i].i * complexIn[i].i);

		// divide by inverse lag to deal with scale bias towards small lags
		acf[i] = absValue / lag;

		// this division by 1024 is technically unnecessary but it ensures the
		// algorithm produces exactly the same ACF output as the old time
		// domain implementation. The time difference is minimal so I decided
		// to keep it
		//acf[i] = acf[i] / ( float )FFTLengthForACFCalculation;
		lag -= 1.0F;
	}
}

float btrack::BeatTracker::calculateMeanOfArray(const float* array,
                                                size_t start,
                                                size_t end)
{
	float sum = 0;
	size_t length = end - start;

	// find sum
	for (size_t i = start; i < end; i++) {
		sum = sum + array[i];
	}

	return (length > 0) ? sum / length : 0;
}

void btrack::BeatTracker::normalizeArray(float* array, size_t N)
{
	float sum = 0.0F;

	for (size_t i = 0; i < N; i++) {
		if (array[i] > 0) {
			sum += array[i];
		}
	}

	if (sum > 0) {
		for (size_t i = 0; i < N; i++) {
			array[i] /= sum;
		}
	}
}

void btrack::BeatTracker::adaptiveThreshold(float* x, size_t N)
{
	size_t i = 0;
	size_t k = 0;
	size_t t = 0;
	float x_thresh[N];

	size_t p_post = 7;
	size_t p_pre = 8;

	t = std::min(N, p_post); // what is smaller, p_post or df size. This is to
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

	// for last few samples calculate threshold, again, not enough samples to
	// do as above
	for (i = N - p_post; i < N; ++i) {
		k = std::max<size_t>(i - p_post, 1);
		x_thresh[i] = calculateMeanOfArray(x, k, N);
	}

	// subtract the threshold from the detection function and check that it is
	// not less than 0
	for (i = 0; i < N; ++i) {
		x[i] = x[i] - x_thresh[i];
		if (x[i] < 0) {
			x[i] = 0;
		}
	}
}

void btrack::BeatTracker::calculateOutputOfCombFilterBank()
{
	std::fill(combFilterBankOutput.begin(), combFilterBankOutput.end(), 0.0F);

	for (int i = 2; i <= 127; ++i) {   // max beat period
		for (int a = 1; a <= 4; ++a) { // number of comb elements
			for (int b = 1 - a; b <= a - 1;
			     ++b) { // general state using normalisation of comb elements
				combFilterBankOutput[i - 1]
				    = combFilterBankOutput[i - 1]
				      + (acf[(a * i + b) - 1]
				         * precomputed::RayleighWeightingVector128[i - 1])
				            / (2 * a
				               - 1); // calculate value for comb filter row
			}
		}
	}
}

btrack::BeatTracker::~BeatTracker()
{
	// destroy FFT things here
	ne10_fft_destroy_c2c_float32(acfFFT);
}
