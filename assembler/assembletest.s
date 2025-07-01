
    li x1, 0x00000010     # x1 = 16
    li x2, 0x00000008     # x2 = 8
    bgeu x1, x2, label   
    li x3, 0              
    j end
label:
    li x3, 1             
end:
