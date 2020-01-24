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
    : p(ne10_fft_alloc_c2c_float32_neon(FrameSize))
{
}

OnsetDetectionFunction::~OnsetDetectionFunction()
{
	ne10_fft_destroy_c2c_float32(p);
}

// TODO - replace with std::copy
float OnsetDetectionFunction::calculateOnsetDetectionFunctionSample(
    std::vector<float>& buffer)
{
	// shift audio samples back in frame by hop size
	for (int i = 0; i < (FrameSize - HopSize); i++) {
		frame[i] = frame[i + HopSize];
	}

	// add new samples to frame from input buffer
	int j = 0;
	for (int i = (FrameSize - HopSize); i < FrameSize; i++) {
		frame[i] = buffer[j];
		j++;
	}

	return complexSpectralDifferenceHWR();
}

// TODO - go back to r2c
void OnsetDetectionFunction::performFFT()
{
	size_t fsize2 = FrameSize / 2;

	for (size_t i = 0; i < fsize2; ++i) {
		complexIn[i].r
		    = frame[i + fsize2] * precomputed::HanningWindow1024[i + fsize2];
		complexIn[i].i = 0.0F;
		complexIn[i + fsize2].r = frame[i] * precomputed::HanningWindow1024[i];
		complexIn[i + fsize2].i = 0.0F;
	}

	ne10_fft_c2c_1d_float32_neon(complexOut.data(), complexIn.data(), p, 0);
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
	for (int i = 0; i < FrameSize; i++) {
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
