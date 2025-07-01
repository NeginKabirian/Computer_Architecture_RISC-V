    li x1, 0         # x1 = 0
    jal x5, jump     
    li x1, 999       

jump:
    li x1, 123
    jalr x0, 0(x5)   

    li x2, 77        
