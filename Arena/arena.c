#include "arena.h"
#include "align.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 1024

#ifdef ARENA_DEBUG
static void metadata_log(Arena* a, uintptr_t addr, size_t s, size_t padd)
{
    Metadata* md = malloc(sizeof(Metadata));
    assert(md);
    md->aloc = addr;
    md->asize = s;
    md->apadd = padd;
    md->next = NULL;
    if (a->md_head == NULL) {
        a->md_head = md;
        a->md_tail = a->md_head;
    } else {
        a->md_tail->next = md;
        a->md_tail = a->md_tail->next;
    }
}

static void metadata_destroy(Metadata* md)
{
    free(md);
}

void arena_memory_dump(Arena* a)
{
    size_t header = sizeof(Block);
    Metadata* tmp = a->md_head;
    Metadata* tmp_head = a->md_head;
    Block* blk = a->head;
    int i = 0;
    uintptr_t last_ptr;
    while (blk != NULL) {
        int j = 0;
        size_t frsp = a->blk_data_size - blk->filled;
        size_t blk_base = (uintptr_t)blk;
        size_t blk_end = blk_base + header + a->blk_data_size;

        while ((tmp != NULL) && (tmp->aloc > blk_base) && (tmp->aloc < blk_end))
            tmp = tmp->next;

        fprintf(stderr, "\t\t_________________________________\n");
        fprintf(stderr, "%p\t|           Block #%d:           |\n", (void*)blk_base, ++i);
        fprintf(stderr, "\t\t|_______________________________|\n");
        fprintf(stderr, "%ldB (0x%lx)\t|            HEADER             |\n", header, header);
        fprintf(stderr, "\t\t|_______________________________|\n");
        while (tmp_head != tmp) {
            j += 1;
            if (tmp_head->apadd != 0) {
                fprintf(stderr, "%p\t|\t\t\t\t| \n", (void*)((uintptr_t)tmp_head->aloc - (uintptr_t)tmp_head->apadd));
                fprintf(stderr, "%ldB (0x%lx)\t|          Padding #%d:\t\t| \n", tmp_head->apadd, tmp_head->apadd, j);
                fprintf(stderr, "\t\t|_______________________________| \n");
            }
            fprintf(stderr, "%p\t|\t\t\t\t| \n", (void*)tmp_head->aloc);
            fprintf(stderr, "%ldB (0x%lx)\t|            Al #%d:\t\t| \n", tmp_head->asize, tmp_head->asize, j);
            fprintf(stderr, "\t\t|_______________________________| \n");
            last_ptr = tmp_head->aloc + tmp_head->asize;
            tmp_head = tmp_head->next;
        }
        if (frsp != 0) {
            fprintf(stderr, "%p\t|\t\t\t\t| \n", (void*)last_ptr);
            fprintf(stderr, "%ldB (0x%lx)\t|          FREE SPACE\t\t| \n", frsp, frsp);
            fprintf(stderr, "\t\t|_______________________________| \n");
            fprintf(stderr, "\n\n");
        }
        blk = blk->next;
    }
}

#endif

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
    a->blk_data_size = s + (MAX_ALIGN - 1); // add MAX_ALIGN-1 so that if allocating exactly s bytes there is always padding space
    a->head = block_create(s);
    a->tail = a->head;
    return a;
}

// FIX: there is a bug when checking dimension to allocate new block
// if creating an allocation with size = block size
// what happens is that if you're adding padding to align memory
// you will give back to caller a pointer that is in the middle of the allocation,
// so effectively allocating less memory than requested
// if the caller fills up all the memory that requested it will certainly lead to buffer overflow
void* arena_alloc_align(Arena* a, size_t s, size_t align)
{
    uintptr_t curr_addr;
    size_t padding;
    size_t offset;
    Block* blk;

    assert(s <= a->blk_data_size);
    assert(is_power_of_2(align));

    blk = a->tail;
    curr_addr = (uintptr_t)blk->data + (uintptr_t)blk->filled;
    padding = calc_align_padding(curr_addr, align, NORM_PAD);

    // create new block, need to recompute padding, curr_addr and block
    if ((blk->filled + s + padding) > a->blk_data_size) {
        Block* new_blk = block_create(a->blk_data_size);
        padding = calc_align_padding((uintptr_t)new_blk->data, align, NORM_PAD);
        curr_addr = (uintptr_t)new_blk->data;
        // add the block to list
        blk->next = new_blk;
        a->tail = new_blk;

        blk = new_blk;
    }

    curr_addr += (uintptr_t)padding;
    offset = padding + s;
    blk->filled += offset;

#ifdef ARENA_DEBUG
    metadata_log(a, curr_addr, s, padding);
#endif
    printf("%p\n", (void*)curr_addr);
    return (void*)curr_addr;
}

void* arena_alloc(Arena* a, size_t s)
{
    return arena_alloc_align(a, s, DEFAULT_ALIGN);
}

// TODO destroy metadata
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
