#include "../arena.h"
#include "../../common/align.h"
#include "vmem.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Block Block;

typedef void* (*mem_alloc_func)(size_t s);
typedef void (*mem_dealloc_func)(void*);

struct Block {
    size_t filled;
    Block* next;
    unsigned char data[];
};

struct Arena {
#ifdef ARENA_DEBUG
    Metadata* md_head;
    Metadata* md_tail;
    unsigned char rng_padd_char; // random character put in padding space, to check eventual overflows with arena_memory_dump()
#endif
    Block* head;
    Block* tail;
    size_t blk_data_size;
    size_t blk_size;
    mem_alloc_func mem_alloc; // allocation function used from blocks
    mem_dealloc_func mem_dealloc; // deallocation function used from blocks
};

#ifdef ARENA_DEBUG

struct Metadata {
    uintptr_t aloc;
    size_t asize;
    size_t apadd;
    Metadata* next;
};

static void metadata_print(const char* str, void* addr, size_t size, int idx)
{
    fprintf(stderr, "%p\t|\t\t\t\t| \n", addr);
    fprintf(stderr, "%zuB (0x%zx)\t|             %s#%d\t\t|\n", size, size, str, idx);
    fprintf(stderr, "\t\t|_______________________________| \n");
}

static void memory_sanitize(unsigned char* curr_pos, size_t apadd, unsigned char padd_char)
{
    // iterate backwards from allocation address to check if the expected padding character has been overwritten
    while (apadd > 0) {
        curr_pos -= 1;
        apadd -= 1;
        assert((*(curr_pos) == padd_char) && "[MEMORY CORRUPTION] - SANITIZATION FAILED");
    };
}

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

void arena_memory_dump(const Arena* a)
{
    size_t header = sizeof(Block), frsp;
    uintptr_t last_ptr = 0, blk_base, blk_end, padd_addr;
    int i = 0, j = 0;
    Metadata *tmp_md = a->md_head, *tmp_head = a->md_head;
    Block* blk = a->head;

    // loop every block
    for (; blk != NULL; blk = blk->next) {
        // compute free space of block
        frsp = a->blk_data_size - blk->filled;

        // compute base and ending block address
        blk_base = (uintptr_t)blk;
        blk_end = blk_base + header + a->blk_data_size;

        // print header
        fprintf(stderr, "\t\t_________________________________\n");
        fprintf(stderr, "%p\t|            Block#%d:\t\t|\n", (void*)blk_base, ++i);
        fprintf(stderr, "\t\t|_______________________________|\n");
        fprintf(stderr, "%zuB (0x%zx)\t|             HEADER            |\n", header, header);
        fprintf(stderr, "\t\t|_______________________________|\n");

        // move tmp pointer to the first metadata element that isn't about current block
        while ((tmp_md != NULL) && (tmp_md->aloc > blk_base) && (tmp_md->aloc < blk_end)) {
            memory_sanitize((unsigned char*)tmp_md->aloc, tmp_md->apadd, a->rng_padd_char);
            tmp_md = tmp_md->next;
        }

        // print information about every metadata element of this block
        while (tmp_head != tmp_md) {
            padd_addr = tmp_head->aloc - tmp_head->apadd;
            j += 1;
            if (tmp_head->apadd != 0)
                metadata_print("Padd", (void*)padd_addr, tmp_head->apadd, j);

            metadata_print("Al", (void*)tmp_head->aloc, tmp_head->asize, j);
            last_ptr = tmp_head->aloc + tmp_head->asize;
            tmp_head = tmp_head->next;
        }

        if ((frsp != 0) && (last_ptr != 0)) {
            metadata_print("FREE", (void*)last_ptr, frsp, i);
            memory_sanitize((unsigned char*)last_ptr + frsp, frsp, a->rng_padd_char);
        }
        fprintf(stderr, "\n\n");
    }
}

#endif

Arena* arena_create(size_t s)
{
    mem_alloc_func mem_alloc;
    mem_dealloc_func mem_dealloc;
    size_t blk_size;
    Block* blk;

    // if s == 0 use virtual memory pages to create blocks
    if (s == 0) {
        // to make sure that all the block fits in a page we subtract the size of the block header from the page size
        // to get data size
        s = vmpage_get_size() - sizeof(Block);
        mem_alloc = (mem_alloc_func)vmalloc;
        mem_dealloc = (mem_dealloc_func)vfree;
    } else {
        mem_alloc = (mem_alloc_func)malloc;
        mem_dealloc = (mem_dealloc_func)free;
    }
    blk_size = sizeof(Block) + s;
    blk = mem_alloc(blk_size);
    if (blk == NULL)
        return NULL;

    memset(blk, 0, blk_size);

    Arena* a = malloc(sizeof(Arena));
    if (a == NULL)
        return NULL;

    a->head = blk;
    a->blk_size = blk_size;
    a->blk_data_size = s;
    a->mem_alloc = mem_alloc;
    a->mem_dealloc = mem_dealloc;
    a->tail = a->head;

#if ARENA_DEBUG
    a->md_head = NULL;
    a->md_tail = NULL;
    a->rng_padd_char = rand() % 256;
    memset(a->head->data, a->rng_padd_char, a->blk_data_size);
#endif
    return a;
}

// This function is assuming that s+padding calculated from the align requested is smaller then the
// allocation blk_size
void* arena_alloc_align(Arena* a, size_t s, size_t align)
{
    unsigned char* curr_addr;
    size_t padding;
    size_t offset;
    Block* blk;

    assert(is_power_of_2(align));

    blk = a->tail;
    curr_addr = blk->data + blk->filled;
    padding = calc_align_padding((uintptr_t)curr_addr, align);

    assert((s + padding) <= a->blk_data_size);

    // create new block, need to recompute padding, curr_addr and block
    if ((blk->filled + s + padding) > a->blk_data_size) {
        Block* new_blk = a->mem_alloc(a->blk_size);
        if (new_blk == NULL)
            return NULL;
        padding = calc_align_padding((uintptr_t)new_blk->data, align);
        curr_addr = new_blk->data;
        // add the block to list
        blk->next = new_blk;
        a->tail = new_blk;
        blk = new_blk;
#ifdef ARENA_DEBUG
        memset(a->tail->data, (int)a->rng_padd_char, a->blk_data_size);
#endif
    }

    curr_addr += padding;
    offset = padding + s;
    blk->filled += offset;

#ifdef ARENA_DEBUG
    metadata_log(a, (uintptr_t)curr_addr, s, padding);
#endif

    return curr_addr;
}

void* arena_alloc(Arena* a, size_t s)
{
    return arena_alloc_align(a, s, DEFAULT_ALIGN);
}

void arena_destroy(Arena* a)
{
    Block* temp_blk = NULL;
    if (a == NULL)
        return;
    while (a->head != NULL) {
        temp_blk = a->head;
        a->head = a->head->next;
        a->mem_dealloc(temp_blk);
    }
    assert(temp_blk == a->tail);

#ifdef ARENA_DEBUG
    Metadata* temp_md_blk = NULL;
    while (a->md_head != NULL) {
        temp_md_blk = a->md_head;
        a->md_head = a->md_head->next;
        metadata_destroy(temp_md_blk);
    }
    assert(temp_md_blk == a->md_tail);
#endif
    free(a);
}
