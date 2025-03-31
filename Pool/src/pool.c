#include "../pool.h"
#include "../../common/align.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Block Block;

struct Block {
    uintptr_t free_obj;
    Block* next;
    size_t filled;
    size_t padd;
    unsigned char data[];
};

struct PoolAlc {
    Block* head;
    Block* tail;
    size_t obj_size;
    size_t blk_data_size;
    size_t align;
    uint32_t obj_num;
    uint32_t blks_num;
};

void blk_init(Block* blk, size_t obj_size, uint32_t obj_num)
{
    uintptr_t p = ((uintptr_t)blk->data) + blk->padd;
    uintptr_t prev_addr = (uintptr_t)NULL;

    for (uint32_t i = 0; i < obj_num; i++) {
        *((uintptr_t*)(p)) = prev_addr;
        //printf("%p, %p, %p \n", (void*)p, (void*)prev_addr, (void*)*(uintptr_t*)p);
        prev_addr = p;
        p += obj_size;
    }
    blk->free_obj = prev_addr;
}

void* pool_alc_alloc(PoolAlc* pa)
{
    Block* blk;
    size_t padd;
    void* p;
    uintptr_t addr;
    if ((pa->head == NULL) || (pa->obj_num == pa->head->filled)) {
        // allocate considering the eventual padding cause by the align
        blk = malloc(sizeof(Block) + pa->align + pa->blk_data_size);
        if (blk == NULL)
            return NULL;
        padd = calc_align_padding((uintptr_t)blk->data, pa->align);
        blk->padd = padd;
        blk_init(blk, pa->obj_size, pa->obj_num);
        if (pa->blks_num == 0)
            pa->tail = blk;
        blk->next = pa->head;
        pa->head = blk;
        pa->blks_num += 1;
    }

    // take next free obj and replace next pointer with pointer to the block it belongs
    p = (void*)pa->head->free_obj;
    addr = (uintptr_t)p + pa->obj_size;
    *((uintptr_t*)addr) = (uintptr_t)&blk;
    pa->head->filled += 1;

    // blk is full put it in the tail
    if ((pa->head->filled == pa->obj_num) && (pa->blks_num > 1)) {
        pa->tail->next = pa->head;
        pa->tail = pa->head;
        pa->head = pa->head->next;
        pa->tail->next = NULL;
    }

    return p;
}

/* void pool_alc_free(PoolAlc* pa, void* p) */
/* { */
/* } */

PoolAlc* pool_alc_create_align(size_t obj_size, size_t obj_num, size_t align)
{
    assert(is_power_of_2(align));
    PoolAlc* pa = malloc(sizeof(PoolAlc));
    if (pa == NULL)
        return NULL;
    pa->head = NULL;
    pa->tail = NULL;
    pa->obj_num = obj_num;
    pa->obj_size = obj_size;
    pa->align = align;
    // add size of pointer for linking free objects or to reference block on which the object is allocated
    pa->blk_data_size = (obj_size + sizeof(void*)) * obj_num;
    return pa;
}

PoolAlc* pool_alc_create(size_t obj_size, size_t obj_num)
{
    return pool_alc_create_align(obj_size, obj_num, DEFAULT_ALIGN);
}
