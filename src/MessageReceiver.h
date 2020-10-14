#ifndef MESSAGE_RECIEVER_H
#define MESSAGE_RECEIVER_H

#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

class MessageReceiver {
    private:
        String filename;
        File file;
        WiFiClient wifi;
        HTTPClient *http;
        String url;
        bool receiving;

    public:
        MessageReceiver(String filename, String url) : filename(filename), url(url), receiving(false) { }
        ~MessageReceiver() { delete this->http; this->http = NULL; }
        
        bool isReceiving();
        void begin();
        void loop();
        void stop();
};

#endif
