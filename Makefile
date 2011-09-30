CC=gcc
CFLAGS=-std=gnu99 -static -lc

all: ld-nacl-x86-32.so.1 ld-nacl-x86-64.so.1

ld-nacl-x86-32.so.1: nacl_interp.c
	$(CC) -o ld-nacl-x86-32.so.1 nacl_interp.c $(CFLAGS) -m32

ld-nacl-x86-64.so.1: nacl_interp.c
	$(CC) -o ld-nacl-x86-64.so.1 nacl_interp.c $(CFLAGS) -m64

clean:
	rm -rf *.o *.so.1

