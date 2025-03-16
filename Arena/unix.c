#include "unix.h"
#include <assert.h>
#include <stddef.h>
#include <unistd.h>

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
    //TODO
}

void vmfree(void* p){
    //TODO
}
