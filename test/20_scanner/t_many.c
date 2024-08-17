#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
tstsuite("manyskp") {
  char *next;
  int testcount;
  tstcase("simple many") {
    next = "\x0F";
    testcount = 0;
    skpgroup("ABCx", &next) {
      manyskp {
        ifskp("u")  {testcount += 1;}
      }
    }
    tstcheck(testcount == 3);
    tstcheck(*next == 'x');
  }

  tstcase("simple many") {
    next = "\x0F";
    testcount = 0;
    skpgroup("Ax", &next) {
      manyskp {
        ifskp("u")  {testcount += 1;}
      }
    }
    tstcheck(testcount == 1);
    tstcheck(*next == 'x');
  }

  tstcase("simple many") {
    next = "\x0F";
    testcount = 0;
    skpgroup("Ax", &next) {
      manyskp {
        ifskp("l")  {testcount += 1;}
      }
    }
    tstcheck(testcount == 0);
    tstcheck(*next == '\x0F');
  }

  tstcase("simple while") {
    testcount = 0;
    next = "\x0F";
    skpgroup("xABCx", &next) {
      manyskp ifskp("u") {testcount += 1;}
    }
    tstcheck(testcount == 0);
    tstcheck(*next == '\x0F');
  }
}