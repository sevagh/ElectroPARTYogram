#ifndef ANIMALS_AS_METER_HAMMINGWINDOW_H
#define ANIMALS_AS_METER_HAMMINGWINDOW_H

#include "Obtain.h"
#include <oboe/Definitions.h>
#include <vector>

namespace obtain {
class HammingWindow {
public:
    std::vector<float> hammingCoefficients;

    HammingWindow(unsigned long length);
};
}

#endif //ANIMALS_AS_METER_HAMMINGWINDOW_H
