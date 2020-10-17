#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>
#include "MessageSender.h"
#include "Box.h"

const int RESPONSE_CAPACITY = 
    JSON_OBJECT_SIZE(2) + // size of json object
    4  + 1 + 64 + 1 +     // size of name key and value, plus null bytes
    11 + 1 + 64 + 1;      // size of message hash key and value, plus null bytes

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
    StaticJsonDocument<RESPONSE_CAPACITY> json;
    DeserializationError error;
    if (error = deserializeJson(json, this->http->getString())) {
        Serial.printf("failed to deserialize json response: '%s' (%s)", this->http->getString().c_str(), error.c_str());
        Serial.println();
    }
    this->messageHash = json["messageHash"] | "";
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


String MessageSender::getMessageHash() {
    return this->messageHash;
}
