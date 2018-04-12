#include "nes/Mapper.h"
#include "nes/Console.h"
#include "nes/Mapper1.h"
#include "nes/Mapper2.h"
#include <cstdio>

Mapper::~Mapper() {}

Mapper *Mapper::create(Console *console) {
    Cartridge *cartridge = console->cartridge;
    switch (cartridge->mapper) {
    case 0:
        return new Mapper2(cartridge);
    case 1:
        return new Mapper1(cartridge);
    case 2:
        return new Mapper2(cartridge);
    default:
        std::printf("error: unsupported mapper %d\n", cartridge->mapper);
        return nullptr;
    }
}

void Mapper::free(Mapper *mapper) {
    delete mapper;
}