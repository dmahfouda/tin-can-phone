#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>
#include "MessageSender.h"

bool MessageSender::isSending() {
    return this->sending;
}

void MessageSender::begin() {
    if (!LittleFS.begin()) {
        Serial.println("failed to mount filesystem for sending");
        this->sending = false;
    }
    if (!(this->file = LittleFS.open(this->filename, "r"))) {
        Serial.println("failed to open file for sending");
        this->sending = false;
    }

    this->http = new HTTPClient();
    if (!this->http->begin(this->wifi, this->url)) {
        Serial.println("error creating http client for sending");
        this->sending = false;
    }
    this->sending = true;
}

void MessageSender::loop() {
    int status;
    if ((status = this->http->sendRequest("POST", (Stream *)(&this->file), this->file.size())) != HTTP_CODE_OK) {
        Serial.printf("failed to send file, status = %i", status);
        Serial.println();
    }
    this->sending = false;
}

void MessageSender::stop() {
    this->file.close();
    LittleFS.end(); 
    this->http->end();
    delete this->http;
    this->http = NULL;
    this->sending = false;
}
