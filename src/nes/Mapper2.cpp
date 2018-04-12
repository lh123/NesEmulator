#include "nes/Mapper2.h"
#include <cstdio>

Mapper2::Mapper2(Cartridge *cartridge)
    : Mapper(), cartridge(cartridge) {
    int prgBanks = cartridge->prgLength() / 0x4000;
    prgBank1 = 0;
    prgBank2 = prgBanks - 1;
}

uint8_t Mapper2::read(uint16_t address) {
    if (address < 0x2000) {
        return cartridge->chr[address];
    } else if (address >= 0xC000) {
        int index = prgBank2 * 0x4000 + (address - 0xC000);
        return cartridge->prg[index];
    } else if (address >= 0x8000) {
        int index = prgBank1 * 0x4000 + (address - 0x8000);
        return cartridge->prg[index];
    } else if (address >= 0x6000) {
        int index = address - 0x6000;
        return cartridge->sram[index];
    } else {
        std::printf("error: mapper1 read at address: 0x%04X\n", address);
        return 0;
    }
}

void Mapper2::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        cartridge->chr[address] = value;
    } else if (address >= 0x8000) {
        prgBank1 = value;
    } else if (address >= 0x6000) {
        int index = address - 0x6000;
        cartridge->sram[index] = value;
    } else {
        std::printf("error: mapper1 write at address: 0x%04X\n", address);
    }
}