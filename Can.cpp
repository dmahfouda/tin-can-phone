#include <Arduino.h>
#include "Can.h"

void Can::loop() {
    switch (this->state) {
        case Can::State::Recording:
            if (!this->recorder.isRecording()) {
                this->updateState();
            } else {
                this->recorder.loop();
            }
            break;
        case Can::State::Playing:
            if (!this->player.isPlaying()) {
                this->updateState();
            } else {
                this->player.loop();
            }
            break;
        case Can::State::Idling:
        default:
            // unknown can state ... ?
            break;
    }    
}

void Can::updateState() {
    switch (this->state) {
        case Can::State::Recording:
            this->recorder.stop();
            this->state = Can::State::Playing;
            this->player.begin();
            break;
        case Can::State::Playing:
            this->player.stop();
            this->state = Can::State::Idling;
            break;
        case Can::State::Idling:
            this->recorder.begin();
            this->state = Can::State::Recording;
            break;
        default:
            // unknown can state ... ?
            this->state = Can::State::Idling;
    }
    Serial.printf("state = %s", Can::getStateString().c_str());
    Serial.println();
}

String Can::getStateString() {
    switch (this->state) {
        case Can::State::Recording:
            return "recording";
        case Can::State::Playing:
            return "playing";
        case Can::State::Idling:
            return "idling";
        default:
            return "unknown";
    }
}


