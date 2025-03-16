#ifndef _ARENA_H
#define _ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "block.h"

// allocation backend
#define VM_BACKEND 0

typedef struct Arena Arena;

#ifdef ARENA_DEBUG
typedef struct Metadata Metadata;

// prints to stderr a dump of arena's memory layout
void arena_memory_dump(const Arena* a);

struct Metadata {
    uintptr_t aloc;
    size_t asize;
    size_t apadd;
    Metadata* next;
};

#endif

struct Arena {
#ifdef ARENA_DEBUG
    Metadata* md_head;
    Metadata* md_tail;
    unsigned char rng_padd_char; // random character put in padding space, to check eventual overflows with arena_memory_dump()
#endif
    Block* head;
    Block* tail;
    size_t blk_data_size;
    mem_alloc_func mem_alloc;
    mem_dealloc_func mem_dealloc;
};

// by default it will use malloc, if specified as VM_BACKEND it will use virtual memory pages to allocate blocks
Arena* arena_create(size_t s);
void arena_destroy(Arena* a);

// size isn't check for performance reason, so be sure to pass a size to allocate smaller than the size used in arena_create
void* arena_alloc(Arena* a, size_t s);

// crash the program if align isn't a power of 2 (use align.h macros)
void* arena_alloc_align(Arena* a, size_t s, size_t align);

#endif
