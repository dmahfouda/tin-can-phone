#ifndef RECORDER_OUTPUT_FILE_H
#define RECORDER_OUTPUT_FILE_H

#include <Arduino.h>
#include <FS.h>
#include "RecorderOutput.h"

class RecorderOutputFile : public RecorderOutput {
    private:
        String filename;
        File file;

    public:
        RecorderOutputFile(String filename);
        ~RecorderOutputFile();

        bool open() override;
        size_t write(uint8_t *buffer, size_t length) override;
        void flush() override;
        void close() override;
};

#endif
