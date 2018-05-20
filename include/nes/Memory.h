#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>

class Console;
enum class Mirror;

static const int MirrorLookUp[5][4] = {
    {0, 0, 1, 1}, //
    {0, 1, 0, 1}, //
    {0, 0, 0, 0}, //
    {1, 1, 1, 1}, //
    {0, 1, 2, 3}  //
};

class Memory {
public:
    Memory(Console *console);
    virtual ~Memory();
    virtual uint8_t read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t value) = 0;

protected:
    uint16_t mirrorAddress(Mirror mode, uint16_t address);

protected:
    Console *console;
};

class CPUMemory : public Memory {
public:
    CPUMemory(Console *console) : Memory(console) {}
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
};

class PPUMemory : public Memory {
public:
    PPUMemory(Console *console) : Memory(console) {}
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
};

#endif