#include <stdint.h>
#include <gmp.h>
#include <inttypes.h>
#include "randstate.h"

//initialize extern randstate variable

gmp_randstate_t state;

//initalizes the random state with a user input seed

void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
}

//clears the random state

void randstate_clear(void) {
    gmp_randclear(state);
}
