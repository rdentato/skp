
TOP = ../..
TEST = t_$(notdir $(realpath .))

TOP=../..

CFLAGS =  -O2 -I$(TOP)/src

eval%: eval%.o
	$(CC) $(LNFLAGS) -o $@ eval$*.o 

clean:
	$(RM) *.o eval?
