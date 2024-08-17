#  SPDX-FileCopyrightText: © 2023 Remo Dentato <rdentato@gmail.com>
#  SPDX-License-Identifier: MIT

# Uncomment the following line to generate a minimal log.
# COMPACT="-DTST_STR_COMPACT"

# Uncomment the following line to disable ANSI colors
# NOCOLOR="-DTST_STR_NOCOLOR"

# Uncomment the following line to enable debugging
# DEBUG="-DDEBUG"

SRCDIR=../../src
DEPSDIR=../../deps

_EXE=.exe
STATIC=-static

ifeq "$(COMSPEC)" ""
_EXE=
STATIC=
endif
DEBUG=-DDEBUG=DEBUG_TEST
CFLAGS=-g -Wall -I$(SRCDIR) -I. -I$(DEPSDIR) $(STATIC) $(COMPACT) $(DEBUG)
LIBS=

TESTS_SRC_C=$(wildcard t_*.c)
TESTS_C=$(TESTS_SRC_C:.c=)

TESTS_SRC=$(TST_SRC_C)
TESTS=$(TESTS_C)

# targets
all: $(TESTS)

runtest: all
	@../tstrun 

.PRECIOUS: %.o

rebuild:
	cd $(SRCDIR); make -B
#	make -B all

clean:
	rm -f $(TESTS) $(TESTS:=.exe) $(TESTS:=.o) test.log

