
TOP = ../..

CC=gcc
#CC=clang

OPT=-O2
#OPT=-ggdb3 -O0

CFLAGS =  $(OPT) -Wall -I$(TOP)/src

.PRECIOUS: %.o
all:
	@ls eval?.c

eval%: eval%.o
	$(CC) $(LNFLAGS) -o $@ eval$*.o 

clean:
	$(RM) *.o eval?
