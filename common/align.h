#ifndef _ALIGN_H
#define _ALIGN_H

#include <stddef.h>
#include <stdint.h>

#define is_power_of_2(align) !(align & (align - 1))

// Paddings modes
// if already aligned returns align so that allocations aren't touching, it's useful for debug to check if
// an alocation is going outside it's space and overwriting the padding

// if align is a power of 2, (align-1) is just a mask with all 1.
// can use it to get the modulo without doing a division
static inline size_t calc_align_padding(uintptr_t p, size_t align)
{
    uintptr_t modulo;
    size_t ret;
    modulo = p & (align - 1);
#ifdef ARENA_DEBUG
    ret = (size_t)(align - modulo);
#else
    ret = modulo != 0 ? (size_t)(align - modulo) : modulo;
#endif
    return ret;
}

#endif // !_ALIGN_H
