#ifndef MAPPER7_H
#define MAPPER7_H
#include "nes/Mapper.h"
#include "nes/Cartridge.h"

class Mapper7 : public Mapper {
public:
    Mapper7(Cartridge *cartridge);
    ~Mapper7() override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void step() override;

    virtual void save(Serialize &serialize) override;
    virtual void load(Serialize &serialize) override;

private:
    Cartridge *mCartridge;
    int mPrgBank;
};

#endif