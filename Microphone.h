#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <stdint.h>

class Microphone {
    public:
        virtual uint16_t getSample() { return 0; };
};

#endif
