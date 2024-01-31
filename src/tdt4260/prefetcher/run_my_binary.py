#!/usr/bin/env python3
import os
import shutil
import sys
import subprocess

cwd = os.getcwd()
gem5_root = os.path.abspath("../../..")
gem5_bin = f"{gem5_root}/build/X86/gem5.opt"
config = f"{gem5_root}/configs/tdt4260/prefetcher_my_bin.py"

my_binary = "hello"
my_options = [] # start with "-o" if used

os.chdir("my_binaries")
output_dir = f"prefetcher_out_{my_binary}"
if (os.path.exists(output_dir)):
    shutil.rmtree(output_dir)

subprocess.run([gem5_bin, "-r", f"--outdir={output_dir}", config,
                "-c", my_binary])
os.chdir(cwd)



        
