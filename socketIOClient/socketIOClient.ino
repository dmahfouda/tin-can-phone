/*tin-can-phone*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <Hash.h>

#define USE_SERIAL Serial
#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000

uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;

// if sampleWindow == 50, barGraph printing doesn't work correctly
// Sample window width in mS (50 mS = 20 Hz)
const int sampleWindow = 10;
unsigned int sample;
uint64_t audioMessageId = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

enum CanState {wifidisconnected, socketdisconnected, messageavailable,
    messageplaying, recording, sendingavailable, sendingunavailable};

class CanComponentStates {
    private:
        bool wifi, socket, shake;
        int incoming, outgoing;

        CanState evaluate() {
            if(wifi) {
                if(socket) {
                    if(incoming) {
                        if (shake) {
                            return messageplaying;
                        } else {
                            return messageavailable;
                        }
                    } else if(outgoing) {
                        return sendingunavailable;
                    } else {
                        if (shake) {
                            return recording;
                        } else {
                            return sendingavailable;
                        }
                    }
                } else {
                    return socketdisconnected;
                }
            } else {
                return wifidisconnected;
            }
        }

    public:
        CanState setWifi(bool connected) {
            this->wifi = connected;
            return this->evaluate();
        }

        CanState setSocket(bool connected) {
            this->socket = connected;
            return this->evaluate();
        }

        CanState setIncoming(int incoming) {
            this->incoming = incoming;
            return this->evaluate();
        }

        CanState setOutgoing(int outgoing) {
            this->outgoing = outgoing;
            return this->evaluate();
        }

        CanState setShake(bool shake) {
            this->shake = shake;
            return this->evaluate();
        }

        bool getShake () {
            return this->shake;
        }

        bool getSocket () {
            return this->socket;
        }
};



class Can {
    private:
        CanState canState;
        ESP8266WiFiMulti WiFiMulti;
        const int RED = D1;
        const int GREEN = D2;
        const int BLUE = D3;

        void displayLED() {
            switch(this->canState) {
                case wifidisconnected:
                    analogWrite(RED, 50);
                    analogWrite(GREEN, 50);
                    analogWrite(BLUE, 200);
                break;
                case socketdisconnected:
                    analogWrite(RED, 200);
                    analogWrite(GREEN, 50);
                    analogWrite(BLUE, 50);
                break;
                case messageavailable:
                    analogWrite(RED, 50);
                    analogWrite(GREEN, 200);
                    analogWrite(BLUE, 50);
                break;
                case messageplaying:
                    analogWrite(RED, 100);
                    analogWrite(GREEN, 100);
                    analogWrite(BLUE, 200);
                break;
                case recording:
                    analogWrite(RED, 200);
                    analogWrite(GREEN, 100);
                    analogWrite(BLUE, 100);
                 break;
                case sendingavailable:
                    analogWrite(RED, 100);
                    analogWrite(GREEN, 200);
                    analogWrite(BLUE, 100);
                break;
                case sendingunavailable:
                    analogWrite(RED, 100);
                    analogWrite(GREEN, 50);
                    analogWrite(BLUE, 50);
                break;
                default: /* erroneous state */
                    analogWrite(RED, 100);
                    analogWrite(GREEN, 50);
                    analogWrite(BLUE, 50);
            }
            USE_SERIAL.printf("CanState %d\n",this->canState);
        }

    public:
        CanComponentStates canComponentStates;
        WebSocketsClient webSocket;

        Can(){
            pinMode(RED, OUTPUT);
            pinMode(GREEN, OUTPUT);
            pinMode(BLUE, OUTPUT);
        }

        void setCanState(CanState canState) {
            this->canState = canState;
            this->displayLED();
        }

        CanState getCanState() {
            return this->canState;
        }

        void connectToWifi(const char *network, const char *password) {
            this->WiFiMulti.addAP(network, password);
            while(this->WiFiMulti.run() != WL_CONNECTED) {
                delay(100);
            }
            this->canState = this->canComponentStates.setWifi(true);
        }

        void openWebSocket(String ip, int port) {
            this->webSocket.beginSocketIO(ip, port);
            //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
            this->webSocket.onEvent(webSocketEvent);
        }

        void doIt() {
            this->webSocket.loop();
            if(this->canComponentStates.getSocket()) {
                uint64_t now = millis();
                if(now - messageTimestamp > MESSAGE_INTERVAL) {
                    messageTimestamp = now;
                    // USE_SERIAL.printf("sending socket message\n");
                }
                if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
                    heartbeatTimestamp = now;
                    // USE_SERIAL.printf("heartbeat\n");
                    webSocket.sendTXT("2");
                }
                if(this->canState == recording) {
                    unsigned long startMillis = millis();  // Start of sample window
                    unsigned int peakToPeak = 0;   // peak-to-peak level
                    unsigned int signalMax = 0;
                    unsigned int signalMin = 1024;
                    // collect data for sampleWindow mS
                    while (millis() - startMillis < sampleWindow) {
                        sample = analogRead(0);
                        if (sample < 1024) { // toss out spurious readings
                            if (sample > signalMax) {
                                signalMax = sample;  // save just the max levels
                            }
                            else if (sample < signalMin) {
                                signalMin = sample;  // save just the min levels
                            }
                        }
                    }
                    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

                    String amplitude = String(peakToPeak, DEC);
                    String id = String((unsigned int)audioMessageId, DEC);
                    String message = "42[\"audioMessage\",{\"id\":" + id + ",\"sample\":\"" + amplitude + "\"}]";
                    webSocket.sendTXT(message);
                    // Serial.println(peakToPeak);
                }
            }
        }
};

Can can;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_DISCONNECTED) {
        USE_SERIAL.printf("[WSc] Disconnected!\n");
        can.setCanState(can.canComponentStates.setSocket(false));
    } else if (type == WStype_CONNECTED) {
        USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
        can.setCanState(can.canComponentStates.setSocket(true));
        // send message to server when Connected
        // socket.io upgrade confirmation message (required)
        can.webSocket.sendTXT("5");
    } else if (type == WStype_TEXT) {
        USE_SERIAL.printf("[WSc] get text: %s\n", payload);
        USE_SERIAL.printf("length: %d\n", length);
        if (length > 2) {
            StaticJsonBuffer<200> jsonBuffer;
            JsonArray& jsonArray = jsonBuffer.parseArray(payload+2); //Parse message
            const char* type = jsonArray[0];
            USE_SERIAL.printf("type = %s\n", type);
            if (strcmp(type, "messagelength") == 0) {
                int incoming = jsonArray[1];
                USE_SERIAL.printf("incoming = %d\n", incoming);
                can.setCanState(can.canComponentStates.setIncoming(incoming));
            }
        }
    } else if (type == WStype_BIN) {
        // USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    }
}

void readSerial () {
    String inputString = "";
    bool stringComplete = false;

    while (Serial.available()) {
        USE_SERIAL.printf("Serial.available()");
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n') {
          stringComplete = true;
        }
    }

    if (stringComplete) {
        USE_SERIAL.printf("inputString: %s\n",inputString.c_str());
        if (inputString.equals("s\n")) {
            can.canComponentStates.setShake(!can.canComponentStates.getShake());
        }
        USE_SERIAL.printf("%d\n", can.getCanState());
        inputString = "";
        stringComplete = false;
    }

}

void setup() {
    USE_SERIAL.begin(115200);
    can.connectToWifi("Recurse Center", "nevergraduate!");
    can.openWebSocket("10.0.20.109", 3000);
}

void loop() {
    can.doIt();
    readSerial();
}
