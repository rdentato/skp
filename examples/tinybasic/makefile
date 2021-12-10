
TOP=../..

OPT=-O2
#OPT=-ggdb3 -O0

CFLAGS =  $(OPT) -Wall -I$(TOP)/src

tbc: tbc.o
	$(CC) $(LNFLAGS) -o $@ tbc$*.o 

clean:
	$(RM) *.o tbc
