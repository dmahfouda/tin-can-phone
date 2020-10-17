#include <Arduino.h>
#include <i2s.h>
#include "Config.h"
#include "src/Button.h"
#include "src/TinCanPhone.h"
#include "src/Recorder.h"
#include "src/RecorderOutputFile.h"
#include "src/RecorderOutputBuffer.h"
#include "src/Player.h"
#include "src/PlayerInputFile.h"
#include "src/MAX4466.h"
#include "src/MAX98357A.h"
#include "src/ExponentialMovingAverage.h"

extern "C" {
    #include <user_interface.h>
}

/* button */
Button                   button(D0);
/* sender */
MessageSender            messageSender(TIN_CAN_PHONE_AUDIO_FILENAME, TIN_CAN_SWITCH_PROTOCOL "://" TIN_CAN_SWITCH_HOST ":" TIN_CAN_SWITCH_PORT "/box/" TIN_CAN_SWITCH_BOX "/message");
/* receiver */
MessageReceiver          messageReceiver(TIN_CAN_PHONE_AUDIO_FILENAME, TIN_CAN_SWITCH_PROTOCOL "://" TIN_CAN_SWITCH_HOST ":" TIN_CAN_SWITCH_PORT "/box/" TIN_CAN_SWITCH_BOX "/message");
/* recorder */
MAX4466                  microphone(A0);
ExponentialMovingAverage ema(microphone.getSample(), TIN_CAN_PHONE_EMA_ALPHA);
RecorderOutputFile       outputFile(TIN_CAN_PHONE_AUDIO_FILENAME);
RecorderOutputBuffer     outputBuffer(&outputFile, TIN_CAN_PHONE_SAMPLE_RATE / 2);
Recorder                 recorder(&microphone, TIN_CAN_PHONE_SAMPLE_RATE, &ema, &outputBuffer);
/* player */
PlayerInputFile          inputFile(TIN_CAN_PHONE_AUDIO_FILENAME);
MAX98357A                dac;
Player                   player(&inputFile, &dac);
/* box */
Box                      box(TIN_CAN_SWITCH_BOX);
/* can */
TinCanPhone              tinCanPhone(messageSender, messageReceiver, recorder, player, box);

void setup() {
    system_update_cpu_freq(160);
    Serial.begin(115200);
    pinMode(D0, INPUT);
    pinMode(A0, INPUT);
    i2s_begin();
    i2s_set_rate(TIN_CAN_PHONE_SAMPLE_RATE);
}

void loop() {
    button.loop();
    if (button.wasClicked()) {
        tinCanPhone.updateState();
    }
    tinCanPhone.loop();
}
