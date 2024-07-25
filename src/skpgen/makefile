SRC=10_skpgen.md
TNG=../../deps/tng

CC=gcc
CFLAGS=-O2 -Wall -I./deps -I..

skpgen: skpgen.o deps/val.o

skpgen.c: $(SRC) 
	$(TNG) -o skpgen.c $(SRC) 

clean:
	rm -f skpgen.c skpgen.o skpgen skpgen.exe 
