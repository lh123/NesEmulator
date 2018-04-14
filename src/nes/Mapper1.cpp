#include "nes/Mapper1.h"
#include "nes/Memory.h"
#include <cstdio>

Mapper1::Mapper1(Cartridge *cartridge)
    : Mapper(), cartridge(cartridge), shiftRegister(0x10), control(0), prgMode(0), chrMode(0),
      prgBank(0), chrBank0(0), chrBank1(0), prgOffsets{0}, chrOffsets{0} {
    prgOffsets[1] = prgBankOffset(-1);
}

Mapper1::~Mapper1() {}

uint8_t Mapper1::read(uint16_t address) {
    if (address < 0x2000) {
        uint16_t bank = address / 0x1000;
        uint16_t offset = address % 0x1000;
        return cartridge->chr[chrOffsets[bank] + offset];
    } else if (address >= 0x8000) {
        address = address - 0x8000;
        uint16_t bank = address / 0x4000;
        uint16_t offset = address % 0x4000;
        return cartridge->prg[prgOffsets[bank] + offset];
    } else if (address >= 0x6000) {
        return cartridge->sram[address - 0x6000];
    } else {
        std::printf("error: mapper1 read at address: 0x%04X\n", address);
        return 0;
    }
}

void Mapper1::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        uint16_t bank = address / 0x1000;
        uint16_t offset = address % 0x1000;
        cartridge->chr[chrOffsets[bank] + offset] = value;
    } else if (address >= 0x8000) {
        loadRegister(address, value);
    } else if (address >= 0x6000) {
        cartridge->sram[address - 0x6000] = value;
    } else {
        std::printf("error: mapper1 write at address: 0x%04X\n", address);
    }
}

void Mapper1::step() {}

void Mapper1::loadRegister(uint16_t address, uint8_t value) {
    if ((value & 0x80) == 0x80) {
        shiftRegister = 0x10;
        writeControl(control | 0x0C);
    } else {
        bool complete = (shiftRegister & 0x1) == 0x1;
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
    } else {
        writePRGBank(value);
    }
}

void Mapper1::writeControl(uint8_t value) {
    control = value;
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
    prgBank = value & 0x0F;
    updateOffset();
}

int Mapper1::prgBankOffset(int index) {
    if (index >= 0x80) {
        index -= 0x100;
    }
    index %= cartridge->prgLength() / 0x4000;
    int offset = index * 0x4000;
    if (offset < 0) {
        offset += cartridge->prgLength();
    }
    return offset;
}

int Mapper1::chrBankOffset(int index) {
    if (index >= 0x80) {
        index -= 0x100;
    }
    index %= cartridge->chrLength() / 0x1000;
    int offset = index * 0x1000;
    if (offset < 0) {
        offset += cartridge->chrLength();
    }
    return offset;
}

void Mapper1::updateOffset() {
    switch (prgMode) {
    case 0:
    case 1:
        prgOffsets[0] = prgBankOffset(prgBank & 0xFE);
        prgOffsets[1] = prgBankOffset(prgBank | 0x01);
        break;
    case 2:
        prgOffsets[0] = 0;
        prgOffsets[1] = prgBankOffset(prgBank);
        break;
    case 3:
        prgOffsets[0] = prgBankOffset(prgBank);
        prgOffsets[1] = prgBankOffset(-1);
        break;
    }
    switch (chrMode) {
    case 0:
        chrOffsets[0] = chrBankOffset(chrBank0 & 0xFE);
        chrOffsets[1] = chrBankOffset(chrBank0 | 0x01);
        break;
    case 1:
        chrOffsets[0] = chrBankOffset(chrBank0);
        chrOffsets[1] = chrBankOffset(chrBank1);
        break;
    }
}
