#include "nes/Console.h"
#include <cstdio>

#include "nes/CPU.h"
#include "nes/APU.h"
#include "nes/PPU.h"
#include "nes/Cartridge.h"
#include "nes/Controller.h"
#include "nes/Mapper.h"
#include "nes/Palette.h"
#include "nes/Filter.h"
#include "nes/Serialize.hpp"

Console::Console() : cartridge(nullptr), mapper(nullptr), ram{0}, mIsOpenRom(false), mOpenAudio(false) {
    cpu = new CPU(this);
    ppu = new PPU(this);
    apu = new APU(this);
    controller1 = new Controller();
    controller2 = new Controller();
}

Console::~Console() {
    delete controller2;
    delete controller1;
    delete apu;
    delete ppu;
    delete cpu;
    if (cartridge != nullptr) {
        delete[] cartridge;
    }
    if (mapper != nullptr) {
        Mapper::free(mapper);
    }
}

bool Console::loadRom(std::string rom) {
    if (cartridge != nullptr) {
        delete cartridge;
        cartridge = nullptr;
    }
    if (mapper != nullptr) {
        Mapper::free(mapper);
        mapper = nullptr;
    }
    cartridge = new Cartridge();
    mIsOpenRom = false;
    if (cartridge->loadNesFile(rom.c_str())) {
        mapper = Mapper::create(this);
        if (mapper != nullptr) {
            reset();
            mIsOpenRom = true;
        }
    } else {
        delete cartridge;
        cartridge = nullptr;
    }
    return mIsOpenRom;
}

void Console::reset() {
    cpu->reset();
    ppu->reset();
    apu->reset();
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
    uint64_t frame = ppu->currentFrame();
    while (frame == ppu->currentFrame()) {
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

Frame *Console::buffer() const { return ppu->front; }

Frame::RGBA Console::backgroundColor() const { return palette[ppu->readPalette(0) % 64]; }

AudioBuffer *Console::getAudioBuffer() { return apu->getAudioBuffer(); }

bool Console::isOpenRom() const { return mIsOpenRom; }

void Console::save(Serialize &serialize) {
    serialize.writeArray(ram, RAM_SIZE);
    cpu->save(serialize);
    ppu->save(serialize);
    apu->save(serialize);
    cartridge->save(serialize);
    mapper->save(serialize);
}

void Console::load(Serialize &serialize) {
    serialize.readArray(ram, RAM_SIZE);
    cpu->load(serialize);
    ppu->load(serialize);
    apu->load(serialize);
    cartridge->load(serialize);
    mapper->load(serialize);
}