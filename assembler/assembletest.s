    li x1, 10
    li x2, 10
    add x3, x1, x2
    sub x4, x2, x1
    xor x5, x1, x2
    or  x6, x1, x2
    and x7, x1, x2
    sll x8, x1, x0
    srl x9, x2, x1
    sra x10, x2, x1
    slt x11, x1, x2
    sltu x12, x1, x2
    mul x13, x1, x2
    mulh x14, x1, x2
    div x15, x2, x1
    rem x16, x2, x1
    addi x17, x1, 5
    lh x18, 0(x1)
    lw x19, 0(x1)
    sw x2, 4(x1)
    sh x2, 6(x1)
    beq x1, x2, fail
    bne x1, x2, skip
fail:
    li x20, 1
skip:
    blt x1, x2, less
    li x21, 1
less:
    bge x2, x1, geq
    li x22, 1
geq:
    bltu x1, x2, lessu
    li x23, 1
lessu:
    bgeu x2, x1, end
    li x24, 1
end:
    jal x25, done
    li x26, 1
done:
    jalr x0, 0(x0)

    .org 0x1000
    .word 0x12345678
