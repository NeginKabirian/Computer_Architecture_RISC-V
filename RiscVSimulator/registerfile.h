#ifndef REGISTERFILE_H
#define REGISTERFILE_H
#include <array>
#include <cstdint>
#include <QObject>

class RegisterFile  : public QObject{
    Q_OBJECT
private:
    std::array<uint32_t, 32> regs;

public:
    RegisterFile();

    uint32_t read(uint8_t index) const;

    void write(uint8_t index, uint32_t value);

    void reset();

    const std::array<uint32_t, 32>& getAll() const;

    void printRegisters();

signals:
    void registerChanged(int index, QString content);
};


#endif // REGISTERFILE_H
