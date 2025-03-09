#ifndef _ARENA_H
#define _ARENA_H

#include <stddef.h>
#include <stdint.h>

typedef struct Arena Arena;
typedef struct Block Block;


#ifdef ARENA_DEBUG
typedef struct Metadata Metadata;

void arena_memory_dump(Arena* a);

struct Metadata {
    uintptr_t aloc;
    size_t asize;
    size_t apadd;
    Metadata* next;
};

#endif

struct Block {
    size_t filled;
    Block* next;
    unsigned char data[];
};

struct Arena {
#ifdef ARENA_DEBUG
    Metadata* md_head;
    Metadata* md_tail;
#endif
    Block* head;
    Block* tail;
    size_t blk_data_size;
};

Arena* arena_create(size_t s);
void arena_destroy(Arena* a);

// size isn't check for performance reason, so be sure to pass a size to allocate smaller than the size used in arena_create
void* arena_alloc(Arena* a, size_t s);

// crash the program if align isn't a power of 2 (use align.h macros)
void* arena_alloc_align(Arena* a, size_t s, size_t align);

#endif
