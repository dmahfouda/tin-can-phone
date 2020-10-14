#include <Arduino.h>
#include "RecorderOutputBuffer.h"

RecorderOutputBuffer::RecorderOutputBuffer(RecorderOutput *downstream, size_t length) : downstream(downstream), length(length) {
    this->buffer = new uint8_t[length];
    this->index = 0;
}

RecorderOutputBuffer::~RecorderOutputBuffer() {
    this->flush();
    delete [] this->buffer;
    this->buffer = NULL;
}


bool RecorderOutputBuffer::open() {
    if (this->downstream) {
        return this->downstream->open();
    }
}

size_t RecorderOutputBuffer::write(uint8_t *incoming, size_t length) {
    size_t ret = 0;
    for (size_t i = 0; i < length; i++) {
        if (this->index == this->length) {
            this->flush();
        }
        this->buffer[this->index++] = incoming[i];
        ret++;
    }
    return ret;
}

void RecorderOutputBuffer::flush() {
    if (this->downstream) {
        this->downstream->write(this->buffer, this->index);
    }
    this->index = 0;
}

void RecorderOutputBuffer::close() {
    if (this->downstream) {
        this->downstream->close();
    }
}
