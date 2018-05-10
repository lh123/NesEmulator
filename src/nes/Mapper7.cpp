#include "nes/Mapper7.h"
#include "nes/Memory.h"
#include <cstdio>

Mapper7::Mapper7(Cartridge *cartridge) : mCartridge(cartridge), mPrgBank(0) {}

Mapper7::~Mapper7() {}

uint8_t Mapper7::read(uint16_t address) {
    if (address < 0x2000) {
        return mCartridge->chr[address];
    } else if (address >= 0x8000) {
        uint16_t index = mPrgBank * 0x8000 + address - 0x8000;
        return mCartridge->prg[index];
    } else if (address >= 0x6000) {
        uint16_t index = address - 0x6000;
        return mCartridge->sram[index];
    } else {
        std::printf("error: mapper7 read at address: 0x%04X\n", address);
        return 0;
    }
}

void Mapper7::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        mCartridge->chr[address] = value;
    } else if (address >= 0x8000) {
        mPrgBank = value & 0x7;
        if ((value & 0x10) == 0x00) {
            mCartridge->mirror = (uint8_t)Mirror::Single0;
        } else if ((value & 0x10) == 0x10) {
            mCartridge->mirror = (uint8_t)Mirror::Single1;
        }
    } else if (address >= 0x6000) {
        uint16_t index = address - 0x6000;
        mCartridge->sram[index] = value;
    } else {
        std::printf("error: mapper7 write at address: 0x%04X\n", address);
    }
}

void Mapper7::step() {}