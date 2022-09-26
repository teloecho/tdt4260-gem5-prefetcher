#!/usr/bin/env python3
import re
import subprocess

def run_workload(exectuable, arguments=None, l1CacheSize=16384,
                 l1CacheAssociativity=1):
    if not arguments:
        arguments = ""
    elif not isinstance(arguments, str):
        arguments = " ".join(str(a) for a in arguments)
    subprocess.run(["./build/X86/gem5.opt", "configs/tdt4260/cache_lab.py",
                    "--l1CacheSize", str(l1CacheSize),
                    "--l1CacheAssociativity", str(l1CacheAssociativity),
                    "--cmd", exectuable,
                    "--options", arguments])
def parse_stats():
    with open("m5out/stats.txt") as f:
        lines = f.readlines()
    stats = []
    for line in lines:
        if line == "" or line == "\n":
            continue
        elif line.startswith("---------- Begin "):
            snapshot = {}
        elif line.startswith("---------- End "):
            stats.append(snapshot)
        elif match := re.match(r"(\S+)\s+([0-9.e]+).*", line):
            assert match.group(1) not in snapshot
            snapshot[match.group(1)] = float(match.group(2))
        else:
            print("unparsed stat line:", "'"+line+"'")
            assert False
    return stats

def check_test_associativety_stats(cache_size_bytes, cache_associativety,
                                   stats):
    BYTES_PER_LINE = 64
    cache_num_lines = cache_size_bytes//BYTES_PER_LINE
    def hits(stat):
        return int(stat['system.cpu.cache_hierarchy.L1_Cache.reqsServiced'])
    def misses(stat):
        return int(stat['system.cpu.cache_hierarchy.L1_IC.reqsReceived'])
    def loads(stat):
        return int(stat['system.cpu.exec_context.thread_0.numLoadInsts'])
    def stores(stat):
        return int(stat['system.cpu.exec_context.thread_0.numStoreInsts'])

    # we expect cache_num_lines misses
    assert loads(stats[0]) == 0
    assert stores(stats[0]) == cache_num_lines
    assert misses(stats[0]) == cache_num_lines

    # we expect cache_num_lines hits
    assert loads(stats[1]) == cache_num_lines
    assert stores(stats[1]) == 0
    assert hits(stats[1]) == cache_num_lines

    # we expect cache_num_lines misses
    assert loads(stats[2]) == 0
    assert stores(stats[2]) == cache_num_lines
    assert misses(stats[2]) == cache_num_lines

    # we expect cache_num_lines misses
    assert loads(stats[3]) == cache_num_lines
    assert stores(stats[3]) == 0
    assert misses(stats[3]) == cache_num_lines

    # we expect cache_size_bytes hits
    assert loads(stats[4]) == cache_size_bytes
    assert stores(stats[4]) == 0
    assert hits(stats[4]) == cache_size_bytes

    # we expect cache_num_lines misses
    assert loads(stats[5]) == cache_num_lines
    assert stores(stats[5]) == 0
    assert misses(stats[5]) == cache_num_lines

    if cache_associativety != 1:
        # we expect cache_associativety-1 hits
        assert loads(stats[6]) == cache_associativety-1
        assert stores(stats[6]) == 0
        assert hits(stats[6]) == cache_associativety-1

        # we expect 1 miss
        assert loads(stats[7]) == 1
        assert stores(stats[7]) == 0
        assert misses(stats[7]) == 1

        # we expect 1 hit
        assert loads(stats[8]) == 0
        assert stores(stats[8]) == 1
        assert hits(stats[8]) == 1

        # we expect 1 hit
        assert loads(stats[9]) == 1
        assert stores(stats[9]) == 0
        assert hits(stats[9]) == 1

        # we expect 1 miss
        assert loads(stats[10]) == 1
        assert stores(stats[10]) == 0
        assert misses(stats[10]) == 1


def test_associativety():
    for associativety in [
        1,
        2, 4, 8, 16]:
        for cache_size in [1024, 2048, 4096, 8192, 16384, 32768, 65536]:
            run_workload("src/tdt4260/cache_lab/programs/test_associativety/test_associativety",
                [cache_size, associativety], cache_size, associativety)
            stats = parse_stats()
            check_test_associativety_stats(cache_size, associativety, stats)


if __name__ == '__main__':
    test_associativety()