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

    uint8_t* prg;
    uint8_t* chr;
    uint8_t sram[SRAM_SIZE];

    uint8_t mapper;
    uint8_t mirror;
    uint8_t battery;

    bool trainer;

public:
    Cartridge();
    ~Cartridge();
    bool loadNesFile(const char * path);
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);

    uint16_t nameTableAddress(uint16_t address);

private:
    INesHeader header;
    int prgBanks;
    int chrBanks;
    int prgBank1;
    int prgBank2;
    int chrBank;
};

#endif // CARTRIDGE_H