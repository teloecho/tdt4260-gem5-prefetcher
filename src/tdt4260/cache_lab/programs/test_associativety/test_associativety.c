#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define BYTES_PER_LINE 64

// arcane black magic to get gem5 to reset and dump stats. also acts like memory barrier for the compiler
#define GEM5_DUMPSTATS  __asm__ __volatile__ (".word 0x040F; .word 0x0041;" : : "D" (0), "S" (0) :"memory")
#define GEM5_RESETSTATS __asm__ __volatile__ (".word 0x040F; .word 0x0040;" : : "D" (0), "S" (0) :"memory")
/* For ghidra disassembly to work add the following to Ghidra/Processors/x86/data/languages/ia.sinc :
define pcodeop Gem5DumpStats;
:GEM5_DUMPSTATS          is byte=0x0F; byte=0x04; byte=0x41; byte=0x00 { Gem5DumpStats(); }

define pcodeop Gem5ResetStats;
:GEM5_RESETSTATS          is byte=0x0F; byte=0x04; byte=0x40; byte=0x00 { Gem5ResetStats(); }
 */

#define IS_POW2(x) (__builtin_popcountl((x))==1)

typedef struct
{
    u_int64_t data;
    u_int64_t garbage[7];
} cache_line;


void * get_alligned_array(u_int64_t size){
    // allocate an array and select a point that is aligned to size.
    void * data = malloc(size*2);
    return data + size - (((u_int64_t)data)%size);
}

int
main(int argc, char *argv[])
{
    assert(argc==3);
    u_int64_t cache_size_bytes = atol(argv[1]);
    assert(IS_POW2(cache_size_bytes));
    assert(cache_size_bytes>=BYTES_PER_LINE);
    u_int64_t cache_num_lines = cache_size_bytes/BYTES_PER_LINE;
    u_int64_t cache_associativety = atol(argv[2]);
    assert(IS_POW2(cache_associativety));
    assert(cache_num_lines>=cache_associativety);
    u_int64_t cache_num_sets = cache_num_lines/cache_associativety;

    // create two arrays that are as big as and aligned to the cache size, i.e. alias
    cache_line * data_array = get_alligned_array(cache_size_bytes);
    cache_line * flush_array = get_alligned_array(cache_size_bytes);

    // go over the arrays once so there are no page faults, which could lead to repeated instructions
    for (u_int64_t i = 0; i < cache_num_lines; ++i) {
        data_array[i].data = 0;
    }
    for (u_int64_t i = 0; i < cache_num_lines; ++i) {
        flush_array[i].data = 0;
    }

    u_int64_t sum1 = 0;
    GEM5_RESETSTATS;
    // write to all cache lines in data_array - we expect cache_num_lines misses
    for (u_int64_t i = 0; i < cache_num_lines; ++i) {
        data_array[i].data = i;
    }
    GEM5_DUMPSTATS;
    GEM5_RESETSTATS;
    // access each cache line in data_array - we expect cache_num_lines hits
    for (u_int64_t i = 0; i < cache_num_lines; ++i) {
        sum1 += data_array[i].data;
    }
    GEM5_DUMPSTATS;
    GEM5_RESETSTATS;
    // write to all cache lines in flush_array - we expect cache_num_lines misses
    for (u_int64_t i = 0; i < cache_num_lines; ++i) {
        flush_array[i].data = i;
    }
    GEM5_DUMPSTATS;
    GEM5_RESETSTATS;
    // access each cache line in data_array - we expect cache_num_lines misses
    for (u_int64_t i = 0; i < cache_num_lines; ++i) {
        sum1 += data_array[i].data;
    }
    GEM5_DUMPSTATS;
    GEM5_RESETSTATS;
    // access each byte in data_array - we expect cache_size_bytes hits
    u_int8_t * data_array_bytes = (u_int8_t *)data_array;
    for (u_int64_t i = 0; i < cache_size_bytes; ++i) {
        sum1 += data_array_bytes[i];
    }
    GEM5_DUMPSTATS;
    GEM5_RESETSTATS;
    // access each cache line in flush_array - we expect cache_num_lines misses
    for (u_int64_t i = 0; i < cache_num_lines; ++i) {
        sum1 += flush_array[i].data;
    }
    GEM5_DUMPSTATS;
    if (cache_associativety!=1){
        // access all ways of the first set
        for (u_int64_t i = 0; i < cache_associativety; ++i) {
            sum1 += data_array[i*cache_num_sets].data;
        }
        // access aliasing element from flush array
        sum1 += flush_array[0].data;
        GEM5_RESETSTATS;
        // access all but the first way - they should still be in the cache - we expect cache_associativety-1 hits
        for (u_int64_t i = 1; i < cache_associativety; ++i) {
            sum1 += data_array[i*cache_num_sets].data;
        }
        GEM5_DUMPSTATS;
        GEM5_RESETSTATS;
        // access first way - we expect 1 miss
        sum1 += data_array[0].data;
        GEM5_DUMPSTATS;
        // access all ways of the first set
        for (u_int64_t i = 0; i < cache_associativety; ++i) {
            sum1 += data_array[i*cache_num_sets].data;
        }
        GEM5_RESETSTATS;
        // write first way, to check if lru updates on writes - we expect 1 hit
        data_array[0].data = 123;
        GEM5_DUMPSTATS;
        // access aliasing element from flush array
        sum1 += flush_array[0].data;
        GEM5_RESETSTATS;
        // access first way - we expect 1 hit
        sum1 += data_array[0].data;
        GEM5_DUMPSTATS;
        GEM5_RESETSTATS;
        // access second way - we expect 1 miss
        sum1 += data_array[cache_num_sets].data;
        GEM5_DUMPSTATS;
        GEM5_RESETSTATS;
    }
    assert(sum1>0);
    return 0;
}
