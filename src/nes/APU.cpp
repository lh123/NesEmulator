#include "nes/APU.h"
#include "nes/Console.h"

#include <cstdio>

static constexpr double FRAME_COUNTER_RATE = CPU::CPU_FREQUENCY / double(240.0);

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
    : console(console), sampleRate(0), cycle(0), framePeriod(0), frameValue(0), frameIRQ(false) {
    pulse1 = new Pulse();
    pulse2 = new Pulse();
    triangle = new Triangle();
    noise = new Noise();
    dmc = new DMC(console->cpu);

    initTable();
}

APU::~APU() {
    delete pulse1;
    delete pulse2;
    delete triangle;
    delete noise;
    delete dmc;
}

void APU::step() {
    uint64_t cycle1 = cycle;
    cycle++;
    uint64_t cycle2 = cycle;
    stepTimer();
    int f1 = cycle1 / FRAME_COUNTER_RATE;
    int f2 = cycle2 / FRAME_COUNTER_RATE;
    if (f1 != f2) {
        stepFrameCounter();
    }

    int s1 = cycle1 / sampleRate;
    int s2 = cycle2 / sampleRate;
    if (s1 != s2) {
        sendSample();
    }
}

void APU::sendSample() {
    // TODO
}

float APU::output() {
    uint8_t p1 = pulse1->output();
    uint8_t p2 = pulse2->output();
    uint8_t t = triangle->output();
    uint8_t n = noise->output();
    uint8_t d = dmc->output();

    float pulseOut = pulseTable[p1 + p2];
    float tndOut = tndTable[3 * t + 2 * n + d];
    return pulseOut + tndOut;
}

void APU::stepFrameCounter() {
    switch (framePeriod) {
    case 4:
        frameValue = (frameValue + 1) % 4;
        switch (frameValue) {
        case 0:
        case 2:
            stepEnvelope();
            break;
        case 1:
            stepEnvelope();
            stepSweep();
            stepLength();
            break;
        case 3:
            stepEnvelope();
            stepSweep();
            stepLength();
            fireIRQ();
            break;
        }
        break;
    case 5:
        frameValue = (frameValue + 1) % 5;
        switch (frameValue) {
        case 1:
        case 3:
            stepEnvelope();
            break;
        case 0:
        case 2:
            stepEnvelope();
            stepSweep();
            stepLength();
            break;
        }
        break;
    }
}

void APU::stepTimer() {
    if (cycle % 2 == 0) {
        pulse1->stepTimer();
        pulse2->stepTimer();
        noise->stepTimer();
        dmc->stepTimer();
    }
    triangle->stepTimer();
}

void APU::stepEnvelope() {
    pulse1->stepEnvelope();
    pulse2->stepEnvelope();
    triangle->stepCounter();
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
        dmc->writeValue(value);
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

uint8_t APU::readStatus() {
    uint8_t result = 0;
    if (pulse1->lengthValue > 0) {
        result |= 0x01;
    }
    if (pulse2->lengthValue > 0) {
        result |= 0x02;
    }
    if (triangle->lengthValue > 0) {
        result |= 0x04;
    }
    if (noise->lengthValue > 0) {
        result |= 0x08;
    }
    if (dmc->currentLength > 0) {
        result |= 0x10;
    }
    return result;
}

void APU::writeControl(uint8_t value) {
    pulse1->enable = (value & 0x01) == 0x01;
    pulse2->enable = (value & 0x02) == 0x02;
    triangle->enable = (value & 0x04) == 0x04;
    noise->enable = (value & 0x08) == 0x08;
    dmc->enable = (value & 0x10) == 0x10;

    if (!pulse1->enable) {
        pulse1->lengthValue = 0;
    }
    if (!pulse2->enable) {
        pulse2->lengthValue = 0;
    }
    if (!triangle->enable) {
        triangle->lengthValue = 0;
    }
    if (!noise->enable) {
        noise->lengthValue = 0;
    }
    if (!dmc->enable) {
        dmc->currentLength = 0;
    } else {
        if (dmc->currentLength == 0) {
            dmc->restart();
        }
    }
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

Pulse::Pulse()
    : enable(false), channel(0), lengthEnable(false), lengthValue(0), timerPeriod(0), timerValue(0),
      dutyMode(0), dutyValue(0), sweepReload(false), sweepEnable(false), sweepNegate(false),
      sweepShift(0), sweepPeriod(0), sweepValue(0), envelopeEnable(false), envelopeLoop(false),
      envelopeStart(false), envelopePeriod(0), envelopeValue(0), envelopeVolume(0),
      constantVolume(0) {}

Pulse::~Pulse() {}

void Pulse::writeControl(uint8_t value) {
    dutyMode = (value >> 6) & 0x3;
    lengthEnable = ((value >> 5) & 0x1) == 0x0;
    envelopeLoop = ((value >> 5) & 0x1) == 0x1;
    envelopeEnable = ((value >> 4) & 0x1) == 0x0;
    envelopePeriod = value & 0x0F;
    constantVolume = value & 0x0F;
    envelopeStart = true;
}

void Pulse::writeSweep(uint8_t value) {
    sweepEnable = ((value >> 7) & 0x1) == 0x1;
    sweepPeriod = ((value >> 4) & 0x7) + 1;
    sweepNegate = ((value >> 3) & 0x1) == 0x1;
    sweepShift = value & 0x7;
    sweepReload = true;
}

void Pulse::writeTimerLow(uint8_t value) { timerPeriod = (timerPeriod & 0xFF00) | value; }

void Pulse::writeTimerHigh(uint8_t value) {
    timerValue = lengthTable[value >> 3];
    timerPeriod = (timerPeriod & 0x00FF) | (uint16_t(value & 0x7) << 8);
    envelopeStart = true;
    dutyValue = 0;
}

void Pulse::stepTimer() {
    if (timerValue == 0) {
        timerValue = timerPeriod;
        dutyValue = (dutyValue + 1) % 8;
    } else {
        timerValue--;
    }
}

void Pulse::stepEnvelope() {
    if (envelopeStart) {
        envelopeVolume = 15;
        envelopeValue = envelopePeriod;
        envelopeStart = false;
    } else if (envelopeValue > 0) {
        envelopeValue--;
    } else {
        if (envelopeVolume > 0) {
            envelopeVolume--;
        } else if (envelopeLoop) {
            envelopeVolume = 15;
        }
        envelopeValue = envelopePeriod;
    }
}

void Pulse::stepSweep() {
    if (sweepReload) {
        if (sweepEnable && sweepValue == 0) {
            sweep();
        }
        sweepValue = sweepPeriod;
        sweepReload = false;
    } else if (sweepValue > 0) {
        sweepValue--;
    } else {
        if (sweepEnable) {
            sweep();
        }
        sweepValue = sweepPeriod;
    }
}

void Pulse::stepLength() {
    if (lengthEnable && lengthValue > 0) {
        lengthValue--;
    }
}

void Pulse::sweep() {
    uint16_t delta = timerPeriod >> sweepShift;
    if (sweepNegate) {
        timerPeriod -= delta;
        if (channel == 1) {
            timerPeriod--;
        }
    } else {
        timerPeriod += delta;
    }
}

uint8_t Pulse::output() {
    if (!enable) {
        return 0;
    }
    if (lengthValue == 0) {
        return 0;
    }
    if (dutyTable[dutyMode][dutyValue] == 0) {
        return 0;
    }
    if (timerPeriod < 8 || timerPeriod > 0x7FF) {
        return 0;
    }
    if (envelopeEnable) {
        return envelopeVolume;
    } else {
        return constantVolume;
    }
}

Triangle::Triangle()
    : enable(false), lengthEnable(false), lengthValue(0), timerPeriod(0), timerValue(0),
      dutyValue(0), counterPeriod(0), counterValue(0), counterReload(false) {}

Triangle::~Triangle() {}

void Triangle::writeControl(uint8_t value) {
    lengthEnable = ((value >> 7) & 0x1) == 0;
    counterPeriod = value & 0x7F;
}

void Triangle::writeTimerLow(uint8_t value) { timerPeriod = (timerPeriod & 0xFF00) | value; }

void Triangle::writeTimerHigh(uint8_t value) {
    lengthValue = lengthTable[value >> 3];
    timerPeriod = (timerPeriod & 0x00FF) | (uint16_t(value & 0x7) << 8);
    timerValue = timerPeriod;
    counterReload = true;
}

void Triangle::stepTimer() {
    if (timerValue == 0) {
        timerValue = timerPeriod;
        if (lengthValue > 0 && counterValue > 0) {
            dutyValue = (dutyValue + 1) % 32;
        }
    } else {
        timerValue--;
    }
}

void Triangle::stepLength() {
    if (lengthEnable && lengthValue > 0) {
        lengthValue--;
    }
}

void Triangle::stepCounter() {
    if (counterReload) {
        counterValue = counterPeriod;
    } else if (counterValue > 0) {
        counterValue--;
    }
    if (lengthEnable) {
        counterReload = false;
    }
}

uint8_t Triangle::output() {
    if (!enable) {
        return 0;
    }
    if (lengthValue == 0) {
        return 0;
    }
    if (counterValue == 0) {
        return 0;
    }
    return triangleTable[dutyValue];
}

Noise::Noise()
    : enable(false), mode(false), shiftRegister(0), lenghtEnable(false), lengthValue(0),
      timerPeriod(0), timerValue(0), envelopeEnable(false), envelopeLoop(false),
      envelopeStart(false), envelopePeriod(0), envelopeValue(0), envelopeVolume(0),
      constantVolume(0) {}

Noise::~Noise() {}

void Noise::writeControl(uint8_t value) {
    lenghtEnable = ((value >> 5) & 0x1) == 0;
    envelopeLoop = ((value >> 5) & 0x1) == 0x1;
    envelopeEnable = ((value >> 4) & 0x1) == 0;
    envelopePeriod = value & 0x0F;
    constantVolume = value & 0x0F;
    envelopeStart = true;
}

void Noise::writePeriod(uint8_t value) {
    mode = (value & 0x80) == 0x80;
    timerPeriod = noiseTable[value & 0x0F];
}

void Noise::writeLength(uint8_t value) {
    lengthValue = lengthTable[value >> 3];
    envelopeStart = true;
}

void Noise::stepTimer() {
    if (timerValue == 0) {
        timerValue = timerPeriod;
        uint8_t shift = 0;
        if (mode) {
            shift = 6;
        } else {
            shift = 1;
        }
        uint8_t b1 = shiftRegister & 0x1;
        uint8_t b2 = (shiftRegister >> shift) & 0x1;
        shiftRegister >>= 1;
        shiftRegister |= (b1 ^ b2) << 14;
    } else {
        timerValue--;
    }
}

void Noise::stepEnvelope() {
    if (envelopeStart) {
        envelopeVolume = 15;
        envelopeValue = envelopePeriod;
        envelopeStart = false;
    } else if (envelopeValue > 0) {
        envelopeValue--;
    } else {
        if (envelopeVolume > 0) {
            envelopeVolume--;
        } else if (envelopeLoop) {
            envelopeVolume = 15;
        }
        envelopeValue = envelopePeriod;
    }
}

void Noise::stepLength() {
    if (lenghtEnable && lengthValue > 0) {
        lengthValue--;
    }
}

uint8_t Noise::output() {
    if (!enable) {
        return 0;
    }
    if (lengthValue == 0) {
        return 0;
    }
    if ((shiftRegister & 0x1) == 0x1) {
        return 0;
    }
    if (envelopeEnable) {
        return envelopeVolume;
    } else {
        return constantVolume;
    }
}

DMC::DMC(CPU *cpu)
    : enable(false), value(0), sampleAddress(0), sampleLength(0), currentAddress(0),
      currentLength(0), shiftRegister(0), bitCount(0), tickPeriod(0), tickValue(0), loop(false),
      irq(false), cpu(cpu) {}

DMC::~DMC() {}

void DMC::writeControl(uint8_t value) {
    irq = (value & 0x80) == 0x80;
    loop = (value & 0x40) == 0x40;
    tickPeriod = dmcTable[value & 0x0F];
}

void DMC::writeValue(uint8_t value) { this->value = value & 0x7F; }

void DMC::writeAddress(uint8_t value) { sampleAddress = 0xC000 | (uint16_t(value) << 6); }

void DMC::writeLength(uint8_t value) { sampleLength = (uint16_t(value) << 4) | 0x1; }

void DMC::restart() {
    currentAddress = sampleAddress;
    currentLength = sampleLength;
}

void DMC::stepTimer() {
    if (!enable) {
        return;
    }
    stepReader();
    if (tickValue == 0) {
        tickValue = tickPeriod;
        stepShifter();
    } else {
        tickValue--;
    }
}

void DMC::stepReader() {
    if (currentLength > 0 && bitCount == 0) {
        cpu->stall += 4;
        shiftRegister = cpu->read(currentAddress);
        bitCount = 8;
        currentAddress++;
        if (currentAddress == 0) {
            currentAddress = 0x8000;
        }
        currentLength--;
        if (currentLength == 0 && loop) {
            restart();
        }
    }
}

void DMC::stepShifter() {
    if (bitCount == 0) {
        return;
    }
    if ((shiftRegister & 0x1) == 0x1) {
        if (value <= 125) {
            value += 2;
        }
    } else {
        if (value >= 2) {
            value -= 2;
        }
    }
    shiftRegister >>= 1;
    bitCount--;
}

uint8_t DMC::output() { return value; }
