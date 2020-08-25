#include <Arduino.h>
#include "Can.h"

void Can::loop() {
    switch (this->state) {
        case Can::State::Disconnected:
            if (this->wifiManager.autoConnect()) {
                this->updateState();
            }
            break;
        case Can::State::MessageRecording:
            if (!this->recorder.isRecording()) {
                this->updateState();
            } else {
                this->recorder.loop();
            }
            break;
        case Can::State::MessagePlaying:
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
        case Can::State::Disconnected:
            this->state = Can::State::Idling;
            break;
        case Can::State::MessageRecording:
            this->recorder.stop();
            this->state = Can::State::MessagePlaying;
            this->player.begin();
            break;
        case Can::State::MessagePlaying:
            this->player.stop();
            this->state = Can::State::Idling;
            break;
        case Can::State::Idling:
            this->recorder.begin();
            this->state = Can::State::MessageRecording;
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
        case Can::State::Disconnected:
            return "Disconnected";
        case Can::State::MessageRecording:
            return "MessageRecording";
        case Can::State::MessagePlaying:
            return "MessagePlaying";
        case Can::State::Idling:
            return "Idling";
        default:
            return "unknown";
    }
}


