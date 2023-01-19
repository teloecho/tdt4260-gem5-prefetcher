import os
import shutil
import sys

cwd = os.getcwd()
gem5_root = os.path.abspath("../../..")
gem5_bin = f"{gem5_root}/build/X86/gem5.opt"
config = f"{gem5_root}/configs/tdt4260/prefetcher_my_bin.py"

my_binary = "hello"
my_options = ""

os.chdir("my_binaries")
output_dir = f"prefetcher_out_{my_binary}"
if (os.path.exists(output_dir)):
    shutil.rmtree(output_dir)
run_ref = f"{gem5_bin} -r --outdir={output_dir} {config} -c {my_binary} "\
          f" {f'-o {my_options}' if len(my_options) > 0 else ''}"
os.system(run_ref)
os.chdir(cwd)



        
