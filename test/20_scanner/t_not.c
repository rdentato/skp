#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
tstsuite("peek") {
  char *next;
  int testcount;

  tstcase("simple not") {
    next = "\x0F";
    testcount = 0;
    skpgroup("ABC3y", &next) {
      ifskp("*u") ;
      notskp {
        ifskp("l") ;
      }
      if (skpmatched) {testcount += 1;}
    }
    tstcheck(testcount == 1);
    tstcheck(*next == '3');
  }

  tstcase("simple not") {
    next = "\x0F";
    testcount = 0;
    skpgroup("ABCxy", &next) {
      ifskp("*u") ;
      notskp {
        ifskp("l") ;
      }
      if (skpmatched) {testcount += 1;}
    }
    tstcheck(testcount == 0);
    tstcheck(*next == '\x0F');
  }

  tstcase("simple not") {
    next = "\x0F";
    testcount = 0;
    skpgroup("xy", &next) {
      ifskp("*u") ;
      notskp ifskp("'x'") {}
      elseskp {testcount += 10;}
    }
    tstcheck(testcount == 10);
    tstcheck(*next == 'x');
  }



}