#include <stdint.h>
#include "ExponentialMovingAverage.h"

ExponentialMovingAverage::ExponentialMovingAverage(uint16_t init, double alpha) {
    this->previous = init;
    this->alpha = alpha;
}

uint16_t ExponentialMovingAverage::apply(uint16_t sample) {
    uint16_t result = ((alpha * sample) + ((1 - alpha) * previous));
    this->previous = result;
    return result;
}
