#include "../align.h"
#include "../arena.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ASSERT(a)                        \
    do {                                 \
        if (a)                           \
            fprintf(stderr, "[PASS]\n"); \
        else                             \
            fprintf(stderr, "[FAIL]\n"); \
    } while (0)

#define ALIGNV_SIZE 5

int main(void)
{
    if (__STDC_VERSION__ >= 201710L)
        printf("We are using C18!\n");
    else if (__STDC_VERSION__ >= 201112L)
        printf("We are using C11!\n");
    else if (__STDC_VERSION__ >= 199001L)
        printf("We are using C99!\n");
    else
        printf("We are using C89/C90!\n");

    size_t s = 512;
    int *mock, random_align_idx, random_align, random_size, num_it = 10;
    int alignv[ALIGNV_SIZE] = { DEFAULT_ALIGN, MAX_ALIGN, QUAD_ALIGN, WORD_ALIGN, DOUBLE_ALIGN };

    srand(time(0));

    fprintf(stderr, "Allocating arena with blk size %ld \n", s);
    fprintf(stderr, "Allocating %d random nums and check the memory dump\n", num_it);

    Arena* a = arena_create(s);
    mock = (int*)arena_alloc(a, s);
    fprintf(stderr, "size: %ld, align: %ld, Testing for correct align: ", s, DEFAULT_ALIGN);
    ASSERT(((uintptr_t)mock % DEFAULT_ALIGN == 0));
    for (int i = 0; i < num_it; i++) {
        random_size = (rand() % s + 1);
        random_align_idx = (rand() % ALIGNV_SIZE);
        random_align = alignv[random_align_idx];
        mock = (int*)arena_alloc_align(a, random_size, random_align);
        fprintf(stderr, "size: %d, align: %d, Testing for correct align: ", random_size, random_align);
        ASSERT(((uintptr_t)mock % random_align == 0));
    }
    mock = (int*)arena_alloc(a, s);
    fprintf(stderr, "size: %ld, align: %ld, Testing for correct align: ", s, DEFAULT_ALIGN);
    ASSERT(((uintptr_t)mock % DEFAULT_ALIGN == 0));
    fprintf(stderr, "\n");

    fprintf(stderr, "Memory dump: \n");
    arena_memory_dump(a);
    arena_destroy(a);
}
