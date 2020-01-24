#include "OnsetDetection.h"
#include "MathNeon.h"
#include "Precomputed.h"
#include "logging_macros.h"
#include <algorithm>
#include <cmath>

// modified for Animals-as-Meter by Sevag
// original from https://github.com/adamstark/BTrack

namespace onset {
OnsetDetectionFunction::OnsetDetectionFunction()
	: fftCfg(ne10_fft_alloc_r2c_float32(global::FrameSize))
	{
	}

OnsetDetectionFunction::~OnsetDetectionFunction()
{
	ne10_fft_destroy_r2c_float32(fftCfg);
}

float OnsetDetectionFunction::calculateOnsetDetectionFunctionSample(
    std::vector<float>& buffer)
{
	// shift audio samples back in frame by hop size
	std::copy(frame.begin()+global::HopSize, frame.end(), frame.begin());

	// add new samples to frame from input buffer
	std::copy(buffer.begin(), buffer.begin()+global::HopSize, frame.end()-global::HopSize);

	return complexSpectralDifferenceHWR();
}

void OnsetDetectionFunction::performFFT()
{
	size_t fsize2 = global::FrameSize2;

	for (size_t i = 0; i < fsize2; ++i) {
		std::iter_swap(frame.begin()+i, frame.begin()+i+fsize2);
		frame[i] *= precomputed::HanningWindow1152[i + fsize2];
		frame[i+fsize2] *= precomputed::HanningWindow1152[i];
	}

	ne10_fft_r2c_1d_float32_neon(complexOut.data(), frame.data(), fftCfg);
}

float OnsetDetectionFunction::complexSpectralDifferenceHWR()
{
	float phaseDeviation;
	float sum;
	float magnitudeDifference;
	float csd;

	// perform the FFT
	performFFT();

	sum = 0; // initialise sum to zero

	// compute phase values from fft output and sum deviations
	for (int i = 0; i < global::FrameSize; i++) {
		// calculate phase value
		phase[i] = atan2(complexOut[i].i, complexOut[i].r);

		// calculate magnitude value
		magSpec[i] = sqrtf(powf(complexOut[i].r, 2) + powf(complexOut[i].i, 2));

		// phase deviation
		phaseDeviation = phase[i] - (2 * prevPhase[i]) + prevPhase2[i];

		// calculate magnitude difference (real part of Euclidean distance
		// between complex frames)
		magnitudeDifference = magSpec[i] - prevMagSpec[i];

		// if we have a positive change in magnitude, then include in sum,
		// otherwise ignore (half-wave rectification)
		if (magnitudeDifference > 0) {
			// calculate complex spectral difference for the current spectral bin
			csd = sqrtf(powf(magSpec[i], 2) + powf(prevMagSpec[i], 2)
			            - 2 * magSpec[i] * prevMagSpec[i]
			                  * cosf(phaseDeviation));

			// add to sum
			sum = sum + csd;
		}

		// store values for next calculation
		prevPhase2[i] = prevPhase[i];
		prevPhase[i] = phase[i];
		prevMagSpec[i] = magSpec[i];
	}

	return sum;
}
} // namespace onset
