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

bool Memory::loadFromFile(const std::string& filepath, uint32_t startAddress) {
    qDebug() << "Attempting to load binary from:" << QString::fromStdString(filepath);

    if (startAddress == 0) {
        qCritical() << "-> CRITICAL: Start address cannot be 0.";
        qCritical() << "-> Address 0 is reserved for the system's reset vector.";
        qCritical() << "-> Please load the program at a higher address (e.g., 0x0100).";
        return false;
    }

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        qCritical() << "-> CRITICAL: Failed to open file. Check path and permissions.";
        return false;
    }
    qDebug() << "-> File opened successfully.";


    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    qDebug() << "-> File size detected:" << fileSize << "bytes.";


    if (fileSize == 0) {
        qWarning() << "-> WARNING: File is empty. Nothing to load.";
        return true;
    }
    if (fileSize < 0) {
        qCritical() << "-> CRITICAL: Could not determine file size. Stream error.";
        return false;
    }


    if (startAddress + fileSize > MEM_SIZE) {
        qCritical() << "-> CRITICAL: Not enough memory to load the file.";
        return false;
    }
    qDebug() << "-> Memory check passed.";


    qDebug() << "-> Reading" << fileSize << "bytes into memory at address" << startAddress;
    file.read(reinterpret_cast<char*>(data.data() + startAddress), fileSize);


    if (file.fail() && !file.eof()) {
        qCritical() << "-> CRITICAL: An error occurred while reading the file.";

        std::fill(data.begin() + startAddress, data.begin() + startAddress + fileSize, 0);
        return false;
    }
    qDebug() << "-> Read operation completed. Bytes read:" << file.gcount();


    qDebug() << "-> Successfully processed file loading.";
    return true;
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
/*void Memory::dump(uint32_t startAddress, uint32_t numBytes) const {
    qDebug() << "\n--- Memory Dump from" << QString("0x%1").arg(startAddress, 8, 16, QChar('0')) << "---";

    for (uint32_t i = 0; i < numBytes; i += 4) {
        uint32_t addr = startAddress + i;
        if (addr + 3 >= MEM_SIZE) break;

        uint32_t word = read32(addr);

        QString rawBytes = QString("(%1 %2 %3 %4)")
                               .arg(data[addr], 2, 16, QChar('0'))
                               .arg(data[addr+1], 2, 16, QChar('0'))
                               .arg(data[addr+2], 2, 16, QChar('0'))
                               .arg(data[addr+3], 2, 16, QChar('0'));

        qDebug().noquote() << QString("0x%1: 0x%2  %3")
                                  .arg(addr, 8, 16, QChar('0'))
                                  .arg(word, 8, 16, QChar('0'))
                                  .arg(rawBytes);
    }
    qDebug() << "--- End of Memory Dump ---\n";
}
*/


QString Memory::dump(uint32_t startAddress, uint32_t numBytes) const {
    QString result;
    QTextStream stream(&result);

    stream << "\n--- Memory Dump from "
           << QString("0x%1").arg(startAddress, 8, 16, QChar('0')) << " ---\n";

    for (uint32_t i = 0; i < numBytes; i += 4) {
        uint32_t addr = startAddress + i;
        if (addr + 3 >= MEM_SIZE) break;

        uint32_t word = read32(addr);

        QString rawBytes = QString("(%1 %2 %3 %4)")
                               .arg(data[addr], 2, 16, QChar('0'))
                               .arg(data[addr+1], 2, 16, QChar('0'))
                               .arg(data[addr+2], 2, 16, QChar('0'))
                               .arg(data[addr+3], 2, 16, QChar('0'));

        stream << QString("0x%1: 0x%2  %3\n")
                      .arg(addr, 8, 16, QChar('0'))
                      .arg(word, 8, 16, QChar('0'))
                      .arg(rawBytes);
    }

    stream << "--- End of Memory Dump ---\n";
    qDebug()<<result;
    return result;
}
