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

    size_t s = 2048;
    int *mock, random_align_idx, random_align, random_size, num_it = 10;
    int alignv[ALIGNV_SIZE] = { DEFAULT_ALIGN, MAX_ALIGN, QUAD_ALIGN, WORD_ALIGN, DOUBLE_ALIGN };

    srand(time(0));

    Arena* a = arena_create(ARENA_BIG);

    mock = (int*)arena_alloc(a, sizeof(int));

    Arena* b = arena_create(VM_BACKEND);

    fprintf(stderr, "\n");
    fprintf(stderr, "Allocating on HEAP: \n\n");

    for (int i = 0; i < num_it; i++) {
        random_size = (rand() % s + 1);
        random_align_idx = (rand() % ALIGNV_SIZE);
        random_align = alignv[random_align_idx];

        mock = (int*)arena_alloc_align(a, 10, random_align);
        fprintf(stderr, "size: %d, align: %d, Testing for correct align: ", random_size, random_align);
        ASSERT(((uintptr_t)mock % random_align == 0));
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "Allocating on VM: \n\n");

    for (int i = 0; i < num_it; i++) {
        random_size = (rand() % s + 1);
        mock = (int*)arena_alloc_align(b, random_size, random_align);
        fprintf(stderr, "size: %d, align: %d, Testing for correct align: ", random_size, random_align);
        ASSERT(((uintptr_t)mock % random_align == 0));
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "Memory dump of Malloc allocated Arena: \n\n");
    arena_memory_dump(a);

    fprintf(stderr, "Memory dump of VM allocated Arena: \n\n");
    arena_memory_dump(b);

    arena_destroy(a);
    arena_destroy(b);
}
