#ifndef _ALIGN_H
#define _ALIGN_H

#include <stddef.h>
#include <stdint.h>
#define MAX_ALIGN 16
#define DEFAULT_ALIGN sizeof(void*)
#define QUAD_ALIGN 8
#define WORD_ALIGN 4
#define DOUBLE_ALIGN 2
#define is_power_of_2(align) !(align & (align - 1))

// if align is a power of 2, (align-1) is just a mask with all 1.
// can use it to get the modulo without doing a division

static inline size_t calc_align_padding(uintptr_t p, size_t align)
{
    uintptr_t a = (uintptr_t)align;
    uintptr_t modulo;
    modulo = p & (a - 1);
    return (size_t)(a - modulo);
}

#endif // !_ALIGN_H
