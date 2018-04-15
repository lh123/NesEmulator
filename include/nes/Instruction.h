#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "nes/CPU.h"
#include <cstdint>

using InstructionFunc = void (*)(CPU *, CPU::StepInfo *);

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

#endif