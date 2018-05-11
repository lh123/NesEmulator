#ifndef CPU_H
#define CPU_H

#include "nes/Memory.h"
#include <cstdint>

class Console;
class Serialize;

class CPU : public CPUMemory {

public:
    static constexpr uint16_t NMI_ADDRESS = 0xFFFA;
    static constexpr uint16_t IRQ_ADDRESS = 0xFFFE;
    static constexpr int CPU_FREQUENCY = 1789773;

    enum class InterruptType { None = 1, NMI, IRQ };

    enum class AddressMode {
        Absolute = 1,
        AbsoluteX,
        AbsoluteY,
        Accumulator,
        Immediate,
        Implied,
        IndexedIndirect,
        Indirect,
        IndirectIndexed,
        Relative,
        ZeroPage,
        ZeroPageX,
        ZeroPageY
    };

    struct StepInfo {
        uint16_t address;
        uint16_t pc;
        AddressMode mode;
    };

public:
    CPU(Console *console);
    ~CPU();

    // uint8_t read(uint16_t address);
    uint16_t read16(uint16_t address);
    uint16_t read16bug(uint16_t address);

    // void write(uint16_t address, uint8_t value);

    bool pageDiffer(uint16_t a, uint16_t b);
    void addBranchCycles(StepInfo *info);
    void compare(uint8_t a, uint8_t b);

    void push(uint8_t value);
    void push16(uint16_t value);
    uint8_t pull();
    uint16_t pull16();

    void reset();
    void printInstruction();

    uint32_t step();

    uint8_t flags();
    void setFlags(uint8_t flags);

    void setN(uint8_t value);
    void setZ(uint8_t value);
    void setZN(uint8_t value);

    void triggerNMI();
    void triggerIRQ();

    void nmi();
    void irq();

    void save(Serialize &serialize);
    void load(Serialize &serialize);

public:
    friend void adc(CPU *cpu, CPU::StepInfo *info);
    friend void and_(CPU *cpu, CPU::StepInfo *info);
    friend void asl(CPU *cpu, CPU::StepInfo *info);
    friend void bcc(CPU *cpu, CPU::StepInfo *info);
    friend void bcs(CPU *cpu, CPU::StepInfo *info);
    friend void beq(CPU *cpu, CPU::StepInfo *info);
    friend void bit(CPU *cpu, CPU::StepInfo *info);
    friend void bmi(CPU *cpu, CPU::StepInfo *info);
    friend void bne(CPU *cpu, CPU::StepInfo *info);
    friend void bpl(CPU *cpu, CPU::StepInfo *info);
    friend void brk(CPU *cpu, CPU::StepInfo *info);
    friend void bvc(CPU *cpu, CPU::StepInfo *info);
    friend void bvs(CPU *cpu, CPU::StepInfo *info);
    friend void clc(CPU *cpu, CPU::StepInfo *info);
    friend void cld(CPU *cpu, CPU::StepInfo *info);
    friend void cli(CPU *cpu, CPU::StepInfo *info);
    friend void clv(CPU *cpu, CPU::StepInfo *info);
    friend void cmp(CPU *cpu, CPU::StepInfo *info);
    friend void cpx(CPU *cpu, CPU::StepInfo *info);
    friend void cpy(CPU *cpu, CPU::StepInfo *info);
    friend void dec(CPU *cpu, CPU::StepInfo *info);
    friend void dex(CPU *cpu, CPU::StepInfo *info);
    friend void dey(CPU *cpu, CPU::StepInfo *info);
    friend void eor(CPU *cpu, CPU::StepInfo *info);
    friend void inc(CPU *cpu, CPU::StepInfo *info);
    friend void inx(CPU *cpu, CPU::StepInfo *info);
    friend void iny(CPU *cpu, CPU::StepInfo *info);
    friend void jmp(CPU *cpu, CPU::StepInfo *info);
    friend void jsr(CPU *cpu, CPU::StepInfo *info);
    friend void lda(CPU *cpu, CPU::StepInfo *info);
    friend void ldx(CPU *cpu, CPU::StepInfo *info);
    friend void ldy(CPU *cpu, CPU::StepInfo *info);
    friend void lsr(CPU *cpu, CPU::StepInfo *info);
    friend void nop(CPU *cpu, CPU::StepInfo *info);
    friend void ora(CPU *cpu, CPU::StepInfo *info);
    friend void pha(CPU *cpu, CPU::StepInfo *info);
    friend void php(CPU *cpu, CPU::StepInfo *info);
    friend void pla(CPU *cpu, CPU::StepInfo *info);
    friend void plp(CPU *cpu, CPU::StepInfo *info);
    friend void rol(CPU *cpu, CPU::StepInfo *info);
    friend void ror(CPU *cpu, CPU::StepInfo *info);
    friend void rti(CPU *cpu, CPU::StepInfo *info);
    friend void rts(CPU *cpu, CPU::StepInfo *info);
    friend void sbc(CPU *cpu, CPU::StepInfo *info);
    friend void sec(CPU *cpu, CPU::StepInfo *info);
    friend void sed(CPU *cpu, CPU::StepInfo *info);
    friend void sei(CPU *cpu, CPU::StepInfo *info);
    friend void sta(CPU *cpu, CPU::StepInfo *info);
    friend void stx(CPU *cpu, CPU::StepInfo *info);
    friend void sty(CPU *cpu, CPU::StepInfo *info);
    friend void tax(CPU *cpu, CPU::StepInfo *info);
    friend void tay(CPU *cpu, CPU::StepInfo *info);
    friend void tsx(CPU *cpu, CPU::StepInfo *info);
    friend void txa(CPU *cpu, CPU::StepInfo *info);
    friend void txs(CPU *cpu, CPU::StepInfo *info);
    friend void tya(CPU *cpu, CPU::StepInfo *info);
    friend void ahx(CPU *cpu, CPU::StepInfo *info);
    friend void alr(CPU *cpu, CPU::StepInfo *info);
    friend void anc(CPU *cpu, CPU::StepInfo *info);
    friend void arr(CPU *cpu, CPU::StepInfo *info);
    friend void axs(CPU *cpu, CPU::StepInfo *info);
    friend void dcp(CPU *cpu, CPU::StepInfo *info);
    friend void isc(CPU *cpu, CPU::StepInfo *info);
    friend void kil(CPU *cpu, CPU::StepInfo *info);
    friend void las(CPU *cpu, CPU::StepInfo *info);
    friend void lax(CPU *cpu, CPU::StepInfo *info);
    friend void rla(CPU *cpu, CPU::StepInfo *info);
    friend void rra(CPU *cpu, CPU::StepInfo *info);
    friend void sax(CPU *cpu, CPU::StepInfo *info);
    friend void shx(CPU *cpu, CPU::StepInfo *info);
    friend void shy(CPU *cpu, CPU::StepInfo *info);
    friend void slo(CPU *cpu, CPU::StepInfo *info);
    friend void sre(CPU *cpu, CPU::StepInfo *info);
    friend void tas(CPU *cpu, CPU::StepInfo *info);
    friend void xaa(CPU *cpu, CPU::StepInfo *info);

public:
    uint8_t stall;   // number of cycles to stall
    uint64_t cycles; // number of cycles

private:
    // Console *console;

    uint16_t PC; // program counter
    uint8_t SP;  // stack pointer

    uint8_t A; // accumulator
    uint8_t X; // index register X
    uint8_t Y; // index register Y

    uint8_t C; // carry flag
    uint8_t Z; // zero flag
    uint8_t I; // interrupt flag
    uint8_t D; // decimal flag
    uint8_t B; // break command
    uint8_t U; // unused flag
    uint8_t V; // overflow flag
    uint8_t N; // negative flag

    InterruptType interrupt; // interrupt type to perform
};

#include "nes/Instruction.h"

#endif