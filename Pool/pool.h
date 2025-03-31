#ifndef _POOL_H
#define _POOL_H

#include "../common/macros.h"
#include <stddef.h>

typedef struct PoolAlc PoolAlc;

PoolAlc* pool_alc_create(size_t obj_size, size_t obj_num);

PoolAlc* pool_alc_create_align(size_t obj_size, size_t obj_num, size_t align);

void* pool_alc_alloc(PoolAlc* pa);

#endif // !_POOL_H
