#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdint>

#include "nes/CPU.h"
#include "nes/PPU.h"
#include "nes/APU.h"
#include "nes/Cartridge.h"
#include "nes/Controller.h"
#include "nes/Mapper.h"
#include "nes/Palette.h"

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