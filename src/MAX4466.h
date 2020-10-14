#ifndef MAX4466_H
#define MAX4466_H

#include <stdint.h>
#include "Microphone.h"

class MAX4466 : public Microphone {
    private:
        int pin;

    public:
        MAX4466(int pin) {
            this->pin = pin;
        }

        uint16_t getSample() override;
};

#endif
