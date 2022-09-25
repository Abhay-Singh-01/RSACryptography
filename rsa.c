#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

//generates a publike key

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    mpz_t tempp;
    mpz_init(tempp);

    mpz_t tempq;
    mpz_init(tempq);

    mpz_t totient;
    mpz_init(totient);

    mpz_t product;
    mpz_init(product);

    mpz_t gc;
    mpz_init(gc);

    mpz_t num;
    mpz_init(num);

    //generates a random number in the range of bits
    uint64_t random = 0;
    uint64_t mod = 1 + ((3 * nbits) / 4);
    uint64_t add = nbits / 4;
    random = (rand() % mod) + add;

    //the rest of nbits - generated bits go to q
    add = nbits - random;

    //make prime for p
    make_prime(tempp, random, iters);

    //make prime for q
    make_prime(tempq, add, iters);
    mpz_set(p, tempp);
    mpz_set(q, tempq);
    mpz_mul(product, tempp, tempq);
    mpz_sub_ui(tempp, tempp, 1);
    mpz_sub_ui(tempq, tempq, 1);
    mpz_mul(totient, tempp, tempq);

    //keep generating random numbers until
    //we get one coprime to the totient
    do {
        mpz_urandomb(num, state, nbits);
        gcd(gc, num, totient);
    } while (mpz_cmp_ui(gc, 1) != 0);

    //set the otuput vars and clear the mpzs
    mpz_set(e, num);
    mpz_set(n, product);
    mpz_clear(tempp);
    mpz_clear(tempq);
    mpz_clear(totient);
    mpz_clear(product);
    mpz_clear(gc);
    mpz_clear(num);
}

//writes out the public key to pbfile
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n", n, e, s);
    fprintf(pbfile, "%s\n", username);
}

//read the public key from the  pbfile
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    char buffer[1024];
    fgets(buffer, 1024, pbfile);
    gmp_sscanf(buffer, "%Zx\n", n);
    fgets(buffer, 1024, pbfile);
    gmp_sscanf(buffer, "%Zx\n", e);
    fgets(buffer, 1024, pbfile);
    gmp_sscanf(buffer, "%Zx\n", s);
    fgets(buffer, 1024, pbfile);
    sscanf(buffer, "%s\n", username);
}

//generates a private key
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t tempp;
    mpz_init(tempp);
    mpz_set(tempp, p);
    mpz_sub_ui(tempp, tempp, 1);

    mpz_t tempq;
    mpz_init(tempq);
    mpz_set(tempq, q);
    mpz_sub_ui(tempq, tempq, 1);

    //product of p and q
    mpz_t product;
    mpz_init(product);
    mpz_mul(product, tempp, tempq);

    mpz_t inverse;
    mpz_init(inverse);
    mod_inverse(inverse, e, product);

    //sets de
    mpz_set(d, inverse);

    //clears all mpzs
    mpz_clear(tempp);
    mpz_clear(tempq);
    mpz_clear(product);
    mpz_clear(inverse);
}

//writes theprivate key to pvfile
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
}

//reads the provate key from pvfile
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx %Zx", n, d);
}

//encrypts the message using power mod
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
}

//function to compute log base 2 of a number
void logtwo(mpz_t k, mpz_t n) {
    mpz_t tempn;
    mpz_init(tempn);
    mpz_set(tempn, n);

    mpz_t count;
    mpz_init(count);
    mpz_set_ui(count, 0);

    mpz_abs(tempn, tempn);
    while (mpz_sgn(tempn) == 1) {
        mpz_fdiv_q_ui(tempn, tempn, 2);
        mpz_add_ui(count, count, 1);
    }
    mpz_set(k, count);
    mpz_clear(count);
    mpz_clear(tempn);
}

//encryps a message and write it out to the outfile
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {

    mpz_t k;
    mpz_init(k);

    mpz_t c;
    mpz_init(c);

    logtwo(k, n);
    mpz_sub_ui(k, k, 1);
    mpz_fdiv_q_ui(k, k, 8);

    uint8_t size = mpz_get_ui(k);

    mpz_t m;
    mpz_init(m);

    int j = 1;

    //allocate an array of message size
    uint8_t *array = (uint8_t *) calloc(size, sizeof(uint8_t));

    array[0] = 0xFF;

    //read until end of file
    //convert m to cyphertext
    //encrypt the cyphertext
    //rite it out in blocks
    while (true) {
        j = fread(array + 1, sizeof(uint8_t), size - 1, infile);
        if (j == 0) {
            break;
        }
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, array);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }

    //clear plzs and free array
    mpz_clear(k);
    mpz_clear(m);
    mpz_clear(c);
    free(array);
}

//decrypt the message using power mod
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
}

//decrypts the file and writes it out to outfile
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t k;
    mpz_init(k);

    mpz_t tempn;
    mpz_init(tempn);
    mpz_set(tempn, n);

    mpz_t c;
    mpz_init(c);

    logtwo(k, tempn);
    mpz_sub_ui(k, k, 1);
    mpz_fdiv_q_ui(k, k, 8);

    mpz_t m;
    mpz_init(m);

    uint8_t size = mpz_get_ui(k);

    //allocate an array for the message
    uint8_t *array = (uint8_t *) calloc(size, sizeof(uint8_t));

    size_t j = 0;

    //scan in 1 hexstring at a time until it returns EOF
    //decrypt the hexstring
    // export it to plaintext
    //write out the plaintest
    while (true) {
        int value = gmp_fscanf(infile, "%Zx\n", c);
        if (value == EOF) {
            break;
        }
        rsa_decrypt(m, c, d, n);
        mpz_export(array, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(array + 1, sizeof(uint8_t), j - 1, outfile);
    }

    //clear the mpzs and free the array
    mpz_clear(k);
    mpz_clear(tempn);
    mpz_clear(c);
    mpz_clear(m);
    free(array);
}

//performs the signature using power mod
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

//verifies the signature
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);

    //computes t to compare with the signature
    pow_mod(t, s, e, n);

    //if the signature is verified return true
    //else return false
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    } else {
        mpz_clear(t);
        return false;
    }
}
