#ifndef _ARENA_H
#define _ARENA_H

#include <stddef.h>

typedef struct Block Block;

struct Block {
    size_t filled;
    Block* next;
    unsigned char data[];
};

typedef struct {
    Block* head;
    Block* tail;
    size_t blocks_size;
} Arena;

Arena* arena_create(size_t s);
void arena_destroy(Arena* a);

// size isn't check for performance reason, so be sure to pass a size to allocate smaller than the size used in arena_create
void* arena_alloc(Arena* a, size_t s);

#ifdef DEBUG

#endif

#endif
