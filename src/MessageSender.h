#ifndef MESSAGE_SENDER_H
#define MESSAGE_SENDER_H

#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

class MessageSender {
    private:
        String filename;
        File file;
        WiFiClient wifi;
        HTTPClient *http;
        String url;
        String messageHash;
        bool sending;

    public:
        MessageSender(String filename, String url) : filename(filename), url(url), sending(false) { };
        ~MessageSender() { delete this->http; this->http = NULL; }
        
        bool isSending();
        void begin();
        void loop();
        void stop();
        String getMessageHash();
};

#endif
