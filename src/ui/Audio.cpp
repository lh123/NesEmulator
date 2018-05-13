#include "ui/Audio.h"
#include <iostream>
#include <cstring>

Audio::Audio() : mHaveInit(false), mAudioBuffer(nullptr) {}

Audio::~Audio() { release(); }

void fillAudio(void *udata, uint8_t *stream, int len) {
    std::memset(stream, 0, len);
    Audio *audio = reinterpret_cast<Audio *>(udata);
    float *pStram = reinterpret_cast<float *>(stream);
    if (audio->mAudioBuffer != nullptr) {
        AudioBuffer *buffer = audio->mAudioBuffer;
        int maxlen = len / sizeof(float);
        int fillLength = buffer->pop(pStram, maxlen);
        float fillValue = fillLength > 0 ? pStram[fillLength - 1] : 0;
        for (int i = fillLength; i < maxlen; i++) {
            pStram[i] = fillValue;
        }
    }
}

bool Audio::init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return false;
    }
    mHaveInit = true;
    return true;
}

void Audio::release() {
    if (mHaveInit) {
        mHaveInit = false;
        SDL_Quit();
    }
}

bool Audio::openAudioDevice(uint16_t sampleRate) {
    SDL_AudioSpec spec;
    spec.freq = sampleRate;
    spec.format = AUDIO_F32SYS;
    spec.channels = 1;
    spec.samples = 4096;
    spec.callback = fillAudio;
    spec.userdata = this;

    if (SDL_OpenAudio(&spec, nullptr) < 0) {
        std::printf("sdl error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void Audio::play() { SDL_PauseAudio(0); }

void Audio::pause() { SDL_PauseAudio(1); }

void Audio::close() { SDL_Quit(); }

void Audio::setAudioBuffer(AudioBuffer *buffer) { mAudioBuffer = buffer; }