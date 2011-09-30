CC=gcc
CFLAGS=-std=gnu99 -static -lc

all: ld-nacl-x86-32.so.1 ld-nacl-x86-64.so.1

ld-nacl-x86-32.so.1: nacl_interp.c
	$(CC) -o ld-nacl-x86-32.so.1 nacl_interp.c $(CFLAGS) -m32

ld-nacl-x86-64.so.1: nacl_interp.c
	$(CC) -o ld-nacl-x86-64.so.1 nacl_interp.c $(CFLAGS) -m64

clean:
	rm -rf *.o *.so.1

install: ld-nacl-x86-32.so.1 ld-nacl-x86-64.so.1
	cp ld-nacl-x86-32.so.1 /lib/ld-nacl-x86-32.so.1
	chmod 755 /lib/ld-nacl-x86-32.so.1
	cp ld-nacl-x86-64.so.1 /lib64/ld-nacl-x86-64.so.1
	chmod 755 /lib64/ld-nacl-x86-64.so.1
	echo "Please, manually setup nacl_interp_loader.sh and set NACL_INTERP_LOADER env variable to point to nacl_intep_loader.sh"

