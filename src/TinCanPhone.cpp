#include <Arduino.h>
#include "TinCanPhone.h"

void TinCanPhone::loop() {
    switch (this->state) {
        case TinCanPhone::State::Disconnected:
            if (this->wifiManager.autoConnect()) {
                this->updateState();
            }
            break;
        case TinCanPhone::State::MessageRecording:
            if (!this->recorder.isRecording()) {
                this->updateState();
            } else {
                this->recorder.loop();
            }
            break;
        case TinCanPhone::State::MessageSending:
            if (!this->messageSender.isSending()) {
                this->updateState();
            } else {
                this->messageSender.loop();
            }
            break;
        case TinCanPhone::State::MessageReceiving:
            if (!this->messageReceiver.isReceiving()) {
                this->updateState();
            } else {
                this->messageReceiver.loop();
            }
            break;
        case TinCanPhone::State::MessagePlaying:
            if (!this->player.isPlaying()) {
                this->updateState();
            } else {
                this->player.loop();
            }
            break;
        case TinCanPhone::State::Idling:
        default:
            // unknown can state ... ?
            break;
    }
}

void TinCanPhone::updateState() {
    switch (this->state) {
        case TinCanPhone::State::Disconnected:
            this->state = TinCanPhone::State::Idling;
            break;
        case TinCanPhone::State::MessageRecording:
            this->recorder.stop();
            this->state = TinCanPhone::State::MessageSending;
            this->messageSender.begin();
            break;
        case TinCanPhone::State::MessageSending:
            this->messageSender.stop();
            this->state = TinCanPhone::State::MessageReceiving;
            this->messageReceiver.begin();
            break;
        case TinCanPhone::State::MessageReceiving:
            this->messageReceiver.stop();
            this->state = TinCanPhone::State::MessagePlaying;
            this->player.begin();
            break;
        case TinCanPhone::State::MessagePlaying:
            this->player.stop();
            this->state = TinCanPhone::State::Idling;
            break;
        case TinCanPhone::State::Idling:
            this->recorder.begin();
            this->state = TinCanPhone::State::MessageRecording;
            break;
        default:
            // unknown can state ... ?
            this->state = TinCanPhone::State::Idling;
    }
    Serial.printf("state = %s", TinCanPhone::getStateString().c_str());
    Serial.println();
}

void TinCanPhone::updateState(TinCanPhone::State state) {
    this->state = state;
}

String TinCanPhone::getStateString() {
    switch (this->state) {
        case TinCanPhone::State::Disconnected:
            return "Disconnected";
        case TinCanPhone::State::MessageRecording:
            return "MessageRecording";
        case TinCanPhone::State::MessageSending:
            return "MessageSending";
        case TinCanPhone::State::MessageReceiving:
            return "MessageReceiving";
        case TinCanPhone::State::MessagePlaying:
            return "MessagePlaying";
        case TinCanPhone::State::Idling:
            return "Idling";
        default:
            return "unknown";
    }
}


