import os

path = os.path.abspath(os.path.dirname(__file__))

class Benchmark:

    def __init__(self, name, bin, opt, ckpt_inst):
        self.name = name
        self.bin = bin
        self.opt = opt
        self.ckpt_inst = ckpt_inst


benchmarks = []

with open(f"{path}/commands.txt", "r") as commands:
    lines = commands.read().split("\n")
    for command in lines:
        bin = command.split(" ", 1)[0][2:]
        opt = command.split(" ", 1)[1].split(";")[0]
        ckpt_inst = command.split(";")[1]
        benchmarks.append(Benchmark(bin, bin, opt, ckpt_inst))


