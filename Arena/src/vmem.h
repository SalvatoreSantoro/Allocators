#ifndef _A_VMEMORY_H
#define _A_VMEMORY_H

#include <stddef.h>
size_t vmpage_get_size(void);

void* vmalloc(size_t s);
void vfree(void* p);

#endif // !A_UNIX_H

