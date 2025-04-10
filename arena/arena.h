#ifndef _ARENA_H
#define _ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../common/macros.h"

// allocation backend
#define VM_BACKEND 0

#define ARENA_SMALL 1024
#define ARENA_BIG 4096

typedef struct Arena Arena;

#ifdef ARENA_DEBUG
typedef struct Metadata Metadata;
// prints to stderr a dump of arena's memory layout and sanitize the memory
void arena_memory_dump(const Arena* a);
#endif

// RETURN NULL WHEN OUT OF MEMORY
// by default it will use malloc, if specified size as VM_BACKEND
// it will use virtual memory pages to allocate blocks
Arena* arena_create(size_t s);

// just a destructor
void arena_destroy(Arena* a);

// RETURN NULL WHEN OUT OF MEMORY
// When allocating an object that won't fit in remaining free space, 
// the arena will automatically create a new block of size "s"(the one specified during creation) to accomodate the request
// but won't check that the requested allocation can actually fit in the real space assigned for allocations
// at the time of the creation of the arena.
// So the general raccomandation is to create the arena using a pretty big size compared to
// the actual sizes that will be allocated on the arena.
// Use ARENA_BIG and ARENA_SMALL macros.
// (no check done for performance reason and also to avoid boilerplate code for checking eventual errors returned, 
// may change in future)
void* arena_alloc(Arena* a, size_t s);

//same as arena_alloc plus 
//crash the program if align isn't a power of 2 (use macros)
void* arena_alloc_align(Arena* a, size_t s, size_t align);

#endif
