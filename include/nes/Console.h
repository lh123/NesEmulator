#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdint>
#include "nes/Image.h"
#include "nes/AudioBuffer.h"

class CPU;
class PPU;
class APU;
class Cartridge;
class Controller;
enum class Button;
class Mapper;
class Palette;

class Console {
public:
    static constexpr uint32_t RAM_SIZE = 2048;
    Console(const char *path);
    ~Console();

    void reset();
    uint32_t step();
    uint32_t stepFrame();

    void stepSeconds(double seconds);
    void setPressed(int controller, Button button, bool pressed);

    Image *buffer() const;
    Image::RGBA backgroundColor() const;

    void setAudioSampleRate(uint32_t value);
    AudioBuffer *getAudioBuffer();
public:
    CPU *cpu;
    PPU *ppu;
    APU *apu;
    Cartridge *cartridge;
    Mapper *mapper;
    Controller *controller1;
    Controller *controller2;

    uint8_t ram[RAM_SIZE];

private:
    bool isSuccess;
};
#endif