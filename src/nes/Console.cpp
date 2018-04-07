#include "nes/Console.h"

Console::Console(Cartridge *cartridge) : cartridge(cartridge), ram{0} {
    cpu = new CPU(this);
    cpu->reset();
}

Console::~Console() {
    delete cpu;
    cpu = nullptr;
}

uint32_t Console::step() {
    uint32_t cpuCycles = cpu->step();
    return cpuCycles;
}

uint32_t Console::stepFrame() {

}