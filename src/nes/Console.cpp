#include "nes/Console.h"

Console::Console(Cartridge *cartridge) : cartridge(cartridge), ram{0} {
    cpu = new CPU(this);
    cpu->reset();
    ppu = new PPU(this);
    ppu->reset();
}

Console::~Console() {
    delete cpu;
    delete ppu;
}

uint32_t Console::step() {
    uint32_t cpuCycles = cpu->step();
    for (int i = 0; i < cpuCycles * 3; i++) {
        ppu->step();
    }
    return cpuCycles;
}

uint32_t Console::stepFrame() {
    uint64_t frame = ppu->frame;
    while (frame == ppu->frame) {
        step();
    }
}