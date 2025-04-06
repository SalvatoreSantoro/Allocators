#ifndef _ALIGN_MACROS_H
#define _ALIGN_MACROS_H

// alignment macros
#define DEFAULT_ALIGN sizeof(void*)
#define PTR_ALIGN sizeof(void*)
#define CACHE_BLOCK_ALIGN 64
#define MAX_ALIGN __alignof__(long double)
#define QUAD_ALIGN __alignof__(long long)
#define WORD_ALIGN __alignof__(int)
#define DOUBLE_ALIGN __alignof__(short)


#endif // !_ALIGN_MACROS_H


