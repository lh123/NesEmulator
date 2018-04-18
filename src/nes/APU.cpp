#include "nes/APU.h"
#include "nes/Console.h"
#include "nes/CPU.h"
#include "nes/Filter.h"
#include <cstdio>

static const uint8_t lengthTable[32] = {
    10, 254, 20, 2,  40, 4,  80, 6,  160, 8,  60, 10, 14, 12, 26, 14, //
    12, 16,  24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30  //
};

static const uint8_t dutyTable[4][8] = {
    {0, 1, 0, 0, 0, 0, 0, 0}, //
    {0, 1, 1, 0, 0, 0, 0, 0}, //
    {0, 1, 1, 1, 1, 0, 0, 0}, //
    {1, 0, 0, 1, 1, 1, 1, 1}  //
};

static const uint8_t triangleTable[] = {
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,  4,  3,  2,  1,  0, //
    0,  1,  2,  3,  4,  5,  6, 7, 8, 9, 10, 11, 12, 13, 14, 15 //
};

static const uint16_t noiseTable[] = {
    4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068 //
};

static const uint8_t dmcTable[] = {
    214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27 //
};

static float pulseTable[31];
static float tndTable[203];

static void initTable() {
    for (int i = 0; i < 31; i++) {
        pulseTable[i] = 95.52 / (8128.0 / i + 100);
    }
    for (int i = 0; i < 203; i++) {
        tndTable[i] = 163.67 / (24329.0 / i + 100);
    }
}

APU::APU(Console *console)
    : console(console), sampleRate(0), sampleCounter(0), cycle(0), framePeriod(0), frameCounter(0),
      frameIRQ(false), filterChain{new Filter(Filter::Type::HighPass),
                                   new Filter(Filter::Type::HighPass),
                                   new Filter(Filter::Type::LowPass)} {
    initTable();
    pulse1 = new Pulse(1);
    pulse2 = new Pulse(2);
    triangle = new Triangle();
    noise = new Noise();
    dmc = new DMC(console->cpu);

    audio = new AudioBuffer(8192);
}

APU::~APU() {
    delete pulse1;
    delete pulse2;
    delete triangle;
    delete noise;
    delete dmc;

    for (int i = 0; i < 3; i++) {
        delete filterChain[i];
    }
}

// call peer cpu cycle
void APU::step() {
    cycle++;
    stepTimer();
    if (cycle == 7457) {
        // 3728.5
        stepEnvelope();
    } else if (cycle == 14913) {
        // 7456.5
        stepEnvelope();
        stepLength();
        stepSweep();
    } else if (cycle == 22371) {
        // 1118.5
        stepEnvelope();
    } else if (cycle == 29828) {
        // 14914
    } else if (cycle == 29829) {
        // 14914.5
        if (framePeriod == 4) {
            stepEnvelope();
            stepLength();
            stepSweep();
        }
    } else if (cycle == 29830) {
        // 14915
        if (framePeriod == 4) {
            cycle = 0;
            fireIRQ();
        }
    } else if (cycle == 37281) {
        // 18640.5
        if (framePeriod == 5) {
            stepEnvelope();
            stepLength();
            stepSweep();
        }
    } else if (cycle == 37282) {
        // 18641
        if (framePeriod == 5) {
            cycle = 0;
        }
    }
    sampleCounter++;
    if (sampleCounter == CPU::CPU_FREQUENCY / sampleRate) {
        sendSample();
        sampleCounter = 0;
    }
}

void APU::setSampleRate(uint32_t value) {
    sampleRate = value;
    filterChain[0]->setParamter(value, 90);
    filterChain[1]->setParamter(value, 440);
    filterChain[2]->setParamter(value, 14000);
}

void APU::sendSample() {
    float x = output();
    for (int i = 0; i < 3; i++) {
        x = filterChain[i]->step(x);
    }
    audio->push(x);
}

AudioBuffer *APU::getAudioBuffer() const { return audio; }

float APU::output() {
    uint8_t p1 = pulse1->output();
    uint8_t p2 = pulse2->output();
    uint8_t t = triangle->output();
    uint8_t n = noise->output();
    uint8_t d = dmc->outputLevel;

    float pulseOut = pulseTable[p1 + p2];
    float tndOut = tndTable[3 * t + 2 * n + d];
    return pulseOut + tndOut;
}

void APU::stepTimer() {
    if (cycle % 2 == 0) {
        pulse1->stepTimer();
        pulse2->stepTimer();
        noise->stepTimer();
        dmc->stepReader();
        dmc->stepTimer();
    }
    triangle->stepTimer();
}

void APU::stepEnvelope() {
    pulse1->stepEnvelope();
    pulse2->stepEnvelope();
    triangle->stepLinearCounter();
    noise->stepEnvelope();
}

void APU::stepSweep() {
    pulse1->stepSweep();
    pulse2->stepSweep();
}

void APU::stepLength() {
    pulse1->stepLength();
    pulse2->stepLength();
    triangle->stepLength();
    noise->stepLength();
}

void APU::fireIRQ() {
    if (frameIRQ) {
        console->cpu->triggerIRQ();
    }
}

uint8_t APU::readRegister(uint16_t address) {
    if (address == 0x4015) {
        return readStatus();
    } else {
        std::printf("error: apu register read at address: 0x%04X\n", address);
        return 0;
    }
}

void APU::writeRegister(uint16_t address, uint8_t value) {
    switch (address) {
    case 0x4000:
        pulse1->writeControl(value);
        break;
    case 0x4001:
        pulse1->writeSweep(value);
        break;
    case 0x4002:
        pulse1->writeTimerLow(value);
        break;
    case 0x4003:
        pulse1->writeTimerHigh(value);
        break;
    case 0x4004:
        pulse2->writeControl(value);
        break;
    case 0x4005:
        pulse2->writeSweep(value);
        break;
    case 0x4006:
        pulse2->writeTimerLow(value);
        break;
    case 0x4007:
        pulse2->writeTimerHigh(value);
        break;
    case 0x4008:
        triangle->writeControl(value);
        break;
    case 0x4009:
        // unused
        break;
    case 0x400A:
        triangle->writeTimerLow(value);
        break;
    case 0x400B:
        triangle->writeTimerHigh(value);
        break;
    case 0x400C:
        noise->writeControl(value);
        break;
    case 0x400D:
        // unused
        break;
    case 0x400E:
        noise->writePeriod(value);
        break;
    case 0x400F:
        noise->writeLength(value);
        break;
    case 0x4010:
        dmc->writeControl(value);
        break;
    case 0x4011:
        dmc->writeDirectLoad(value);
        break;
    case 0x4012:
        dmc->writeAddress(value);
        break;
    case 0x4013:
        dmc->writeLength(value);
        break;
    case 0x4015:
        writeControl(value);
        break;
    case 0x4017:
        writeFrameCounter(value);
        break;
    default:
        std::printf("error: apu register write at address: 0x%04X\n", address);
        break;
    }
}

/**
 * $4015(read)
 * IF-D NT21
 */
uint8_t APU::readStatus() {
    uint8_t result = 0;
    if (pulse1->lengthCounter > 0) {
        result |= 0x1;
    }
    if (pulse2->lengthCounter > 0) {
        result |= 0x2;
    }
    if (triangle->lengthCounter > 0) {
        result |= 0x4;
    }
    if (noise->lengthCounter > 0) {
        result |= 0x8;
    }
    if (dmc->bytesRemainingCounter > 0) {
        result |= 0x10;
    }
    if (frameIRQ) {
        frameIRQ = false;
        result |= 0x40;
    }
    if (dmc->interrupt) {
        result |= 0x80;
    }
    return result;
}

/**
 * $4015(write)
 * ---D NT21
 *    D         Enable DMC
 *      N       Enable Noise
 *       T      Enable Traingle
 *        21    Enable Pulse 1/2
 */
void APU::writeControl(uint8_t value) {
    pulse1->enabled = (value & 0x01) == 0x01;
    pulse2->enabled = (value & 0x02) == 0x02;
    triangle->enabled = (value & 0x04) == 0x04;
    noise->enabled = (value & 0x08) == 0x08;
    dmc->enabled = (value & 0x10) == 0x10;

    if (!pulse1->enabled) {
        pulse1->lengthCounter = 0;
    }
    if (!pulse2->enabled) {
        pulse2->lengthCounter = 0;
    }
    if (!triangle->enabled) {
        triangle->lengthCounter = 0;
    }
    if (!noise->enabled) {
        noise->lengthCounter = 0;
    }
    if (!dmc->enabled) {
        dmc->bytesRemainingCounter = 0;
    } else {
        if (dmc->bytesRemainingCounter == 0) {
            dmc->restart();
        }
    }
    dmc->interrupt = false;
}

void APU::writeFrameCounter(uint8_t value) {
    framePeriod = 4 + ((value >> 7) & 0x1);
    frameIRQ = ((value >> 6) & 0x1) == 0;
    // 5-step mode has extra step
    if (framePeriod == 5) {
        stepEnvelope();
        stepSweep();
        stepLength();
    }
}

Pulse::Pulse(uint8_t channel)
    : enabled(false), channel(channel), dutyCycle(0), dutyCounter(0), lengthCounterHalt(false),
      lengthCounter(0), envelopeEnabled(false), envelopeLoop(false), envelopeStart(false),
      envelopeDividerPeriod(0), envelopeDividerCounter(0), envelopeDecayCounter(0),
      constantVolume(0), timerPeriod(0), timerCounter(0), sweepEnabled(false), sweepReload(0),
      sweepNegate(0), sweepShift(0), sweepDividerPeriod(0), sweepDividerCounter(0) {}

Pulse::~Pulse() {}

/**
 * $4000/$4004
 * ddLC VVVV
 * dd           dutyCycle
 *   L          length counter halt/envelope loop flag
 *    C         constant volume/envelope flag
 *      VVVV    volume in constant volume (C set) mode/reload value for the envelope's divider
 *
 * The envelope is also restarted. The period divider is not reset.
 */
void Pulse::writeControl(uint8_t value) {
    dutyCycle = (value >> 6) & 0x3;
    lengthCounterHalt = ((value >> 5) & 0x1) == 0x1;
    envelopeLoop = ((value >> 5) & 0x1) == 0x1;
    envelopeEnabled = ((value >> 4) & 0x1) == 0;
    constantVolume = value & 0xF;
    envelopeDividerPeriod = value & 0xF;
    envelopeStart = true;
}

/**
 * $4001/$4005
 * EPPP NSSS
 * E            enabled flag
 *  PPP         divider period is (P + 1)
 *      N       negate flag
 *       SSS    shift count
 *
 * Sets the reload flag
 */
void Pulse::writeSweep(uint8_t value) {
    sweepEnabled = ((value >> 7) & 0x1) == 0x1;
    sweepDividerPeriod = ((value >> 4) & 0x7) + 1;
    sweepNegate = ((value >> 3) & 0x1) == 0x1;
    sweepShift = value & 0x7;
    sweepReload = true;
}

/**
 * $4002/$4006
 * 	LLLL LLLL   timer Low 8 bits
 */
void Pulse::writeTimerLow(uint8_t value) { timerPeriod = (timerPeriod & 0xFF00) | value; }

/**
 * $4003/$4007
 * 	llll lHHH
 *  llll l      length counter period
 *        HHH   timer high 3 bits
 * The sequencer is immediately restarted at the first value of the current sequence.
 * The envelope is restarted. The period divider is not reset.
 */
void Pulse::writeTimerHigh(uint8_t value) {
    if (enabled) {
        lengthCounter = lengthTable[value >> 3];
        timerPeriod = (timerPeriod & 0x00FF) | (uint16_t(value & 0x7) << 8);
        dutyCounter = 0;
        envelopeStart = true;
    }
}

// this timer is updated every APU cycle(every second CPU cycle)
void Pulse::stepTimer() {
    if (timerCounter == 0) {
        timerCounter = timerPeriod;
        // stepSequences();
        dutyCounter = (dutyCounter + 1) % 8;
    } else {
        timerCounter--;
    }
}

// clocked by the frame counter
void Pulse::stepEnvelope() {
    if (envelopeStart) {
        envelopeStart = false;
        envelopeDividerCounter = envelopeDividerPeriod;
        envelopeDecayCounter = 15;
    } else {
        // clock divider
        if (envelopeDividerCounter > 0) {
            envelopeDividerCounter--;
        } else {
            envelopeDividerCounter = envelopeDividerPeriod;
            // clock decay level counter.
            if (envelopeDecayCounter > 0) {
                envelopeDecayCounter--;
            } else {
                if (envelopeLoop) {
                    envelopeDecayCounter = 15;
                }
            }
        }
    }
}

// frame counter sends a half-frame clock
void Pulse::stepSweep() {
    if (sweepDividerCounter > 0) {
        sweepDividerCounter--;
    } else {
        if (sweepEnabled) {
            // adjusted
            uint8_t changeAmount = timerPeriod >> sweepShift;
            if (sweepNegate) {
                timerPeriod -= changeAmount;
                if (channel == 1) {
                    timerPeriod--;
                }
            } else {
                timerPeriod += changeAmount;
            }
        }
        sweepDividerCounter = sweepDividerPeriod;
        sweepReload = false;
    }
    if (sweepReload) {
        sweepDividerCounter = sweepDividerPeriod;
        sweepReload = false;
    }
}

// clocked by the frame counter
void Pulse::stepLength() {
    if (enabled && !lengthCounterHalt && lengthCounter > 0) {
        lengthCounter--;
    }
}

uint8_t Pulse::output() {
    if (!enabled) {
        return 0;
    }
    if (lengthCounter == 0) {
        return 0;
    }
    if (dutyTable[dutyCycle][dutyCounter] == 0) {
        return 0;
    }
    if (timerPeriod < 8 || timerPeriod > 0x7FF) {
        return 0;
    }
    if (envelopeEnabled) {
        return envelopeDecayCounter;
    } else {
        return constantVolume;
    }
}

Triangle::Triangle()
    : enabled(false), lengthCounterHalt(false), control(false), lengthCounter(0), timerPeriod(0),
      timerCounter(0), linearCounterPeriod(0), linearCounter(0), linearCounterReload(false),
      sequencesStep(0) {}

Triangle::~Triangle() {}

/**
 * $4008
 * CRRR RRRR
 * C            Control flag (this bit is also the length counter halt flag)
 *  RRR RRRR    Counter reload value
 */
void Triangle::writeControl(uint8_t value) {
    lengthCounterHalt = ((value >> 7) & 0x1) == 0x1;
    control = ((value >> 7) & 0x1) == 0x1;
    linearCounterPeriod = value & 0x7F;
}

/**
 * $400A
 * LLLL LLLL Timer low (write)
 */
void Triangle::writeTimerLow(uint8_t value) { timerPeriod = (timerPeriod & 0xFF00) | value; }

/**
 * $400B
 * llll lHHH
 * llll l       Linear counter load
 *       HHH    Timer high 3 bits
 * Sets the linear counter reload flag
 */
void Triangle::writeTimerHigh(uint8_t value) {
    if (enabled) {
        lengthCounter = lengthTable[value >> 3];
        timerPeriod = (timerPeriod & 0x00FF) | (uint16_t(value & 0x7) << 8);
        timerCounter = timerPeriod;
        linearCounterReload = true;
    }
}

// clock every cpu cycle
void Triangle::stepTimer() {
    if (timerCounter == 0) {
        timerCounter = timerPeriod;
        // sequencer is clocked by the timer as long as both the linear counter and the length
        // counter are nonzero
        if (lengthCounter > 0 && linearCounter > 0) {
            sequencesStep = (sequencesStep + 1) % 32;
        }
    } else {
        timerCounter--;
    }
}

void Triangle::stepLength() {
    if (enabled && !lengthCounterHalt && lengthCounter > 0) {
        lengthCounter--;
    }
}

// clock by frame counter
void Triangle::stepLinearCounter() {
    if (linearCounterReload) {
        linearCounter = linearCounterPeriod;
    } else {
        if (linearCounter > 0) {
            linearCounter--;
        }
    }
    // If the control flag is clear, the linear counter reload flag is cleared.
    if (!control) {
        linearCounterReload = false;
    }
}

uint8_t Triangle::output() {
    if (!enabled) {
        return 0;
    }
    if (lengthCounter == 0) {
        return 0;
    }
    if (linearCounter == 0) {
        return 0;
    }
    return triangleTable[sequencesStep];
}

Noise::Noise()
    : enabled(false), mode(false), shiftRegister(0), lenghtCounterHalt(false), lengthCounter(0),
      timerPeriod(0), timerCounter(0), envelopeEnabled(false), envelopeLoop(false),
      envelopeStart(false), envelopeDividerPeriod(0), envelopeDividerCounter(0),
      envelopeDecayCounter(0), constantVolume(0), envelopeOutputVolume(0) {}

Noise::~Noise() {}

/**
 * $400C
 * --lc.vvvv
 *   l          Length Counter halt flag/Envelope loop flag
 *    c         Constant volume/Envelope flag (0: use volume from envelope; 1: use constant volume)
 *      vvvv    Constant volume/Envelope divider period (write)
 */
void Noise::writeControl(uint8_t value) {
    lenghtCounterHalt = ((value >> 5) & 0x1) == 0x1;
    envelopeLoop = ((value >> 5) & 0x1) == 0x1;
    envelopeEnabled = ((value >> 4) & 0x1) == 0;
    envelopeDividerPeriod = value & 0x0F;
    constantVolume = value & 0x0F;
}

/**
 * $400E
 * M--- PPPP
 * M            Mode flag
 *      PPPP    Timer period
 */
void Noise::writePeriod(uint8_t value) {
    mode = (value & 0x80) == 0x80;
    timerPeriod = noiseTable[value & 0x0F];
}

/**
 * $400F
 * llll l---    Length counter load
 * Envelope restart
 */
void Noise::writeLength(uint8_t value) {
    if (enabled) {
        lengthCounter = lengthTable[value >> 3];
    }
    envelopeStart = true;
}

void Noise::stepTimer() {
    if (timerCounter == 0) {
        timerCounter = timerPeriod;
        // clocks the shift register
        stepShiftRegister();
    } else {
        timerCounter--;
    }
}

// clocked by timer
void Noise::stepShiftRegister() {
    // bit 6 if Mode flag is set, otherwise bit 1.
    uint8_t bit = 0;
    if (mode) {
        bit = 6;
    } else {
        bit = 1;
    }
    uint8_t b1 = shiftRegister & 0x1;
    uint8_t b2 = (shiftRegister >> bit) & 0x1;
    shiftRegister >>= 1;
    // Bit 14, the leftmost bit, is set to the feedback calculated earlier.
    shiftRegister |= (b1 ^ b2) << 14;
}

void Noise::stepEnvelope() {
    if (envelopeStart) {
        envelopeStart = false;
        envelopeDividerCounter = envelopeDividerPeriod;
        envelopeDecayCounter = 15;
    } else {
        // clock divider
        if (envelopeDividerCounter > 0) {
            envelopeDividerCounter--;
        } else {
            envelopeDividerCounter = envelopeDividerPeriod;
            // clock decay level counter.
            if (envelopeDecayCounter > 0) {
                envelopeDecayCounter--;
            } else {
                if (envelopeLoop) {
                    envelopeDecayCounter = 15;
                }
            }
        }
    }
}

void Noise::stepLength() {
    if (enabled && !lenghtCounterHalt && lengthCounter > 0) {
        lengthCounter--;
    }
}

// Bit 0 of the shift register is set, or
// The length counter is zero
uint8_t Noise::output() {
    if (!enabled) {
        return 0;
    }
    if ((shiftRegister & 0x1) == 0x1) {
        return 0;
    }
    if (lengthCounter == 0) {
        return 0;
    }
    if (envelopeEnabled) {
        return envelopeDecayCounter;
    } else {
        return constantVolume;
    }
}

DMC::DMC(CPU *cpu)
    : enabled(false), loop(false), irqEnable(false), interrupt(false), rateIndex(0), frequency(0),
      sampleAddress(0), sampleLength(0), sampleBuffer(0), sampleBufferBitCount(0),
      addressCounter(0), bytesRemainingCounter(0), timerPeriod(0), timerCounter(0),
      shiftRegister(0), bitsRemainingCounter(0), outputLevel(0), silence(false),
      outputCycleEnd(false), cpu(cpu) {}

DMC::~DMC() {}

/**
 * $4010
 * IL-- RRRR
 * I            IRQ enabled flag. If clear, the interrupt flag is cleared.
 *  L           Loop flag
 *      RRRR    Rate index
 */
void DMC::writeControl(uint8_t value) {
    irqEnable = (value & 0x80) == 0x80;
    if (!irqEnable) {
        interrupt = false;
    }
    loop = (value & 0x40) == 0x40;
    rateIndex = value & 0x0F;
    frequency = dmcTable[rateIndex];
}

/**
 * $4011
 * -DDD DDDD
 * The DMC output level is set to D, an unsigned value
 */
void DMC::writeDirectLoad(uint8_t value) { outputLevel = value & 0x7F; }

void DMC::writeAddress(uint8_t value) { sampleAddress = 0xC000 | (uint16_t(value) << 6); }

void DMC::writeLength(uint8_t value) { sampleLength = (uint16_t(value) << 4) | 0x1; }

void DMC::restart() {
    addressCounter = sampleAddress;
    bytesRemainingCounter = sampleLength;
}

// clock every second cpu cycle
void DMC::stepTimer() {
    if (outputCycleEnd) {
        outputCycleEnd = false;
        bitsRemainingCounter = 8;
        if (sampleBufferBitCount == 0) {
            silence = true;
        } else {
            silence = false;
            shiftRegister = sampleBuffer;
            sampleBufferBitCount = 0;
        }
    }
    if (timerCounter == 0) {
        timerCounter = timerPeriod;
        if (!silence) {
            int delta = 0;
            if ((shiftRegister & 0x1) == 0x1) {
                delta = 2;
            } else {
                delta = -2;
            }
            if (outputLevel + delta <= 127 && outputLevel + delta >= 0) {
                outputLevel += delta;
            }
            shiftRegister >>= 1;
            bitsRemainingCounter--;
            if (bitsRemainingCounter == 0) {
                outputCycleEnd = true;
            }
        }
    } else {
        timerCounter--;
    }
}

// clock every cycle
void DMC::stepReader() {
    if (sampleBufferBitCount == 0 && bytesRemainingCounter > 0) {
        cpu->stall += 4;
        sampleBuffer = cpu->read(addressCounter);
        addressCounter++;
        if (addressCounter == 0) {
            addressCounter = 0x8000;
        }
        bytesRemainingCounter--;
        if (bytesRemainingCounter == 0 && loop) {
            restart();
        } else {
            if (irqEnable) {
                interrupt = true;
            }
        }
    }
    if (interrupt) {
        cpu->triggerIRQ();
    }
}