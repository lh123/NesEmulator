#include "nes/Memory.h"
#include "nes/Console.h"
#include "nes/CPU.h"
#include "nes/PPU.h"
#include "nes/APU.h"
#include "nes/Mapper.h"
#include "nes/Controller.h"
#include "nes/Cartridge.h"
#include <cstdio>

static const int MirrorLookUp[5][4] = {
    {0, 0, 1, 1}, //
    {0, 1, 0, 1}, //
    {0, 0, 0, 0}, //
    {1, 1, 1, 1}, //
    {0, 1, 2, 3}  //
};

Memory::Memory(Console *console) : console(console) {}

Memory::~Memory() {}

uint8_t CPUMemory::read(uint16_t address) {
    if (address < 0x2000) {
        return console->ram[address % 0x0800];
    } else if (address < 0x4000) {
        return console->ppu->readRegister(0x2000 + address % 8);
    } else if (address == 0x4014) {
        return console->ppu->readRegister(address);
    } else if (address == 0x4015) {
        return console->apu->readRegister(address);
    } else if (address == 0x4016) {
        return console->controller1->read();
    } else if (address == 0x4017) {
        return console->controller2->read();
    } else if (address < 0x6000) {
        // TODO I/O register
        return 0;
    } else if (address >= 0x6000) {
        return console->mapper->read(address);
    } else {
        std::printf("error: cpu read at address: 0x%04X\n", address);
        return 0;
    }
}

void CPUMemory::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        console->ram[address % 0x0800] = value;
    } else if (address < 0x4000) {
        console->ppu->writeRegister(0x2000 + address % 8, value);
    } else if (address < 0x4014) {
        console->apu->writeRegister(address, value);
    } else if (address == 0x4014) {
        console->ppu->writeRegister(address, value);
    } else if (address == 0x4015) {
        console->apu->writeRegister(address, value);
    } else if (address == 0x4016) {
        console->controller1->write(value);
        console->controller2->write(value);
    } else if (address == 0x4017) {
        console->apu->writeRegister(address, value);
    } else if (address < 0x6000) {
        // TODO: I/O register
    } else if (address >= 0x6000) {
        console->mapper->write(address, value);
    } else {
        std::printf("error: cpu write at address: 0x%04X\n", address);
    }
}

uint8_t PPUMemory::read(uint16_t address) {
    address = address % 0x4000;
    if (address < 0x2000) {
        return console->mapper->read(address);
    } else if (address < 0x3F00) {
        uint8_t mode = console->cartridge->mirror;
        return console->ppu
            ->nameTableData[mirrorAddress(mode, address) % PPU::NAME_TABLE_DATA_SIZE];
    } else if (address < 0x4000) {
        return console->ppu->readPalette(address % 32);
    } else {
        std::printf("error: ppu read at address: 0x%04X\n", address);
    }
    return 0;
}

void PPUMemory::write(uint16_t address, uint8_t value) {
    address = address % 0x4000;
    if (address < 0x2000) {
        console->mapper->write(address, value);
    } else if (address < 0x3F00) {
        uint8_t mode = console->cartridge->mirror;
        console->ppu->nameTableData[mirrorAddress(mode, address) % PPU::NAME_TABLE_DATA_SIZE] =
            value;
    } else if (address < 0x4000) {
        console->ppu->writePalette(address % 32, value);
    } else {
        std::printf("error: ppu write at address: 0x%04X\n", address);
    }
}

uint16_t Memory::mirrorAddress(uint8_t mode, uint16_t address) {
    address = (address - 0x2000) % 0x1000;
    uint16_t table = address / 0x0400;
    uint16_t offset = address % 0x0400;
    return 0x2000 + MirrorLookUp[mode][table] * 0x0400 + offset;
}