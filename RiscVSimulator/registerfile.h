#ifndef REGISTERFILE_H
#define REGISTERFILE_H
#include <array>
#include <cstdint>


class RegisterFile {
private:
    std::array<uint32_t, 32> regs;

public:
    RegisterFile();

    uint32_t read(uint8_t index) const;

    void write(uint8_t index, uint32_t value);

    void reset();

    const std::array<uint32_t, 32>& getAll() const;

    void printRegisters();
};


#endif // REGISTERFILE_H
