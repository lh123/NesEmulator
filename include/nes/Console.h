#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdint>

#include "nes/CPU.h"
#include "nes/PPU.h"
#include "nes/Cartridge.h"
#include "nes/Mapper.h"
#include "nes/Controller.h"

class Console {
public:
    static constexpr uint32_t RAM_SIZE = 2048;
    Console(Cartridge *cartridge);
    ~Console();

    void reset();
    uint32_t step();
    uint32_t stepFrame();
    void setPressed(int controller, Button button, bool pressed);
    Image* buffer();

public:
    CPU *cpu;
    PPU *ppu;
    Cartridge *cartridge;
    Mapper *mapper;
    Controller *controller1;
    Controller *controller2;
    
    uint8_t ram[RAM_SIZE];
};
#endif