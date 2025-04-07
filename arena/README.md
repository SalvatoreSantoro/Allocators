# Arena Allocator

A pretty standard Arena allocator with some useful features for debugging memory related bugs.

## Features

The main features are:
- Memory alignment: when allocating memory on the arena it's possible to specify a type of alignment, 
as a default the arena will align the allocations on the actual architecture memory parallelism (sizeof(void*))
- Allocations using virtual pages
- When compiled with the macro "ARENA_DEBUG"
    - The arena will keep track of all allocations made, in order to reconstruct the memory image and print it out to stderr for debug purposes
    - The arena will take advantage of the padding bytes added to align correctly the data to store a "guard" character that will be checked to assure that none of the allocations made has exceeded the asigned size, pratically corrupting the arena allocations memory

## API
Check the arena.h file

```c
// prints to stderr a dump of arena's memory layout and sanitize the memory
void arena_memory_dump(const Arena* a);


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

```
