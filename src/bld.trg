##  (C) by Remo Dentato (rdentato@gmail.com)
##
## This software is distributed under the terms of the MIT license:
##  https://opensource.org/licenses/MIT
##

set -e 

_all () {
  bld skptest
}

_skplib () {
  CCFLAGS="$CCFLAGS -I../utl"
  DBGFLAGS="-DDEBUG=DBGLVL_TEST"
  bld_old libskp.a skpopcodes.h skp.h && {
    bld_cmd touch skpchar.c skpcomp.c skpdisasm.c skpexec.c
  }

  bld_old skpchar.o   skpchar.c   &&  cc_obj skpchar.o   
  bld_old skpcomp.o   skpcomp.c   &&  cc_obj skpcomp.o   
  bld_old skpdisasm.o skpdisasm.c &&  cc_obj skpdisasm.o   
  bld_old skpexec.o   skpexec.c   &&  cc_obj skpexec.o

  bld_old libskp.a skpchar.o skpcomp.o skpdisasm.o skpexec.o && {
    bld_cmd $AR libskp.a skpcomp.o skpchar.o skpdisasm.o skpexec.o
  }
}

_skptest () {
  CCFLAGS="$CCFLAGS -I../utl"
  DBGFLAGS="-DDEBUG=DBGLVL_TEST"
  LIBS="-lskp"
  LNFLAGS="$LNFLAGS -L./"
  bld skplib
  bld_old skp$_EXE skptest.c libskp.a && {
    cc_exe skp skptest.o 
    #strip skp$_EXE
  }
}


_clean () {
  bld_cmd $RM *.o
  bld_cmd $RM skp$_EXE
  bld_cmd $RM libskp.a
}
