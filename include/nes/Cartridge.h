#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
class Serialize;

enum class Mirror { Horizontal = 0, Vertical = 1, Single0 = 2, Single1 = 3, Four = 4 };

struct INesHeader {

    uint32_t magic;
    uint8_t numPRG;
    uint8_t numCHR;
    uint8_t controller1;
    uint8_t controller2;

    uint8_t numRAM;
};

class Cartridge {
public:
    static constexpr int SRAM_SIZE = 0x2000;

    Cartridge();
    ~Cartridge();
    bool loadNesFile(const char *path);

    uint8_t readPRG(int index) const;

    void writeCHR(int index, uint8_t value);
    uint8_t readCHR(int index) const;

    void writeSRAM(int index, uint8_t value);
    uint8_t readSRAM(int index) const;

    void setMirror(Mirror mirror);
    Mirror currentMirror() const;
    uint8_t currentMapper() const;

    int prgLength() const;
    int chrLength() const;

    void save(Serialize &serialize);
    void load(Serialize &serialize);

private:
    INesHeader mHeader;
    uint8_t *mPRG;
    uint8_t *mCHR;
    uint8_t mSRAM[SRAM_SIZE];

    uint8_t mMapper;
    Mirror mMirror;
    uint8_t mBattery;

    bool mTrainer;

    int mPRGLength;
    int mCHRLength;
};

#endif // CARTRIDGE_H