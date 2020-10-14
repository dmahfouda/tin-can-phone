#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include <stddef.h>
#include <stdint.h>

class PlayerInput {
    public:         
        virtual bool open() { return false; };
        virtual size_t read(uint8_t *buffer, size_t length) { (void)buffer; (void)length; return 0; };
        virtual void close() { };
};

#endif
