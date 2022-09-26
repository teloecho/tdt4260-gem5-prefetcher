

with open("simout", "r") as read, open("bits.txt", "w") as write:
    scale = 16
    num_of_bits = 58
    while (True):
        line = read.readline()
        if (not line):
            break
        line = line.strip()
        num = int(line, scale)
        num = num >> 6
        bits = bin(num)[2:].zfill(num_of_bits)
        write.write(bits)
        write.write("\n")

