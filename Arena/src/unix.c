#include "unix.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int pagesize=0;

size_t vmpage_get_size(void)
{
    long size;
    size = sysconf(_SC_PAGESIZE);

    if (size == -1)
        size = sysconf(_SC_PAGESIZE);

    assert((size != -1) && "IMPOSSIBLE TO FIND VM PAGE SIZE");

    return (size_t)size;
}

void* vmalloc(size_t s)
{
    void* p = mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assert(p && "IMPOSSIBLE TO ALLOCATE VM");
    return p;
}

void vfree(void* p)
{
    munmap(p, 0);
}
