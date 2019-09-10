##  (C) by Remo Dentato (rdentato@gmail.com)
##
## This software is distributed under the terms of the MIT license:
##  https://opensource.org/licenses/MIT
##

_all () {
  bld tests
}

setflags_ () {
  CCFLAGS="$CCFLAGS -I../src -I. -DDEBUG=DBGLVL_TEST"
}

_tests () {
  local f
  local t

  setflags_

  for f in $(ls ut_*.c); do
    t=${f:1}
    t=${t%.*}$_EXE
    echo "Building target: '${t%.*}'" >&2
    bld_old $t $f && {
      cc_exe $t ${f%.*}.o 
    }
  done
}

_default () {
  local f
  local t

  setflags_

  case $1 in
    ''|*[!0-9]*)  f=u$1.c ;  t=$1$_EXE ;;
    *)            f=ut_pattern$1.c ; t=t_p$1$_EXE ;;
  esac
  
  echo "Building target: '${t%.*}'" >&2
  bld_old $t $f && {
    cc_exe $t ${f%.*}.o 
  }
}

_clean () {
  bld_cmd $RM *.o
  bld_cmd $RM t_*$_EXE
  bld_cmd $RM *.log
}
