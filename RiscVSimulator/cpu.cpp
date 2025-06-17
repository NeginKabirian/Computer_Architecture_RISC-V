#include "cpu.h"

void CPU::fetch() {
    AR = PC;
    IR = memory->read32(AR);
    PC += 4;
}

