#include "nes/Instruction.h"
#include "nes/CPU.h"

// ADC: Add with Carry
void adc(CPU *cpu, CPU::StepInfo *info) {
    uint8_t a = cpu->A;
    uint8_t b = cpu->read(info->address);
    uint8_t c = cpu->C;

    cpu->A = a + b + c;
    cpu->setZN(cpu->A);
    if (int16_t(a) + int16_t(b) + int16_t(c) > 0xFF) {
        cpu->C = 1;
    } else {
        cpu->C = 0;
    }
    if (((a ^ b) & 0x80) == 0 && ((a ^ cpu->A) & 0x80) != 0) {
        cpu->V = 1;
    } else {
        cpu->V = 0;
    }
}

void and_(CPU *cpu, CPU::StepInfo *info) {
    cpu->A = cpu->A & cpu->read(info->address);
    cpu->setZN(cpu->A);
}

void asl(CPU *cpu, CPU::StepInfo *info) {
    if (info->mode == CPU::AddressMode::Accumulator) {
        cpu->C = (cpu->A >> 7) & 0x1;
        cpu->A <<= 1;
        cpu->setZN(cpu->A);
    } else {
        uint8_t value = cpu->read(info->address);
        cpu->C = (value >> 7) & 0x1;
        value <<= 1;
        cpu->write(info->address, value);
        cpu->setZN(value);
    }
}

void bcc(CPU *cpu, CPU::StepInfo *info) {
    if (cpu->C == 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(info);
    }
}

void bcs(CPU *cpu, CPU::StepInfo *info) {
    if (cpu->C != 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(info);
    }
}

void beq(CPU *cpu, CPU::StepInfo *info) {
    if (cpu->Z != 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(info);
    }
}

void bit(CPU *cpu, CPU::StepInfo *info) {
    uint8_t value = cpu->read(info->address);
    cpu->V = (value >> 6) & 0x1;
    cpu->setZ(cpu->A & value);
    cpu->setN(value);
}

void bmi(CPU *cpu, CPU::StepInfo *info) {
    if (cpu->N != 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(info);
    }
}

void bne(CPU *cpu, CPU::StepInfo *info) {
    if (cpu->Z == 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(info);
    }
}

void bpl(CPU *cpu, CPU::StepInfo *info) {
    if (cpu->N == 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(info);
    }
}

void brk(CPU *cpu, CPU::StepInfo *info) {
    cpu->push16(cpu->PC);
    php(cpu, info);
    sei(cpu, info);
    cpu->PC = cpu->read16(0xFFFE);
}

void bvc(CPU *cpu, CPU::StepInfo *info) {
    if (cpu->V == 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(info);
    }
}

void bvs(CPU *cpu, CPU::StepInfo *info) {
    if (cpu->V != 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(info);
    }
}

void clc(CPU *cpu, CPU::StepInfo *info) { cpu->C = 0; }

void cld(CPU *cpu, CPU::StepInfo *info) { cpu->D = 0; }

void cli(CPU *cpu, CPU::StepInfo *info) { cpu->I = 0; }

void clv(CPU *cpu, CPU::StepInfo *info) { cpu->V = 0; }

void cmp(CPU *cpu, CPU::StepInfo *info) {
    uint8_t value = cpu->read(info->address);
    cpu->compare(cpu->A, value);
}

void cpx(CPU *cpu, CPU::StepInfo *info) {
    uint8_t value = cpu->read(info->address);
    cpu->compare(cpu->X, value);
}

void cpy(CPU *cpu, CPU::StepInfo *info) {
    uint8_t value = cpu->read(info->address);
    cpu->compare(cpu->Y, value);
}

void dec(CPU *cpu, CPU::StepInfo *info) {
    uint8_t value = cpu->read(info->address) - 1;
    cpu->write(info->address, value);
    cpu->setZN(value);
}

void dex(CPU *cpu, CPU::StepInfo *info) {
    cpu->X--;
    cpu->setZN(cpu->X);
}

void dey(CPU *cpu, CPU::StepInfo *info) {
    cpu->Y--;
    cpu->setZN(cpu->Y);
}

void eor(CPU *cpu, CPU::StepInfo *info) {
    uint8_t value = cpu->read(info->address);
    cpu->A = cpu->A ^ value;
    cpu->setZN(cpu->A);
}

void inc(CPU *cpu, CPU::StepInfo *info) {
    uint8_t value = cpu->read(info->address) + 1;
    cpu->write(info->address, value);
    cpu->setZN(value);
}

void inx(CPU *cpu, CPU::StepInfo *info) {
    cpu->X++;
    cpu->setZN(cpu->X);
}

void iny(CPU *cpu, CPU::StepInfo *info) {
    cpu->Y++;
    cpu->setZN(cpu->Y);
}

void jmp(CPU *cpu, CPU::StepInfo *info) { cpu->PC = info->address; }

void jsr(CPU *cpu, CPU::StepInfo *info) {
    cpu->push16(cpu->PC - 1);
    cpu->PC = info->address;
}

void lda(CPU *cpu, CPU::StepInfo *info) {
    cpu->A = cpu->read(info->address);
    cpu->setZN(cpu->A);
}

void ldx(CPU *cpu, CPU::StepInfo *info) {
    cpu->X = cpu->read(info->address);
    cpu->setZN(cpu->X);
}

void ldy(CPU *cpu, CPU::StepInfo *info) {
    cpu->Y = cpu->read(info->address);
    cpu->setZN(cpu->Y);
}

void lsr(CPU *cpu, CPU::StepInfo *info) {
    if (info->mode == CPU::AddressMode::Accumulator) {
        cpu->C = cpu->A & 0x1;
        cpu->A >>= 1;
        cpu->setZN(cpu->A);
    } else {
        uint8_t value = cpu->read(info->address);
        cpu->C = value & 0x1;
        value >>= 1;
        cpu->write(info->address, value);
        cpu->setZN(value);
    }
}

void nop(CPU *cpu, CPU::StepInfo *info) {}

void ora(CPU *cpu, CPU::StepInfo *info) {
    cpu->A = cpu->A | cpu->read(info->address);
    cpu->setZN(cpu->A);
}

void pha(CPU *cpu, CPU::StepInfo *info) { cpu->push(cpu->A); }

void php(CPU *cpu, CPU::StepInfo *info) { cpu->push(cpu->flags() | 0x10); }

void pla(CPU *cpu, CPU::StepInfo *info) {
    cpu->A = cpu->pull();
    cpu->setZN(cpu->A);
}

void plp(CPU *cpu, CPU::StepInfo *info) { cpu->setFlags((cpu->pull() & 0xEF) | 0x20); }

void rol(CPU *cpu, CPU::StepInfo *info) {
    if (info->mode == CPU::AddressMode::Accumulator) {
        uint8_t c = cpu->C;
        cpu->C = (cpu->A >> 7) & 0x1;
        cpu->A = (cpu->A << 1) | c;
        cpu->setZN(cpu->A);
    } else {
        uint8_t c = cpu->C;
        uint8_t value = cpu->read(info->address);
        cpu->C = (value >> 7) & 0x1;
        value = (value << 1) | c;
        cpu->write(info->address, value);
        cpu->setZN(value);
    }
}

void ror(CPU *cpu, CPU::StepInfo *info) {
    if (info->mode == CPU::AddressMode::Accumulator) {
        uint8_t c = cpu->C;
        cpu->C = cpu->A & 0x1;
        cpu->A = (cpu->A >> 1) | (c << 7);
        cpu->setZN(cpu->A);
    } else {
        uint8_t c = cpu->C;
        uint8_t value = cpu->read(info->address);
        cpu->C = value & 0x1;
        value = (value >> 1) | (c << 7);
        cpu->write(info->address, value);
        cpu->setZN(value);
    }
}

void rti(CPU *cpu, CPU::StepInfo *info) {
    cpu->setFlags((cpu->pull() & 0xEF) | 0x20);
    cpu->PC = cpu->pull16();
}

void rts(CPU *cpu, CPU::StepInfo *info) { cpu->PC = cpu->pull16() + 1; }

void sbc(CPU *cpu, CPU::StepInfo *info) {
    uint8_t a = cpu->A;
    uint8_t b = cpu->read(info->address);
    uint8_t c = cpu->C;
    cpu->A = a - b - (1 - c);
    cpu->setZN(cpu->A);
    if (int16_t(a) - int16_t(b) - int16_t(1 - c) >= 0) {
        cpu->C = 1;
    } else {
        cpu->C = 0;
    }
    if (((a ^ b) & 0x80) != 0 && ((a ^ cpu->A) & 0x80) != 0) {
        cpu->V = 1;
    } else {
        cpu->V = 0;
    }
}

void sec(CPU *cpu, CPU::StepInfo *info) { cpu->C = 1; }

void sed(CPU *cpu, CPU::StepInfo *info) { cpu->D = 1; }

void sei(CPU *cpu, CPU::StepInfo *info) { cpu->I = 1; }

void sta(CPU *cpu, CPU::StepInfo *info) { cpu->write(info->address, cpu->A); }

void stx(CPU *cpu, CPU::StepInfo *info) { cpu->write(info->address, cpu->X); }

void sty(CPU *cpu, CPU::StepInfo *info) { cpu->write(info->address, cpu->Y); }

void tax(CPU *cpu, CPU::StepInfo *info) {
    cpu->X = cpu->A;
    cpu->setZN(cpu->X);
}

void tay(CPU *cpu, CPU::StepInfo *info) {
    cpu->Y = cpu->A;
    cpu->setZN(cpu->Y);
}

void tsx(CPU *cpu, CPU::StepInfo *info) {
    cpu->X = cpu->SP;
    cpu->setZN(cpu->X);
}

void txa(CPU *cpu, CPU::StepInfo *info) {
    cpu->A = cpu->X;
    cpu->setZN(cpu->A);
}

void txs(CPU *cpu, CPU::StepInfo *info) {
    cpu->SP = cpu->X;
    // cpu->setZN(cpu->SP);
}

void tya(CPU *cpu, CPU::StepInfo *info) {
    cpu->A = cpu->Y;
    cpu->setZN(cpu->A);
}

//非法操作符
void ahx(CPU *cpu, CPU::StepInfo *info) {}
void alr(CPU *cpu, CPU::StepInfo *info) {}
void anc(CPU *cpu, CPU::StepInfo *info) {}
void arr(CPU *cpu, CPU::StepInfo *info) {}
void axs(CPU *cpu, CPU::StepInfo *info) {}
void dcp(CPU *cpu, CPU::StepInfo *info) {}
void isc(CPU *cpu, CPU::StepInfo *info) {}
void kil(CPU *cpu, CPU::StepInfo *info) {}
void las(CPU *cpu, CPU::StepInfo *info) {}
void lax(CPU *cpu, CPU::StepInfo *info) {}
void rla(CPU *cpu, CPU::StepInfo *info) {}
void rra(CPU *cpu, CPU::StepInfo *info) {}
void sax(CPU *cpu, CPU::StepInfo *info) {}
void shx(CPU *cpu, CPU::StepInfo *info) {}
void shy(CPU *cpu, CPU::StepInfo *info) {}
void slo(CPU *cpu, CPU::StepInfo *info) {}
void sre(CPU *cpu, CPU::StepInfo *info) {}
void tas(CPU *cpu, CPU::StepInfo *info) {}
void xaa(CPU *cpu, CPU::StepInfo *info) {}