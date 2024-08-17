#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
tstsuite("while") {
  char *next;
  int testcount;
  tstcase("simple while") {
    next = "\x0F";
    testcount = 0;
    skpgroup("ABCx", &next) {
      whileskp("u") {testcount += 1;}
    }
    tstcheck(testcount == 3);
    tstcheck(*next == 'x');
  }

  tstcase("simple while") {
    testcount = 0;
    next = "\x0F";
    skpgroup("xABCx", &next) {
      whileskp("u") {testcount += 1;}
    }
    tstcheck(testcount == 0);
    tstcheck(*next == '\x0F');
  }
}