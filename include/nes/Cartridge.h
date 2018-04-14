#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>

struct INesHeader {

    uint32_t magic;
    uint8_t numPRG;
    uint8_t numCHR;
    uint8_t controller1;
    uint8_t controller2;

    uint8_t numRAM;
};

class Cartridge {
public:
    static constexpr uint16_t SRAM_SIZE = 0x2000;

    uint8_t *prg;
    uint8_t *chr;
    uint8_t sram[SRAM_SIZE];

    uint8_t mapper;
    uint8_t mirror;
    uint8_t battery;

    bool trainer;

public:
    Cartridge();
    ~Cartridge();
    bool loadNesFile(const char *path);
    int prgLength() const;
    int chrLength() const;

private:
    INesHeader header;
    int mPRGLength;
    int mCHRLength;
};

#endif // CARTRIDGE_H