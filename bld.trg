##  (C) by Remo Dentato (rdentato@gmail.com)
##
## This software is distributed under the terms of the MIT license:
##  https://opensource.org/licenses/MIT
##

_all () {
  bld -d test all
}

_clean () {
  bld -d test clean
  rm -d src/skp.o
}
