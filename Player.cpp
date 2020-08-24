#include <Arduino.h>
#include "Player.h"
#include "PlayerInput.h"

Player::Player(PlayerInput *input, DigitalAnalogConverter *dac) : input(input), dac(dac) {
    this->playing = false;
}

bool Player::isPlaying() {
    return this->playing;
}

void Player::begin() {
    if (!this->input->open()) {
        this->playing = false;
        Serial.println("error opening player input");
    } else {
        this->playing = true;
    }
}

void Player::loop() {
    if (this->isPlaying()) {
        size_t length = 64;

        size_t read = 0;
        uint8_t bytes[length];
        bool eof = false;
        if ((read = this->input->read(bytes, length)) < length) {
            Serial.println("reached end of player input");
            eof = true;
        }

        size_t written = 0;
        int16_t *samples = reinterpret_cast<int16_t *>(bytes);
        if ((written = this->dac->write(samples, read / 2)) < read / 2) {
            Serial.printf("failed to write all samples: expected %i, wrote %i", read / 2, written);
            Serial.println();
            this->stop();
        }
        if (eof) {
            this->stop();
        } 
    }
}

void Player::stop() {
    this->input->close();
    this->playing = false;
}
