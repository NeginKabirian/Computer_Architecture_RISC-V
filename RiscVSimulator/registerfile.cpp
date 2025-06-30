#include "registerfile.h"
#include <iostream>
#include <QDebug>


RegisterFile::RegisterFile() {
    regs.fill(0);
}

uint32_t RegisterFile::read(uint8_t index) const {
    return (index == 0) ? 0 : regs[index];
}

void RegisterFile::write(uint8_t index, uint32_t value) {
    if (index != 0)
        regs[index] = value;
}

void RegisterFile::reset() {
    regs.fill(0);
}

const std::array<uint32_t, 32> &RegisterFile::getAll() const {
    return regs;
}

void RegisterFile::printRegisters() {
    for (size_t i = 0; i < regs.size(); ++i) {
        qDebug() << "x" << i << "= 0x" << QString::number(regs[i], 16);
    }
}
