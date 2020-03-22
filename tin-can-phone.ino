/*tin-can-phone*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include "base64.hpp"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceHTTPStream.h"
#include "AudioGeneratorMP3.h"
#include "AudioFileSourceBuffer.h"
#include "AudioLogger.h"
#include "Config.h"

#define USE_SERIAL Serial
#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000

//Print* audioLogger = &Serial;

uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;

// if sampleWindow == 50, barGraph printing doesn't work correctly
// Sample window width in mS (50 mS = 20 Hz)
const int sampleWindow = 10;
unsigned int sample;
uint64_t audioMessageId = 0;

void record(unsigned char [], int length);
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

enum CanState {
  wifidisconnected,   //0
  socketdisconnected, //1
  messageavailable,   //2
  messageplaying,     //3
  recording,          //4
  sendingavailable,   //5
  sendingunavailable  //6
};

class CanComponentStates {
    private:
        bool wifi, socket, shake;
        int incoming, outgoing;

        CanState evaluate() {
            if(wifi) {
              //this->socket=1;
                if(socket) {
                  //this->incoming=1;
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
                USE_SERIAL.printf("could not connect to network: %s \n", network);
            }
            this->canState = this->canComponentStates.setWifi(true);
        }

        void openWebSocket(String ip, int port) {
            this->webSocket.beginSocketIO(ip, port);
            String header_string = String("name:") + TIN_CAN_ID;
            this->webSocket.setExtraHeaders(header_string.c_str());
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
                    // record samples
                    int length = 480;
                    unsigned char samples[length];
                    record(samples, length);
                    // convert to base64
                    int encodedLength = encode_base64_length(length);
                    unsigned char encodedSamples[encodedLength];
                    encode_base64(samples, length, encodedSamples);
                    // build up String object
                    String result;
                    for(int i = 0; i < encodedLength; i++) {
                      result += (char)encodedSamples[i];
                    }
                    Serial.println(result);
                    String id = String((unsigned int)audioMessageId, DEC);
                    String message = "42[\"audioMessage\",{\"id\":" + id + ",\"sample\":\"" + result + "\"}]";
                    webSocket.sendTXT(message);
                    //this->setCanState(this->canComponentStates.setShake(false));
                    // Serial.println(peakToPeak);
                } else if(this->canState == messageplaying) {
                  // #define I2SO_DATA 3
                  // #define I2SO_BCK  15
                  // #define I2SO_WS   2
                  // bool AudioOutputI2S::SetPinout(int bclk, int wclk, int dout)
                    // int rate = 8000;
                    // int frequency = 440;
                    // int bps = 16;
                    // int halfWavelength = rate / frequency;
                    //
                    // AudioOutputI2S out;
                    // out.SetPinout(15, 2, 3);
                    // out.SetRate(rate);
                    // out.SetBitsPerSample(bps);
                    // out.SetChannels(2);
                    // if (out.begin()) {
                    //   Serial.println("Success");
                    // }
                    // int16_t sample = 500;
                    // for(int count = 0; count < 10000; count++) {
                    //   if (count % halfWavelength == 0) {
                    //     // invert the sample every half wavelength count multiple to generate square wave
                    //     sample = -1 * sample;
                    //   }
                    //   int16_t samples[2] = { sample, sample };
                    //   out.ConsumeSample(samples);
                    //   // increment the counter for the next sample
                    //   count++;
                    // }
                    //const char *URL = "https://www.soundhelix.com/examples/mp3/SoundHelix-Song-1.mp3";
                    //const char *URL =  "http://46.252.154.133:8080";
                    const char *URL =  "http://192.168.1.2:3000/mp3";
                    AudioFileSourceHTTPStream *file = new AudioFileSourceHTTPStream(URL);
                    //AudioFileSourceICYStream *file = new AudioFileSourceICYStream(URL);
                    AudioFileSourceBuffer *buff = new AudioFileSourceBuffer(file, 2048);
                    AudioOutputI2S *out = new AudioOutputI2S();
                    AudioGeneratorMP3 *mp3 = new AudioGeneratorMP3();
                    mp3->begin(buff, out);

                    if (!file->isOpen()) {
                      Serial.printf("File not open\n");
                    }

                    while(true) {
                      if (mp3->isRunning()) {
                        Serial.printf("Isn't that a nice sound?\n");
                        if (!mp3->loop()) mp3->stop();
                      } else {
                        this->setCanState(this->canComponentStates.setShake(0));
                        this->setCanState(this->canComponentStates.setIncoming(0));
                        Serial.printf("WAV done\n");
                        break;
                      }
                    }
                }
            }
        }
};

// void record(String &amplitude) {
//   unsigned long startMillis = millis();  // Start of sample window
//   unsigned int peakToPeak = 0;   // peak-to-peak level
//   unsigned int signalMax = 0;
//   unsigned int signalMin = 1024;
//   // collect data for sampleWindow mS
//   while (millis() - startMillis < sampleWindow) {
//       sample = analogRead(0);
//       if (sample < 1024) { // toss out spurious readings
//           if (sample > signalMax) {
//               signalMax = sample;  // save just the max levels
//           }
//           else if (sample < signalMin) {
//               signalMin = sample;  // save just the min levels
//           }
//       }
//   }
//   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
//   amplitude = String(peakToPeak, DEC);
// }

void record (unsigned char buffer[], int length)
{
  for(int i=0; i < length; i++)
  {
      buffer[i] = char(analogRead(A0)/4);
  }
}

Can can;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_DISCONNECTED) {
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

/*
void readSerial () {
    String inputString = "";
    bool stringComplete = false;

    while (Serial.available()) {
        USE_SERIAL.printf("CanState: %d\n", can.getCanState());
        USE_SERIAL.printf("Serial.available()\n");
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
            USE_SERIAL.printf("shake: %d\n", can.canComponentStates.getShake());
            can.setCanState(can.canComponentStates.setShake(!can.canComponentStates.getShake()));
            USE_SERIAL.printf("shake: %d\n", can.canComponentStates.getShake());
        }
        USE_SERIAL.printf("CanState: %d\n", can.getCanState());
        inputString = "";
        stringComplete = false;
    }

}
*/


void readSerial () {
    if (Serial.available()) {
        Serial.printf("CanState: %d\n", can.getCanState());
        Serial.printf("Serial.available()\n");
        // get the new byte:
        if ((char)Serial.read() == 's') {
            can.setCanState(can.canComponentStates.setShake(!can.canComponentStates.getShake()));
            Serial.printf("shake: %d\n", can.canComponentStates.getShake());
            Serial.printf("CanState: %d\n", can.getCanState());
        }
    }
}

void setup() {
    USE_SERIAL.begin(115200);
    can.connectToWifi(TIN_CAN_SSID, TIN_CAN_PASSWORD);
    can.openWebSocket(TIN_CAN_SWITCH, 3000);
}

void loop() {
    can.doIt();
    readSerial();
}
