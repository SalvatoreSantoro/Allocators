#include "arena.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 1024

static Block* block_create(size_t s)
{
    Block* b = malloc(sizeof(Block) + s);
    assert(b);
    b->filled = 0;
    b->next = NULL;
    memset(b->data, 0, s);
    return b;
}

static void block_destroy(Block* blk)
{
    free(blk);
}

Arena* arena_create(size_t s)
{
    Arena* a = malloc(sizeof(Arena));
    assert(a);
    a->blocks_size = s;
    a->head = block_create(s);
    a->tail = a->head;
    return a;
}

void* arena_alloc(Arena* a, size_t s)
{
    assert(s <= a->blocks_size);

    uintptr_t ret;
    Block* blk = a->tail;

    if ((blk->filled + s) > a->blocks_size) {
        Block* new_blk = block_create(a->blocks_size);
        blk->next = new_blk;
        a->tail = new_blk;
        blk = new_blk;
    }

    ret = (uintptr_t)blk->data;
    ret += blk->filled;
    blk->filled += s;
    return (void*)ret;
}

void arena_destroy(Arena* a)
{
    Block* temp_blk;
    while (a->head != NULL) {
        temp_blk = a->head;
        a->head = a->head->next;
        block_destroy(temp_blk);
    }
    assert(temp_blk == a->tail);
    free(a);
}
