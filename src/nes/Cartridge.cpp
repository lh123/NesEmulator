#include "nes/Cartridge.h"
#include "nes/Serialize.hpp"
#include <cstdio>

const uint32_t NesMagic = 0x1a53454e;

Cartridge::Cartridge()
    : prg(nullptr), chr(nullptr), sram{0}, mapper(0), mirror(0), battery(0), trainer(false), header{0}, mPRGLength(0),
      mCHRLength(0) {}

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
        std::printf("open file \"%s\" failed!\n", path);
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

    uint8_t mapper1 = header.controller1 >> 4;
    uint8_t mapper2 = header.controller2 >> 4;
    mapper = mapper1 | (mapper2 << 4);
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

int Cartridge::prgLength() const { return mPRGLength; }

int Cartridge::chrLength() const { return mCHRLength; }

void Cartridge::save(Serialize &serialize) { serialize.writeArray(sram, SRAM_SIZE); }

void Cartridge::load(Serialize &serialize) { serialize.readArray(sram, SRAM_SIZE); }