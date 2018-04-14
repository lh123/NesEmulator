#ifndef MAPPER2_H
#define MAPPER2_H

#include "nes/Cartridge.h"
#include "nes/Mapper.h"

class Mapper2 : public Mapper {
public:
    Mapper2(Cartridge *cartridge);
    ~Mapper2();
    virtual uint8_t read(uint16_t address) override;
    virtual void write(uint16_t address, uint8_t value) override;
    virtual void step() override;

private:
    Cartridge *cartridge;
    int prgBanks;
    int prgBank1;
    int prgBank2;
};

#endif