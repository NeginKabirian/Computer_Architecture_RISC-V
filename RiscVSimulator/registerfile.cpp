#include "registerfile.h"

RegisterFile::RegisterFile() {}

uint32_t RegisterFile::read(uint8_t index) const {
    return (index == 0) ? 0 : regs[index];
}
