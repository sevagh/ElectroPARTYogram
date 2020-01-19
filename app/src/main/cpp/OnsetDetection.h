#ifndef ANIMALS_AS_METER_ONSETDETECTION_H
#define ANIMALS_AS_METER_ONSETDETECTION_H

// modified for Animals-as-Meter by Sevag
// original from https://github.com/adamstark/BTrack

#include "NE10.h"
#include <vector>

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

	std::vector<ne10_fft_cpx_float32_t> complexOut;
	ne10_fft_r2c_cfg_float32_t p;

	std::vector<float> frame;
	std::vector<float> magSpec;
	std::vector<float> prevMagSpec;
	std::vector<float> phase;
	std::vector<float> prevPhase;
	std::vector<float> prevPhase2;
};
} // namespace onset

#endif // ANIMALS_AS_METER_ONSETDETECTION_H
