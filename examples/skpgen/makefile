
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

skpgen2: skpgen2.o
	$(CC) -o $@ $< $(LNFLAGS) 

skphdr: skphdr.o
	$(CC) -o $@ $< $(LNFLAGS)

clean:
	$(RM) *.o skpgen skphdr
