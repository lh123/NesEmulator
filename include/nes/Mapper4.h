#ifndef MAPPER4_H
#define MAPPER4_H

#include "nes/Cartridge.h"
#include "nes/Mapper.h"

class Console;

class Mapper4 : public Mapper {
public:
    Mapper4(Console *console, Cartridge *cartridge);
    ~Mapper4() override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void step() override;

    virtual void save(Serialize &serialize) override;
    virtual void load(Serialize &serialize) override;

private:
    void handleScanLine();
    void writeRegister(uint16_t address, uint8_t value);
    void writeBankSelect(uint8_t value);
    void writeBankData(uint8_t value);
    void writeMirror(uint8_t value);
    void writeProtect(uint8_t value);
    void writeIRQLatch(uint8_t value);
    void writeIRQReload(uint8_t value);
    void writeIRQDisable(uint8_t value);
    void writeIRQEnable(uint8_t value);

    int prgBankOffset(int index);
    int chrBankOffset(int index);

    void updateOffset();

private:
    Console *console;
    Cartridge *cartridge;
    uint8_t _register;
    uint8_t _registers[8];
    uint8_t prgMode;
    uint8_t chrMode;
    int prgOffsets[4];
    int chrOffsets[8];
    uint8_t reload;
    uint8_t counter;
    bool irqEnable;
};

#endif