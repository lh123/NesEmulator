#include "nes/CPU.h"
#include <cstdio>
#include <cstring>

#include "nes/Console.h"
#include "nes/Serialize.hpp"

static InstructionFunc InstructionTable[256] = {
    brk, ora,  kil, slo, nop, ora,  asl, slo, //
    php, ora,  asl, anc, nop, ora,  asl, slo, //
    bpl, ora,  kil, slo, nop, ora,  asl, slo, //
    clc, ora,  nop, slo, nop, ora,  asl, slo, //
    jsr, and_, kil, rla, bit, and_, rol, rla, //
    plp, and_, rol, anc, bit, and_, rol, rla, //
    bmi, and_, kil, rla, nop, and_, rol, rla, //
    sec, and_, nop, rla, nop, and_, rol, rla, //
    rti, eor,  kil, sre, nop, eor,  lsr, sre, //
    pha, eor,  lsr, alr, jmp, eor,  lsr, sre, //
    bvc, eor,  kil, sre, nop, eor,  lsr, sre, //
    cli, eor,  nop, sre, nop, eor,  lsr, sre, //
    rts, adc,  kil, rra, nop, adc,  ror, rra, //
    pla, adc,  ror, arr, jmp, adc,  ror, rra, //
    bvs, adc,  kil, rra, nop, adc,  ror, rra, //
    sei, adc,  nop, rra, nop, adc,  ror, rra, //
    nop, sta,  nop, sax, sty, sta,  stx, sax, //
    dey, nop,  txa, xaa, sty, sta,  stx, sax, //
    bcc, sta,  kil, ahx, sty, sta,  stx, sax, //
    tya, sta,  txs, tas, shy, sta,  shx, ahx, //
    ldy, lda,  ldx, lax, ldy, lda,  ldx, lax, //
    tay, lda,  tax, lax, ldy, lda,  ldx, lax, //
    bcs, lda,  kil, lax, ldy, lda,  ldx, lax, //
    clv, lda,  tsx, las, ldy, lda,  ldx, lax, //
    cpy, cmp,  nop, dcp, cpy, cmp,  dec, dcp, //
    iny, cmp,  dex, axs, cpy, cmp,  dec, dcp, //
    bne, cmp,  kil, dcp, nop, cmp,  dec, dcp, //
    cld, cmp,  nop, dcp, nop, cmp,  dec, dcp, //
    cpx, sbc,  nop, isc, cpx, sbc,  inc, isc, //
    inx, sbc,  nop, sbc, cpx, sbc,  inc, isc, //
    beq, sbc,  kil, isc, nop, sbc,  inc, isc, //
    sed, sbc,  nop, isc, nop, sbc,  inc, isc  //
};

static const uint8_t instructionMode[256] = {
    6,  7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1, //
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2, //
    1,  7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1, //
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2, //
    6,  7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1, //
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2, //
    6,  7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1, //
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2, //
    5,  7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1, //
    10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3, //
    5,  7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1, //
    10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3, //
    5,  7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1, //
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2, //
    5,  7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1, //
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2  //
};

static const uint8_t instructionSize[256] = {
    1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0, //
    3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0, //
    1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0, //
    1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0, //
    2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, //
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0  //
};

static const uint8_t instructionCycle[256] = {
    7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, //
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //
    6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, //
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //
    6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, //
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //
    6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6, //
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, //
    2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5, //
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, //
    2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4, //
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, //
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, //
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7  //
};

static const uint8_t instructionPageCycle[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, //
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, //
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, //
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, //
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, //
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, //
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0  //
};

static const char *instructionNames[256] = {
    "BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO", //
    "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO", //
    "BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO", //
    "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO", //
    "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA", //
    "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA", //
    "BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA", //
    "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA", //
    "RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE", //
    "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE", //
    "BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE", //
    "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE", //
    "RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA", //
    "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA", //
    "BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA", //
    "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA", //
    "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX", //
    "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX", //
    "BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX", //
    "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX", //
    "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", //
    "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX", //
    "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX", //
    "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX", //
    "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP", //
    "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP", //
    "BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP", //
    "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP", //
    "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC", //
    "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC", //
    "BEQ", "SBC", "KIL", "ISC", "NOP", "SBC", "INC", "ISC", //
    "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC"  //
};

CPU::CPU(Console *console)
    : CPUMemory(console), stall(0), cycles(0), PC(0), SP(0), A(0), X(0), Y(0), C(0), Z(0), I(0), D(0), B(0), U(0), V(0),
      N(0), interrupt(InterruptType::None) {
    reset();
}

CPU::~CPU() {}

uint16_t CPU::read16(uint16_t address) {
    uint16_t lo = uint16_t(read(address));
    uint16_t hi = uint16_t(read(address + 1));
    return (hi << 8) | lo;
}

uint16_t CPU::read16bug(uint16_t address) {
    uint16_t a = address;
    uint16_t b = (a & 0xFF00) | uint16_t(uint8_t(a + 1));
    uint16_t lo = uint16_t(read(a));
    uint16_t hi = uint16_t(read(b));
    return (hi << 8) | lo;
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
    std::sprintf(buffer, "%4X  %s %s %s  %s %28sA:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\n", PC, w0, w1, w2, name,
                 "", A, X, Y, flags(), SP, int(cycles * 3 % 341));
    std::printf(buffer);
}

uint32_t CPU::step() {
    if (stall > 0) {
        stall--;
        return 1;
    }
    // printInstruction();
    switch (interrupt) {
    case InterruptType::NMI:
        nmi();
        break;
    case InterruptType::IRQ:
        irq();
        break;
    case InterruptType::None:
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
        address = read16bug(uint16_t((read(PC + 1) + X)) & 0xFF);
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

    uint64_t preCycles = cycles;

    PC += uint16_t(instructionSize[opcode]);
    cycles += uint64_t(instructionCycle[opcode]);
    if (pageCrossed) {
        cycles += uint64_t(instructionPageCycle[opcode]);
    }
    StepInfo info{address, PC, mode};
    (InstructionTable[opcode])(this, &info);
    return uint32_t(cycles - preCycles);
}

bool CPU::pageDiffer(uint16_t a, uint16_t b) { return (a & 0xFF00) != (b & 0xFF00); }

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

void CPU::save(Serialize &serialize) {
    serialize << stall;
    serialize << cycles;
    serialize << PC;
    serialize << SP;
    serialize << A;
    serialize << X;
    serialize << Y;
    serialize << C;
    serialize << Z;
    serialize << I;
    serialize << D;
    serialize << B;
    serialize << U;
    serialize << V;
    serialize << N;
    serialize << interrupt;
}

void CPU::load(Serialize &serialize) {
    serialize >> stall;
    serialize >> cycles;
    serialize >> PC;
    serialize >> SP;
    serialize >> A;
    serialize >> X;
    serialize >> Y;
    serialize >> C;
    serialize >> Z;
    serialize >> I;
    serialize >> D;
    serialize >> B;
    serialize >> U;
    serialize >> V;
    serialize >> N;
    serialize >> interrupt;
}