#include "cpu.h"

void CPU::fetch() {
    AR = PC;
    IR = memory->read32(AR);
    PC += 4;
}

DecodedInstruction CPU::decode(uint32_t instruction) {
    DecodedInstruction di;

    uint8_t opcode = instruction & 0x7F;
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t funct3 = (instruction >> 12) & 0x07;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint8_t funct7 = (instruction >> 25) & 0x7F;

    // R type instructions(FMT=R)
    if (opcode == 0b0110011) {

        di.rd = rd;
        di.rs1 = rs1;
        di.rs2 = rs2;

        switch (funct3) {
        case 0x0: {
            if (funct7 == 0x00)
                di.opcode = inst::add;
            else if (funct7 == 0x20)
                di.opcode = inst::sub;
            else if (funct7 == 0x01)
                di.opcode = inst::mul;

            break;
        }
        case 0x1: {
            if (funct7 == 0x00)
                di.opcode = inst::sll;
            else if (funct7 == 0x01)
                di.opcode = inst::mulh;
            break;
        }
        case 0x2: {
            if (funct7 == 0x00)
                di.opcode = inst::slt;
            else if (funct7 == 0x01)
                di.opcode = inst::mulhsu;
            break;
        }
        case 0x3: {
            if (funct7 == 0x00)
                di.opcode = inst::sltu;
            else if (funct7 == 0x01)
                di.opcode = inst::mulhu;
            break;
        }
        case 0x4: {
            if (funct7 == 0x00)
                di.opcode = inst::_xor;
            else if (funct7 == 0x01)
                di.opcode = inst::div;
            break;
        }
        case 0x5: {
            if (funct7 == 0x00)
                di.opcode = inst::srl;
            else if (funct7 == 0x20)
                di.opcode = inst::sra;
            else if (funct7 == 0x01)
                di.opcode = inst::divu;
            break;
        }
        case 0x6: {
            if (funct7 == 0x00)
                di.opcode = inst::_or;
            else if (funct7 == 0x01)
                di.opcode = inst::rem;
            break;
        }
        case 0x7: {
            if (funct7 == 0x00)
                di.opcode = inst::_and;
            else if (funct7 == 0x01)
                di.opcode = inst::remu;
            break;
        }
        default:
            break;
        }
    }
    // I type instructions(FMT=I)
    else if (opcode == 0b0010011) {
        di.rd = rd;
        di.rs1 = rs1;
        di.immediate = static_cast<int32_t>(instruction) >> 20;
        switch (funct3) {
        case 0x0: {
            di.opcode = inst::addi;
            break;
        }
        case 0x1: {
            di.opcode = inst::slli;
            break;
        }
        case 0x6: {
            di.opcode = inst::srli;
            break;
        }
        case 0x7: {
            di.opcode = inst::andi;
            break;
        }

        default:
            break;
        }
    }
    // I type instructions(FMT=I)
    else if (opcode == 0b0000011) {
        di.rd = rd;
        di.rs1 = rs1;
        di.immediate = static_cast<int32_t>(instruction) >> 20;
        switch (funct3) {
        case 0x00: {
            di.opcode = inst::lb;
            break;
        }
        case 0x01: {
            di.opcode = inst::lh;
            break;
        }
        case 0x02: {
            di.opcode = inst::lw;
            break;
        }
        case 0x04: {
            di.opcode = inst::lbu;
            break;
        }
        case 0x05: {
            di.opcode = inst::lhu;
            break;
        }
        default:
            break;
        }
    }

    // I type instructions(FMT=I)
    else if (opcode == 0b1100111) {
        di.rd = rd;
        di.rs1 = rs1;
        di.immediate = static_cast<int32_t>(instruction) >> 20;
        if (funct3 == 0x00)
            di.opcode = inst::jalr;
    }

    // B type instructions(FMT=B)
    else if (opcode == 0b1100011) {
        int32_t imm = 0;
        imm |= ((instruction >> 31) & 0x1) << 12;
        imm |= ((instruction >> 7) & 0x1) << 11;
        imm |= ((instruction >> 25) & 0x3F) << 5;
        imm |= ((instruction >> 8) & 0xF) << 1;
        if (imm & (1 << 12))
            imm |= 0xFFFFE000; // sign-extend
        di.immediate = imm;

        di.opcode = (funct3 == 0b000)   ? inst::beq
                    : (funct3 == 0b001) ? inst::bne
                    : (funct3 == 0b100) ? inst::blt
                    : (funct3 == 0b101) ? inst::bge
                                        : inst::invalid;
    }

    // S type instructions(FMT=S)
    else if (opcode == 0b0100011) {
        int32_t imm = ((instruction >> 25) << 5) | ((instruction >> 7) & 0x1F);
        if (imm & (1 << 11)) // sign-extend
            imm |= 0xFFFFF000;
        di.immediate = imm;

        di.opcode = (funct3 == 0b000)   ? inst::sb
                    : (funct3 == 0b001) ? inst::sh
                    : (funct3 == 0b010) ? inst::sw
                                        : inst::invalid;

    }

    // U type instructions(FMT=U)
    else if (opcode == 0b0110111) {
        di.immediate = instruction & 0xFFFFF000;
        di.opcode = inst::lui;
    }

    // U type instructions(FMT=U)
    else if (opcode == 0b0010111) {
        di.immediate = instruction & 0xFFFFF000;
        di.opcode = inst::auipc;
    }

    // J type instructions(FMT=J)
    else if (opcode == 0b1101111) {
        int32_t imm = 0;
        imm |= ((instruction >> 31) & 0x1) << 20;
        imm |= ((instruction >> 12) & 0xFF) << 12;
        imm |= ((instruction >> 20) & 0x1) << 11;
        imm |= ((instruction >> 21) & 0x3FF) << 1;
        if (imm & (1 << 20))
            imm |= 0xFFF00000; // sign-extend
        di.immediate = imm;
        di.opcode = inst::jal;
    }

    return di;
}
