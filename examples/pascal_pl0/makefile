
TOP=../..

SKPGEN=../skpgen/skpgen

CC=gcc
#CC=clang

OPT=-O2
OPT=-O2 -Os -fdata-sections -ffunction-sections
#OPT=-ggdb3 -O0

LNFLAGS=-Wl,--gc-sections
#LNFLAGS=

CFLAGS =  $(OPT) -Wall -I$(TOP)/src

all:
	@cd ../skpgen; make skpgen
	make pl0

pl0: pl0.o pl0_main.o
	$(CC) -o $@ pl0.o pl0_main.o $(LNFLAGS) 

pl0_main.o: pl0_main.c
	$(CC) -c $(CFLAGS) -DSKP_MAIN $<

pl0.c: pl0.skp
	$(SKPGEN) pl0

clean:
	$(RM) *.o pl0 pl0.c pl0.h pl0.t
	$(RM) pas/*.t
