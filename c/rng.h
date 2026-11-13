#ifndef RNG_H
#define RNG_H

/* Small xorshift32 generator so a seed gives the same run on every platform. */
void rng_seed(unsigned seed);
unsigned rng_next(void);
int rng_below(int n);

#endif
