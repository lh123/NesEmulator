#include "nes/Mapper2.h"
#include "nes/Serialize.hpp"
#include <cstdio>

Mapper2::Mapper2(Cartridge *cartridge) : Mapper(), cartridge(cartridge) {
    prgBanks = cartridge->prgLength() / 0x4000;
    prgBank1 = 0;
    prgBank2 = prgBanks - 1;
}

Mapper2::~Mapper2() {}

uint8_t Mapper2::read(uint16_t address) {
    if (address < 0x2000) {
        return cartridge->readCHR(address);
    } else if (address >= 0xC000) {
        int index = prgBank2 * 0x4000 + int(address - 0xC000);
        return cartridge->readPRG(index);
    } else if (address >= 0x8000) {
        int index = prgBank1 * 0x4000 + int(address - 0x8000);
        return cartridge->readPRG(index);
    } else if (address >= 0x6000) {
        int index = address - 0x6000;
        return cartridge->readSRAM(index);
    } else {
        std::printf("error: mapper1 read at address: 0x%04X\n", address);
        return 0;
    }
}

void Mapper2::step() {}

void Mapper2::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        cartridge->writeCHR(address, value);
    } else if (address >= 0x8000) {
        prgBank1 = value % prgBanks;
    } else if (address >= 0x6000) {
        int index = address - 0x6000;
        cartridge->writeSRAM(index, value);
    } else {
        std::printf("error: mapper1 write at address: 0x%04X\n", address);
    }
}

void Mapper2::save(Serialize &serialize) {
    serialize << prgBanks;
    serialize << prgBank1;
    serialize << prgBank2;
}

void Mapper2::load(Serialize &serialize) {
    serialize >> prgBanks;
    serialize >> prgBank1;
    serialize >> prgBank2;
}