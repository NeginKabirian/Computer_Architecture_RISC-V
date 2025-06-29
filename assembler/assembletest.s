	addi x5, x0, 10      
    add x7, x5, x5       
    sw x7, 0(x5)         
loop:
    beq x5, x0, loop     