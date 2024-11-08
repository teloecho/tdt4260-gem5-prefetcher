# Sceleton provided by the teachers of the course Computer Architecture
# (TDT4260) at Norwegian University of Science and Technology (NTNU), in 2024.
# However heavily modified to fit our needs.

# Defining the system configuration to be used for a simulation run. Some
# prefetcher parameters can be set using commandline arguments, e.g. scoreMax,
# roundMax, badScore and rrSize.

import argparse
import sys
import os
import shutil

import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.objects.TDT4260Cache import BaseCacheHierarchy
from m5.params import NULL
from m5.util import addToPath, fatal, warn

addToPath('../')

from common import Options, Simulation, CacheConfig,\
                   CpuConfig, ObjectList, MemConfig
from common.FileSystemConfig import config_filesystem
from common.Caches import *

from benchmarks import benchmarks

parser = argparse.ArgumentParser()
parser.add_argument("--scoreMax", type=int, default=8)
parser.add_argument("--roundMax", type=int, default=16)
parser.add_argument("--badScore", type=int, default=1)
parser.add_argument("--rrSize", type=int, default=64)
Options.addCommonOptions(parser)
Options.addSEOptions(parser)

args = parser.parse_args()


iteration = args.iteration
for x in benchmarks:
    print(x.bin)
benchmark = benchmarks[iteration]

#args.maxinsts = str(int(benchmark.ckpt_inst) + 100000000)
args.maxinsts = 10000000

#args.take_checkpoints = benchmark.ckpt_inst
#args.at_instruction = True

args.checkpoint_restore = 1

#args.cpu_type = "AtomicSimpleCPU"
args.cpu_type = "O3CPU"
args.mem_type = "DDR4_2400_8x8"

args.caches = True
args.l2cache = True
args.l3cache = True

args.l1d_size = "48KiB"
args.l1d_assoc = 12
#args.l1d_hwp_type = "TDTPrefetcher"

args.l1i_size = "32KiB"
#args.l1i_hwp_type = "TDTPrefetcher"

args.l2_size = "1280KiB" #1.25MiB
args.l2_assoc = 20
args.l2_hwp_type = "BestOffsetPrefetcher"

args.l3_size = "3MiB"
args.l3_assoc = 12
#args.l3_hwp_type = "BestOffsetPrefetcher"


num_cpus = 1

(cpu, mem, futureclass) = Simulation.setCPUClass(args)
cpu.numThreads = 1

# in case of warning in simout file:
# build/X86/mem/dram_interface.cc:690: warn: DRAM device capacity
# (16384 Mbytes) does not match the address range assigned (8192 Mbytes)
# changing mem_ranges = [AddrRange("8GB")] does _not_ help!
# instead see README.md for how to increase docker memory limit
system = System(cpu = [cpu(cpu_id=0)],
                mem_mode = mem,
                mem_ranges = [AddrRange("8GB")],
                cache_line_size = "64")

system.voltage_domain = VoltageDomain(voltage = "3.3V")

system.clk_domain = SrcClockDomain(clock = "3GHz",
                                   voltage_domain = system.voltage_domain)

system.cpu_voltage_domain = VoltageDomain()

system.cpu_clk_domain = SrcClockDomain(clock = "3GHz",
                                voltage_domain = system.cpu_voltage_domain)


system.cpu[0].clk_domain = system.cpu_clk_domain


MemClass = Simulation.setMemClass(args)
system.membus = SystemXBar()
system.system_port = system.membus.cpu_side_ports
CacheConfig.config_cache(args, system)
MemConfig.config_mem(args, system)
config_filesystem(system, args)


process = Process(pid = 100)

wrkld = benchmark.bin
process.executable = wrkld

process.cwd = os.getcwd()
process.gid = os.getgid()
process.cmd = [wrkld] + benchmark.opt.split()

mp0_path = process.executable
system.cpu[0].dcache.prefetcher.table_assoc = 16
system.cpu[0].icache.prefetcher.table_assoc = 16
#system.l2.prefetcher.table_assoc = 16

# L2 prefetcher params
system.l2.prefetcher.scoreMax = args.scoreMax
system.l2.prefetcher.roundMax = args.roundMax
system.l2.prefetcher.badScore = args.badScore
system.l2.prefetcher.rrSize = args.rrSize

#system.l3.prefetcher.table_assoc = 16
system.cpu[0].workload = process
system.cpu[0].createThreads()


#print(os.listdir())
src = f"spec2017_cpts/{benchmark.name}-cpt"
dst = f"prefetcher_out_{iteration}/cpt.1"
shutil.copytree(src, dst)


system.workload = SEWorkload.init_compatible(mp0_path)

print(f"Simulating gem5 with custom configuration, "\
      f"bin: {benchmark.bin}, "\
      f"cpu: {args.cpu_type}, "\
      f"max_inst: {args.maxinsts}\n")
root = Root(full_system = False, system = system)
Simulation.run(args, root, system, futureclass)
