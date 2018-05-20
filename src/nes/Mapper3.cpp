#include "nes/Mapper3.h"
#include "nes/Serialize.hpp"
#include <cstdio>

Mapper3::Mapper3(Cartridge *cartridge) : mCartridge(cartridge) {
    mChrBank = 0;
    mPrgBank1 = 0;
    mPrgBank2 = cartridge->prgLength() / 0x4000;
}

Mapper3::~Mapper3() {}

uint8_t Mapper3::read(uint16_t address) {
    if (address < 0x2000) {
        uint16_t index = mChrBank * 0x2000 + address;
        return mCartridge->readCHR(index);
    } else if (address >= 0xC000) {
        uint16_t index = mPrgBank2 * 0x4000 + address - 0xC000;
        return mCartridge->readPRG(index);
    } else if (address >= 0x8000) {
        uint16_t index = mPrgBank1 * 0x4000 + address - 0x8000;
        return mCartridge->readPRG(index);
    } else if (address >= 0x6000) {
        uint16_t index = address - 0x6000;
        return mCartridge->readSRAM(index);
    } else {
        std::printf("error: mapper3 read at address: 0x%04X\n", address);
        return 0;
    }
}

void Mapper3::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        uint16_t index = mChrBank * 0x2000 + address;
        mCartridge->writeCHR(index, value);
    } else if (address >= 0x8000) {
        mChrBank = value & 0x3;
    } else if (address >= 0x6000) {
        uint16_t index = address - 0x6000;
        mCartridge->writeSRAM(index, value);
    } else {
        std::printf("error: mapper3 write at address: 0x%04X\n", address);
    }
}

void Mapper3::step() {}

void Mapper3::save(Serialize &serialize) {
    serialize << mChrBank;
    serialize << mPrgBank1;
    serialize << mPrgBank2;
}

void Mapper3::load(Serialize &serialize) {
    serialize >> mChrBank;
    serialize >> mPrgBank1;
    serialize >> mPrgBank2;
}