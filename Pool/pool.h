#ifndef _POOL_ALC_H
#define _POOL_ALC_H

#include "../common/macros.h"
#include <stddef.h>

typedef struct PoolAlc PoolAlc;

// assumes that the obj_size is at least equal(or greater) than the size of (void*)
// using this allocator for object smaller then a (void*) could lead to great internal fragmentation
// due to alignment constraints
PoolAlc* pool_alc_create(size_t obj_size, size_t obj_num);

PoolAlc* pool_alc_create_align(size_t obj_size, size_t obj_num, size_t align);

void* pool_alc_alloc(PoolAlc* pa);

#endif // !_POOL_H
