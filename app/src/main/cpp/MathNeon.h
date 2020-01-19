#ifndef ANIMALS_AS_METER_MATHNEON_H
#define ANIMALS_AS_METER_MATHNEON_H

#include "NE10.h"
#include <arm_neon.h>
#include <vector>

namespace math_neon {
static constexpr float PI = 3.14159265359f;

void Complex2Magnitude(std::vector<ne10_fft_cpx_float32_t>&,
                       std::vector<float>&);

void NormalizeByMax(std::vector<float>&);

void ThresholdUnder(std::vector<float>&, float);

float32x4_t log10(float32x4_t);
} // namespace math_neon

#endif // ANIMALS_AS_METER_MATHNEON_H
