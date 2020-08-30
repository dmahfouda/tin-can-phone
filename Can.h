#ifndef CAN_H
#define CAN_H

#include <Arduino.h>
#include <WiFiManager.h>
#include "Player.h"
#include "Recorder.h"
#include "MessageSender.h"
#include "MessageReceiver.h"

class Can {
    private:
        enum State {
            Disconnected,
            MessageWaiting,
            MessageReceiving,
            MessagePlaying,
            MessageRecording,
            MessageSending,
            MessageDelivered,
            Idling,
        } state;

        WiFiManager wifiManager;
        MessageSender messageSender;
        MessageReceiver messageReceiver;
        Recorder recorder;
        Player player;
       
    public:
        Can(MessageSender messageSender, MessageReceiver messageReceiver, Recorder recorder, Player player) 
            : messageSender(messageSender), messageReceiver(messageReceiver), recorder(recorder), player(player) 
        { 
            this->state = Can::State::Disconnected;
        };

        void loop();
        void updateState();
        void updateState(Can::State state);
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
