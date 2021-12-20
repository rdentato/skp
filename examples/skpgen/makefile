
TOP=../..

CC=gcc

OPT=-O2
#OPT=-ggdb3 -O0

CFLAGS =  $(OPT) -Wall -I$(TOP)/src

skpgen: skpgen.o
	$(CC) $(LNFLAGS) -o $@ $<

clean:
	$(RM) *.o skpgen
