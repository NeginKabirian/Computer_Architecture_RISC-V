// Memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <cstdint>
#include <fstream>
#include<iostream>
#include <QString>
#include <QDebug>
class Memory {
private:
    static constexpr uint32_t MEM_SIZE = 64 * 1024; // 64KB
    std::vector<uint8_t> data;

public:
    Memory();

    uint8_t read8(uint32_t addr) const;

    uint32_t read32(uint32_t addr) const;

    uint16_t read16(uint32_t addr) const;

    void write16(uint32_t addr, uint16_t value);

    void write8(uint32_t addr, uint8_t value);

    void write32(uint32_t addr, uint32_t value);
    bool loadFromFile(const std::string& filepath, uint32_t startAddress);
    void dump(uint32_t startAddress, uint32_t numBytes) const;

private:
    void check_address(uint32_t addr) const;
};

#endif // MEMORY_H
