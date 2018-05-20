#include "nes/Mapper225.h"
#include "nes/Memory.h"
#include "nes/Serialize.hpp"
#include <cstdio>

Mapper225::Mapper225(Cartridge *cartridge) : mCartridge(cartridge) {
    mChrBank = 0;
    mPrgBank1 = 0;
    mPrgBank2 = cartridge->prgLength() / 0x4000 - 1;
}

Mapper225::~Mapper225() {}

uint8_t Mapper225::read(uint16_t address) {
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
        std::printf("error: mapper225 read at address: 0x%04X\n", address);
        return 0;
    }
}

void Mapper225::write(uint16_t address, uint8_t value) {
    if (address < 0x8000) {
        return;
    }

    int A = address;
    int bank = (A >> 14) & 0x1;
    mChrBank = (A & 0x3F) | (bank << 6);
    int prg = ((A >> 6) & 0x3F) | (bank << 6);

    int mode = (A >> 12) & 0x1;
    if (mode == 1) {
        mPrgBank1 = prg;
        mPrgBank2 = prg;
    } else {
        mPrgBank1 = prg;
        mPrgBank2 = prg + 1;
    }

    int mirr = (A >> 13) & 0x1;
    if (mirr == 1) {
        mCartridge->setMirror(Mirror::Horizontal);
    } else {
        mCartridge->setMirror(Mirror::Vertical);
    }
}

void Mapper225::step() {}

void Mapper225::save(Serialize &serialize) {
    serialize << mChrBank;
    serialize << mPrgBank1;
    serialize << mPrgBank2;
}

void Mapper225::load(Serialize &serialize) {
    serialize >> mChrBank;
    serialize >> mPrgBank1;
    serialize >> mPrgBank2;
}