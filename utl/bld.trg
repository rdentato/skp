##  (C) by Remo Dentato (rdentato@gmail.com)
##
## This software is distributed under the terms of the MIT license:
##  https://opensource.org/licenses/MIT
##

_all () {
  bld libutl
}

_libutl () {
  UTLH=$(ls|grep ".h$"|grep -v "utl.h")
  UTLC=$(ls|grep ".c$")
  echo $UTLH
  echo $UTLC
  bld_old utl.h $UTLH && {
    echo "cat $UTLH > utl.h"
    cat $UTLH > utl.h
  }
  if [ "$UTLC" != "" ] ; then
    bld_old libutl.a $UTLC $UTLH && {
       for f in $UTLC ; do
         bld_cmd $CC -O2 -Wall -DNDEBUG -c $f
       done
       bld_cmd $AR libutl.a *.o
    }
  fi
}

_clean () {
  bld_cmd $RM *.o
  bld_cmd $RM utl.h libutl.a
}
