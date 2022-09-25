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
    FILE *pbfile;

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
            pbfile = fopen(optarg, "r");
            if (pbfile == NULL) {
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
        printf("   Encrypts data using RSA encryption.\nEncrypted data is decrypted by the decrypt "
               "program.\n\n");
        printf("USAGE\n");
        printf("   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n   -v              Display "
               "verbose program output.\n   -i infile       Input file of data to encrypt "
               "(default: stdin).\n   -o outfile     Output file for encrypted data (default: "
               "stdout).\n   -n pbfile      Public key file (default: rsa.pub).\n");
        exit(0);
    }

    //if a public file was not specified defualt it to rsa.pub

    if (nvalue == false) {
        pbfile = fopen("rsa.pub", "r");
        if (pbfile == NULL) {
            fprintf(stderr, "Invalid file\n");
        }
    }

    mpz_t n;
    mpz_init(n);

    mpz_t e;
    mpz_init(e);

    mpz_t s;
    mpz_init(s);

    mpz_t user;
    mpz_init(user);

    //create an array to store the username

    char username[1024];

    //read the username from the oublicfile

    rsa_read_pub(n, e, s, username, pbfile);

    //if verbose is selected print out the elements and their bit counts

    if (vvalue) {
        printf("user = %s\n", username);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    //set the username  to a mpz_t

    mpz_set_str(user, username, 62);

    //verify that the username is correct

    if (rsa_verify(user, s, e, n) == false) {
        fprintf(stderr, "Signature could not be verified.\n");
        exit(0);
    }

    //encrypt the file and write out to outfile

    rsa_encrypt_file(infile, outfile, n, e);

    //cose the files and clear the mpzs

    fclose(pbfile);
    fclose(infile);
    fclose(outfile);
    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(s);
    mpz_clear(user);
}
