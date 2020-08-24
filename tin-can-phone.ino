#include <Arduino.h>
#include <i2s.h>
#include "Config.h"
#include "Button.h"
#include "Can.h"
#include "Recorder.h"
#include "RecorderOutputFile.h"
#include "RecorderOutputBuffer.h"
#include "Player.h"
#include "PlayerInputFile.h"
#include "MAX4466.h"
#include "MAX98357A.h"
#include "ExponentialMovingAverage.h"

#define FILENAME "audio.pcm"
#define SAMPLE_RATE 8000
#define ALPHA 0.6

extern "C" {
    #include <user_interface.h>
}

Button button(D0);

// recorder
MAX4466 microphone(A0);
ExponentialMovingAverage ema(microphone.getSample(), ALPHA);
RecorderOutputFile outputFile(FILENAME);
RecorderOutputBuffer outputBuffer(&outputFile, SAMPLE_RATE / 2);
Recorder recorder(&microphone, SAMPLE_RATE, &ema, &outputBuffer);

// player
PlayerInputFile inputFile(FILENAME);
MAX98357A dac;
Player player(&inputFile, &dac);

// can
Can can(recorder, player);

void setup() {
    system_update_cpu_freq(160);
    Serial.begin(115200);
    pinMode(D0, INPUT);
    pinMode(A0, INPUT);
    i2s_begin();
    i2s_set_rate(SAMPLE_RATE);
}

void loop() {
    button.loop();
    if (button.wasClicked()) {
        can.updateState();
    }
    can.loop();
}
