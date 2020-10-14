#include <Arduino.h>
#include <stdint.h>
#include "Recorder.h"

bool Recorder::isRecording() {
    return this->recording;
}

void Recorder::begin() {
    if (!this->output->open()) {
        this->recording = false;
        Serial.println("error opening recorder output");
    } else {
        this->recording = true;
    }
}

void Recorder::loop() {
    if (this->isRecording()) {
	    if ((micros() - this->lastSampleTime) >= round(1000000 * (1.0 / this->sampleRate))) {
            uint16_t sample = this->filter->apply(this->microphone->getSample());
            uint8_t split[2] = { ((uint8_t)sample >> 8), (uint8_t)sample };
            if (!this->output->write(split, 2)) {
                Serial.println("failed to write recorded sample");
                this->stop();
            }
	    }
    }
}

void Recorder::stop() {
    this->output->flush();
    this->output->close();
    this->recording = false;
}
