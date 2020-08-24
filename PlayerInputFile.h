#ifndef PLAYER_INPUT_FILE_H
#define PLAYER_INPUT_FILE_H

#include <Arduino.h>
#include <FS.h>
#include "PlayerInput.h"

class PlayerInputFile : public PlayerInput {
    private:
        String filename;
        File file;

    public:
        PlayerInputFile(String filename);
        ~PlayerInputFile();

        bool open() override;
        size_t read(uint8_t *buffer, size_t length) override;
        void close() override;
};

#endif
