#include "nes/Mapper1.h"
#include "nes/Memory.h"
#include <cstdio>

Mapper1::Mapper1(Cartridge *cartridge)
    : Mapper(), cartridge(cartridge), shiftRegister(0x10), prgMode(0),
      chrMode(0), prgBank(0), chrBank0(0), chrBank1(0), prgOffset0(0),
      prgOffset1(0), chrOffset0(0), chrOffset1(0) {
    prgOffset1 = cartridge->prgLength() - 0x4000;
}

uint8_t Mapper1::read(uint16_t address) {
    if (address < 0x1000) {
        return cartridge->chr[chrOffset0 + address - 0x0000];
    } else if (address < 0x2000) {
        return cartridge->chr[chrOffset1 + address - 0x1000];
    } else if (address >= 0xC000) {
        return cartridge->prg[prgOffset1 + address - 0xC000];
    } else if (address >= 0x8000) {
        return cartridge->prg[prgOffset0 + address - 0x8000];
    } else if (address >= 0x6000) {
        return cartridge->sram[address - 0x6000];
    } else {
        std::printf("error: mapper1 read at address: 0x%04X\n", address);
        return 0;
    }
}

void Mapper1::write(uint16_t address, uint8_t value) {
    if (address < 0x1000) {
        cartridge->chr[chrOffset0 + address - 0x0000] = value;
    } else if (address < 0x2000) {
        cartridge->chr[chrOffset1 + address - 0x1000] = value;
    } else if (address >= 0x8000) {
        loadRegister(address, value);
    } else if (address >= 0x6000) {
        cartridge->sram[address - 0x6000] = value;
    } else {
        std::printf("error: mapper1 write at address: 0x%04X\n", address);
    }
}

void Mapper1::loadRegister(uint16_t address, uint8_t value) {
    if (value & 0x80 == 0x80) {
        shiftRegister = 0x10;
    } else {
        bool complete = shiftRegister & 0x1 == 0x1;
        shiftRegister >>= 1;
        shiftRegister |= (value & 0x1) << 4;
        if (complete) {
            writeRegister(address, shiftRegister);
            shiftRegister = 0x10;
        }
    }
}

void Mapper1::writeRegister(uint16_t address, uint8_t value) {
    if (address <= 0x9FFF) {
        writeControl(value);
    } else if (address <= 0xBFFF) {
        writeCHRBank0(value);
    } else if (address <= 0xDFFF) {
        writeCHRBank1(value);
    } else if (address <= 0xFFFF) {
        writePRGBank(value);
    } else {
        std::printf("error: mapper write register at address:0x%04X\n",
                    address);
    }
}

void Mapper1::writeControl(uint8_t value) {
    chrMode = (value >> 4) & 0x1;
    prgMode = (value >> 2) & 0x3;

    uint8_t mirror = value & 0x3;
    switch (mirror) {
    case 0:
        cartridge->mirror = uint8_t(Mirror::Single0);
        break;
    case 1:
        cartridge->mirror = uint8_t(Mirror::Single1);
        break;
    case 2:
        cartridge->mirror = uint8_t(Mirror::Vertical);
        break;
    case 3:
        cartridge->mirror = uint8_t(Mirror::Horizontal);
        break;
    }
    updateOffset();
}

void Mapper1::writeCHRBank0(uint8_t value) {
    chrBank0 = value;
    updateOffset();
}

void Mapper1::writeCHRBank1(uint8_t value) {
    chrBank1 = value;
    updateOffset();
}

void Mapper1::writePRGBank(uint8_t value) {
    prgBank = value;
    updateOffset();
}

void Mapper1::updateOffset() {
    switch (prgMode) {
    case 0:
    case 1:
        prgOffset0 = int(prgBank & 0xFE) * 0x8000;
        prgOffset1 = prgOffset0 + 0x4000;
        break;
    case 2:
        prgOffset0 = 0;
        prgOffset1 = int(prgBank) * 0x4000;
        break;
    case 3:
        prgOffset0 = int(prgBank) * 0x4000;
        prgOffset1 = cartridge->prgLength() - 0x4000;
        break;
    }
    switch (chrMode) {
    case 0:
        chrOffset0 = int(chrBank0 & 0xFE) * 0x2000;
        chrOffset1 = chrOffset0 + 0x1000;
        break;
    case 1:
        chrOffset0 = int(chrBank0) * 0x1000;
        chrOffset1 = int(chrBank1) * 0x1000;
        break;
    }
}
