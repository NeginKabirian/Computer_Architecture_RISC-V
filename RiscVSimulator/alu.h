#ifndef ALU_H
#define ALU_H

#include <cstdint>
struct ALUFlags {
    bool zero = false;
    bool carry = false;
    bool negative = false;
    bool overflow = false;
};
class ALU {
public:
    ALUFlags flags;
    uint32_t add(uint32_t a, uint32_t b);
    uint32_t sub(uint32_t a, uint32_t b);
    uint32_t and_op(uint32_t a, uint32_t b);
    uint32_t or_op(uint32_t a, uint32_t b);
    uint32_t xor_op(uint32_t a, uint32_t b);
    uint32_t sll(uint32_t a, uint32_t b);
    uint32_t srl(uint32_t a, uint32_t b);
    uint32_t sra(uint32_t a, uint32_t b);
    //...
};

#endif // ALU_H
