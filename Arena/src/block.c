#include "block.h"
#include <assert.h>
#include <string.h>

Block* block_create(size_t s, mem_alloc_func mem_alloc)
{
    Block* b = mem_alloc(sizeof(Block) + s);
    assert(b);
    b->filled = 0;
    b->next = NULL;
    return b;
}

void block_destroy(Block* blk, mem_dealloc_func mem_dealloc)
{
    mem_dealloc(blk);
}
