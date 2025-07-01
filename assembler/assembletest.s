li x1, 0xAAAA5555
li x2, 0x12345678
xor x3, x1, x2
or  x4, x1, x2
and x5, x1, x2
sll x6, x5, x0        # shift by 0 = no effect
sra x7, x1, x0        # shift right by 0 = no effect
