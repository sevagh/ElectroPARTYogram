#ifndef ANIMALS_AS_METER_ONSETDETECTION_H
#define ANIMALS_AS_METER_ONSETDETECTION_H

// modified for Animals-as-Meter by Sevag
// original from https://github.com/adamstark/BTrack

#include "NE10.h"
#include <array>

namespace onset {
class OnsetDetectionFunction {
public:
	static constexpr size_t FrameSize = 1024;

	OnsetDetectionFunction();
	~OnsetDetectionFunction();

	float calculateOnsetDetectionFunctionSample(std::vector<float>& buffer);

private:
	static constexpr int HopSize = 512;
	void performFFT();

	float complexSpectralDifferenceHWR();

	std::array<ne10_fft_cpx_float32_t, FrameSize> complexIn;
	std::array<ne10_fft_cpx_float32_t, FrameSize> complexOut;
	ne10_fft_cfg_float32_t p;

	std::array<float, FrameSize> frame;
	std::array<float, FrameSize> magSpec;
	std::array<float, FrameSize> prevMagSpec;
	std::array<float, FrameSize> phase;
	std::array<float, FrameSize> prevPhase;
	std::array<float, FrameSize> prevPhase2;
};
} // namespace onset

#endif // ANIMALS_AS_METER_ONSETDETECTION_H
