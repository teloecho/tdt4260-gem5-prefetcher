// Copyright (c) 2022 Björn Gottschall
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <stdint.h>

#define KiB(x) (1024ULL * x)
#define MiB(x) (1024ULL * KiB(x))
#define GiB(x) (1024ULL * MiB(x))

#ifndef MEMORY_SIZE
#define MEMORY_SIZE ((uint64_t) MiB(64))
#endif

#ifndef CHASE_SIZE
#define CHASE_SIZE ((uint64_t) MiB(512))
#endif

#include <stdlib.h>
#include <sys/time.h>
static struct timeval tp;
static struct timezone tz;
#define myTime() ({                                                     \
            gettimeofday(&tp, &tz);                                     \
            (((uint64_t) tp.tv_sec * 1000000) + (uint64_t) tp.tv_usec); \
        })

#ifndef USE_HEAP
static uint64_t MEMORY_BASE[MEMORY_SIZE / sizeof(uint64_t)];
#endif
#define myRand() rand()

// LFSR implementation loosly based on ccbench's at
// https://github.com/ucb-bar/ccbench

typedef struct {
   uint32_t value;
   uint32_t width;
   uint32_t mask;
   uint32_t *taps;
} lfsr_t;

#define LFSR_MAX_WIDTH 32
#define LFSR_MAX_TAPS 4

// Sourced from https://docs.xilinx.com/v/u/en-US/xapp052
static uint32_t lfsr_taps[LFSR_MAX_WIDTH][LFSR_MAX_TAPS] = {
   { 0, -1, -1, -1}, // 1 Bit - Invalid
   { 1, -1, -1, -1}, // 2 Bit - Invalid
   { 2,  1, -1, -1}, // 3 Bit
   { 3,  2, -1, -1}, // 4 Bit
   { 4,  2, -1, -1}, // 5 Bit
   { 5,  4, -1, -1}, // 6 Bit
   { 6,  5, -1, -1}, // 7 Bit
   { 7,  5,  4,  3}, // 8 Bit
   { 8,  4, -1, -1}, // 9 Bit
   { 9,  6, -1, -1}, // 10 Bit
   {10,  8, -1, -1}, // 11 Bit
   {11,  5,  3,  0}, // 12 Bit
   {12,  3,  2,  0}, // 13 Bit
   {13,  4,  2,  0}, // 14 Bit
   {14, 13, -1, -1}, // 15 Bit
   {15, 14, 12,  3}, // 16 Bit
   {16, 13, -1, -1}, // 17 Bit
   {17, 10, -1, -1}, // 18 Bit
   {18,  5,  1,  0}, // 19 Bit
   {19, 16, -1, -1}, // 20 Bit
   {20, 18, -1, -1}, // 21 Bit
   {21, 20, -1, -1}, // 22 Bit
   {22, 17, -1, -1}, // 23 Bit
   {23, 22, 21, 16}, // 24 Bit
   {24, 21, -1, -1}, // 25 Bit
   {25,  5,  1,  0}, // 26 Bit
   {26,  4,  1,  0}, // 27 Bit
   {27, 24, -1, -1}, // 28 Bit
   {28, 26, -1, -1}, // 29 Bit
   {29,  5,  3,  0}, // 30 Bit
   {30, 27, -1, -1}, // 31 Bit
   {31, 21,  1,  0}  // 32 Bit
};


uint32_t lfsr_init(lfsr_t * const lfsr, uint32_t initVal, uint32_t width)
{
   if (width > LFSR_MAX_WIDTH) {
      printf("ERROR: lfsr width exceeded max of %d bits!\n", LFSR_MAX_WIDTH);
      return -1;
   }

   if (initVal == 0) {
      printf("ERROR: lfsr cannot start at 0!\n");
      return -1;
   }

   lfsr->width = width;
   lfsr->mask  = (0x1 << width) - 0x1;
   lfsr->taps = lfsr_taps[width - 1];
   lfsr->value = initVal & lfsr->mask;

   return 0;
}


uint32_t lfsr_next(lfsr_t* lfsr)
{
   uint32_t feedback = 0;
   for (uint32_t i = 0; i < LFSR_MAX_TAPS; i++) {
      if (lfsr->taps[i] == (uint32_t ) -1)
         break;
      feedback ^= (lfsr->value >> lfsr->taps[i]);
   }

   lfsr->value = (((lfsr->value << 1) & lfsr->mask) | (feedback & 0x1));
   return lfsr->value;
}

int main(){
    uint64_t stime, etime;
    uint64_t const timeCost = myTime() - myTime();
#ifdef BAREMETAL
    uint64_t * const base = (uint64_t *) MEMORY_BASE;
#else
#ifdef USE_HEAP
    uint64_t * const base = (uint64_t *) malloc(MEMORY_SIZE);
	if (!base) {
		printf("Could not allocate enoug heap space!\n");
		return 1;
	}
#else
    uint64_t * const base = (uint64_t *) MEMORY_BASE;
#endif
#endif

    for(uint64_t i = 0; i < MEMORY_SIZE / sizeof(uint64_t); i++){
        base[i] = 0;
    }

    uint32_t lfsrWidth = 3;

    printf("size;iterations;time;laddr\n");

    while ((0x1U << lfsrWidth) * sizeof(uint64_t) <= MEMORY_SIZE) {
        uint32_t const arrayElements = 0x1U << lfsrWidth;
        uint64_t const arraySize = arrayElements * sizeof(uint64_t);
        uint64_t const arrayIterations =  (arraySize >= CHASE_SIZE) ? 1U : ((CHASE_SIZE + (arraySize - 1U)) / arraySize);
        uint64_t const chaseIterations =  arrayIterations * arrayElements;

        lfsr_t lfsr;
        uint32_t lfsrStart;

        // a LFSR should never start at 0
        do {
            lfsrStart = myRand();
        } while ((lfsrStart & ((0x1U << lfsrWidth) - 1U)) == 0);

        lfsr_init(&lfsr, lfsrStart, lfsrWidth);

        for(unsigned i = 0; i < arrayElements - 1; i++){
            base[i] = (uint64_t) &base[lfsr_next(&lfsr)];
        }

        base[arrayElements - 1] = (uint64_t) &base[0];

        // Start at 0
        uint64_t *chase = (uint64_t *) base[0];

        stime = myTime();
        for(uint64_t i = 0; i < chaseIterations; i++) {
            chase = (uint64_t *) (*chase);
        }
        etime = myTime();


        printf("%lu;%lu;%lu;%p\n", arraySize, chaseIterations, (etime - stime - timeCost), chase);

        lfsrWidth += 1;
    }
    return 0;
}