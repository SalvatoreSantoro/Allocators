#ifndef _POOL_ALC_H
#define _POOL_ALC_H

#include "../common/macros.h"
#include <stddef.h>


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

#endif // !_POOL_H
