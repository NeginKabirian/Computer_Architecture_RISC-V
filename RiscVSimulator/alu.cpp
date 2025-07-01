#include "alu.h"



uint32_t ALU::add(uint32_t a, uint32_t b) { return a + b; }

uint32_t ALU::sub(uint32_t a, uint32_t b) { return a - b; }

uint32_t ALU::and_op(uint32_t a, uint32_t b) { return a & b; }

uint32_t ALU::or_op(uint32_t a, uint32_t b) { return a | b; }

uint32_t ALU::xor_op(uint32_t a, uint32_t b) { return a ^ b; }

uint32_t ALU::sll(uint32_t a, uint32_t b) { return a << b; }

uint32_t ALU::srl(uint32_t a, uint32_t b) { return a >> b; }

uint32_t ALU::sra(uint32_t a, uint32_t b) {
    int32_t signed_val = static_cast<int32_t>(a);
    return static_cast<uint32_t>(signed_val >> b);
}

uint32_t ALU::passThrough(uint32_t val) {
    return val;
}

uint32_t ALU::multiply(uint32_t a, uint32_t b) {  return a * b; }
