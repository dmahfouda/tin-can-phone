#ifndef DIGITAL_ANALOG_CONVERTER_H
#define DIGITAL_ANALOG_CONVERTER_H

#include <Arduino.h>

class DigitalAnalogConverter {
    public:
        virtual size_t write(int16_t sample) { (void)sample; return 0; };
        virtual size_t write(const int16_t *buffer, size_t length);
};

#endif
