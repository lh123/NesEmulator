#ifndef APU_H
#define APU_H

#include <cstdint>

class Console;
class CPU;

class Pulse;
class Triangle;
class Noise;
class DMC;

class APU {
public:

public:
    APU(Console *console);
    ~APU();

    void step();
    void sendSample();
    float output();

    void stepFrameCounter();
    void stepTimer();
    void stepEnvelope();
    void stepSweep();
    void stepLength();
    void fireIRQ();

    uint8_t readRegister(uint16_t address);
    void writeRegister(uint16_t address, uint8_t value);

    uint8_t readStatus();

    void writeControl(uint8_t value);
    void writeFrameCounter(uint8_t value);

private:
    Console *console;
    double sampleRate;
    uint64_t cycle;

    Pulse *pulse1;      // $4000-$4003
    Pulse *pulse2;      // $4004-$4007
    Triangle *triangle; // $4008-$400B
    Noise *noise;       // $400C-$400F
    DMC *dmc;           // $4010-$4013

    uint8_t framePeriod; // step mode (4 or 5)
    uint8_t frameValue;
    bool frameIRQ;
};

class Pulse {
public:
    Pulse();
    ~Pulse();

    void writeControl(uint8_t value);
    void writeSweep(uint8_t value);
    void writeTimerLow(uint8_t value);
    void writeTimerHigh(uint8_t value);

    void stepTimer();
    void stepEnvelope();
    void stepSweep();
    void stepLength();

    void sweep();

    uint8_t output();

public:
    bool enable;
    uint8_t channel;

    bool lengthEnable;
    uint8_t lengthValue;

    uint16_t timerPeriod;
    uint16_t timerValue;

    uint8_t dutyMode;
    uint8_t dutyValue;

    bool sweepReload;
    bool sweepEnable;
    bool sweepNegate;
    uint8_t sweepShift;
    uint8_t sweepPeriod;
    uint8_t sweepValue;

    bool envelopeEnable;
    bool envelopeLoop;
    bool envelopeStart;
    uint8_t envelopePeriod;
    uint8_t envelopeValue;
    uint8_t envelopeVolume;

    uint8_t constantVolume;
};

class Triangle {
public:
    Triangle();
    ~Triangle();

    void writeControl(uint8_t value);
    void writeTimerLow(uint8_t value);
    void writeTimerHigh(uint8_t value);

    void stepTimer();
    void stepLength();
    void stepCounter();
    uint8_t output();

public:
    bool enable;
    bool lengthEnable;
    uint8_t lengthValue;

    uint16_t timerPeriod;
    uint16_t timerValue;

    uint8_t dutyValue;

    uint8_t counterPeriod;
    uint8_t counterValue;
    bool counterReload;
};

class Noise {
public:
    Noise();
    ~Noise();

    void writeControl(uint8_t value);
    void writePeriod(uint8_t value);
    void writeLength(uint8_t value);

    void stepTimer();
    void stepEnvelope();
    void stepLength();

    uint8_t output();

public:
    bool enable;
    bool mode;

    uint16_t shiftRegister;

    bool lenghtEnable;
    uint8_t lengthValue;

    uint16_t timerPeriod;
    uint16_t timerValue;

    bool envelopeEnable;
    bool envelopeLoop;
    bool envelopeStart;
    uint8_t envelopePeriod;
    uint8_t envelopeValue;
    uint8_t envelopeVolume;

    uint8_t constantVolume;
};

class DMC {
public:
    DMC(CPU *cpu);
    ~DMC();

    void writeControl(uint8_t value);
    void writeValue(uint8_t value);
    void writeAddress(uint8_t value);
    void writeLength(uint8_t value);

    void restart();

    void stepTimer();
    void stepReader();
    void stepShifter();

    uint8_t output();

public:
    bool enable;
    uint8_t value;

    uint16_t sampleAddress;
    uint16_t sampleLength;

    uint16_t currentAddress;
    uint16_t currentLength;

    uint8_t shiftRegister;
    uint8_t bitCount;

    uint8_t tickPeriod;
    uint8_t tickValue;

    bool loop;
    bool irq;
private:
    CPU *cpu;
};

#endif