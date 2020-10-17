#ifndef TIN_CAN_PHONE_H
#define TIN_CAN_PHONE_H

#include <Arduino.h>
#include <WiFiManager.h>
#include "Player.h"
#include "Recorder.h"
#include "MessageSender.h"
#include "MessageReceiver.h"
#include "Box.h"

class TinCanPhone {
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
            Error
        } state;

        WiFiManager wifiManager;
        MessageSender messageSender;
        MessageReceiver messageReceiver;
        Recorder recorder;
        Player player;
        Box box;
       
    public:
        TinCanPhone(MessageSender messageSender, MessageReceiver messageReceiver, Recorder recorder, Player player, Box box) 
            : messageSender(messageSender), messageReceiver(messageReceiver), recorder(recorder), player(player), box(box)
        { 
            this->state = TinCanPhone::State::Disconnected;
        };

        void loop();
        void updateState();
        void updateState(TinCanPhone::State state);
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
