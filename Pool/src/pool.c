#include "../pool.h"
#include "../../common/align.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STORE_PTR(dest, src) \
    memcpy(dest, src, sizeof(void*))

typedef struct Block Block;

struct Block {
    // starting byte of a free chunk(obj_size + addr)
    unsigned char* free_chunk;
    Block* next;
    size_t filled;
    // chunks are in the form of -|DATA|ADDRESS|PADDING|- all the code make this assumption to manipulate the pointers
    unsigned char data[];
};

struct PoolAlc {
    Block* head;
    Block* tail;
    size_t chunk_size;
    size_t obj_size;
    size_t blk_data_size;
    size_t align;
    uint32_t chunk_num;
    uint32_t blks_num;
};

void* pool_alc_alloc(PoolAlc* pa)
{
    Block* blk;
    size_t start_padd;
    unsigned char* curr_pos = NULL;
    unsigned char* prev_pos = NULL;
    unsigned char* free_chunk;
    if ((pa->head == NULL) || (pa->chunk_num == pa->head->filled)) {
        // allocate considering the eventual padding caused by the align
        blk = malloc(sizeof(Block) + pa->align + pa->blk_data_size);
        if (blk == NULL)
            return NULL;
        start_padd = calc_align_padding((uintptr_t)blk->data, pa->align);
        curr_pos = start_padd + blk->data;

        for (uint32_t i = 0; i < pa->chunk_num; i++) {
            STORE_PTR(curr_pos + pa->obj_size, &prev_pos);
            prev_pos = curr_pos;
            curr_pos += pa->chunk_size;
        }
        blk->free_chunk = prev_pos;

        if (pa->blks_num == 0)
            pa->tail = blk;
        blk->next = pa->head;
        pa->head = blk;
        pa->blks_num += 1;
    }

    free_chunk = pa->head->free_chunk;
    // change next free chunk
    STORE_PTR(pa->head->free_chunk, free_chunk+pa->obj_size);
    // take next free obj and replace next pointer with pointer to the block it belongs
    STORE_PTR(free_chunk+pa->obj_size, &blk);
    pa->head->filled += 1;

    // blk is full put it in the tail
    if ((pa->head->filled == pa->chunk_num) && (pa->blks_num > 1)) {
        pa->tail->next = pa->head;
        pa->tail = pa->head;
        pa->head = pa->head->next;
        pa->tail->next = NULL;
    }

    printf("%p, %p, %p\n", (void*)free_chunk, (void*)pa->head->free_chunk, (void*)(free_chunk+pa->obj_size));
    return free_chunk;
}

/* void pool_alc_free(PoolAlc* pa, void* p) */
/* { */
/* } */

PoolAlc* pool_alc_create_align(size_t obj_size, size_t obj_num, size_t align)
{

    size_t obj_padd;
    size_t chunk_padd;
    size_t chunk_size;
    PoolAlc* pa;

    assert(is_power_of_2(align));
    pa = malloc(sizeof(PoolAlc));

    if (pa == NULL)
        return NULL;

    pa->head = NULL;
    pa->tail = NULL;

    // compute the padding to add to the obj to align addresses
    obj_padd = calc_align_padding(obj_size, PTR_ALIGN);
    obj_size += obj_padd;

    // chunks is (obj+addr)
    // add size of pointer for linking free objects or to reference block on which the object is allocated
    // get the padding to add to the chunks(obj+addr) in order to align them
    chunk_size = obj_size + sizeof(void*);
    chunk_padd = calc_align_padding(chunk_size, align);
    chunk_size += chunk_padd;

    pa->chunk_num = obj_num;
    pa->obj_size = obj_size;
    pa->chunk_size = chunk_size;
    pa->align = align;
    pa->blk_data_size = chunk_size * obj_num;
    return pa;
}

PoolAlc* pool_alc_create(size_t obj_size, size_t obj_num)
{
    return pool_alc_create_align(obj_size, obj_num, DEFAULT_ALIGN);
}
