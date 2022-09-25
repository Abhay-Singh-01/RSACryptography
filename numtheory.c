#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"

//gcd algorithm given in prof longs asgn6 manual

//computes the greatest common divisor of the two inputs
//stores the result in g

void gcd(mpz_t g, mpz_t a, mpz_t b) {
    mpz_t tempa;
    mpz_t tempb;
    mpz_t t;
    mpz_init(t);
    mpz_init(tempa);
    mpz_init(tempb);
    mpz_set(tempa, a);
    mpz_set(tempb, b);

    //while tempb is not equal to 0
    while (mpz_sgn(tempb) != 0) {
        mpz_set(t, tempb);

        //compute modulus and store in tembb
        mpz_mod(tempb, tempa, tempb);
        mpz_set(tempa, t);
    }

    //set the answer variable
    mpz_set(g, tempa);

    //clear all mpszs
    mpz_clear(tempa);
    mpz_clear(tempb);
    mpz_clear(t);
}

//mod inverse algorithm from prog longs asgn6 manual

//computes mod inverse and store the result in o

void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
    mpz_t r;
    mpz_init(r);
    mpz_set(r, n);

    mpz_t tempr;
    mpz_init(tempr);
    mpz_set(tempr, n);

    mpz_t rprime;
    mpz_init(rprime);
    mpz_set(rprime, a);

    mpz_t t;
    mpz_init(t);
    mpz_set_ui(t, 0);

    mpz_t tempt;
    mpz_init(tempt);
    mpz_set_ui(tempt, 0);

    mpz_t tprime;
    mpz_init(tprime);
    mpz_set_ui(tprime, 1);

    mpz_t q;
    mpz_init(q);

    //while rprime is not equal to 0
    while (mpz_sgn(rprime) != 0) {
        mpz_fdiv_q(q, r, rprime);
        mpz_set(r, rprime);
        mpz_mul(rprime, q, rprime);
        mpz_sub(rprime, tempr, rprime);
        mpz_set(t, tprime);
        mpz_mul(tprime, q, tprime);
        mpz_sub(tprime, tempt, tprime);
        mpz_set(tempr, r);
        mpz_set(tempt, t);
    }

    //if r is greater than 1
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(o, 0);
        return;

        //if t is less than 0
    } else if (mpz_sgn(t) == -1) {
        mpz_add(t, t, n);
    }

    //set the answer variable
    mpz_set(o, t);

    //clear all mpzs
    mpz_clear(r);
    mpz_clear(rprime);
    mpz_clear(t);
    mpz_clear(tprime);
    mpz_clear(q);
    mpz_clear(tempr);
    mpz_clear(tempt);
}

//power mod algorithm from prof longs asgn6 manual

//computes the power mod and stores the result in 0
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
    mpz_t v;
    mpz_init(v);
    mpz_set_ui(v, 1);

    mpz_t p;
    mpz_init(p);
    mpz_set(p, a);

    mpz_t tempd;
    mpz_init(tempd);
    mpz_set(tempd, d);

    //while tempd is greater than 0
    while (mpz_sgn(tempd) == 1) {
        //tempd is odd
        if (mpz_odd_p(tempd) != 0) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, n);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);
        mpz_fdiv_q_ui(tempd, tempd, 2);
    }
    //set the otuput variable
    mpz_set(o, v);

    //clear all mpsz's
    mpz_clear(v);
    mpz_clear(p);
    mpz_clear(tempd);
}

//is rpime from prof longs asgn6 manual

//returns if a number is prime or not
bool is_prime(mpz_t n, uint64_t iters) {
    //signifies the 2 in 2^s
    uint64_t power = 0;

    //set the base of 2 in 2^s as 2
    mpz_t base;
    mpz_init(base);
    mpz_set_ui(base, 1);

    //copies n into tempn
    mpz_t tempn;
    mpz_init(tempn);
    mpz_set(tempn, n);

    //checks if n is even or odd
    mpz_t mod;
    mpz_init(mod);
    mpz_mod_ui(mod, n, 2);

    //sets s to 0 in 2^s
    //will be iterated upon in function
    mpz_t s;
    mpz_init(s);
    mpz_set_ui(s, 0);

    //set r to an even number so loop can start
    mpz_t r;
    mpz_init(r);
    mpz_set_ui(r, 2);

    //for loop iterator
    mpz_t i;
    mpz_init(i);

    //random number generated
    mpz_t a;
    mpz_init(a);

    //power mod operator
    mpz_t y;
    mpz_init(y);

    //j stores 1 and is iterated upon in function
    mpz_t j;
    mpz_init(j);

    //set two to 2
    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);

    //base cases for n = 0 -> 3
    if (mpz_cmp_ui(n, 2) == 0) {
        mpz_clears(i, a, y, j, two, base, s, r, tempn, mod, NULL);
        return true;
    } else if (mpz_cmp_ui(n, 3) == 0) {
        mpz_clears(i, a, y, j, two, base, s, r, tempn, mod, NULL);
        return true;
    } else if (mpz_cmp_ui(n, 1) == 0) {
        mpz_clears(i, a, y, j, two, base, s, r, tempn, mod, NULL);
        return false;
    } else if (mpz_sgn(mod) == 0) {
        mpz_clears(i, a, y, j, two, base, s, r, tempn, mod, NULL);
        return false;
    } else {

        //keep dividing n-1 by s to a incremented pwoer
        //once r is an odd number keep that r value
        mpz_sub_ui(tempn, tempn, 1);
        while (mpz_even_p(r) != 0) {
            mpz_fdiv_q_2exp(r, tempn, power);
            power += 1;
        }
    }

    //iterate from 1 to k
    for (mpz_set_ui(i, 1); mpz_cmp_ui(i, iters) <= 0; mpz_add_ui(i, i, 1)) {

        //get a random number from 2 to n-2
        mpz_urandomm(a, state, tempn);
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);

        //if y is not equal to 1 or n - 1
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, tempn) != 0) {
            mpz_set_ui(j, 1);

            //while j is <= s-1 and y is not equal to n - 1
            while (mpz_cmp_ui(j, power - 1) <= 0 && mpz_cmp(y, tempn) != 0) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(i, a, y, j, two, base, s, r, tempn, mod, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }

            //if y is not equal to n -1
            if (mpz_cmp(y, tempn) != 0) {
                mpz_clears(i, a, y, j, two, base, s, r, tempn, mod, NULL);
                return false;
            }
        }
    }
    //return true if prime
    mpz_clears(i, a, y, j, two, base, s, r, tempn, mod, NULL);
    return true;
}

//generates a prime number of a length bit size
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t number;
    mpz_init(number);
    mpz_set_ui(number, 4);

    //create random numbers until  the number is prime
    do {
        mpz_rrandomb(number, state, bits);
    } while (is_prime(number, iters) == false);

    //set outpurt var to number
    mpz_set(p, number);

    //clear the number variable
    mpz_clear(number);
}
