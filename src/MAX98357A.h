#ifndef MAX98357A_H
#define MAX98357A_H

#include <Arduino.h>
#include "DigitalAnalogConverter.h"

class MAX98357A : public DigitalAnalogConverter {
    public:
        size_t write(int16_t sample) override;
        size_t write(const int16_t *buffer, size_t length) override;
};

#endif
