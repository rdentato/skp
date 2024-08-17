#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
tstsuite("peek") {
  char *next;
  int testcount;

  tstcase("simple peek") {
    next = "\x0F";
    testcount = 0;
    skpgroup("ABCxy", &next) {
      ifskp("*u") ;
      peekskp {
        ifskp("l") {testcount += 1;}
      }
    }
    tstcheck(testcount == 1);
    tstcheck(*next == 'x');
  }

  tstcase("simple peek") {
    next = "\x0F";
    testcount = 0;
    skpgroup("xy", &next) {
      ifskp("*u") ;
      peekskp {
        ifskp("'x'") {testcount += 1;}
      }
    }
    tstcheck(testcount == 1);
    tstcheck(*next == 'x');
  }

  tstcase("simple peek") {
    next = "\x0F";
    testcount = 0;
    skpgroup("yx", &next) {
      ifskp("*u") {}
      peekskp ifskp("'x'") {testcount += 1;}
    }
    tstcheck(testcount == 0);
    tstcheck(*next == '\x0F');
  }

}