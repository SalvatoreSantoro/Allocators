#include "../arena.h"
#include <stdint.h>
#include <stdio.h>

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
    size_t size = 24;
    mock* m1;
    void* m2;

    fprintf(stderr, "Should create arena with %ld long blocks: ", size);
    Arena* a = arena_create(size);
    ASSERT((a->blocks_size == size));
    fprintf(stderr, "Arena allocated at %p\n", (void*)a);
    fprintf(stderr, "Block allocated at %p\n", (void*)a->tail);
    fprintf(stderr, "\n");

    fprintf(stderr, "Should allocate in arena %ld bytes: ", sizeof(mock));
    m1 = arena_alloc(a, sizeof(mock));
    ASSERT(((uintptr_t)a->tail + sizeof(Block)) == (uintptr_t)m1);
    m1->a = 0xaa;
    m1->b = 0xbb;
    m1->c = 0xcc;
    fprintf(stderr, "Mock allocated at %p\n", (void*)m1);
    fprintf(stderr, "\n");

    fprintf(stderr, "Should allocate in arena %ld bytes: ", size);
    m2 = arena_alloc(a, size);
    ASSERT(m2);
    fprintf(stderr, "Mock allocated at %p\n", (void*)m2);
    fprintf(stderr, "\n");

    fprintf(stderr, "Should have allocated a new block: ");
    ASSERT(m2 != a->head);
    fprintf(stderr, "\n");

    arena_destroy(a);
}
