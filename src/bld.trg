##  (C) by Remo Dentato (rdentato@gmail.com)
##
## This software is distributed under the terms of the MIT license:
##  https://opensource.org/licenses/MIT
##

_all () {
  bld skptest
}

_skptest () {
  CCFLAGS="$CCFLAGS -I../utl"
  DBGFLAGS="-DDEBUG=DBGLVL_TEST"
  bld_old skp$_EXE skpopcodes.h skp.h && {
    touch skptest.c skpchar.c skpcomp.c
  }
  bld_old skp$_EXE skptest.c skpchar.c skpcomp.c && {
    cc_exe skp skpcomp.o skpchar.o skptest.o 
    #strip skp$_EXE
  }
}

_clean () {
  bld_cmd $RM *.o
  bld_cmd $RM skp$_EXE
}
