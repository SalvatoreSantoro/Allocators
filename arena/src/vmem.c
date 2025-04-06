#include "vmem.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __unix
    #include <sys/mman.h>
    #include <unistd.h>
#endif

#ifdef __WIN32
    #include <sysinfoapi.h>
    #include <memoryapi.h>
#endif

long pagesize = 0;

size_t vmpage_get_size(void)
{

    long size;
    
    if (pagesize != 0)
        return pagesize;

#ifdef __unix
    size = sysconf(_SC_PAGESIZE);

    if (size == -1)
        size = sysconf(_SC_PAGESIZE);
#endif

#ifdef __WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    size = (long)sysInfo.dwPageSize;
#endif

    assert((size != -1) && "IMPOSSIBLE TO FIND VM PAGE SIZE");

    pagesize = size;
    return (size_t)size;
}

void* vmalloc(size_t s)
{
#ifdef __unix
    void* p = mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif

#ifdef __WIN32
    void* p = VirtualAlloc(NULL, s, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif
    assert(p && "IMPOSSIBLE TO ALLOCATE VM");
    return p;
}

void vfree(void* p)
{
#ifdef __unix
    munmap(p, pagesize);
#endif

#ifdef __WIN32
    VirtualFree(p, pagesize, MEM_RELEASE);
#endif
}
