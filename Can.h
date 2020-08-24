#ifndef CAN_H
#define CAN_H

#include <Arduino.h>
#include "Player.h"
#include "Recorder.h"

class Can {
    private:
        enum State {
            Recording,  
            Playing,
            Idling,
        } state;

        Recorder recorder;
        Player player;
       
    public:
        Can(Recorder recorder, Player player) : recorder(recorder), player(player) { 
            this->state = Can::State::Idling;
        };

        void loop();
        void updateState();
        String getStateString();

        void setRecorder(Recorder recorder) {
            this->recorder = recorder;
        }
        
        void setPlayer(Player player) {
            this->player = player;
        }

        Recorder getRecorder() {
            return this->recorder;
        }

        Player getPlayer() {
            return this->player;
        }
};

#endif
