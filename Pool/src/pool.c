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
    Block* prev;
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
    // minimum number of free chunk to place a block in the head, in order to try
    // to favour allocation on almost full blocks, to avoid keeping unused memory
    // (trying to create few blocks completely used instead of a lot of blocks almost full)
    uint32_t max_filled_head;
};

void* pool_alc_alloc(PoolAlc* pa)
{
    Block* blk = pa->head;
    size_t start_padd;
    unsigned char* curr_pos = NULL;
    unsigned char* prev_pos = NULL;
    unsigned char* free_chunk;
    unsigned char* free_chunk_next;

    if ((blk == NULL) || (pa->chunk_num == pa->head->filled)) {
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

        // place block in the head
        if (pa->blks_num == 0)
            pa->tail = blk;
        else
            pa->head->prev = blk;

        pa->blks_num += 1;
        blk->prev = NULL;
        blk->next = pa->head;

        pa->head = blk;
    }


    free_chunk = pa->head->free_chunk;
    free_chunk_next = free_chunk + pa->obj_size;
    // change next free chunk
    STORE_PTR(&pa->head->free_chunk, free_chunk_next);
    // take next free obj and replace next pointer with pointer to the block it belongs
    STORE_PTR(free_chunk_next, &blk);
    pa->head->filled += 1;

    // blk is full put it in the tail
    if ((pa->head->filled == pa->chunk_num) && (pa->blks_num > 1)) {
        pa->head->prev = pa->tail;
        pa->tail->next = pa->head;

        pa->tail = pa->head;
        pa->head = pa->head->next;

        pa->tail->next = NULL;
        pa->head->prev = NULL;
    }


    // printf("%p, %p, %p\n", (void*)free_chunk, (void*)pa->head->free_chunk, (void*)(free_chunk+pa->obj_size));
    return free_chunk;
}

void pool_alc_free(PoolAlc* pa, void* p)
{
    Block* blk;
    unsigned char* chunk = p;
    unsigned char* chunk_ptr = chunk + pa->obj_size;

    // get block associated to chunk
    STORE_PTR(&blk, chunk_ptr);
    // free the chunk
    STORE_PTR(chunk_ptr, &blk->free_chunk);
    STORE_PTR(&blk->free_chunk, &chunk);

    blk->filled -= 1;

    if ((blk == pa->head) || ((blk->filled != 0) && (blk->filled != pa->max_filled_head)))
        return;

    if (blk == pa->tail)
        pa->tail = blk->prev;
    else
        blk->next->prev = blk->prev;

    blk->prev->next = blk->next;

    if (blk->filled == pa->max_filled_head) {
        blk->prev = NULL;
        pa->head->prev = blk;
        blk->next = pa->head;
        pa->head = blk;
    }

    if (blk->filled == 0) {
        pa->blks_num -= 1;
        free(blk);
    }
}

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
    

    //printf("OBJ PADDING: %ld\n", obj_padd);

    // chunks is (obj+addr)
    // add size of pointer for linking free objects or to reference block on which the object is allocated
    // get the padding to add to the chunks(obj+addr) in order to align them
    chunk_size = obj_size + sizeof(void*);
    chunk_padd = calc_align_padding(chunk_size, align);
    chunk_size += chunk_padd;

    //printf("CHUNK PADDING: %ld\n", chunk_padd);

    pa->chunk_num = obj_num;
    pa->max_filled_head = obj_num - ((obj_num / MIN_FREE_CHUNKS_FRACTION) + 1);
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

void pool_alc_destroy(PoolAlc* pa)
{
    Block* temp;
    while (pa->head != NULL) {
        temp = pa->head;
        pa->head = pa->head->next;
        free(temp);
    }
    free(pa);
}
