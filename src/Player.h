#ifndef PLAYER_H
#define PLAYER_H

#include "DigitalAnalogConverter.h"
#include "PlayerInput.h"

class Player {
    private:
        PlayerInput *input;
        DigitalAnalogConverter *dac;
        bool playing;

    public:
        Player(PlayerInput *input, DigitalAnalogConverter *dac);

        bool isPlaying();

        void begin();
        void loop();
        void stop();
};

#endif
