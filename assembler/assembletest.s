li x6, 0x0F0F0F0F   # x6 = 0x0F0F0F0F
li x7, 0x00FF00FF   # x7 = 0x00FF00FF
xor x8, x6, x7      # x8 = xor of x6 and x7
or  x9, x6, x7      # x9 = or
and x10, x6, x7     # x10 = and
not x11, x10        # x11 = ~x10 (xori with -1)
