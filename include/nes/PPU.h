#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include "nes/Image.h"

class Console;

class PPU {
public:
    static constexpr uint32_t OAM_DATA_SIZE = 256;
    static constexpr uint32_t PALETTE_DATA_SIZE = 32;
    static constexpr uint32_t NAME_TABLE_DATA_SIZE = 2048;

public:
    PPU(Console *console);
    ~PPU();
    void reset();
    uint8_t readRegister(uint16_t address);
    void writeRegister(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    uint16_t read16(uint16_t address);
    void write(uint16_t address, uint8_t value);
    void step();

private:
    uint8_t readPalette(uint16_t address);
    void writePalette(uint16_t address, uint8_t value);


    void writeControl(uint8_t value);
    void writeMask(uint8_t value);

    uint8_t readStatus();

    void writeOAMAddress(uint8_t value);
    uint8_t readOAMData();
    void writeOAMData(uint8_t value);

    void writeScroll(uint8_t value);

    void writeAddress(uint8_t value);
    uint8_t readData();
    void writeData(uint8_t value);
    void writeDMA(uint8_t value);

    void incrementX();
    void incrementY();

    void copyX();
    void copyY();

    void setVerticalBlank();
    void clearVerticalBlank();

    void fetchNameTableByte();
    void fetchAttributeTableByte();
    void fetchLowTileByte();
    void fetchHighTileByte();

    void storeTileData();
    uint32_t fetchTileData();

    uint8_t backgroundPixel();
    void spritePixel(uint8_t *i, uint8_t *sprite);

    void renderPixel();

    uint32_t fetchSpritePattern(int i, int row);
    void evaluteSprites();

    void tick();

public:
    uint64_t frame; // frame counter

private:
    Console *console;
    uint32_t cycle; // 0-340
    uint32_t scanLine; // 0-261, 0-239=visible, 240=post, 241-260=vblank, 261=pre

    uint8_t verticalBlank; // vertical blank flag

    uint8_t paletteData[PALETTE_DATA_SIZE];
    uint8_t nameTableData[NAME_TABLE_DATA_SIZE];
    uint8_t oamData[OAM_DATA_SIZE];
    Image *buffer;

    uint16_t v;
    uint16_t t;
    uint8_t x;
    uint8_t w;
    uint8_t f;

    uint8_t nameTableByte;
    uint8_t attributeTableByte;
    uint8_t lowTileByte;
    uint8_t highTileByte;
    uint64_t tileData;

    int spriteCount;
    uint32_t spritePatterns[8];
    uint8_t spritePositions[8];
    uint8_t spritePriorities[8];
    uint8_t spriteIndexes[8];

    // $2000 PPUCTRL
    uint8_t flagNameTable;       // 0: $2000; 1: $2400; 2: $2800; 3: $2C00
    uint8_t flagIncrement;       // 0: +1; 1: +32
    uint8_t flagSpriteTable;     // 0: $0000; 1: $1000; ignored in 8*16 mode
    uint8_t flagBackgroundTable; // 0: $0000; 1: $1000
    uint8_t flagSpriteSize;      // 0: 8*8; 1: 8*16
    uint8_t flagMasterSlave;     // 0: read EXT; 1: write EXT
    uint8_t flagGenerateNMI;     // 0: off; 1: on

    // $2001 PPUMASK
    uint8_t flagGrayScale;          // 0: color; 1: gray scale
    uint8_t flagShowLeftBackground; // 0: hide; 1: show
    uint8_t flagShowLeftSprites;    // 0: hide; 1: show
    uint8_t flagShowBackground;     // 0: hide; 1: show
    uint8_t flagShowSprites;        // 0: hide; 1: show
    uint8_t flagRedHit;             // 0: normal; 1: emphasized
    uint8_t flagGreenHit;           // 0: normal; 1: emphasized
    uint8_t flagBlueHit;            // 0: normal; 1: emphasized

    // $2002 PPUSTATUS
    uint8_t flagSpriteZeroHit;
    uint8_t flagSpriteOverflow;

    // $2003 OAMADDR
    uint8_t oamAddress;

    // $2005 PPUSCROLL
    uint16_t scroll;

    // $2006 PPUADDR
    //uint16_t address; // address used by $2007 PPUDATA

    // $2007 PPUDATA
    uint8_t bufferedData;

};

#endif