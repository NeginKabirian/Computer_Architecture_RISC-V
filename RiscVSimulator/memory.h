// Memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <cstdint>

class Memory {
private:
    static constexpr uint32_t MEM_SIZE = 64 * 1024; // 64KB
    std::vector<uint8_t> data;

public:
    Memory();

    uint8_t read8(uint32_t addr) const;

    uint32_t read32(uint32_t addr) const;

    void write8(uint32_t addr, uint8_t value);

    void write32(uint32_t addr, uint32_t value);

private:
    void check_address(uint32_t addr) const;
};

#endif // MEMORY_H
