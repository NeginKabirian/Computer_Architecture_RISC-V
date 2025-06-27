#include "memory.h"

#include <stdexcept>

Memory::Memory() : data(MEM_SIZE, 0) {}

uint8_t Memory::read8(uint32_t addr) const {
    check_address(addr);
    return data[addr];
}

uint32_t Memory::read32(uint32_t addr) const {
    check_address(addr);
    if (addr + 3 >= MEM_SIZE) throw std::out_of_range("32-bit read out of bounds");
    return data[addr] |
           (data[addr+1] << 8) |
           (data[addr+2] << 16) |
           (data[addr+3] << 24);
}

void Memory::write8(uint32_t addr, uint8_t value) {
    check_address(addr);
    data[addr] = value;
}

void Memory::write32(uint32_t addr, uint32_t value) {
    check_address(addr);
    if (addr + 3 >= MEM_SIZE) throw std::out_of_range("32-bit write out of bounds");
    data[addr]     = value & 0xFF;
    data[addr + 1] = (value >> 8) & 0xFF;
    data[addr + 2] = (value >> 16) & 0xFF;
    data[addr + 3] = (value >> 24) & 0xFF;
}

uint16_t Memory::read16(uint32_t addr) const {
    check_address(addr);
    check_address(addr + 1);
    return data[addr] | (data[addr + 1] << 8);
}

void Memory::write16(uint32_t addr, uint16_t value) {
    check_address(addr);
    check_address(addr + 1);
    data[addr] = value & 0xFF;
    data[addr + 1] = (value >> 8) & 0xFF;
}


void Memory::check_address(uint32_t addr) const {
    if (addr >= MEM_SIZE) {
        throw std::out_of_range("Memory access out of range");
    }
}
