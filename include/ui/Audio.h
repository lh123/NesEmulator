#ifndef AUDIO_H
#define AUDIO_H
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <functional>
#include "nes/AudioBuffer.h"

class Audio {
public:
    using FillAudioListener = std::function<void(const float*, int)>;

    Audio();
    ~Audio();

    bool init();
    void release();
    bool openAudioDevice(uint16_t sampleRate);
    void play();
    void pause();
    void close();

    void setAudioBuffer(AudioBuffer *buffer);
    void setOnFillAudioListener(FillAudioListener listener);

    friend void fillAudio(void *udata, uint8_t *buffer, int len);

private:
    bool mHaveInit;
    AudioBuffer *mAudioBuffer;
    FillAudioListener mListener;
};

#endif