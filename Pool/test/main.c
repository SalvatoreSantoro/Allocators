#include "../pool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ALLOCS 100

typedef struct {
    int a;
    int b;
    int c;
} Mock;

int main()
{
    Mock* mocks[NUM_ALLOCS];
    Mock mocks_check[NUM_ALLOCS];
    int random_idx;

    PoolAlc* pa;
    srand(time(0));

    pa = pool_alc_create_align(sizeof(Mock), NUM_ALLOCS, __alignof__(Mock));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = (rand() % NUM_ALLOCS);
        mocks[random_idx] = pool_alc_alloc(pa);
        mocks[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks[random_idx]->b = (rand() % NUM_ALLOCS);
        mocks[random_idx]->c = (rand() % NUM_ALLOCS);
        mocks_check[random_idx].a = mocks[random_idx]->a;
        mocks_check[random_idx].b = mocks[random_idx]->b;
        mocks_check[random_idx].c = mocks[random_idx]->c;
        printf("%d\n", mocks[random_idx]->a);
        printf("%d\n", mocks_check[random_idx].a);
    }

    printf("\n\n\n");
    for (int i = 0; i < NUM_ALLOCS; i++) {
        printf("%d\n", mocks[i]->a);
        printf("%d\n", mocks_check[i].a);
        assert(mocks_check[i].a == mocks[i]->a);
        assert(mocks_check[i].b == mocks[i]->b);
        assert(mocks_check[i].c == mocks[i]->c);
    }
}
