#include "memory.h"

Memory::Memory() {}

Memory::Memory(size_t size) : mem(size, 0) {}

uint32_t Memory::read(uint32_t address) const {
    if (address / 4 < mem.size())
        return mem[address / 4];
    throw std::out_of_range("Invalid memory read");
}
