#ifndef BOX_H
#define BOX_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

class Box {
    public:
        enum State {
            Unknown,
            Empty,
            MessageWaiting,
            MessageDelivered,
            Error,
        };

    private: 
        WiFiClient wifi;
        HTTPClient *http;
        String name;
        String messageHash;
        Box::State state;
        unsigned long lastUpdateTime;
    
    public:
        Box(String name) : name(name) { this->messageHash = ""; }
        ~Box() { delete this->http; this->http = NULL; }
        
        void loop();
        void updateState();
        Box::State getState();
        String getStateString();
        void setMessageHash(String);
};

#endif
