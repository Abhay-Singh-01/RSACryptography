CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic -g $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: keygen encrypt decrypt

encrypt: encrypt.o numtheory.o randstate.o rsa.o
	$(CC) -o encrypt encrypt.o numtheory.o randstate.o rsa.o  $(LFLAGS)

keygen: keygen.o numtheory.o randstate.o rsa.o
	$(CC) -o keygen keygen.o numtheory.o randstate.o rsa.o $(LFLAGS)

decrypt: decrypt.o numtheory.o randstate.o rsa.o
	$(CC) -o decrypt decrypt.o numtheory.o randstate.o rsa.o $(LFLAGS)

keygen.o: keygen.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -o keygen.o -c keygen.c

decrypt.o: decrypt.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -o decrypt.o -c decrypt.c

encrypt.o: encrypt.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -o encrypt.o -c encrypt.c

numtheory.o: numtheory.c 
	$(CC) $(CFLAGS) -o numtheory.o -c numtheory.c

randstate.o: randstate.c
	$(CC) $(CFLAGS) -o randstate.o -c randstate.c

rsa.o: rsa.c
	$(CC) $(CFLAGS) -o rsa.o -c rsa.c

format:
	clang-format -i -style=file *.[ch]

clean:
	rm -f keygen encrypt decrypt *.o

