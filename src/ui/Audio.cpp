#include "ui/Audio.h"
#include <cstdio>
#include <cstring>

Audio::Audio(Console *console) : console(console) {}

Audio::~Audio() {}

void fillAudio(void *udata, uint8_t *stream, int len) {
    std::memset(stream, 0, len);
    Audio *audio = reinterpret_cast<Audio *>(udata);
    float *pStram = reinterpret_cast<float *>(stream);
    AudioBuffer *buffer = audio->console->getAudioBuffer();
    len = len / sizeof(float);
    for (int i = 0; i < len; i++) {
        pStram[i] = buffer->pop();
    }
}

bool Audio::init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return false;
    }
    return true;
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