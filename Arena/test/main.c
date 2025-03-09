#include "../arena.h"
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

typedef struct {
    char* d;
    int a;
    int b;
    short c;
} mock;

int main()
{
    size_t size = 1024;
    mock* m1;
    mock* m2;
    void* m3;

    //changed the allocation logic
    fprintf(stderr, "Should create arena with %ld long blocks: ", size);
    Arena* a = arena_create(size);
    ASSERT((a->blk_data_size == size));
    fprintf(stderr, "Arena allocated at %p\n", (void*)a);
    fprintf(stderr, "Block allocated at %p\n", (void*)a->tail);
    fprintf(stderr, "\n");

    fprintf(stderr, "Should allocate in arena %ld bytes: ", sizeof(mock));
    m1 = arena_alloc(a, sizeof(mock));
    m2 = arena_alloc(a, sizeof(mock));
    ASSERT(((uintptr_t)a->tail + sizeof(Block)) == (uintptr_t)m1);
    m1->a = 0xaa;
    m1->b = 0xbb;
    m1->c = 0xcc;
    m2->a = 0xaa;
    m2->b = 0xbb;
    m2->c = 0xcc;
    fprintf(stderr, "\n");

    fprintf(stderr, "Should allocate in arena %ld bytes: ", size);
    m3 = arena_alloc(a, size);
    ASSERT(m3);
    fprintf(stderr, "\n");

    srand(time(0));
    size_t s2 = 512;
    int num_it = 30;
    int random_num;

    fprintf(stderr, "Allocating arena with blk size %ld \n", s2);
    fprintf(stderr, "Allocating %d random nums and check the memory dump\n", num_it);

    Arena* b = arena_create(s2);
    for (int i = 0; i < num_it; i++) {
        random_num = (rand() % 100) + 1;
        arena_alloc(b, random_num);
        fprintf(stderr, "%d ", random_num);
    }
    fprintf(stderr, "\n");

    arena_memory_dump(b);
    arena_destroy(a);
    arena_destroy(b);
}
