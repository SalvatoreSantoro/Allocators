#include "../../common/test_assert.h"
#include "../pool.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_ALLOCS 10000

// testing different allocation sizes

typedef struct {
    uint8_t a;
} Mock1;

typedef struct {
    uint16_t a;
} Mock2;

typedef struct {
    uint32_t a;
} Mock4;

typedef struct {
    uint64_t a;
} Mock8;

typedef struct {
    uint64_t a;
    uint64_t b;
} Mock16;

typedef struct {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
} Mock32;

typedef struct {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
    uint64_t e;
    uint64_t f;
    uint64_t g;
    uint64_t h;
} Mock64;

typedef struct {
    int ag_idx;
    int already_generated_idx[NUM_ALLOCS];
} RngState;

int rng_no_rep(RngState* state, int max)
{
    int check;
    int random_idx;

    do {
        check = 0;
        random_idx = (rand() % max);
        for (int i = 0; i < NUM_ALLOCS; i++) {
            if (state->already_generated_idx[i] == random_idx) {
                check = 1;
                break;
            }
        }
    } while (check);

    state->already_generated_idx[state->ag_idx] = random_idx;
    state->ag_idx += 1;

    return random_idx;
}

void rng_reset(RngState* state)
{
    memset(state->already_generated_idx, -1, sizeof(state->already_generated_idx));
    state->ag_idx = 0;
}

RngState* rng_state_init(void)
{
    RngState* state = malloc(sizeof(RngState));
    rng_reset(state);
    return state;
}

int main()
{
    int pool_sizes[10] = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
    int random_idx;
    RngState* rng = rng_state_init();

    srand(time(0));
    Mock1* mocks1[NUM_ALLOCS];
    Mock1 mocks1_check[NUM_ALLOCS];
    PoolAlc* pa1 = pool_alc_create_align(sizeof(Mock1), pool_sizes[0], __alignof__(Mock1));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        mocks1[random_idx] = pool_alc_alloc(pa1);
        mocks1[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks1_check[random_idx].a = mocks1[random_idx]->a;
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        assert(mocks1_check[i].a == mocks1[i]->a);
    }

    rng_reset(rng);

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        pool_alc_free(pa1, mocks1[random_idx]);
    }

    pool_alc_destroy(pa1);
    rng_reset(rng);

    Mock2* mocks2[NUM_ALLOCS];
    Mock2 mocks2_check[NUM_ALLOCS];
    PoolAlc* pa2 = pool_alc_create_align(sizeof(Mock2), pool_sizes[0], __alignof__(Mock2));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        mocks2[random_idx] = pool_alc_alloc(pa2);
        mocks2[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks2_check[random_idx].a = mocks2[random_idx]->a;
    }

    rng_reset(rng);

    for (int i = 0; i < NUM_ALLOCS; i++) {
        assert(mocks2_check[i].a == mocks2[i]->a);
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        pool_alc_free(pa2, mocks2[random_idx]);
    }

    pool_alc_destroy(pa2);
    rng_reset(rng);

    exit(0);

    Mock4* mocks4[NUM_ALLOCS];
    Mock4 mocks4_check[NUM_ALLOCS];
    PoolAlc* pa4 = pool_alc_create_align(sizeof(Mock4), pool_sizes[0], __alignof__(Mock4));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        mocks4[random_idx] = pool_alc_alloc(pa4);
        mocks4[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks4_check[random_idx].a = mocks4[random_idx]->a;
    }

    rng_reset(rng);

    for (int i = 0; i < NUM_ALLOCS; i++) {
        assert(mocks4_check[i].a == mocks4[i]->a);
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        pool_alc_free(pa4, mocks4[random_idx]);
    }

    pool_alc_destroy(pa4);
    rng_reset(rng);

    Mock8* mocks8[NUM_ALLOCS];
    Mock8 mocks8_check[NUM_ALLOCS];
    PoolAlc* pa8 = pool_alc_create_align(sizeof(Mock8), pool_sizes[0], __alignof__(Mock8));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        mocks8[random_idx] = pool_alc_alloc(pa8);
        mocks8[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks8_check[random_idx].a = mocks8[random_idx]->a;
    }

    rng_reset(rng);

    for (int i = 0; i < NUM_ALLOCS; i++) {
        assert(mocks8_check[i].a == mocks8[i]->a);
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        pool_alc_free(pa8, mocks8[random_idx]);
    }

    pool_alc_destroy(pa8);
    rng_reset(rng);

    Mock16* mocks16[NUM_ALLOCS];
    Mock16 mocks16_check[NUM_ALLOCS];
    RngState* rng16 = rng_state_init();
    PoolAlc* pa16 = pool_alc_create_align(sizeof(Mock16), pool_sizes[0], __alignof__(Mock16));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng16, NUM_ALLOCS);
        mocks16[random_idx] = pool_alc_alloc(pa16);
        mocks16[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks16[random_idx]->b = (rand() % NUM_ALLOCS);
        mocks16_check[random_idx].a = mocks16[random_idx]->a;
        mocks16_check[random_idx].b = mocks16[random_idx]->b;
    }

    rng_reset(rng16);

    for (int i = 0; i < NUM_ALLOCS; i++) {
        assert(mocks16_check[i].a == mocks16[i]->a);
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng16, NUM_ALLOCS);
        pool_alc_free(pa16, mocks16[random_idx]);
    }

    pool_alc_destroy(pa16);
    rng_reset(rng);

    Mock32* mocks32[NUM_ALLOCS];
    Mock32 mocks32_check[NUM_ALLOCS];
    PoolAlc* pa32 = pool_alc_create_align(sizeof(Mock32), pool_sizes[0], __alignof__(Mock32));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        mocks32[random_idx] = pool_alc_alloc(pa32);
        mocks32[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks32[random_idx]->b = (rand() % NUM_ALLOCS);
        mocks32[random_idx]->c = (rand() % NUM_ALLOCS);
        mocks32[random_idx]->d = (rand() % NUM_ALLOCS);
        mocks32_check[random_idx].a = mocks32[random_idx]->a;
        mocks32_check[random_idx].b = mocks32[random_idx]->b;
        mocks32_check[random_idx].c = mocks32[random_idx]->c;
        mocks32_check[random_idx].d = mocks32[random_idx]->d;
    }

    rng_reset(rng);

    for (int i = 0; i < NUM_ALLOCS; i++) {
        assert(mocks32_check[i].a == mocks32[i]->a);
        assert(mocks32_check[i].b == mocks32[i]->b);
        assert(mocks32_check[i].c == mocks32[i]->c);
        assert(mocks32_check[i].d == mocks32[i]->d);
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        pool_alc_free(pa32, mocks32[random_idx]);
    }

    pool_alc_destroy(pa32);
    rng_reset(rng);

    Mock64* mocks64[NUM_ALLOCS];
    Mock64 mocks64_check[NUM_ALLOCS];
    PoolAlc* pa64 = pool_alc_create_align(sizeof(Mock64), pool_sizes[0], __alignof__(Mock64));

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        mocks64[random_idx] = pool_alc_alloc(pa64);
        mocks64[random_idx]->a = (rand() % NUM_ALLOCS);
        mocks64[random_idx]->b = (rand() % NUM_ALLOCS);
        mocks64[random_idx]->c = (rand() % NUM_ALLOCS);
        mocks64[random_idx]->d = (rand() % NUM_ALLOCS);
        mocks64[random_idx]->e = (rand() % NUM_ALLOCS);
        mocks64[random_idx]->f = (rand() % NUM_ALLOCS);
        mocks64[random_idx]->g = (rand() % NUM_ALLOCS);
        mocks64[random_idx]->h = (rand() % NUM_ALLOCS);
        mocks64_check[random_idx].a = mocks64[random_idx]->a;
        mocks64_check[random_idx].b = mocks64[random_idx]->b;
        mocks64_check[random_idx].c = mocks64[random_idx]->c;
        mocks64_check[random_idx].d = mocks64[random_idx]->d;
        mocks64_check[random_idx].e = mocks64[random_idx]->e;
        mocks64_check[random_idx].f = mocks64[random_idx]->f;
        mocks64_check[random_idx].g = mocks64[random_idx]->g;
        mocks64_check[random_idx].h = mocks64[random_idx]->h;
    }

    rng_reset(rng);

    for (int i = 0; i < NUM_ALLOCS; i++) {
        assert(mocks64_check[i].a == mocks64[i]->a);
        assert(mocks64_check[i].b == mocks64[i]->b);
        assert(mocks64_check[i].c == mocks64[i]->c);
        assert(mocks64_check[i].d == mocks64[i]->d);
        assert(mocks64_check[i].e == mocks64[i]->e);
        assert(mocks64_check[i].f == mocks64[i]->f);
        assert(mocks64_check[i].g == mocks64[i]->g);
        assert(mocks64_check[i].h == mocks64[i]->h);
    }

    for (int i = 0; i < NUM_ALLOCS; i++) {
        random_idx = rng_no_rep(rng, NUM_ALLOCS);
        pool_alc_free(pa64, mocks2[random_idx]);
    }

    pool_alc_destroy(pa64);
    rng_reset(rng);
}
