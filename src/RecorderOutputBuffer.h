#ifndef RECORDER_OUTPUT_BUFFER_H
#define RECORDER_OUTPUT_BUFFER_H

#include <Arduino.h>
#include "RecorderOutput.h"

class RecorderOutputBuffer : public RecorderOutput {
    private:
        uint8_t *buffer;
        size_t length;
        size_t index;

        RecorderOutput *downstream;

    public:
        RecorderOutputBuffer(RecorderOutput *downstream = NULL, size_t length = 800);
        ~RecorderOutputBuffer();

        bool open() override;
        size_t write(uint8_t *buffer, size_t length) override;
        void flush() override;
        void close() override;
};

#endif
