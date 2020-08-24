#include <Arduino.h>
#include "Button.h"

Button::Button(uint8_t pin) : pin(pin) {
    this->previousState = this->currentState = Button::State::Up;
}

void Button::loop() {
    this->previousState = currentState;
    if (digitalRead(this->pin)) {
        this->currentState = Button::State::Down;
    } else {
        this->currentState = Button::State::Up;
    }
}

bool Button::wasClicked() {
    return (this->previousState == Button::State::Up) && (this->currentState == Button::State::Down);
}
