#ifndef DECODEDINSTRUCTION_H
#define DECODEDINSTRUCTION_H

#include <cstdint>

#include<iostream>

#include <QDebug>

typedef enum inst {
    add,
    sub,
    _xor,
    _or,
    _and,
    sll,
    srl,
    sra,
    slt,
    sltu,
    addi,
    ori,
    andi,
    slli,
    srli,
    lb,
    lh,
    lw,
    lbu,
    lhu,
    sb,
    sh,
    sw,
    beq,
    bne,
    blt,
    bge,
    bltu,
    bgeu,
    jal,
    jalr,
    lui,
    auipc,
    mul,
    mulh,
    mulhsu,
    mulhu,
    _div,
    divu,
    rem,
    remu,
    invalid

}inst;


struct DecodedInstruction {
    inst opcode=invalid;
    uint8_t rd=0;
    uint8_t rs1=0;
    uint8_t rs2=0;
    int32_t immediate=0;
    bool has_rd=false;
    bool has_rs1=false;
    bool has_rs2=false;
    bool has_imm=false;



    void print() const {
        qDebug().noquote() << QString("   Decoded -> Opcode: %1, rd: %2, rs1: %3, rs2: %4, imm: 0x%5")
                                  .arg(static_cast<int>(opcode))
                                  .arg(rd)
                                  .arg(rs1)
                                  .arg(rs2)
                                  .arg(static_cast<uint32_t>(immediate), 8, 16, QChar('0'));
    }
};

#endif // DECODEDINSTRUCTION_H
