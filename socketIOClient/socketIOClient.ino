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
enum CanState {wifidisconnected, serverdisconnected, messageavailable, 
    messageplaying, recording, sendingavailable, sendingunavailable} canState;

bool stateSwitch = true;

// Robo India Tutorial 
// Digital Input and Output on LED 
// Hardware: NodeMCU

const int RED = D1;
const int GREEN = D2;
const int BLUE = D3;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
   
    if (type == WStype_DISCONNECTED) {
        USE_SERIAL.printf("[WSc] Disconnected!\n");
        isConnected = false;
        canState = serverdisconnected;
        stateSwitch = true;
    } else if (type == WStype_CONNECTED) {
        USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
        isConnected = true;

	    // send message to server when Connected
        // socket.io upgrade confirmation message (required)
		webSocket.sendTXT("5");
    } else if (type == WStype_TEXT) {
        // USE_SERIAL.printf("[WSc] get text: %s\n", payload);
        USE_SERIAL.printf("[WSc] get text: %s\n", payload);
        USE_SERIAL.printf("length: %d\n", length);
        
        if (length > 2) {
            StaticJsonBuffer<200> jsonBuffer;
            JsonArray& parsed2 = jsonBuffer.parseArray(payload+2); //Parse message
            const char* typeofmessage = parsed2[0];

            USE_SERIAL.printf("typeofmessage %s\n", typeofmessage);

            if (strcmp(typeofmessage,"messagelength")==0) {
                messageslength = parsed2[1];
                
                USE_SERIAL.printf("Message type: %s\n", typeofmessage);
                USE_SERIAL.printf("parsed message length = %d\n", messageslength);
            
                if (messageslength > 0) {
                    USE_SERIAL.printf("Messages greater than zero\n");
                    canState = messageavailable;
                    stateSwitch = true;
                } else {
                    USE_SERIAL.printf("Messages zero\n");
                    //we'll want to do a lookup to see if partner has read our messages
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
bool sent = false;
bool sentInitialConnection = false;
int counter = 0;
bool listening = false;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
uint64_t audioMessageId = 0;

void setup() {
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);

    // WiFi.begin("Recurse Center", "nevergraduate!");
    inputString.reserve(200);
    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(false);

    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);

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
    canState = serverdisconnected; 
    stateSwitch = true;

    webSocket.beginSocketIO("10.0.20.109", 3000);
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    webSocket.loop();
    counter++;
    //todo check for persistent wifi connection and update state if disconnected
    if (stateSwitch) {
        switch (canState) {
            case wifidisconnected:
                analogWrite(RED, 50);
                analogWrite(GREEN, 50);
                analogWrite(BLUE, 200);
            break;
            case serverdisconnected:
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
        }
        USE_SERIAL.printf("CanState %d\n",canState);
        stateSwitch = false;
    }

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
            listening = !listening;
            audioMessageId = millis();
        } else if (inputString.equals("b\n")) {
            switch (canState) {
                case messageavailable:
                    canState = messageplaying;
                    stateSwitch = true;
                break;
                case recording:
                    canState = sendingunavailable;
                    stateSwitch = true;
                break;
                case sendingavailable:
                    canState = recording;
                    stateSwitch = true;
                break;
            }
            canState = static_cast<CanState>((canState+1)%7);
            USE_SERIAL.printf("%d\n", canState);
        }
        inputString = "";
        stringComplete = false;
    }

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
    
        if (listening) {
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
            String t = String((unsigned int)audioMessageId, DEC);
            String d = "42[\"audioMessage\",{\"id\":"+t+",\"sample\":\""+c+"\"}]";
            webSocket.sendTXT(d);
            // webSocket.sendBIN(&peakToPeak, sizeof(unsigned int));
            // Serial.println(peakToPeak);
        }

        // if (counter%100000 == 0) {
        //     String messageText = "42[\"incomingMessage\", \"mewhats up?????\"]";
        //     webSocket.sendTXT(messageText);
        //     USE_SERIAL.printf("sent whats up message\n");
        // }

    } else {
        canState = serverdisconnected;
        stateSwitch = true;
    }

    if (messageslength > 0) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    
}

void serialEvent() {
  USE_SERIAL.printf("Serial onEvent");  
  while (Serial.available()) {
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
}