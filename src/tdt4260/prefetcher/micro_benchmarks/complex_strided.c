#include <stdio.h>
#include <stdint.h>


#define KiB(x) (1024ULL * x)
#define MiB(x) (1024ULL * KiB(x))
#define GiB(x) (1024ULL * MiB(x))

#ifndef STACK_SIZE
#define STACK_SIZE (MiB(64))
#endif

#define STRIDE 64
#define STEP (STRIDE / sizeof(uint64_t))

static uint64_t MEMORY[1024*512];

#include <sys/time.h>

uint64_t values[15] =
{11, 17, 48, 97,
 31, 10, 24, 95,
 81, 40, 49, 62,
 55, 19, 55};

int pattern[15] =
{1, 1, 2, 1,
 1, 2, 1, 1,
 3, 1, 1, 3,
 1, 1, 2};

int base1 = 0;
int base2 = 1024*128;
int base3 = 1024*256;

int i_off_1 = 0;
int i_off_2 = 5;
int i_off_3 = 10;

void accessElements(int num) {
    uint64_t v1, v2, v3 = 0;
    uint64_t m1, m2, m3 = 0;
    int offset1, offset2, offset3 = 0;
    
    int a1 = base1;
    int a2 = base2;
    int a3 = base3;

    for (int i = 0; i < num; i++) {
        offset1 = pattern[(i + i_off_1) % 15];
        offset2 = pattern[(i + i_off_2) % 15];
        offset3 = pattern[(i + i_off_3) % 15];

        a1 += offset1 * STEP;
        a2 += offset2 * STEP;
        a3 += offset3 * STEP;

#define DEBUG

#ifdef DEBUG
        printf("[DEBUG] Iterator at %d, Index at : %d, %d, %d\n",
               i, a1, a2, a3);
#endif

        v1 = values[(i + i_off_3) % 15];
        v2 = values[(i + i_off_2) % 15];
        v3 = values[(i + i_off_1) % 15];
        
        for (int j = 0; j < STEP; j++) {
            m1 = MEMORY[a1 + j];
            MEMORY[a1 + j] = m1 * v1;

            m2 = MEMORY[a2 + j];
            MEMORY[a2 + j] = m2 * v2;

            m3 = MEMORY[a3 + j];
            MEMORY[a3 + j] = m3 * v3;
        }
    }
}

int main() {
    accessElements(1024 * 32);
}