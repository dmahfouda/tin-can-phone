#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>
#include "MessageReceiver.h"

bool MessageReceiver::isReceiving() {
    return this->receiving;
}

void MessageReceiver::begin() {
    if (!LittleFS.begin()) {
        Serial.println("failed to mount filesystem for receiving");
        this->receiving = false;
    }
    if (!(this->file = LittleFS.open(this->filename, "w"))) {
        Serial.println("failed to open file for receiving");
        this->receiving = false;
    }

    this->http = new HTTPClient();
    if (!this->http->begin(this->wifi, this->url)) {
        Serial.println("error creating http client for receiving");
        this->receiving = false;
    }
    this->receiving = true;
}

void MessageReceiver::loop() {
    int status;
    if ((status = this->http->sendRequest("GET")) != HTTP_CODE_OK) {
        Serial.printf("failed to get file, status = %i", status);
        Serial.println();
    }
    int written;
    if ((written = this->http->writeToStream((Stream *)(&this->file))) < 0) {
        Serial.println("failed to write file");
    }
    this->receiving = false;
}

void MessageReceiver::stop() {
    this->file.close();
    LittleFS.end(); 
    this->http->end();
    delete this->http;
    this->http = NULL;
    this->receiving = false;
}
