#include "nes/Mapper7.h"
#include "nes/Memory.h"
#include "nes/Serialize.hpp"
#include <cstdio>

Mapper7::Mapper7(Cartridge *cartridge) : mCartridge(cartridge), mPrgBank(0) {}

Mapper7::~Mapper7() {}

uint8_t Mapper7::read(uint16_t address) {
    if (address < 0x2000) {
        return mCartridge->readCHR(address);
    } else if (address >= 0x8000) {
        uint16_t index = mPrgBank * 0x8000 + address - 0x8000;
        return mCartridge->readPRG(index);
    } else if (address >= 0x6000) {
        uint16_t index = address - 0x6000;
        return mCartridge->readSRAM(index);
    } else {
        std::printf("error: mapper7 read at address: 0x%04X\n", address);
        return 0;
    }
}

void Mapper7::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        mCartridge->writeCHR(address, value);
    } else if (address >= 0x8000) {
        mPrgBank = value & 0x7;
        if ((value & 0x10) == 0x00) {
            mCartridge->setMirror(Mirror::Single0);
        } else if ((value & 0x10) == 0x10) {
            mCartridge->setMirror(Mirror::Single1);
        }
    } else if (address >= 0x6000) {
        uint16_t index = address - 0x6000;
        mCartridge->writeSRAM(index, value);
    } else {
        std::printf("error: mapper7 write at address: 0x%04X\n", address);
    }
}

void Mapper7::step() {}

void Mapper7::save(Serialize &serialize) { serialize << mPrgBank; }

void Mapper7::load(Serialize &serialize) { serialize >> mPrgBank; }