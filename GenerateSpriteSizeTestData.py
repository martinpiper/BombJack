nybbleToScreenCode = ["30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "01", "02", "03", "04", "05", "06"]
           
for hi in range (0,16):
    for lo in range (0,16):
        print("s$9a000100")
        print("b$1" + hex(lo)[2:] + ",b$0" + hex(hi)[2:])

        print("d$904101" + nybbleToScreenCode[lo])
        print("d$904301" + nybbleToScreenCode[hi])
        
        print("d$0")
        print("^-$01")
        print("d$0")

