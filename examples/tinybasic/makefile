
TOP = ../..
TEST = t_$(notdir $(realpath .))

TOP=../..

CFLAGS =  -O2 -I$(TOP)/src

tbc: tbc.o
	$(CC) $(LNFLAGS) -o $@ tbc$*.o 

clean:
	$(RM) *.o tbc
