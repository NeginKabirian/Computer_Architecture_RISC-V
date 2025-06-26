#ifndef CPU_H
#define CPU_H

#include "alu.h"
#include "memory.h"

#include <RegisterFile.h>
#include <cstdint>
#include "DecodedInstruction.h"


class CPU {
public:
    uint32_t PC = 0x1000;
    uint32_t IR = 0;
    uint32_t DR = 0;
    uint16_t AR = 0;
    uint32_t A = 0, B = 0;
    uint32_t Imm = 0;
    ALU Alu;
    Memory* memory = nullptr;
    RegisterFile* regFile = nullptr;
    CPU(Memory* mem, RegisterFile* rf);
    void fetch();
    DecodedInstruction decode(uint32_t instruction);
    void execute();

};

#endif // CPU_H
