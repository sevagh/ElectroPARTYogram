#include "OnsetDetection.h"
#include "MathNeon.h"
#include "Precomputed.h"
#include <algorithm>
#include <cmath>

// modified for Animals-as-Meter by Sevag
// original from https://github.com/adamstark/BTrack

//=======================================================================
/** @file OnsetDetectionFunction.cpp
 *  @brief A class for calculating onset detection functions
 *  @author Adam Stark
 *  @copyright Copyright (C) 2008-2014  Queen Mary University of London
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

namespace onset {
OnsetDetectionFunction::OnsetDetectionFunction()
    : frame(std::vector<float>(FrameSize))
    , phase(std::vector<float>(FrameSize))
    , prevPhase(std::vector<float>(FrameSize))
    , prevPhase2(std::vector<float>(FrameSize))
    , magSpec(std::vector<float>(FrameSize))
    , prevMagSpec(std::vector<float>(FrameSize))
    , p(ne10_fft_alloc_r2c_float32(FrameSize))
    , complexOut(std::vector<ne10_fft_cpx_float32_t>(FrameSize))
{
}

OnsetDetectionFunction::~OnsetDetectionFunction()
{
	ne10_fft_destroy_r2c_float32(p);
}

float OnsetDetectionFunction::calculateOnsetDetectionFunctionSample(
    std::vector<float>& buffer)
{
	// shift audio samples back in frame by hop size
	std::copy(frame.begin() + HopSize, frame.end(), frame.begin());

	// add new samples to frame from input buffer
	std::copy(buffer.begin(), buffer.begin() + HopSize, frame.end() - HopSize);

	return complexSpectralDifferenceHWR();
}

void OnsetDetectionFunction::performFFT()
{
	size_t fsize2 = FrameSize / 2;
	size_t idx = 0;

	for (size_t i = 0; i < fsize2; ++i) {
		idx = i + fsize2;
		frame[i] *= precomputed::HanningWindow1024[i];
		frame[idx] *= precomputed::HanningWindow1024[idx];
		std::swap(frame[i], frame[idx]);
	}

	ne10_fft_r2c_1d_float32_neon(complexOut.data(), frame.data(), p);
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
