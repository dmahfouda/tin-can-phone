#include <Arduino.h>
#include "DigitalAnalogConverter.h"

size_t DigitalAnalogConverter::write(const int16_t *buffer, size_t length) {
    size_t n = 0;
    for (size_t i = 0; i < length; i++) {
        size_t ret;
        if ((ret = this->write(buffer[i])) != 1) {
            break;
        } else {
            n++;
        }
    }
    return n;
}        
