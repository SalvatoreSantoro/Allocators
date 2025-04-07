# Pool Allocator

The pool is a type of allocator that is designed to manage the allocation of a certain number of
objects with the same size in a centralized way, in order to simplify allocation/deallocation logic.
Due to it's internal functioning, the pool allocator let's you deallocate objects in arbitrary order.
This come to a cost, basically the allocator keeps a series of pointers internally to make it possible
to always reconstruct the list of free chunks to use for allocations, in general this wouldn't be a problem
because the pointers only interest free chunks of memory, so when allocating an object on the pool
it would just erase the pointer from that particular chunk and also consider it as allocation space, but in this
particular implementation every allocated chunk will keep a pointer pointing back to the particular block of memory
the chunk belongs, basically enabling some other "freeing" logic discusseed in the following part.
So keep in mind that in order to minimize internal fragmentation (due to pointers + padding to manage memory alignment)
you should keep the object sizes relatively bigger than the size of pointers (look the case of object sizes equal to 8 bytes
that lead to basically half of the allocated memory wasted for metadata).


| Object Size (bytes) | Internal Fragmentation |
|---------------------|------------------------|
| 1                   | 0.938                  |
| 2                   | 0.876                  |
| 4                   | 0.752                  |
| 8                   | 0.505                  |
| 16                  | 0.338                  |
| 32                  | 0.203                  |
| 64                  | 0.113                  |

## Features

This allocator lets the user specify a particular memory alignment for the objects, it also lets the user specify the number of objects to allocate, but in reality it automatically expands itself, so the number of objects is an hint for the memory blocks sizes internally used. The allocator uses the macro MIN_FREE_CHUNKS_FRACTION to manage "almost full" blocks of memory, the logic is the following: <br>
-The allocator will release an empty block of memory (except when it is the only one allocated)<br>
-The allocator favor allocations on "almost full" blocks of memory, in order to compact all the allocations in fewer blocks
(it is more memory efficient to keep few full blocks of memory than a lot of almost empty blocks)<br>
So the MIN_FREE_CHUNKS_FRACTION is basically used to determine the fraction of "free" chunks of memory inside a block, in order to consider it almost full and start allocating on it.<br>
For example (with MIN_FREE_CHUNKS_FRACTION = 8) if a pool is created specifying 256 objects, the "min fraction" will be 256/8=32,
so when a block inside the pool reaches a number of allocated objects equal to 224 (256-32) it will be considered almost full and subsequent allocations will be done on it.
All this is explained because the MIN_FREE_CHUNKS_FRACTION macro it's tunable, in order to modify the allocator's freeing logic(in conjunction with a particular obj_num input). So observing the particular allocation/deallocation ratio that your application does you can tune this value in order to preserve more memory if needed

## API
Check the pool.h file

```c

// tunable
// keep it always smaller than obj_num
#define MIN_FREE_CHUNKS_FRACTION 8

typedef struct PoolAlc PoolAlc;


// RETURNS NULL IF OUT OF MEMORY
// assumes that the obj_size is at least equal(or greater) than the size of (void*)
// using this allocator for object smaller then a (void*) 
// could lead to a lot of memory internal fragmentation due internal padding added
// in the allocator to guarantee correct memory alignment
PoolAlc* pool_alc_create(size_t obj_size, size_t obj_num);

// same as pool_alc_create but lets you specify a particular alignment size
// it is suggested to use this function instead of pool_alc_create
// specifying alignment of the object using __alignof__ in order to minimize
// internal memory fragmentation
PoolAlc* pool_alc_create_align(size_t obj_size, size_t obj_num, size_t align);

// completely destroy all the memory managed by the pool and the pool itself
void pool_alc_destroy(PoolAlc* pa);

// RETURNS NULL IF OUT OF MEMORY
// allocates a block on a pool
void* pool_alc_alloc(PoolAlc* pa);

// releases an object from the pool
void pool_alc_free(PoolAlc* pa, void* p);

// returns the blocks memory internal fragmentation of the pool
float pool_alc_frag(PoolAlc* pa);

```
