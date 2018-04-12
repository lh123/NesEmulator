#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>

class Console;

class Mapper {
public:
    virtual ~Mapper();
    virtual uint8_t read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t value) = 0;

    static Mapper *create(Console *console);
    static void free(Mapper *mapper);
};

#endif