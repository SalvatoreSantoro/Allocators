#include "../pool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_ALLOCS 10000

typedef struct {
    int a;
    int b;
    int c;
} Mock;

int main()
{
    Mock* mocks[NUM_ALLOCS];
    Mock mocks_check[NUM_ALLOCS];
    int already_generated_idx[NUM_ALLOCS];
    memset(already_generated_idx, -1, sizeof(already_generated_idx));
    int ag_idx = 0;
    int check;
    int random_idx;

    PoolAlc* pa;
    srand(time(0));

    pa = pool_alc_create(sizeof(Mock), 10);
    // pa = pool_alc_create_align(sizeof(Mock), NUM_ALLOCS, __alignof__(Mock));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        do {
            check = 0;
            random_idx = (rand() % NUM_ALLOCS);
            for (int j = 0; j < NUM_ALLOCS; j++) {
                if (already_generated_idx[j] == random_idx) {
                    check = 1;
                    break;
                }
            }
        } while (check);

        already_generated_idx[ag_idx] = random_idx;
        ag_idx += 1;

        mocks[random_idx] = pool_alc_alloc(pa);
        mocks[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks[random_idx]->b = (rand() % NUM_ALLOCS);
        mocks[random_idx]->c = (rand() % NUM_ALLOCS);
        mocks_check[random_idx].a = mocks[random_idx]->a;
        mocks_check[random_idx].b = mocks[random_idx]->b;
        mocks_check[random_idx].c = mocks[random_idx]->c;
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        assert(mocks_check[i].a == mocks[i]->a);
        assert(mocks_check[i].b == mocks[i]->b);
        assert(mocks_check[i].c == mocks[i]->c);
    }

    memset(already_generated_idx, -1, sizeof(already_generated_idx));
    ag_idx = 0;

    for (int i = 0; i < NUM_ALLOCS; i++) {
        do {
            check = 0;
            random_idx = (rand() % NUM_ALLOCS);
            for (int j = 0; j < NUM_ALLOCS; j++) {
                if (already_generated_idx[j] == random_idx) {
                    check = 1;
                    break;
                }
            }
        } while (check);

        already_generated_idx[ag_idx] = random_idx;
        ag_idx += 1;

        pool_alc_free(pa, mocks[random_idx]);
    }
}
