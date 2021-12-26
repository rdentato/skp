
TOP=../..

CC=gcc
#CC=clang

OPT=-O2
OPT=-O2 -Os -fdata-sections -ffunction-sections
#OPT=-ggdb3 -O0

LNFLAGS=-Wl,--gc-sections
#LNFLAGS=

CFLAGS =  $(OPT) -Wall -I$(TOP)/src

all: skpgen skphdr

skpgen: skpgen.o
	$(CC) -o $@ $< $(LNFLAGS) 

skphdr: skphdr.o
	$(CC) -o $@ $< $(LNFLAGS)

expr: expr.o expr_main.o

expr.o: expr.c
	$(CC) -c $(CFLAGS) -DSKP_MAIN $<

expr.c: skpgen expr.skp
	./skpgen expr

clean:
	$(RM) *.o skpgen skphdr
	$(RM) expr expr.o expr.c expr.t
