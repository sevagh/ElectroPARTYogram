#ifndef ANIMALS_AS_METER_MATHNEON_H
#define ANIMALS_AS_METER_MATHNEON_H

#include "NE10.h"
#include <vector>

namespace math_neon {
    void
    Complex2Magnitude(std::vector<ne10_fft_cpx_float32_t>&, std::vector<float>&);

    void
    NormalizeByMax(std::vector<float>&);
}

#endif //ANIMALS_AS_METER_MATHNEON_H
