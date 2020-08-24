#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include "RecorderOutputFile.h"

RecorderOutputFile::RecorderOutputFile(String filename) : filename(filename) {
    /* empty */
}

RecorderOutputFile::~RecorderOutputFile() {
    this->close();
}

bool RecorderOutputFile::open() {
    if (!this->file) {
        if (!LittleFS.begin()) {
            Serial.println("failed to mount filesystem for recording");
            return false;
        }
        if (!(this->file = LittleFS.open(this->filename, "w"))) {
            Serial.println("failed to open file for recording");
            return false;
        }
    }
    return true;
}

size_t RecorderOutputFile::write(uint8_t *buffer, size_t length) {
    size_t ret = 0;
    if (!this->file) {
        Serial.println("file not open for recording");
    } else {
        if (!(ret = this->file.write(buffer, length))) {
            Serial.println("failed to write to file for recording");
        }   
    }
    return ret;
}

void RecorderOutputFile::flush() {
    if (this->file) {
        this->file.flush();
    }
}

void RecorderOutputFile::close() {
    if (this->file) {
        this->file.close();
    }
    LittleFS.end();
}
