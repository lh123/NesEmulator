#include "nes/Console.h"
#include <cstdio>

Console::Console(const char *path) : ram{0} {
    cartridge = new Cartridge();
    isSuccess = cartridge->loadNesFile(path);
    if (isSuccess) {
        mapper = Mapper::create(this);        
        cpu = new CPU(this);
        cpu->reset();
        ppu = new PPU(this);
        ppu->reset();
        controller1 = new Controller();
        controller2 = new Controller();
    }
}

Console::~Console() {
    if (isSuccess) {
        delete cpu;
        delete ppu;
        delete controller1;
        delete controller2;
        Mapper::free(mapper);
    }
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

void Console::setPressed(int controller, Button button, bool pressed) {
    if (controller == 1) {
        controller1->setPressed(button, pressed);
    } else if (controller == 2) {
        controller2->setPressed(button, pressed);
    } else {
        std::printf("error: controller press: %d\n", controller);
    }
}

Image *Console::buffer() { return ppu->buffer; }