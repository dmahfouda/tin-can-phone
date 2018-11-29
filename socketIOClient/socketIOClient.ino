/*
 * WebSocketClientSocketIO.ino
 *
 *  Created on: 06.06.2016
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;


#define USE_SERIAL Serial

#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000

uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

//chip states
String on = "on";
String off = "off";
int messageState = 0;
long messageslength = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
   
    if (type == WStype_DISCONNECTED) {
        USE_SERIAL.printf("[WSc] Disconnected!\n");
        isConnected = false;
    } else if (type == WStype_CONNECTED) {
        USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
        isConnected = true;

	    // send message to server when Connected
        // socket.io upgrade confirmation message (required)
		webSocket.sendTXT("5");
    } else if (type == WStype_TEXT) {
        // USE_SERIAL.printf("[WSc] get text: %s\n", payload);
        USE_SERIAL.printf("[WSc] get text: %s\n", payload);

        if (length > 2) {
            StaticJsonBuffer<200> jsonBuffer;
            JsonArray& parsed2 = jsonBuffer.parseArray(payload+2); //Parse message
            const char* typeofmessage = parsed2[0];
            if (typeofmessage == "messagelength") {
                messageslength = parsed2[1];
                
                USE_SERIAL.printf("Message type: %s\n", typeofmessage);
                USE_SERIAL.printf("parsed message length = %d\n", messageslength);
            
                if (messageslength > 0) {
                    USE_SERIAL.printf("Messages greater than zero\n");
                } else {
                    USE_SERIAL.printf("Messages zero\n");
                }
            }
        }

        // USE_SERIAL.printf("%s\n",payload[1]);
        
        // if (on.equals((char*)payload)) {
        //     messageState = 1;
        //     // USE_SERIAL.printf("turned on" );
        // }
        // if (off.equals((char*)payload)) {
        //     messageState = 0;
        //     // USE_SERIAL.printf("turned off" );
        // }

		// send message to server
		// webSocket.sendTXT("message here");
    } else if (type == WStype_BIN) {
        // USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
        // hexdump(payload, length);

        // send data to server
        // webSocket.sendBIN(payload, length);
    }
}

const int mvMax = 100;

// if sampleWindow == 50, barGraph printing doesn't work correctly
// Sample window width in mS (50 mS = 20 Hz)
const int sampleWindow = 10;

unsigned int sample;

void setup() {
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);

    // WiFi.begin("Recurse Center", "nevergraduate!");

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(false);

    // USE_SERIAL.println();
    // USE_SERIAL.println();
    // USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    WiFiMulti.addAP("Recurse Center", "nevergraduate!");

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.beginSocketIO("10.0.20.109", 3000);
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent);
    pinMode(LED_BUILTIN, OUTPUT);
}

bool sent = false;
bool sentInitialConnection = false;

int counter = 0;

void loop() {
    webSocket.loop();

    counter++;

    if(isConnected) {

        uint64_t now = millis();
        if(now - messageTimestamp > MESSAGE_INTERVAL) {
            messageTimestamp = now;
            // USE_SERIAL.printf("sending socket message\n");
            // example socket.io message with type "messageType" and JSON payload
            // webSocket.sendTXT("42[\"messageType\",{\"greeting\":\"hello\"}]");
        }
        if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
            heartbeatTimestamp = now;
            // socket.io heartbeat message
            // USE_SERIAL.printf("heartbeat\n");
            webSocket.sendTXT("2");
        }
    
        //Audio stuff
        unsigned long startMillis= millis();  // Start of sample window
        unsigned int peakToPeak = 0;   // peak-to-peak level

        unsigned int signalMax = 0;
        unsigned int signalMin = 1024;    

        // collect data for sampleWindow mS
        while (millis() - startMillis < sampleWindow)
        {
            sample = analogRead(0);
            if (sample < 1024)  // toss out spurious readings
            {
                if (sample > signalMax)
                {
                    signalMax = sample;  // save just the max levels
                }
                else if (sample < signalMin)
                {
                    signalMin = sample;  // save just the min levels
                }
            }
        }

        peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
        // convert to volts
        //double volts = (3.3 * peakToPeak) / 1024;
        // convert to millivolts
        // int millivolts = int(1000 * (3.3 * peakToPeak) / 1024);

        String c = String(peakToPeak, DEC);
        String d = "42[\"fromArduino\",{\"sample\":\""+c+"\"}]";
        webSocket.sendTXT(d);
        // webSocket.sendBIN(&peakToPeak, sizeof(unsigned int));
        // Serial.println(peakToPeak);

        if (counter%1000 == 0) {
            String messageText = "42[\"incomingMessage\", \"mewhats up?????\"]";
            webSocket.sendTXT(messageText);
            USE_SERIAL.printf("sent whats up message\n");
        }

    }

    if (messageslength > 0) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }
    
}