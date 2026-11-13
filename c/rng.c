#include "rng.h"

static unsigned state = 2463534242u;

void rng_seed(unsigned seed)
{
    state = seed ? seed : 2463534242u;
}

unsigned rng_next(void)
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

int rng_below(int n)
{
    return (int)(rng_next() % (unsigned)n);
}
