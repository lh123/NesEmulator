#ifndef AUDIO_H
#define AUDIO_H
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include "nes/AudioBuffer.h"

class Audio {
public:
    Audio();
    ~Audio();

    bool init();
    void release();
    bool openAudioDevice(uint16_t sampleRate);
    void play();
    void pause();
    void close();

    void setAudioBuffer(AudioBuffer *buffer);

    friend void fillAudio(void *udata, uint8_t *buffer, int len);

private:
    bool mHaveInit;
    AudioBuffer *mAudioBuffer;
};

#endif