# Prefetcher Lab
This is our implementation, plotting code and report for the prefetcher lab of the
course [computer architecture (TDT4260) at NTNU](https://www.ntnu.edu/studies/courses/TDT4260/2023).

The goal is to research on, understand and implement a self-chosen prefetcher.
We dig into [Best-Offset-Prefetching as presented by Michaud](https://inria.hal.science/hal-01254863v1).

The prefetcher header and implementation can be found in
[best_offset.hh](/src/mem/cache/prefetch/best_offset.hh) and
[best_offset.cc](/src/mem/cache/prefetch/best_offset.cc) respectively.
Our developemnt starts on an adapted gem5 version.
Our teachers provide the [prepared version of the gem5 repository](https://github.com/davidmetz/gem5-tdt4260),
leaving only basic prefetchers in `src/men/cache/prefetch/`. There our develoments start. See commit history.

Plotting logic for our measuements can be found in the [plot jupyter notebook](/_Plots_for_report/plot.ipynb).

Our [report](/report_prefechter_lab_bo/report_bop.pdf) describes the background of prefetching in general,
our implementation and our performance measuement results compared to less sophisticated or no prefetching.

# Getting started

https://www.gem5.org/documentation/general_docs/building
```bash
# docker pull ghcr.io/gem5/ubuntu-24.04_all-dependencies:v24-0
# docker run -u $UID:$GID --volume ./:/gem5 --rm -it ghcr.io/gem5/ubuntu-24.04_all-dependencies:v24-0
docker pull ghcr.io/gem5/ubuntu-22.04_all-dependencies:v23-0
docker run -u $UID:$GID --volume ./:/gem5 --rm -it ghcr.io/gem5/ubuntu-22.04_all-dependencies:v23-0
```

Inside the docker container run
```bash
cd /gem5
scons -j9 build/X86/gem5.opt
```

Download the teachers provided excerpt of the [spec2017](https://static.teloecho.eu/tdt4260/spec2017.tar.xz)
benchmark suit ("gcc_s", "exchange2_s", "mcf_s", "deepsjeng_s", "x264_s", "imagick_s").
And extract it to `src/tdt4260/prefetcher/spec2017`.
See also [README/task description of prefetcher lab](/src/tdt4260/README.md).

spec2017.tar.xz: [sha256sum](https://static.teloecho.eu/tdt4260/spec2017.tar.xz.sha256sum) `aae165e54e7144463ff2017ca6c5883c90791ca4395681b84e2f00c312628551`

Then run the simulator with the benchmarks using the `run_prefetcher.py` script. (Attention currently there are _a lot_ of simulations run after another such that it might take a hole night.) If you see `Redirecting stdout and stderr to prefetcher_out_0/simout` the second or third time, you can be somewhat sure, everything kind of works, ctrl+c,c,c,c... to stop it. Modify the script to just run a selection of configs.
```bash
cd src/tdt4260/prefetcher
python3 run_prefetcher.py
```

-----
Sorry for the doubled readme files (`README.md` and `README`).
Gem5 depends on the `README` file to exist for successful builds.
# Gem5 Origianl README

This is the gem5 simulator.

The main website can be found at http://www.gem5.org

A good starting point is http://www.gem5.org/about, and for
more information about building the simulator and getting started
please see http://www.gem5.org/documentation and
http://www.gem5.org/documentation/learning_gem5/introduction.

To build gem5, you will need the following software: g++ or clang,
Python (gem5 links in the Python interpreter), SCons, zlib, m4, and lastly
protobuf if you want trace capture and playback support. Please see
http://www.gem5.org/documentation/general_docs/building for more details
concerning the minimum versions of these tools.

Once you have all dependencies resolved, type 'scons
build/<CONFIG>/gem5.opt' where CONFIG is one of the options in build_opts like
ARM, NULL, MIPS, POWER, SPARC, X86, Garnet_standalone, etc. This will build an
optimized version of the gem5 binary (gem5.opt) with the the specified
configuration. See http://www.gem5.org/documentation/general_docs/building for
more details and options.

The main source tree includes these subdirectories:
   - build_opts: pre-made default configurations for gem5
   - build_tools: tools used internally by gem5's build process.
   - configs: example simulation configuration scripts
   - ext: less-common external packages needed to build gem5
   - include: include files for use in other programs
   - site_scons: modular components of the build system
   - src: source code of the gem5 simulator
   - system: source for some optional system software for simulated systems
   - tests: regression tests
   - util: useful utility programs and files

To run full-system simulations, you may need compiled system firmware, kernel
binaries and one or more disk images, depending on gem5's configuration and
what type of workload you're trying to run. Many of those resources can be
downloaded from http://resources.gem5.org, and/or from the git repository here:
https://gem5.googlesource.com/public/gem5-resources/

If you have questions, please send mail to gem5-users@gem5.org

Enjoy using gem5 and please share your modifications and extensions.
