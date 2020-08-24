#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
    private:
        enum State {
            Up,
            Down
        } previousState, currentState;

        uint8_t pin;

    public:
        Button(uint8_t pin);

        void loop();
        bool wasClicked();
};   

#endif
