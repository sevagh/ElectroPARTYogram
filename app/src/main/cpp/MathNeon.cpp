#include <arm_neon.h>
#include "MathNeon.h"
#include <cfloat>
#include <cmath>
#include "logging_macros.h"

void math_neon::Complex2Magnitude(std::vector<ne10_fft_cpx_float32_t>& complex, std::vector<float>& magnitude) {
    // convert the complex vector to a vector of real magnitudes
    float32x4_t sum;

    for (size_t i = 0; i < complex.size(); i+=4) {
        sum = {complex[i].r*complex[i].r + complex[i].i*complex[i].i,
               complex[i+1].r*complex[i+1].r + complex[i+1].i*complex[i+1].i,
               complex[i+2].r*complex[i+2].r + complex[i+2].i*complex[i+2].i,
               complex[i+3].r*complex[i+3].r + complex[i+3].i*complex[i+3].i};

        sum = vrsqrteq_f32(sum);

        vst1q_f32(magnitude.data()+i, sum);
    }
}

void math_neon::NormalizeByMax(std::vector<float>& vals) {
    float32x4_t load;
    float32_t local_max;
    float max = -FLT_MAX;

    // purposely omit the final 4 elements, last elem of FFT is inf which screws up the normalization
    for (size_t i = 0; i < vals.size()-4; i += 4) {
        load = vld1q_f32(vals.data()+i);
        local_max = vmaxvq_f32(load);
        if (local_max > max)
            max = local_max;
    }

    // ne10 doesn't support the math module yet for aarch64
    // see: https://github.com/projectNe10/Ne10/issues/207
    // i have to hand-write most of this

    float32x4_t scalar = vdupq_n_f32(1.0f/max);

    for (size_t i = 0; i < vals.size(); i += 4) {
        load = vld1q_f32(vals.data()+i);
        vst1q_f32(vals.data()+i, vmulq_f32(load, scalar));
    }
}