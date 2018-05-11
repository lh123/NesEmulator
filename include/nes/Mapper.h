#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>

class Console;
class Serialize;

class Mapper {
public:
    Mapper();
    virtual ~Mapper();
    virtual uint8_t read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t value) = 0;
    virtual void step() = 0;

    virtual void save(Serialize &serialize) = 0;
    virtual void load(Serialize &serialize) = 0;

    static Mapper *create(Console *console);
    static void free(Mapper *mapper);
};

#endif