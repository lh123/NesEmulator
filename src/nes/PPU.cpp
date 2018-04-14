#include "nes/PPU.h"
#include "nes/Console.h"
#include <cstdio>

PPU::PPU(Console *console)
    : PPUMemory(console), cycle(0), scanLine(0),
      frame(0), paletteData{0}, nameTableData{0}, oamData{0}, v(0), t(0), x(0), w(0), f(0), reg(0),
      nmiOccurred(false), nmiOutput(false), nmiPrevious(false), nmiDelay(0), nameTableByte(0),
      attributeTableByte(0), lowTileByte(0), highTileByte(0), tileData(0),
      spriteCount(0), spritePatterns{0}, spritePositions{0}, spritePriorities{0}, spriteIndexes{0},
      flagNameTable(0), flagIncrement(0), flagSpriteTable(0), flagBackgroundTable(0),
      flagSpriteSize(0), flagMasterSlave(0), flagGrayScale(0), flagShowLeftBackground(0),
      flagShowLeftSprites(0), flagShowBackground(0), flagShowSprites(0), flagRedTint(0),
      flagGreenTint(0), flagBlueTint(0), flagSpriteZeroHit(0), flagSpriteOverflow(0), oamAddress(0),
      bufferedData(0) {
    front = new Image{256, 240};
    back = new Image{256, 240};
    reset();
}

PPU::~PPU() {
    delete front;
    delete back;
}

void PPU::reset() {
    cycle = 340;
    scanLine = 240;
    frame = 0;
    writeControl(0);
    writeMask(0);
    writeOAMAddress(0);
}

uint8_t PPU::readPalette(uint16_t address) {
    if (address >= 16 && (address % 4) == 0) {
        address -= 16;
    }
    return paletteData[address];
}

void PPU::writePalette(uint16_t address, uint8_t value) {
    if (address >= 16 && (address % 4) == 0) {
        address -= 16;
    }
    paletteData[address] = value;
}

uint8_t PPU::readRegister(uint16_t address) {
    switch (address) {
    case 0x2002:
        return readStatus();
    case 0x2004:
        return readOAMData();
    case 0x2007:
        return readData();
    default:
        std::printf("error: ppu read at address: %04X\n", address);
        return 0;
    }
}

void PPU::writeRegister(uint16_t address, uint8_t value) {
    reg = value;
    switch (address) {
    case 0x2000:
        writeControl(value);
        break;
    case 0x2001:
        writeMask(value);
        break;
    case 0x2003:
        writeOAMAddress(value);
        break;
    case 0x2004:
        writeOAMData(value);
        break;
    case 0x2005:
        writeScroll(value);
        break;
    case 0x2006:
        writeAddress(value);
        break;
    case 0x2007:
        writeData(value);
        break;
    case 0x4014:
        writeDMA(value);
        break;
    default:
        std::printf("error: ppu write at address: %04X\n", address);
    }
}

void PPU::writeControl(uint8_t value) {
    flagNameTable = (value >> 0) & 0x3;
    flagIncrement = (value >> 2) & 0x1;
    flagSpriteTable = (value >> 3) & 0x1;
    flagBackgroundTable = (value >> 4) & 0x1;
    flagSpriteSize = (value >> 5) & 0x1;
    flagMasterSlave = (value >> 6) & 0x1;
    nmiOutput = ((value >> 7) & 0x1) == 0x1;
    nmiChange();
    // t: ....AA.. ........; value: ......AA
    t = (t & 0xF3FF) | ((uint16_t(value) & 0x03) << 10);
}

void PPU::writeMask(uint8_t value) {
    flagGrayScale = (value >> 0) & 0x1;
    flagShowLeftBackground = (value >> 1) & 0x1;
    flagShowLeftSprites = (value >> 2) & 0x1;
    flagShowBackground = (value >> 3) & 0x1;
    flagShowSprites = (value >> 4) & 0x1;
    flagRedTint = (value >> 5) & 0x1;
    flagGreenTint = (value >> 6) & 0x1;
    flagBlueTint = (value >> 7) & 0x1;
}

uint8_t PPU::readStatus() {
    uint8_t result = reg & 0x1F;
    result |= flagSpriteOverflow << 5;
    result |= flagSpriteZeroHit << 6;
    if (nmiOccurred) {
        result |= 1 << 7;
    }
    nmiOccurred = false;
    nmiChange();
    w = 0;
    return result;
}

void PPU::writeOAMAddress(uint8_t value) { oamAddress = value; }

uint8_t PPU::readOAMData() { return oamData[oamAddress]; }

void PPU::writeOAMData(uint8_t value) {
    oamData[oamAddress] = value;
    oamAddress++;
}

void PPU::writeScroll(uint8_t value) {
    if (w == 0) {
        // t: ........ ...ABCDE = value: ABCDE...
        // x:               ABC = value: .....ABC
        // w:                   = 1
        t = (t & 0xFFE0) | (uint16_t(value) >> 3);
        x = value & 0x07;
        w = 1;
    } else {
        // t: .ABC..DE FGH..... = value: DEFGHABC
        // w:                   = 0
        t = (t & 0x8FFF) | ((uint16_t(value) & 0x07) << 12);
        t = (t & 0xFC1F) | ((uint16_t(value) & 0xF8) << 2);
        w = 0;
    }
}

void PPU::writeAddress(uint8_t value) {
    if (w == 0) {
        // t: ..ABCDEF ........ = value: ..ABCDEF
        // t: .A...... ........ = 0
        // w:                   = 1
        t = (t & 0x80FF) | ((uint16_t(value) & 0x3F) << 8);
        w = 1;
    } else {
        // t: ........ ABCDEFGH = value: ABCDEFGH
        // v:                   = t
        // w:                   = 0
        t = (t & 0xFF00) | uint16_t(value);
        v = t;
        w = 0;
    }
}

uint8_t PPU::readData() {
    uint8_t value = read(v);
    // emulate buffered reads
    if (v % 0x4000 < 0x3F00) {
        uint8_t buffered = bufferedData;
        bufferedData = value;
        value = buffered;
    } else {
        bufferedData = read(v - 0x1000);
    }
    if (flagIncrement == 0) {
        v += 1;
    } else {
        v += 32;
    }
    return value;
}

void PPU::writeData(uint8_t value) {
    write(v, value);
    if (flagIncrement == 0) {
        v += 1;
    } else {
        v += 32;
    }
}

void PPU::writeDMA(uint8_t value) {
    CPU *cpu = console->cpu;
    uint16_t address = uint16_t(value) << 8;
    for (int i = 0; i < 256; i++) {
        oamData[oamAddress] = cpu->read(address);
        oamAddress++;
        address++;
    }
    cpu->stall += 513;
    if (cpu->cycles % 2 == 1) {
        cpu->stall++;
    }
}

void PPU::incrementX() {
    if ((v & 0x001F) == 31) {
        v &= 0xFFE0;
        v ^= 0x0400;
    } else {
        v++;
    }
}

void PPU::incrementY() {
    if ((v & 0x7000) != 0x7000) {
        v += 0x1000;
    } else {
        v &= 0x8FFF;
        uint16_t y = (v & 0x03E0) >> 5;
        if (y == 29) {
            y = 0;
            v ^= 0x0800;
        } else if (y == 31) {
            y = 0;
        } else {
            y++;
        }
        v = (v & 0xFC1F) | (y << 5);
    }
}

void PPU::copyX() {
    // v: .....A.. ...BCDEF = t: .....A.. ...BCDEF
    v = (v & 0xFBE0) | (t & 0x041F);
}

void PPU::copyY() {
    // v: .ABCD.EF GHI..... = t: .ABCD.EF GHI.....
    v = (v & 0x841F) | (t & 0x7BE0);
}

void PPU::nmiChange() {
    bool nmi = nmiOutput && nmiOccurred;
    if (nmi && !nmiPrevious) {
        nmiDelay = 15;
    }
    nmiPrevious = nmi;
}

void PPU::setVerticalBlank() {
    Image *temp = front;
    front = back;
    back = temp;
    nmiOccurred = true;
    nmiChange();
}

void PPU::clearVerticalBlank() {
    nmiOccurred = false;
    nmiChange();
}

void PPU::fetchNameTableByte() {
    uint16_t address = 0x2000 | (v & 0x0FFF);
    nameTableByte = read(address);
}

void PPU::fetchAttributeTableByte() {
    uint16_t address = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
    uint16_t shift = ((v >> 4) & 0x4) | (v & 0x2);
    attributeTableByte = ((read(address) >> shift) & 0x3) << 2;
}

void PPU::fetchLowTileByte() {
    uint16_t fineY = (v >> 12) & 0x7;
    uint8_t table = flagBackgroundTable;
    uint8_t tile = nameTableByte;
    uint16_t address = 0x1000 * uint16_t(table) + uint16_t(tile) * 16 + fineY;
    lowTileByte = read(address);
}

void PPU::fetchHighTileByte() {
    uint16_t fineY = (v >> 12) & 0x7;
    uint8_t table = flagBackgroundTable;
    uint8_t tile = nameTableByte;
    uint16_t address = 0x1000 * uint16_t(table) + uint16_t(tile) * 16 + fineY;
    highTileByte = read(address + 8);
}

void PPU::storeTileData() {
    uint32_t data = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t a = attributeTableByte;
        uint8_t p1 = (lowTileByte & 0x80) >> 7;
        uint8_t p2 = (highTileByte & 0x80) >> 6;

        lowTileByte <<= 1;
        highTileByte <<= 1;
        data <<= 4;
        data |= uint32_t(a | p1 | p2);
    }
    tileData |= uint64_t(data);
}

uint32_t PPU::fetchTileData() { return uint32_t(tileData >> 32); }

uint8_t PPU::backgroundPixel() {
    if (flagShowBackground == 0) {
        return 0;
    }
    uint32_t data = fetchTileData() >> ((7 - x) * 4);
    return uint8_t(data & 0x0F);
}

void PPU::spritePixel(uint8_t *index, uint8_t *sprite) {
    if (flagShowSprites == 0) {
        *index = 0;
        *sprite = 0;
        return;
    }
    for (int i = 0; i < spriteCount; i++) {
        uint32_t offset = (cycle - 1) - spritePositions[i];
        if (offset < 0 || offset > 7) {
            continue;
        }
        offset = 7 - offset;
        uint8_t color = uint8_t((spritePatterns[i] >> uint8_t(offset * 4)) & 0x0F);
        if (color % 4 == 0) {
            continue;
        }
        *index = uint8_t(i);
        *sprite = color;
        return;
    }
    *index = 0;
    *sprite = 0;
    return;
}

void PPU::renderPixel() {
    uint32_t x = cycle - 1;
    uint32_t y = scanLine;
    uint8_t background = backgroundPixel();
    uint8_t i = 0, sprite = 0;
    spritePixel(&i, &sprite);
    if (x < 8 && flagShowLeftBackground == 0) {
        background = 0;
    }
    if (x < 8 && flagShowLeftSprites == 0) {
        sprite = 0;
    }
    bool b = (background % 4) != 0;
    bool s = (sprite % 4) != 0;
    uint8_t color = 0;
    if (!b && !s) {
        color = 0;
    } else if (!b && s) {
        color = sprite | 0x10;
    } else if (b && !s) {
        color = background;
    } else {
        if (spriteIndexes[i] == 0 && x < 255) {
            flagSpriteZeroHit = 1;
        }
        if (spritePriorities[i] == 0) {
            color = sprite | 0x10;
        } else {
            color = background;
        }
    }
    uint32_t c = palette[readPalette(uint16_t(color)) % 64];
    back->setRGBA(x, y, c);
}

uint32_t PPU::fetchSpritePattern(int i, int row) {
    uint8_t tile = oamData[i * 4 + 1];
    uint8_t attributes = oamData[i * 4 + 2];
    uint16_t address = 0;
    if (flagSpriteSize == 0) {
        if ((attributes & 0x80) == 0x80) {
            row = 7 - row;
        }
        uint8_t table = flagSpriteTable;
        address = 0x1000 * uint16_t(table) + uint16_t(tile) * 16 + uint16_t(row);
    } else {
        if ((attributes & 0x80) == 0x80) {
            row = 15 - row;
        }
        uint8_t table = tile & 0x1;
        tile &= 0xFE;
        if (row > 7) {
            tile++;
            row -= 8;
        }
        address = 0x1000 * uint16_t(table) + uint16_t(tile) * 16 + uint16_t(row);
    }
    uint8_t a = (attributes & 0x3) << 2;
    uint8_t lowTileByte = read(address);
    uint8_t highTileByte = read(address + 8);
    uint32_t data = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t p1 = 0, p2 = 0;
        if ((attributes & 0x40) == 0x40) {
            p1 = (lowTileByte & 0x1) << 0;
            p2 = (highTileByte & 0x1) << 1;
            lowTileByte >>= 1;
            highTileByte >>= 1;
        } else {
            p1 = (lowTileByte & 0x80) >> 7;
            p2 = (highTileByte & 0x80) >> 6;
            lowTileByte <<= 1;
            highTileByte <<= 1;
        }
        data <<= 4;
        data |= uint32_t(a | p1 | p2);
    }
    return data;
}

void PPU::evaluateSprites() {
    int h = 0;
    if (flagSpriteSize == 0) {
        h = 8;
    } else {
        h = 16;
    }
    int count = 0;
    for (int i = 0; i < 64; i++) {
        uint8_t y = oamData[i * 4 + 0];
        uint8_t a = oamData[i * 4 + 2];
        uint8_t x = oamData[i * 4 + 3];
        int row = scanLine - int(y);
        if (row < 0 || row >= h) {
            continue;
        }
        if (count < 8) {
            spritePatterns[count] = fetchSpritePattern(i, row);
            spritePositions[count] = x;
            spritePriorities[count] = (a >> 5) & 0x1;
            spriteIndexes[count] = uint8_t(i);
        }
        count++;
    }
    if (count > 8) {
        count = 8;
        flagSpriteOverflow = 1;
    }
    spriteCount = count;
}

void PPU::tick() {
    if (nmiDelay > 0) {
        nmiDelay--;
        if (nmiDelay == 0 && nmiOutput && nmiOccurred) {
            console->cpu->triggerNMI();
        }
    }
    if (flagShowBackground != 0 || flagShowSprites != 0) {
        if (f == 1 && scanLine == 261 && cycle == 339) {
            cycle = 0;
            scanLine = 0;
            frame++;
            f ^= 0x1;
            return;
        }
    }
    cycle++;
    if (cycle > 340) {
        cycle = 0;
        scanLine++;
        if (scanLine > 261) {
            scanLine = 0;
            frame++;
            f ^= 0x1;
        }
    }
}

void PPU::step() {
    tick();

    bool renderingEnabled = (flagShowBackground != 0 || flagShowSprites != 0);
    bool preLine = (scanLine == 261);
    bool visibleLine = (scanLine < 240);
    bool renderLine = (preLine || visibleLine);
    bool preFetchCycle = (cycle >= 321 && cycle <= 336);
    bool visibleCycle = (cycle >= 1 && cycle <= 256);
    bool fetchCycle = (preFetchCycle || visibleCycle);

    if (renderingEnabled) {
        if (visibleLine && visibleCycle) {
            renderPixel();
        }
        if (renderLine && fetchCycle) {
            tileData <<= 4;
            switch (cycle % 8) {
            case 1:
                fetchNameTableByte();
                break;
            case 3:
                fetchAttributeTableByte();
                break;
            case 5:
                fetchLowTileByte();
                break;
            case 7:
                fetchHighTileByte();
                break;
            case 0:
                storeTileData();
                break;
            }
        }
        if (preLine && cycle >= 280 && cycle <= 304) {
            copyY();
        }
        if (renderLine) {
            if (fetchCycle && cycle % 8 == 0) {
                incrementX();
            }
            if (cycle == 256) {
                incrementY();
            }
            if (cycle == 257) {
                copyX();
            }
        }
    }

    if (renderingEnabled) {
        if (cycle == 257) {
            if (visibleLine) {
                evaluateSprites();
            } else {
                spriteCount = 0;
            }
        }
    }
    if (scanLine == 241 && cycle == 1) {
        setVerticalBlank();
    }
    if (preLine && cycle == 1) {
        clearVerticalBlank();
        flagSpriteZeroHit = 0;
        flagSpriteOverflow = 0;
    }
}