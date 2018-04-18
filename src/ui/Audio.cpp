#include "ui/Audio.h"
#include <cstdio>
#include <cstring>

Audio::Audio(Console *console) : console(console) {}

Audio::~Audio() {}

void fillAudio(void *udata, uint8_t *stream, int len) {
    std::memset(stream, 0, len);
    Audio *audio = reinterpret_cast<Audio *>(udata);
    uint16_t *pFStream = reinterpret_cast<uint16_t *>(stream);
    AudioBuffer *buffer = audio->console->getAudioBuffer();
    int index = 0;
    while (!buffer->isEmpty() && index < len / 2) {
        uint16_t o = (buffer->pop()) * 0xFFFF;
        *(pFStream + index) = o;
        index++;
    }
}

bool Audio::init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return false;
    }
    return true;
}

bool Audio::openAudioDevice() {
    SDL_AudioSpec spec;
    spec.freq = 44100;
    spec.format = AUDIO_U16SYS;
    spec.channels = 1;
    spec.samples = 44100;
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