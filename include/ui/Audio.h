#ifndef AUDIO_H
#define AUDIO_H
#define SDL_MAIN_HANDLED

#include "nes/Console.h"
#include <SDL2/SDL.h>

class Audio {
public:
    Audio(Console *buffer);
    ~Audio();

    bool init();
    bool openAudioDevice();
    void play();
    void pause();
    void close();

    friend void fillAudio(void *udata, uint8_t *buffer, int len);

private:
    Console *console;
};

#endif