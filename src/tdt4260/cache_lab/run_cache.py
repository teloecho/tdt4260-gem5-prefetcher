import os
import sys

gem5 = os.path.abspath("../../..")
gem5_bin = os.path.abspath(f"{gem5}/X86/gem5.opt")

config = os.path.abspath(f"{gem5}/configs/tdt4260/cache_lab.py")

run_ref = f"{gem5_bin} {config}"
os.system(run_ref)