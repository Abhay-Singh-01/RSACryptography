#include <stdio.h>
#include <gmp.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include "numtheory.h"
#include "rsa.h"
#include "randstate.h"
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    int opt = 0;

    int idle = 0;

    bool hvalue = false;
    bool vvalue = false;
    bool nvalue = false;
    bool dvalue = false;

    int mriter = 50;

    int bits = 256;

    FILE *pbfile;
    FILE *pvfile;

    uint64_t seed = time(NULL);

    // opt loop to parse command line options

    while ((opt = getopt(argc, argv, "hvb:i:n:d:s:")) != -1) {
        switch (opt) {
        case 'v': vvalue = true; break;
        case 'h':
            hvalue = true;
            idle += 1;
            break;
        case 'n':
            nvalue = true;
            pbfile = fopen(optarg, "w");
            if (pbfile == NULL) {
                fprintf(stderr, "Invalid file.\n");
                exit(0);
            }
            idle += 2;
            break;
        case 'd':
            dvalue = true;
            pvfile = fopen(optarg, "w");
            if (pvfile == NULL) {
                fprintf(stderr, "Invalid file.\n");
                exit(0);
            }
            idle += 2;
            break;
        case 's':
            seed = atoi(optarg);
            idle += 2;
            break;
        case 'c':
            mriter = atoi(optarg);
            idle += 2;
            break;
        case 'b':
            bits = atoi(optarg);
            idle += 2;
            break;
        default: break;
        }
    }

    // prints help message is havalue is true

    if (hvalue && idle == 1) {
        printf("SYNOPSIS\n");
        printf("   Generates an RSA public/private key pair\n\n");
        printf("USAGE\n");
        printf("   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n   -v              Display "
               "verbose program output.\n   -b bits         Minimum bits needed for public key "
               "n.\n   -c confidence   Miller-Rabin iterations for testing primes (default: 50).\n "
               "  -n pbfile       Public key file (default: rsa.pub).\n   -d pvfile       Private "
               "key file (default: rsa.priv).\n   -s seed         Random seed for testing.\n");
        exit(0);
    }

    if (nvalue == false) {
        pbfile = fopen("rsa.pub", "w");
    }
    if (dvalue == false) {
        pvfile = fopen("rsa.priv", "w");
    }

    fchmod(fileno(pvfile), 0600);

    randstate_init(seed);

    mpz_t p;
    mpz_init(p);

    mpz_t q;
    mpz_init(q);

    mpz_t n;
    mpz_init(n);

    mpz_t e;
    mpz_init(e);

    mpz_t user;
    mpz_init(user);

    mpz_t s;
    mpz_init(s);

    mpz_t d;
    mpz_init(d);

    rsa_make_pub(p, q, n, e, bits, mriter);

    rsa_make_priv(d, e, p, q);

    char *username = getenv("USER");

    mpz_set_str(user, username, 62);

    rsa_sign(s, user, d, n);

    rsa_write_pub(n, e, s, username, pbfile);

    rsa_write_priv(n, d, pvfile);

    if (vvalue) {
        printf("user = %s\n", username);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(user);
    mpz_clear(s);
    mpz_clear(d);
    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
}
