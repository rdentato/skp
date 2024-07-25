#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  


tstsuite("Switch") {
  char *next = "";

  tstcase("Matching switch") {
    char *text = "ab43";
    next = text;
    skpswitch(next,&next) {
      skpcase("+a") { 
        tstcheck(skplen(0) == 2);
        tstnote("ALPHA: %.*s",skplen(0),skpfrom[0]); 
      }
      skpcase("+d") { 
        tstcheck("SHOULDN'T be here!");
      }
    }
    tstcheck(*next == '4');
  
    skpswitch(next,&next) {
      skpcase("+a") { 
        tstcheck("SHOULDN'T be here!");
      }
      skpcase("+d") { 
        tstcheck(skplen(0) == 2);
        tstnote("DIGITS: %.*s",skplen(0),skpfrom[0]); 
      }
    }
  }

  tstcase("Non matching switch") {
    char *text = "?-ab43";
    next = text;
    skpswitch(next,&next) {
      skpcase("+a") { 
        tstcheck("SHOULDN'T be here!");
      }
      skpcase("+d") { 
        tstcheck("SHOULDN'T be here!");
      }
    }
    tstcheck(*next == '?');
  }

  tstcase("Matching with multi") {
    char *text = "ab433";
    next = text;
    skpswitch(next,&next) {
      skpcase("+a","+d") { 
        tstcheck(skplen(0) == 5);
        tstcheck(skplen(1) == 2);
        tstcheck(skplen(2) == 3);
        tstcheck(strncmp("ab",skpfrom[1],skplen(1)) == 0);
        tstcheck(strncmp("433",skpfrom[2],skplen(2)) == 0);
      }
      skpcase("+d") { 
        tstcheck("SHOULDN'T be here!");
      }
    }
  }
}