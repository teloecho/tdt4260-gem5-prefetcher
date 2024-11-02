scons -j9 build/X86/gem5.opt
cd src/tdt4260/prefetcher
python3 run_prefetcher.py
cd ../../../