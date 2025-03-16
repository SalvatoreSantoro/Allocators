#ifndef _BLOCK_H
#define _BLOCK_H

#include <stddef.h>
typedef struct Block Block;

typedef void* (*mem_alloc_func)(size_t s);
typedef void (*mem_dealloc_func)(void*);


Block* block_create(size_t s, mem_alloc_func mem_alloc);
void block_destroy(Block* blk, mem_dealloc_func mem_dealloc);

struct Block {
    size_t filled;
    Block* next;
    unsigned char data[];
};


#endif // !_BLOCK_H
