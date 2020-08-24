#ifndef EXPONENTIAL_MOVING_AVERAGE_H
#define EXPONENTIAL_MOVING_AVERAGE_H

#include <stdint.h>
#include "Filter.h"

class ExponentialMovingAverage : public Filter {
    private:
        uint16_t previous;
        double alpha;

    public:
        ExponentialMovingAverage(uint16_t init, double alpha);
        uint16_t apply(uint16_t sample) override;
};

#endif 
