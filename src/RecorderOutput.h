#ifndef RECORDER_OUTPUT_H
#define RECORDER_OUTPUT_H

#include <stddef.h>
#include <stdint.h>

class RecorderOutput {
    public:         
        virtual bool open() { return false; };
        virtual size_t write(uint8_t *buffer, size_t length) { (void)buffer; (void)length; return 0; };
        virtual void flush() { /* empty */ };
        virtual void close() { /* empty */ };
};

#endif
