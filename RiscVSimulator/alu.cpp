#include "alu.h"



uint32_t ALU::add(uint32_t a, uint32_t b) { return a + b; }

uint32_t ALU::sub(uint32_t a, uint32_t b) { return a - b; }

uint32_t ALU::and_op(uint32_t a, uint32_t b) { return a & b; }

uint32_t ALU::or_op(uint32_t a, uint32_t b) { return a | b; }

uint32_t ALU::xor_op(uint32_t a, uint32_t b) { return a ^ b; }

uint32_t ALU::sll(uint32_t a, uint32_t b) { return a << b; }

uint32_t ALU::srl(uint32_t a, uint32_t b) { return a >> b; }
