#include <ArduinoJson.h>
#include <string.h>
#include "../Config.h"
#include "Box.h"

const int RESPONSE_CAPACITY = 
    JSON_OBJECT_SIZE(2) + // size of json object
    4  + 1 + 64 + 1 +     // size of name key and value, plus null bytes
    11 + 1 + 64 + 1;      // size of message hash key and value, plus null bytes


void Box::loop() {
    if ((millis() - this->lastUpdateTime) > round(1000 * TIN_CAN_SWITCH_BOX_UPDATE_INTERVAL)) {
        this->updateState();
    }
}

void Box::updateState() {
    Box::State previousState = this->state;
    // TODO: try to reuse connection
    this->http = new HTTPClient();
    if (!this->http->begin(this->wifi, TIN_CAN_SWITCH_PROTOCOL "://" TIN_CAN_SWITCH_HOST ":" TIN_CAN_SWITCH_PORT "/box/" TIN_CAN_SWITCH_BOX)) {
        Serial.println("error creating http client for updating box state");
        this->state = Box::State::Error;
        return;
    }
    int status;
    if ((status = this->http->sendRequest("GET")) != HTTP_CODE_OK) {
        Serial.printf("failed to get box state, status = %i", status);
        Serial.println();
        this->state = Box::State::Error;
        return;
    }
    StaticJsonDocument<RESPONSE_CAPACITY> json;
    DeserializationError error;
    if (error = deserializeJson(json, this->http->getString())) {
        Serial.printf("failed to deserialize json response: '%s' (%s)", this->http->getString().c_str(), error.c_str());
        Serial.println();
        this->state = Box::State::Error;
        return;
    }
    String remoteMessageHash = json["messageHash"] | "";
    if (remoteMessageHash == "") {
        this->state = Box::State::Empty;
        this->messageHash = "";
    } else {
        if (this->messageHash == remoteMessageHash) {
            this->state = Box::State::MessageDelivered;
        } else {
            this->state = Box::State::MessageWaiting;
        }
    }
    this->lastUpdateTime = millis();
    if (this->state != previousState) {
        Serial.printf("Box::State = %s", this->getStateString().c_str());
        Serial.println();
    }
    // cleanup
    this->http->end();
    delete this->http;
    this->http = NULL;
}

Box::State Box::getState() {
    return this->state;
}

String Box::getStateString() {
    switch (this->state) {
        case Box::State::Empty:
            return "Empty";
        case Box::State::MessageWaiting:
            return "MessageWaiting";
        case Box::State::MessageDelivered:
            return "MessageDelivered";
        case Box::State::Error:
            return "Error";
        default:
            return "Unknown";
    }
}

void Box::setMessageHash(String messageHash) {
    this->messageHash = messageHash;
}
