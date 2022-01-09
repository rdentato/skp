
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
	make  tinybasic

 tinybasic:  tinybasic.o  tb_main.o
	$(CC) -o $@  tinybasic.o  tb_main.o $(LNFLAGS) 

 tb_main.o:  tb_main.c
	$(CC) -c $(CFLAGS) -DSKP_MAIN $<

 tinybasic.c:  tinybasic.skp
	$(SKPGEN)  tinybasic

clean:
	$(RM) *.o  tinybasic  tinybasic.c  tinybasic.h  tinybasic.t
	$(RM) test/*.t test/*.o
