##  (C) by Remo Dentato (rdentato@gmail.com)
##
## This software is distributed under the terms of the MIT license:
##  https://opensource.org/licenses/MIT
##

_all () {
  bld tests
}

_tests () {
  local f
  local t
  CFLAGS="$CFLAGS -I../src -I. -DDEBUG"

  bld_old ../src/skp.o ../src/skp.c ../src/skp.h && {
    cc_obj -f ../src/skp.o
  }

  for f in $(ls ut_*.c); do
    t=${f:1}
    t=${t%.*}$_EXE
    echo "Building target: '${t%.*}'" >&2
    bld_old $t $f ../src/skp.h ../src/skp.o && {
      cc_exe $t ${f%.*}.o ../src/skp.o
    }
  done
}

_default () {
  local f
  local t
  CFLAGS="$CFLAGS -I../src -I. -DDEBUG"

  case $1 in
    ''|*[!0-9]*)  f=u$1.c ;  t=$1$_EXE ;;
    *)            f=ut_pattern$1.c ; t=t_p$1$_EXE ;;
  esac

  
  bld_old ../src/skp.o ../src/skp.c ../src/skp.h && {
    cc_obj -f ../src/skp.o
  }
  echo "Building target: '${t%.*}'" >&2
  bld_old $t $f ../src/skp.o ../src/skp.h && {
    cc_exe $t ${f%.*}.o ../src/skp.o
  }
}

_clean () {
  bld_cmd $RM *.o
  bld_cmd $RM t_*$_EXE
  bld_cmd $RM *.log
}
