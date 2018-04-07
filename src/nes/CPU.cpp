#include "nes/CPU.h"
#include <cstdio>
#include <cstring>

#include "nes/Console.h"

CPU::CPU(Console *console)
    : console(console), cycles(0), PC(0), SP(0), A(0), X(0), Y(0), C(0), Z(0),
      I(0), D(0), B(0), V(0), N(0) {}

CPU::~CPU() {}

uint8_t CPU::read(uint16_t address) {
    if (address < 0x2000) {
        return console->ram[address % 0x0800];
    } else if (address >= 0x6000) {
        return console->cartridge->read(address);
    }
    return 0;
}

uint16_t CPU::read16(uint16_t address) {
    uint16_t lo = uint16_t(read(address));
    uint16_t hi = uint16_t(read(address + 1));
    return (hi << 8) | lo;
}

uint16_t CPU::read16bug(uint16_t address) {
    uint16_t a = address;
    uint16_t b = (a & 0xFF00) | ((a + 1) & 0xFF);
    uint16_t lo = uint16_t(read(a));
    uint16_t hi = uint16_t(read(b));
    return (hi << 8) | lo;
}

void CPU::write(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        console->ram[address % 0x0800] = value;
    } else if (address >= 0x6000) {
        console->cartridge->write(address, value);
    }
}

void CPU::reset() {
    cycles = 0;
    PC = read16(0xFFFC);
    SP = 0xFD;
    setFlags(0x24);
}

void CPU::printInstruction() {
    uint8_t opcode = read(PC);
    uint8_t byte = instructionSize[opcode];
    const char *name = instructionNames[opcode];
    char buffer[512];
    char w0[4] = {0}, w1[4] = {0}, w2[4] = {0};
    std::sprintf(buffer, "%02X", read(PC + 0));
    std::strncpy(w0, buffer, 3);

    std::sprintf(buffer, "%02X", read(PC + 1));
    std::strncpy(w1, buffer, 3);

    std::sprintf(buffer, "%02X", read(PC + 2));
    std::strncpy(w2, buffer, 3);
    if (byte < 2) {
        std::strncpy(w1, "  ", 3);
    }
    if (byte < 3) {
        std::strncpy(w2, "  ", 3);
    }
    std::sprintf(
        buffer,
        "%4X  %s %s %s  %s %28sA:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\n",
        PC, w0, w1, w2, name, "", A, X, Y, flags(), SP, cycles * 3 % 341);
    std::printf(buffer);
}

uint32_t CPU::step() {
    // if (stall > 0) {
    //     stall--;
    //     return 1;
    // }
    // printInstruction();
    uint64_t preCycles = cycles;
    switch (interrupt) {
    case CPU::InterruptType::NMI:
        nmi();
        break;
    case CPU::InterruptType::IRQ:
        irq();
        break;
    }
    interrupt = InterruptType::None;

    uint8_t opcode = read(PC);

    AddressMode mode = AddressMode(instructionMode[opcode]);

    uint16_t address = 0;
    bool pageCrossed = false;
    switch (mode) {
    case AddressMode::Absolute:
        address = read16(PC + 1);
        break;
    case AddressMode::AbsoluteX:
        address = read16(PC + 1) + uint16_t(X);
        pageCrossed = pageDiffer(address - uint16_t(X), address);
        break;
    case AddressMode::AbsoluteY:
        address = read16(PC + 1) + uint16_t(Y);
        pageCrossed = pageDiffer(address - uint16_t(Y), address);
        break;
    case AddressMode::Accumulator:
        address = 0;
        break;
    case AddressMode::Immediate:
        address = PC + 1;
        break;
    case AddressMode::Implied:
        address = 0;
        break;
    case AddressMode::IndexedIndirect: 
        address = read16bug(uint16_t((read(PC + 1) + X) & 0xFF));
        break;
    case AddressMode::Indirect:
        address = read16bug(uint16_t(read16(PC + 1)));
        break;
    case AddressMode::IndirectIndexed:
        address = read16bug(uint16_t(read(PC + 1))) + uint16_t(Y);
        pageCrossed = pageDiffer(address - uint16_t(Y), address);
        break;
    case AddressMode::Relative: {
        uint16_t offset = uint16_t(read(PC + 1));
        if (offset < 0x80) {
            address = PC + 2 + offset;
        } else {
            address = PC + 2 + offset - 0x100;
        }
        break;
    }
    case AddressMode::ZeroPage:
        address = uint16_t(read(PC + 1));
        break;
    case AddressMode::ZeroPageX:
        address = uint16_t(read(PC + 1) + X) & 0xFF;
        break;
    case AddressMode::ZeroPageY:
        address = uint16_t(read(PC + 1) + Y) & 0xFF;
        break;
    }

    PC += uint16_t(instructionSize[opcode]);
    cycles += uint64_t(instructionCycle[opcode]);
    if (pageCrossed) {
        cycles += uint64_t(instructionPageCycle[opcode]);
    }
    StepInfo info{address, PC, mode};
    (table[opcode])(this, &info);
    return uint32_t(cycles - preCycles);
}

bool CPU::pageDiffer(uint16_t a, uint16_t b) {
    return (a & 0xFF00) != (b & 0xFF00);
}

void CPU::addBranchCycles(CPU::StepInfo *info) {
    cycles++;
    if (pageDiffer(PC, info->address)) {
        cycles++;
    }
}

void CPU::compare(uint8_t a, uint8_t b) {
    setZN(a - b);
    if (a >= b) {
        C = 1;
    } else {
        C = 0;
    }
}

void CPU::push(uint8_t value) {
    write(0x100 | uint16_t(SP), value);
    SP--;
}

void CPU::push16(uint16_t value) {
    uint8_t hi = uint8_t(value >> 8);
    uint8_t lo = uint8_t(value & 0xFF);
    push(hi);
    push(lo);
}

uint8_t CPU::pull() {
    SP++;
    return read(0x100 | uint16_t(SP));
}

uint16_t CPU::pull16() {
    uint16_t lo = uint16_t(pull());
    uint16_t hi = uint16_t(pull());
    return (hi << 8) | lo;
}

uint8_t CPU::flags() {
    uint8_t flags = 0;
    flags |= C << 0;
    flags |= Z << 1;
    flags |= I << 2;
    flags |= D << 3;
    flags |= B << 4;
    flags |= U << 5;
    flags |= V << 6;
    flags |= N << 7;
    return flags;
}

void CPU::setFlags(uint8_t flags) {
    C = (flags >> 0) & 0x1;
    Z = (flags >> 1) & 0x1;
    I = (flags >> 2) & 0x1;
    D = (flags >> 3) & 0x1;
    B = (flags >> 4) & 0x1;
    U = (flags >> 5) & 0x1;
    V = (flags >> 6) & 0x1;
    N = (flags >> 7) & 0x1;
}

// 设置zero flag，如果value为0
void CPU::setZ(uint8_t value) {
    if (value == 0) {
        Z = 1;
    } else {
        Z = 0;
    }
}

// 设置negative flag，如果value为负值(最高位为1)
void CPU::setN(uint8_t value) {
    if ((value & 0x80) != 0) {
        N = 1;
    } else {
        N = 0;
    }
}

// 设置zero flag和negative flag
void CPU::setZN(uint8_t value) {
    setZ(value);
    setN(value);
}

void CPU::triggerNMI() { interrupt = InterruptType::NMI; }

void CPU::triggerIRQ() {
    if (I == 0) {
        interrupt = InterruptType::IRQ;
    }
}

void CPU::nmi() {
    push16(PC);
    php(this, nullptr);
    PC = read16(NMI_ADDRESS);
    I = 1;
    cycles += 7;
}

void CPU::irq() {
    push16(PC);
    php(this, nullptr);
    PC = read16(IRQ_ADDRESS);
    I = 1;
    cycles += 7;
}