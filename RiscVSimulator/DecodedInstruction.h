#ifndef DECODEDINSTRUCTION_H
#define DECODEDINSTRUCTION_H

#include <cstdint>





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
};

#endif // DECODEDINSTRUCTION_H
