#ifndef ANIMALS_AS_METER_ONSETDETECTION_H
#define ANIMALS_AS_METER_ONSETDETECTION_H

// modified for Animals-as-Meter by Sevag
// original from https://github.com/adamstark/BTrack

#include "NE10.h"
#include "GlobalParams.h"
#include <array>

namespace onset {
class OnsetDetectionFunction {
public:
	OnsetDetectionFunction();
	~OnsetDetectionFunction();

	float calculateOnsetDetectionFunctionSample(std::vector<float>& buffer);

private:
	void performFFT();

	float complexSpectralDifferenceHWR();

	std::array<ne10_fft_cpx_float32_t, global::FrameSize> complexOut;
    ne10_fft_r2c_cfg_float32_t fftCfg;

	std::array<float, global::FrameSize> frame;
	std::array<float, global::FrameSize> magSpec;
	std::array<float, global::FrameSize> prevMagSpec;
	std::array<float, global::FrameSize> phase;
	std::array<float, global::FrameSize> prevPhase;
	std::array<float, global::FrameSize> prevPhase2;
};
} // namespace onset

#endif // ANIMALS_AS_METER_ONSETDETECTION_H
