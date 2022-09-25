#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <unistd.h>

int main(int argc, char **argv) {
    int opt = 0;

    int idle = 0;

    bool hvalue = false;
    bool vvalue = false;
    bool nvalue = false;
    bool ivalue = false;
    bool ovalue = false;

    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pvfile;

    // opt loop to parse command line options

    while ((opt = getopt(argc, argv, "hvi:n:o:")) != -1) {
        switch (opt) {
        case 'v': vvalue = true; break;
        case 'h':
            hvalue = true;
            idle += 1;
            break;
        case 'n':
            nvalue = true;
            pvfile = fopen(optarg, "r");
            if (pvfile == NULL) {
                fprintf(stderr, "Invalid file.\n");
                exit(0);
            }
            idle += 2;
            break;
        case 'i':
            ivalue = true;
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                fprintf(stderr, "Invalid file.\n");
            }
            idle += 2;
            break;
        case 'o':
            ovalue = true;
            outfile = fopen(optarg, "w");
            if (outfile == NULL) {
                fprintf(stderr, "Invalid file.\n");
            }
            idle += 2;
            break;
        default: break;
        }
    }

    // prints help message is havalue is true

    if (hvalue && idle == 1) {
        printf("SYNOPSIS\n");
        printf("   Decrypts data using RSA decryption.\nEncrypted data is encrypted by the encrypt "
               "program.\n\n");
        printf("USAGE\n");
        printf("   ./encrypt [-hv] [-i infile] [-o outfile] -n privkey\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n   -v              Display "
               "verbose program output.\n   -i infile       Input file of data to decrypt "
               "(default: stdin).\n   -o outfile     Output file for decrypted data (default: "
               "stdout).\n   -n pvfile      Private key file (default: rsa.priv).\n");
        exit(0);
    }

    //if a private file is not specified  default it to rsa.priv

    if (nvalue == false) {
        pvfile = fopen("rsa.priv", "r");
        if (pvfile == NULL) {
            fprintf(stderr, "Invalid file\n");
        }
    }

    mpz_t n;
    mpz_init(n);

    mpz_t d;
    mpz_init(d);

    rsa_read_priv(n, d, pvfile);

    //if v is selected print out the size of vars with decimals

    if (vvalue) {
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    //decrpy the file to the outfile

    rsa_decrypt_file(infile, outfile, n, d);

    //close files and clear mpzs

    fclose(pvfile);
    mpz_clear(n);
    mpz_clear(d);
    fclose(infile);
    fclose(outfile);
}
