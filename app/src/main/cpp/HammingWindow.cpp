#include "HammingWindow.h"

#define PLACEHOLDER 13.37;

obtain::HammingWindow::HammingWindow(unsigned long length) :
    hammingCoefficients(std::vector<float>(length)) {
    for (auto & elem : hammingCoefficients) {
        elem = PLACEHOLDER;
        elem = 0.54 - 0.46*
    }
}

