#!/usr/bin/env python3

# Sceleton provided by the teachers of the course Computer Architecture
# (TDT4260) at Norwegian University of Science and Technology (NTNU), in 2024.
# However heavily modified to fit our needs.

# Kicking off the prefetcher simulations and gathering a summary of the stats.

import os
import shutil
import sys
import subprocess

cwd = os.getcwd()
gem5_root = os.path.abspath("../../..")
gem5_bin = f"{gem5_root}/build/X86/gem5.opt"
config = f"{gem5_root}/configs/tdt4260/prefetcher.py"

# ["gcc_s", "exchange2_s", "mcf_s", "deepsjeng_s", "x264_s", "imagick_s"]
# 1 ... only "gcc_s"
# 2 ... "gcc_s", "exchange2_s"
# etc.
# in `configs/tdt4260/commands.txt` the order can be changed, mind to change
# the order in b_names list either
num_benchmarks = 2
rrSize = 64
roundMax = 100
badScore = 1

for scoreMax in range(1, roundMax): 

    for x in range(num_benchmarks):
        os.chdir("spec2017")
        output_dir = f"prefetcher_out_{x}"
        if (os.path.exists(f"prefetcher_out_{x}")):
            shutil.rmtree(f"prefetcher_out_{x}")
        subprocess.run([gem5_bin, "-v", "-r", f"--outdir={output_dir}", config,
                        "--iteration", str(x), 
                        "--scoreMax", str(scoreMax),
                        "--roundMax", str(roundMax),
                        "--rrSize", str(rrSize),
                        "--badScore", str(badScore)])
        os.chdir(cwd)

    result_dst = f"results/RRsize:{str(rrSize)}_scoreMax:{str(scoreMax)}_roundMax:{str(roundMax)}.txt"

    ipcs = []
    others = []
    for x in range(num_benchmarks):
        data = []
        with open(f"spec2017/prefetcher_out_{x}/stats.txt", "r") as stats:
            lines = stats.read().split("\n")
            for line in lines:
                line = line.split()
                if (len(line) == 0):
                    continue
                if "ipc" in line[0]:
                    ipcs.append(line[1])
                if "prefetcher" in line[0]:
                    data.append((line[0], line[1]))
        others.append(data)

    b_names = ["gcc_s", "exchange2_s", "mcf_s", "deepsjeng_s", "x264_s", "imagick_s"]

    with open(result_dst, "w") as out:
        out.write("Summary of benchmarking follows...\n")
        out.write("-----IPC-----\n")
        for x in range(len(ipcs)):
            out.write(f"{b_names[x]}: {ipcs[x]}\n")

        out.write("\n\n\n")

        out.write("Detailed stats follow...\n")
        for x in range(len(others)):
            out.write(f"{b_names[x]} stats:\n")
            for y in others[x]:
                out.write(f"{y[0]}: {y[1]}\n")
            out.write("\n\n\n")


