
TOP=../..

OPT=-O2
#OPT=-ggdb3 -O0

CFLAGS =  $(OPT) -Wall -I$(TOP)/src

samban: samban.o
	$(CC) $(LNFLAGS) -o $@ $<

clean:
	$(RM) *.o tbc
