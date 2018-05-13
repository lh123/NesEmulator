#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <cstdint>
#include "nes/Frame.h"
#include "nes/AudioBuffer.h"

class CPU;
class PPU;
class APU;
class Cartridge;
class Controller;
enum class Button;
class Mapper;
class Palette;
class Filter;
class Serialize;

class Console {
public:
    static constexpr uint32_t RAM_SIZE = 2048;
    Console();
    ~Console();

    bool loadRom(std::string rom);
    void reset();
    uint32_t step();
    uint32_t stepFrame();

    void stepSeconds(double seconds);
    void setPressed(int controller, Button button, bool pressed);

    Frame *buffer() const;
    Frame::RGBA backgroundColor() const;

    AudioBuffer *getAudioBuffer();

    bool isOpenRom() const;

    void setOpenAudio(bool open);

    void save(Serialize &serialize);
    void load(Serialize &serialize);
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
    bool mIsOpenRom;
    bool mOpenAudio;
};
#endif