#ifndef CPU_H
#define CPU_H

#include "alu.h"
#include "memory.h"
#include <iostream>
#include <RegisterFile.h>
#include <cstdint>
#include "DecodedInstruction.h"
#include <iomanip>

#include <QDebug>
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
    uint32_t PC = 0;
    uint32_t IR = 0;
    uint32_t DR = 0;
    uint16_t AR = 0;
    uint32_t A = 0, B = 0;
    uint32_t Imm = 0;
    mutable uint32_t lastPrintedIR = 0xFFFFFFFF;
public:
    void reset() ;
    ALU Alu;
    Memory* memory = nullptr;
    RegisterFile* regFile = nullptr;
    CPU(Memory* mem, RegisterFile* rf);
    void decode(uint32_t instruction);
    //void execute();
    void clockTick();
    void executeMicroStep();
    void printState() const;
    const DecodedInstruction& getCurrentInstruction() const { return currentInstruction; }
    uint32_t getPC() const { return PC; }
    uint32_t getAR() const { return AR; }
    uint32_t getIR() const { return IR; }
    uint32_t getDR() const { return DR; }
    uint32_t getA() const { return A; }
    uint32_t getB() const { return B; }
    uint32_t getImm() const { return Imm; }
};

#endif // CPU_H
