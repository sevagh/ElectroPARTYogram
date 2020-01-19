#include "OnsetDetection.h"
#include <algorithm>

#define _USE_MATH_DEFINES //for M_PI
#include <cmath>


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
OnsetDetectionFunction::OnsetDetectionFunction(int hopSize, size_t frameSize) :
        hopSize(hopSize),
        frameSize(frameSize),
        frame(std::vector<float>(frameSize)),
        window(std::vector<float>(frameSize)),
        phase(std::vector<float>(frameSize)),
        prevPhase(std::vector<float>(frameSize)),
        prevPhase2(std::vector<float>(frameSize)),
        magSpec(std::vector<float>(frameSize)),
        prevMagSpec(std::vector<float>(frameSize)),
        p(ne10_fft_alloc_r2c_float32(frameSize)),
        complexOut(std::vector<ne10_fft_cpx_float32_t>(frameSize))
        {
    calculateHanningWindow();
}

OnsetDetectionFunction::~OnsetDetectionFunction()
{
    ne10_fft_destroy_r2c_float32(p);
}

float OnsetDetectionFunction::calculateOnsetDetectionFunctionSample(std::vector<float>& buffer) {
    // shift audio samples back in frame by hop size
    std::copy(frame.begin()+hopSize, frame.end(), frame.begin());

    // add new samples to frame from input buffer
    std::copy(buffer.begin(), buffer.begin()+hopSize, frame.end()-hopSize);

    return complexSpectralDifferenceHWR();
}


//=======================================================================
void OnsetDetectionFunction::performFFT()
{
    size_t fsize2 = frameSize/2;
    size_t idx = 0;

    for (size_t i = 0; i < fsize2; ++i) {
        idx = i + fsize2;
        frame[i] *= window[i];
        frame[idx] *= window[idx];
        std::swap(frame[i], frame[idx]);
    }

    ne10_fft_r2c_1d_float32_neon(complexOut.data(), frame.data(), p);
}

//=======================================================================
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
    for (int i = 0;i < frameSize;i++)
    {
        // calculate phase value
        phase[i] = atan2(complexOut[i].i, complexOut[i].r);

        // calculate magnitude value
        magSpec[i] = sqrt(pow (complexOut[i].r, 2) + pow(complexOut[i].i, 2));

        // phase deviation
        phaseDeviation = phase[i] - (2 * prevPhase[i]) + prevPhase2[i];

        // calculate magnitude difference (real part of Euclidean distance between complex frames)
        magnitudeDifference = magSpec[i] - prevMagSpec[i];

        // if we have a positive change in magnitude, then include in sum, otherwise ignore (half-wave rectification)
        if (magnitudeDifference > 0)
        {
            // calculate complex spectral difference for the current spectral bin
            csd = sqrt(pow (magSpec[i], 2) + pow (prevMagSpec[i], 2) - 2 * magSpec[i] * prevMagSpec[i] * cos (phaseDeviation));

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


//=======================================================================
void OnsetDetectionFunction::calculateHanningWindow()
{
    float N;		// variable to store framesize minus 1

    N = (float) (frameSize-1);	// framesize minus 1

    // Hanning window calculation
    for (int n = 0; n < frameSize; n++)
    {
        window[n] = 0.5 * (1 - cos (2 * M_PI * (n / N)));
    }
}
}
