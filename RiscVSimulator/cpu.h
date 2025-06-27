#ifndef CPU_H
#define CPU_H

#include "alu.h"
#include "memory.h"
#include <iostream>
#include <RegisterFile.h>
#include <cstdint>
#include "DecodedInstruction.h"

enum class CPUStage {
    Fetch1,
    Fetch2,
    Decode,
    Exec,
    Mem,
    WriteBack,
    HALT
};

class CPU {
private:
    DecodedInstruction currentInstruction;
    CPUStage stage = CPUStage::Fetch1;
    int cycleStep = 0;
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
    void decode(uint32_t instruction);
    void execute();
    void clockTick();
    void executeMicroStep();
};

#endif // CPU_H
