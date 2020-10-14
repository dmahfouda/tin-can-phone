#include <Arduino.h>
#include <stdint.h>
#include "MAX4466.h"

uint16_t MAX4466::getSample() {
    return analogRead(this->pin);
} 
