#include "arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 1024

#ifdef ARENA_DEBUG
static void metadata_log(Arena* a, uintptr_t addr, size_t s)
{
    Metadata* md = malloc(sizeof(Metadata));
    assert(md);
    md->aloc = addr;
    md->asize = s;
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
            fprintf(stderr, "%p\t|\t\t\t\t| \n", (void*)tmp_head->aloc);
            fprintf(stderr, "%ldB (0x%lx)\t|            Al #%d:\t\t| \n", tmp_head->asize, tmp_head->asize, ++j);
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
    a->blk_data_size = s;
    a->head = block_create(s);
    a->tail = a->head;
    return a;
}

void* arena_alloc(Arena* a, size_t s)
{
    assert(s <= a->blk_data_size);

    uintptr_t ret;
    Block* blk = a->tail;

    if ((blk->filled + s) > a->blk_data_size) {
        Block* new_blk = block_create(a->blk_data_size);
        blk->next = new_blk;
        a->tail = new_blk;
        blk = new_blk;
    }

    ret = (uintptr_t)blk->data;
    ret += blk->filled;
    blk->filled += s;
#ifdef ARENA_DEBUG
    metadata_log(a, ret, s);
#endif
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
