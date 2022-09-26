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
​
#include <stdio.h>
#include <stdint.h>
​
​
#define KiB(x) (1024ULL * x)
#define MiB(x) (1024ULL * KiB(x))
#define GiB(x) (1024ULL * MiB(x))
​
#ifndef STACK_SIZE
#define STACK_SIZE (MiB(128))
//#define STACK_SIZE (512)
#endif
​
#define STREAM_TYPE uint64_t
​
#ifndef STREAM_SIZE
#define STREAM_SIZE ((uint64_t) GiB(8))
#endif
​
​
#define CACHELINE_SIZE 64U
​
// In case one wants to only access e.g. once per cache line
#if !defined(COMPLEX_STRIDE)
#if defined(STRIDED)
#define STRIDE_ELEMENTS ((uint64_t) (CACHELINE_SIZE / sizeof(STREAM_TYPE)))
#else
#define STRIDE_ELEMENTS ((uint64_t) (1U))
#endif
#define STRIDE_ELEMENTS_0 (0 * STRIDE_ELEMENTS)
#define STRIDE_ELEMENTS_1 (1 * STRIDE_ELEMENTS)
#define STRIDE_ELEMENTS_2 (2 * STRIDE_ELEMENTS)
#define STRIDE_ELEMENTS_3 (3 * STRIDE_ELEMENTS)
#define STRIDE_ELEMENTS_4 (4 * STRIDE_ELEMENTS)
#define STRIDE_ELEMENTS_5 (5 * STRIDE_ELEMENTS)
#define STRIDE_ELEMENTS_6 (6 * STRIDE_ELEMENTS)
#define STRIDE_ELEMENTS_7 (7 * STRIDE_ELEMENTS)
​
#define STRIDE_ELEMENTS_ITERATION (8 * STRIDE_ELEMENTS)
#else
​
#define STRIDE_ELEMENTS ((uint64_t) (CACHELINE_SIZE / sizeof(STREAM_TYPE)))
#define STRIDE_ELEMENTS_0 ((0 * STRIDE_ELEMENTS) + 0) // First Cacheline
#define STRIDE_ELEMENTS_1 ((0 * STRIDE_ELEMENTS) + 1) // First Cacheline
#define STRIDE_ELEMENTS_2 ((1 * STRIDE_ELEMENTS) + 0) // Second Cacheline
#define STRIDE_ELEMENTS_3 ((1 * STRIDE_ELEMENTS) + 1) // Second Cacheline
#define STRIDE_ELEMENTS_4 ((3 * STRIDE_ELEMENTS) + 0) // Fourth Cacheline
#define STRIDE_ELEMENTS_5 ((4 * STRIDE_ELEMENTS) + 0) // Fifth Cacheline
#define STRIDE_ELEMENTS_6 ((5 * STRIDE_ELEMENTS) + 0) // Sixth Cacheline
#define STRIDE_ELEMENTS_7 ((6 * STRIDE_ELEMENTS) + 0) // Seventh Cacheline
#define STRIDE_ELEMENTS_ITERATION (7 * STRIDE_ELEMENTS)
#endif
​
#define MAX_KERNEL_LOAD 2U
#define KERNEL_UNROLL_FACTOR 8U
​
#include <sys/time.h>
static struct timeval tp;
static struct timezone tz;
#define myTime() ({                                                 \
        gettimeofday(&tp, &tz);                                     \
        (((uint64_t) tp.tv_sec * 1000000) + (uint64_t) tp.tv_usec); \
    })
​
static STREAM_TYPE MEMORY_BASE[STACK_SIZE / sizeof(STREAM_TYPE)];
​
static STREAM_TYPE *streamA = (STREAM_TYPE *) MEMORY_BASE;
static STREAM_TYPE *streamB = (STREAM_TYPE *) MEMORY_BASE + (STACK_SIZE / (2 * sizeof(STREAM_TYPE)));
​
​
#define loadKernel(memorySize) {                                                                                                      \
        uint64_t const kernelLoad = 1;                                                                                                \
        uint64_t const maxStreamElements = memorySize / (sizeof(STREAM_TYPE) * kernelLoad);                                           \
        uint64_t const copyIterations = (kernelLoad * STRIDE_ELEMENTS_ITERATION * STREAM_SIZE) / (KERNEL_UNROLL_FACTOR * memorySize); \
        stime = myTime();                                                                                                             \
        for (n = 0; n < copyIterations; n++) {                                                                                        \
            for (stream = 0; stream < maxStreamElements; stream += STRIDE_ELEMENTS_ITERATION) {                                       \
                _use += streamA[stream + STRIDE_ELEMENTS_0] +                                                                         \
                    streamA[stream + STRIDE_ELEMENTS_1] +                                                                             \
                    streamA[stream + STRIDE_ELEMENTS_2] +                                                                             \
                    streamA[stream + STRIDE_ELEMENTS_3] +                                                                             \
                    streamA[stream + STRIDE_ELEMENTS_4] +                                                                             \
                    streamA[stream + STRIDE_ELEMENTS_5] +                                                                             \
                    streamA[stream + STRIDE_ELEMENTS_6] +                                                                             \
                    streamA[stream + STRIDE_ELEMENTS_7];                                                                              \
            }                                                                                                                         \
        }                                                                                                                             \
        etime = myTime();                                                                                                             \
    }
​
#define storeKernel(memorySize) {                                                                                                     \
        uint64_t const kernelLoad = 1;                                                                                                \
        uint64_t const maxStreamElements = memorySize / (sizeof(STREAM_TYPE) * kernelLoad);                                           \
        uint64_t const copyIterations = (kernelLoad * STRIDE_ELEMENTS_ITERATION * STREAM_SIZE) / (KERNEL_UNROLL_FACTOR * memorySize); \
        stime = myTime();                                                                                                             \
        for (n = 0; n < copyIterations; n++) {                                                                                        \
            for (stream = 0; stream < maxStreamElements; stream += STRIDE_ELEMENTS_ITERATION) {                                       \
                streamA[stream + STRIDE_ELEMENTS_0] = 1;                                                                              \
                streamA[stream + STRIDE_ELEMENTS_1] = 1;                                                                              \
                streamA[stream + STRIDE_ELEMENTS_2] = 1;                                                                              \
                streamA[stream + STRIDE_ELEMENTS_3] = 1;                                                                              \
                streamA[stream + STRIDE_ELEMENTS_4] = 1;                                                                              \
                streamA[stream + STRIDE_ELEMENTS_5] = 1;                                                                              \
                streamA[stream + STRIDE_ELEMENTS_6] = 1;                                                                              \
                streamA[stream + STRIDE_ELEMENTS_7] = 1;                                                                              \
            }                                                                                                                         \
        }                                                                                                                             \
        etime = myTime();                                                                                                             \
    }
​
​
​
#define loadStoreKernel(memorySize) {                                                                                                 \
        uint64_t const kernelLoad = 2;                                                                                                \
        uint64_t const maxStreamElements = memorySize / (sizeof(STREAM_TYPE) * kernelLoad);                                           \
        uint64_t const copyIterations = (kernelLoad * STRIDE_ELEMENTS_ITERATION * STREAM_SIZE) / (KERNEL_UNROLL_FACTOR * memorySize); \
        stime = myTime();                                                                                                             \
        for (n = 0; n < copyIterations; n++) {                                                                                        \
            for (stream = 0; stream < maxStreamElements; stream += STRIDE_ELEMENTS_ITERATION) {                                       \
                streamA[stream + STRIDE_ELEMENTS_0] = streamB[stream + STRIDE_ELEMENTS_0];                                            \
                streamA[stream + STRIDE_ELEMENTS_1] = streamB[stream + STRIDE_ELEMENTS_1];                                            \
                streamA[stream + STRIDE_ELEMENTS_2] = streamB[stream + STRIDE_ELEMENTS_2];                                            \
                streamA[stream + STRIDE_ELEMENTS_3] = streamB[stream + STRIDE_ELEMENTS_3];                                            \
                streamA[stream + STRIDE_ELEMENTS_4] = streamB[stream + STRIDE_ELEMENTS_4];                                            \
                streamA[stream + STRIDE_ELEMENTS_5] = streamB[stream + STRIDE_ELEMENTS_5];                                            \
                streamA[stream + STRIDE_ELEMENTS_6] = streamB[stream + STRIDE_ELEMENTS_6];                                            \
                streamA[stream + STRIDE_ELEMENTS_7] = streamB[stream + STRIDE_ELEMENTS_7];                                            \
            }                                                                                                                         \
        }                                                                                                                             \
        etime = myTime();                                                                                                             \
    }
​
#define loadWarmup(memorySize) {                                                            \
        uint64_t const kernelLoad = 1;                                                      \
        uint64_t const maxStreamElements = memorySize / (sizeof(STREAM_TYPE) * kernelLoad); \
        for (stream = 0; stream < maxStreamElements; stream++) {                            \
            _use += streamA[stream];                                                        \
        }                                                                                   \
    }
​
#define storeWarmup(memorySize) {                                                           \
        uint64_t const kernelLoad = 1;                                                      \
        uint64_t const maxStreamElements = memorySize / (sizeof(STREAM_TYPE) * kernelLoad); \
        for (stream = 0; stream < maxStreamElements; stream++) {                            \
            streamA[stream] = 1;                                                            \
        }                                                                                   \
    }
​
#define loadStoreWarmup(memorySize) {                                                       \
        uint64_t const kernelLoad = 2;                                                      \
        uint64_t const maxStreamElements = memorySize / (sizeof(STREAM_TYPE) * kernelLoad); \
        for (stream = 0; stream < maxStreamElements; stream++) {                            \
            streamA[stream] = streamB[stream];                                              \
        }                                                                                   \
    }
​
int main() {
    uint64_t memorySize = sizeof(STREAM_TYPE) * MAX_KERNEL_LOAD * KERNEL_UNROLL_FACTOR;
    uint64_t stream, n;
    uint64_t stime, etime;
    uint64_t _use = 0;
​
    printf("partition;size;load;store;load_store\n");
​
    while (memorySize <= STACK_SIZE) {
        printf("%lu;%lu", memorySize, STREAM_SIZE);
        loadWarmup(memorySize);
        loadKernel(memorySize);
        printf(";%lu", (etime-stime));
​
        storeWarmup(memorySize);
        storeKernel(memorySize);
        printf(";%lu", (etime-stime));
​
        loadStoreWarmup(memorySize);
        loadStoreKernel(memorySize);
        printf(";%lu\n", (etime-stime));
​
		memorySize *= 2;
    }
    printf("# [%lu]\n", _use);
    return 0;
}