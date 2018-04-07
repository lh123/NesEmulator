#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "nes/CPU.h"
#include <cstdint>

typedef void (*InstructionFunc)(CPU *, CPU::StepInfo *);

static constexpr uint8_t instructionMode[256] = {
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

static constexpr uint8_t instructionSize[256] = {
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

static constexpr uint8_t instructionCycle[256] = {
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

static constexpr uint8_t instructionPageCycle[256] = {
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

static constexpr const char *instructionNames[256] = {
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

void adc(CPU *cpu, CPU::StepInfo *info);
void and_(CPU *cpu, CPU::StepInfo *info);
void asl(CPU *cpu, CPU::StepInfo *info);
void bcc(CPU *cpu, CPU::StepInfo *info);
void bcs(CPU *cpu, CPU::StepInfo *info);
void beq(CPU *cpu, CPU::StepInfo *info);
void bit(CPU *cpu, CPU::StepInfo *info);
void bmi(CPU *cpu, CPU::StepInfo *info);
void bne(CPU *cpu, CPU::StepInfo *info);
void bpl(CPU *cpu, CPU::StepInfo *info);
void brk(CPU *cpu, CPU::StepInfo *info);
void bvc(CPU *cpu, CPU::StepInfo *info);
void bvs(CPU *cpu, CPU::StepInfo *info);
void clc(CPU *cpu, CPU::StepInfo *info);
void cld(CPU *cpu, CPU::StepInfo *info);
void cli(CPU *cpu, CPU::StepInfo *info);
void clv(CPU *cpu, CPU::StepInfo *info);
void cmp(CPU *cpu, CPU::StepInfo *info);
void cpx(CPU *cpu, CPU::StepInfo *info);
void cpy(CPU *cpu, CPU::StepInfo *info);
void dec(CPU *cpu, CPU::StepInfo *info);
void dex(CPU *cpu, CPU::StepInfo *info);
void dey(CPU *cpu, CPU::StepInfo *info);
void eor(CPU *cpu, CPU::StepInfo *info);
void inc(CPU *cpu, CPU::StepInfo *info);
void inx(CPU *cpu, CPU::StepInfo *info);
void iny(CPU *cpu, CPU::StepInfo *info);
void jmp(CPU *cpu, CPU::StepInfo *info);
void jsr(CPU *cpu, CPU::StepInfo *info);
void lda(CPU *cpu, CPU::StepInfo *info);
void ldx(CPU *cpu, CPU::StepInfo *info);
void ldy(CPU *cpu, CPU::StepInfo *info);
void lsr(CPU *cpu, CPU::StepInfo *info);
void nop(CPU *cpu, CPU::StepInfo *info);
void ora(CPU *cpu, CPU::StepInfo *info);
void pha(CPU *cpu, CPU::StepInfo *info);
void php(CPU *cpu, CPU::StepInfo *info);
void pla(CPU *cpu, CPU::StepInfo *info);
void plp(CPU *cpu, CPU::StepInfo *info);
void rol(CPU *cpu, CPU::StepInfo *info);
void ror(CPU *cpu, CPU::StepInfo *info);
void rti(CPU *cpu, CPU::StepInfo *info);
void rts(CPU *cpu, CPU::StepInfo *info);
void sbc(CPU *cpu, CPU::StepInfo *info);
void sec(CPU *cpu, CPU::StepInfo *info);
void sed(CPU *cpu, CPU::StepInfo *info);
void sei(CPU *cpu, CPU::StepInfo *info);
void sta(CPU *cpu, CPU::StepInfo *info);
void stx(CPU *cpu, CPU::StepInfo *info);
void sty(CPU *cpu, CPU::StepInfo *info);
void tax(CPU *cpu, CPU::StepInfo *info);
void tay(CPU *cpu, CPU::StepInfo *info);
void tsx(CPU *cpu, CPU::StepInfo *info);
void txa(CPU *cpu, CPU::StepInfo *info);
void txs(CPU *cpu, CPU::StepInfo *info);
void tya(CPU *cpu, CPU::StepInfo *info);
void ahx(CPU *cpu, CPU::StepInfo *info);
void alr(CPU *cpu, CPU::StepInfo *info);
void anc(CPU *cpu, CPU::StepInfo *info);
void arr(CPU *cpu, CPU::StepInfo *info);
void axs(CPU *cpu, CPU::StepInfo *info);
void dcp(CPU *cpu, CPU::StepInfo *info);
void isc(CPU *cpu, CPU::StepInfo *info);
void kil(CPU *cpu, CPU::StepInfo *info);
void las(CPU *cpu, CPU::StepInfo *info);
void lax(CPU *cpu, CPU::StepInfo *info);
void rla(CPU *cpu, CPU::StepInfo *info);
void rra(CPU *cpu, CPU::StepInfo *info);
void sax(CPU *cpu, CPU::StepInfo *info);
void shx(CPU *cpu, CPU::StepInfo *info);
void shy(CPU *cpu, CPU::StepInfo *info);
void slo(CPU *cpu, CPU::StepInfo *info);
void sre(CPU *cpu, CPU::StepInfo *info);
void tas(CPU *cpu, CPU::StepInfo *info);
void xaa(CPU *cpu, CPU::StepInfo *info);

static InstructionFunc table[256] = {
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

#endif