#ifndef RECORDER_H
#define RECORDER_H

#include "Microphone.h"
#include "Filter.h"
#include "RecorderOutput.h"

class Recorder {
    private:
        bool recording;
        int sampleRate;
        unsigned long lastSampleTime;
 
    protected:
        Microphone *microphone;
        Filter *filter;
        RecorderOutput *output;

    public:
        Recorder(Microphone *microphone, int sampleRate, Filter *filter, RecorderOutput *output) :
            microphone(microphone), 
            sampleRate(sampleRate), 
            filter(filter), 
            output(output) 
        { }

        bool isRecording();

        void begin();
        void loop();
        void stop();

        void setSampleRate(int sampleRate) {
            this->sampleRate = sampleRate;
        }

        void setMicrophone(Microphone *microphone) {
            this->microphone = microphone;
        }

        void setFilter(Filter *filter) {
            this->filter = filter;
        }

        void setRecorderOutput(RecorderOutput *output) {
            this->output = output;
        }
};

#endif
