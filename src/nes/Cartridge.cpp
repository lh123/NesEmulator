#include "nes/Cartridge.h"

#include <cstdio>

const uint32_t NesMagic = 0x1a53454e;

Cartridge::Cartridge()
    : prg(nullptr), chr(nullptr), mPRGLength(0), mCHRLength(0), sram{0},
      mapper(0), mirror(0), battery(0), trainer(false) {}

Cartridge::~Cartridge() {
    delete[] chr;
    delete[] prg;
}

/**
 * 读取Nes文件
 * @param path nes文件路径
 * @return true读取成功 false读取失败
 */
bool Cartridge::loadNesFile(const char *path) {
    std::FILE *file = std::fopen(path, "rb");
    if (file == nullptr) {
        std::printf("open file \"%s\" failed!\n");
        return false;
    }
    std::fread(&header.magic, sizeof(header.magic), 1, file);
    if (header.magic != NesMagic) {
        std::printf("invalid nes file!\n");
        return false;
    }

    std::fread(&header.numPRG, sizeof(header.numPRG), 1, file);
    std::fread(&header.numCHR, sizeof(header.numPRG), 1, file);
    std::fread(&header.controller1, sizeof(header.controller1), 1, file);
    std::fread(&header.controller2, sizeof(header.controller2), 1, file);
    std::fread(&header.numRAM, sizeof(header.numRAM), 1, file);
    uint8_t padding[7];
    std::fread(padding, sizeof(padding), 1, file);
    for (int i = 0; i < 7; i++) {
        if (padding[i] != 0) {
            std::printf("invalid nes file!\n");
            return false;
        }
    }

    mapper = (header.controller1 >> 4) | (header.controller2 & 0xF0);
    mirror = (header.controller1 & 0x1) | ((header.controller1 >> 2) & 0x2);
    battery = (header.controller1 >> 1) & 0x1;

    trainer = (header.controller1 & 0x4) == 0x4;

    if (trainer) {
        std::printf("current not support trainer\n");
        return false;
    }

    mPRGLength = header.numPRG * 0x4000;
    prg = new uint8_t[mPRGLength];
    std::fread(prg, mPRGLength, 1, file);

    if (header.numCHR != 0) {
        mCHRLength = header.numCHR * 0x2000;
        chr = new uint8_t[mCHRLength];
        std::fread(chr, mCHRLength, 1, file);
    } else {
        mCHRLength = 0x2000;
        chr = new uint8_t[mCHRLength]{0};
    }

    std::fclose(file);
    return true;
}

uint8_t Cartridge::read(uint16_t address) {
    if (address < 0x2000) {
        return chr[address];
    } else if (address >= 0x8000) {
        uint16_t index = (address - 0x8000) % (header.numPRG * 16384);
        return prg[index];
    } else if (address >= 0x6000) {
        uint16_t index = address - 0x6000;
        return sram[index];
    } else {
        std::printf("error: cartridge read at address: %04X\n", address);
        return 0;
    }
}

void Cartridge::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        chr[address] = value;
    } else if (address >= 0x8000) {
        // todo
    } else if (address >= 0x6000) {
        uint16_t index = address - 0x6000;
        sram[index] = value;
    } else {
        std::printf("error: cartridge read at address: %04X\n", address);
    }
}

uint8_t Cartridge::prgLength() const { return mPRGLength; }

uint8_t Cartridge::chrLength() const { return mCHRLength; }

// uint16_t Cartridge::nameTableAddress(uint16_t address) {
//     address = (address - 0x2000) % 0x1000;
//     uint16_t table = address / 0x0400;
//     uint16_t offset = address % 0x0400;
//     return 0x2000 + MirrorLookUp[mirror][table] * 0x0400 + offset;
// }