
TOP=../..

SKPGEN=../skpgen/skpgen

CC=gcc
##CC=clang

OPT=-O2
OPT=-O2 -Os -fdata-sections -ffunction-sections
#OPT=-ggdb3 -O0

LNFLAGS=-Wl,--gc-sections
#LNFLAGS=

CFLAGS =  $(OPT) -Wall -I$(TOP)/src

all:
	@cd ../skpgen; make skpgen
	make lox

lox: lox.o lox_main.o
	$(CC) -o $@ lox.o lox_main.o $(LNFLAGS) 

lox_main.o: lox_main.c
	$(CC) -c $(CFLAGS) -DSKP_MAIN $<

lox.c: lox.skp
	$(SKPGEN) lox

clean:
	$(RM) *.o lox lox.c lox.h lox.t
	$(RM) test/*.t test/*.o
