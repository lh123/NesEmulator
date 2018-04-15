#include "nes/Mapper4.h"
#include "nes/Console.h"

Mapper4::Mapper4(Console *console, Cartridge *cartridge)
    : console(console), cartridge(cartridge), _register(0), _registers{0}, prgMode(0),
      chrMode(0), prgOffsets{0}, chrOffsets{0}, reload(0), counter(0), irqEnable(false) {
    prgOffsets[0] = prgBankOffset(0);
    prgOffsets[1] = prgBankOffset(1);
    prgOffsets[2] = prgBankOffset(-2);
    prgOffsets[3] = prgBankOffset(-1);
}

Mapper4::~Mapper4(){}

uint8_t Mapper4::read(uint16_t address) {
    if (address < 0x2000) {
        uint16_t bank = address / 0x0400;
        uint16_t offset = address % 0x0400;
        return cartridge->chr[chrOffsets[bank] + offset];
    } else if (address >= 0x8000) {
        address = address - 0x8000;
        uint16_t bank = address / 0x2000;
        uint16_t offset = address % 0x2000;
        return cartridge->prg[prgOffsets[bank] + offset];
    } else if (address >= 0x6000) {
        return cartridge->sram[address - 0x6000];
    } else {
        return 0;
    }
}

void Mapper4::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        uint16_t bank = address / 0x0400;
        uint16_t offset = address % 0x0400;
        cartridge->chr[chrOffsets[bank] + offset] = value;
    } else if (address >= 0x8000) {
        writeRegister(address, value);
    } else if (address >= 0x6000) {
        cartridge->sram[address - 0x6000] = value;
    }
}

void Mapper4::step() {
    PPU *ppu = console->ppu;
    if (ppu->cycle != 260) {
        return;
    }
    if (ppu->scanLine > 239 && ppu->scanLine < 261) {
        return;
    }
    if (ppu->flagShowBackground == 0 && ppu->flagShowSprites == 0) {
        return;
    }
    handleScanLine();
}

void Mapper4::handleScanLine() {
    if (counter == 0) {
        counter = reload;
    } else {
        counter--;
        if (counter == 0 && irqEnable) {
            console->cpu->triggerIRQ();
        }
    }
}

void Mapper4::writeRegister(uint16_t address, uint8_t value) {
    if (address <= 0x9FFF && address % 2 == 0) {
        writeBankSelect(value);
    } else if (address <= 0x9FFF && address % 2 == 1) {
        writeBankData(value);
    } else if (address <= 0xBFFF && address % 2 == 0) {
        writeMirror(value);
    } else if (address <= 0xBFFF && address % 2 == 1) {
        writeProtect(value);
    } else if (address <= 0xDFFF && address % 2 == 0) {
        writeIRQLatch(value);
    } else if (address <= 0xDFFF && address % 2 == 1) {
        writeIRQReload(value);
    } else if (address <= 0xFFFF && address % 2 == 0) {
        writeIRQDisable(value);
    } else if (address <= 0xFFFF && address % 2 == 1) {
        writeIRQEnable(value);
    }
}

void Mapper4::writeBankSelect(uint8_t value) {
    prgMode = (value >> 6) & 0x1;
    chrMode = (value >> 7) & 0x1;
    _register = value & 0x7;
    updateOffset();
}

void Mapper4::writeBankData(uint8_t value) {
    _registers[_register] = value;
    updateOffset();
}

void Mapper4::writeMirror(uint8_t value) {
    switch (value & 0x1) {
    case 0:
        cartridge->mirror = uint8_t(Mirror::Vertical);
        break;
    case 1:
        cartridge->mirror = uint8_t(Mirror::Horizontal);
        break;
    }
}

void Mapper4::writeProtect(uint8_t value) {}

void Mapper4::writeIRQLatch(uint8_t value) { reload = value; }

void Mapper4::writeIRQReload(uint8_t value) { counter = 0; }

void Mapper4::writeIRQDisable(uint8_t value) { irqEnable = false; }

void Mapper4::writeIRQEnable(uint8_t value) { irqEnable = true; }

int Mapper4::prgBankOffset(int index) {
    if (index >= 0x80) {
        index -= 0x100;
    }
    index %= cartridge->prgLength() / 0x2000;
    int offset = index * 0x2000;
    if (offset < 0) {
        offset += cartridge->prgLength();
    }
    return offset;
}

int Mapper4::chrBankOffset(int index) {
    if (index >= 0x80) {
        index -= 0x100;
    }
    index %= cartridge->chrLength() / 0x0400;
    int offset = index * 0x0400;
    if (offset < 0) {
        offset += cartridge->chrLength();
    }
    return offset;
}

void Mapper4::updateOffset() {
    switch (prgMode) {
    case 0:
        prgOffsets[0] = prgBankOffset(_registers[6]);
        prgOffsets[1] = prgBankOffset(_registers[7]);
        prgOffsets[2] = prgBankOffset(-2);
        prgOffsets[3] = prgBankOffset(-1);
        break;
    case 1:
        prgOffsets[0] = prgBankOffset(-2);
        prgOffsets[1] = prgBankOffset(_registers[7]);
        prgOffsets[2] = prgBankOffset(_registers[6]);
        prgOffsets[3] = prgBankOffset(-1);
        break;
    }
    switch (chrMode) {
    case 0:
        chrOffsets[0] = chrBankOffset(_registers[0] & 0xFE);
        chrOffsets[1] = chrBankOffset(_registers[0] | 0x01);
        chrOffsets[2] = chrBankOffset(_registers[1] & 0xFE);
        chrOffsets[3] = chrBankOffset(_registers[1] | 0x01);
        chrOffsets[4] = chrBankOffset(_registers[2]);
        chrOffsets[5] = chrBankOffset(_registers[3]);
        chrOffsets[6] = chrBankOffset(_registers[4]);
        chrOffsets[7] = chrBankOffset(_registers[5]);
        break;
    case 1:
        chrOffsets[0] = chrBankOffset(_registers[2]);
        chrOffsets[1] = chrBankOffset(_registers[3]);
        chrOffsets[2] = chrBankOffset(_registers[4]);
        chrOffsets[3] = chrBankOffset(_registers[5]);
        chrOffsets[4] = chrBankOffset(_registers[0] & 0xFE);
        chrOffsets[5] = chrBankOffset(_registers[0] | 0x01);
        chrOffsets[6] = chrBankOffset(_registers[1] & 0xFE);
        chrOffsets[7] = chrBankOffset(_registers[1] | 0x01);
        break;
    }
}
