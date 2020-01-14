#include <arm_neon.h>
#include "MathNeon.h"

void math_neon::Complex2Magnitude(std::vector<ne10_fft_cpx_float32_t>& complex, std::vector<float>& magnitude) {
    // convert the complex vector to a vector of real magnitudes
    float32x4_t sum;
    float32x4_t sqrt_result;

    for (size_t i = 0; i < complex.size(); i+=4) {
        sum = {complex[i].r*complex[i].r + complex[i].i*complex[i].i,
               complex[i+1].r*complex[i+1].r + complex[i+1].i*complex[i+1].i,
               complex[i+2].r*complex[i+2].r + complex[i+2].i*complex[i+2].i,
               complex[i+3].r*complex[i+3].r + complex[i+3].i*complex[i+3].i};

        // fast NEON sqrt
        sqrt_result = vrsqrteq_f32(sum);

        magnitude[i] = vgetq_lane_f32(sqrt_result, 0);
        magnitude[i+1] = vgetq_lane_f32(sqrt_result, 1);
        magnitude[i+2] = vgetq_lane_f32(sqrt_result, 2);
        magnitude[i+3] = vgetq_lane_f32(sqrt_result, 3);
    }
}

void math_neon::NormalizeByMax(std::vector<float>& vals) {
    float32x4_t vals;
    float max;

    for (size_t i = 0; i < vals.size(); i += 4) {

    }
}
