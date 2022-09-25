# Asignment 6 - Public Key Cryptography

This program implements a the RSA method of cryptography. Essentially a key generator will be user to generate the public and private keys. The keys will then be used to encrypt a message, and then decrypt the message back to its original form. The files will be incorporating the RSA algorithm, but some of the numtheory mathematical methods will be their own methods, such as Miller-Rabin for testing primes.

## Building

Build the program with:

$ make all

The make will refer to the Makefile that compiles all the programs into one executable. Make clean will remove excess programs (.o files) and make format will format all the programs in clang format. the makefile in this case will build 3 executables: keygen, encrypt, and decrypt.

## Running

Run the program with:

$ ./keygen -[hvb:i:n:d:s:]

-h prints out program usage and help

-v Displays vervose program output

-b bits Minimum bits needed for public key

-c confidence Miller-Rabin iterations for testing primes

-n pbfile Public key files

-d pvfile Private key file

-s seed Random seed for testing


$ ./encrypt -[hvi:n:o:]

-h prints out program usage and help

-v Display verbose program output

-i infile Input file of data to encrypt

-o outfile Output file for encrypted data

-n pbfile Public key file


$ ./decrypt -{hvi:n:o}

-h prints out program usage and help

-v Display verbose program output

-i infile Input file of data to decrypt

-o outfile Output file for decrypted data

-n pvfile Private key file


