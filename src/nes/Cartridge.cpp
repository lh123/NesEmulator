#include "nes/Cartridge.h"
#include "nes/Serialize.hpp"
#include <cstdio>

const uint32_t NesMagic = 0x1a53454e;

Cartridge::Cartridge()
    : mHeader{0}, mPRG(nullptr), mCHR(nullptr), mSRAM{0}, mMapper(0), mMirror(Mirror::Horizontal), mBattery(0), mTrainer(false),
      mPRGLength(0), mCHRLength(0) {}

Cartridge::~Cartridge() {
    delete[] mCHR;
    delete[] mPRG;
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
    std::fread(&mHeader.magic, sizeof(mHeader.magic), 1, file);
    if (mHeader.magic != NesMagic) {
        std::printf("invalid nes file!\n");
        return false;
    }

    std::fread(&mHeader.numPRG, sizeof(mHeader.numPRG), 1, file);
    std::fread(&mHeader.numCHR, sizeof(mHeader.numPRG), 1, file);
    std::fread(&mHeader.controller1, sizeof(mHeader.controller1), 1, file);
    std::fread(&mHeader.controller2, sizeof(mHeader.controller2), 1, file);
    std::fread(&mHeader.numRAM, sizeof(mHeader.numRAM), 1, file);
    uint8_t padding[7];
    std::fread(padding, sizeof(padding), 1, file);
    for (int i = 0; i < 7; i++) {
        if (padding[i] != 0) {
            std::printf("invalid nes file!\n");
            return false;
        }
    }

    uint8_t mapper1 = mHeader.controller1 >> 4;
    uint8_t mapper2 = mHeader.controller2 >> 4;
    mMapper = mapper1 | (mapper2 << 4);
    mMirror = static_cast<Mirror>((mHeader.controller1 & 0x1) | ((mHeader.controller1 >> 2) & 0x2));
    mBattery = (mHeader.controller1 >> 1) & 0x1;

    mTrainer = (mHeader.controller1 & 0x4) == 0x4;

    if (mTrainer) {
        std::printf("current not support trainer\n");
        return false;
    }

    mPRGLength = mHeader.numPRG * 0x4000;
    mPRG = new uint8_t[mPRGLength];
    std::fread(mPRG, mPRGLength, 1, file);

    if (mHeader.numCHR != 0) {
        mCHRLength = mHeader.numCHR * 0x2000;
        mCHR = new uint8_t[mCHRLength];
        std::fread(mCHR, mCHRLength, 1, file);
    } else {
        mCHRLength = 0x2000;
        mCHR = new uint8_t[mCHRLength]{0};
    }

    std::fclose(file);
    return true;
}

uint8_t Cartridge::readPRG(int index) const { return mPRG[index]; }

void Cartridge::writeCHR(int index, uint8_t value) { mCHR[index] = value; }

uint8_t Cartridge::readCHR(int index) const { return mCHR[index]; }

void Cartridge::writeSRAM(int index, uint8_t value) { mSRAM[index] = value; }

uint8_t Cartridge::readSRAM(int index) const { return mSRAM[index]; }

void Cartridge::setMirror(Mirror mirror) { mMirror = mirror; }

Mirror Cartridge::currentMirror() const { return mMirror; }

uint8_t Cartridge::currentMapper() const { return mMapper; }

int Cartridge::prgLength() const { return mPRGLength; }

int Cartridge::chrLength() const { return mCHRLength; }

void Cartridge::save(Serialize &serialize) {
    serialize.writeArray(mCHR, mCHRLength);
    serialize.writeArray(mSRAM, SRAM_SIZE);
    serialize << mMirror;
}

void Cartridge::load(Serialize &serialize) {
    serialize.readArray(mCHR, mCHRLength);
    serialize.readArray(mSRAM, SRAM_SIZE);
    serialize >> mMirror;
}