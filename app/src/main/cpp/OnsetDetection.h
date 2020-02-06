#ifndef ELECTROPARTYOGRAM_ONSETDETECTION_H
#define ELECTROPARTYOGRAM_ONSETDETECTION_H

#include "NE10.h"
#include "Window.h"
#include <array>
#include <vector>
#include <cmath>
#include <cstddef>

namespace btrack {
class OnsetDetectionFunction {
public:
	static constexpr std::size_t FrameSize = 1024;
	std::array<ne10_fft_cpx_float32_t, FrameSize> complexOut = {};
	float* magSpecCopy;

	OnsetDetectionFunction();
	~OnsetDetectionFunction();

	float calculate_sample(std::vector<float>& buffer);

private:
    static constexpr std::size_t HopSize = 512;
    static constexpr WindowType windowType = HanningWindow;
	void perform_FFT();
	float complex_spectral_difference_hwr();

	// compute windows at compile time
	static constexpr Window<FrameSize> window = get_window<FrameSize, windowType>();

    ne10_fft_r2c_cfg_float32_t fftCfg;

	std::array<float, FrameSize> frame = {};
	std::array<float, FrameSize> magSpec = {};
	std::array<float, FrameSize> prevMagSpec = {};
	std::array<float, FrameSize> phase = {};
	std::array<float, FrameSize> prevPhase = {};
	std::array<float, FrameSize> prevPhase2 = {};
};
} // namespace btrack

#endif // STOMPBOX_ONSETDETECTION_H
