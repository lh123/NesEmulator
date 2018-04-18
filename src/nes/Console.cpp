#include "nes/Console.h"
#include <cstdio>

#include "nes/CPU.h"
#include "nes/APU.h"
#include "nes/PPU.h"
#include "nes/Cartridge.h"
#include "nes/Controller.h"
#include "nes/Mapper.h"
#include "nes/Palette.h"

Console::Console(const char *path) : ram{0} {
    cartridge = new Cartridge();
    isSuccess = cartridge->loadNesFile(path);
    if (isSuccess) {
        mapper = Mapper::create(this);
        cpu = new CPU(this);
        ppu = new PPU(this);
        apu = new APU(this);
        controller1 = new Controller();
        controller2 = new Controller();
    }
}

Console::~Console() {
    if (isSuccess) {
        Mapper::free(mapper);
        delete controller2;
        delete controller1;
        delete apu;
        delete ppu;
        delete cpu;
    }
}

uint32_t Console::step() {
    uint32_t cpuCycles = cpu->step();
    uint32_t ppuCycles = cpuCycles * 3;
    for (uint32_t i = 0; i < ppuCycles; i++) {
        ppu->step();
        mapper->step();
    }
    for (uint32_t i = 0; i < cpuCycles; i++) {
        apu->step();
    }
    return cpuCycles;
}

uint32_t Console::stepFrame() {
    uint64_t cpuCycle = 0;
    uint64_t frame = ppu->frame;
    while (frame == ppu->frame) {
        cpuCycle += step();
    }
    return cpuCycle;
}

void Console::stepSeconds(double seconds) {
    int cycles = CPU::CPU_FREQUENCY * seconds;
    while (cycles > 0) {
        cycles -= step();
    }
}

void Console::setPressed(int controller, Button button, bool pressed) {
    if (controller == 1) {
        controller1->setPressed(button, pressed);
    } else if (controller == 2) {
        controller2->setPressed(button, pressed);
    } else {
        std::printf("error: controller press: %d\n", controller);
    }
}

Image *Console::buffer() const { return ppu->front; }

Image::RGBA Console::backgroundColor() const { return palette[ppu->readPalette(0) % 64]; }

void Console::setAudioSampleRate(uint32_t value) {
    if (value != 0) {
        apu->setSampleRate(CPU::CPU_FREQUENCY / value);
    }
}

AudioBuffer *Console::getAudioBuffer() { return apu->getAudioBuffer(); }