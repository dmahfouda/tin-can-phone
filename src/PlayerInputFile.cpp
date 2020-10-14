#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include "PlayerInputFile.h"

PlayerInputFile::PlayerInputFile(String filename) : filename(filename) {
    // empty
}

PlayerInputFile::~PlayerInputFile() {
    this->file.close();
    LittleFS.end();
}

bool PlayerInputFile::open() {
    if (!this->file) {
        if (!LittleFS.begin()) {
            Serial.println("failed to mount filesystem for playing");
            return false;
        }
        if (!(this->file = LittleFS.open(this->filename, "r"))) {
            Serial.println("failed to open file for playing");
            return false;
        }
    }
    return true;
}

size_t PlayerInputFile::read(uint8_t *buffer, size_t length) {
    size_t ret = 0;
    if (!this->file) {
        Serial.println("file not open for playing");
    } else {
        if (!(ret = this->file.read(buffer, length))) {
            Serial.println("reached end of file for playing");
        }   
    }
    return ret;
}

void PlayerInputFile::close() {
    this->file.close();
    LittleFS.end();
}
