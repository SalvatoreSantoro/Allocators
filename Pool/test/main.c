#include "../pool.h"

typedef struct{
    int a;
    int b;
    int c;
}Mock;

int main()
{
    PoolAlc* pa;
    pa = pool_alc_create(sizeof(Mock), 100);

    Mock* m = pool_alc_alloc(pa);
    m->a=1;
    return 0;
}
