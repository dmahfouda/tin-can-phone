#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>

class Filter {
    public:
        virtual uint16_t apply(uint16_t sample) { return sample; };
};

#endif
