#ifndef APU_H
#define APU_H

#include <cstdint>
#include <functional>
#include "nes/AudioBuffer.h"

class Console;
class CPU;

class Pulse;
class Triangle;
class Noise;
class DMC;
class Filter;

class APU {
public:
public:
    APU(Console *console);
    ~APU();

    void step();
    void setSampleRate(uint32_t sampleRate);
    uint8_t readRegister(uint16_t address);
    void writeRegister(uint16_t address, uint8_t value);

    AudioBuffer *getAudioBuffer() const;
    float output();

private:
    void sendSample();

    void stepFrameCounter();
    void stepTimer();
    void stepEnvelope();
    void stepSweep();
    void stepLength();
    void fireIRQ();

    uint8_t readStatus();

    void writeControl(uint8_t value);
    void writeFrameCounter(uint8_t value);

private:
    Console *console;
    AudioBuffer *audio;
    uint32_t sampleRate;
    uint32_t sampleCounter;
    uint64_t cycle;

    Pulse *pulse1;      // $4000-$4003
    Pulse *pulse2;      // $4004-$4007
    Triangle *triangle; // $4008-$400B
    Noise *noise;       // $400C-$400F
    DMC *dmc;           // $4010-$4013

    uint8_t framePeriod; // step mode (4 or 5)
    uint8_t frameCounter;
    bool frameIRQ;

    Filter *filterChain[3];
};

class Pulse {
public:
    Pulse(uint8_t channel);
    ~Pulse();

    void writeControl(uint8_t value);
    void writeSweep(uint8_t value);
    void writeTimerLow(uint8_t value);
    void writeTimerHigh(uint8_t value);

    void stepTimer();
    void stepEnvelope();
    void stepSweep();
    void stepLength();

    uint8_t output();

public:
    bool enabled;
    uint8_t channel;

    uint8_t dutyCycle;
    uint8_t dutyCounter; // Sequence step value

    bool lengthCounterHalt;
    uint8_t lengthCounter;

    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    uint8_t envelopeDividerPeriod;
    uint8_t envelopeDividerCounter;
    uint8_t envelopeDecayCounter;
    uint8_t constantVolume;

    uint16_t timerPeriod;
    uint16_t timerCounter;

    bool sweepEnabled;
    bool sweepReload;
    bool sweepNegate;
    uint8_t sweepShift;
    uint8_t sweepDividerPeriod;
    uint8_t sweepDividerCounter;
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
    void stepLinearCounter();

    uint8_t output();

public:
    bool enabled;
    bool lengthCounterHalt;
    bool control;
    uint8_t lengthCounter;

    uint16_t timerPeriod;
    uint16_t timerCounter;

    uint8_t linearCounterPeriod;
    uint8_t linearCounter;
    bool linearCounterReload;

    uint8_t sequencesStep;
};

class Noise {
public:
    Noise();
    ~Noise();

    void writeControl(uint8_t value);
    void writePeriod(uint8_t value);
    void writeLength(uint8_t value);

    void stepTimer();
    void stepShiftRegister();
    void stepEnvelope();
    void stepLength();

    uint8_t output();

public:
    bool enabled;
    bool mode;

    uint16_t shiftRegister;

    bool lenghtCounterHalt;
    uint8_t lengthCounter;

    uint16_t timerPeriod;
    uint16_t timerCounter;

    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    uint8_t envelopeDividerPeriod;
    uint8_t envelopeDividerCounter;
    uint8_t envelopeDecayCounter;
    uint8_t constantVolume;
    uint8_t envelopeOutputVolume;
};

class DMC {
public:
    DMC(CPU *cpu);
    ~DMC();

    void writeControl(uint8_t value);
    void writeDirectLoad(uint8_t value);
    void writeAddress(uint8_t value);
    void writeLength(uint8_t value);

    void restart();

    void stepTimer();
    void stepReader();
    void stepShifter();

public:
    bool enabled;
    bool loop;
    bool irqEnable;
    bool interrupt;
    uint8_t rateIndex;
    uint8_t frequency;

    uint16_t sampleAddress;
    uint16_t sampleLength;

    uint8_t sampleBuffer;
    uint8_t sampleBufferBitCount;

    // reader unit
    uint16_t addressCounter;
    uint16_t bytesRemainingCounter;

    // timer unit
    uint8_t timerPeriod;
    uint8_t timerCounter;

    // output unit
    uint8_t shiftRegister;
    uint8_t bitsRemainingCounter;
    uint8_t outputLevel;
    bool silence;
    bool outputCycleEnd;

private:
    CPU *cpu;
};

#endif