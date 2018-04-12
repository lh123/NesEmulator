#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>

class Console;

enum class Mirror {
    Horizontal = 0,
    Vertical = 1,
    Single0 = 2,
    Single1 = 3,
    Four = 4
};

constexpr const int MirrorLookUp[4][4] = {
    {0, 0, 1, 1}, //
    {0, 1, 0, 1}, //
    {0, 0, 0, 0}, //
    {0, 1, 2, 3}  //
};

class Memory {
public:
    Memory(Console *console);
    virtual uint8_t read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t value) = 0;

protected:
    uint16_t mirrorAddress(uint8_t mode, uint16_t address);

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