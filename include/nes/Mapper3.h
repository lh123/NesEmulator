#ifndef MAPPER3_H
#define MAPPER3_H
#include "nes/Mapper.h"
#include "nes/Cartridge.h"

class Mapper3 : public Mapper {
public:
    Mapper3(Cartridge *cartridge);
    ~Mapper3() override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void step() override;

private:
    Cartridge *mCartridge;
    int mChrBank;
    int mPrgBank1;
    int mPrgBank2;
};

#endif